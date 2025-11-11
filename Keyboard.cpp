#include <SDL3/SDL.h>
#include "Keyboard.h"

int keyToNote(SDL_Scancode scancode) {
	switch (scancode) {
	case SDL_SCANCODE_Z:		case SDL_SCANCODE_Q: return 0; //C
	case SDL_SCANCODE_S:		case SDL_SCANCODE_2: return 1; //C#
	case SDL_SCANCODE_X:		case SDL_SCANCODE_W: return 2; //D
	case SDL_SCANCODE_D:		case SDL_SCANCODE_3: return 3; //D#
	case SDL_SCANCODE_C:		case SDL_SCANCODE_E: return 4; //E
	case SDL_SCANCODE_V:		case SDL_SCANCODE_R: return 5; //F
	case SDL_SCANCODE_G:		case SDL_SCANCODE_5: return 6; //F#
	case SDL_SCANCODE_B:		case SDL_SCANCODE_T: return 7; //G
	case SDL_SCANCODE_H:		case SDL_SCANCODE_6: return 8; //G#
	case SDL_SCANCODE_N:		case SDL_SCANCODE_Y: return 9; //A
	case SDL_SCANCODE_J:		case SDL_SCANCODE_7: return 10; //A#
	case SDL_SCANCODE_M:		case SDL_SCANCODE_U: return 11; //B
	case SDL_SCANCODE_COMMA:	case SDL_SCANCODE_I: return 12; //C+1
	case SDL_SCANCODE_L:		case SDL_SCANCODE_9: return 13; //C#+1
	case SDL_SCANCODE_PERIOD:	case SDL_SCANCODE_O: return 14; //D+1
	case SDL_SCANCODE_SEMICOLON:case SDL_SCANCODE_0: return 15; //D#+1
	case SDL_SCANCODE_SLASH:	case SDL_SCANCODE_P: return 16; //E+1
	case 47: return 17; //F+1
	case 46: return 18; //F#+1
	case 48: return 19; //G+1
	}
	return -1;
}