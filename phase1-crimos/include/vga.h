/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * VGA Definitions
 */
#ifndef VGA_H
#define VGA_H

#include <spede/stdio.h>        // For snprintf, used in vga_printf
#include <spede/stdbool.h>      // for true/false values

#define VGA_BASE                ((unsigned short *)(0xB8000))
#define VGA_ATTR(bg, fg)        (((bg & 0xf) << 4) | (fg & 0xf))
#define VGA_CHAR(bg, fg, c)     (((VGA_ATTR((bg & 0xf), (fg & 0xf)) << 8)) | (c))

// VGA Address Port -> Set the register address to write data into
#define VGA_PORT_ADDR 0x3D4
// VGA Data Port -> The data to be written into the register
#define VGA_PORT_DATA 0x3D5

// Width and height of the VGA display
#define VGA_WIDTH               80
#define VGA_HEIGHT              25

// VGA color definitions
#define VGA_COLOR_BLACK         0x0
#define VGA_COLOR_BLUE          0x1
#define VGA_COLOR_GREEN         0x2
#define VGA_COLOR_CYAN          0x3
#define VGA_COLOR_RED           0x4
#define VGA_COLOR_MAGENTA       0x5
#define VGA_COLOR_BROWN         0x6
#define VGA_COLOR_LIGHT_GREY    0x7

// Extended VGA color definitions
#define VGA_COLOR_DARK_GREY     0x8
#define VGA_COLOR_LIGHT_BLUE    0x9
#define VGA_COLOR_LIGHT_GREEN   0xA
#define VGA_COLOR_LIGHT_CYAN    0xB
#define VGA_COLOR_LIGHT_RED     0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW        0xE
#define VGA_COLOR_WHITE         0xF

/**
 * Initializes the VGA driver and configuration
 *  - Clears the screen
 *  - Enables or Disables the cursor to the default state
 */
void vga_init(void);

/**
 * Clears the VGA output and resets the x/y position
 */
void vga_clear(void);

/**
 * Clears the background color for all characters and sets to the
 * specified value
 *
 * @param bg background color value
 */
void vga_clear_bg(int bg);

/**
 * Clears the foreground color for all characters and sets to the
 * specified value
 *
 * @param fg foreground color value
 */
void vga_clear_fg(int fg);

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void);

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void);

/**
 * Indicates if the cursor is enabled or disabled
 */
bool vga_cursor_enabled(void);

/**
 * Moves the "cursor" to the specified row and column
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_rowcol(int row, int col);

/**
 * Gets the current row position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_row(void);

/**
 * Gets the current column position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_col(void);

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg);

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void);

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg);

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void);

/**
 * Prints a character on the screen without modifying the cursor or other attributes
 *
 * @param c - Character to print
 */
void vga_setc(unsigned char c);

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
void vga_putc(unsigned char c);

/**
 * Prints a string on the screen at the current cursor (row/column) position
 *
 * @param s - string to print
 */
void vga_puts(char *s);

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
void vga_putc_at(int x, int y, int bg, int fg, unsigned char c);

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
void vga_puts_at(int x, int y, int bg, int fg, char *s);

/**
 * Prints a formatted string to the VGA output
 *
 * @param fmt the string format
 * @param ... variable argument list for the string format
 * @return length of the string printed
 */
#define vga_printf(fmt, ...) { \
    char _vga_printf_buf[VGA_WIDTH * VGA_HEIGHT] = {0}; \
    snprintf(_vga_printf_buf, sizeof(_vga_printf_buf), (fmt), ##__VA_ARGS__); \
    vga_puts(_vga_printf_buf); \
}

#endif
