#ifndef _Kernel_LinuxulatorAbi_h_
#define _Kernel_LinuxulatorAbi_h_

#include "Common.h"
#include "Defs.h"
#include "ProcessControlBlock.h"
#include "AbiMultiplexer.h"
#include "Linuxulator.h"

// Linuxulator ABI context for process-specific data
struct LinuxulatorAbiContext {
    LinuxProcess* linux_process;      // Linux-specific process data
    uint32_t abi_flags;               // ABI-specific flags
    uint8_t* personality_mask;        // Process personality/compatibility settings
    uint32_t signal_mask;             // Signal mask for this process
    uint32_t blocked_signals;         // Currently blocked signals
    uint32_t pending_signals;         // Pending signals
    uint32_t ignored_signals;         // Ignored signals
    uint32_t caught_signals;          // Caught signals
    void* alt_stack;                  // Alternative signal stack
    size_t alt_stack_size;            // Size of alternative stack
    void* vDSO_mapping;               // Virtual Dynamic Shared Object mapping
    uint32_t vDSO_size;               // Size of vDSO
    uint32_t vDSO_addr;               // Address of vDSO
    uint32_t auxv_entries[32];        // Auxiliary vector entries
    uint32_t auxv_count;              // Number of auxiliary vector entries
};

// Linuxulator ABI interface class
class LinuxulatorAbi {
private:
    LinuxulatorAbiContext global_context;  // Global context for Linuxulator ABI
    Spinlock linuxulator_abi_lock;         // Lock for thread safety

public:
    LinuxulatorAbi();
    ~LinuxulatorAbi();
    
    // Initialize the Linuxulator ABI
    bool Initialize();
    
