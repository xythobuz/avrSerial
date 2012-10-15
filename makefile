MCU = atmega32
F_CPU = 16000000
RM = rm -rf
OPT = s
CSTANDARD = gnu99

SRC = serial.c
OBJ = $(SRC:.c=.o)

CARGS = -mmcu=$(MCU)
CARGS += -I$(EXTRAINCDIR)
CARGS += -O$(OPT)
CARGS += -funsigned-char
CARGS += -funsigned-bitfields
CARGS += -fpack-struct
CARGS += -fshort-enums
CARGS += -Wall -Wstrict-prototypes
CARGS += -Iinclude
CARGS += -std=$(CSTANDARD)
CARGS += -DF_CPU=$(F_CPU)

all: lib

lib: libavrSerial.a sizelibafter

sizelibafter:
	avr-size --mcu=$(MCU) -C libavrSerial.a

libavrSerial.a: $(OBJ)
	avr-ar -c -r -s libavrSerial.a $(OBJ)

%.o: %.c
	avr-gcc -c $< -o $@ $(CARGS)

clean:
	$(RM) *.o
	$(RM) *.a
