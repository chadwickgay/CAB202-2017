

#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Millisecond delay between game updates
#define DELAY (10)

// Is the game over?
bool game_over = false;

// String containing sprite bitmap
char * bird_img =
"   _  "
"__(0)<"
"\\___) ";

// (a) Declare global variable of type sprite_id called angry_bird.
sprite_id angry_bird;

void setup( void ) {
	// (b) Set the value of angry_bird: create a sprite which displays the bird-like 
	//     bitmap which is provided for you. The width of the bitmap is 6 units 
	//     and the height is 3 units. The sprite should be placed so it is 
	//     sitting on the bottom row of the terminal window, flush with the 
	//     left side of the window.
	angry_bird = sprite_create( 0, screen_height() - 3, 6, 3, bird_img );

	// (c) Call sprite_turn_to(...) to set angry_bird moving with
	//     initial horizontal step-size of 0.33 units and vertical step-size
	//     -0.41 units.
	sprite_turn_to( angry_bird, 0.33, -.41 );
		// (d) Call sprite_draw(...) to make angry_bird visible at the start of the game.
	sprite_draw(angry_bird);


}

void process(void) {
	// (e) Call sprite_step(...) to move angry_bird one step in the current 
	//     direction determined by its internal (dx,dy) step values.
	sprite_step(angry_bird);

	// (f) Declare floating point variable called dx, of type double.
	//     Assign a value to dx by calling sprite_dx(...) to find the current 
	//     horizontal step size of angry_bird.
	double dx = sprite_dx(angry_bird);

	// (g) Declare floating point variable called dy, of type double.
	//     Assign a value to  dy by calling sprite_dy(...) to find the current 
	//     vertical step size of angry_bird.
	double dy = sprite_dy(angry_bird);

	// (h) Add constant gravitational acceleration of 0.0025 to dy.
	dy = dy + 0.0025;
	
	// (i) Call sprite_turn_to(...) to update angry_bird's internal step values:
	//     dx does not change, but dy does.
	sprite_turn_to( angry_bird, dx, dy );

	// (j) Call sprite_draw(...) to show angry_bird in its new position.
	sprite_draw(angry_bird);
	//timer_pause(100000);

	// Don’t mess with the following statement. It detects when angry_bird 
	// falls off the bottom of the terminal and terminates the game.
	if ( sprite_y(angry_bird) >= screen_height() ) {
		game_over = true;
	}
}

// Clean up game
void cleanup(void) {
	// No change is required here.
}

// Program entry point.
int main( void ) {
	setup_screen();
	setup();
	show_screen();

	while ( !game_over ) {
		clear_screen();
		process();
		show_screen();
		timer_pause( DELAY );
	}

	cleanup();
	return 0;
}