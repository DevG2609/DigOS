/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Test functions
 */
#ifndef TEST_H
#define TEST_H

#include "timer.h"
#include "kernel.h"
#include "vga.h"

/**
 * Displays a "spinner" to show activity at the top-right corner of the
 * VGA output
 */
void test_spinner(void) {
    static char spin[] = { '|', '/', '-', '\\' };
    static int count = 0;

    vga_putc_at(VGA_WIDTH-1, 0, VGA_COLOR_BLACK, VGA_COLOR_GREEN,
                spin[count++ % sizeof(spin)]);
}

/**
 * Displays the number of seconds that have passed since startup
 * Obtains the number of timer ticks and converts to seconds
 */
void test_timer(void) {
    vga_set_xy(73, 0);
    vga_printf("%5d", timer_get_ticks() / 100);
}

/**
 * Initializes all tests
 */
void test_init(void) {
    kernel_log_info("Initializing test functions");

    // Register the spinner to update at a rate of 10 times per second
    timer_callback_register(&test_spinner, 10, -1);

    // Register the timer to update at a rate of 4 times per second
    timer_callback_register(&test_timer, 24, -1);
}
#endif
