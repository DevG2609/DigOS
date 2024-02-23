#include <spede/stdarg.h>
#include <spede/stdio.h>

#include "bit.h"
#include "io.h"
#include "kernel.h"
#include "vga.h"

/**
 * Forward Declarations
 */
void vga_cursor_update(void);

/**
 * Global variables in this file scope
 */
static bool cursor_enabled = false;
static int current_row = 0;
static int current_col = 0;
static int bg_color = VGA_COLOR_BLACK;
static int fg_color = VGA_COLOR_LIGHT_GREY;

/**
* to navigate the cursor a value of 4 spaces when the tab is pressed
*/
#define TAB_STOP 4

/**
 * Initializes the VGA driver and configuration
 *  - Defaults variables
 *  - Clears the screen
 */
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");

    // Clear the screen and initiates the vga driver
    vga_clear();
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    // Clear all character data, set the foreground and background colors
    // Set the cursor position to the top-left corner (0, 0)
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; ++i) {
        VGA_BASE[i] = VGA_CHAR(bg_color, fg_color, ' ');
    }
    current_row = 0;
    current_col = 0;
    vga_cursor_update();
}

/**
 * Clears the background color for all characters and sets to the
 * specified value
 *
 * @param bg background color value
 */
void vga_clear_bg(int bg) {
    // Iterate through all VGA memory and set only the background color bits
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; ++i) {
        VGA_BASE[i] = (VGA_BASE[i] & 0x0F) | (bg << 4);
    }
}

/**
 * Clears the foreground color for all characters and sets to the
 * specified value
 *
 * @param fg foreground color value
 */
void vga_clear_fg(int fg) {
    // Iterate through all VGA memory and set only the foreground color bits.
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; ++i) {
        VGA_BASE[i] = (VGA_BASE[i] & 0xF0) | fg;
    }
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // The cursor will be displayed as a series of horizontal lines that create a
    // "block" or rectangular symbol. The position/size is determined by the "starting"
    // scanline and the "ending" scanline. Scanlines range from 0x0 to 0xF.

    // In both the cursor start register and cursor end register, the scanline values
    // are specified in bits 0-4

    // To ensure that we do not change bits we are not intending,
    // read the current register state and mask off the bits we
    // want to save

    // Set the cursor starting scanline (register 0x0A, cursor start register)

    // Set the cursor ending scanline (register 0x0B, cursor end register)
    // Ensure that bit 5 is not set so the cursor will be enabled

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled

    // Update the cursor location once it is enabled

    //Phase 1 code @DevG
    // Set cursor start and end registers to enable cursor
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, (inportb(0x3D5) & 0xC0) | 0);
    cursor_enabled = true;
    vga_cursor_update();
    
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // Since we are disabling the cursor, we can simply set the bit of interest
    // as we won't care what the current cursor scanline start/stop values are

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled

    // Phase 1 code @DevG
    // Set cursor start and end registers to disable cursor
    outportb(VGA_PORT_ADDR, 0x0A);
    outportb(VGA_PORT_DATA, 0x20);
    cursor_enabled = false;
}

/**
 * Indicates if the VGA text mode cursor is enabled or disabled
 */
bool vga_cursor_enabled(void) {
    return cursor_enabled;
}

/**
 * Sets the vga text mode cursor position to the current VGA row/column
 * position if the cursor is enabled
 */
void vga_cursor_update(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor position is represented as an unsigned short (2-bytes). As
    // VGA register values are single-byte, the position representation is
    // split between two registers:
    //   0x0F Cursor Location High Register
    //   0x0E Cursor Location Low Register

    // The Cursor Location High Register is the least significant byte
    // The Cursor Location Low Register is the most significant byte

    // If the cursor is enabled:
        // Calculate the cursor position as an offset into
        // memory (unsigned short value)

        // Set the VGA Cursor Location High Register (0x0F)
        //   Should be the least significant byte (0x??<00>)

        // Set the VGA Cursor Location Low Register (0x0E)
        //   Should be the most significant byte (0x<00>??)
    if (cursor_enabled) {
        unsigned short pos = current_row * VGA_WIDTH + current_col;
        outportb(VGA_PORT_ADDR, 0x0F);
        outportb(VGA_PORT_DATA, (unsigned char)(pos & 0xFF));
        outportb(VGA_PORT_ADDR, 0x0E);
        outportb(VGA_PORT_DATA, (unsigned char)((pos >> 8) & 0xFF));
    }    
}

