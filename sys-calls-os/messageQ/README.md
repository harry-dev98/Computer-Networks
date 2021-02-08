##### A process P1 has to create a message queue and places several messages on the queue for processes P2 (type as 2 ) and P3 ( type as 3 ) and inquire about the state of the queue with msgctl(), and prints the many details of the message queue. Now Process P2 and P3 will read few of their messages. Now again process P1 has to inquire about the state of the queue with msgctl(), and prints the many details of the message queue. Next Process P1 has to do the below: It has to use msgctl() to alter (change ) a message on the queue. Something like it mayhavv eto change the text of a message. And check whether the change has occurred by printing all the messages of the queue without msgrcv().P has to use msgctl() to delete a message from the queue, then print all the messages of the queue to check whether a message has been deleted. Note that the message queue contents should not be changed after printing all the messages by using msgctl().

### System call used:


## New Calls introduced



```cpp

int msgget(key_t key, int msgflg);
/*
    Upon creation, the least significant bits of the argument msgflg
    define the permissions of the message queue. 

    A new message queue is created only if key has the value IPC_PRIVATE
    or key isn't IPC_PRIVATE, no message queue with the given key key
    exists, and IPC_CREAT is specified in msgflg.

    If msgflg specifies both IPC_CREAT and IPC_EXCL and a message
    queue already exists for key, then msgget() fails with errno set
    to EEXIST.

    If a new message queue is created, then its associated data
    structure msqid_ds accessible only via with msgctl is initialized as follows:

    • msg_perm.cuid and msg_perm.uid are set to the effective user ID
    of the calling process.

    • msg_perm.cgid and msg_perm.gid are set to the effective group
    ID of the calling process.

    • The least significant 9 bits of msg_perm.mode are set to the
    least significant 9 bits of msgflg.

    • msg_qnum, msg_lspid, msg_lrpid, msg_stime, and msg_rtime are
    set to 0.

    • msg_ctime is set to the current time.

    • msg_qbytes is set to the system limit MSGMNB.

    If the message queue already exists the permissions are verified,
    and a check is made to see if it is marked for destruction.

    Note:-    
        The in`clusion of <sys/types.h> and <sys/ipc.h> isn't required on
        Linux or by any version of POSIX.  However, some old
        implementations required the inclusion of these header files, and
        the SVID also documented their inclusion.  Applications intended
        to be portable to such old systems may need to include these
        header files.

        IPC_PRIVATE isn't a flag field but a key_t type.  If this special
        value is used for key, the system call ignores everything but the
        least significant 9 bits of msgflg and creates a new message
        queue (on success).

        The following is a system limit on message queue resources
        affecting a msgget() call:

        MSGMNI System-wide limit on the number of message queues.  Before
        Linux 3.19, the default value for this limit was
        calculated using a formula based on available system
        memory.  Since Linux 3.19, the default value is 32,000.
        On Linux, this limit can be read and modified via
        /proc/sys/kernel/msgmni.
*/
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
/*
    The msgsnd() system call appends a copy of the message pointed to
    by msgp to the message queue whose identifier is specified by
    msqid.

    If sufficient space is available in the queue, msgsnd() succeeds
    immediately.  The queue capacity is governed by the msg_qbytes
    field in the associated data structure for the message queue.
    During queue creation this field is initialized to MSGMNB bytes,
    but this limit can be modified using msgctl(2).  A message queue
    is considered to be full if either of the following conditions is
    true:

    • Adding a new message to the queue would cause the total number
        of bytes in the queue to exceed the queue's maximum size (the
        msg_qbytes field).

    • Adding another message to the queue would cause the total
        number of messages in the queue to exceed the queue's maximum
        size (the msg_qbytes field).  This check is necessary to
        prevent an unlimited number of zero-length messages being
        placed on the queue.  Although such messages contain no data,
        they nevertheless consume (locked) kernel memory.

    If insufficient space is available in the queue, then the default
    behavior of msgsnd() is to block until space becomes available.
    If IPC_NOWAIT is specified in msgflg, then the call instead fails
    with the error EAGAIN.

    A blocked msgsnd() call may also fail if:

    • the queue is removed, in which case the system call fails with
        errno set to EIDRM; or

    • a signal is caught, in which case the system call fails with
        errno set to EINTR;see signal(7).  (msgsnd() is never
        automatically restarted after being interrupted by a signal
        handler, regardless of the setting of the SA_RESTART flag when
        establishing a signal handler.)

    Upon successful completion the message queue data structure is
    updated as follows:

    • msg_lspid is set to the process ID of the calling process.

    • msg_qnum is incremented by 1.

    • msg_stime is set to the current time.  

*/
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

