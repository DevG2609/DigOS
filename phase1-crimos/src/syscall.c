/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 * 
 *
 * System call APIs
 */
#include "syscall.h"

/**
 * Executes a system call without any arguments
 * @param syscall - the system call identifier
 * @return return code from the the system call
 */
int _syscall0(int syscall) {
    int rc = -1;

    // Note - Data received from the kernel:  
    // If data is to be received from the kernel, such as return code or return value, 
    // it will be exchanged using a register. In our implementation, we will only allow a single 
    // integer value to be returned via the EAX register (if anything returned at all)

    // Note - Data sent to kernel:
    // If data is being sent to the kernel, it will be done using registers.
    // EAX will always be used to send the system call indientifer to the kernel
    // EBX, ECX and EDX can be used for up to 3 additional parameters 

    // Reminder: movl var, %eax     // Moves contents of memory location var into register eax

    asm("movl %1, %%eax;"           // Data copied into registers to be sent to the kernel
        "int $0x80;"                // IRQ_SYSCALL (0x80)
        "movl %%eax, %0;"           // Operands indicating data received from the kernel (only receiving from eax)
        : "=g"(rc)                  // Operands indicating data sent to the kernel
        : "g"(syscall)              // Register used in the operation so the compiler can
        : "%eax");                  // optimize/save/restore

    return rc;
}

/**
 * Executes a system call with one argument
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @return return code from the the system call
 */
int _syscall1(int syscall, int arg1) {
    int rc = -1;

    asm("movl %1, %%eax;"
        "movl %2, %%ebx;"           // Same as _syscall0 but adding an argument
        "int $0x80;"
        "movl %%eax, %0;"           // Only receiving eax from kernel
        : "=g"(rc)                  // What is being sent to the kernel... 
        : "g"(syscall), "g"(arg1)   // Same as _syscall0 but adding an argument
        : "%eax", "%ebx");

    return rc;
}

/**
 * Executes a system call with two arguments
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @param arg2 - second argument
 * @return return code from the the system call
 */
int _syscall2(int syscall, int arg1, int arg2) {
    int rc = -1;

    // Implement me!

     asm("movl %1, %%eax;"
        "movl %2, %%ebx;"
        "movl %3, %%ecx;"         // Move the second argument to ECX
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall), "g"(arg1), "g"(arg2)
        : "%eax", "%ebx", "%ecx");

    return rc;
}

/**
 * Executes a system call with three arguments
 * @param syscall - the system call identifier
 * @param arg1 - first argument
 * @param arg2 - second argument
 * @param arg3 - third argument
 * @return return code from the the system call
 */
int _syscall3(int syscall, int arg1, int arg2, int arg3) {
    int rc = -1;

    // Implement me!

    asm("movl %1, %%eax;"
        "movl %2, %%ebx;"
        "movl %3, %%ecx;"
        "movl %4, %%edx;"         // Move the third argument to EDX
        "int $0x80;"
        "movl %%eax, %0;"
        : "=g"(rc)
        : "g"(syscall), "g"(arg1), "g"(arg2), "g"(arg3)
        : "%eax", "%ebx", "%ecx", "%edx");

    return rc;
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int sys_get_time(void) {
    return _syscall0(SYSCALL_SYS_GET_TIME);
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int sys_get_name(char *name) {
    return _syscall1(SYSCALL_SYS_GET_NAME, (int)name);
}

/**
 * Puts the current process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
void proc_sleep(int secs) {
    _syscall1(SYSCALL_PROC_SLEEP, secs);
}

/**
 * Exits the current process
 * @param exitcode An exit code to return to the parent process
 */
void proc_exit(int exitcode) {

    _syscall1(SYSCALL_PROC_EXIT, exitcode);
}

/**
 * Gets the current process' id
 * @return process id
 */
int proc_get_pid(void) {
    return _syscall0(SYSCALL_PROC_GET_PID);
}

/**
 * Gets the current process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int proc_get_name(char *name) {
     return _syscall1(SYSCALL_PROC_GET_NAME, (int)name);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int io_write(int io, char *buf, int n) {
    return _syscall3(SYSCALL_IO_WRITE, io, (int)buf, n);
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int io_read(int io, char *buf, int n) {
    return _syscall3(SYSCALL_IO_READ, io, (int)buf, n);
}

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int io_flush(int io) {
    return _syscall1(SYSCALL_IO_FLUSH, io);
}
