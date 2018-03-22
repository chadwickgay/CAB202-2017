#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <string.h>
#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"
#include <graphics.h>
#include <sprite.h>
#include "usb_serial.h"
#include <math.h>
#include <stdbool.h>
#include "cab202_adc.h"

void setup_usb_serial( void );
void usb_serial_send(char * message);
void srand ( unsigned int seed );
int rand(void);


#define FREQ     (8000000.0)
#define PRESCALE (1024.0)

#define sprite_step(sprite) (sprite.x += sprite.dx, sprite.y += sprite.dy)
#define sprite_turn_to(sprite,_dx,_dy) (sprite.dx = _dx, sprite.dy = _dy)

#define PI 3.141592653589
#define BIT_ON(port, pin) port |= (1 << pin)
#define GET_BIT(port, pin) (port >> pin) & 1


char *name = "Chadwick Gay";
char *student_no = "n9410392";
bool pause_menu = false;
bool start_screen = true;
bool start_countdown = false;
int countdown_counter = 3;
int score = 0, lives = 3, floor_num = 0, seconds = 0, minutes = 0;
int system_seconds = 0, system_minutes = 0;

bool attach_key = false;
bool attach_shield = false;
bool attach_bow = false;
bool attach_bomb = false;
bool item_held = false;

bool left = true, right = true, up = true, down = true;
bool start_game = false;
bool end_game = false;
bool load_screen = false;

bool shield_active = false;
bool shield_exist = false;

bool bow_exist = false;
bool bomb_exist = false;
bool bomb_timer_start = false;
int bomb_timer = 2000;
bool monster_follow;

volatile int overflow_counter = 0;
volatile int system_overflow_counter = 0;

int tower_x, tower_y;
int hero_x, hero_y;
int key_x, key_y;
int monster_x, monster_y;

int num_treasure;

double random_y = 0; 
double random_x = 20; 

int games_played = 0;

Sprite hero;
Sprite key;
Sprite tower;
Sprite door;
Sprite monster;
Sprite treasure;
Sprite shield;
Sprite bow;
Sprite bomb;

Sprite treasure_array[5];


double hero_dx = 0;
double hero_dy = 0;

char buffer[30];

uint8_t hero_bitmap[] = {
	0b00101000,
	0b00111000,
	0b00101000,
	0b00111000,
	0b00000000,
	0b11111110,
	0b10101010,
	0b10111010,
	0b00101000,
	0b00101000,
	0b00101000,
};

uint8_t tower_bitmap[] = {
	0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b11110000, 0b00111100, 0b00001111, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001,
	0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001,
	0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 
};

uint8_t key_bitmap[] = 
    {
     0b11100000,
	 0b10111110,
	 0b11101010
};

uint8_t door_bitmap[] = {
	0b00111100,
	0b01000010,
	0b10000001,
	0b10000001,
	0b10000001,
	0b10001101,
	0b10001101,
	0b10000001,
	0b10000001,
	0b10100101,
};

uint8_t monster_bitmap[] = 
    {
     0b01110000,
	 0b11111000,
	 0b10101000,
	 0b11111000,
	 0b11111000,
	 0b10101000,
};

uint8_t treasure_bitmap[] = 
    {
     0b00100000,
	 0b01110000,
	 0b11111000,
};

uint8_t shield_bitmap[] = 
    {
     0b11111110,
	 0b01111100,
	 0b01111000,
	 0b00110000,
};

uint8_t bow_bitmap[] = 
    {
     0b11000000,
	 0b10100000,
	 0b10100000,
	 0b10100000,
	 0b11000000,

};

uint8_t bomb_bitmap[] = 
    {
     0b11111110,
	 0b10101010,
	 0b10101010,
	 0b10101010,
	 0b11111110,

};

void setup(void) ;


// -------------------------------------------------
// Helper functions.
// -------------------------------------------------
void draw_int(uint8_t x, uint8_t y, int value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%d", value);
	draw_string(x, y, buffer, colour);
}
// -------------------------------------------------
void log_console(void){	

	if (start_game){
		usb_serial_send( "----------------" );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Score: %d", score);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Floor: %d", floor_num);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Hero X: %d", hero_x);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Hero Y: %d", hero_y);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Lives: %d", lives);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );

		sprintf(buffer, "Time: %d : %d", system_minutes, system_seconds);
		usb_serial_send( buffer );
		usb_serial_send( "\r\n" );
	}

	

	/*
	sprintf(buffer, "Tower X: %d", tower_x);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );

	sprintf(buffer, "Tower Y: %d", tower_y);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );

	sprintf(buffer, "Monster X: %d", monster_x);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );

	sprintf(buffer, "Monster Y: %d", monster_y);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );
	
	sprintf(buffer, "Key X: %d", key_x);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );

	sprintf(buffer, "key Y: %d", key_y);
	usb_serial_send( buffer );
	usb_serial_send( "\r\n" );
	*/



}

