#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
//#include <time.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (5)
#define DOOR_WIDTH (5)
#define DOOR_HEIGHT (5)
#define HERO_WIDTH (5)
#define HERO_HEIGHT (3)
#define BAT_WIDTH (3)
#define BAT_HEIGHT (2)


// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */
bool left = true; /* Changes between true and false depending on Zombie Location. */
bool leftPlatB = false;
int counterMill = 0;
int lvlNum = 1;
bool treasureTaken = false;
bool gateAccess = false;

/* Movement variables for the hero */
bool rightM = false; 
bool leftM = false; 
bool rightMRun = false;
bool leftMRun = false;

double jumpCount = 0;
int jumpHeight = HERO_HEIGHT * 6;
bool jump = false;
bool enableKeys = true;
bool land = false;
bool landed = false;
int tempJumpCount;

//Boulder Vlariables
int boulderX = 0;
int fallCount = 0;
bool boulderLEFT = false;
bool boulderFALL = true;


//GUI
int score = 0;
int timerSec = 00;
int timerMin = 00;
int lives = 10;
int level = 1;

char * platform_image = 
/**/"===============================================================================================================================";

char * zombie_image =
/**/	"ZZZZZ"
/**/	"   Z "
/**/	"  Z  "
/**/	" Z   "
/**/	"ZZZZZ";

char * zombie_image_right =
/**/	"ZZZZZ"
/**/	" Z   "
/**/	"  Z  "
/**/	"   Z "
/**/	"ZZZZZ";

char * amulet_image =
/**/	" _ " 
/**/	"/ \\"
/**/	"\\ /"
/**/	" @ ";

char * boulder_image =
/**/	"/\"\"\\"
/**/	"\\__/";

char * bat_image =
/**/	" | " 
/**/	"/ \\";

char * gate_image =
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
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]"
/**/	"[]";

char * hero_image =
/**/	"  o  "
/**/	" /:\\ "
/**/	" / \\ ";

char * hero_image_fall =
/**/	" \\o/ "
/**/	"  :  "
/**/	" / \\ ";

char * treasure_image =
/**/	"~~~"
/**/	"~$~"
/**/	"~~~";

char * key_image =
/**/	"   "
/**/	"0-+"
/**/	"   ";

char * door_image =
/**/	" ___ "
/**/	"|   |"
/**/	"|   |"
/**/	"|  *|"
/**/	"|___|";

sprite_id zombie;
sprite_id hero;
sprite_id door;
sprite_id bat;
sprite_id treasure;
sprite_id platform;
sprite_id platformSmall;
sprite_id gate;
sprite_id platformOne;
sprite_id platformTwo;
sprite_id platformThree;
sprite_id key_treasure;
sprite_id platformLVL5A;
sprite_id platformLVL5B;
sprite_id amulet;
sprite_id boulder;

//DECLARATIONS
bool sprites_collided( sprite_id s1, sprite_id s2 );
bool sprites_collided_Treasure();
void gui(void);
void DRAW_PLATFORM(int x, int y);
void HEROLOCATION(double x, double y);
void DOORLOCATION(double x, double y);
void ZOMBIELOCATION(double x, double y);
void BATLOCATION(double x, double y);
void TREASURELOCATION(double x, double y);
void PLATFORMLOCATION(void);
void PLATFORMLOCATIONSMALL(void);
void treasureCheck(void);
void PLATFORMONELOCATION(void);
void PLATFORMTWOLOCATION(void);
void PLATFORMTHREELOCATION(void);
void GATELOCATION(void);
bool sprites_collided_Gate( sprite_id s1, sprite_id s2 );
void KEYLOCATION();
void WORLD_BARRIER(void);
void move_platformLVL5B();
void AMULETLOCATION(double x, double y);
void BOULDERLOCATION(double x, double y);
void boulder_Fall();
void PLATFORMLOCATION_LVL5A(void);
void PLATFORMLOCATION_LVL5B(void);

void reset(void){
	clear_screen();
	jumpCount = 0;
	leftM = false;
	leftMRun = false;
	rightM = false;
	rightMRun = false;
	jump = false;
	tempJumpCount = 0;
	landed = false;
	treasureTaken = false;
	gateAccess = false;
	
	
}

void WORLDBORDER(void){
	int topmax = screen_height()- 1;
	draw_line( 0, 2, screen_width()-1, 2, '-');
	draw_line( 0, 3, 0, topmax, ':' );
	draw_line( screen_width()-1, 3, screen_width()-1, topmax, ':' );
	draw_line( 0, screen_height()-1, screen_width()-1, screen_height()-1, '=' );
	
}

