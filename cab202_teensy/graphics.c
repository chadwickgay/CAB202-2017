/*
*	CAB202 Teensy Library: 'cab202_teensy'
*	graphics.c
*
*	B.Talbot, September 2015
*	L.Buckingham, September 2017
*	Queensland University of Technology
*/
#include <avr/pgmspace.h>
#include <stdint.h>

#include "graphics.h"
#include "macros.h"

/*
 *  Array of bytes used as screen buffer.
 *  (accessible from any file that includes graphics.h)
 */
uint8_t screen_buffer[LCD_BUFFER_SIZE];

/*
 *  Copy the contents of the screen buffer to the LCD.
 *	This is the only function that interfaces with the LCD hardware
 *  (sends entire current buffer to LCD screen)
 */
void show_screen(void) {
	// Reset our position in the LCD RAM
	lcd_position(0, 0);

	// Iterate through our buffer and write each byte to the LCD.
	for ( int i = 0; i < LCD_BUFFER_SIZE; i++ ) {
		lcd_write(LCD_D, screen_buffer[i]);
	}
}

/*
 * Clear the screen buffer (all pixels set to BG_COLOUR).
 */
void clear_screen(void) {
	// Set every byte in the buffer to 0b00000000
	for ( int i = 0; i < LCD_BUFFER_SIZE; i++ ) {
		screen_buffer[i] = 0;
	}
}

/**
 *	Draw or erase a designated pixel in the screen buffer.
 *
 *	Parameters:
 *		x - The horizontal position of the pixel. The left edge of the screen
 *			is at x=0; the right edge is at (LCD_X-1).
 *		y - The vertical position of the pixel. The top edge of the screen is
 *			at y=0; the bottom edge is at (LCD_Y-1).
 *		colour - The colour, FG_COLOUR or BG_COLOUR.
 */
void set_pixel(uint8_t x, uint8_t y, colour_t colour) {
	// Do nothing if requested pixel is out of bounds. 
	if ( x >= LCD_X || y >= LCD_Y ) {
		return;
	}

	// Calculate the pixel 'subrow', within that LCD row
	uint8_t row = y >> 3;
	uint8_t subrow = y & 7;

	// Set that particular pixel in our screen buffer
	if ( colour ) {
		// Draw Pixel
		screen_buffer[row*LCD_X + x] |= (1 << subrow);
	}
	else {
		// Erase Pixel
		screen_buffer[row*LCD_X + x] &= ~(1 << subrow);
	}
}

/**
 *	Draw a line in the screen buffer.
 *
 *	Parameters:
 *		x1 - The horizontal position of the start point of the line.
 *		y1 - The vertical position of the start point of the line.
 *		x2 - The horizontal position of the end point of the line.
 *		y2 - The vertical position of the end point of the line.
 *		colour - if zero, line is erased; otherwise line is drawn.
 */
void draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t colour) {
	if ( x1 == x2 ) {
		// Draw vertical line
		for ( int i = y1; (y2 > y1) ? i <= y2 : i >= y2; (y2 > y1) ? i++ : i-- ) {
			set_pixel(x1, i, colour);
		}
	}
	else if ( y1 == y2 ) {
		// Draw horizontal line
		for ( int i = x1; (x2 > x1) ? i <= x2 : i >= x2; (x2 > x1) ? i++ : i-- ) {
			set_pixel(i, y1, colour);
		}
	}
	else {
		//	Always draw from left to right, regardless of the order the endpoints are 
		//	presented.
		if ( x1 > x2 ) {
			int t = x1;
			x1 = x2;
			x2 = t;
			t = y1;
			y1 = y2;
			y2 = t;
		}

		// Get Bresenhaming...
		float dx = x2 - x1;
		float dy = y2 - y1;
		float err = 0.0;
		float derr = ABS(dy / dx);

		for ( int x = x1, y = y1; (dx > 0) ? x <= x2 : x >= x2; (dx > 0) ? x++ : x-- ) {
			set_pixel(x, y, colour);
			err += derr;
			while ( err >= 0.5 && ((dy > 0) ? y <= y2 : y >= y2) ) {
				set_pixel(x, y, colour);
				y += (dy > 0) - (dy < 0);
				err -= 1.0;
			}
		}
	}
}

/**
 *	Render one of the printable ASCII characters into the screen buffer.
 *
 *	Parameters:
 *		x - The horizontal position of the top-left corner of the glyph.
 *		y - The vertical position of the top-left corner of the glyph.
 *		character - The ASCII code of the character to render. Valid values
 *			range from 0x20 == 32 to 0x7f == 127.
 */
void draw_char(uint8_t top_left_x, uint8_t top_left_y, char character, colour_t colour) {
	// loop through each pixel in the character array and plot each one individually
	// TODO: This can be done more efficiently by careful bit-masking.
	for ( uint8_t i = 0; i < CHAR_WIDTH; i++ ) {
		uint8_t pixel_data = pgm_read_byte(&(ASCII[character - 0x20][i]));

		if ( colour == BG_COLOUR ) {
			pixel_data = ~pixel_data;
		}

		for ( uint8_t j = 0; j < CHAR_HEIGHT; j++ ) {
			set_pixel(top_left_x + i, top_left_y + j, (pixel_data & (1 << j)) >> j);
		}
	}
}

/**
 *	Render a string of printable ASCII characters into the screen buffer.
 *
 *	Parameters:
 *		x - The horizontal position of the top-left corner of the displayed
 *			text.
 *		y - The vertical position of the top-left corner of the displayed
 *			text.
 *		character - The ASCII code of the character to render. Valid values
 *			range from 0x20 == 32 to 0x7f == 127.
 *		colour - The colour, FG_COLOUR or BG_COLOUR. If colour is BG_COLOUR,
 *			the text is rendered as an inverse video block.
 */
void draw_string(uint8_t top_left_x, uint8_t top_left_y, char *text, colour_t colour) {
	// Draw each character until the null terminator is reached
	for ( uint8_t x = top_left_x, i = 0; text[i] != 0; x += CHAR_WIDTH, i++ ) {
		draw_char(x, top_left_y, text[i], colour);
		// Add a column of spaces here if you want to space out the lettering.
		// (see lcd.c for a hint on how to do this)
	}
}