    // Handle Linuxulator syscalls
    int HandleSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                     uint32 arg4, uint32 arg5, uint32 arg6);
    
    // Linuxulator-specific implementations
    int LinuxulatorRead(int fd, void* buf, size_t count);
    int LinuxulatorWrite(int fd, const void* buf, size_t count);
    int LinuxulatorOpen(const char* pathname, int flags, mode_t mode);
    int LinuxulatorClose(int fd);
    int LinuxulatorStat(const char* pathname, struct FileStat* statbuf);
    int LinuxulatorFstat(int fd, struct FileStat* statbuf);
    int LinuxulatorLstat(const char* pathname, struct FileStat* statbuf);
    int LinuxulatorLseek(int fd, off_t offset, int whence);
    int LinuxulatorMmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    int LinuxulatorMunmap(void* addr, size_t length);
    int LinuxulatorBrk(void* addr);
    int LinuxulatorClone(uint32 flags, void* stack, int* parent_tid, int* child_tid, uint32 tls);
    int LinuxulatorFork();
    int LinuxulatorVFork();
    int LinuxulatorExecve(const char* filename, char* const argv[], char* const envp[]);
    int LinuxulatorExit(int status);
    int LinuxulatorWait4(pid_t pid, int* status, int options, struct rusage* rusage);
    int LinuxulatorKill(pid_t pid, int sig);
    int LinuxulatorUname(struct utsname* buf);
    int LinuxulatorGetPid();
    int LinuxulatorGetPpid();
    int LinuxulatorGetUid();
    int LinuxulatorGetGid();
    int LinuxulatorGetEuid();
    int LinuxulatorGetEgid();
    int LinuxulatorSetUid(uid_t uid);
    int LinuxulatorSetGid(gid_t gid);
    int LinuxulatorChdir(const char* path);
    int LinuxulatorGetCwd(char* buf, size_t size);
    int LinuxulatorAccess(const char* pathname, int mode);
    int LinuxulatorPipe(int pipefd[2]);
    int LinuxulatorDup(int oldfd);
    int LinuxulatorDup2(int oldfd, int newfd);
    int LinuxulatorMkdir(const char* pathname, mode_t mode);
    int LinuxulatorRmdir(const char* pathname);
    int LinuxulatorUnlink(const char* pathname);
    int LinuxulatorRename(const char* oldpath, const char* newpath);
    int LinuxulatorChmod(const char* pathname, mode_t mode);
    int LinuxulatorFchmod(int fd, mode_t mode);
    int LinuxulatorChown(const char* pathname, uid_t owner, gid_t group);
    int LinuxulatorFchown(int fd, uid_t owner, gid_t group);
    int LinuxulatorGetTimeOfDay(struct timeval* tv, struct timezone* tz);
    int LinuxulatorNanosleep(const struct timespec* req, struct timespec* rem);
    int LinuxulatorSelect(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
    
    // Signal handling
    int LinuxulatorSignal(int signum, void (*handler)(int));
    int LinuxulatorSigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
    int LinuxulatorSigprocmask(int how, const sigset_t* set, sigset_t* oldset);
    int LinuxulatorSigreturn(void* ucontext);
    int LinuxulatorSigsuspend(const sigset_t* mask);
    int LinuxulatorSigpending(sigset_t* set);
    int LinuxulatorSigtimedwait(const sigset_t* set, siginfo_t* info, const struct timespec* timeout);
    int LinuxulatorSigqueueinfo(pid_t tgid, int sig, siginfo_t* uinfo);
    int LinuxulatorSigaltstack(const stack_t* ss, stack_t* oss);
    
    // Memory management
    int LinuxulatorMprotect(void* addr, size_t len, int prot);
    int LinuxulatorMremap(void* old_address, size_t old_size, size_t new_size, int flags, void* new_address);
    int LinuxulatorMsync(void* addr, size_t length, int flags);
    int LinuxulatorMincore(void* addr, size_t length, unsigned char* vec);
    int LinuxulatorMadvise(void* addr, size_t length, int advice);
    int LinuxulatorMlock(const void* addr, size_t len);
    int LinuxulatorMunlock(const void* addr, size_t len);
    int LinuxulatorMlockall(int flags);
    int LinuxulatorMunlockall(void);
    
    // File system operations
    int LinuxulatorStatfs(const char* path, struct statfs* buf);
    int LinuxulatorFstatfs(int fd, struct statfs* buf);
    int LinuxulatorTruncate(const char* path, off_t length);
    int LinuxulatorFtruncate(int fd, off_t length);
    int LinuxulatorGetdents(unsigned int fd, struct linux_dirent* dirp, unsigned int count);
    int LinuxulatorGetdents64(unsigned int fd, struct linux_dirent64* dirp, unsigned int count);
    int LinuxulatorSymlink(const char* target, const char* linkpath);
    int LinuxulatorReadlink(const char* pathname, char* buf, size_t bufsiz);
    int LinuxulatorLink(const char* oldpath, const char* newpath);
    int LinuxulatorMount(const char* source, const char* target, const char* filesystemtype, 
                        unsigned long mountflags, const void* data);
    int LinuxulatorUmount(const char* target);
    int LinuxulatorUmount2(const char* target, int flags);
    
    // Process scheduling
    int LinuxulatorSchedYield();
    int LinuxulatorSchedSetparam(pid_t pid, const struct sched_param* param);
    int LinuxulatorSchedGetparam(pid_t pid, struct sched_param* param);
    int LinuxulatorSchedSetscheduler(pid_t pid, int policy, const struct sched_param* param);
    int LinuxulatorSchedGetscheduler(pid_t pid);
    int LinuxulatorSchedGetPriorityMax(int policy);
    int LinuxulatorSchedGetPriorityMin(int policy);
    int LinuxulatorSchedRrGetInterval(pid_t pid, struct timespec* tp);
    int LinuxulatorSchedSetaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t* mask);
    int LinuxulatorSchedGetaffinity(pid_t pid, size_t cpusetsize, cpu_set_t* mask);
    
    // IPC operations
    int LinuxulatorSemget(key_t key, int nsems, int semflg);
    int LinuxulatorSemop(int semid, struct sembuf* sops, size_t nsops);
    int LinuxulatorSemctl(int semid, int semnum, int cmd, ...);
    int LinuxulatorMsgget(key_t key, int msgflg);
    int LinuxulatorMsgsnd(int msqid, const void* msgp, size_t msgsz, int msgflg);
    int LinuxulatorMsgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg);
    int LinuxulatorMsgctl(int msqid, int cmd, struct msqid_ds* buf);
    int LinuxulatorShmget(key_t key, size_t size, int shmflg);
    int LinuxulatorShmat(int shmid, const void* shmaddr, int shmflg);
    int LinuxulatorShmdt(const void* shmaddr);
    int LinuxulatorShmctl(int shmid, int cmd, struct shmid_ds* buf);
    
    // Socket operations
    int LinuxulatorSocket(int domain, int type, int protocol);
    int LinuxulatorConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int LinuxulatorAccept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxulatorBind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int LinuxulatorListen(int sockfd, int backlog);
    int LinuxulatorSend(int sockfd, const void* buf, size_t len, int flags);
    int LinuxulatorRecv(int sockfd, void* buf, size_t len, int flags);
    int LinuxulatorSendto(int sockfd, const void* buf, size_t len, int flags,
                         const struct sockaddr* dest_addr, socklen_t addrlen);
    int LinuxulatorRecvfrom(int sockfd, void* buf, size_t len, int flags,
                           struct sockaddr* src_addr, socklen_t* addrlen);
    int LinuxulatorSendmsg(int sockfd, const struct msghdr* msg, int flags);
    int LinuxulatorRecvmsg(int sockfd, struct msghdr* msg, int flags);
    int LinuxulatorGetsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen);
    int LinuxulatorSetsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
    int LinuxulatorGetsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxulatorGetpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int LinuxulatorShutdown(int sockfd, int how);
    int LinuxulatorSocketpair(int domain, int type, int protocol, int sv[2]);
    
    // Process and thread management
    int LinuxulatorClone3(struct clone_args* cl_args, size_t size);
    int LinuxulatorTkill(int tid, int sig);
    int LinuxulatorTgkill(int tgid, int tid, int sig);
    int LinuxulatorGettid();
    int LinuxulatorSetns(int fd, int nstype);
    int LinuxulatorUnshare(int flags);
    int LinuxulatorSetpgid(pid_t pid, pid_t pgid);
    int LinuxulatorGetpgid(pid_t pid);
    int LinuxulatorGetsid(pid_t pid);
    int LinuxulatorSetsid();
    int LinuxulatorSetuid(uid_t uid);
    int LinuxulatorSetgid(gid_t gid);
    int LinuxulatorSetreuid(uid_t ruid, uid_t euid);
    int LinuxulatorSetregid(gid_t rgid, gid_t egid);
    int LinuxulatorSetresuid(uid_t ruid, uid_t euid, uid_t suid);
    int LinuxulatorSetresgid(gid_t rgid, gid_t egid, gid_t sgid);
    int LinuxulatorSetfsuid(uid_t fsuid);
    int LinuxulatorSetfsgid(uid_t fsgid);
    int LinuxulatorGetgroups(int size, gid_t list[]);
    int LinuxulatorSetgroups(size_t size, const gid_t* list);
    int LinuxulatorGetrlimit(int resource, struct rlimit* rlim);
    int LinuxulatorSetrlimit(int resource, const struct rlimit* rlim);
    int LinuxulatorGetrusage(int who, struct rusage* usage);
    int LinuxulatorPrlimit(pid_t pid, int resource, const struct rlimit* new_limit, struct rlimit* old_limit);
    int LinuxulatorPrctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);
    int LinuxulatorArchPrctl(int code, unsigned long addr);
    int LinuxulatorPersonality(unsigned long persona);
    int LinuxulatorCapget(cap_user_header_t hdrp, cap_user_data_t datap);
    int LinuxulatorCapset(cap_user_header_t hdrp, const cap_user_data_t datap);
    int LinuxulatorIopl(int level);
    int LinuxulatorIoperm(unsigned long from, unsigned long num, int turn_on);
    
    // File descriptor operations
    int LinuxulatorFcntl(int fd, int cmd, ...);
    int LinuxulatorFlock(int fd, int operation);
    int LinuxulatorFsync(int fd);
    int LinuxulatorFdatasync(int fd);
    // int LinuxulatorFtruncate(int fd, off_t length);  // Duplicate declaration
    // int LinuxulatorFchmod(int fd, mode_t mode);  // Duplicate declaration
    // int LinuxulatorFchown(int fd, uid_t owner, gid_t group);  // Duplicate declaration
    int LinuxulatorFchdir(int fd);
    int LinuxulatorFgetxattr(int fd, const char* name, void* value, size_t size);
    int LinuxulatorFsetxattr(int fd, const char* name, const void* value, size_t size, int flags);
    int LinuxulatorFlistxattr(int fd, char* list, size_t size);
    int LinuxulatorFremovexattr(int fd, const char* name);
    int LinuxulatorFaccessat(int dirfd, const char* pathname, int mode, int flags);
    int LinuxulatorFchmodat(int dirfd, const char* pathname, mode_t mode, int flags);
    int LinuxulatorFchownat(int dirfd, const char* pathname, uid_t owner, gid_t group, int flags);
    int LinuxulatorFstatat(int dirfd, const char* pathname, struct stat* statbuf, int flags);
    int LinuxulatorFutimesat(int dirfd, const char* pathname, const struct timeval times[2]);
    int LinuxulatorUtimensat(int dirfd, const char* pathname, const struct timespec times[2], int flags);
    int LinuxulatorOpenat(int dirfd, const char* pathname, int flags, mode_t mode);
    int LinuxulatorMkdirat(int dirfd, const char* pathname, mode_t mode);
    int LinuxulatorMknodat(int dirfd, const char* pathname, mode_t mode, dev_t dev);
    int LinuxulatorUnlinkat(int dirfd, const char* pathname, int flags);
    int LinuxulatorRenameat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath);
    int LinuxulatorLinkat(int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags);
    int LinuxulatorSymlinkat(const char* target, int newdirfd, const char* linkpath);
    int LinuxulatorReadlinkat(int dirfd, const char* pathname, char* buf, size_t bufsiz);
    
    // Advanced I/O operations
    int LinuxulatorReadahead(int fd, off64_t offset, size_t count);
    int LinuxulatorSplice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);
    int LinuxulatorVmsplice(int fd, const struct iovec* iov, unsigned long nr_segs, unsigned int flags);
    int LinuxulatorTee(int fd_in, int fd_out, size_t len, unsigned int flags);
    int LinuxulatorSyncFileRange(int fd, off64_t offset, off64_t nbytes, unsigned int flags);
    int LinuxulatorIoSetup(unsigned nr_events, aio_context_t* ctx);
    int LinuxulatorIoDestroy(aio_context_t ctx);
    int LinuxulatorIoSubmit(aio_context_t ctx, long nr, struct iocb** iocbpp);
    int LinuxulatorIoCancel(aio_context_t ctx, struct iocb* iocb, struct io_event* result);
    int LinuxulatorIoGetEvents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout);
    int LinuxulatorIoPgetevents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* timeout, const struct __aio_sigset* sigmask);
    
    // Asynchronous I/O operations
    int LinuxulatorIoUringSetup(unsigned entries, struct io_uring_params* p);
    int LinuxulatorIoUringEnter(unsigned int fd, unsigned int to_submit, unsigned int min_complete, unsigned int flags, sigset_t* sig);
    int LinuxulatorIoUringRegister(unsigned int fd, unsigned int opcode, void* arg, unsigned int nr_args);
    
    // Process control
    int LinuxulatorPtrace(long request, pid_t pid, void* addr, void* data);
    int LinuxulatorWaitid(idtype_t idtype, id_t id, siginfo_t* infop, int options);
    
    // Memory mapping operations
    int LinuxulatorMmap2(unsigned long addr, unsigned long len, unsigned long prot, 
                        unsigned long flags, unsigned long fd, unsigned long pgoff);
    int LinuxulatorRemapFilePages(void* addr, size_t size, int prot, size_t pgoff, int flags);
    int LinuxulatorMbind(void* addr, unsigned long len, int mode, const unsigned long* nodemask,
                        unsigned long maxnode, unsigned flags);
    int LinuxulatorSetMempolicy(int mode, const unsigned long* nodemask, unsigned long maxnode);
    int LinuxulatorGetMempolicy(int* mode, unsigned long* nodemask, unsigned long maxnode, void* addr, unsigned long flags);
    int LinuxulatorMigratePages(int pid, unsigned long maxnode, const unsigned long* old_nodes, const unsigned long* new_nodes);
    int LinuxulatorMovePages(int pid, unsigned long count, void** pages, const int* nodes, int* status, int flags);
    
    // Key management
    int LinuxulatorAddKey(const char* type, const char* description, const void* payload, size_t plen, int ringid);
    int LinuxulatorRequestKey(const char* type, const char* description, const char* callout_info, int destringid);
    int LinuxulatorKeyctl(int cmd, ...);
    
    // Security operations
    int LinuxulatorSeccomp(unsigned int operation, unsigned int flags, void* args);
    int LinuxulatorLandlockCreateRuleset(const struct landlock_ruleset_attr* attr, size_t size, __u32 flags);
    int LinuxulatorLandlockAddRule(int ruleset_fd, enum landlock_rule_type rule_type, const void* rule_attr, __u32 flags);
    int LinuxulatorLandlockRestrictSelf(int ruleset_fd, __u32 flags);
    
    // Performance monitoring
    int LinuxulatorPerfEventOpen(struct perf_event_attr* attr, pid_t pid, int cpu, int group_fd, unsigned long flags);
    
    // Miscellaneous
    int LinuxulatorSysinfo(struct sysinfo* info);
    int LinuxulatorSyslog(int type, char* bufp, int len);
    int LinuxulatorAdjtimex(struct timex* buf);
    int LinuxulatorSettimeofday(const struct timeval* tv, const struct timezone* tz);
    int LinuxulatorClockSettime(clockid_t clk_id, const struct timespec* tp);
    int LinuxulatorClockGettime(clockid_t clk_id, struct timespec* tp);
    int LinuxulatorClockGetres(clockid_t clk_id, struct timespec* res);
    int LinuxulatorClockNanosleep(clockid_t clock_id, int flags, const struct timespec* request, struct timespec* remain);
    // int LinuxulatorNanosleep(const struct timespec* req, struct timespec* rem);  // Duplicate declaration
    int LinuxulatorGetrandom(void* buf, size_t buflen, unsigned int flags);
    int LinuxulatorMemfdCreate(const char* name, unsigned int flags);
    int LinuxulatorKexecLoad(unsigned long entry, unsigned long nr_segments, struct kexec_segment* segments, unsigned long flags);
    int LinuxulatorKexecFileLoad(int kernel_fd, int initrd_fd, unsigned long cmdline_len, const char* cmdline, unsigned long flags);
    int LinuxulatorReboot(int magic1, int magic2, unsigned int cmd, void* arg);
    int LinuxulatorSethostname(const char* name, size_t len);
    int LinuxulatorSetdomainname(const char* name, size_t len);
    // int LinuxulatorUname(struct utsname* buf);  // Duplicate declaration
    int LinuxulatorUstat(dev_t dev, struct ustat* ubuf);
    int LinuxulatorUtime(const char* filename, const struct utimbuf* times);
    int LinuxulatorUtimes(const char* filename, const struct timeval times[2]);
    int LinuxulatorFutimes(int fd, const struct timeval times[2]);
    int LinuxulatorFutimens(int fd, const struct timespec times[2]);
    // int LinuxulatorUtimensat(int dirfd, const char* pathname, const struct timespec times[2], int flags);  // Duplicate declaration
    int LinuxulatorAcct(const char* filename);
    int LinuxulatorSwapon(const char* path, int swapflags);
    int LinuxulatorSwapoff(const char* path);
    int LinuxulatorQuotactl(int cmd, const char* special, int id, caddr_t addr);
    int LinuxulatorNfsservctl(int cmd, struct nfsctl_arg* arg, union nfsctl_res* res);
    int LinuxulatorBpf(int cmd, union bpf_attr* attr, unsigned int size);
    int LinuxulatorFinitModule(int fd, const char* param_values, int flags);
    int LinuxulatorInitModule(void* module_image, unsigned long len, const char* param_values);
    int LinuxulatorDeleteModule(const char* name, unsigned int flags);
    int LinuxulatorCreateModule(const char* name, size_t size);
    int LinuxulatorQueryModule(const char* name, int which, void* buf, size_t bufsize, size_t* ret);
    int LinuxulatorGetKernelSyms(struct kernel_sym* table);
    int LinuxulatorLookupDcookie(uint64_t cookie64, char* buf, size_t len);
    int LinuxulatorKcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2);
    int LinuxulatorProcessVmReadv(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                                 const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int LinuxulatorProcessVmWritev(pid_t pid, const struct iovec* liov, unsigned long liovcnt,
                                  const struct iovec* riov, unsigned long riovcnt, unsigned long flags);
    int LinuxulatorPkeyMprotect(void* addr, size_t len, int prot, int pkey);
    int LinuxulatorPkeyAlloc(unsigned long flags, unsigned long access_rights);
    int LinuxulatorPkeyFree(int pkey);
    int LinuxulatorStatx(int dirfd, const char* pathname, int flags, unsigned int mask, struct statx* statxbuf);
    int LinuxulatorIoPgetevents(aio_context_t ctx_id, long min_nr, long nr, struct io_event* events,
                               const struct timespec* timeout, const struct __aio_sigset* sevp);
    int LinuxulatorRseq(struct rseq* rseq, uint32_t rseq_len, int flags, uint32_t sig);
    int LinuxulatorPidfdSendSignal(int pidfd, int sig, siginfo_t* info, unsigned int flags);
    int LinuxulatorOpenTree(int dfd, const char* pathname, unsigned int flags);
    int LinuxulatorMoveMount(int from_dfd, const char* from_pathname, int to_dfd, const char* to_pathname, unsigned int flags);
    int LinuxulatorFsopen(const char* fs_name, unsigned int flags);
    int LinuxulatorFsconfig(int fs_fd, unsigned int cmd, const char* key, const void* value, int aux);
    int LinuxulatorFsmount(int fs_fd, unsigned int flags, unsigned int mount_attrs);
    int LinuxulatorFspick(int dfd, const char* path, unsigned int flags);
    int LinuxulatorPidfdOpen(pid_t pid, unsigned int flags);
    // int LinuxulatorClone3(struct clone_args* cl_args, size_t size);  // Duplicate declaration
    int LinuxulatorCloseRange(unsigned int fd, unsigned int max_fd, unsigned int flags);
    int LinuxulatorOpenat2(int dirfd, const char* pathname, struct open_how* how, size_t size);
    int LinuxulatorPidfdGetfd(int pidfd, int targetfd, unsigned int flags);
    int LinuxulatorFaccessat2(int dirfd, const char* pathname, int mode, int flags);
    int LinuxulatorProcessMadvise(int pidfd, const struct iovec* iov, size_t iovcnt, int advice, unsigned long flags);
    int LinuxulatorEpollPwait2(int epfd, struct epoll_event* events, int maxevents, const struct timespec* timeout,
                              const sigset_t* sigmask, size_t sigsetsize);
    int LinuxulatorMountSetattr(int dfd, const char* path, unsigned int flags, struct mount_attr* uattr, size_t usize);
    int LinuxulatorQuotactlFd(unsigned int fd, unsigned int cmd, int id, void* addr);
    int LinuxulatorMemfdSecret(unsigned int flags);
    int LinuxulatorProcessMrelease(int pidfd, unsigned int flags);
    int LinuxulatorFutexWaitv(struct futex_waitv* waiters, unsigned int nr_futexes, unsigned int flags,
                             struct timespec* timeout, clockid_t clockid);
    int LinuxulatorSetMempolicyHomeNode(unsigned long start, unsigned long len, unsigned long home_node, unsigned long flags);
    
private:
    // Internal helper functions
    int DispatchSyscall(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3, 
                       uint32 arg4, uint32 arg5, uint32 arg6);
    LinuxulatorAbiContext* GetProcessAbiContext(ProcessControlBlock* pcb);
    bool SetProcessAbiContext(ProcessControlBlock* pcb, LinuxulatorAbiContext* context);
    int TranslateLinuxulatorError(int linux_error);
    int TranslateLinuxulatorToKernelError(int linux_error);
    int TranslateKernelToLinuxulatorError(int kernel_error);
    bool IsProcessLinuxulator(ProcessControlBlock* pcb);
    
    // ELF loading and execution helpers
    bool LoadAndExecuteElf(LinuxElfHeader* elf_header, const char* filename, 
                          char* const argv[], char* const envp[]);
    bool PrepareProgramHeaders(LinuxElfHeader* elf_header, const char* filename);
    bool MapSegment(uint32_t type, uint32_t offset, uint32_t vaddr, uint32_t paddr, 
                   uint32_t filesz, uint32_t memsz, uint32_t flags, uint32_t align);
    void* AllocateVirtualMemory(uint32_t size, uint32_t prot, uint32_t flags);
    bool SetupProcessStack(void* stack, size_t size, char* const argv[], char* const envp[]);
    bool SetupProcessAuxv(void* auxv, LinuxElfHeader* elf_header);
    
    // Linuxulator-specific helpers
    int GetLinuxulatorPid();
    int GetLinuxulatorTid();
    bool ValidateLinuxulatorParams();
    void InitializeLinuxulatorProcess(LinuxulatorAbiContext* context);
    void CleanupLinuxulatorProcess(LinuxulatorAbiContext* context);
    bool SetupSignalHandlers(LinuxulatorAbiContext* context);
    bool SetupVDSO(LinuxulatorAbiContext* context);
    bool SetupMemoryMappings(LinuxulatorAbiContext* context);
    bool SetupPersonality(LinuxulatorAbiContext* context, uint32 personality);
};

// Global Linuxulator ABI instance
extern LinuxulatorAbi* g_linuxulator_abi;

// Initialize the Linuxulator ABI
bool InitializeLinuxulatorAbi();

// Handle Linuxulator syscalls
extern "C" int HandleLinuxulatorSyscall(uint32 syscall_num, 
                                       uint32 arg1, uint32 arg2, uint32 arg3, 
                                       uint32 arg4, uint32 arg5, uint32 arg6);

// Setup Linuxulator ABI syscall table for the ABI multiplexer
bool SetupLinuxulatorAbiSyscallTable();

#endif