ISR(TIMER3_OVF_vect) {
		overflow_counter ++;
		system_overflow_counter++;
		log_console();
}

void draw_startup(void){
	if (start_screen){
		draw_string(10, 15 / 2, name, FG_COLOUR);
		draw_string(17, 35 / 2, student_no, FG_COLOUR);
	}	
}

void draw_pause_menu(void){
	
	if ( BIT_IS_SET(PINB, 0)){
		clear_screen();
		pause_menu = true;

		draw_string(5, 5, "Score:", FG_COLOUR);
		draw_int(40, 5, score, FG_COLOUR);

		draw_string(5, 15, "Lives:", FG_COLOUR);
		draw_int(40, 15, lives, FG_COLOUR);

		draw_string(5, 25, "Floor:", FG_COLOUR);
		draw_int(40, 25, floor_num, FG_COLOUR);
		
		draw_string(5, 35, "Time :", FG_COLOUR);
		if (minutes < 10){
			draw_string(40, 35, "0", FG_COLOUR);
		}
		draw_int(46, 35, minutes, FG_COLOUR);
		draw_string(51, 35, ":", FG_COLOUR);
		if (seconds < 10){
			draw_string(56, 35, "0", FG_COLOUR);
			draw_int(61, 35, seconds, FG_COLOUR);	
		} else {
			draw_int(56, 35, seconds, FG_COLOUR);	
		}
		
	} else {
		pause_menu = false;
	}
}

void draw_load_screen(void){
	if (load_screen){
		clear_screen();
		draw_string(15, 25, "Floor:", FG_COLOUR);
		draw_int(50, 25, floor_num, FG_COLOUR);
		draw_string(15, 5, "Score:", FG_COLOUR);
		draw_int(50,5, score, FG_COLOUR);
		show_screen();
		_delay_ms(2000);
		load_screen = false;
	} 
}

void setup_switches(void){
	// Input from the left and right buttons
	CLEAR_BIT(DDRF, 5); // SW3
	CLEAR_BIT(DDRF, 6); // SW2
	CLEAR_BIT(DDRB, 1); // LEFT
    CLEAR_BIT(DDRD, 0); // RIGHT
    CLEAR_BIT(DDRD, 1); // UP
    CLEAR_BIT(DDRB, 7); // DOWN
    CLEAR_BIT(DDRB, 0); // CENTRE

    // LEDS
    // Enable LEDS 0 and 1
	SET_BIT(DDRB, 2);
	SET_BIT(DDRB, 3);
}

void check_start(void){
	if ( BIT_IS_SET(PINF, 6) || ( BIT_IS_SET(PINF, 5))){
		int seed=0;
		while( BIT_IS_SET(PINF, 6) || ( BIT_IS_SET(PINF, 5))){
			seed++;
		}
		srand(seed);

		start_countdown = true;
	}
}

void setup_timer(void){
	// Timer 3
	TCCR3A = 0;
	TCCR3B = 3;
	TIMSK3 = 1;
	// Enable timer overflow, and turn on interrupts.
	sei();
}

void countdown_timer(void){
	if (countdown_counter == 0){
		start_screen = false;
		start_countdown = false;
		floor_num = 1;
		start_game = true;
	}

	clear_screen();
	draw_int(LCD_X / 2, (LCD_Y / 2) - 2, countdown_counter, FG_COLOUR);
	_delay_ms(330);
	countdown_counter--;
}

void process_time(void){
	seconds = ( overflow_counter * 65536.0 + TCNT3)  * 64.0 / 8e6;

	if (seconds == 60){
		minutes++;
		seconds = 0;
		overflow_counter = 0;
	}

	system_seconds = ( system_overflow_counter * 65536.0 + TCNT3)  * 64.0 / 8e6;
	
	if (system_seconds >= 60){
		system_minutes = system_minutes + 1;
		system_seconds = 0;
		system_overflow_counter = 0;
	}
}