/**
 * Sets the current row/column position
 *
 * @param row position (0 to VGA_HEIGHT-1)
 * @param col position (0 to VGA_WIDTH-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_rowcol(int row, int col) {
    // Update the text mode cursor (if enabled)
    current_row = (row >= 0 && row < VGA_HEIGHT) ? row : (row < 0 ? 0 : VGA_HEIGHT - 1);
    current_col = (col >= 0 && col < VGA_WIDTH) ? col : (col < 0 ? 0 : VGA_WIDTH - 1);
    vga_cursor_update();
}

/**
 * Gets the current row position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_row(void) {
    return current_row;
}

/**
 * Gets the current column position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_col(void) {
    return current_col;
}

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg) {
    bg_color = bg;
}

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void) {
    return bg_color;
}

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg) {
    fg_color = fg;
}

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void) {
    return fg_color;
}

/**
 * Prints a character on the screen without modifying the cursor or other attributes
 *
 * @param c - Character to print
 */
void vga_setc(unsigned char c) {
    unsigned short *vga_buf = VGA_BASE;
    vga_buf[current_row * VGA_WIDTH + current_col] = VGA_CHAR(bg_color, fg_color, c);
    current_col++;
    if (current_col >= VGA_WIDTH) {
        current_col = 0;
        current_row++;
        if (current_row >= VGA_HEIGHT) {
            current_row = 0;
        }
    }
    vga_cursor_update();
}

/**
 * Prints a character on the screen at the current cursor (row/column) position
 *
 * When a character is printed, will do the following:
 *  - Update the row and column positions
 *  - If needed, will wrap from the end of the current line to the
 *    start of the next line
 *  - If the last line is reached, the cursor position will reset to the top-left (0, 0) position
 *  - Special characters are handled as such:
 *    - tab character (\t) prints 'tab_stop' spaces
 *    - backspace (\b) character moves the character back one position,
 *      prints a space, and then moves back one position again
 *    - new-line (\n) should move the cursor to the beginning of the next row
 *    - carriage return (\r) should move the cursor to the beginning of the current row
 *
 * @param c - character to print
 */
void vga_putc(unsigned char c) {
    unsigned short *vga_buf = VGA_BASE;
    // Handle scecial characters
    // Handle end of lines
    // Wrap-around to the top/left corner
    // Update the text mode cursor, if enabled
    switch (c) {
        case '\n':
            current_row++;
            current_col = 0;
            break;
        case '\r':
            current_col = 0;
            break;
        case '\t':
            // Tab character - Move to the next tab stop
            current_col = (current_col + TAB_STOP) & ~(TAB_STOP - 1);
            break;
        case '\b':
            // Backspace character
            if (current_col > 0) {
                current_col--;
                vga_buf[current_row * VGA_WIDTH + current_col] = VGA_CHAR(bg_color, fg_color, ' ');
            }
            break;
        default:
            // Print any other character
            vga_buf[current_row * VGA_WIDTH + current_col] = VGA_CHAR(bg_color, fg_color, c);
            current_col++;
            break;
    }

    // Handle wrapping
    if (current_col >= VGA_WIDTH) {
        current_col = 0;
        current_row++;
    }
    if (current_row >= VGA_HEIGHT) {
        current_row = 0;
    }
    vga_cursor_update();
}

/**
 * Prints a string on the screen at the current cursor (row/column) position
 *
 * @param s - string to print
 */
void vga_puts(char *str) {
    while (*str != '\0') {
        vga_putc(*str);
        str++;
    }
}

/**
 * Prints a character on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param c character to print
 */
void vga_putc_at(int row, int col, int bg, int fg, unsigned char c) {
    unsigned short *vga_buf = VGA_BASE;
    vga_buf[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, c);
}

/**
 * Prints a string on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param s string to print
 */
void vga_puts_at(int row, int col, int bg, int fg, char *s) {
    unsigned short *vga_buf = VGA_BASE;
    int i = 0;
    while (s[i] != '\0') {
        vga_buf[row * VGA_WIDTH + col + i] = VGA_CHAR(bg, fg, s[i]);
        i++;
    }

}

/**
 * Scrolls the VGA text buffer up by one line
 */
void vga_scroll(void) {
    unsigned short *vga_buf = VGA_BASE;
    for (int i = 0; i < VGA_HEIGHT - 1; i++) {
        for (int j = 0; j < VGA_WIDTH; j++) {
            vga_buf[i * VGA_WIDTH + j] = vga_buf[(i + 1) * VGA_WIDTH + j];
        }
    }
    for (int j = 0; j < VGA_WIDTH; j++) {
        vga_buf[(VGA_HEIGHT - 1) * VGA_WIDTH + j] = VGA_CHAR(bg_color, fg_color, ' ');
    }
    current_row--;
    if (current_row < 0) {
        current_row = 0;
    }
    vga_cursor_update();
}

