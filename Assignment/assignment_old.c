#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define PLAYER_WIDTH (3)
#define PLAYER_HEIGHT (3)

#define ZOMBIE_WIDTH (4)
#define ZOMBIE_HEIGHT (4)

#define PLATFORM_WIDTH (screen_width() / 3)
#define PLATFORM_HEIGHT (1)

#define DOOR_WIDTH (4)
#define DOOR_HEIGHT (4)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */
int score = 0;
int lives = 10;
int level = 1;

// Movement variables
bool jump = false;
double speed = 0;

// Sprite images
char * player_image =
/**/	" o "
/**/	"/:\\"
/**/	"/ \\";

char * zombie_image =
/**/	"ZZZZ"
/**/	"  Z "
/**/	" Z  "
/**/	"ZZZZ";

char * platform_image = "============================================================================================================";

char * door_image =
/**/	"EXIT"
/**/	":  :"
/**/	": .:"
/**/	":  :";

// Declare a sprite_id called player
sprite_id player;

// Declare a sprite_id called zombie
sprite_id zombie;

// Declare a sprite_id called platform
sprite_id platform;

// Declare a sprite_id called door
sprite_id door;

void draw_border( void ) {
	int left = 0;
	int top = 1;
	int right = screen_width() - 1;
	int bottom = screen_height() - 1;
    
    //Draw a line from (left, top) to (right, top)
	draw_line (left, top, right, top, '-');
    // Draw a line from (right, top) to (right, bottom)
	draw_line (right, top + 1, right, bottom - 1, ':');
    // Draw a line from (left, bottom) to (right, bottom)
	draw_line (left, bottom, right, bottom, '=');
    // Draw a line from (left, top) to (left, bottom)
	draw_line (left, top + 1, left, bottom - 1, ':');
    // Do not touch the following line.
    show_screen();
}

void display_lives(int label_offset) {
	draw_formatted((screen_width() * 0.25) + label_offset, 0, "Lives: %d", lives);
}

void display_score(int label_offset) {
	draw_formatted((screen_width() * 0.75) + label_offset, 0, "Score: %d", score);
}

void display_level(int label_offset) {
	draw_formatted((screen_width() * 0.50) + label_offset, 0, "Level: %d", level);
}

void draw_header (void) {
	int label_offset = 1;
	// Draw Time
	draw_string(1, 0, "Time: 00:00"); // Come back and do this
	// Draw Lives
	display_lives(label_offset);
	// Draw Level
	display_level(label_offset);
	// Draw Score 
	display_score(label_offset);

}

void create_sprites(void){
	// Sprite variables
	// Player
	int p_start_x = 3;
	int p_start_y = screen_height() - 4;
	// Zombie
	int z_start_x = screen_width() - 10;
	int z_start_y = screen_height() - 5;
	// Platform
	int plat_start_x = screen_width() * 0.33;
	int plat_start_y = screen_height() - (3.5 * ZOMBIE_HEIGHT);
	// Door
	int door_start_x = screen_width() - 10;
	int door_start_y = screen_height() - 5;

	// Create player sprite
	player = sprite_create(p_start_x, p_start_y, PLAYER_WIDTH, PLAYER_HEIGHT, player_image);
	// Create zombie sprite
	zombie = sprite_create(z_start_x, z_start_y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_image);
	// Create platform sprite
	platform = sprite_create(plat_start_x, plat_start_y, PLATFORM_WIDTH, PLATFORM_HEIGHT, platform_image);
	// Create platform sprite
	door = sprite_create(door_start_x, door_start_y, DOOR_WIDTH, DOOR_HEIGHT, door_image);
}

void draw_sprites(void){
	// Draw the player.
    sprite_draw(player);
    // Draw the zombie
    sprite_draw(zombie);
    // Draw the platform
    sprite_draw(platform);
    // Draw the platform
    sprite_draw(door);
}

// Setup game.
void setup(void) {
	draw_border();
	draw_header();
	create_sprites();

	// Set the zombie in motion
	sprite_turn_to( zombie, -0.5, 0 );

	draw_sprites();
}

void move_zombie(void){
	// Start zombie movement 
	sprite_step(zombie);

	// Get screen location of zombie.
	int zx = round(sprite_x(zombie));
	//int zy = round(sprite_y(zombie));

	// Get the displacement vector of the zombie.
	double zdx = sprite_dx( zombie );
	double zdy = sprite_dy( zombie );

	// Test to see if the zombie hit the left or right border.
	if ( zx <= 0 ) {
		zdx = fabs(zdx);
	} 
	else if ( zx >= screen_width() - ZOMBIE_WIDTH ) {
		zdx = -fabs(zdx);
	}

	// Test to see if the zombie needs to step back and change direction.
	if ( zdx != sprite_dx( zombie ) || zdy != sprite_dy(zombie) ) {
		sprite_back( zombie );
		sprite_turn_to( zombie, zdx, zdy );
	}
	// End zombie movement
}

// Play one turn of game.
void process(void) {

	move_zombie();	

	// Start player movement
	// Get a character code from standard input without waiting.
	int key = get_char();

	/*
	sprite_step(player);

	// Get the current screen coordinates of the player in integer variables 
	// by rounding the actual coordinates.
	int px = round(sprite_x(player));

	// Checks if wall has been hit
	if (px == 1 || px == screen_width() - 1 - PLAYER_WIDTH){
		speed = 0;
	}

	// These speeds will need to be made dynamic somehow...
	// Adjusts player speed based on key press with maximum allowed
	if (key == 'd'){
		if (speed < 1){
			speed = speed + 0.5;
		}			
	} else if (key == 'a' && px > 1){
		if (speed > -1){
			speed = speed - 0.5;
		}		
	}
	// Moves player according to input speed set
	sprite_turn_to(player, speed, 0);

	// End player left/right movement
	*/


	

	// Start player jumping
	if (key == 'w'){
		jump = true;
	}

	double p_dx = sprite_dx(player);
	double p_dy = sprite_dy(player);

	int p_y = sprite_y(player);	

	if (jump){
		p_dy = p_dy - 0.0025;
		sprite_step(player);	

		if (p_y == screen_height() - (6 * PLAYER_HEIGHT)){
			jump = false;
		}				
	} else if (!jump && p_y < screen_height() - 3 - PLATFORM_HEIGHT){	
		sprite_back(player);
		p_dy = p_dy + 0.0025;
	}

	sprite_turn_to( player, p_dx, p_dy );	

	// End player jumping

	


	// Clear screen & redraw.
    clear_screen();

    // Draw header
    draw_header();

    // Draw border
    draw_border();

	draw_sprites();
}



int main( void ) {

	setup_screen();
	setup();
	show_screen();

	while ( !game_over ) {
		process();

		if ( update_screen ) {
			show_screen();
		}

		timer_pause(DELAY);
	}

	return 0;
}