void draw_sprites(void){
	if (floor_num == 1 && !pause_menu){
		sprite_draw(&hero);
		sprite_draw(&key);
		sprite_draw(&tower);
		sprite_draw(&door);
		sprite_draw(&monster);
	} else if (floor_num == 2 && !pause_menu){
		sprite_draw(&hero);
		sprite_draw(&door);
		sprite_draw(&key);
		sprite_draw(&monster);
		sprite_draw(&treasure);
	} else if (floor_num > 2 && !pause_menu){
		sprite_draw(&hero);
		sprite_draw(&door);
		sprite_draw(&key);
		if (monster.is_visible){
			sprite_draw(&monster);
		} 
		if (shield_exist && shield.is_visible){
			sprite_draw(&shield);
		}
		if (bow_exist){
			sprite_draw(&bow);
		}
		if (bomb_exist){
			sprite_draw(&bomb);
		}
		for (int i = 0; i < num_treasure; i++){
			sprite_draw(&treasure_array[i]);
		}
	}
}

void boundary_zone(void){
	if (floor_num > 0 && !pause_menu) {

		if (random_x > 21 ) {
			for ( int i = 0; i < random_x - 21; i++) {
				draw_line(i, 0, i, 48, FG_COLOUR);
			}
		}
		if (random_x < -21) {
			for ( int i = 0; i > random_x + 21; i--) {
				draw_line(i+84, 0, i+84, 48, FG_COLOUR);
			}
		}
		if (random_y < -12) {
			for (int i = 0; i > random_y + 12; i--) {
				draw_line(0, i + 48, 84, i + 48, FG_COLOUR);
			}
		}
		if (random_y > 12) {
			for (int i = 0; i < random_y - 12; i++) {
				draw_line(0, i, 84, i, FG_COLOUR);
			}
		}
	}
}

void hero_move(void){
	double movement_speed = 0.5;

	if (BIT_IS_SET(PIND, 1) && up) { // UP
		if (random_y <= 35) {
			hero_dy = movement_speed;
			random_y += 0.5;
		} else {
			hero_dy = 0;
		}
	}

	//  if the Down joystick switch 
	else if (BIT_IS_SET(PINB, 7) && down){ // DOWN
		if (random_y > - 26) {
			hero_dy = -movement_speed;
			random_y -= 0.5;
		} else {
			hero_dy = 0;
		}
	}

	//  if the Left joystick switch 
	else if (BIT_IS_SET(PINB, 1) && left){ // LEFT
		if (random_x < 58) {
			random_x += 0.5;
			hero_dx = +movement_speed;
		} else {
			hero_dx = 0;
		}
	}

	//  if the Right joystick switch 
	else if (BIT_IS_SET(PIND, 0) && right){ // RIGHT
		if (random_x > -62) {
			hero_dx = -movement_speed;
			random_x -= 0.5;
		} else {
			hero_dx = 0;
		}
	}

	//	If none of the joystick switches are closed, do stop movement.
	else {
		hero_dx = 0;
		hero_dy = 0;
	}
	// Moves everything except the hero according to dx, dy values set
	if (!attach_key){ // stops moving the key when hero collides
		sprite_turn_to(key, hero_dx, hero_dy);
		sprite_step(key);
	}

	sprite_turn_to(tower, hero_dx, hero_dy);
	sprite_step(tower);

	sprite_turn_to(door, hero_dx, hero_dy);
	sprite_step(door);

	sprite_turn_to(monster, hero_dx, hero_dy);
	sprite_step(monster);

	/////////// come back and fix this if there is time ////////////
	if (floor_num == 2 && treasure.is_visible){
		sprite_turn_to(treasure, hero_dx, hero_dy);
		sprite_step(treasure);
	} 
	for (int i = 0; i < num_treasure; i++){
		if (floor_num > 1 && treasure_array[i].is_visible){
			sprite_turn_to(treasure_array[i], hero_dx, hero_dy);
			sprite_step(treasure_array[i]);
		}
	}

	if (!attach_shield){ // stops moving the key when hero collides
		if (floor_num > 1 && shield.is_visible){
			sprite_turn_to(shield, hero_dx, hero_dy);
			sprite_step(shield);
		}
	}
	if (!attach_bow){
		if (floor_num > 2){
			sprite_turn_to(bow, hero_dx, hero_dy);
			sprite_step(bow);
		}
	}
		if (!attach_bomb){ // stops moving the key when hero collides
		if (floor_num > 1 && bomb.is_visible){
			sprite_turn_to(bomb, hero_dx, hero_dy);
			sprite_step(bomb);
		}
	}
}	

double sprite_y( sprite_id sprite ) {
	return sprite->y;
}

double sprite_x( sprite_id sprite ) {
	return sprite->x;
}

