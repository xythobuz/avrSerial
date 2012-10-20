/*
 * serialBlocking.c
 *
 * Copyright (c) 2012, Thomas Buck <xythobuz@me.com>
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
#include <stdint.h>

#include "serial.h"

void serialInit(uint16_t baud) {
    // Default: 8N1
    SERIALC |= (1 << SERIALUCSZ0);
    SERIALC |= (1 << SERIALUCSZ1);

    // Set baudrate
#ifdef SERIALBAUD8
    SERIALUBRRH = (baud >> 8);
    SERIALUBRRL = baud;
#else
    SERIALUBRR = baud;
#endif

    SERIALB |= (1 << SERIALRXEN) | (1 << SERIALTXEN); // Enable Receiver/Transmitter
}

void serialClose(void) {
    if (!serialHasChar())
        while (!(SERIALA & (1 << UDRE))); // Wait for transmissions to complete

    SERIALB = 0; // Disable USART
    SERIALC = 0; // Reset config
#ifdef SERIALBAUD8
    SERIALUBRRH = 0;
    SERIALUBRRL = 0;
#else
    SERIALUBRR = 0;
#endif
}

void setFlow(uint8_t on) {
    if (on) {
        serialWrite(0x11);
    } else {
        serialWrite(0x13);
    }
}

// ---------------------
// |     Reception     |
// ---------------------

uint8_t serialHasChar(void) {
    if (SERIALA & (1 << RXC)) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t serialGet(void) {
    while(!serialHasChar());
    return SERIALDATA;
}

uint8_t serialGetBlocking(void) {
    return serialGet();
}

uint8_t serialRxBufferFull(void) {
    return 0;
}

uint8_t serialRxBufferEmpty(void) {
    return 1;
}

// ----------------------
// |    Transmission    |
// ----------------------

void serialWrite(uint8_t data) {
#ifdef SERIALINJECTCR
    if (data == '\n') {
        serialWrite('\r');
    }
#endif
    while (!(SERIALA & (1 << UDRE))); // Wait for empty buffer
    SERIALDATA = data;
}

void serialWriteString(const char *data) {
    while (*data != '\0') {
        serialWrite(*data++);
    }
}

uint8_t serialTxBufferFull(void) {
    return 0;
}

uint8_t serialTxBufferEmpty(void) {
    return 1;
}