void WORLDBORDERLVL2(void){
	int topmax = screen_height()- 1;
	draw_line( 0, 2, screen_width()-1, 2, '-');
	draw_line( 0, 3, 0, topmax, ':' );
	draw_line( screen_width()-1, 3, screen_width()-1, topmax, ':' );
	draw_line( 0, screen_height()-1, screen_width()/4-1, screen_height()-1, '=' );
	draw_line( screen_width() - screen_width()/4-1, screen_height()-1, screen_width(), screen_height()-1, '=' );
}

void WORLDBORDERLVL5(void){
	int topmax = screen_height()- 1;
	draw_line( 0, 2, screen_width()-1, 2, '-');
	draw_line( 0, 3, 0, topmax, ':' );
	draw_line( screen_width()-1, 3, screen_width()-1, topmax, ':' );
	draw_line( 0, screen_height()-1, screen_width()/4-1, screen_height()-1, '=' );
	//draw_line( screen_width() - screen_width()/4-1, screen_height()-1, screen_width(), screen_height()-1, '=' );
}

//Draws the sores and stuff at top
void gui(void){
	// int score = 0;
	// int time = 0;
	// int lives = 10;
	// int level = 1;
	if (timerSec < 10){
		draw_formatted(1,1, "Time:%02d:%02d \t\t Level:%2d \t\t Lives:%2d \t\t Score:%2d", timerMin, timerSec, lvlNum, lives, score);
	} else {
		draw_formatted(1,1, "Time:%02d:%2d \t\t Level:%2d \t\t Lives:%2d \t\t Score:%2d", timerMin, timerSec, lvlNum, lives, score);
	}
		
}

void LEVEL1(void){
	jumpCount = 0;
	WORLDBORDER();
	
	int platPOSX = screen_width()/3;
	int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
	
	
	DRAW_PLATFORM(platPOSX, platPOSY);
	
	double zombieX = screen_width()- ZOMBIE_WIDTH - 1;
	double zombieY = screen_height() - ZOMBIE_HEIGHT - 1;
	double heroX = 1;
	double heroY = screen_height() - HERO_HEIGHT - 1;
	
	HEROLOCATION(heroX, heroY);
	ZOMBIELOCATION(zombieX, zombieY);
	DOORLOCATION(zombieX, zombieY);
	gui();
	
}

void LEVEL2(void){
	
	reset();
	lvlNum = 2;
	
	jumpCount = 0;
	WORLDBORDER();
	
	int platPOSX = screen_width()/3;
	int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
	
	
	DRAW_PLATFORM(platPOSX, platPOSY);
	
	double zombieX = screen_width()- ZOMBIE_WIDTH - 1;
	double zombieY = screen_height() - ZOMBIE_HEIGHT - 1;
	double heroX = 1;
	double heroY = screen_height() - HERO_HEIGHT - 1;
	double treasureX = screen_width()/2;
	double treasureY = (screen_height()-(2*(HERO_HEIGHT*3.5)));
	
	HEROLOCATION(heroX, heroY);
	ZOMBIELOCATION(-10, -10);
	DOORLOCATION(zombieX, zombieY);
	BATLOCATION(zombieX, zombieY+1);
	TREASURELOCATION(treasureX, treasureY);
	gui();
	
}

void LEVEL3(void){

	
	reset();
	lvlNum = 3;
	
	jumpCount = 0;
	WORLDBORDERLVL2();
	
	double doorX = screen_width()/2-2.5;
	int doorY = (screen_height()-2*(HERO_HEIGHT*3.5))-DOOR_HEIGHT;
	double zombieX = screen_width()- ZOMBIE_WIDTH - 1;
	double zombieY = screen_height() - ZOMBIE_HEIGHT - 1;
	double heroX = 1;
	double heroY = screen_height() - HERO_HEIGHT - 1;
	
	HEROLOCATION(heroX, heroY);
	DOORLOCATION(-10, -10);
	TREASURELOCATION(zombieX - 3, zombieY + 1);
	ZOMBIELOCATION(-10, -10);
	PLATFORMLOCATION();
	PLATFORMLOCATIONSMALL();
	DOORLOCATION(doorX, doorY);

	gui();
}

void LEVEL4(void){
	jumpCount = 0;
	WORLDBORDER();
	land = false;
	
	
	double zombieX = screen_width()- ZOMBIE_WIDTH - 1;
	double zombieY = screen_height() - ZOMBIE_HEIGHT - 1;
	double heroX = 1;
	double heroY = screen_height() - HERO_HEIGHT - 1;
	
	HEROLOCATION(heroX, heroY);
	ZOMBIELOCATION(-10, -10);
	DOORLOCATION(zombieX, zombieY);
	gui();
	GATELOCATION();
	KEYLOCATION();
	PLATFORMONELOCATION();
	PLATFORMTWOLOCATION();
	PLATFORMTHREELOCATION();
}

