/*
 * test.c
 *
 * Copyright (c) 2012 - 2017 Thomas Buck <xythobuz@xythobuz.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "serial.h"

/** \example test.c
 *  Initializes all available UART Modules.
 *  Then prints a welcome message on each and waits
 *  for incoming characters, which will be repeated
 *  on the UART module they were received on.
 */

static void initCPU(void);

int main(void) {
    // Initialize clock subsystem (when running on an XMega device)
    initCPU();

    // Initialize UART modules
    for (int i = 0; i < serialAvailable(); i++) {
        serialInit(i, BAUD(38400, F_CPU));
    }

    // Enable Interrupts
    sei();

    // Print Welcome Message
    for (int i = 0; i < serialAvailable(); i++) {
        serialWriteString(i, "Hello from UART");
        serialWrite(i, i + '0');
        serialWriteString(i, "... :)\n");
    }

    // Wait for incoming bytes
    for(;;) {
        for (int i = 0; i < serialAvailable(); i++) {
            if (serialHasChar(i)) {
                serialWrite(i, serialGet(i));
            }
        }
    }
    return 0;
}

#if __AVR_ARCH__ >= 100
static void initCPU(void) {
    // Set GPIO directions
    // FTDI FT232RL on PC6 (Rx) and PC7 (Tx) / USARTC1
    PORTC.DIRSET = PIN7_bm; // Tx as Output
    PORTC.OUTSET = PIN7_bm; // Set to logic '1'

    // Status LEDs on PE6 and PE7
    PORTE.DIRSET = PIN6_bm | PIN7_bm; // LEDs as Outputs

    // Enable both LEDs after reset
    PORTE.OUTCLR = PIN6_bm | PIN7_bm;

    // Enable all interrupt levels on XMega devices
    PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

    // Setup system clock source
    // 2MHz * 10 = 20MHz
    //CLKSYS_PLL_Config(OSC_PLLSRC_RC2M_gc, 10);
    _PROTECTED_WRITE(OSC_PLLCTRL, 10);

    // Enable PLL
    //CLKSYS_Enable(OSC_PLLEN_bm);
    _PROTECTED_WRITE(OSC_CTRL, OSC_PLLEN_bm | OSC_RC2MEN_bm | OSC_XOSCEN_bm);

    // No Prescaling: 1 / 1-1
    //CLKSYS_Prescalers_Config(CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc);
    _PROTECTED_WRITE(CLK_PSCTRL, 0);

    // Wait for PLL to be ready...
    //do {} while (CLKSYS_IsReady(OSC_PLLRDY_bm) == 0);
    while (!(OSC.STATUS & OSC_PLLRDY_bm));

    // Set PLL as main clock-source
    //CLKSYS_Main_ClockSource_Select(CLK_SCLKSEL_PLL_gc);
    _PROTECTED_WRITE(CLK_CTRL, 0x04);

    // Disable external clock source
    //CLKSYS_Disable(OSC_XOSCEN_bm);
    _PROTECTED_WRITE(OSC_CTRL, OSC_PLLEN_bm | OSC_RC2MEN_bm);

    // Disable LEDs after clock init
    PORTE.OUTSET = PIN6_bm | PIN7_bm;
}
#else
static void initClock(void) { }
#endif

