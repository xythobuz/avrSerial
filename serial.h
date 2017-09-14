/*
 * serial.h
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
#ifndef _serial_h
#define _serial_h

/** \addtogroup uart UART Library
 *  UART Library enabling you to control all available
 *  UART Modules. With XON/XOFF Flow Control and buffered
 *  Receiving and Transmitting.
 *  @{
 */

/** \file serial.h
 *  UART Library Header File
 */

/** Calculate Baudrate Register Value */
#define BAUD(baudRate,xtalCpu) ((xtalCpu) / ((baudRate) * 16l) - 1)

/** Get number of available UART modules.
 *  \returns number of modules
 */
uint8_t serialAvailable(void);

/** Initialize the UART Hardware.
 *  \param uart UART Module to initialize
 *  \param baud Baudrate. Use the BAUD() macro!
 */
void serialInit(uint8_t uart, uint16_t baud);

/** Stop the UART Hardware.
 *  \param uart UART Module to stop
 */
void serialClose(uint8_t uart);

/** Manually change the flow control.
 *  Flow Control has to be compiled into the library!
 *  \param uart UART Module to operate on
 *  \param on 1 of on, 0 if off
 */
void setFlow(uint8_t uart, uint8_t on);

/** Check if a byte was received.
 *  \param uart UART Module to check
 *  \returns 1 if a byte was received, 0 if not
 */
uint8_t serialHasChar(uint8_t uart);

/** Read a single byte.
 *  \param uart UART Module to read from
 *  \returns Received byte or 0
 */
uint8_t serialGet(uint8_t uart);

/** Wait until a character is received.
 *  \param uart UART Module to read from
 *  \returns Received byte
 */
uint8_t serialGetBlocking(uint8_t uart);

/** Check if the receive buffer is full.
 *  \param uart UART Module to check
 *  \returns 1 if buffer is full, 0 if not
 */
uint8_t serialRxBufferFull(uint8_t uart);

/** Check if the receive buffer is empty.
 *  \param uart UART Module to check
 *  \returns 1 if buffer is empty, 0 if not.
 */
uint8_t serialRxBufferEmpty(uint8_t uart);

/** Send a byte.
 *  \param uart UART Module to write to
 *  \param data Byte to send
 */
void serialWrite(uint8_t uart, uint8_t data);

/** Send a string.
 *  \param uart UART Module to write to
 *  \param data Null-Terminated String
 */
void serialWriteString(uint8_t uart, const char *data);

/** Send a 16bit integer.
 *  \param uart UART Module to write to
 *  \param num Unsigned integer to send as decimal ASCII
 */
void serialWriteInt16(uint8_t uart, uint16_t num);

/** Check if the transmit buffer is full.
 *  \param uart UART Module to check
 *  \returns 1 if buffer is full, 0 if not
 */
uint8_t serialTxBufferFull(uint8_t uart);

/** Check if the transmit buffer is empty.
 *  \param uart UART Module to check
 *  \returns 1 if buffer is empty, 0 if not.
 */
uint8_t serialTxBufferEmpty(uint8_t uart);

#endif // _serial_h
/** @} */