void LEVEL5(void){

	
	reset();
	lvlNum = 5;
	sprite_destroy(door);
	jumpCount = 0;
	WORLDBORDERLVL5();
	
	double doorX = screen_width()/2-2.5;
	int doorY = (screen_height()-2*(HERO_HEIGHT*3.5))-DOOR_HEIGHT;
	double zombieX = screen_width()- ZOMBIE_WIDTH - 1;
	double zombieY = screen_height() - ZOMBIE_HEIGHT - 1;
	double heroX = 1;
	double heroY = screen_height() - HERO_HEIGHT - 1;
	double amuletX = screen_width()/2;
	double amuletY = screen_height()-((HERO_HEIGHT*3.5)*3+(HERO_HEIGHT*0.5));
	
	HEROLOCATION(heroX, heroY);
	AMULETLOCATION(amuletX, amuletY);
	TREASURELOCATION(zombieX - 3, zombieY + 1);
	ZOMBIELOCATION(-10, -10);
	PLATFORMLOCATION_LVL5B();
	PLATFORMLOCATION_LVL5A();
	DOORLOCATION(doorX, doorY);
	double boulderY = (screen_height()-2*(HERO_HEIGHT*3.5)-2);
	double boulderX = (sprite_width(platformLVL5A)-4)/2;
	BOULDERLOCATION(boulderX,boulderY);
	gui();
}

void FINISH(void){
	reset();
	if (lives == 0){
			draw_formatted((screen_width()/2 - 35/2),screen_height()/2, "GAME OVER! \n Press any key to exit.", score);
		} else {
			draw_formatted((screen_width()/2 - 61/2),screen_height()/2, "GAME OVER! \n Your final score was: %2d \n Press any key to exit.", score);
		
		}	
	
}

void HEROLOCATION(double x, double y){
	double heightMid = y;
	double widthMid = x;
	
	hero = sprite_create( widthMid, heightMid, HERO_WIDTH, HERO_HEIGHT, hero_image);
	sprite_draw( hero );
}

void DOORLOCATION(double x, double y){
	double heightMid = y;
	double widthMid = x;
	
	door = sprite_create( widthMid, heightMid, DOOR_WIDTH, DOOR_HEIGHT, door_image);
	sprite_draw( door );
}

void ZOMBIELOCATION(double x, double y){

	double heightMid = y;
	double widthMid = x;
	
	zombie = sprite_create( widthMid, heightMid, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, zombie_image);
	sprite_draw( zombie );
}

void BATLOCATION(double x, double y){

	double heightMid = y;
	double widthMid = x;
	
	bat = sprite_create( widthMid, heightMid, BAT_WIDTH, BAT_HEIGHT, bat_image);
	sprite_draw( bat );
}

void TREASURELOCATION(double x, double y){
	
	double heightMid = y;
	double widthMid = x;
	
	treasure = sprite_create( widthMid, heightMid, 3, 3, treasure_image);
	sprite_draw( treasure );
}

void AMULETLOCATION(double x, double y){
	
	double heightMid = y;
	double widthMid = x;
	
	amulet = sprite_create( widthMid, heightMid, 3, 4, amulet_image);
	sprite_draw( amulet );
}

void BOULDERLOCATION(double x, double y){
	
	double heightMid = y;
	double widthMid = x;
	
	boulder = sprite_create( widthMid, heightMid, 4, 2, boulder_image);
	sprite_draw( boulder );
}

void KEYLOCATION(){
	
	double heightMid = screen_height()*0.2;
	double widthMid = screen_width()-screen_width()*0.1;
	
	key_treasure = sprite_create( widthMid, heightMid, 3, 3, key_image);
	sprite_draw( key_treasure );
}

void PLATFORMLOCATION(void){

		double ph = 1;
		double pw = screen_width()*0.4;
		int platPOSX = screen_width()/3;
		int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
		
		platform = sprite_create( platPOSX, platPOSY, pw, ph, platform_image);
		sprite_draw(platform);

	
}

void PLATFORMLOCATION_LVL5A(void){


		double ph = 1;
		double pw = screen_width()*0.5-1;
		int platPOSX = 1;
		int platPOSY =  (screen_height()-2*(HERO_HEIGHT*3.5));
		
		platformLVL5A = sprite_create( platPOSX, platPOSY, pw, ph, platform_image);
		sprite_draw(platformLVL5A);

	
}

