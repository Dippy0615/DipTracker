#include <SDL3/SDL.h>
#include "Constants.h"
#include "Keyboard.h"

int keyToNote(SDL_Scancode scancode) {
	switch (scancode) {
	//Bottom octave
	case SDL_SCANCODE_Z:		 return 0; //C
	case SDL_SCANCODE_S:		 return 1; //C#
	case SDL_SCANCODE_X:		 return 2; //D
	case SDL_SCANCODE_D:		 return 3; //D#
	case SDL_SCANCODE_C:		 return 4; //E
	case SDL_SCANCODE_V:		 return 5; //F
	case SDL_SCANCODE_G:		 return 6; //F#
	case SDL_SCANCODE_B:		 return 7; //G
	case SDL_SCANCODE_H:		 return 8; //G#
	case SDL_SCANCODE_N:		 return 9; //A
	case SDL_SCANCODE_J:		 return 10; //A#
	case SDL_SCANCODE_M:		 return 11; //B
	case SDL_SCANCODE_COMMA:	 return 12; //C+1
	case SDL_SCANCODE_L:		 return 13; //C#+1
	case SDL_SCANCODE_PERIOD:	 return 14; //D+1
	case SDL_SCANCODE_SEMICOLON: return 15; //D#+1
	case SDL_SCANCODE_SLASH:	 return 16; //E+1
	case 47: return 17; //F+1
	case 46: return 18; //F#+1
	case 48: return 19; //G+1
	//Top octave
	case SDL_SCANCODE_Q: return 12; //C+1
	case SDL_SCANCODE_2: return 13; //C#+1
	case SDL_SCANCODE_W: return 14; //D+1
	case SDL_SCANCODE_3: return 15; //D#+1
	case SDL_SCANCODE_E: return 16; //E+1
	case SDL_SCANCODE_R: return 17; //F+1
	case SDL_SCANCODE_5: return 18; //F#+1
	case SDL_SCANCODE_T: return 19; //G+1
	case SDL_SCANCODE_6: return 20; //G#+1
	case SDL_SCANCODE_Y: return 21; //A+1
	case SDL_SCANCODE_7: return 22;  //A#+1
	case SDL_SCANCODE_U: return 23;  //B+1
	case SDL_SCANCODE_I: return 24;  //C+2
	case SDL_SCANCODE_9: return 25;  //C#+2
	case SDL_SCANCODE_O: return 26;  //D+2
	case SDL_SCANCODE_0: return 27;  //D#+2
	case SDL_SCANCODE_P: return 28;  //E+2
	case SDL_SCANCODE_1: return NOTE_CUT;
	}
	return -1;
}

int keyToValue(SDL_Scancode scancode, bool hex) {
	switch (scancode) {
		case SDL_SCANCODE_0: return 0;
		case SDL_SCANCODE_1: return 1;
		case SDL_SCANCODE_2: return 2;
		case SDL_SCANCODE_3: return 3;
		case SDL_SCANCODE_4: return 4;
		case SDL_SCANCODE_5: return 5;
		case SDL_SCANCODE_6: return 6;
		case SDL_SCANCODE_7: return 7;
		case SDL_SCANCODE_8: return 8;
		case SDL_SCANCODE_9: return 9;
		if (hex) {
			case SDL_SCANCODE_A: return 10;
			case SDL_SCANCODE_B: return 11;
			case SDL_SCANCODE_C: return 12;
			case SDL_SCANCODE_D: return 13;
			case SDL_SCANCODE_E: return 14;
			case SDL_SCANCODE_F: return 15;
		}
	}
	return -1;
}

int keyToInstrument(SDL_Scancode scancode) {
	switch (scancode) {
		case SDL_SCANCODE_0: return 0;
		case SDL_SCANCODE_1: return 1;
		case SDL_SCANCODE_2: return 2;
		case SDL_SCANCODE_3: return 3;
		case SDL_SCANCODE_4: return 4;
	}
	return -1;
}