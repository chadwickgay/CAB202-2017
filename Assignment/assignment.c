#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define HERO_WIDTH (3)
#define HERO_HEIGHT (3)

#define ZOMBIE_WIDTH (4)
#define ZOMBIE_HEIGHT (4)

#define BAT_WIDTH (3)
#define BAT_HEIGHT (2)

#define PLATFORM_WIDTH (screen_width() / 3)
#define PLATFORM_HEIGHT (1)

#define DOOR_WIDTH (4)
#define DOOR_HEIGHT (4)

#define FLOOR_WIDTH (screen_width()-1)
#define FLOOR_HEIGHT (1)

#define TREASURE_WIDTH (3)
#define TREASURE_HEIGHT (3)

#define KEY_WIDTH (3)
#define KEY_HEIGHT (3)

#define BAT_WIDTH (3)
#define BAT_HEIGHT (2)

#define ROCK_WIDTH (5)
#define ROCK_HEIGHT (2)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */
int score = 0;
int lives = 10;
int level = 1;

double hero_dx = 0;
double hero_dy = 0;
double rock_dx = 1.1 * 0.5;
double rock_dy = +0.1;

int second;
int minute;

bool rock_onGround = false;
bool onGround = true;
bool treasure_taken;
bool treasure_active = true;

bool key_taken;
bool key_active = true;
bool barrier_active = true;
bool barrier_destroyed = false;

bool swap_bat_image = true;
bool swap_treasure_image = true;
bool swap_key_image = true;

int hx, hy, rx, ry;
int px, px_2, px_3, py;
int fy;

char * hero_image =
/**/	" o "
/**/	"/|\\"
/**/	"/ \\";

char * hero_image_jump =
/**/	"\\o/"
/**/	" | "
/**/	"/ \\";

char * zombie_image =
/**/	"ZZZZ"
/**/	"  Z "
/**/	" Z  "
/**/	"ZZZZ";

char * platform_image = "============================================================================================================================================================================================================================";

char * roof_image = "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

char * door_image =
/**/	"EXIT"
/**/	":  :"
/**/	": .:"
/**/	":  :";

char * door_image_1 =
/**/	"EXIT"
/**/	" o :"
/**/	"/|\\:"
/**/	"/ \\:";

char * door_image_2 =
/**/	"EXIT"
/**/	": o:"
/**/	":/|:"
/**/	":/ :";

char * door_image_3 =
/**/	"EXIT"
/**/	":  o"
/**/	": /|"
/**/	": /:";

char * bat_image =
/**/	" | "
/**/	"/ \\";

char * bat_image_2 =
/**/	"\\ /"
/**/	" | ";

char * treasure_image =
/**/	"  '"
/**/	" $ "
/**/	"'  ";

char * treasure_image_2 =
/**/	"'  "
/**/	" $ "
/**/	"  '";

char * key_image =
/**/	"  '"
/**/	"0-+"
/**/	"'  ";

char * key_image_2 =
/**/	"'  "
/**/	"0-+"
/**/	"  '";

char * barrier_image =
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]";

char * rock_image =
/**/	"/'''\\"
/**/	"\\___/";

// Declare a sprite_id called hero
sprite_id hero;

// Declare a sprite_id called zombie
sprite_id zombie;

// Declare a sprite_id called platform
sprite_id platform;
sprite_id platform_2;
sprite_id platform_3;

// Declare a sprite_id called door
sprite_id door;

// Declare a sprite_id called floor
sprite_id floor_plat_1;
sprite_id floor_plat_2;

// Declare a sprite_id called roof_plat
sprite_id roof_plat;

// Declare a sprite_id called treasure
sprite_id treasure;

// Declare a sprite_id called bat
sprite_id bat;

// Declare a sprite_id called key
sprite_id barrier_key;

// Declare a sprite_id called barrier
sprite_id barrier;

// Declare a sprite_id called rock
sprite_id rock;

// Declare a timer_id called my_timer
timer_id my_timer;

void setup_timer() {
	my_timer = create_timer( 1000 );
}

void process_time(){
	if ( timer_expired( my_timer ) ){
		second++;

		if (second == 60){
			minute++;
			second = 0;
		}	
	timer_reset(my_timer);
	}
}

void draw_border( void ) {
	int left = 0;
	int top = 1;
	int right = screen_width() - 1;
	int bottom = screen_height() - 1;
    
    //Draw a line from (left, top) to (right, top)
	//draw_line (left, top, right, top, '-');
	
    // Draw a line from (right, top) to (right, bottom)
	draw_line (right, top + 1, right, bottom - 1, ':');
    // Draw a line from (left, top) to (left, bottom)
	draw_line (left, top + 1, left, bottom - 1, ':');
}

void display_time(int label_offset){
	if (second < 10 && minute < 10){
		draw_formatted(label_offset, 0, "Time: 0%d:0%d", minute, second);
	} else if (second < 10 && minute >= 10){
		draw_formatted(label_offset, 0, "Time: %d:0%d", minute, second);
	} else if (second >= 10 && minute < 10){
		draw_formatted(label_offset, 0, "Time: 0%d:%d", minute, second);
	} else {
		draw_formatted(label_offset, 0, "Time: %2d:%2d", minute, second);
	}
}

