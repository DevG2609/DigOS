/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Context Switch Functions
 */
#include <spede/machine/asmacros.h>
#include "interrupts.h"

// Keyboard ISR Entry
ENTRY(isr_entry_keyboard)
    // Save register state
    pusha

    // Pass the irq number to the irq handler via the stack
    pushl $IRQ_KEYBOARD

    // Call the irq handler function
    call CNAME(interrupts_irq_handler)

    // Adjust the stack pointer before restoring the register
    // state since we pushed the irq number to the stack
    // when calling the IRQ handler
    add $4, %esp

    // Restore register state
    popa
    iret

// Timer ISR Entry
ENTRY(isr_entry_timer)
    // Save register state
    pusha

    // Pass the irq number to the irq handler via the stack
    pushl $IRQ_TIMER

    // Call the irq handler function
    call CNAME(interrupts_irq_handler)

    // Adjust the stack pointer before restoring the register
    // state since we pushed the irq number to the stack
    // when calling the IRQ handler
    add $4, %esp

    // Restore register state
    popa
    iret

