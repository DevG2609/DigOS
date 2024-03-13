/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * TTY Definitions
 */

#include <spede/string.h>

#include "kernel.h"
#include "timer.h"
#include "tty.h"
#include "vga.h"

// local variable
#define TTY_REFRESH_INTERVAL 10000 // Refresh interval in milliseconds
#define TAB_WIDTH 4 // Tab width in spaces

// TTY Table
struct tty_t tty_table[TTY_MAX];

// Current Active TTY
struct tty_t *active_tty;

// Flag to track whether echo is enabled
int echo_enabled = 1; // 1 means echo is enabled by default


/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int n) {
    // Set the active tty to point to the entry in the tty table
    // if a new tty is selected, the tty should trigger a refresh
    if (n >= 0 && n < TTY_BUF_SIZE) {
        active_tty = &tty_table[n];
        active_tty->refresh = 1;
    } else {
        kernel_log_error("tty: Invalid TTY number %d", n);
    }
}

/**
 * Refreshes the tty if needed
 */
void tty_refresh(void) {
    if (!active_tty) {
        kernel_panic("No TTY is selected!");
        return;
    }

    // If the TTY needs to be refreshed, copy the tty buffer
    // to the VGA output.
    // ** hint: use vga_putc_at() since you are setting specific characters
    //          at specific locations
    // Reset the tty's refresh flag so we don't refresh unnecessarily
    if (active_tty->refresh) {
        for (int i = 0; i < TTY_HEIGHT; i++) {
            for (int j = 0; j < TTY_WIDTH; j++) {
                char c = active_tty->buf[i * TTY_WIDTH + j];
                vga_putc_at(j, i, active_tty->color_bg, active_tty->color_fg, c);
            }
        }
        active_tty->refresh = 0;
    }
}


/**
 * Updates the active TTY with the given character
 */
void tty_update(char c) {
    if (!active_tty) {
        return;
    }

    // Since this is a virtual wrapper around the VGA display, treat each
    // input character as you would for the VGA output
    //   Adjust the x/y positions as necessary
    //   Handle scrolling at the bottom

    // Instead of writing to the VGA display directly, you will write
    // to the tty buffer.
    //
    // If the screen should be updated, the refresh flag should be set
    // to trigger the the VGA update via the tty_refresh callback
    // Handle control characters
    switch (c) {
        case '\n': // New line
            active_tty->pos_x = 0; // Reset x position to start of line
            active_tty->pos_y++;   // Move to the next line
            break;
        case '\r': // Carriage return
            active_tty->pos_x = 0; // Reset x position to start of line
            break;
        case '\b': // Backspace character
            if (active_tty->pos_x > 0) {
                active_tty->pos_x--; // Move back one position
                active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x] = ' ';
            }
            break;
        case '\t': // Tab character
            active_tty->pos_x = (active_tty->pos_x + TAB_WIDTH) % TTY_WIDTH;
            break;
        default: // Regular character
            active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x] = c;
            active_tty->pos_x++; // Move to the next position
            break;
    }

    // Handle scrolling
    if (!active_tty->pos_scroll) {
        if (active_tty->pos_x >= TTY_WIDTH) {
            active_tty->pos_x = 0; // Move to the start of the next line
            active_tty->pos_y++;   // Move to the next line
        }
        if (active_tty->pos_y >= TTY_HEIGHT) {
            active_tty->pos_y = 0; // Wrap to the top row
        }
    } else {
        if (active_tty->pos_x >= TTY_WIDTH) {
            active_tty->pos_x = 0; // Move to the start of the next line
            active_tty->pos_y++;   // Move to the next line
            tty_scroll_up(); // Scroll up the contents of the TTY buffer
        }
        if (active_tty->pos_y >= TTY_HEIGHT) {
            active_tty->pos_y = TTY_HEIGHT - 1; // Move to the last row
        }
    }

    // Update the refresh flag
    active_tty->refresh = 1;

    // Echo the character if echo is enabled
    if (echo_enabled) {
        // Output the character to the TTY buffer
        active_tty->pos_x--; // Move back to the previous position
        active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x] = c;
        active_tty->pos_x++; // Move back to the next position
    }
}

void tty_scroll_up(void) {
    // Copy each row of the TTY buffer to the row above it
    for (int y = 1; y < TTY_HEIGHT; y++) {
        for (int x = 0; x < TTY_WIDTH; x++) {
            active_tty->buf[(y - 1) * TTY_WIDTH + x] = active_tty->buf[y * TTY_WIDTH + x];
        }
    }

    // Clear the last row of the TTY buffer
    for (int x = 0; x < TTY_WIDTH; x++) {
        active_tty->buf[(TTY_HEIGHT - 1) * TTY_WIDTH + x] = ' ';
    }
}

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */
void tty_init(void) {
    kernel_log_info("tty: Initializing TTY driver");

    // Initialize the tty_table
    for (int i = 0; i < TTY_BUF_SIZE; ++i) {
        tty_table[i].id = i;
        tty_table[i].refresh = 0;
    }

    // Select tty 0 to start with
    active_tty = &tty_table[0];


    // Register a timer callback to update the screen on a regular interval
    timer_callback_register(tty_refresh, TTY_REFRESH_INTERVAL, -1);
}

