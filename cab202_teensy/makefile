TARGET = libcab202_teensy.a

SRC = graphics.c lcd.c ram_utils.c sprite.c
HDR = graphics.h lcd.h ram_utils.h sprite.h macros.h
OBJ = graphics.o lcd.o ram_utils.o sprite.o

FLAGS = \
	-mmcu=atmega32u4 \
	-DF_CPU=8000000UL \
	-Os \
	-funsigned-char \
	-funsigned-bitfields \
	-ffunction-sections \
	-fpack-struct \
	-fshort-enums \
	-Wall \
	-Werror \
	-std=gnu99 

all: $(TARGET)

clean:
	if [ -f $(TARGET) ]; then rm $(TARGET); fi
	for f in $(OBJ); do if [ -f $${f} ] ; then rm $${f}; fi; done

rebuild: clean all

$(TARGET) : $(HDR) $(SRC) 
	avr-gcc -c $(SRC) $(FLAGS)
	avr-ar rcs $(TARGET) $(OBJ)
