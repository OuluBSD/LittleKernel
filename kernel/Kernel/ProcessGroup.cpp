#include "ProcessGroup.h"
#include "Global.h"
#include "MemoryManager.h"
#include "Logging.h"
#include "Timer.h"

// Global process group manager instance
ProcessGroupManager* process_group_manager = nullptr;

// ProcessGroupManager implementation
ProcessGroupManager::ProcessGroupManager() {
    group_list_head = nullptr;
    session_list_head = nullptr;
    next_pgid = INITIAL_PGID;
    next_sid = INITIAL_SID;
    current_group = nullptr;
    current_session = nullptr;
}

ProcessGroupManager::~ProcessGroupManager() {
    // Clean up all process groups and sessions
    ForceCleanupAllGroups();
    ForceCleanupAllSessions();
}

ProcessGroup* ProcessGroupManager::CreateProcessGroup(uint32 leader_pid, const char* name) {
    // Allocate memory for the new process group
    ProcessGroup* new_group = (ProcessGroup*)malloc(sizeof(ProcessGroup));
    if (!new_group) {
        LOG("Failed to allocate memory for new process group");
        return nullptr;
    }
    
    // Initialize the process group
    new_group->pgid = next_pgid++;
    new_group->session_id = INVALID_SID;  // Will be set when added to session
    new_group->leader_pid = leader_pid;
    new_group->process_count = 0;
    new_group->processes = nullptr;
    new_group->next = group_list_head;
    new_group->prev = nullptr;
    new_group->creation_time = global_timer ? global_timer->GetTickCount() : 0;
    new_group->flags = 0;
    
    // Set group name
    if (name) {
        for (int i = 0; i < 31 && name[i] != '\0'; i++) {
            new_group->name[i] = name[i];
        }
        new_group->name[31] = '\0';
    } else {
        snprintf(new_group->name, sizeof(new_group->name), "PG-%u", new_group->pgid);
    }
    
    // Add to group list
    if (group_list_head) {
        group_list_head->prev = new_group;
    }
    group_list_head = new_group;
    
    DLOG("Created process group PGID: " << new_group->pgid << ", name: " << new_group->name);
    
    // Add the leader process to this group if process manager is available
    if (process_manager) {
        ProcessControlBlock* leader = process_manager->GetProcessById(leader_pid);
        if (leader) {
            AddProcessToGroup(leader_pid, new_group->pgid);
        }
    }
    
    return new_group;
}

bool ProcessGroupManager::DestroyProcessGroup(uint32 pgid) {
    ProcessGroup* target = GetProcessGroupById(pgid);
    if (!target) {
        LOG("Attempted to destroy non-existent process group with PGID: " << pgid);
        return false;
    }
    
    // Remove from group list
    if (target->prev) {
        target->prev->next = target->next;
    } else {
        group_list_head = target->next;
    }
    
    if (target->next) {
        target->next->prev = target->prev;
    }
    
    // If this was the current group, update current_group
    if (current_group == target) {
        current_group = nullptr;
    }
    
    // Free allocated memory
    free(target);
    
    DLOG("Destroyed process group with PGID: " << pgid);
    
    return true;
}

ProcessGroup* ProcessGroupManager::GetProcessGroupById(uint32 pgid) {
    ProcessGroup* current = group_list_head;
    while (current) {
        if (current->pgid == pgid) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

ProcessGroup* ProcessGroupManager::GetProcessGroupByPid(uint32 pid) {
    // Find the process group that contains the specified process
    ProcessGroup* current_group = group_list_head;
    while (current_group) {
        // Check if this group contains the process
        ProcessControlBlock* current_process = current_group->processes;
        while (current_process) {
            if (current_process->pid == pid) {
                return current_group;
            }
            // Move to next process in this group (need to implement linked list for processes)
            // For now, we'll assume each group has a single process or use the process manager
            if (process_manager) {
                ProcessControlBlock* process = process_manager->GetProcessById(pid);
                if (process && process->pgid == current_group->pgid) {
                    return current_group;
                }
            }
            break;  // Simplified implementation
        }
        current_group = current_group->next;
    }
    return nullptr;
}

bool ProcessGroupManager::AddProcessToGroup(uint32 pid, uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to add process to non-existent group with PGID: " << pgid);
        return false;
    }
    
    // Get the process
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Attempted to add non-existent process with PID: " << pid);
        return false;
    }
    
    // Add process to group (simplified implementation)
    process->pgid = pgid;
    target_group->process_count++;
    
    DLOG("Added process PID: " << pid << " to process group PGID: " << pgid);
    
    return true;
}

