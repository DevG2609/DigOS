/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard Functions
 */
#include "io.h"
#include "kernel.h"
#include "keyboard.h"


// Global variables to track modifier keys state
static int shift_pressed = 0;
static int caps_lock_on = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

char ascii_values[] = {                                                 
      '\0', '\001', '\002', '\003', '\004', '\005', '\006', '\007',                         
     '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',                       
      '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',                    
     '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',                        
       ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',      
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',              
       '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',            
      'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',                 
       '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',              
      'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '\177'                                        
  }; 

  // ascii definations of the special keys
#define ASCII_LEFT_SHIFT    0x2A
#define ASCII_RIGHT_SHIFT   0x36
#define ASCII_CTRL          0x1D
#define ASCII_ALT           0x38
#define ASCII_CAPS_LOCK     0x3A

/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard driver");
    // Enable keyboard by sending the command to the appropriate port (0x64)
    outportb(0x64, 0xAE);

    // Wait until input buffer is empty
    while ((inportb(0x64) & 0x01) != 0);

    // Send the command to set scan code set to 1
    outportb(0x64, 0xF0); // Command byte to set scan code set
    while ((inportb(0x64) & 0x01) != 0); // Wait until input buffer is empty

    // Send the value to set scan code set to 1
    outportb(0x60, 0x01); // Value to set scan code set to 1

    // Wait until acknowledgement is received
    unsigned char ack;
    do {
        ack = inportb(0x60);
    } while (ack != 0xFA); // Wait until receiving acknowledgement (0xFA)

    kernel_log_info("Keyboard driver initialization completed");
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {    
    unsigned int c = KEY_NULL;        
    c = inportb(KBD_PORT_DATA);  
    if (c == KEY_NULL) {
        kernel_log_warn("Failed to read from keyboard");
    }
    kernel_log_info("keyboard_scan() returns: %10x\n", c);                  
    return c;                        
 }    

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {   
   unsigned int c = KEY_NULL;        
    if((c = keyboard_scan()) != KEY_NULL)                                           
   {                                
        c = keyboard_decode(c);                                   
    }                                                             
    return c;                                                     
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *             for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}

/**
 * Processes raw keyboard input and decodes it.
 *
 * Should keep track of the keyboard status for the following keys:
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 *
 * If *all* of the status keys defined in KEY_KERNEL_DEBUG are pressed,
 * while another character is entered, the kernel_debug_command()
 * function should be called.
 */
unsigned int keyboard_decode(unsigned int c) {
     // Check if key is being pressed or released
    int is_pressed = (c & 0x80) == 0;

    // Extract the key code (bits 0-6)
    int key_code = c & 0x7F;

    // Check if the key is a modifier key (SHIFT, CTRL, ALT)
    if (key_code == ASCII_LEFT_SHIFT || key_code == ASCII_RIGHT_SHIFT) {
        shift_pressed = is_pressed;
        return KEY_NULL; // Modifier keys don't produce characters
    } else if (key_code == ASCII_CAPS_LOCK) {
        if (is_pressed) {
            caps_lock_on = !caps_lock_on; 
        }
        return KEY_NULL;
    } else if (key_code == ASCII_CTRL) {
        ctrl_pressed = is_pressed;
        return KEY_NULL; 
    } else if (key_code == ASCII_ALT) {
        alt_pressed = is_pressed;
        return KEY_NULL; 
    }

    // Handle alphanumeric characters and symbols
    if (key_code < 0x80) { 
        // Apply SHIFT and CAPS LOCK if necessary
        int is_uppercase = (shift_pressed && !caps_lock_on) || (!shift_pressed && caps_lock_on);

        // Determine ASCII code based on key code and modifier keys
        unsigned int ascii_code = ascii_values[key_code];

        // If the key represents an alphabetic character, adjust the case based on modifiers
        if (ascii_code >= 'a' && ascii_code <= 'z') {
            if (is_uppercase) {
                // Convert to uppercase
                ascii_code -= 'a' - 'A';
            } else {
                // Convert to lowercase
                ascii_code += 'a' - 'A';
            }
        }
        
        if (is_pressed) {
            return ascii_code; 
        }

        return KEY_NULL; 
    }

    return KEY_NULL; // Default case: return KEY_NULL for unmapped keys
}