void draw_header (void) {
	int label_offset = 1;
	// Draw Time
	display_time(label_offset);
	// Draw Lives
	draw_formatted((screen_width() * 0.25) + label_offset, 0, "Lives: %d", lives);
	// Draw Level
	draw_formatted((screen_width() * 0.50) + label_offset, 0, "Level: %d", level);
	// Draw Score 
	draw_formatted((screen_width() * 0.75) + label_offset, 0, "Score: %d", score);
	/*
	// For debugging 
	draw_formatted((screen_width() * 0.75), 18, "Hero_X: %d", hx);
	draw_formatted((screen_width() * 0.75), 20, "Hero_Y: %d", hy);
	draw_formatted((screen_width() * 0.75), 22, "PLAT_Y: %d", py);
	draw_formatted((screen_width() * 0.75), 24, "Floor_Y: %d", fy);

	if (onGround) {draw_formatted((screen_width() * 0.75), 26, "onGround: True");} else if (!onGround){
		draw_formatted((screen_width() * 0.75), 26, "onGround: False");
	} */
}

void do_game_over(){
	while ( get_char() >= 0 ) {}

		if (lives == 0){
			draw_string(screen_width()/2 - 5, screen_height()/2 + 2, "Game Over!");			
		} else if (level > 5) {
			draw_string(screen_width()/2 - 15, screen_height()/2 + 2, "Congratulations. You finished!");
			draw_formatted (screen_width()/2 - 9, screen_height()/2 + 4, "Your final score: %d", score);
		}
		draw_string(screen_width()/2 - 10, screen_height()/2 + 6, "Press any key to exit!");
		show_screen();
		wait_char();
}

void draw_sprites(void){

	// Draw the hero.
    sprite_draw(hero);

	if (level == 1) {
		// Roof 
		sprite_draw(roof_plat);
	    // Draw the zombie
	    sprite_draw(zombie);
	    // Draw the platform
	    sprite_draw(platform);
	    // Draw the door
		sprite_draw(door);
	    // Draw the platform
	    sprite_draw(floor_plat_1);
	} else if (level == 2){
		// Roof 
		sprite_draw(roof_plat);
	    // Draw the platform
	    sprite_draw(platform);
	    // Draw the floor_1
	    sprite_draw(floor_plat_1);
	   	// Draw the floor_2
	    sprite_draw(floor_plat_2);
	    if (!treasure_taken){
		    // Draw the treasure
		    sprite_draw(treasure);
	    }
	    // Draw the bat
	    sprite_draw(bat);
	    // Draw the door
		sprite_draw(door);	
	} else if (level == 3){
		// Roof 
		sprite_draw(roof_plat);
		// Draw the platform
	    sprite_draw(platform);
	    // Draw the 2nd platform
	    sprite_draw(platform_2);
	    // Draw the floor_1
	    sprite_draw(floor_plat_1);
	   	// Draw the floor_2
	    sprite_draw(floor_plat_2);
	    if (!treasure_taken){
		// Draw the treasure
			sprite_draw(treasure);
	    }
	    // Draw the door
		sprite_draw(door);
	} else if (level == 4){
		// Roof 
		sprite_draw(roof_plat);
		// Draw the floor_1
	    sprite_draw(floor_plat_1);
	    // Draw the platform
	    sprite_draw(platform);
	    // Draw the 2nd platform
	    sprite_draw(platform_2);
	    // Draw the 3rd platform
	    sprite_draw(platform_3);
	    if (!key_taken){
	    	// Draw the key
	    	sprite_draw(barrier_key);
	    }
	    if (!barrier_destroyed){
		   	// Draw the barrier
		    sprite_draw(barrier);
	    }
	    // Draw the door
		sprite_draw(door);
	} else if (level == 5){
		// Roof 
		sprite_draw(roof_plat);
		// Draw the floor_1
	    sprite_draw(floor_plat_1);
	    // Draw the floor_2
	    sprite_draw(floor_plat_2);
	    // Draw the platform
	    sprite_draw(platform);
	    // Draw the 2nd platform
	    sprite_draw(platform_2);
	    // Draw the 3rd platform
	    sprite_draw(platform_3);

	    if (!key_taken){
	    	// Draw the key
	    	sprite_draw(barrier_key);
	    }
	    if (!barrier_destroyed){
		   	// Draw the barrier
		    sprite_draw(barrier);
	    }
	    if (!treasure_taken){
		// Draw the treasure
			sprite_draw(treasure);
	    }
	    // Draw the rock
		sprite_draw(rock);
	    // Draw the door
		sprite_draw(door);
	}
}

