/*
 *  CAB202 Teensy Library (cab202_teensy)
 *	sprite.c
 *
 *	B.Talbot, September 2015
 *	L.Buckingham, September 2017
 *	Queensland University of Technology
 */
#include <math.h>
#include "sprite.h"
#include "lcd.h"
#include "graphics.h"

/**
 *  Sets the state of a Sprite. In addition to the explicit values provided
 *	via paramters, the horizontal and vertical step sizes are set to 0.0, and
 *	the sprite is made visible.
 *
 *	Parameters:
 *		sprite -  the address of a Sprite whichis to be initialised.
 *		x - the (possibly fractional) horizontal location of the top-left 
 *			corner of the sprite.
 *		y - the (possibly fractional) vertical location of the top-left 
 *			corner of the sprite.
 *		width - the width of the sprite, measured in pixels.
 *		height - the width of the sprite, measured in pixels.
 *		bitmap - the address of a sequence of bytes that contain bit-packed 
 *			pixel data. The data is not copied into the sprite; a reference to 
 *			the original bitmap is retained. Changes to that bitmapo are 
 *			automatically reflected in the sprite.
 */
void sprite_init(sprite_id sprite,
	float x, float y, uint8_t width, uint8_t height, uint8_t bitmap[]) {
	// Apply supplied values
	sprite->x = x;
	sprite->y = y;
	sprite->width = width;
	sprite->height = height;
	sprite->bitmap = bitmap;	// This is a SHALLOW copy!!!

	// Default values.
	sprite->is_visible = 1;
	sprite->dx = 0.0f;
	sprite->dy = 0.0f;
}

/**
 *	Renders the image of the sprite at its current location on the screen.
 *	Fractional coordinates are rounded. If the sprite is not visible (i.e
 *	sprite->is_visible == 0) then nothing is drawn. If the sprite is not on 
 *	the screen, nothing is drawn.
 */
void sprite_draw(sprite_id sprite) {
	// Do nothing if not visible
	if (!sprite->is_visible) {
		return;
	}

	int x = round(sprite->x);
	int y = round(sprite->y);

	if ( x >= LCD_X || x + sprite->width <= 0 ) return;
	if ( y >= LCD_Y || y + sprite->height <= 0 ) return;

	// Loop through the bit-packed bitmap data, drawing each individual bit
	// (assume that the bitmap size is h * ceil(w/8))
	int byte_width = (sprite->width + 7) / 8;

	for (int dy = 0; dy < sprite->height; dy++) {
		for (int dx = 0; dx < sprite->width; dx++) {
			set_pixel( x+dx, y+dy,
				(sprite->bitmap[(int) (dy*byte_width + dx / 8)] >> (7 - dx % 8)) & 1 // ouch...
			);
		}
	}
}
