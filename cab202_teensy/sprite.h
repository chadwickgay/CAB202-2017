/*
 *  CAB202 Teensy Library (cab202_teensy)
 *	sprite.h
 *
 *	B.Talbot, September 2015
 *	L.Buckingham, September 2017
 *	Queensland University of Technology
 */
#ifndef SPRITE_H_
#define SPRITE_H_

#include <stdint.h>

/*
 * 	Sprite type definition
 */
typedef struct sprite {
	float x, y;             // Position of top-left sprite corner.
	float dx, dy;			// Horizontal and vertical step size. 
	uint8_t width, height;	// Pixel width and height of sprite.
	uint8_t is_visible;		// Boolean visibility of sprite.
	uint8_t *bitmap;		// Bit-packed pixel data (should be h*ceil(w/8) bytes long!)
} Sprite;

/**
 *	A sprite_id is the address of a Sprite structure.
 */
typedef Sprite * sprite_id;

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
	float x, float y, uint8_t width, uint8_t height, uint8_t bitmap[]);

/**
 *	Renders the image of the sprite at its current location on the screen.
 *	Fractional coordinates are rounded. If the sprite is not visible (i.e
 *	sprite->is_visible == 0) then nothing is drawn. If the sprite is not on 
 *	the screen, nothing is drawn.
 */
void sprite_draw(sprite_id sprite);

#endif /* SPRITE_H_ */
