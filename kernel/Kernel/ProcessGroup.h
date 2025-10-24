#ifndef _Kernel_ProcessGroup_h_
#define _Kernel_ProcessGroup_h_

#include "Defs.h"
#include "ProcessControlBlock.h"

// Process group structure
struct ProcessGroup {
    uint32 pgid;                    // Process group ID (same as PID of session leader)
    uint32 session_id;             // Session ID this group belongs to
    uint32 leader_pid;             // PID of group leader
    uint32 process_count;          // Number of processes in this group
    ProcessControlBlock* processes; // Linked list of processes in this group
    ProcessGroup* next;           // Next process group in session
    ProcessGroup* prev;           // Previous process group in session
    char name[32];                // Group name for debugging
    uint32 creation_time;         // Time when group was created
    uint32 flags;                 // Group flags
};

// Session structure
struct Session {
    uint32 sid;                    // Session ID (same as PID of session leader)
    uint32 leader_pid;            // PID of session leader
    uint32 group_count;           // Number of process groups in this session
    ProcessGroup* groups;         // Linked list of process groups in this session
    Session* next;                // Next session
    Session* prev;                // Previous session
    char name[32];                // Session name for debugging
    uint32 creation_time;        // Time when session was created
    uint32 flags;                // Session flags
    
    // Session control terminal
    uint32 controlling_terminal;  // Terminal device ID controlling this session
    bool has_controlling_terminal; // Whether session has a controlling terminal
    ProcessControlBlock* terminal_owner; // Process that owns the terminal
};

// Process group and session flags
const uint32 PG_FLAG_ORPHANED = 0x00000001;     // Group is orphaned (leader died)
const uint32 PG_FLAG_JOB_CONTROL = 0x00000002;  // Group supports job control
const uint32 SESSION_FLAG_LOGIN = 0x00000001;   // Session is a login session
const uint32 SESSION_FLAG_FOREGROUND = 0x00000002; // Session is in foreground

// Process group and session management constants
const uint32 INVALID_PGID = 0xFFFFFFFF;
const uint32 INVALID_SID = 0xFFFFFFFF;
const uint32 INITIAL_PGID = 1;     // Initial process group ID
const uint32 INITIAL_SID = 1;       // Initial session ID

// Process group and session states
enum ProcessGroupState {
    PG_STATE_ACTIVE = 0,          // Group is active with processes
    PG_STATE_EMPTY,               // Group has no processes
    PG_STATE_TERMINATING,        // Group is being terminated
    PG_STATE_TERMINATED           // Group has been terminated
};

enum SessionState {
    SESSION_STATE_ACTIVE = 0,     // Session is active with groups
    SESSION_STATE_EMPTY,          // Session has no groups
    SESSION_STATE_TERMINATING,    // Session is being terminated
    SESSION_STATE_TERMINATED      // Session has been terminated
};

// Process group and session management functions
class ProcessGroupManager {
private:
    ProcessGroup* group_list_head;
    Session* session_list_head;
    uint32 next_pgid;
    uint32 next_sid;
    ProcessGroup* current_group;  // Currently active process group
    Session* current_session;      // Currently active session
    
public:
    ProcessGroupManager();
    ~ProcessGroupManager();
    
    // Process group management
    ProcessGroup* CreateProcessGroup(uint32 leader_pid, const char* name = nullptr);
    bool DestroyProcessGroup(uint32 pgid);
    ProcessGroup* GetProcessGroupById(uint32 pgid);
    ProcessGroup* GetProcessGroupByPid(uint32 pid);
    bool AddProcessToGroup(uint32 pid, uint32 pgid);
    bool RemoveProcessFromGroup(uint32 pid, uint32 pgid);
    bool SetProcessGroupLeader(uint32 pid, uint32 pgid);
    uint32 GetProcessGroupLeader(uint32 pgid);
    uint32 GetProcessGroupId(uint32 pid);
    bool IsProcessGroupLeader(uint32 pid);
    bool IsProcessGroupEmpty(uint32 pgid);
    uint32 GetProcessGroupCount(uint32 pgid);
    
    // Session management
    Session* CreateSession(uint32 leader_pid, const char* name = nullptr);
    bool DestroySession(uint32 sid);
    Session* GetSessionById(uint32 sid);
    Session* GetSessionByPid(uint32 pid);
    bool AddProcessGroupToSession(uint32 pgid, uint32 sid);
    bool RemoveProcessGroupFromSession(uint32 pgid, uint32 sid);
    bool SetSessionLeader(uint32 pid, uint32 sid);
    uint32 GetSessionLeader(uint32 sid);
    uint32 GetSessionId(uint32 pid);
    bool IsSessionLeader(uint32 pid);
    bool IsSessionEmpty(uint32 sid);
    uint32 GetSessionGroupCount(uint32 sid);
    
    // Process group and session state management
    bool SetProcessGroupState(uint32 pgid, ProcessGroupState new_state);
    ProcessGroupState GetProcessGroupState(uint32 pgid);
    bool SetSessionState(uint32 sid, SessionState new_state);
    SessionState GetSessionState(uint32 sid);
    
    // Process group and session navigation
    ProcessGroup* GetCurrentProcessGroup();
    Session* GetCurrentSession();
    bool SetCurrentProcessGroup(uint32 pgid);
    bool SetCurrentSession(uint32 sid);
    
    // Process group and session signaling
    bool SendSignalToGroup(uint32 pgid, uint32 signal);
    bool SendSignalToSession(uint32 sid, uint32 signal);
    bool SendSignalToForegroundProcess(uint32 signal);
    
    // Job control support
    bool SetForegroundProcessGroup(uint32 pgid);
    uint32 GetForegroundProcessGroup();
    bool IsProcessGroupInForeground(uint32 pgid);
    bool SuspendBackgroundProcessGroup(uint32 pgid);
    bool ResumeForegroundProcessGroup(uint32 pgid);
    
    // Orphaned process group handling
    bool HandleOrphanedProcessGroup(uint32 pgid);
    bool IsProcessGroupOrphaned(uint32 pgid);
    bool AdoptOrphanedProcessGroup(uint32 pgid, uint32 new_parent_sid);
    
    // Terminal control
    bool SetControllingTerminal(uint32 sid, uint32 terminal_id);
    uint32 GetControllingTerminal(uint32 sid);
    bool HasControllingTerminal(uint32 sid);
    bool ReleaseControllingTerminal(uint32 sid);
    
    // Utility functions
    uint32 GetProcessGroupCount();
    uint32 GetSessionCount();
    void PrintProcessGroupList();
    void PrintSessionList();
    void PrintProcessGroupTree();
    const char* GetProcessGroupStateName(ProcessGroupState state);
    const char* GetSessionStateName(SessionState state);
    
    // Process group and session cleanup
    bool CleanupTerminatedGroups();
    bool CleanupTerminatedSessions();
    bool ForceCleanupAllGroups();
    bool ForceCleanupAllSessions();
};

// Global process group manager instance
extern ProcessGroupManager* process_group_manager;

// Process group and session related system calls
uint32 SysCallSetProcessGroup(uint32 pid, uint32 pgid);
uint32 SysCallGetProcessGroup(uint32 pid);
uint32 SysCallSetSession(uint32 pid, uint32 sid);
uint32 SysCallGetSession(uint32 pid);
uint32 SysCallCreateSession(uint32 pid);
uint32 SysCallSetForegroundProcessGroup(uint32 pgid);
uint32 SysCallGetForegroundProcessGroup();

#endif // _Kernel_ProcessGroup_h_