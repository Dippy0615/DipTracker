#pragma once
#include "Pattern.h"

enum class PatternEditorMode {
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
extern int editor_row;
extern int editor_octave;
extern int editor_channel;
extern int editor_channel_column;
extern bool is_editor_jamming;
extern int preview_channel;
void initializePatternEditor();
void playPattern();
void stopPlaying();