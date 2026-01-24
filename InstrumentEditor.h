#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Screen.h"

extern SDL_Texture* sine_texture;
extern SDL_Texture* square_texture;
extern SDL_Texture* saw_texture;
extern SDL_Texture* tri_texture;
extern SDL_Texture* envelope_bar_texture;

void handleInstrumentEditorMouseButtonDown(SDL_Event& event, Screen& current_screen);
void drawInstrumentEditor(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font);