void PLATFORMLOCATION_LVL5B(void){


		double ph = 1;
		double pw = screen_width()*0.5-1;
		int platPOSX = screen_width()-pw;
		int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
		
		platformLVL5B = sprite_create( platPOSX, platPOSY, pw, ph, platform_image);
		sprite_draw(platformLVL5B);

	
}

void PLATFORMONELOCATION(void){
	double ph = 1;
	double pw = screen_width()*0.6-1;
	int platPOSX1 = screen_width()*0.4;
	int platPOSY2 =  (screen_height()-(HERO_HEIGHT*3.5));
		
	platformOne = sprite_create( platPOSX1, platPOSY2, pw, ph, platform_image);
	sprite_draw(platformOne);
}

void PLATFORMTWOLOCATION(void){
	double ph = 1;
	double pw = screen_width()*0.6-1;
	int platPOSX1 = 1;
	int platPOSY2 =  (screen_height()-2*(HERO_HEIGHT*3.5));
		
	platformTwo = sprite_create( platPOSX1, platPOSY2, pw, ph, platform_image);
	sprite_draw(platformTwo);
}

void PLATFORMTHREELOCATION(void){
	double ph = 1;
	double pw = screen_width()*0.6-1;
	int platPOSX1 = screen_width()*0.4;
	int platPOSY2 =  (screen_height()-3*(HERO_HEIGHT*3.5));
		
	platformThree = sprite_create( platPOSX1, platPOSY2, pw, ph, platform_image);
	sprite_draw(platformThree);
}

void PLATFORMLOCATIONSMALL(void){
	double ph = 1;
	double pw = 12;
	int platPOSX = screen_width()/2-6;
	int platPOSY = (screen_height()-2*(HERO_HEIGHT*3.5));
	
	platformSmall = sprite_create( platPOSX, platPOSY, pw, ph, platform_image);
	sprite_draw(platformSmall);
}

void GATELOCATION(void){
	double ph = 3*3.5-1;
	double pw = 2;
	int platPOSX = screen_width()*0.7;
	int platPOSY = (screen_height()-(HERO_HEIGHT*3.5)+1);
	
	gate = sprite_create( platPOSX, platPOSY, pw, ph, gate_image);
	sprite_draw(gate);	
}

// Setup game.
void setup(void) {
	if(lvlNum == 1){
		LEVEL1();
	} else if (lvlNum == 2){
		LEVEL2();
	} else if (lvlNum == 3){
		LEVEL3();
	} else if (lvlNum == 4){
		LEVEL4();
	} else if (lvlNum == 5){
		LEVEL5();
	} else {
		FINISH();
	}
	
}

//Detecting a collision between the zombie and the hero.
void COLLISION(void){
	if (lvlNum == 1){
		if ( sprites_collided( hero, zombie ) == true ) {
			clear_screen();
			setup();
			leftM = false;
			leftMRun = false;
			rightM = false;
			rightMRun = false;
			lives--;
		} else if (sprites_collided( hero, door ) == true){
			clear_screen();
			lvlNum++;
			setup();
			score= score+100;
		} 
	}
	
	if (lvlNum == 2){
		if (sprites_collided(hero, treasure) == true){
			treasureTaken = true;
			sprite_destroy(treasure);
			score = score + 100;
		} 
		if (sprites_collided(hero, bat) == true){
			clear_screen();
			treasureCheck();
			setup();
			leftM = false;
			leftMRun = false;
			rightM = false;
			rightMRun = false;
			lives--;
		}	if (sprites_collided( hero, door ) == true){
			clear_screen();
			lvlNum++;
			setup();
			score= score+100;
		} 
	}
	if (lvlNum == 3){
		if (sprites_collided(hero, treasure) == true){
			treasureTaken = true;
			sprite_destroy(treasure);
			score  = score + 100;
		}
		if (sprites_collided(hero, door) == true){
			clear_screen();
			lvlNum++;
			setup();
			score= score+100;
		} 
	}
	if (lvlNum == 4){
		if (sprites_collided_Gate(hero, gate) == true && gateAccess == false){
			// game_over = true;
			rightM = false;
			rightMRun = false;
			leftM = false;
			leftMRun = false;
			sprite_move(hero, -0.5, 0);
		} 
		if (sprites_collided_Gate(hero, gate) == true && gateAccess == true){
			sprite_destroy(gate);
		} 
		if (sprites_collided(hero, key_treasure) == true){
			gateAccess = true;
			sprite_destroy(key_treasure);
		}
		if (sprites_collided(hero, door) == true){
			clear_screen();
			lvlNum++;
			setup();
			score= score+100;
		} 
	} if (lvlNum == 5){
		if (sprites_collided(hero, amulet) == true){
			score = score + 1000;
			clear_screen();
			lvlNum++;
			setup();
		}
		if (sprites_collided(hero, boulder) == true){
			clear_screen();
			boulderLEFT = false;
			treasureCheck();
			setup();
			leftM = false;
			leftMRun = false;
			rightM = false;
			rightMRun = false;
			lives--;
		}
	}
	
	gui();
	
	//WHEN PLAYER RUNS OUT OF LIVES
	if (lives == 0){
		clear_screen();
		lvlNum = 6;
		setup();
	}
}

