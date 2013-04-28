# avrSerial

This is a serial library for many different Atmel AVR MCUs. It is using two FIFO Buffers per USART module for interrupt driven UART communication. XON/XOFF Flow control for incoming data can be enabled for all UART modules, it will operate independently for each.

Device-specific configuration is in serial_device.h. You should be able to easily add new AVR MCUs. Just get the relevant register and bit names from the data-sheet.

A small test application is included. It will be built when calling either of these commands

    make all
    make test.hex

You can also flash it with

    make program

just adjust your programmer type and port in the makefile.

For normal use, either include serial.c, serial.h and serial_device.h in your project, or build a statically-linked library by calling

    make lib

and linking this to your project, as well as including serial.h.

The current Doxygen Documentation can be found in doc/ and [on the web](http://www.xythobuz.org/avrserial/) or as [PDF](http://www.xythobuz.org/avrserial.pdf)...

## License

Everything is released under a BSD 2-Clause License.

Copyright (c) 2012, Thomas Buck
All rights reserved.

> Redistribution and use in source and binary forms, with or without
> modification, are permitted provided that the following conditions
> are met:
>
>  - Redistributions of source code must retain the above copyright notice,
>    this list of conditions and the following disclaimer.
>
>  - Redistributions in binary form must reproduce the above copyright
>    notice, this list of conditions and the following disclaimer in the
>    documentation and/or other materials provided with the distribution.
>
> THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
> "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
> TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
> PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
> CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
> EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
> PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
> PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
> LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
> NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
> SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

