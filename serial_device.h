/*
 * serial_device.h
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
#ifndef _serial_device_h
#define _serial_device_h

/** \addtogroup uart UART Library
 *  UART Library enabling you to control all available
 *  UART Modules. With XON/XOFF Flow Control and buffered
 *  Receiving and Transmitting.
 *  @{
 */

/** \file serial_device.h
 *  UART Library device-specific configuration.
 *  Contains Register and Bit Positions for different AVR devices.
 */

#if  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
    || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__) \
|| defined(__AVR_ATmega323__)

#define UART_COUNT 1
#define UART_REGISTERS 6
#define UART_BITS 7
volatile uint8_t *serialRegisters[UART_COUNT][UART_REGISTERS] = {{
    &UDR,
    &UCSRB,
    &UCSRC,
    &UCSRA,
    &UBRRH,
    &UBRRL
}};
#define SERIALBAUDBIT 8
uint8_t serialBits[UART_COUNT][UART_BITS] = {{
    UCSZ0,
    UCSZ1,
    RXCIE,
    RXEN,
    TXEN,
    UDRIE,
    UDRE
}};
#define SERIALRECIEVEINTERRUPT USART_RXC_vect
#define SERIALTRANSMITINTERRUPT USART_UDRE_vect

#elif defined(__AVR_ATmega168__)

#define UART_COUNT 1
#define UART_REGISTERS 5
#define UART_BITS 7
volatile uint8_t *serialRegisters[UART_COUNT][UART_REGISTERS] = {{
    &UDR0,
    &UCSR0B,
    &UCSR0C,
    &UCSR0A
}};
#define SERIALBAUDBIT 16
volatile uint16_t *serialBaudRegisters[UART_COUNT] = {
    &UBRR0
};
uint8_t serialBits[UART_COUNT][UART_BITS] = {{
    UCSZ00,
    UCSZ01,
    RXCIE0,
    RXEN0,
    TXEN0,
    UDRIE0,
    UDRE0
}};
#define SERIALRECIEVEINTERRUPT USART_RX_vect
#define SERIALTRANSMITINTERRUPT USART_UDRE_vect

#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega1280__) \
    || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega640__)

#define UART_COUNT 4
#define UART_REGISTERS 4
#define UART_BITS 7
volatile uint8_t *serialRegisters[UART_COUNT][UART_REGISTERS] = {
    {
        &UDR0,
        &UCSR0B,
        &UCSR0C,
        &UCSR0A
    },
    {
        &UDR1,
        &UCSR1B,
        &UCSR1C,
        &UCSR1A
    },
    {
        &UDR2,
        &UCSR2B,
        &UCSR2C,
        &UCSR2A
    },
    {
        &UDR3,
        &UCSR3B,
        &UCSR3C,
        &UCSR3A
    }
};
#define SERIALBAUDBIT 16
volatile uint16_t *serialBaudRegisters[UART_COUNT] = {
    &UBRR0, &UBRR1, &UBRR2, &UBRR3
};
uint8_t serialBits[UART_COUNT][UART_BITS] = {
    {
        UCSZ00,
        UCSZ01,
        RXCIE0,
        RXEN0,
        TXEN0,
        UDRIE0,
        UDRE0
    },
    {
        UCSZ10,
        UCSZ11,
        RXCIE1,
        RXEN1,
        TXEN1,
        UDRIE1,
        UDRE1
    },
    {
        UCSZ20,
        UCSZ21,
        RXCIE2,
        RXEN2,
        TXEN2,
        UDRIE2,
        UDRE2
    },
    {
        UCSZ30,
        UCSZ31,
        RXCIE3,
        RXEN3,
        TXEN3,
        UDRIE3,
        UDRE3
    }
};
#define SERIALRECIEVEINTERRUPT   USART0_RX_vect
#define SERIALTRANSMITINTERRUPT  USART0_UDRE_vect
#define SERIALRECIEVEINTERRUPT1  USART1_RX_vect
#define SERIALTRANSMITINTERRUPT1 USART1_UDRE_vect
#define SERIALRECIEVEINTERRUPT2  USART2_RX_vect
#define SERIALTRANSMITINTERRUPT2 USART2_UDRE_vect
#define SERIALRECIEVEINTERRUPT3  USART3_RX_vect
#define SERIALTRANSMITINTERRUPT3 USART3_UDRE_vect

#else
#error "AvrSerialLibrary not compatible with your MCU!"
#endif

#endif // _serial_device_h
/** @} */