bool ProcessGroupManager::RemoveProcessFromGroup(uint32 pid, uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to remove process from non-existent group with PGID: " << pgid);
        return false;
    }
    
    // Get the process
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Attempted to remove non-existent process with PID: " << pid);
        return false;
    }
    
    // Remove process from group (simplified implementation)
    if (process->pgid == pgid) {
        process->pgid = INVALID_PGID;
        if (target_group->process_count > 0) {
            target_group->process_count--;
        }
        
        DLOG("Removed process PID: " << pid << " from process group PGID: " << pgid);
        return true;
    }
    
    LOG("Process PID: " << pid << " is not in process group PGID: " << pgid);
    return false;
}

bool ProcessGroupManager::SetProcessGroupLeader(uint32 pid, uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to set leader for non-existent group with PGID: " << pgid);
        return false;
    }
    
    // Get the process
    if (!process_manager) {
        LOG("Process manager not available");
        return false;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Attempted to set leader for non-existent process with PID: " << pid);
        return false;
    }
    
    // Set the process as group leader
    target_group->leader_pid = pid;
    process->pgid = pgid;  // Ensure process is in the group
    
    DLOG("Set process PID: " << pid << " as leader of process group PGID: " << pgid);
    
    return true;
}

uint32 ProcessGroupManager::GetProcessGroupLeader(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to get leader for non-existent group with PGID: " << pgid);
        return INVALID_PID;
    }
    
    return target_group->leader_pid;
}

uint32 ProcessGroupManager::GetProcessGroupId(uint32 pid) {
    if (!process_manager) {
        LOG("Process manager not available");
        return INVALID_PGID;
    }
    
    ProcessControlBlock* process = process_manager->GetProcessById(pid);
    if (!process) {
        LOG("Attempted to get group ID for non-existent process with PID: " << pid);
        return INVALID_PGID;
    }
    
    return process->pgid;
}