// Used To move the zombie sprite ~ May need adjustment for future sprites
void move_zombie( sprite_id zombie ) {
	sprite_step( zombie );
	int x = round(sprite_x(zombie));
	if (x == screen_width() - sprite_width(zombie) -1){
		left = true;
	} 
	if (x == 1){
		left = false;
	}
	if (left == true){
		sprite_move( zombie, -0.5, 0 );
	} else {
		sprite_move( zombie, 0.5, 0 );
	}
}

void move_platformLVL5B( void ) {
	sprite_step( platformLVL5B );
	int x = round(sprite_x(platformLVL5B));
	if (x == screen_width() - sprite_width(platformLVL5B) -1){
		leftPlatB = true;
	} 
	if (x == 1){
		leftPlatB = false;
	}
	if (leftPlatB == true){
		sprite_move( platformLVL5B, -0.5, 0 );
	} else {
		sprite_move( platformLVL5B, 0.5, 0 );
	}
}

void move_boulder(void){
	sprite_step( boulder );
	int x = round(sprite_x(boulder));
	if (x == screen_width() - sprite_width(boulder) -1){
		boulderLEFT = true;
	} 
	if (x == 1){
		boulderLEFT = false;
	}
	if (boulderLEFT == true){
		sprite_move( boulder, -0.5*1.33, 0 );
	} else {
		sprite_move( boulder, 0.5*1.33, 0 );
	}
}

//Creates a platform 1/3 of the screen at any x and y location.
void DRAW_PLATFORM(int x, int y){
	int platformLength = screen_width()/3;
	draw_line(x, y, x + platformLength, y, '=');
	
}

void PIT(void){
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	
	if (y == screen_height()-1 && ((x > screen_width()/4) && (x < screen_width()-screen_width()/4))){
		jumpCount = 8;
	}
}

void PIT_LVL5(void){
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int bx = round(sprite_x(boulder));
	int by = round(sprite_y(boulder))+2;
	
	if (y == screen_height()-1 && (x > screen_width()/4)){
		jumpCount = 8;
	}
	if (by == screen_height()-1 && (bx > screen_width()/4)){
		fallCount = 8;
		
	}
}

//draw_line( 0, 2, screen_width()-1, 2, '-');
void WORLD_BARRIER(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platPOSX = 0;
	int platPOSY = 2;
	int platEND = screen_width()-1;
	
	// if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		// land = true;
		// jumpCount = 0;
	// }
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jump = false;
		land = true;
	}
	// if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true){
		// land = false;
		// jumpCount = HERO_HEIGHT*3.5;
		// jump = false;
	// }
	// if (x > platEND && x <= screen_width() && y == platPOSY && land == true){
		// land = false;
		// jumpCount = HERO_HEIGHT*3.5;
		// jump = false;
	// }
}

void LEVEL3_BARRIER(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = screen_width()*0.4;
	int platPOSX = round(sprite_x(platform));
	int platPOSY =  round(sprite_y(platform));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = false;
	}
	if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5-1;
		jump = false;
	}
	if (x > platEND && x <= screen_width() && y == platPOSY && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5-1;
		jump = false;
	}
}

