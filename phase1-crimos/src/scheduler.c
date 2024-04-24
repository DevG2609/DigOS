/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */

#include <spede/string.h>
#include <spede/stdio.h>
#include <spede/time.h>
#include <spede/machine/proc_reg.h>

#include "kernel.h"
#include "kproc.h"
#include "scheduler.h"
#include "timer.h"

#include "queue.h"

// Process Queues
queue_t run_queue;      // Run queue -> processes that will be scheduled to run
queue_t sleep_queue; // Sleep queue -> processes that are sleeping

/**
 * Scheduler timer callback
 */
void scheduler_timer(void) {
    // Update the active process' run time and CPU time
    if (active_proc) {
        active_proc->run_time++;
        active_proc->cpu_time++;
    }
}

/**
 * Executes the scheduler
 * Should ensure that `active_proc` is set to a valid process entry
 */
void scheduler_run(void) {
    int pid;

    // Ensure that processes not in the active state aren't still scheduled
    if (active_proc && active_proc->state != ACTIVE) {
        active_proc = NULL;
    }

    // Check if we have an active process
    if (active_proc) {
        // Check if the current process has exceeded its time slice
        if (active_proc->cpu_time >= SCHEDULER_TIMESLICE) {
            // Reset the active time
            active_proc->cpu_time = 0;

            // If the process is not the idle task, add it back to the scheduler
            // Otherwise, simply set the state to IDLE
            
            if (active_proc->pid != 0) {
                // Add the process to the scheduler
                scheduler_add(active_proc);
            } else {
                active_proc->state = IDLE;
            }

            // Unschedule the current process
            kernel_log_trace("Unscheduling process pid=%d, name=%s", active_proc->pid, active_proc->name);
            active_proc = NULL;
        }
    }

    // Check if we have a process scheduled or not
    if (!active_proc) {
        // Check if there are any processes in the sleep queue that need to wake up
        if (!queue_is_empty(&sleep_queue)) {
            int current_time = timer_get_ticks();

            // Loop through the sleep queue to wake up processes if necessary
            for (int i = 0; i < sleep_queue.size; i++) {
                int sleep_pid;
                if (queue_out(&sleep_queue, &sleep_pid) != 0) {
                    kernel_panic("Unable to dequeue process from sleep queue");
                }
                proc_t *sleep_proc = pid_to_proc(sleep_pid);
                if (!sleep_proc) {
                    kernel_panic("Invalid process in sleep queue");
                }
                // Check if the process should wake up
                if (current_time >= sleep_proc->sleep_time) {
                    // Add the process back to the scheduler
                    scheduler_add(sleep_proc);
                    kernel_log_trace("Process pid=%d woke up from sleep", sleep_proc->pid);
                } else {
                    // Process should remain asleep, add it back to the sleep queue
                    queue_in(&sleep_queue, sleep_proc->pid);
                }
            }
        }

        // Get the process id from the run queue
        if (queue_out(&run_queue, &pid) != 0) {
            // default to process id 0 (idle task)
            pid = 0;
        }

        active_proc = pid_to_proc(pid);
        kernel_log_trace("Scheduling process pid=%d, name=%s", active_proc->pid, active_proc->name);
    }

    // Make sure we have a valid process at this point
    if (!active_proc) {
        kernel_panic("Unable to schedule a process!");
    }

    // Ensure that the process state is correct
    active_proc->state = ACTIVE;
}

/**
 * Adds a process to the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_add(proc_t *proc) {
    if (!proc) {
        kernel_panic("Invalid process!");
    }

    proc->scheduler_queue = &run_queue;
    proc->state = IDLE;
    proc->cpu_time = 0;

    if (queue_in(proc->scheduler_queue, proc->pid) != 0) {
        kernel_panic("Unable to add the process to the scheduler");
    }
}

/**
 * Removes a process from the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_remove(proc_t *proc) {
    int pid;

    if (!proc) {
        kernel_panic("Invalid process!");
        exit(1);
    }

    if (proc->scheduler_queue) {
        for (int i = 0; i < proc->scheduler_queue->size; i++) {
            if (queue_out(proc->scheduler_queue, &pid) != 0) {
                kernel_panic("Unable to queue out the process entry");
            }

            if (proc->pid == pid) {
                // Found the process
                // continue iterating so the run queue order is maintained
                continue;
            }

            // Add the item back to the run queue
            if (queue_in(proc->scheduler_queue, pid) != 0) {
                kernel_panic("Unable to queue process back to the run queue");
            }
        }

        // Set the queue to NULL since it does not exist in a queue any longer
        proc->scheduler_queue = NULL;
    }

    // If the process is the current process, ensure that the current
    // process is reset so a new process will be scheduled
    if (proc == active_proc) {
        active_proc = NULL;
    }
}

/**
 * Puts a process to sleep
 * @param proc - pointer to the process entry
 * @param time - number of ticks to sleep
 */
void scheduler_sleep(proc_t *proc, int time) {
     // Set the sleep time
    proc->sleep_time = timer_get_ticks() + time;
    // Set the process state to SLEEPING
    proc->state = SLEEPING;
    // Remove the process from the scheduler
    scheduler_remove(proc);
    // Add the process to the sleep queue
    queue_in(&sleep_queue, proc->pid);
}

/**
 * Initializes the scheduler, data structures, etc.
 */
void scheduler_init(void) {
    kernel_log_info("Initializing scheduler");

    /* Initialize the run queue */
    queue_init(&run_queue);

    /* Initialize the sleep queue */
    queue_init(&sleep_queue);

    /* Register the timer callback */
    timer_callback_register(&scheduler_timer, 1, -1);
}

