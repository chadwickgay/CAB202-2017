void hero_jump(int key){
	double dx = sprite_dx(hero);
	int hy = round(sprite_y(hero));

	if (key == 259 && enableKeys == true){
		jumpUp = true;
	}

	if (jumpUp == true && jumpHeight < maxJumpHeight){
		//sprite_move(hero, 0, -0.5);	
		sprite_turn_to(hero, dx, -0.5);
		sprite_step(hero);
		jumpHeight = jumpHeight + 0.5;
		enableKeys = false;
	}
	if (jumpUp == true && jumpHeight == maxJumpHeight){
		jumpUp = false;
		jumpDown = true;
	}
	if (jumpDown == true && jumpUp == false && jumpHeight <= maxJumpHeight){	
		//sprite_move(hero, 0, +0.5);
		sprite_turn_to(hero, dx, 0.5);
		sprite_step(hero);
		jumpHeight = jumpHeight - 0.5;

		// Stops sprite falling through the floor
		if (hy == screen_height() - 1 - HERO_HEIGHT){
			jumpHeight = 0;
		}
	}
	
	if (jumpHeight == 0){
		jumpUp = false;
		jumpDown = false;
		enableKeys = true;
	}
}