/*
 * serial.c
 *
 * Copyright (c) 2012, 2013, Thomas Buck <xythobuz@me.com>
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
#include "serial_device.h"

/** \addtogroup uart UART Library
 *  UART Library enabling you to control all available
 *  UART Modules. With XON/XOFF Flow Control and buffered
 *  Receiving and Transmitting.
 *  @{
 */

/** \file serial.c
 *  UART Library Implementation
 */

/** If you define this, a '\\r' (CR) will be put in front of a '\\n' (LF) when sending a byte.
 *  Binary Communication will then be impossible!
 */
// #define SERIALINJECTCR

#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 32 /**< RX Buffer Size in Bytes (Power of 2) */
#endif

#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 16 /**< TX Buffer Size in Bytes (Power of 2) */
#endif

/** Defining this enables incoming XON XOFF (sends XOFF if rx buff is full) */
#define FLOWCONTROL

#define FLOWMARK 5 /**< Space remaining to trigger xoff/xon */
#define XON 0x11 /**< XON Value */
#define XOFF 0x13 /**< XOFF Value */

#if (RX_BUFFER_SIZE < 2) || (TX_BUFFER_SIZE < 2)
#error SERIAL BUFFER TOO SMALL!
#endif

#ifdef FLOWCONTROL
#if (RX_BUFFER_SIZE < 8) || (TX_BUFFER_SIZE < 8)
#error SERIAL BUFFER TOO SMALL!
#endif
#endif

#if ((RX_BUFFER_SIZE + TX_BUFFER_SIZE) * UART_COUNT) >= (RAMEND - 0x60)
#error SERIAL BUFFER TOO LARGE!
#endif

#if (RX_BUFFER_SIZE > 65535) || (TX_BUFFER_SIZE > 65535)
#error SERIAL BUFFER INDEX HAS TO FIT 16BIT!
#endif

// serialRegisters
#define SERIALDATA  0
#define SERIALB     1
#define SERIALC     2
#define SERIALA     3
#define SERIALUBRRH 4
#define SERIALUBRRL 5

// serialBits
#define SERIALUCSZ0 0
#define SERIALUCSZ1 1
#define SERIALRXCIE 2
#define SERIALRXEN  3
#define SERIALTXEN  4
#define SERIALUDRIE 5
#define SERIALUDRE  6

static uint8_t volatile rxBuffer[UART_COUNT][RX_BUFFER_SIZE];
static uint8_t volatile txBuffer[UART_COUNT][TX_BUFFER_SIZE];
static uint16_t volatile rxRead[UART_COUNT];
static uint16_t volatile rxWrite[UART_COUNT];
static uint16_t volatile txRead[UART_COUNT];
static uint16_t volatile txWrite[UART_COUNT];
static uint8_t volatile shouldStartTransmission[UART_COUNT];

#ifdef FLOWCONTROL
static uint8_t volatile sendThisNext[UART_COUNT];
static uint8_t volatile flow[UART_COUNT];
static uint16_t volatile rxBufferElements[UART_COUNT];
#endif

uint8_t serialAvailable(void) {
    return UART_COUNT;
}

void serialInit(uint8_t uart, uint16_t baud) {
    if (uart >= UART_COUNT) {
        return;
    }

    // Initialize state variables
    rxRead[uart] = 0;
    rxWrite[uart] = 0;
    txRead[uart] = 0;
    txWrite[uart] = 0;
    shouldStartTransmission[uart] = 1;

#ifdef FLOWCONTROL
    sendThisNext[uart] = 0;
    flow[uart] = 1;
    rxBufferElements[uart] = 0;
#endif

    // Default Configuration: 8N1
    *serialRegisters[uart][SERIALC] = (1 << serialBits[uart][SERIALUCSZ0])
            | (1 << serialBits[uart][SERIALUCSZ1]);

    // Set baudrate
#if SERIALBAUDBIT == 8
    *serialRegisters[uart][SERIALUBRRH] = (baud >> 8);
    *serialRegisters[uart][SERIALUBRRL] = baud;
#else
    *serialBaudRegisters[uart] = baud;
#endif

    // Enable Interrupts
    *serialRegisters[uart][SERIALB] = (1 << serialBits[uart][SERIALRXCIE]);

    // Enable Receiver/Transmitter
    *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALRXEN])
            | (1 << serialBits[uart][SERIALTXEN]);
}