bool ProcessGroupManager::IsProcessGroupLeader(uint32 pid) {
    // Find the process group that this process leads
    ProcessGroup* current = group_list_head;
    while (current) {
        if (current->leader_pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool ProcessGroupManager::IsProcessGroupEmpty(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to check emptiness for non-existent group with PGID: " << pgid);
        return true;  // Consider non-existent groups as empty
    }
    
    return target_group->process_count == 0;
}

uint32 ProcessGroupManager::GetProcessGroupCount(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to get count for non-existent group with PGID: " << pgid);
        return 0;
    }
    
    return target_group->process_count;
}

// Session management functions
Session* ProcessGroupManager::CreateSession(uint32 leader_pid, const char* name) {
    // Allocate memory for the new session
    Session* new_session = (Session*)malloc(sizeof(Session));
    if (!new_session) {
        LOG("Failed to allocate memory for new session");
        return nullptr;
    }
    
    // Initialize the session
    new_session->sid = next_sid++;
    new_session->leader_pid = leader_pid;
    new_session->group_count = 0;
    new_session->groups = nullptr;
    new_session->next = session_list_head;
    new_session->prev = nullptr;
    new_session->creation_time = global_timer ? global_timer->GetTickCount() : 0;
    new_session->flags = 0;
    new_session->controlling_terminal = 0;
    new_session->has_controlling_terminal = false;
    new_session->terminal_owner = nullptr;
    
    // Set session name
    if (name) {
        for (int i = 0; i < 31 && name[i] != '\0'; i++) {
            new_session->name[i] = name[i];
        }
        new_session->name[31] = '\0';
    } else {
        snprintf(new_session->name, sizeof(new_session->name), "SESSION-%u", new_session->sid);
    }
    
    // Add to session list
    if (session_list_head) {
        session_list_head->prev = new_session;
    }
    session_list_head = new_session;
    
    DLOG("Created session SID: " << new_session->sid << ", name: " << new_session->name);
    
    return new_session;
}

bool ProcessGroupManager::DestroySession(uint32 sid) {
    Session* target = GetSessionById(sid);
    if (!target) {
        LOG("Attempted to destroy non-existent session with SID: " << sid);
        return false;
    }
    
    // Remove from session list
    if (target->prev) {
        target->prev->next = target->next;
    } else {
        session_list_head = target->next;
    }
    
    if (target->next) {
        target->next->prev = target->prev;
    }
    
    // If this was the current session, update current_session
    if (current_session == target) {
        current_session = nullptr;
    }
    
    // Free allocated memory
    free(target);
    
    DLOG("Destroyed session with SID: " << sid);
    
    return true;
}

Session* ProcessGroupManager::GetSessionById(uint32 sid) {
    Session* current = session_list_head;
    while (current) {
        if (current->sid == sid) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

Session* ProcessGroupManager::GetSessionByPid(uint32 pid) {
    // Find the session that contains the process group with the specified process
    ProcessGroup* group = GetProcessGroupByPid(pid);
    if (group) {
        return GetSessionById(group->session_id);
    }
    return nullptr;
}

bool ProcessGroupManager::AddProcessGroupToSession(uint32 pgid, uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to add group to non-existent session with SID: " << sid);
        return false;
    }
    
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to add non-existent group with PGID: " << pgid);
        return false;
    }
    
    // Add group to session
    target_group->session_id = sid;
    target_session->group_count++;
    
    DLOG("Added process group PGID: " << pgid << " to session SID: " << sid);
    
    return true;
}

bool ProcessGroupManager::RemoveProcessGroupFromSession(uint32 pgid, uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to remove group from non-existent session with SID: " << sid);
        return false;
    }
    
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to remove non-existent group with PGID: " << pgid);
        return false;
    }
    
    // Remove group from session
    if (target_group->session_id == sid) {
        target_group->session_id = INVALID_SID;
        if (target_session->group_count > 0) {
            target_session->group_count--;
        }
        
        DLOG("Removed process group PGID: " << pgid << " from session SID: " << sid);
        return true;
    }
    
    LOG("Process group PGID: " << pgid << " is not in session SID: " << sid);
    return false;
}

bool ProcessGroupManager::SetSessionLeader(uint32 pid, uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to set leader for non-existent session with SID: " << sid);
        return false;
    }
    
    // Set the process as session leader
    target_session->leader_pid = pid;
    
    DLOG("Set process PID: " << pid << " as leader of session SID: " << sid);
    
    return true;
}

uint32 ProcessGroupManager::GetSessionLeader(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to get leader for non-existent session with SID: " << sid);
        return INVALID_PID;
    }
    
    return target_session->leader_pid;
}

uint32 ProcessGroupManager::GetSessionId(uint32 pid) {
    Session* session = GetSessionByPid(pid);
    if (session) {
        return session->sid;
    }
    return INVALID_SID;
}

