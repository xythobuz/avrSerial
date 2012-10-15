/*
 * serialBlocking.c
 *
 * Copyright 2012 Thomas Buck <xythobuz@me.com>
 *
 * This file is part of avrSerial.
 *
 * avrSerial is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * avrSerial is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with avrSerial.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <stdint.h>

#include "serial.h"

uint8_t serialInit(uint16_t baud) {
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

    return 0;
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