void serialClose(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return;
    }

    uint8_t sreg = SREG;
    sei();
    while (!serialTxBufferEmpty(uart));

    // Wait while Transmit Interrupt is on
    while (*serialRegisters[uart][SERIALB] & (1 << serialBits[uart][SERIALUDRIE]));

    cli();
    *serialRegisters[uart][SERIALB] = 0;
    *serialRegisters[uart][SERIALC] = 0;
    SREG = sreg;
}

#ifdef FLOWCONTROL
void setFlow(uint8_t uart, uint8_t on) {
    if (uart >= UART_COUNT) {
        return;
    }

    if (flow[uart] != on) {
        if (on == 1) {
            // Send XON
            while (sendThisNext[uart] != 0);
            sendThisNext[uart] = XON;
            flow[uart] = 1;
            if (shouldStartTransmission[uart]) {
                shouldStartTransmission[uart] = 0;

                // Enable Interrupt
                *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALUDRIE]);

                // Trigger Interrupt
                *serialRegisters[uart][SERIALA] |= (1 << serialBits[uart][SERIALUDRE]);
            }
        } else {
            // Send XOFF
            sendThisNext[uart] = XOFF;
            flow[uart] = 0;
            if (shouldStartTransmission[uart]) {
                shouldStartTransmission[uart] = 0;

                // Enable Interrupt
                *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALUDRIE]);

                // Trigger Interrupt
                *serialRegisters[uart][SERIALA] |= (1 << serialBits[uart][SERIALUDRE]);
            }
        }

        // Wait until it's transmitted
        while (*serialRegisters[uart][SERIALB] & (1 << serialBits[uart][SERIALUDRIE]));
    }
}
#endif

// ---------------------
// |     Reception     |
// ---------------------

uint8_t serialHasChar(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    if (rxRead[uart] != rxWrite[uart]) {
        // True if char available
        return 1;
    } else {
        return 0;
    }
}

uint8_t serialGetBlocking(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    while(!serialHasChar(uart));
    return serialGet(uart);
}

uint8_t serialGet(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    uint8_t c;

    if (rxRead[uart] != rxWrite[uart]) {
#ifdef FLOWCONTROL
        // This should not underflow as long as the receive buffer is not empty
        rxBufferElements[uart]--;

        if ((flow[uart] == 0) && (rxBufferElements[uart] <= FLOWMARK)) {
            while (sendThisNext[uart] != 0);
            sendThisNext[uart] = XON;
            flow[uart] = 1;
            if (shouldStartTransmission[uart]) {
                shouldStartTransmission[uart] = 0;

                // Enable Interrupt
                *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALUDRIE]);

                // Trigger Interrupt
                *serialRegisters[uart][SERIALA] |= (1 << serialBits[uart][SERIALUDRE]);
            }
        }
#endif
        c = rxBuffer[uart][rxRead[uart]];
        rxBuffer[uart][rxRead[uart]] = 0;
        if (rxRead[uart] < (RX_BUFFER_SIZE - 1)) {
            rxRead[uart]++;
        } else {
            rxRead[uart] = 0;
        }
        return c;
    } else {
        return 0;
    }
}

uint8_t serialRxBufferFull(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    return (((rxWrite[uart] + 1) == rxRead[uart])
            || ((rxRead[uart] == 0) && ((rxWrite[uart] + 1) == RX_BUFFER_SIZE)));
}

uint8_t serialRxBufferEmpty(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    if (rxRead[uart] != rxWrite[uart]) {
        return 0;
    } else {
        return 1;
    }
}

// ----------------------
// |    Transmission    |
// ----------------------

void serialWrite(uint8_t uart, uint8_t data) {
    if (uart >= UART_COUNT) {
        return;
    }

#ifdef SERIALINJECTCR
    if (data == '\n') {
        serialWrite(uart, '\r');
    }
#endif
    while (serialTxBufferFull(uart));

    txBuffer[uart][txWrite[uart]] = data;
    if (txWrite[uart] < (TX_BUFFER_SIZE - 1)) {
        txWrite[uart]++;
    } else {
        txWrite[uart] = 0;
    }
    if (shouldStartTransmission[uart]) {
        shouldStartTransmission[uart] = 0;

        // Enable Interrupt
        *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALUDRIE]);

        // Trigger Interrupt
        *serialRegisters[uart][SERIALA] |= (1 << serialBits[uart][SERIALUDRE]);
    }
}

