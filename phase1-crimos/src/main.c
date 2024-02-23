/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Operating system entry point
 */

#include "vga.h"
#include "keyboard.h"
#include "bit.h"

void main(void) {
    // Initialize the VGA driver
    vga_init();

    // Initialize the keyboard driver
    keyboard_init();

    vga_printf("Welcome to %s!\n", OS_NAME);

    // Exercise the bit_* functions
    int value = 0xdecafbad;
    int count = bit_count(value);
    int test = bit_test(value, count);

    int toggle1 = 31;
    int toggle2 = 0;

    int value1 = bit_toggle(value, toggle1);
    int value2 = bit_toggle(value, toggle2);

    vga_printf("0x%08x has %d bits set\n", value, count);
    vga_printf("bit %d of 0x%08x is %d\n", count, value, test);

    vga_printf("0x%08x -> toggling bit %d = 0x%08x\n", value, toggle1, value1);
    vga_printf("0x%08x -> toggling bit %d = 0x%08x\n", value, toggle2, value2);

    // Wait until a key is pressed
    vga_printf("Press any key to test VGA output\n");
    keyboard_getc();

    // Display every ASCII character
    for (int c = 0; c < 255; c++) {
        vga_putc((unsigned char) c);
    }

    vga_printf("Press any key to test VGA colors\n");
    keyboard_getc();

    // Test every color combination
    for (int x = 0; x <= 0xf; x++) {
        for (int y = 0; y <= 0xf; y++) {
            vga_putc_at(VGA_HEIGHT - y - 2, VGA_WIDTH - x - 2, y, x, '*');
        }
    }

    // Print some helpful text
    vga_puts_at(VGA_HEIGHT-1, 0, VGA_COLOR_LIGHT_BLUE, VGA_COLOR_WHITE,
               "CTRL-K to clear screen, CTRL-ESC x 3 to exit");
    vga_puts_at(VGA_HEIGHT-2, 0, VGA_COLOR_CYAN, VGA_COLOR_WHITE,
               "CTRL-P to test panic, CTRL-B for breakpoint");

    vga_set_bg(VGA_COLOR_BLACK);
    vga_set_fg(VGA_COLOR_LIGHT_GREY);

    vga_printf("\n");
    vga_printf("Press any key to test keyboard input\n");

    keyboard_getc();

    // Loop in place forever
    while (1) {
        char c = keyboard_poll();
        char buf[80] = {0};

        if (c) {
            // Print the character on the screen
            vga_putc(c);

            // Copy the current x/y (cursor) position to a buffer
            snprintf(buf, sizeof(buf), "%02d, %02d", vga_get_row(), vga_get_col());

            // Print the buffer to the screen
            vga_puts_at(VGA_HEIGHT-1, 74, VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE,
                        buf);
        }
    }

    // We should never get here!
}
