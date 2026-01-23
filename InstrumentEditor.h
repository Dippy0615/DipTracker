#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Screen.h"

void handleInstrumentEditorMouseButtonDown(SDL_Event& event, Screen& current_screen);
void drawInstrumentEditor(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font);