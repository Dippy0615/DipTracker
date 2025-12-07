#include <SDL3/SDL.h>
#include "Audio.h"
#include "Pattern.h"
#include "PatternEditor.h"

PatternEditorMode editor_mode = PatternEditorMode::EDIT;
Pattern* current_pattern = nullptr;
int editor_row = 0;
int editor_octave = 3;
int editor_channel = 0;
bool is_editor_jamming = false;
int preview_channel = 0;
int preview_note = -1;
int preview_instrument = -1;
int editor_channel_column = PatternEditorChannelColumn::NOTE;

void initializePatternEditor() {

}

void playPattern() {
	resetPlaybackVariablesFull();
	editor_mode = PatternEditorMode::PLAY;
}

void stopPlaying() {
	editor_mode = PatternEditorMode::EDIT;
}