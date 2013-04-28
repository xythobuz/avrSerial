/*
 * test.c
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
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

int main(void) {
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