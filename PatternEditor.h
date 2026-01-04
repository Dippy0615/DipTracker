#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include "Pattern.h"

enum PatternEditorMode {
	EDIT,
	PLAY
};

enum PatternEditorChannelColumn {
	NOTE,
	INSTRUMENT,
	VOLUME,
	EFFECT1,
	EFFECT2,
	EFFECT3,
	SIZE
};

extern PatternEditorMode editor_mode;
extern Pattern* current_pattern;
extern int current_pattern_index;
extern int patterns_active;
extern int editor_row;
extern int editor_octave;
extern int editor_channel;
extern int editor_channel_column;
extern bool is_editor_jamming;
extern int preview_channel;
extern int preview_note;
extern int preview_instrument;
extern int preview_time;
extern int first_row_to_render;
void initializePatternEditor();
void playPattern();
void stopPlaying();
void inputNote(SDL_Scancode scancode);
void inputVolume(SDL_Scancode scancode);
void inputEffectType(SDL_Scancode scancode);
void moveUp();
void moveDown();
void moveLeft();
void moveRight();
void deleteNote();
void deleteInstrument();
void deleteVolume();
void deleteEffectType();
void drawPattern(SDL_Renderer* renderer,TTF_TextEngine* text_engine, TTF_Font* font, int x);