void sprite_move_to( sprite_id sprite, double x, double y ){
	sprite->x = x;
	sprite->y = y;
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


void reset_level(void){

	int x_offset = rand() % 40;
	int y_offset = rand() % 20;

    /*
	if (rand() % 10 < 3){
		x_offset = -x_offset;
	}
	if (rand() % 10 > 7){
		y_offset = -y_offset;
	} */

	// Re-create tower sprite
	if (floor_num == 1){
		random_y = 0;
    	random_x = 20;
		// Create hero sprite
		sprite_init(&hero, 37, 24, 7, 11, hero_bitmap);
		// Create key sprite
		sprite_init(&key, 10, 24, 7, 3, key_bitmap);
		// Create tower sprite
		sprite_init(&tower, 20, 0, 40, 22, tower_bitmap);
		// Create door sprite
		sprite_init(&door, 36, 12, 8, 10, door_bitmap);
		// Create monster sprite
		sprite_init(&monster, 70, 24, 5, 6, monster_bitmap);
		/*
		sprite_init(&tower, tower.x + x_offset, tower.y + y_offset, 40, 22, tower_bitmap);
		// Re-create key sprite
		sprite_init(&key, key.x + x_offset, key.y + y_offset, 7, 3, key_bitmap);
		// Re-create door sprite
		sprite_init(&door, door.x + x_offset, door.y + y_offset, 8, 10, door_bitmap);
		// Re-create monster sprite
		sprite_init(&monster, monster.x + x_offset, monster.y + y_offset, 5, 6, monster_bitmap);
		*/
	} else if (floor_num == 2){
		random_y = y_offset;
   		random_x = x_offset;
		// Create hero sprite
		sprite_init(&hero, 37, 24, 7, 11, hero_bitmap);
		// Re-create key sprite
		sprite_init(&key, key.x + x_offset, key.y + y_offset, 7, 3, key_bitmap);
		// Re-create door sprite
		sprite_init(&door, door.x + x_offset, door.y + y_offset, 8, 10, door_bitmap);
		// Re-create monster sprite
		sprite_init(&monster, monster.x + x_offset, monster.y + y_offset, 5, 6, monster_bitmap);
		if (treasure.is_visible){
			// Re-create treasure sprite
			sprite_init(&treasure, treasure.x + x_offset, treasure.y + y_offset, treasure.width, treasure.height, treasure_bitmap);
		}
	} else if (floor_num < 2){
		random_y = y_offset;
    	random_x = x_offset;
		// Create hero sprite
		sprite_init(&hero, 37, 24, 7, 11, hero_bitmap);
		// Re-create key sprite
		sprite_init(&key, key.x + x_offset, key.y + y_offset, 7, 3, key_bitmap);
		// Re-create door sprite
		sprite_init(&door, door.x + x_offset, door.y + y_offset, 8, 10, door_bitmap);
		// Re-create monster sprite
		sprite_init(&monster, monster.x + x_offset, monster.y + y_offset, 5, 6, monster_bitmap);

		for (int i = 0; i < num_treasure; i++){
			if (treasure_array[i].is_visible){
				sprite_init(&treasure_array[i], treasure_array[i].x + x_offset, treasure_array[i].y + y_offset, treasure_array[i].width, treasure_array[i].height, treasure_bitmap);	
			}
		}

		if (bow.is_visible){
			// Re-create bow sprite
			sprite_init(&bow, bow.x + x_offset, bow.y + y_offset, bow.width, bow.height, bow_bitmap);
		}

		if (bomb.is_visible){
			// Re-create bomb sprite
			sprite_init(&bomb, bomb.x + x_offset, bomb.y + y_offset, bomb.width, bomb.height, bomb_bitmap);
		}
	}
}

void player_killed(void){
	lives--;
	attach_key = false; // drops the key
	attach_bow = false; // drop bow
	attach_bomb = false; // drop bow

	usb_serial_send( "Person killed by monster. \r\n" );

	// Reset the level - randomising the location of the hero
	reset_level();

}

bool treasure_collision(Sprite toCompare, int width, int height){
	bool result;
	for (int i = 0; i < num_treasure; i++) {
		result = sprite_collison(&treasure_array[i], &toCompare, treasure_array[i].width, treasure_array[i].height, width, height);
		if (result){break;}
	};
	return result;
}

void next_level(void){
	random_y = 0;
    random_x = 20;
    load_screen = true;

	floor_num++;
	attach_key = false; // reset key pickup

	if (!attach_bow){
		bow_exist = false;
	}
	if (!attach_bomb){
		bomb_exist = false;
	}
	score += 100;

	// Guaranteed
	// Re-create door sprite
	do {
		sprite_init(&door, rand() % 85, rand() % 49, 8, 10, door_bitmap);
	} while (sprite_collison(&door, &hero, door.width, door.height, hero.width, hero.height));

	// Re-create key sprite
	do {
		sprite_init(&key, rand() % 85, rand() % 49, 7, 3, key_bitmap);
	} while (sprite_collison(&key, &door, key.width, key.height, door.width, door.height)
			||  (sprite_collison(&key, &hero, key.width, key.height, hero.width, hero.height))
	);

	if (floor_num > 1){
		// Re-create tower sprite
		sprite_init(&tower, -100, -100, 40, 22, tower_bitmap);

		do {
			// Create monster sprite
			sprite_init(&monster, rand() % 85, rand() % 49, monster.width, monster.height, monster_bitmap);	
		} while ((sprite_collison(&monster, &door, monster.width, monster.height, door.width, door.height)) 
			|| (sprite_collison(&monster, &key, monster.width, monster.height, key.width, key.height))
			|| (sprite_collison(&monster, &hero, monster.width, monster.height, hero.width, hero.height))
		);
	}
	if (floor_num == 2){
		// Additional for level 2
		do {
			// Create treasure sprite
			sprite_init(&treasure, rand() % 85, rand() % 49, 5, 3, treasure_bitmap);	
		} while ((sprite_collison(&treasure, &door, treasure.width, treasure.height, door.width, door.height)) 
			|| (sprite_collison(&treasure, &key, treasure.width, treasure.height, key.width, key.height))
			|| (sprite_collison(&treasure, &hero, treasure.width, treasure.height, hero.width, hero.height))
			|| (sprite_collison(&treasure, &monster, treasure.width, treasure.height, monster.width, monster.height))
			);
	} else if (floor_num > 2){	
		//Chance to generate
		// Create shield sprite
		if (!shield_exist){
		if (rand() % 10 < 3){
			shield_exist = true;
			do {
				sprite_init(&shield, rand() % 85, rand() % 49, 7, 4, shield_bitmap);
			} while (sprite_collison(&shield, &hero, shield.width, shield.height, hero.width, hero.height)
				|| (sprite_collison(&shield, &door, shield.width, shield.height, door.width, door.height))
				|| (sprite_collison(&shield, &key, shield.width, shield.height, key.width, key.height))
				|| (sprite_collison(&shield, &treasure, shield.width, shield.height, treasure.width, treasure.height))
				|| (sprite_collison(&shield, &monster, shield.width, shield.height, monster.width, monster.height))
				);
		}
		}
		// Create bow sprite
		if (!bow_exist){
		if (rand() % 10 < 3){
			bow_exist = true;
			do {
				sprite_init(&bow, rand() % 85, rand() % 49, 3, 5, bow_bitmap);
			} while (sprite_collison(&bow, &hero, bow.width, bow.height, hero.width, hero.height)
				|| (sprite_collison(&bow, &door, bow.width, bow.height, door.width, door.height))
				|| (sprite_collison(&bow, &key, bow.width, bow.height, key.width, key.height))
				|| (sprite_collison(&bow, &shield, bow.width, bow.height, shield.width, shield.height))
				|| (sprite_collison(&bow, &monster, bow.width, bow.height, monster.width, monster.height))
				);
		}
		}
		if (!bomb_exist){
			if (rand() % 10 < 11){
			bomb_exist = true;
			do {
				sprite_init(&bomb, rand() % 85, rand() % 49, 7, 5, bomb_bitmap);
			} while (sprite_collison(&bomb, &hero, bomb.width, bomb.height, hero.width, hero.height)
				|| (sprite_collison(&bomb, &door, bomb.width, bomb.height, door.width, door.height))
				|| (sprite_collison(&bomb, &key, bomb.width, bomb.height, key.width, key.height))
				|| (sprite_collison(&bomb, &shield, bomb.width, bomb.height, shield.width, shield.height))
				|| (sprite_collison(&bomb, &monster, bomb.width, bomb.height, monster.width, monster.height))
				|| (sprite_collison(&bomb, &bow, bomb.width, bomb.height, bow.width, bow.height))
				);
			}
		}

		num_treasure = rand() % 6;
		do {
			for (int i = 0; i < num_treasure; i++){
				Sprite sprite;
				sprite_init(&sprite, rand() % 85, rand() % 49, treasure.width, treasure.height, treasure_bitmap);	
				treasure_array[i] = sprite;
			}
		} while (treasure_collision(bomb, bomb.width, bomb.height)
			|| (treasure_collision(hero, hero.width, hero.height))
			|| (treasure_collision(key, key.width, key.height))
			|| (treasure_collision(monster, monster.width, monster.height))
			|| (treasure_collision(bow, bow.width, bow.height))
			|| (treasure_collision(door, door.width, door.height))
			);
	}
}

void monster_move(void){
	if (!pause_menu && start_game && !load_screen){
		double speed;

		if (monster.x - hero.x > 47 || hero.x - monster.x > (37 + monster.width)){
			speed = 0;
			monster_follow = false;
		} else if (monster.y - hero.y > 24 || hero.y - monster.y > (24 + monster.height)){
			speed = 0;
			monster_follow = false;
		} else {
			speed = 0.125;
			monster_follow = true;
		}	

		double monster_dx = round(hero.x) - round(monster.x);
		double monster_dy = round(hero.y) - round(monster.y);

		double dist = sqrt( monster_dx * monster_dx + monster_dy * monster_dy + 1e-10);

		monster_dx = monster_dx * speed / dist;
		monster_dy = monster_dy * speed / dist;

		sprite_turn_to(monster, monster_dx, monster_dy);
		sprite_step(monster);
	}
}

 float get_shooting_angle() {
  	BIT_ON(ADCSRA, ADSC); // start conversion
  	while (GET_BIT(ADCSRA, ADSC));  // wait until complete
  	return ((float) ADC) / 1023 * PI * 4;
}

void game_over(){
	if (end_game){
		games_played++;
		clear_screen();
		draw_string(10, 5, "GAME OVER!!!", FG_COLOUR);
		draw_string(20, 20, "Floor:", FG_COLOUR);
		draw_int(55, 20, floor_num, FG_COLOUR);
		draw_string(20, 30, "Score:", FG_COLOUR);
		draw_int(55,30, score, FG_COLOUR);
		show_screen();
	}
	
	if ((BIT_IS_SET(PINF, 6)) || (BIT_IS_SET(PINF, 5))){
		while ((BIT_IS_SET(PINF, 6)) || (BIT_IS_SET(PINF, 5))){

			lives = 3;
			score = 0;
			end_game = false;
			pause_menu = false;
			start_screen = true;
			start_countdown = false;
			load_screen = false;
			countdown_counter = 3;
			score = 0, lives = 3, floor_num = 0, seconds = 0, minutes = 0;
			overflow_counter = 0;
			start_game = false;
			setup();
		}
	} 
}

// ---------------------------------------------------------
//	USB serial
// ---------------------------------------------------------

void usb_serial_send(char * message) {
	// Cast to avoid "error: pointer targets in passing argument 1 
	//	of 'usb_serial_write' differ in signedness"
	usb_serial_write((uint8_t *) message, strlen(message));
}

void setup_usb_serial(void) {
	// Set up LCD and display message
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_string(10, 10, "Connect USB...", FG_COLOUR);
	show_screen();

	usb_init();

	while ( !usb_configured() ) {
		// Block until USB is ready.
	}

	clear_screen();
	draw_string(10, 10, "USB connected", FG_COLOUR);
	// SEND WELCOME MESSAGE
	usb_serial_send( "GAME READY!" );
	usb_serial_send( "\r\n" );
	show_screen();
}

// ---------------------------------------------------------
//	Program flow
// ---------------------------------------------------------

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);

	DDRF &= 0b10011100; // SW1, SW2, ADC1, ADC0
	ADMUX = 0;
	  BIT_ON(ADMUX, REFS0); 
	  BIT_ON(ADMUX, MUX0); // enable ADC1 (right)

	  // Enable ADC
	  BIT_ON(ADCSRA, ADEN);
	  // pre-scaler of 128
	  BIT_ON(ADCSRA, ADPS2);
	  BIT_ON(ADCSRA, ADPS1);
	  BIT_ON(ADCSRA, ADPS0);

	setup_timer();
	setup_switches();

	seconds = 0, minutes = 0;
	overflow_counter = 0;

	// Create hero sprite
	sprite_init(&hero, 37, 24, 7, 11, hero_bitmap);
	// Create key sprite
	sprite_init(&key, 10, 24, 7, 3, key_bitmap);
	// Create tower sprite
	sprite_init(&tower, 20, 0, 40, 22, tower_bitmap);
	// Create door sprite
	sprite_init(&door, 36, 12, 8, 10, door_bitmap);
	// Create monster sprite
	sprite_init(&monster, 70, 24, 5, 6, monster_bitmap);
	if (games_played == 0){
		setup_usb_serial();
	}

	clear_screen();
	show_screen();
}