bool ProcessGroupManager::IsSessionLeader(uint32 pid) {
    // Find the session that this process leads
    Session* current = session_list_head;
    while (current) {
        if (current->leader_pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool ProcessGroupManager::IsSessionEmpty(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to check emptiness for non-existent session with SID: " << sid);
        return true;  // Consider non-existent sessions as empty
    }
    
    return target_session->group_count == 0;
}

uint32 ProcessGroupManager::GetSessionGroupCount(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to get group count for non-existent session with SID: " << sid);
        return 0;
    }
    
    return target_session->group_count;
}

// Process group and session state management
bool ProcessGroupManager::SetProcessGroupState(uint32 pgid, ProcessGroupState new_state) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to set state for non-existent group with PGID: " << pgid);
        return false;
    }
    
    // For now, we'll just log the state change
    DLOG("Setting process group PGID: " << pgid << " state to " << GetProcessGroupStateName(new_state));
    
    return true;
}

ProcessGroupState ProcessGroupManager::GetProcessGroupState(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to get state for non-existent group with PGID: " << pgid);
        return PG_STATE_TERMINATED;
    }
    
    // For now, we'll just return ACTIVE state
    return PG_STATE_ACTIVE;
}

bool ProcessGroupManager::SetSessionState(uint32 sid, SessionState new_state) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to set state for non-existent session with SID: " << sid);
        return false;
    }
    
    // For now, we'll just log the state change
    DLOG("Setting session SID: " << sid << " state to " << GetSessionStateName(new_state));
    
    return true;
}

SessionState ProcessGroupManager::GetSessionState(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to get state for non-existent session with SID: " << sid);
        return SESSION_STATE_TERMINATED;
    }
    
    // For now, we'll just return ACTIVE state
    return SESSION_STATE_ACTIVE;
}

// Process group and session navigation
ProcessGroup* ProcessGroupManager::GetCurrentProcessGroup() {
    return current_group;
}

Session* ProcessGroupManager::GetCurrentSession() {
    return current_session;
}

bool ProcessGroupManager::SetCurrentProcessGroup(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to set current group to non-existent group with PGID: " << pgid);
        return false;
    }
    
    current_group = target_group;
    DLOG("Set current process group to PGID: " << pgid);
    
    return true;
}

bool ProcessGroupManager::SetCurrentSession(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to set current session to non-existent session with SID: " << sid);
        return false;
    }
    
    current_session = target_session;
    DLOG("Set current session to SID: " << sid);
    
    return true;
}

// Process group and session signaling
bool ProcessGroupManager::SendSignalToGroup(uint32 pgid, uint32 signal) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to send signal to non-existent group with PGID: " << pgid);
        return false;
    }
    
    DLOG("Sending signal " << signal << " to process group PGID: " << pgid);
    
    // In a real implementation, we would iterate through all processes in the group
    // and send the signal to each one
    
    return true;
}

bool ProcessGroupManager::SendSignalToSession(uint32 sid, uint32 signal) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to send signal to non-existent session with SID: " << sid);
        return false;
    }
    
    DLOG("Sending signal " << signal << " to session SID: " << sid);
    
    // In a real implementation, we would iterate through all process groups in the session
    // and send the signal to each group
    
    return true;
}

bool ProcessGroupManager::SendSignalToForegroundProcess(uint32 signal) {
    if (!current_group) {
        LOG("No current process group");
        return false;
    }
    
    DLOG("Sending signal " << signal << " to foreground process group PGID: " << current_group->pgid);
    
    // In a real implementation, we would send the signal to the foreground process
    
    return true;
}

// Job control support
bool ProcessGroupManager::SetForegroundProcessGroup(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to set foreground group to non-existent group with PGID: " << pgid);
        return false;
    }
    
    DLOG("Setting process group PGID: " << pgid << " as foreground group");
    
    // In a real implementation, we would update terminal control and job control state
    
    return true;
}

uint32 ProcessGroupManager::GetForegroundProcessGroup() {
    if (current_group) {
        return current_group->pgid;
    }
    return INVALID_PGID;
}

