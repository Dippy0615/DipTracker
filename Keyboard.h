#pragma once
#include <SDL3/SDL.h>

int keyToNote(SDL_Scancode scancode);
int keyToValue(SDL_Scancode scancode, bool hex);
int keyToInstrument(SDL_Scancode scancode);