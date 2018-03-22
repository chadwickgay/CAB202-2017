#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <math.h>

// Millisecond delay between game updates
#define DELAY (10)

// Is the game over?
bool game_over = false;

// String containing sprite bitmap
#define BIRD_W 6
#define BIRD_H 3
char * bird_img =
"   _  "
"__(0)<"
"\\___) ";

// The globally visible bird sprite.
sprite_id bird;

// (a) Declare a sprite_id to represent the platform.
sprite_id platform;

// (b) Define width and height constants, PLAT_W and PLAT_H. 
// Also declare a variable called platform_img to define the bitmap 
// for the platform. The platform should be 1 unit high and 18 
// units wide. It should be constructed from '@' characters.
#define PLAT_W 18
#define PLAT_H 1
char * platform_img = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";

void setup( void ) {
	// Create sprite and save in bird
	bird = sprite_create(5, screen_height() - BIRD_H, BIRD_W, BIRD_H, bird_img);

	// Set bird moving with initial horizontal step-size of 0.13 units 
	// and vertical step-size -0.47 units.
	sprite_turn_to(bird, 0.13, -0.47);

	// Make bird visible at the start of the game.
	sprite_draw(bird);

	// (c) Create platform sprite and save it. The platform should be
	// positioned at the right-hand side of the terminal window, and placed
	// 5 units above the bottom of the screen. The width of the platform 
	// is PLAT_W and its height is PLAT_H.
	platform = sprite_create( screen_width() - PLAT_W - 0, screen_height() - 1 - 5,
		PLAT_W, PLAT_H, platform_img
	);

	// (d) Draw the platform at the start of the game.
	sprite_draw(platform);
}

void process_collision( sprite_id who_moved ) {
	// Get platform and bird screen locations.
	int px = round(sprite_x(platform)), py = round(sprite_y(platform));
	int bx = round(sprite_x(bird)), by = round(sprite_y(bird));

	// Check for collision
	bool collided = true;

	if ( px >= bx + BIRD_W ) collided = false;
	if ( py >= by + BIRD_H ) collided = false;
	if ( bx >= px + PLAT_W ) collided = false;
	if ( by >= py + PLAT_H ) collided = false;

	if ( collided ) {
		// (e) If bird hit platform
		if ( who_moved == bird ) {
			// (e.a) declare floating point variables dx and dy and
			// initialise them with the velocity components of the bird.
			double dx = sprite_dx(bird), dy = sprite_dy(bird);

			// (e.b) If bird has struck the platform from below
			// negate dy but leave dx alone. The bird is bumping from 
			// below if the top of the bird is level with the platform
			// and the bird is moving upwards. 
			if ( by == py + PLAT_H - 1 && dy < 0 ) {
				dy = -dy;
			}

			// (e.c) Otherwise, if the bird has bumped into the left end
			// then quench horizontal velocity. The bird bumps the left edge if 
			// it is moving to the right and the right-most edge overlaps
			// the left edge of the platform.
			else if ( bx + BIRD_W - 1 == px && dx > 0 ) {
				dx = 0;
			}			

			// (e.d) Otherwise, if the bird has bumped into the right end
			// then quench horizontal velocity.The bird bumps the right edge if 
			// it is moving to the left and the left-most edge overlaps
			// the right edge of the platform. 
			else if ( bx == px + PLAT_W - 1 && dx < 0 ) {
				dx = 0;
			}			

			// (e.e) Otherwise, we landed on the top, so we quench the 
			// vertical velocity, so the birdcan slide along the top of the
			// platform.
			else {
				dy = 0;
			}

			// (e.f) Make bird take one step backward, then turn bird
			// to move in new (dx,dy) direction.
			sprite_back( bird );
			sprite_turn_to( bird, dx, dy );
		}
		// (f) Otherwise, if platform moved, push bird down until
		// not overlapping the platform.
		else {
			sprite_move_to( bird, sprite_x(bird), sprite_y(platform) + PLAT_H );
		}
	}
}

void process(void) {
	// (g) Get a key code from standard input without waiting.
	int key = get_char();

	// (h) Modify the following block so that the bird moves forward 
	// when user presses 'z', and backward when the user presses 'x'. Later 
	// you can change it so that the bird always moves.
	if ( key == 'z' || key == 'x'  || key < 0 )
	{
		double dx = sprite_dx(bird);
		double dy = sprite_dy(bird);
		
		// Move the bird according to its step size.
		// Accelerate bird due to gravity of 0.0025.
		
		if ( key == 'z' || key < 0 ) {
			sprite_step(bird);
			dy -= 0.0025;	
		}
		else {
			dy += 0.0025;	
			sprite_back(bird);
		}
		
		sprite_turn_to(bird, dx, dy);
		
		// Deal with bird and the platform collision.
		process_collision(bird);
	}

	// Game is over when the bird leaves the screen.
	if ( sprite_y(bird) >= screen_height() ) {
		game_over = true;
	}

	// Use boolean variable to remember if platform moved.
	bool platform_moved = false;

	// (i) Move the platform horizontally if user presses a,d, and
	// set platform_moved to true.
	if ( key == 'a' && sprite_x(platform)>0 ) {
		sprite_move( platform, -1, 0 );
		platform_moved = true;
	}
	else if ( key == 'd' && sprite_x(platform)< screen_width() - PLAT_W) {
		sprite_move( platform, +1, 0 );
		platform_moved = true;
	}

	// Process collision again, due to platform movement.
	if ( platform_moved ) {
		process_collision(platform);
	}

	// Display the bird.
	sprite_draw(bird);

	// (j) Display the platform
	sprite_draw( platform );
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