void LEVEL3_BARRIER_SMALL(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = 12;
	int platPOSX = round(sprite_x(platformSmall));
	int platPOSY =  round(sprite_y(platformSmall));
	int platEND = platPOSX + platformLength-1;
	
	if(((x+HERO_WIDTH >= platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
		//enableKeys = true;
	}
	
	if(((x+HERO_WIDTH >= platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = false;
	}
	if ((x+HERO_WIDTH < platPOSX && x >= 0) && y == platPOSY && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
	if ((x > platEND && x <= screen_width()) && y == platPOSY && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
	
	
}

void LEVEL1_BARRIER(void){
	
	if (land == false){
		tempJumpCount = jumpCount;
	} else {
		
	}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = screen_width()/3;
	int platPOSX = screen_width()/3;
	int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
		
	}
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-HERO_HEIGHT-1;
		jump = false;
		land = false;
	}
	if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true){
		land = false;
		if (leftM == true && leftMRun == false){
			jumpCount = HERO_HEIGHT*3.5;
		} else if (leftM == true && leftMRun == true){
			jumpCount = HERO_HEIGHT*3.5-1;
		}else {
			jumpCount = HERO_HEIGHT*3.5;
		}
		
		jump = false;
	}
	if (x > platEND && x <= screen_width() && y == platPOSY && land == true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
	
	
}

void LEVEL4_BARRIER1(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = screen_width()*0.4;
	int platPOSX = round(sprite_x(platformOne));
	int platPOSY =  round(sprite_y(platformOne));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = false;
	}
	if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
	// if (x > platEND && x <= screen_width() && y == platPOSY && land == true){
		// land = false;
		// jumpCount = HERO_HEIGHT*3.5;
		// jump = false;
	// }
}

void LEVEL4_BARRIER2(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = screen_width()*0.6;
	int platPOSX = round(sprite_x(platformTwo));
	int platPOSY =  round(sprite_y(platformTwo));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = true;
	}
	// if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true){
		// land = false;
		// jumpCount = HERO_HEIGHT*3.5;
		// jump = false;
	// }
	if (x > platEND && x <= screen_width() && y == platPOSY && land == true && jump != true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
}

void LEVEL4_BARRIER3(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int platformLength = screen_width()*0.4;
	int platPOSX = round(sprite_x(platformThree));
	int platPOSY =  round(sprite_y(platformThree));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
		land = true;
	}
	if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true && jump != true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5 +1;
		jump = false;
	}
	// if (x > platEND && x <= screen_width() && y == platPOSY && land == true){
		// land = false;
		// jumpCount = HERO_HEIGHT*3.5;
		// jump = false;
	// }
}

void LEVEL5_BARRIERA(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int bx = round(sprite_x(boulder));
	int by = round(sprite_y(boulder))+2;
	int platformLength = screen_width()*0.5;
	int platPOSX = round(sprite_x(platformLVL5A));
	int platPOSY =  round(sprite_y(platformLVL5A));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	//((bx+4 > platPOSX && bx+4 <= platEND) || (bx >= platPOSX && bx <= platEND)) && by == platPOSY)
	if(((bx+4 > platPOSX && bx+4 <= platEND) || (bx >= platPOSX && bx <= platEND)) && by == platPOSY){
		boulderFALL = false;
		fallCount = 0;
	}	else if (bx+4 < platPOSX && bx >= 0 && by == platPOSY /* && land == true && jump == false */){
		boulderFALL = true;
		fallCount = HERO_HEIGHT*3.5;
	} else if (bx > platEND && bx <= screen_width() && by == platPOSY /* && land == true && jump == false */){
		boulderFALL = true;
		fallCount = HERO_HEIGHT*3.5;
	}
	
	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = false;
	} else if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY && land == true && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	} else if (x > platEND && x <= screen_width() && y == platPOSY && land == true && jump == false){
		land = false;
		jumpCount = HERO_HEIGHT*3.5;
		jump = false;
	}
}