void serialWriteString(uint8_t uart, const char *data) {
    if (uart >= UART_COUNT) {
        return;
    }

    if (data == 0) {
        serialWriteString(uart, "NULL");
    } else {
        while (*data != '\0') {
            serialWrite(uart, *data++);
        }
    }
}

uint8_t serialTxBufferFull(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    return (((txWrite[uart] + 1) == txRead[uart])
            || ((txRead[uart] == 0) && ((txWrite[uart] + 1) == TX_BUFFER_SIZE)));
}

uint8_t serialTxBufferEmpty(uint8_t uart) {
    if (uart >= UART_COUNT) {
        return 0;
    }

    if (txRead[uart] != txWrite[uart]) {
        return 0;
    } else {
        return 1;
    }
}

// ----------------------
// |      Internal      |
// ----------------------

static void serialReceiveInterrupt(uint8_t uart) {
    rxBuffer[uart][rxWrite[uart]] = *serialRegisters[uart][SERIALDATA];

    // Simply skip increasing the write pointer if the receive buffer is overflowing
    if (!serialRxBufferFull(uart)) {
        if (rxWrite[uart] < (RX_BUFFER_SIZE - 1)) {
            rxWrite[uart]++;
        } else {
            rxWrite[uart] = 0;
        }
    }

#ifdef FLOWCONTROL
    if (rxBufferElements[uart] < 0xFFFF) {
        rxBufferElements[uart]++;
    }

    if ((flow[uart] == 1) && (rxBufferElements[uart] >= (RX_BUFFER_SIZE - FLOWMARK))) {
        sendThisNext[uart] = XOFF;
        flow[uart] = 0;
        if (shouldStartTransmission[uart]) {
            shouldStartTransmission[uart] = 0;

            // Enable Interrupt
            *serialRegisters[uart][SERIALB] |= (1 << serialBits[uart][SERIALUDRIE]);

            // Trigger Interrupt
            *serialRegisters[uart][SERIALA] |= (1 << serialBits[uart][SERIALUDRE]);
        }
    }
#endif
}

static void serialTransmitInterrupt(uint8_t uart) {
#ifdef FLOWCONTROL
    if (sendThisNext[uart]) {
        *serialRegisters[uart][SERIALDATA] = sendThisNext[uart];
        sendThisNext[uart] = 0;
    } else {
#endif
        if (txRead[uart] != txWrite[uart]) {
            *serialRegisters[uart][SERIALDATA] = txBuffer[uart][txRead[uart]];
            if (txRead[uart] < (TX_BUFFER_SIZE -1)) {
                txRead[uart]++;
            } else {
                txRead[uart] = 0;
            }
        } else {
            shouldStartTransmission[uart] = 1;

            // Disable Interrupt
            *serialRegisters[uart][SERIALB] &= ~(1 << serialBits[uart][SERIALUDRIE]);
        }
#ifdef FLOWCONTROL
    }
#endif
}

ISR(SERIALRECIEVEINTERRUPT) {
    // Receive complete
    serialReceiveInterrupt(0);
}

ISR(SERIALTRANSMITINTERRUPT) {
    // Data register empty
    serialTransmitInterrupt(0);
}

#if UART_COUNT > 1
ISR(SERIALRECIEVEINTERRUPT1) {
    // Receive complete
    serialReceiveInterrupt(1);
}

ISR(SERIALTRANSMITINTERRUPT1) {
    // Data register empty
    serialTransmitInterrupt(1);
}
#endif

#if UART_COUNT > 2
ISR(SERIALRECIEVEINTERRUPT2) {
    // Receive complete
    serialReceiveInterrupt(2);
}

ISR(SERIALTRANSMITINTERRUPT2) {
    // Data register empty
    serialTransmitInterrupt(2);
}
#endif

#if UART_COUNT > 3
ISR(SERIALRECIEVEINTERRUPT3) {
    // Receive complete
    serialReceiveInterrupt(3);
}

ISR(SERIALTRANSMITINTERRUPT3) {
    // Data register empty
    serialTransmitInterrupt(3);
}
#endif
/** @} */