void door_exit_animation(void){
	sprite_hide(hero);

	// Clear screen & redraw.
    clear_screen();
    // Draw header
    draw_header();
    // Draw border
    draw_border();
	// Draw sprites
	draw_sprites();

	sprite_set_image(door, door_image_1);
	sprite_draw(door);
	timer_pause(150);
	show_screen();

	sprite_set_image(door, door_image_2);
	sprite_draw(door);
	timer_pause(150);
	show_screen();

	sprite_set_image(door, door_image_3);
	sprite_draw(door);
	timer_pause(150);
	show_screen();
}

// Setup first level of the game.
void setup(void) {
	draw_border();
	draw_header();

	// Create hero sprite
	hero = sprite_create(3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
	// Create zombie sprite
	zombie = sprite_create(screen_width() - 10, screen_height() - 5, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_image);
	// Create floor sprite
	floor_plat_1 = sprite_create(0, screen_height() - 1, FLOOR_WIDTH, FLOOR_HEIGHT, platform_image);
	// Create roof sprite
	roof_plat = sprite_create(0, 1, FLOOR_WIDTH, FLOOR_HEIGHT, roof_image);
	// Create platform sprite
	platform = sprite_create(screen_width() * 0.33, screen_height() - (4 * HERO_HEIGHT) , PLATFORM_WIDTH, PLATFORM_HEIGHT, platform_image);
	// Create door sprite
	door = sprite_create(screen_width() - 10, screen_height() - 5, DOOR_WIDTH, DOOR_HEIGHT, door_image);
	// Set the zombie in motion
	sprite_turn_to( zombie, -0.5, 0 );

	draw_sprites();
}

void zombie_move(sprite_id sprite, int sprite_width){
	// Start zombie movement 
	sprite_step(sprite);

	// Get screen location of zombie.
	int zx = round(sprite_x(sprite));
	//int zy = round(sprite_y(zombie));

	// Get the displacement vector of the zombie.
	double zdx = sprite_dx( sprite );
	double zdy = sprite_dy( sprite );

	// Test to see if the zombie hit the left or right border.
	if ( zx <= 0 ) {
		zdx = fabs(zdx);
	} 
	else if ( zx >= screen_width() - sprite_width ) {
		zdx = -fabs(zdx);
	}

	// Test to see if the zombie needs to step back and change direction.
	if ( zdx != sprite_dx( sprite ) || zdy != sprite_dy(sprite) ) {
		sprite_back( sprite );
		sprite_turn_to( sprite, zdx, zdy );
	}

	// End zombie movement
}

void change_bat_image(){
	if (swap_bat_image){
		sprite_set_image(bat, bat_image);
		swap_bat_image = false;
	} else {
		sprite_set_image(bat, bat_image_2);
		swap_bat_image = true;
	}
}

void change_key_image(){
	if (swap_key_image){
		sprite_set_image(barrier_key, key_image);
		swap_key_image = false;
	} else {
		sprite_set_image(barrier_key, key_image_2);
		swap_key_image = true;
	}
}

void change_treasure_image(){
	if (swap_treasure_image){
		sprite_set_image(treasure, treasure_image);
		swap_treasure_image = false;
	} else {
		sprite_set_image(treasure, treasure_image_2);
		swap_treasure_image = true;
	}
}

void rock_move(void){
	// Start rock movement 
	sprite_step(rock);

	// Get screen location of rock.
	int zx = round(sprite_x(rock));
	//int zy = round(sprite_y(rock));
	
	if (!rock_onGround) {
		rock_dy = rock_dy + 0.02;
		//timer_pause(1000);
	} else if (rock_onGround) {
		rock_dy = 0;
	} 

	// Test to see if the rock hit the left or right border.
	if ( zx <= 0 ) {
		rock_dx = fabs(rock_dx);
	} 
	else if ( zx >= screen_width() - ROCK_WIDTH ) {
		rock_dx = -fabs(rock_dx);
	}

	// Test to see if the rock needs to step back and change direction.
	if ( rock_dx != sprite_dx( rock ) || rock_dy != sprite_dy(rock) ) {
		sprite_back( rock );
		sprite_turn_to( rock, rock_dx, rock_dy );
	}

	sprite_turn_to (rock, rock_dx, rock_dy);
	sprite_step(rock);
	// End rock movement
}

void rock_on_ground(void){
	hx = round(sprite_x(rock)); 
	hy = round(sprite_y(rock));

	if (level == 5){
		px = round(sprite_x(platform)); 
		px_2 = round(sprite_x(platform_2)); 
		px_3 = round(sprite_x(platform_3)); 

		if (hy >= screen_height() - ROCK_HEIGHT - 1 && (hx <= FLOOR_WIDTH * 0.78 || hx >= FLOOR_WIDTH * 0.89)){ // GROUND
			rock_onGround = true;
		} else if (hy == sprite_y(platform) - ROCK_HEIGHT && hx >= px && hx <= px + (screen_width() * 0.6)){
			rock_onGround = true;
		} else if (hy == sprite_y(platform_2) - ROCK_HEIGHT && hx >= px_2 && hx <= px_2 + (screen_width() * 0.6)){
			rock_onGround = true;
		} else if (hy == sprite_y(platform_3) - ROCK_HEIGHT && hx >= px_3 && hx <= px_3 + (screen_width() * 0.6)){
			rock_onGround = true;
		}else {
			rock_onGround = false;
		}
	}
}

void sprite_on_ground(){

	hx = round(sprite_x(hero)); 
	hy = round(sprite_y(hero));
	//fx = round(sprite_x(floor_plat)); 
	if (level == 1){
		fy = round(sprite_y(floor_plat_1));

		px = round(sprite_x(platform)); 
		py = round(sprite_y(platform));

		if (hy >= screen_height() - HERO_HEIGHT - 1){
			onGround = true;
		} else if (hy == sprite_y(platform) - HERO_HEIGHT && hx >= px && hx <= px + PLATFORM_WIDTH){
			onGround = true;
		} else {
			onGround = false;
		}
	} else if (level == 2){
		px = round(sprite_x(platform)); 
		fy = round(sprite_y(floor_plat_1));

		if (hy >= screen_height() - HERO_HEIGHT - 1 && (hx <= (screen_width() * 0.25) || hx >= (screen_width() * 0.75))){
			onGround = true;
		} else if (hy == sprite_y(platform) - HERO_HEIGHT && hx >= px && hx <= px + PLATFORM_WIDTH){
			onGround = true;
		} else {
			onGround = false;
		}
	} else if (level == 3){
		px = round(sprite_x(platform)); 
		px_2 = round(sprite_x(platform_2)); 
		fy = round(sprite_y(floor_plat_1));

		if (hy >= screen_height() - HERO_HEIGHT - 1 && (hx <= (screen_width() * 0.25) || hx >= (screen_width() * 0.75))){
			onGround = true;
		} else if (hy == sprite_y(platform) - HERO_HEIGHT && hx >= px && hx <= px + PLATFORM_WIDTH){ // PLATFORM 1
			onGround = true;
		} else if (hy == sprite_y(platform_2) - HERO_HEIGHT && hx >= px_2 && hx <= px_2 + 12){ // PLATFORM 2
			onGround = true;
		}else {
			onGround = false;
		}
	} else if (level == 4){
		px = round(sprite_x(platform)); 
		px_2 = round(sprite_x(platform_2)); 
		px_3 = round(sprite_x(platform_3)); 

		if (hy >= screen_height() - HERO_HEIGHT - 1){ // GROUND
			onGround = true;
		} else if (hy == sprite_y(platform) - HERO_HEIGHT && hx >= px && hx <= px + (screen_width() * 0.6)){
			onGround = true;
		} else if (hy == sprite_y(platform_2) - HERO_HEIGHT && hx >= px_2 && hx <= px_2 + (screen_width() * 0.6)){
			onGround = true;
		} else if (hy == sprite_y(platform_3) - HERO_HEIGHT && hx >= px_3 && hx <= px_3 + (screen_width() * 0.6)){
			onGround = true;
		}else {
			onGround = false;
		}
	} else if (level == 5){
		px = round(sprite_x(platform)); 
		px_2 = round(sprite_x(platform_2)); 
		px_3 = round(sprite_x(platform_3)); 

		if (hy >= screen_height() - HERO_HEIGHT - 1 && (hx <= FLOOR_WIDTH * 0.78 || hx >= FLOOR_WIDTH * 0.89)){ // GROUND
			onGround = true;
		} else if (hy == sprite_y(platform) - HERO_HEIGHT && hx >= px && hx <= px + (screen_width() * 0.6)){
			onGround = true;
		} else if (hy == sprite_y(platform_2) - HERO_HEIGHT && hx >= px_2 && hx <= px_2 + (screen_width() * 0.6)){
			onGround = true;
		} else if (hy == sprite_y(platform_3) - HERO_HEIGHT && hx >= px_3 && hx <= px_3 + (screen_width() * 0.6)){
			onGround = true;
		}else {
			onGround = false;
		}

		//rx = round(sprite_x(rock)); 
		//ry = round(sprite_y(rock));

	}

	// Inserts to prevent first key press bug
	if (hero_dy < 0){
		onGround = false;
	}
}

void hero_move(int key){

	// Get the current screen coordinates of the hero in integer variables 
	// by rounding the actual coordinates.
	int hx = round(sprite_x(hero));

	// If hero is not on the ground - start applying gravity
	if (!onGround){
		hero_dy = hero_dy + 0.008;	
		sprite_set_image( hero ,hero_image_jump);
	} // When hero lands - need to set the dy to 0 to stop him falling anymore
	else if (onGround){
		hero_dy = 0; 
		sprite_set_image( hero ,hero_image);
	}

	// Adjusts hero speed based on key press with maximum allowed
	if (key == 261 && onGround){ // RIGHT
		if (hero_dx < 1 && hx < screen_width() - 1 - HERO_WIDTH){
			hero_dx = hero_dx + 0.5;
		}			
	} else if (key == 260 && hx > 1 && onGround){ // LEFT
		if (hero_dx > -1){
			hero_dx = hero_dx - 0.5;
		}		
	} else if (key == 259 && onGround == true){ // JUMP
		hero_dy = -0.45;
	} else if (hx <= 1 || hx >= screen_width() - 1 - HERO_WIDTH){ // PREVENT LEFT/RIGHT OUT OF BOUNDS
		hero_dx = 0;
	} 

	// Moves hero according to input speed set
	sprite_turn_to(hero, hero_dx, hero_dy);
	sprite_step(hero);
}

void platform_collision( sprite_id who_moved , sprite_id which_platform, int platform_w, int platform_h){
	
	// Get platform and hero screen locations.
	int px = round(sprite_x(which_platform)), py = round(sprite_y(which_platform));
	int hx = round(sprite_x(hero)), hy = round(sprite_y(hero));

	// Check for collision
	bool collided = true;

	if ( px >= hx + HERO_WIDTH ) collided = false;
	if ( py >= hy + HERO_HEIGHT ) collided = false;
	if ( hx >= px + platform_w ) collided = false;
	if ( hy >= py + platform_h ) collided = false;

	if (collided){
		if ( who_moved == hero ) {

			// Breaking through the barrier
			if ((level == 4 || level == 5) && !barrier_active && hx + HERO_WIDTH - 1 == px && hero_dx > 0 && which_platform == barrier){
				//sprite_destroy(barrier_key);
				sprite_move_to(barrier, -100, -100);
				barrier_destroyed = true;

			}
			// (e.b) If hero has struck the platform from below
			// negate dy but leave dx alone.
			else if ( hy == py + platform_h - 1 && hero_dy < 0 ) {
				hero_dy = -hero_dy;
			}

			// (e.c) Otherwise, if the hero has bumped into the left end
			// then quench horizontal velocity. 
			else if ( hx + HERO_WIDTH - 1 == px && hero_dx > 0 ) {
				hero_dx = 0;
			}	

			// (e.d) Otherwise, if the hero has bumped into the right end
			// then quench horizontal velocity.
			else if ( hx == px + platform_w - 1 && hero_dx < 0 ) {
				hero_dx = 0;
			}	

			// (e.e) Otherwise, we landed on the top, so we quench the 
			// vertical velocity.
			else {
				hero_dy = 0;
			}

		sprite_back( hero );
		sprite_turn_to( hero, hero_dx, hero_dy );
		} else {
			sprite_move_to( hero, sprite_x(hero), sprite_y(which_platform) + platform_h );
		}
	}
}

bool sprite_collison(sprite_id sprite1, sprite_id sprite2, int sprite1_width, int sprite1_height, int sprite2_width, int sprite2_height){
	bool collided = true;
	
	int hx = round(sprite_x(sprite1));
	int hy = round(sprite_y(sprite1));
	int hr = hx + sprite1_width - 1;
	int hb = hy + sprite1_height - 1;
	
	int zx = round(sprite_x(sprite2));
	int zy = round(sprite_y(sprite2));
	int zr = zx + sprite2_width - 1;
	int zb = zy + sprite2_height - 1;
	
	if (hr < zx ) { collided = false; }
	if (hb < zy ) { collided = false; }
	
	if (zr < hx ) { collided = false; }
	if (zb < hy ) { collided = false; }
	
	return collided;
}

void reset_level(){
	if (level == 1){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// ZOMBIE
		sprite_init(zombie, screen_width() - 10, screen_height() - 5, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_image);
		// Set the zombie in motion
		sprite_turn_to( zombie, -0.5, 0 );
	} else if (level == 2){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		sprite_init(floor_plat_2, screen_width() * 0.75, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);
		// TREASURE
		sprite_init(treasure, screen_width() * 0.5, screen_height() * 0.55, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		// Create bat sprite
		sprite_init(bat, screen_width() - 10, screen_height() - 5, BAT_WIDTH, BAT_HEIGHT, bat_image);
		sprite_turn_to( bat, -0.5, 0 );
	} else if (level == 3){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		sprite_init(floor_plat_2,screen_width() * 0.75, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);	
		// Create platform sprite
		sprite_init(platform, screen_width() - PLATFORM_WIDTH - 1, screen_height() - (4 * HERO_HEIGHT) , screen_width() * 0.4, PLATFORM_HEIGHT, platform_image);
		sprite_turn_to( platform, -0.5, 0 );
		// Create 2nd platform
		sprite_init(platform_2, screen_width() * 0.5 - 6, screen_height() - HERO_HEIGHT * 8, 12, PLATFORM_HEIGHT, platform_image);
		// TREASURE
		sprite_init(treasure, screen_width() * 0.93 , screen_height() * 0.90, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		// DOOR
		sprite_init(door, (screen_width() * 0.5 - 6) + 4, (screen_height() - HERO_HEIGHT * 8) - DOOR_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT, door_image);
	} else if (level == 5){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, FLOOR_WIDTH * 0.78, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		sprite_init(floor_plat_2, screen_width() * 0.89, screen_height() - 1, screen_width() * 0.11, FLOOR_HEIGHT, platform_image);	
		// PLATFORM 1
		sprite_init(platform, screen_width() * 0.4 - 1, screen_height() - (4 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// PLATFORM 2
		sprite_init(platform_2, 1, (screen_height() - (8 * HERO_HEIGHT)) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		sprite_turn_to( platform_2, -0.5, 0 );
		// PLATFORM 3
		sprite_init(platform_3, screen_width() * 0.4 - 1, screen_height() - (12 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// BARRIER
		sprite_init(barrier, screen_width() * 0.7, round(sprite_y(platform)) + 1, 2, round(sprite_y(platform)) - round(sprite_y(platform_2)) - 2, barrier_image);
		barrier_active = true;
		barrier_destroyed = false;
		// KEY
		barrier_key = sprite_create(screen_width() * 0.93 - 1 , screen_height() * 0.1, KEY_WIDTH, KEY_HEIGHT, key_image);
		key_taken = false;
		key_active = true;
		// TREASURE
		sprite_init(treasure, screen_width() * 0.5 , (screen_height() - (11 * HERO_HEIGHT)) , TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		//
		rock = sprite_create(screen_width() * 0.8, screen_height() * 0.1,  ROCK_WIDTH, ROCK_HEIGHT, rock_image);
		sprite_turn_to( rock, -0.5, 0 );
	}
}

void start_level(void){
	if (level == 2) {
		// Create bat sprite
		bat = sprite_create(screen_width() - 10, screen_height() - 5, BAT_WIDTH, BAT_HEIGHT, bat_image);
		sprite_turn_to( bat, -0.5, 0 );
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		floor_plat_2 = sprite_create(screen_width() * 0.75, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);	
		// TREASURE
		treasure = sprite_create(screen_width() * 0.5, screen_height() * 0.55, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		treasure_taken = false;
		// DOOR
		sprite_init(door, screen_width() - 10, screen_height() - 5, DOOR_WIDTH, DOOR_HEIGHT, door_image);
	} else if (level == 3){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		sprite_init(floor_plat_2,screen_width() * 0.75, screen_height() - 1, screen_width() / 4, FLOOR_HEIGHT, platform_image);	
		// Create platform sprite
		sprite_init(platform, screen_width() - PLATFORM_WIDTH - 1, screen_height() - (4 * HERO_HEIGHT) , screen_width() * 0.4, PLATFORM_HEIGHT, platform_image);
		sprite_turn_to( platform, -0.5, 0 );
		// Create 2nd platform
		platform_2 = sprite_create((screen_width() * 0.5) - 6, screen_height() - (HERO_HEIGHT * 8), 12, PLATFORM_HEIGHT, platform_image);
		// TREASURE
		treasure = sprite_create(screen_width() * 0.93 , screen_height() * 0.90, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		treasure_taken = false;
		treasure_active = true;
		// DOOR
		sprite_init(door, (screen_width() * 0.5 - 6) + 4, (screen_height() - HERO_HEIGHT * 8) - DOOR_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT, door_image);
	} else if (level == 4){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, FLOOR_WIDTH, FLOOR_HEIGHT, platform_image);
		// PLATFORM 1
		sprite_init(platform, screen_width() * 0.4 - 1, screen_height() - (4 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// PLATFORM 2
		sprite_init(platform_2, 1, (screen_height() - (8 * HERO_HEIGHT)) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// PLATFORM 3
		platform_3 = sprite_create(screen_width() * 0.4 - 1, screen_height() - (12 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// KEY
		barrier_key = sprite_create(screen_width() * 0.93 - 1, screen_height() * 0.1 , KEY_WIDTH, KEY_HEIGHT, key_image);
		// DOOR
		sprite_init(door, screen_width() - 10, screen_height() - 5, DOOR_WIDTH, DOOR_HEIGHT, door_image);
		// BARRIER
		barrier = sprite_create(screen_width() * 0.7, round(sprite_y(platform)) + 1, 2, round(sprite_y(platform)) - round(sprite_y(platform_2)) - 2, barrier_image);
	} else if (level == 5){
		// HERO
		sprite_init(hero, 3, screen_height() - 4, HERO_WIDTH, HERO_HEIGHT, hero_image);
		// FLOOR PLAT 1
		sprite_init(floor_plat_1, 0, screen_height() - 1, FLOOR_WIDTH * 0.78, FLOOR_HEIGHT, platform_image);
		// FLOOR PLAT 2
		sprite_init(floor_plat_2, screen_width() * 0.89, screen_height() - 1, screen_width() * 0.11, FLOOR_HEIGHT, platform_image);	
		// PLATFORM 1
		sprite_init(platform, screen_width() * 0.4 - 1, screen_height() - (4 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// PLATFORM 2
		sprite_init(platform_2, 1, (screen_height() - (8 * HERO_HEIGHT)) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		sprite_turn_to( platform_2, -0.5, 0 );
		// PLATFORM 3
		sprite_init(platform_3, screen_width() * 0.4 - 1, screen_height() - (12 * HERO_HEIGHT) , screen_width() * 0.6, PLATFORM_HEIGHT, platform_image);
		// BARRIER
		sprite_init(barrier, screen_width() * 0.7, round(sprite_y(platform)) + 1, 2, round(sprite_y(platform)) - round(sprite_y(platform_2)) - 2, barrier_image);
		barrier_active = true;
		barrier_destroyed = false;
		// KEY
		barrier_key = sprite_create(screen_width() * 0.93 - 1 , screen_height() * 0.1, KEY_WIDTH, KEY_HEIGHT, key_image);
		key_taken = false;
		key_active = true;
		// DOOR
		sprite_init(door, screen_width() - 10, screen_height() - 5, DOOR_WIDTH, DOOR_HEIGHT, door_image);
		// TREASURE
		treasure = sprite_create(screen_width() * 0.5 , (screen_height() - (11 * HERO_HEIGHT)) , TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);
		treasure_taken = false;
		treasure_active = true;
		//
		rock = sprite_create(screen_width() * 0.8, screen_height() * 0.1,  ROCK_WIDTH, ROCK_HEIGHT, rock_image);
		sprite_turn_to( rock, -0.5, 0 );
	} else if (level == 6){
		game_over = true;
	}
}

// Play one turn of game.
void process(void) {
	process_time();
	// Get a character code from standard input without waiting.
	int key = get_char();

	if (level == 1){
		zombie_move(zombie, ZOMBIE_WIDTH);	
		// Process hero left/right movement
		hero_move(key);
		// Check if the hero is on the ground
		sprite_on_ground();
		// Deal with hero and the platform collision.
		platform_collision(hero, platform, PLATFORM_WIDTH, PLATFORM_HEIGHT);		
		// RESET LEVEL UPON DEATH
		if (sprite_collison(hero, zombie, HERO_WIDTH, HERO_HEIGHT, ZOMBIE_WIDTH, ZOMBIE_HEIGHT) == true){
			lives--;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			reset_level();
		} 
		// MOVE TO NEXT LEVEL
		if (sprite_collison(hero, door, HERO_WIDTH, HERO_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT)){		
			door_exit_animation();
			// Add score and move to next level 
			score += 100;
			level++;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			start_level();
		}
	} else if (level == 2){
		// Process bat movement
		zombie_move(bat, BAT_WIDTH);
		change_bat_image();
		change_treasure_image();
		// Process hero left/right movement
		hero_move(key);
		// Check if the hero is on the ground
		sprite_on_ground();
		// RESET LEVEL UPON DEATH
		if (sprite_collison(hero, bat, BAT_WIDTH, BAT_HEIGHT, ZOMBIE_WIDTH, ZOMBIE_HEIGHT)){
			lives--;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			reset_level();
		}
		
		// GETS THE TREASURE
		if (sprite_collison(hero, treasure, HERO_WIDTH, HERO_HEIGHT, TREASURE_WIDTH, TREASURE_HEIGHT)){			

			treasure_taken = true;
			if (treasure_active){
				sprite_destroy(treasure);
				treasure_active = false;
				// Add score
				score = score + 50; 
			}	
		}
		// Check if hero collided with platform
		platform_collision(hero, platform, PLATFORM_WIDTH, PLATFORM_HEIGHT);
		
		// MOVE TO NEXT LEVEL
		if (sprite_collison(hero, door, HERO_WIDTH, HERO_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT)){
			door_exit_animation();		
			// Add score and move to next level 
			score += 100;
			level++;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			start_level();
		}
		// HERO FALLS INTO PIT
		hy = round(sprite_y(hero));
		if (hy > screen_height()){
			lives--;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			reset_level();
		}
	} else if (level == 3){
		// Treasure animation
		change_treasure_image();
		// Process platform movement
		zombie_move(platform, screen_width() * 0.4);
		// Check if platform collided with hero
		platform_collision(platform, platform, PLATFORM_WIDTH, PLATFORM_HEIGHT);

		// Process hero left/right movement
		hero_move(key);
		// Check if the hero is on the ground
		sprite_on_ground();

		// Check if hero collided with platform
		platform_collision(hero, platform, PLATFORM_WIDTH, PLATFORM_HEIGHT);

		// Check if hero collided with platform
		platform_collision(hero, platform_2, 12, PLATFORM_HEIGHT);
		
		// GETS THE TREASURE
		if (sprite_collison(hero, treasure, HERO_WIDTH, HERO_HEIGHT, TREASURE_WIDTH, TREASURE_HEIGHT)){			

			treasure_taken = true;
			if (treasure_active){
				sprite_destroy(treasure);
				treasure_active = false;
				// Add score
				score = score + 50; 
			}	
		}
		// HERO FALLS INTO PIT
		if (hy > screen_height()){
			lives--;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			reset_level();
		}

		// MOVE TO NEXT LEVEL
		if (sprite_collison(hero, door, HERO_WIDTH, HERO_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT)){	
			door_exit_animation();		
			// Add score and move to next level 
			score += 100;
			level++;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			start_level();
		} 
	} else if (level == 4){
		// Process hero left/right movement
		hero_move(key);
		// Key animation
		change_key_image();

		// Check if the hero is on the ground
		sprite_on_ground();

		// Check if hero collided with roof
		platform_collision(hero, roof_plat, screen_width() - 1, 1);

		// Check if hero collided with barrier
		platform_collision(hero, barrier, 2, round(sprite_y(platform)) - round(sprite_y(platform_2)) - 2);

		// Check if hero collided with platform 1
		platform_collision(hero, platform, screen_width() * 0.6, PLATFORM_HEIGHT);

		// Check if hero collided with platform 2
		platform_collision(hero, platform_2, screen_width() * 0.6, PLATFORM_HEIGHT);

		// Check if hero collided with platform 3
		platform_collision(hero, platform_3, screen_width() * 0.6, PLATFORM_HEIGHT);

		// IF HERO GETS THE KEY
		if (sprite_collison(hero, barrier_key, HERO_WIDTH, HERO_HEIGHT, KEY_WIDTH, KEY_HEIGHT)){			

			key_taken = true;
			if (key_active){
				sprite_destroy(barrier_key);
				key_active = false;
				barrier_active = false;
			}	
		}

		// MOVE TO NEXT LEVEL
		if (sprite_collison(hero, door, HERO_WIDTH, HERO_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT)){	
			door_exit_animation();	
			// Add score and move to next level 
			score += 100;
			level++;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			start_level();
		}
	} else if (level == 5){
		// Treasure animation
		change_treasure_image();
		// Key animation
		change_key_image();
		// Process platform movement
		zombie_move(platform_2, screen_width() * 0.6);
		// Check if platform 2 collided with hero
		platform_collision(platform_2, platform_2, screen_width() * 0.6, PLATFORM_HEIGHT);
		// Process hero left/right movement
		hero_move(key);
		// Process rock movement
		rock_move();

		// Check if the hero is on the ground
		sprite_on_ground();
		rock_on_ground();

		// Check if hero collided with roof
		platform_collision(hero, roof_plat, screen_width() - 1, 1);

		// Check if hero collided with barrier
		platform_collision(hero, barrier, 2, round(sprite_y(platform)) - round(sprite_y(platform_2)) - 2);

		// Check if hero collided with platform 1
		platform_collision(hero, platform, screen_width() * 0.6, PLATFORM_HEIGHT);

		// Check if hero collided with platform 2
		platform_collision(hero, platform_2, screen_width() * 0.6, PLATFORM_HEIGHT);

		// Check if hero collided with platform 3
		platform_collision(hero, platform_3, screen_width() * 0.6, PLATFORM_HEIGHT);

		// GETS THE TREASURE
		if (sprite_collison(hero, treasure, HERO_WIDTH, HERO_HEIGHT, TREASURE_WIDTH, TREASURE_HEIGHT)){			

			treasure_taken = true;
			if (treasure_active){
				sprite_move_to(treasure, -100, -100);
				treasure_active = false;
				// Add score
				score = score + 50; 
			}	
		}

		// IF HERO GETS THE KEY
		if (sprite_collison(hero, barrier_key, HERO_WIDTH, HERO_HEIGHT, KEY_WIDTH, KEY_HEIGHT)){			

			key_taken = true;
			if (key_active){
				sprite_destroy(barrier_key);
				key_active = false;
				barrier_active = false;
			}	
		}
		
		// IF HERO GETS THE HIT BY THE ROCK
		if (sprite_collison(hero, rock, HERO_WIDTH, HERO_HEIGHT, ROCK_WIDTH, ROCK_HEIGHT)){			
			lives--;
			reset_level();
			hero_dx = 0;
			hero_dy = 0;
		}  

		// HERO FALLS INTO PIT
		hy = round(sprite_y(hero));
		if (hy > screen_height() - 2){
			lives--;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			reset_level();
		}

		// MOVE TO NEXT LEVEL
		if (sprite_collison(hero, door, HERO_WIDTH, HERO_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT)){		
			// Add score and move to next level 
			score += 100;
			level++;
			// Reset movement speed of hero to 0
			hero_dx = 0;
			hero_dy = 0;
			clear_screen();
			start_level();
		}

		ry = round(sprite_y(rock));
		// IF ROCK HAS FALLED OFF THE BOTTOM
		if (ry > screen_height() - 1){
			rock_dx = -rock_dx;
			sprite_move_to(rock, screen_width() * 0.8, screen_height() * 0.1);
		}
	} 
	
	if (lives == 0){
		game_over = true;
	}
	
	// For all levels
	if ( key == 'l'){
		level++;

		start_level();
	}

	// Clear screen & redraw.
    clear_screen();
    // Draw header
    draw_header();
    // Draw border
    draw_border();
	// Draw sprites
	draw_sprites();
}

int main( void ) {

	setup_timer();

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

	if (game_over){
		clear_screen();
		do_game_over();
	}

	return 0;
}