void HELP (void ) {
	int now = ( overflow_counter * 65536 + TCNT3)  * 64 / 8e6;
	if (BIT_IS_SET(PINB, 0)) {
		clear_screen();
		draw_string(20, 5, "Score:", FG_COLOUR);
		draw_int(55,5, score, FG_COLOUR);
		draw_string(20, 15, "Lives:", FG_COLOUR);
		draw_int(55, 15, lives, FG_COLOUR);
		draw_string(20, 25, "Floor:", FG_COLOUR);
		draw_int(55, 25, level, FG_COLOUR);
		draw_string(20, 35, "Time:", FG_COLOUR);
		if (now == 60) {
			minute++; 
			now = 0;
			overflow_counter = 0;
		}
		if (now < 10) {
			draw_string(65, 35,":0", FG_COLOUR);
			draw_int(75, 35, now, FG_COLOUR);
		} else if (now < 60) {
			draw_string(65, 35, ":", FG_COLOUR);
			draw_int(70, 35, now, FG_COLOUR);
		}
		if (minute == 0) {
			draw_string(55, 35, "00", FG_COLOUR);
		} else if (minute > 0) {
			draw_string(55, 35, "0", FG_COLOUR);
			draw_int(60, 35, minute, FG_COLOUR);
		}

	}
}