void LEVEL5_BARRIERB(void){
	if (land == false){
		tempJumpCount = jumpCount;
	} else {}
	
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero))+3;
	int bx = round(sprite_x(boulder));
	int by = round(sprite_y(boulder))+2;
	int platformLength = screen_width()*0.5;
	int platPOSX = round(sprite_x(platformLVL5B));
	int platPOSY =  round(sprite_y(platformLVL5B));
	int platEND = platPOSX + platformLength;
	
	if(((x+HERO_WIDTH > platPOSX && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y == platPOSY){
		land = true;
		jumpCount = 0;
	}
	
	if(((bx+4 > platPOSX && bx+4 <= platEND) || (bx >= platPOSX && bx <= platEND)) && by == platPOSY){
		boulderFALL = false;
		fallCount = 0;
	}	else if (bx+4 < platPOSX && bx >= 0 && by == platPOSY /* && land == true && jump == false */){
		boulderFALL = true;
		fallCount = HERO_HEIGHT*3.5;
	} else if (bx > platEND && bx <= screen_width() && by == platPOSY /* && land == true && jump == false */){
		boulderFALL = true;
		fallCount = HERO_HEIGHT*3.5;
	}

	
	if(((x+HERO_WIDTH >= platPOSX+1 && x+HERO_WIDTH <= platEND) || (x >= platPOSX && x <= platEND)) && y-3 == platPOSY ){
		jumpCount = HERO_HEIGHT*3.5-4;
		jump = false;
		land = false;
	}
	if (x+HERO_WIDTH < platPOSX && x >= 0 && y == platPOSY /* && land == true*/ && jump != true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5-1;
		jump = false;
	} 
	if (x > platEND && x <= screen_width() && y == platPOSY /* && land == true*/&& jump != true){
		land = false;
		jumpCount = HERO_HEIGHT*3.5-1;
		jump = false;
	}
}

// Movement for the hero (Left, Right, Jump).
void move_hero(int key){
	int x = round(sprite_x(hero));
	int y = round(sprite_y(hero));
	
	
	// RIGHT
	if (key == 261 && enableKeys == true){
		if (leftM == false && rightM == false && rightMRun == false){
			rightM = true;
			leftM = false;
			rightMRun = false;
			leftMRun = false;
		} else if (leftM == true && rightM == false && leftMRun == false){
			leftM = false;
			rightM = false;
			rightMRun = false;
			leftMRun = false;
		} else if (leftM == false && rightM == true && rightMRun == false) {
			leftM = false;
			rightM = true;
			rightMRun = true;
		} else if (leftM == true && rightM == false && leftMRun == true){
			leftM = true;
			rightM = false;
			rightMRun = false;
			leftMRun = false;
		} else {}
	
	// LEFT
	} else if (key == 260 && enableKeys == true){
		if (rightM == false && leftM == false && leftMRun == false){
			leftM = true;
			rightM = false;
		} else if (leftM == false && rightM == true && rightMRun == false){
			leftM = false;
			rightM = false;
			rightMRun = false;
			leftMRun = false;
		} else if (leftM == true && rightM == false && leftMRun == false){
			leftM = true;
			leftMRun = true;
			rightM = false;
		} else if (leftM == false && rightM == true && rightMRun == true){
			leftM = false;
			rightM = true;
			rightMRun = false;
			leftMRun = false;
		} else {}
	} 
	if(jump == true && jumpCount<jumpHeight){
		sprite_move( hero, 0, -0.5 );
		jumpCount = jumpCount + 0.5;
	} else if ( jump == true && jumpCount == jumpHeight){
		jump = false;
		//sprite_init(hero, x, y, HERO_WIDTH, HERO_HEIGHT, hero_image_fall);
		
	} else if (jump == false && jumpCount <= jumpHeight && jumpCount != 0){
		sprite_move( hero, 0, +0.5 );
		jumpCount = jumpCount - 0.5;
		jump = false;
	} else if (jumpCount == 0){
		enableKeys = true;
	}
	if ( key == 259 && y > 1 && enableKeys == true) {
		jump = true;
		enableKeys = false;
	}
	if ((key == 'l' || key == 'L') && enableKeys == true){
		lvlNum++;
		setup();
	}
	
	
	
	if ( leftM == true && rightM == false && leftMRun == false && x > 1 ) sprite_move( hero, -0.5, 0 ); //Left Movement
	
	if ( rightM == true && leftM == false && rightMRun == false && x < screen_width() - sprite_width(hero) -1 ){ //Right Movement
		sprite_move( hero, +0.5, 0 );
	}
	
	if ( leftM == true && rightM == false && leftMRun == true && x > 1 ) sprite_move( hero, -1, 0 ); //Left Movement Run
	
	if ( rightM == true && leftM == false && rightMRun == true && x < screen_width() - sprite_width(hero) -1 ){ //Right Movement Run
		sprite_move( hero, +1, 0 );	
	} 
}

//BOULDER FALL MOVEMENT
void boulder_Fall(void){
	if (fallCount != 0 && boulderFALL == true){
		sprite_move(boulder, 0, +1);
		fallCount = fallCount - 0.5;
	}
}

//Detect if two sprites have collided.
bool sprites_collided( sprite_id s1, sprite_id s2 ) {
	bool collided = true;
	
	int hx = round(sprite_x(s1));
	int hy = round(sprite_y(s1));
	int hr = hx + HERO_WIDTH - 1;
	int hb = hy + HERO_HEIGHT - 1;
	
	int zx = round(sprite_x(s2));
	int zy = round(sprite_y(s2));
	int zr = zx + ZOMBIE_WIDTH - 1;
	int zb = zy + ZOMBIE_HEIGHT - 1;
	
	if (hr < zx ) { collided = false; }
	if (hb < zy ) { collided = false; }
	
	if (zr < hx ) { collided = false; }
	if (zb < hy ) { collided = false; }
	
	return collided;
}

bool sprites_collided_Gate( sprite_id s1, sprite_id s2 ) {
	bool collided = true;
	
	int hx = round(sprite_x(s1));
	int hy = round(sprite_y(s1));
	int hr = hx + HERO_WIDTH - 1;
	int hb = hy + HERO_HEIGHT - 1;
	
	int zx = round(sprite_x(s2));
	int zy = round(sprite_y(s2));
	int zr = zx + (3*3.5-1) - 1;
	int zb = zy + (3*3.5-1) - 1;
	
	if (hr < zx ) { collided = false; }
	if (hb < zy ) { collided = false; }
	
	if (zr < hx ) { collided = false; }
	if (zb < hy ) { collided = false; }
	
	return collided;
}

void treasureCheck(void){
	if (treasureTaken == true){
			score = score - 100;
			treasureTaken = false;
		} else {}
}

void fallCheck(void){
	int y = round(sprite_y(hero));
	if (y >= screen_height()){
		treasureCheck();
		lives--;
		treasureTaken = false;
		clear_screen();
		setup();
		leftM = false;
		leftMRun = false;
		rightM = false;
		rightMRun = false;
		fallCount = 0;
		
	}
}

void fallCheckBOULDER(void){
	int by = round(sprite_y(boulder));
	boulderX = round(sprite_x(boulder));
	if (by >= screen_height()){
		
		sprite_destroy(boulder);
		//sprite_move(boulder, boulderX, 5);
		boulder = sprite_create( boulderX, 5, 4, 2, boulder_image);
		fallCount = screen_height() - (2*(HERO_HEIGHT*3.5));
		
	}
}

// Play one turn of game.
void process(void) {
	
	int key = get_char();
	move_hero(key);	
	
	counterMill++;
	if (counterMill == 100){
		timerSec++;
		counterMill = 0;
	} if (timerSec == 60){
		timerSec = 00;
		timerMin++;
	}
    //	Quit
	if ( key == 'q'){
		game_over = true;
	}
	clear_screen();
	
	if (lvlNum == 1){
		
		move_zombie( zombie );
		int platPOSX = screen_width()/3;
		int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
		LEVEL1_BARRIER();
		DRAW_PLATFORM(platPOSX, platPOSY);
		sprite_draw(door);
		WORLDBORDER();
		sprite_draw(zombie);
		sprite_draw(hero);
	} else if (lvlNum == 2){
		int platPOSX = screen_width()/3;
		int platPOSY =  (screen_height()-(HERO_HEIGHT*3.5));
		DRAW_PLATFORM(platPOSX, platPOSY);
		LEVEL1_BARRIER();
		WORLDBORDERLVL2();
		sprite_draw(door);
		sprite_draw(bat);
		move_zombie(bat);
		sprite_draw(treasure);
		PIT();
		fallCheck();	
		sprite_draw(hero);
	} else if (lvlNum == 3){
		WORLDBORDERLVL2();
		LEVEL3_BARRIER();
		LEVEL3_BARRIER_SMALL();
		sprite_draw(treasure);
		sprite_draw(platform);
		move_zombie(platform);
		sprite_draw(platformSmall);
		PIT();
		fallCheck();
		sprite_draw(door);
		sprite_draw(hero);
	} else if (lvlNum == 4){
		LEVEL4_BARRIER1();
		LEVEL4_BARRIER2();
		LEVEL4_BARRIER3();
		WORLDBORDER();
		sprite_draw(platformOne);
		sprite_draw(platformTwo);
		sprite_draw(platformThree);
		sprite_draw(gate);
		sprite_draw( key_treasure );
		sprite_draw(door);	
		sprite_draw(hero);
	} else if (lvlNum == 5){
		WORLDBORDERLVL5();
		LEVEL5_BARRIERA();
		LEVEL5_BARRIERB();
		sprite_draw(platformLVL5A);
		move_zombie(platformLVL5A);
		sprite_draw(platformLVL5B);
		move_platformLVL5B();
		PIT_LVL5();
		fallCheck();
		sprite_draw(amulet);
		sprite_draw(boulder);
		move_boulder();
		boulder_Fall();
		fallCheckBOULDER();
		sprite_draw(hero);
	} else {
		if (lives == 0){
			draw_formatted((screen_width()/2 - 35/2),screen_height()/2, "GAME OVER! \n Press any key to exit.", score);
		} else {
			draw_formatted((screen_width()/2 - 61/2),screen_height()/2, "GAME OVER! \n Your final score was: %2d \n Press any key to exit.", score);
		
		}
		//getchar();
		char cha;
		scanf("%c",&cha);
		game_over = true;
		// int mint = get_char;
		// if (mint != NULL){
			// game_over = true;
		// }
	}
	
	COLLISION();	
	gui();
	WORLD_BARRIER();
}

// Program entry point.
int main(int argc, char *argv[]) {
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