bool ProcessGroupManager::IsProcessGroupInForeground(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to check foreground status for non-existent group with PGID: " << pgid);
        return false;
    }
    
    // In a real implementation, we would check if this group is the foreground group
    return current_group && current_group->pgid == pgid;
}

bool ProcessGroupManager::SuspendBackgroundProcessGroup(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to suspend non-existent background group with PGID: " << pgid);
        return false;
    }
    
    DLOG("Suspending background process group PGID: " << pgid);
    
    // In a real implementation, we would send SIGSTOP to all processes in the group
    
    return true;
}

bool ProcessGroupManager::ResumeForegroundProcessGroup(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to resume non-existent foreground group with PGID: " << pgid);
        return false;
    }
    
    DLOG("Resuming foreground process group PGID: " << pgid);
    
    // In a real implementation, we would send SIGCONT to all processes in the group
    
    return true;
}

// Orphaned process group handling
bool ProcessGroupManager::HandleOrphanedProcessGroup(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to handle orphaned group for non-existent group with PGID: " << pgid);
        return false;
    }
    
    DLOG("Handling orphaned process group PGID: " << pgid);
    
    // Set the orphaned flag
    target_group->flags |= PG_FLAG_ORPHANED;
    
    // In a real implementation, we would send SIGHUP and SIGCONT to all processes
    
    return true;
}

bool ProcessGroupManager::IsProcessGroupOrphaned(uint32 pgid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    if (!target_group) {
        LOG("Attempted to check orphaned status for non-existent group with PGID: " << pgid);
        return false;
    }
    
    return (target_group->flags & PG_FLAG_ORPHANED) != 0;
}

bool ProcessGroupManager::AdoptOrphanedProcessGroup(uint32 pgid, uint32 new_parent_sid) {
    ProcessGroup* target_group = GetProcessGroupById(pgid);
    ProcessGroup* new_parent_group = GetProcessGroupById(new_parent_sid);
    
    if (!target_group) {
        LOG("Attempted to adopt non-existent group with PGID: " << pgid);
        return false;
    }
    
    if (!new_parent_group) {
        LOG("Attempted to adopt group to non-existent parent group with PGID: " << new_parent_sid);
        return false;
    }
    
    DLOG("Adopting orphaned process group PGID: " << pgid << " to parent group PGID: " << new_parent_sid);
    
    // Clear the orphaned flag
    target_group->flags &= ~PG_FLAG_ORPHANED;
    
    return true;
}

// Terminal control
bool ProcessGroupManager::SetControllingTerminal(uint32 sid, uint32 terminal_id) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to set controlling terminal for non-existent session with SID: " << sid);
        return false;
    }
    
    target_session->controlling_terminal = terminal_id;
    target_session->has_controlling_terminal = true;
    
    DLOG("Set controlling terminal " << terminal_id << " for session SID: " << sid);
    
    return true;
}

uint32 ProcessGroupManager::GetControllingTerminal(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to get controlling terminal for non-existent session with SID: " << sid);
        return 0;
    }
    
    return target_session->controlling_terminal;
}

bool ProcessGroupManager::HasControllingTerminal(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to check controlling terminal for non-existent session with SID: " << sid);
        return false;
    }
    
    return target_session->has_controlling_terminal;
}

bool ProcessGroupManager::ReleaseControllingTerminal(uint32 sid) {
    Session* target_session = GetSessionById(sid);
    if (!target_session) {
        LOG("Attempted to release controlling terminal for non-existent session with SID: " << sid);
        return false;
    }
    
    target_session->controlling_terminal = 0;
    target_session->has_controlling_terminal = false;
    target_session->terminal_owner = nullptr;
    
    DLOG("Released controlling terminal for session SID: " << sid);
    
    return true;
}

