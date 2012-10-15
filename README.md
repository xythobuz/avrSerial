# avrSerial

This is a serial library for many different Atmel AVR MCUs.
In it's normal variant, the library is using two FIFO Buffers for interrupt driven UART communication.
XON/XOFF Flow control for incoming data can be enabled in this mode.

Replace serial.c with serialBlocking.c while linking to get a blocking, interrupt-less version of the same API.

## License

Everything is released under the [LGPLv3](http://www.gnu.org/licenses/lgpl-3.0.html).
> &copy; 2012 Thomas Buck
