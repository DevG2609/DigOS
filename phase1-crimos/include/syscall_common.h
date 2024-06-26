/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Common system call identifiers
 */
#ifndef SYSCALL_COMMON_H
#define SYSCALL_COMMON_H

#define PROC_IO_IN      0       // IO Input Id
#define PROC_IO_OUT     1       // IO Output Id

// Syscall identifiers
typedef enum {
    SYSCALL_NONE,
    SYSCALL_IO_READ,
    SYSCALL_IO_WRITE,
    SYSCALL_IO_FLUSH,
    SYSCALL_SYS_GET_TIME,
    SYSCALL_SYS_GET_NAME,
    SYSCALL_PROC_SLEEP,
    SYSCALL_PROC_EXIT,
    SYSCALL_PROC_GET_PID,
    SYSCALL_PROC_GET_NAME
} syscall_t;

#endif

