/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * User Programs
 */

#include <spede/stdio.h>
#include <spede/string.h>
#include "syscall.h"

#define BUF_SIZE 128

#define pprintf(fmt, ...) { \
    char __pprint_buf[512] = {0}; \
    int i = snprintf(__pprint_buf, sizeof(__pprint_buf), (fmt), ##__VA_ARGS__); \
    if (i > 0) { \
        io_write(PROC_IO_OUT, __pprint_buf, i); \
    } \
}

#define CMD_EXIT "exit"
#define CMD_HELP "help"
#define CMD_SLEEP "sleep"
#define CMD_TIME "time"

void prog_shell(void) {
    char buf[BUF_SIZE];
    char name[32];
    char os_name[128];

    int buflen;
    int reading;

    int pid = proc_get_pid();

    if (proc_get_name(name) != 0) {
        pprintf("error getting process name!");
        proc_exit(-1);
    }

    if (sys_get_name(os_name) != 0) {
        pprintf("error getting system name!");
        proc_exit(-1);
    }

    // Generate a sleep value based upon the process id
    int sleep_seconds = (1 + (pid % 4)) * 4;

    memset(buf, 0, BUF_SIZE);

    // Ensure that the input and output buffers are flushed
    io_flush(PROC_IO_IN);
    io_flush(PROC_IO_OUT);

    pprintf("%s %s (process id %d) is running!\n", os_name, name, pid);
    pprintf("Sleeping for %d seconds at time %d ... ", sleep_seconds, sys_get_time());

    proc_sleep(sleep_seconds);

    pprintf("... and awake at time %d!\n\n", sys_get_time());

    while (1) {
        pprintf("%s[%d]$ ", os_name, pid);
        char input[128] = {0};
        int input_len = 0;

        reading = 1;
        while (reading) {
            buflen = io_read(PROC_IO_IN, buf, BUF_SIZE);

            for (int i = 0; i < buflen; i++) {
                if (buf[i] == '\n' || buf[i] == 0) {
                    io_write(PROC_IO_OUT, &buf[i], 1);
                    reading = 0;
                } else if ( buf[i] != 0) {
                    input[input_len++] = buf[i];
                    io_write(PROC_IO_OUT, &buf[i], 1);
                }
            }
        }

        if (input_len) {
            if (strncmp(input, CMD_HELP, strlen(CMD_HELP)) == 0) {
                pprintf("Enter one of the following commands:\n");
                pprintf("\texit\t  exits the process\n");
                pprintf("\tsleep\t  puts the process to sleep for %d seconds\n", sleep_seconds);
                pprintf("\ttime\t  displays the current system time\n");
                pprintf("\n");
            } else if(strncmp(input, CMD_SLEEP, strlen(CMD_SLEEP)) == 0) {
                pprintf("Sleeping for %d seconds at time %d ... ", sleep_seconds, sys_get_time());
                proc_sleep(sleep_seconds);
                pprintf("... and awake at time %d!\n", sys_get_time());
            } else if (strncmp(input, CMD_TIME, strlen(CMD_TIME)) == 0) {
                pprintf("The current time is %d seconds\n", sys_get_time());
            } else if (strncmp(input, CMD_EXIT, strlen(CMD_EXIT)) == 0) {
                pprintf("Exiting process id %d\n", pid);
                proc_exit(0);
            } else {
                pprintf("You entered the following:\n%s\n", input);
            }
        }
    }
}