// Utility functions
uint32 ProcessGroupManager::GetProcessGroupCount() {
    uint32 count = 0;
    ProcessGroup* current = group_list_head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

uint32 ProcessGroupManager::GetSessionCount() {
    uint32 count = 0;
    Session* current = session_list_head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void ProcessGroupManager::PrintProcessGroupList() {
    LOG("=== Process Group List ===");
    ProcessGroup* current = group_list_head;
    while (current) {
        LOG("  PGID: " << current->pgid 
            << ", Name: " << current->name 
            << ", Leader PID: " << current->leader_pid
            << ", Session ID: " << current->session_id
            << ", Processes: " << current->process_count
            << ", Flags: 0x" << current->flags);
        current = current->next;
    }
    LOG("Total process groups: " << GetProcessGroupCount());
    LOG("===========================");
}

void ProcessGroupManager::PrintSessionList() {
    LOG("======= Session List =======");
    Session* current = session_list_head;
    while (current) {
        LOG("  SID: " << current->sid 
            << ", Name: " << current->name 
            << ", Leader PID: " << current->leader_pid
            << ", Groups: " << current->group_count
            << ", Terminal: " << (current->has_controlling_terminal ? current->controlling_terminal : 0)
            << ", Flags: 0x" << current->flags);
        current = current->next;
    }
    LOG("Total sessions: " << GetSessionCount());
    LOG("=============================");
}

void ProcessGroupManager::PrintProcessGroupTree() {
    LOG("==== Process Group Tree ====");
    Session* current_session = session_list_head;
    while (current_session) {
        LOG("Session SID: " << current_session->sid 
            << " (\"" << current_session->name << "\")");
        
        ProcessGroup* current_group = group_list_head;
        while (current_group) {
            if (current_group->session_id == current_session->sid) {
                LOG("  └─ Group PGID: " << current_group->pgid 
                    << " (\"" << current_group->name << "\")"
                    << ", Processes: " << current_group->process_count);
            }
            current_group = current_group->next;
        }
        current_session = current_session->next;
    }
    LOG("=============================");
}

const char* ProcessGroupManager::GetProcessGroupStateName(ProcessGroupState state) {
    switch (state) {
        case PG_STATE_ACTIVE: return "ACTIVE";
        case PG_STATE_EMPTY: return "EMPTY";
        case PG_STATE_TERMINATING: return "TERMINATING";
        case PG_STATE_TERMINATED: return "TERMINATED";
        default: return "INVALID";
    }
}

const char* ProcessGroupManager::GetSessionStateName(SessionState state) {
    switch (state) {
        case SESSION_STATE_ACTIVE: return "ACTIVE";
        case SESSION_STATE_EMPTY: return "EMPTY";
        case SESSION_STATE_TERMINATING: return "TERMINATING";
        case SESSION_STATE_TERMINATED: return "TERMINATED";
        default: return "INVALID";
    }
}

// Process group and session cleanup
bool ProcessGroupManager::CleanupTerminatedGroups() {
    ProcessGroup* current = group_list_head;
    while (current) {
        ProcessGroup* next = current->next;
        if (GetProcessGroupState(current->pgid) == PG_STATE_TERMINATED) {
            DestroyProcessGroup(current->pgid);
        }
        current = next;
    }
    return true;
}

bool ProcessGroupManager::CleanupTerminatedSessions() {
    Session* current = session_list_head;
    while (current) {
        Session* next = current->next;
        if (GetSessionState(current->sid) == SESSION_STATE_TERMINATED) {
            DestroySession(current->sid);
        }
        current = next;
    }
    return true;
}

bool ProcessGroupManager::ForceCleanupAllGroups() {
    ProcessGroup* current = group_list_head;
    while (current) {
        ProcessGroup* next = current->next;
        free(current);
        current = next;
    }
    group_list_head = nullptr;
    current_group = nullptr;
    return true;
}

bool ProcessGroupManager::ForceCleanupAllSessions() {
    Session* current = session_list_head;
    while (current) {
        Session* next = current->next;
        free(current);
        current = next;
    }
    session_list_head = nullptr;
    current_session = nullptr;
    return true;
}