/*
    The msgrcv() system call removes a message from the queue
    specified by msqid and places it in the buffer pointed to by
    msgp.

    The argument msgsz specifies the maximum size in bytes for the
    member mtext of the structure pointed to by the msgp argument.
    If the message text has length greater than msgsz, then the
    behavior depends on whether MSG_NOERROR is specified in msgflg.
    If MSG_NOERROR is specified, then the message text will be
    truncated (and the truncated part will be lost); if MSG_NOERROR
    is not specified, then the message isn't removed from the queue
    and the system call fails returning -1 with errno set to E2BIG.

    Unless MSG_COPY is specified in msgflg (see below), the msgtyp
    argument specifies the type of message requested, as follows:

    • If msgtyp is 0, then the first message in the queue is read.

    • If msgtyp is greater than 0, then the first message in the
        queue of type msgtyp is read, unless MSG_EXCEPT was specified
        in msgflg, in which case the first message in the queue of type
        not equal to msgtyp will be read.

    • If msgtyp is less than 0, then the first message in the queue
        with the lowest type less than or equal to the absolute value
        of msgtyp will be read.

    The msgflg argument is a bit mask constructed by ORing together
    zero or more of the following flags:

    IPC_NOWAIT
            Return immediately if no message of the requested type is
            in the queue.  The system call fails with errno set to
            ENOMSG.

    MSG_COPY (since Linux 3.8)
            Nondestructively fetch a copy of the message at the
            ordinal position in the queue specified by msgtyp
            (messages are considered to be numbered starting at 0).

            This flag must be specified in conjunction with
            IPC_NOWAIT, with the result that, if there is no message
            available at the given position, the call fails
            immediately with the error ENOMSG.  Because they alter the
            meaning of msgtyp in orthogonal ways, MSG_COPY and
            MSG_EXCEPT may not both be specified in msgflg.

            The MSG_COPY flag was added for the implementation of the
            kernel checkpoint-restore facility and is available only
            if the kernel was built with the CONFIG_CHECKPOINT_RESTORE
            option.

    MSG_EXCEPT
            Used with msgtyp greater than 0 to read the first message
            in the queue with message type that differs from msgtyp.

    MSG_NOERROR
            To truncate the message text if longer than msgsz bytes.

    If no message of the requested type is available and IPC_NOWAIT
    isn't specified in msgflg, the calling process is blocked until
    one of the following conditions occurs:

    • A message of the desired type is placed in the queue.

    • The message queue is removed from the system.  In this case,
        the system call fails with errno set to EIDRM.

    • The calling process catches a signal.  In this case, the system
        call fails with errno set to EINTR.  (msgrcv() is never
        automatically restarted after being interrupted by a signal
        handler, regardless of the setting of the SA_RESTART flag when
        establishing a signal handler.)

    Upon successful completion the message queue data structure is
    updated as follows:

            msg_lrpid is set to the process ID of the calling process.

            msg_qnum is decremented by 1.

            msg_rtime is set to the current time.

*/
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
/*
    msgctl() performs the control operation specified by cmd on the
    System V message queue with identifier msqid.

    The msqid_ds data structure is defined in <sys/msg.h> as follows:

    struct msqid_ds {
        struct ipc_perm msg_perm;   // Ownership and permissions
        time_t          msg_stime;  // Time of last msgsnd(2)
        time_t          msg_rtime;  // Time of last msgrcv(2)
        time_t          msg_ctime;  // Time of creation or last
                                        modification by msgctl()
        unsigned long   msg_cbytes; // # of bytes in queue
        msgqnum_t       msg_qnum;   // # number of messages in queue
        msglen_t        msg_qbytes; // Maximum # of bytes in queue
        pid_t           msg_lspid;  // PID of last msgsnd(2)
        pid_t           msg_lrpid;  // PID of last msgrcv(2) 
    };

The fields of the msgid_ds structure are as follows:

msg_perm
        This is an ipc_perm structure (see below) that specifies
        the access permissions on the message queue.

msg_stime
        Time of the last msgsnd(2) system call.

msg_rtime
        Time of the last msgrcv(2) system call.

msg_ctime
        Time of creation of queue or time of last msgctl() IPC_SET
        operation.

msg_cbytes
        Number of bytes in all messages currently on the message
        queue.  This is a nonstandard Linux extension that is not
        specified in POSIX.

msg_qnum
        Number of messages currently on the message queue.

msg_qbytes
        Maximum number of bytes of message text allowed on the
        message queue.

msg_lspid
        ID of the process that performed the last msgsnd(2) system
        call.

msg_lrpid
        ID of the process that performed the last msgrcv(2) system
        call.

The ipc_perm structure is defined as follows (the highlighted
fields are settable using IPC_SET):

    struct ipc_perm {
        key_t          __key;        Key supplied to msgget(2) 
        uid_t          uid;          Effective UID of owner 
        gid_t          gid;          Effective GID of owner 
        uid_t          cuid;         Effective UID of creator 
        gid_t          cgid;         Effective GID of creator 
        unsigned short mode;         Permissions 
        unsigned short __seq;        Sequence number 
    };

The least significant 9 bits of the mode field of the ipc_perm
structure define the access permissions for the message queue.
The permission bits are as follows:

0400   Read by user
0200   Write by user
0040   Read by group
0020   Write by group
0004   Read by others
0002   Write by others

Bits 0100, 0010, and 0001 (the execute bits) are unused by the
system.

Valid values for cmd are:

IPC_STAT
        Copy information from the kernel data structure associated
        with msqid into the msqid_ds structure pointed to by buf.
        The caller must have read permission on the message queue.

IPC_SET
        Write the values of some members of the msqid_ds structure
        pointed to by buf to the kernel data structure associated
        with this message queue, updating also its msg_ctime
        member.

        The following members of the structure are updated:
        msg_qbytes, msg_perm.uid, msg_perm.gid, and (the least
        significant 9 bits of) msg_perm.mode.

        The effective UID of the calling process must match the
        owner (msg_perm.uid) or creator (msg_perm.cuid) of the
        message queue, or the caller must be privileged.
        Appropriate privilege (Linux: the CAP_SYS_RESOURCE
        capability) is required to raise the msg_qbytes value
        beyond the system parameter MSGMNB.

IPC_RMID
        Immediately remove the message queue, awakening all
        waiting reader and writer processes (with an error return
        and errno set to EIDRM).  The calling process must have
        appropriate privileges or its effective user ID must be
        either that of the creator or owner of the message queue.
        The third argument to msgctl() is ignored in this case.

IPC_INFO (Linux-specific)
        Return information about system-wide message queue limits
        and parameters in the structure pointed to by buf.  This
        structure is of type msginfo (thus, a cast is required),
        defined in <sys/msg.h> if the _GNU_SOURCE feature test
        macro is defined:

            struct msginfo {
                int msgpool; // Size in kibibytes of buffer pool
                             //   used to hold message data;
                             //   unused within kernel 
                int msgmap;  // Maximum number of entries in message
                             //   map; unused within kernel 
                int msgmax;  // Maximum number of bytes that can be
                            //  written in a single message 
                int msgmnb;  // Maximum number of bytes that can be
                             //   written to queue; used to initialize
                             //   msg_qbytes during queue creation
                             //   (msgget(2)) 
                int msgmni;  // Maximum number of message queues 
                int msgssz;  // Message segment size;
                             //   unused within kernel 
                int msgtql;  // Maximum number of messages on all queues
                             //   in system; unused within kernel 
                unsigned short msgseg;
                            // Maximum number of segments;
                            //    unused within kernel 
            };

        The msgmni, msgmax, and msgmnb settings can be changed via
        /proc files of the same name; see proc(5) for details.

MSG_INFO (Linux-specific)
        Return a msginfo structure containing the same information
        as for IPC_INFO, except that the following fields are
        returned with information about system resources consumed
        by message queues: the msgpool field returns the number of
        message queues that currently exist on the system; the
        msgmap field returns the total number of messages in all
        queues on the system; and the msgtql field returns the
        total number of bytes in all messages in all queues on the
        system.

MSG_STAT (Linux-specific)
        Return a msqid_ds structure as for IPC_STAT.  However, the
        msqid argument is not a queue identifier, but instead an
        index into the kernel's internal array that maintains
        information about all message queues on the system.

MSG_STAT_ANY (Linux-specific, since Linux 4.17)
        Return a msqid_ds structure as for MSG_STAT.  However,
        msg_perm.mode is not checked for read access for msqid
        meaning that any user can employ this operation (just as
        any user may read /proc/sysvipc/msg to obtain the same
        information).
*/
```