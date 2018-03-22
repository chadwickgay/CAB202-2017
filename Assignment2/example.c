// Include the AVR IO library
#include <avr/io.h>

// Include delay function (which is quite useful)
#include <util/delay.h>

// Include the cab202 teensy stuff
#include <graphics.h>
#include <sprite.h>
#include "cpu_speed.h"
#include "macros.h"


/*
 | For this demonstration, we're going to create 3 different sized sprites
 | We'll make a small 3x3 sprite, a medium 8x8 sprite, and a large 12x12 sprite
 | So first thing we will do is declare the 3 sprites (essentially set a name for them and sets up memory)
*/
Sprite sprite_small;
Sprite sprite_medium;
Sprite sprite_large;

int main(void)
{
    set_clock_speed(CPU_8MHz);
    lcd_init( LCD_DEFAULT_CONTRAST );
    
    /*
     | Okay, before we do anything what we're going to want do is open sprites.h from the teensy library
     | the main thing we will see, is what variables makeup the sprite and 2 functions for the sprites 
     | which are typedef struct sprite, sprite_init() and sprite_draw().
     | 
     | Lets split up the 3 different sized sprites into their sections and do them one by one.
     | We'll start with the medium size as its just a standard byte making it the most simple.
    */
    
    
    // --- MEDIUM SPRITE 8x8 ---
    /*
     | for the medium sized sprite,
     | lets draw a simple smiley face, something like this:
     |
     |  (*_*)
     |
     | so essentially a circle with eyes and a mouth.
     | we now know what we want to draw, so lets now draw this in computer terms (using ones and zeros)
     | we will draw a 1 where we want a it to draw something, and zero everywhere else (the gaps)
     | 
     | step 1: draw the ones. So remember we wanted a circle with eyes and a mouth, so lets draw the
     |         the rough shape. Additionally this was an 8 by 8 sprite, so we will make sure not to use
     |         more than 8 numbers in a single row or column.
     |
     |    1111
     |   1    1
     |  1      1
     |  1 1  1 1
     |  1      1
     |  1 1111 1
     |   1    1 
     |    1111 
     |
     | step 2: draw zeroes for the gaps, 
     | and remember this is in terms of bytes meaning 8 bits in a byte, so every row should have 8 numbers
     |
     |  00111100
     |  01000010
     |  10000001
     |  10100101
     |  10000001
     |  10111101
     |  01000010
     |  00111100
     |
     |  we now have our image in terms of binary bytes, so lets store that in a variable and make our sprite
     |  we have 8 bytes in total so our variable storing this image will be an array with 8 elements
     |  additionally since our elements are binary elements so we will put a 0b (binary) before each byte
     |  so 00111100 would become 0b00111100
     |  this is important because we want it to read these as binary, for example 100 would normally be 
     |  the number "one hundred" but if it reads it in binary, then 100 is only the number "four".
     */
    
    // so now that we have our image, lets make a variable to store it, and initialize it with the image.
    // note that since our image is made up of 8 binary numbers, that our variable is going to be an array.
    // delare our array which I will simply name medium_image and lets give it our image that we created
    uint8_t medium_image[] = 
    {
        0b00111100,
        0b01000010,
        0b10000001,
        0b10100101,
        0b10000001,
        0b10111101,
        0b01000010,
        0b00111100
    };
    
    /*
     | now we have our image setup, and we setup our sprite name at the start as a globel variable
     | so lets create the sprite and initialize it with our image (and also x,y,width,height)
     | the one thing to remember is when we initialize it, we want to give it the address for the 
     | name of the sprite, if sprite_medium was our name, then the address of that name would 
     | be &sprite_medium as the & symbol is used to retrieve a memory address.
    */
    
    // using sprite_init(sprite, x, y, width, height, image);
    // for this example x,y doesnt matter so I'll let it simple be at coordinate 30,30
    // and its width and height was 8 by 8.
    sprite_init(&sprite_medium, 20, 20, 8, 8, medium_image);
    
    // now lets draw it onto the screen and take a look.
    sprite_draw(&sprite_medium);
    show_screen();
    
    
    // --- SMALL SPRITE 3x3 ---
    /*
     | Now lets look at the small sprite 3x3
     | 
     | first we'll create the image, its pretty hard to get creative with an image this small, so lets
     | just make a simple x shape, so something like this:
     |
     | 1 1
     |  1
     | 1 1
     |
     | which for our bytes would like 
     |
     | 101
     | 010
     | 101
     |
     | but remember earlier I said a byte must have 8 numbers, which is still true
     | thus we can think of this sprite as looking like the above, but with 5 more gaps after it
     | for a total of 8 bits in our byte
     |
     | 0b10100000
     | 0b01000000
     | 0b10100000
     |
     | to cut a long story short, we will pass this in as the image but the sprite_draw function will
     | see that the sprite only has a width of 3, and simple use the first 3 bits of each byte.
     |
     | just a warning for something to watch out for, DONT do this
     |
     | 0b101    
     | 0b010 
     | 0b101
     |
     | as in binary bytes this would become 
     |
     | 0b00000101    
     | 0b00000010    
     | 0b00000101    
     |
     | where the first 3 bits are all zeroes, thus image would appear as entirely 
     | zeroes (essentially no image).
    */
    
    // setup the image
    uint8_t small_image[] = 
    {
        0b10100000,
        0b01000000,
        0b10100000,
    };
    
    // initialize our 3 by 3 sprite
    // this time I will use the x,y as 40,20 so it doesnt overlap with our first sprite
    sprite_init(&sprite_small, 40, 20, 3, 3, small_image);
    
    // and lastly lets draw it and take a look
    sprite_draw(&sprite_small);
    show_screen();
    
    
    // --- LARGE SPRITE 12x12 ---
    /*
     | Now lets look at the final sprite, the 12 by 12, since this is a lot larger then our 8 by 8, 
     | lets draw a more detailed smiley face.
     | heres my improved smiley face, if its a little hard to read, then just run the program to see what
     | it looks like
     |
     |
     |      1111    
     |    11    11  
     |   1        1 
     |   1 11  11 1 
     |  1  11  11  1
     |  1    1     1
     |  1    1     1
     |  1 1  11  1 1
     |   1 1    1 1 
     |   1  1111  1 
     |    11    11  
     |      1111    
     |
     |
     |  000011110000
     |  001100001100
     |  010000000010
     |  010110011010
     |  100110011001
     |  100001000001
     |  100001000001
     |  101001100101
     |  010100001010
     |  010011110010
     |  001100001100
     |  000011110000
     |
     |  Now once again, remember, bytes come in 8 bits, so we need to put this into bytes, 
     |  now clearly 12 is larger then 8, so we can't fit this into a single byte, so instead
     |  we can split it up into 2 bytes. first I'll take the first 8 bits of each row (as the first byte)
     |  and the remaining 4 bits can go in a second byte
     |
     |  00001111  0000
     |  00110000  1100
     |  01000000  0010
     |  01011001  1010
     |  10011001  1001
     |  10000100  0001
     |  10000100  0001
     |  10100110  0101
     |  01010000  1010
     |  01001111  0010
     |  00110000  1100
     |  00001111  0000
     |
     |  to complete that second byte, we can fill the
     |  empty slots with zeroes, as I previously said, when we use the sprite_draw function, it will
     |  read that the sprite has a width of 12 and simple use the first 12 bits of each row
     |
     |  00001111  00000000
     |  00110000  11000000
     |  01000000  00100000
     |  01011001  10100000
     |  10011001  10010000
     |  10000100  00010000
     |  10000100  00010000
     |  10100110  01010000
     |  01010000  10100000
     |  01001111  00100000
     |  00110000  11000000
     |  00001111  00000000
     |
     |
     |            
     |    <------2 bytes------->
     |    <------12bits---->
     |  
     |    0b00001111, 0b00000000    ^
     |    0b00110000, 0b11000000    |
     |    0b01000000, 0b00100000    |
     |    0b01011001, 0b10100000    |
     |    0b10011001, 0b10010000    |  
     |    0b10000100, 0b00010000    |
     |    0b10000100, 0b00010000    |  12 bits/12 rows
     |    0b10100110, 0b01010000    |
     |    0b01010000, 0b10100000    |
     |    0b01001111, 0b00100000    |
     |    0b00110000, 0b11000000    |
     |    0b00001111, 0b00000000    V
     |
    */
    
    // create the image array
    uint8_t large_image[] = 
    {
        0b00001111, 0b00000000,
        0b00110000, 0b11000000,
        0b01000000, 0b00100000,
        0b01011001, 0b10100000,
        0b10011001, 0b10010000,
        0b10000100, 0b00010000,
        0b10000100, 0b00010000,
        0b10100110, 0b01010000,
        0b01010000, 0b10100000,
        0b01001111, 0b00100000,
        0b00110000, 0b11000000,
        0b00001111, 0b00000000,
    };
    
    // initialize the sprite with the image
    sprite_init(&sprite_large, 60, 20, 12, 12, large_image);
    
    // and lets draw it and take a look at our fine work
    sprite_draw(&sprite_large);
    show_screen();

    while(1);
    return 0;
}