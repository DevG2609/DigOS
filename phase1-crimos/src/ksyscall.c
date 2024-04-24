/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel System Call Handlers
 */
#include <spede/time.h>
#include <spede/string.h>
#include <spede/stdio.h>

#include "kernel.h"
#include "kproc.h"
#include "ksyscall.h"
#include "interrupts.h"
#include "scheduler.h"
#include "timer.h"

/**
 * System call IRQ handler
 * Dispatches system calls to the function associate with the specified system call
 */
void ksyscall_irq_handler(void) {
    // Default return value
    int rc = -1;

    if (!active_proc) {
        kernel_panic("Invalid process");
    }

    if (!active_proc->trapframe) {
        kernel_panic("Invalid trapframe");
    }

    // System call identifier is stored on the EAX register
    // Additional arguments should be stored on additional registers (EBX, ECX, etc.)

    // Based upon the system call identifier, call the respective system call handler

    // Ensure that the EAX register for the active process contains the return value

     if (active_proc->trapframe->eax == SYSCALL_SYS_GET_TIME) {
        rc = ksyscall_sys_get_time();
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_SYS_GET_NAME) {
        // Cast the argument as a char pointer
        rc = ksyscall_sys_get_name((char *)active_proc->trapframe->ebx);
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_PROC_EXIT) {
        rc = ksyscall_proc_exit();
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_PROC_GET_PID) {
        rc = ksyscall_proc_get_pid();
        active_proc->trapframe->eax = rc;
        return;
    }

    if (active_proc->trapframe->eax == SYSCALL_PROC_GET_NAME) {
        // Cast the argument as a char pointer
        rc = ksyscall_proc_get_name((char *)active_proc->trapframe->ebx);
        active_proc->trapframe->eax = rc;
        return;
    }

    kernel_panic("Invalid system call %d!", active_proc->trapframe->eax);
}

/**
 * System Call Initialization
 */
void ksyscall_init(void) {
    // Register the IDT entry and IRQ handler for the syscall IRQ (IRQ_SYSCALL)
    // Initialize interrupts to register the syscall IRQ handler
    interrupts_init();

    // Register the syscall IRQ handler
    interrupts_irq_register(IRQ_SYSCALL, isr_entry_syscall, ksyscall_irq_handler);
}

/**
 * Writes up to n bytes to the process' specified IO buffer
 * @param io - the IO buffer to write to
 * @param buf - the buffer to copy from
 * @param n - number of bytes to write
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_write(int io, char *buf, int size) {

    // Ensure there is an active process
    if (!active_proc) {
        return -1;
    }

    // Ensure the IO buffer is within range (PROC_IO_MAX)
    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    // Ensure that the active process has a valid IO buffer
    if (!active_proc->io[io]) {
        return -1;
    }

    // Using ringbuf_write_mem - Write size bytes from buf to active_proc->io[io]
    return ringbuf_write_mem(active_proc->io[io], buf, size);
}

/**
 * Reads up to n bytes from the process' specified IO buffer
 * @param io - the IO buffer to read from
 * @param buf - the buffer to copy to
 * @param n - number of bytes to read
 * @return -1 on error or value indicating number of bytes copied
 */
int ksyscall_io_read(int io, char *buf, int size) {

    // Ensure there is an active process
    if (!active_proc) {
        return -1;
    }

    // Ensure the IO buffer is within range (PROC_IO_MAX)
    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    // Ensure that the active process has a valid IO buffer
    if (!active_proc->io[io]) {
        return -1;
    }

    // Using ringbuf_read_mem - Read size bytes from active_proc->io[io] to buf
    return ringbuf_read_mem(active_proc->io[io], buf, size);
}

/**
 * Flushes (clears) the specified IO buffer
 * @param io - the IO buffer to flush
 * @return -1 on error or 0 on success
 */
int ksyscall_io_flush(int io) {

    // Ensure there is an active process
    if (!active_proc) {
        return -1;
    }

    // Ensure the IO buffer is within range (PROC_IO_MAX)
    if (io < 0 || io >= PROC_IO_MAX) {
        return -1;
    }

    // Ensure that the active process has a valid IO buffer
    if (!active_proc->io[io]) {
        return -1;
    }

    // Use ringbuf_flush to flush the IO buffer
    ringbuf_flush(active_proc->io[io]);
    return 0;
}

/**
 * Gets the current system time (in seconds)
 * @return system time in seconds
 */
int ksyscall_sys_get_time(void) {
    return timer_get_ticks() / 100;
}

/**
 * Gets the operating system name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_sys_get_name(char *name) {
    if (!name) {
        return -1;
    }

    strncpy(name, OS_NAME, sizeof(OS_NAME));
    return 0;
}

/**
 * Puts the active process to sleep for the specified number of seconds
 * @param seconds - number of seconds the process should sleep
 */
int ksyscall_proc_sleep(int seconds) {
     if (!active_proc) {
        return -1;
    }

    // Put the active process to sleep for the specified number of seconds
    scheduler_sleep(active_proc, seconds * 100); // Convert seconds to ticks

    return 0;
}

/**
 * Exits the current process
 */
int ksyscall_proc_exit(void) {
    if (!active_proc) {
        return -1; // No active process
    }

    // Set the process state to NONE to indicate it has exited
    active_proc->state = NONE;

    // Reschedule by running the scheduler
    scheduler_run();

    return 0;
}

/**
 * Gets the active process pid
 * @return process id or -1 on error
 */
int ksyscall_proc_get_pid(void) {
    if (!active_proc) {
        return -1; // No active process
    }

    return active_proc->pid;
}

/**
 * Gets the active process' name
 * @param name - pointer to a character buffer where the name will be copied
 * @return 0 on success, -1 or other non-zero value on error
 */
int ksyscall_proc_get_name(char *name) {
     if (!active_proc || !name) {
        return -1; // No active process or invalid name pointer
    }

    // Copy the process name to the provided buffer
    strncpy(name, active_proc->name, PROC_NAME_LEN);

    return 0;
}