void process(void) {
	tower_x = round(sprite_x(&tower));
	tower_y = round(sprite_y(&tower));
	hero_x = round(sprite_x(&hero));
	hero_y = round(sprite_y(&hero));
	key_x = round(sprite_x(&key));
	key_y = round(sprite_y(&key));
	monster_x = round(sprite_x(&monster));
	monster_y = round(sprite_y(&monster));
	
	if (lives <= 0){
		end_game = true;
		game_over();
	} else {

	draw_load_screen();

	clear_screen();	
	draw_startup();	
	check_start();
	boundary_zone();
	
	if (start_countdown && !start_game){
		countdown_timer();
	}

	hero_move();
	monster_move();

	process_time();
	draw_pause_menu();
	draw_sprites();

	// door collission
	if (sprite_collison(&hero, &door, 7, 11, 8, 10) && attach_key){
			next_level();
	} else if (sprite_collison(&hero, &door, 7, 11, 8, 10)){
		// disable joystick based on where sprite collided	
		if (round(sprite_y(&hero)) == round(sprite_y(&door)) + door.height - 1){
				up = false; // collided from the bottom, disable up
		} else if (round(sprite_x(&hero)) + hero.width - 1 == round(sprite_x(&door))) {
				right = false; // collided fom left hand side, disable right
		} else if (round(sprite_x(&hero)) == round(sprite_x(&door)) + door.width - 1) {
				left = false;// collided fom right hand side, disable left
		} else if (round(sprite_y(&hero)) + hero.height - 1 == round(sprite_y(&door))){
				down = false; // collided fom top, disable down
		}
	} else {
		up = true;
		right = true;
		left = true;
		down = true;
	}

	// Hero / monster collision
	if (floor_num == 1 || floor_num == 2){ // shield introduced from level 2 onwards
		if (sprite_collison(&hero, &monster, 7, 11, 5, 6)){		
			player_killed();	
		}
	}

	// Monster / door collision
		if (sprite_collison(&monster, &door, monster.width, monster.height, door.width, door.height)){
			if (round(sprite_y(&monster)) == round(sprite_y(&door)) + door.height - 1){ // from the bottom
				sprite_move_to(&monster, round(sprite_x(&monster)), round(sprite_y(&monster)) + 2); 
			} else if (round(sprite_x(&monster)) + monster.width - 1 == round(sprite_x(&door))){ // from the left
				sprite_move_to(&monster, round(sprite_x(&monster)) - 2, round(sprite_y(&monster))); 
			} else if(round(sprite_x(&monster)) == round(sprite_x(&door)) + door.width - 1){ // from the right
				sprite_move_to(&monster, round(sprite_x(&monster)) + 2, round(sprite_y(&monster))); 
			} else if (round(sprite_y(&monster)) + monster.height - 1 == round(sprite_y(&door))){ // from the top
				sprite_move_to(&monster, round(sprite_x(&monster)), round(sprite_y(&monster)) - 2); 
			} 
		}
	// Hero / Key collision
	if (sprite_collison(&hero, &key, 7, 11, 7, 3)){	
		if (!attach_key){
			usb_serial_send( "Hero picked up the key. \r\n" );
		}
		attach_key = true;

	}

	// Hero + Key / Door collision
	if (sprite_collison(&hero, &door, 7, 11, 8, 10) && attach_key){
		usb_serial_send( "Hero used the key to go to next level. \r\n" );
		next_level();
	}

	if (floor_num == 1){
		// Monster / tower collision
		if (sprite_collison(&monster, &tower, 5, 6, 40, 22)){
			if (round(sprite_y(&monster)) == round(sprite_y(&tower)) + 22 - 1){ // from the bottom
				sprite_move_to(&monster, round(sprite_x(&monster)), round(sprite_y(&monster)) + 2); 
			} else if (round(sprite_x(&monster)) + 5 - 1 == round(sprite_x(&tower))){ // from the left
				sprite_move_to(&monster, round(sprite_x(&monster)) - 2, round(sprite_y(&monster))); 
			} else if(round(sprite_x(&monster)) == round(sprite_x(&tower)) + 40 - 1){ // from the right
				sprite_move_to(&monster, round(sprite_x(&monster)) + 2, round(sprite_y(&monster))); 
			} else if (round(sprite_y(&monster)) + 6 - 1 == round(sprite_y(&tower))){ // from the top
				sprite_move_to(&monster, round(sprite_x(&monster)), round(sprite_y(&monster)) - 2); 
			} 
		}
		// Hero / Door collision
		if (sprite_collison(&hero, &door, 7, 11, 8, 10) && attach_key){
			next_level();
		}

		// Hero / tower collision
		if (sprite_collison(&hero, &tower, 7, 11, 40, 22)){
			// disable joystick based on where sprite collided	
			if (round(sprite_y(&hero)) == round(sprite_y(&tower)) + 22 - 1){
				up = false; // collided from the bottom, disable up
			} else if (round(sprite_x(&hero)) + 7 - 1 == round(sprite_x(&tower))) {
				right = false; // collided fom left hand side, disable right
			} else if (round(sprite_x(&hero)) == round(sprite_x(&tower)) + 40 - 1) {
				left = false;// collided fom right hand side, disable left
			} else if (round(sprite_y(&hero)) + 11 - 1 == round(sprite_y(&tower))){
				down = false; // collided fom top, disable down
			}
		} else {
			up = true;
			right = true;
			left = true;
			down = true;
		}

	} else if (floor_num == 2){

		// treasure collision
		if (sprite_collison(&hero, &treasure, 7, 11, treasure.width, treasure.height)){			
			if (treasure.is_visible) { score += 10; }
			treasure.is_visible = false;
		}

	} else if (floor_num > 2){

		if (shield_exist){
			// shield collision
			if (!shield_active && (sprite_collison(&hero, &shield, 7, 11, shield.width, shield.height))){		
				shield_active = true;
				attach_shield = true;
				usb_serial_send( "Hero picked up a shield. \r\n" );
				if (item_held){
					attach_bomb = false;
					attach_bow = false;
				} else {
					item_held = true;
				}	
			} 
		}
		// monster collision
		if (sprite_collison(&hero, &monster, 7, 11, 5, 6) && monster.is_visible){	
		
			if (shield_active){
				attach_shield = false;
				shield_active = false;
				shield.is_visible = false;	
				shield_exist = false;

				monster.is_visible = false;
				usb_serial_send( "Hero used shield. \r\n" );
				usb_serial_send( "Monster killed by hero. \r\n" );
			} else {
				player_killed();
			}

			if (item_held){
				attach_bow = false;
				attach_bomb = false;
			} else {
				item_held = true;
			}
		}
		if (bow_exist){
			// bow collision
			if (sprite_collison(&hero, &bow, 7, 11, bow.width, bow.height)){		
				if (!attach_bow){
					usb_serial_send( "Hero picked up a bow. \r\n" );
				}
				attach_bow = true;	

				if (item_held){
					attach_shield = false;
					shield_active = false;
					attach_bomb = false;
				} else {
					item_held = true;
				}	
			}
		}
		if (bomb_exist){
			// bow collision
			if (sprite_collison(&hero, &bomb, 7, 11, bomb.width, bomb.height)){		
				if (!attach_bomb){
					usb_serial_send( "Hero picked up a bomb. \r\n" );
				}
				attach_bomb = true;
				
				if (item_held){
					attach_bow = false;
					attach_shield = false;
					shield_active = false;
				} else {
					item_held = true;
				}
			}
			if (attach_bomb){
				float hero_angle = get_shooting_angle();
				char x2 = hero.x + hero.width /2 +20 * cos(hero_angle);
				char y2 = hero.y + hero.height /2 +20 * sin(hero_angle);

				draw_line(hero.x + hero.width /2, hero.y + hero.height /2, x2, y2, FG_COLOUR);

				// fire the bomb
				if ( BIT_IS_SET(PINF, 6) || ( BIT_IS_SET(PINF, 5))){
					
					bomb_timer_start = true;
					attach_bomb = false;	
					item_held = false;		

					// if the bomb is going to fire onto some place it cant...
					bomb.x = hero.x + hero.width /2 +15 * cos(hero_angle);
					bomb.y = hero.y + hero.height /2 +15 * sin(hero_angle);
				}			
			} else if (bomb_timer_start){
				bomb_timer -= 100;
				if (bomb_timer == 1500){
					SET_BIT(PORTB, 2);
					SET_BIT(PORTB, 3);
				} else if (bomb_timer == 1000){
					CLEAR_BIT(PORTB, 2);
					CLEAR_BIT(PORTB, 3);
				} else if (bomb_timer == 500){
					SET_BIT(PORTB, 2);
					SET_BIT(PORTB, 3);
				} else {
					CLEAR_BIT(PORTB, 2);
					CLEAR_BIT(PORTB, 3);
				}
				if (bomb_timer <= 0){
					usb_serial_send( "Hero used a bomb. \r\n" );
					// destroy sprites here
					if (monster_follow && monster.is_visible){
						if (bomb_timer <= 0){
							monster.is_visible = false;
							usb_serial_send( "Hero killed a monster. \r\n" );
							score += 10;
							bomb_exist = false;
							bomb_timer = 2000;
							bomb_timer_start = false;
						}				
					} else {
							bomb_timer = 2000;
							bomb_timer_start = false;
							bomb_exist = false;
					}
				}
			}
		}

		// treasure collision
		for (int i = 0; i < num_treasure; i++){
			if (sprite_collison(&hero, &treasure_array[i], 7, 11, treasure_array[i].width, treasure_array[i].height)){			
				if (treasure_array[i].is_visible) { score += 10; }
					treasure_array[i].is_visible = false;
			}
		}
	}
	}
	show_screen();
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(10);
	}
}