#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include "Audio.h"
#include "Pattern.h"
#include "PatternEditor.h"
#include "Keyboard.h"
#include "Screen.h"

PatternEditorMode editor_mode = PatternEditorMode::EDIT;
Pattern* current_pattern = nullptr;
int current_pattern_index = 0;
int patterns_active = 0;
int editor_row = 0;
int editor_octave = 3;
int editor_channel = 0;
bool is_editor_jamming = false;
int preview_channel = 0;
int preview_note = -1;
int preview_instrument = -1;
int preview_time = 0;
int editor_channel_column = PatternEditorChannelColumn::NOTE;
int first_row_to_render = 0;

void initializePatternEditor() {

}


void playPattern() {
    resetPlaybackVariablesFull();
    editor_mode = PatternEditorMode::PLAY;
}

void stopPlaying() {
    editor_mode = PatternEditorMode::EDIT;
}

void inputNote(SDL_Scancode scancode) {
    int note = keyToNote(scancode);
    if (note == -1) return;

    if (note > -1) {
        if (note != NOTE_BLANK && note != NOTE_CUT) {
            note = ((editor_octave * 12) + note);
            preview_note = note;
            preview_time = SAMPLE_RATE;
            is_editor_jamming = true;
            preview_channel = editor_channel;
            preview_instrument = current_pattern->getCellInstrument(editor_row, editor_channel);
            if (preview_instrument == INSTRUMENT_BLANK) {
                current_pattern->setCellInstrument(editor_row, editor_channel, 0);
                preview_instrument = 0;
            }
            if (current_pattern->getCellVolume(editor_row, editor_channel) == VOLUME_BLANK) {
                current_pattern->setCellVolume(editor_row, editor_channel, MAX_VOLUME);
            }
        }
        current_pattern->setCellNote(editor_row, editor_channel, note);

        row = editor_row;
    }
}

void inputVolume(SDL_Scancode scancode) {
    int vol = keyToValue(scancode, false);
    if (vol == -1) return;

    if (vol > -1) {
        long long cell = current_pattern->getCell(editor_row, editor_channel);
        int volume = (cell & VOLUME_MASK) >> 13;
        if (volume == VOLUME_BLANK) volume = 0;
        int editing_second_digit = (cell & VOLUME_EDIT_MASK) >> 33;
        if (editing_second_digit == 0) {
            if (volume > 0) {
                volume /= 10;
                volume *= 10;
            }
            volume += vol;
            if (volume > MAX_VOLUME) volume = MAX_VOLUME;
            cell &= ~(VOLUME_MASK);
            cell |= (volume << 13);
            cell |= (VOLUME_EDIT_MASK);
        }
        else {
            volume *= 10;
            volume %= 100;
            volume += vol;
            if (volume > MAX_VOLUME) volume = MAX_VOLUME;
            cell &= ~(VOLUME_MASK);
            cell |= (volume << 13);
            cell &= ~(VOLUME_EDIT_MASK);
        }
        current_pattern->setCell(editor_row, editor_channel, cell);
    }
}

void inputEffectType(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_A: //volume slide
            current_pattern->setCellEffectType(editor_row, editor_channel, EffectType::VOLUMESLIDE);
            break;
        case SDL_SCANCODE_F: //set speed
            current_pattern->setCellEffectType(editor_row, editor_channel, EffectType::SPEED);
            break;
    }
}

void moveUp() {
    if (editor_mode == PatternEditorMode::EDIT) {
        editor_row--;
        if (editor_row < 0) {
            editor_row = current_pattern->row_count - 1;
            if (--current_pattern_index < 0) current_pattern_index = patterns_active - 1;
            current_pattern = &patterns[current_pattern_index];
        }
    }
}
void moveDown() {
    if (editor_mode == PatternEditorMode::EDIT) {
        editor_row++;
        if (editor_row >= current_pattern->row_count) {
            editor_row = 0;
            if (++current_pattern_index == patterns_active) current_pattern_index = 0;
            current_pattern = &patterns[current_pattern_index];
        }
    }
}
void moveLeft() {
    if (editor_mode == PatternEditorMode::EDIT) {
        editor_channel_column--;
        if (editor_channel_column < 0) {
            editor_channel_column = PatternEditorChannelColumn::EFFECT3;
            editor_channel--;
            if (editor_channel < 0) editor_channel = MAX_CHANNELS - 1;
        }
    }
}
void moveRight() {
    if (editor_mode == PatternEditorMode::EDIT) {
        editor_channel_column++;
        if (editor_channel_column > PatternEditorChannelColumn::EFFECT3) {
            editor_channel_column = 0;
            editor_channel++;
            if (editor_channel >= MAX_CHANNELS) editor_channel = 0;
        }
    }
}

void deleteNote() {
    if (current_pattern->getCellNote(editor_row, editor_channel) != NOTE_BLANK) {
        if (current_pattern->getCellInstrument(editor_row, editor_channel) != INSTRUMENT_BLANK)
            current_pattern->setCellInstrument(editor_row, editor_channel, INSTRUMENT_BLANK);
        if (current_pattern->getCellVolume(editor_row, editor_channel) != VOLUME_BLANK)
            current_pattern->setCellVolume(editor_row, editor_channel, VOLUME_BLANK);
        current_pattern->setCellNote(editor_row, editor_channel, NOTE_BLANK);
        editor_row++;
        if (editor_row >= current_pattern->row_count) editor_row = 0;
    }
}

void deleteInstrument() {
    if (current_pattern->getCellInstrument(editor_row, editor_channel) != INSTRUMENT_BLANK) {
        current_pattern->setCellInstrument(editor_row, editor_channel, INSTRUMENT_BLANK);
        editor_row++;
        if (editor_row >= current_pattern->row_count) editor_row = 0;
    }
}

void deleteVolume() {
    if (current_pattern->getCellVolume(editor_row, editor_channel) != VOLUME_BLANK) {
        current_pattern->setCellVolume(editor_row, editor_channel, VOLUME_BLANK);
        editor_row++;
        if (editor_row >= current_pattern->row_count) editor_row = 0;
    }
}

void deleteEffectType() {
    if (current_pattern->getCellEffectType(editor_row, editor_channel) != EffectType::NONE) {
        current_pattern->setCellEffectType(editor_row, editor_channel, EffectType::NONE);
        editor_row++;
        if (editor_row >= current_pattern->row_count) editor_row = 0;
    }
}
void handlePatternEditorKeyDown(SDL_Event& event) {
    if (editor_channel_column == PatternEditorChannelColumn::NOTE) {
        inputNote(event.key.scancode);
    }
    else if (editor_channel_column == PatternEditorChannelColumn::INSTRUMENT) {
        //Instrument inputting
        int ins = keyToInstrument(event.key.scancode);
        if (ins > -1) {
            current_pattern->setCellInstrument(editor_row, editor_channel, ins);
        }
    }
    else if (editor_channel_column == PatternEditorChannelColumn::VOLUME) {
        inputVolume(event.key.scancode);
    }
    else if (editor_channel_column == PatternEditorChannelColumn::EFFECT1) {
        inputEffectType(event.key.scancode);
    }
    else if (editor_channel_column == PatternEditorChannelColumn::EFFECT2) {
        int value = keyToValue(event.key.scancode, true);
        if (value > -1) current_pattern->setCellEffectOne(editor_row, editor_channel, value);
    }
    else if (editor_channel_column == PatternEditorChannelColumn::EFFECT3) {
        int value = keyToValue(event.key.scancode, true);
        if (value > -1) current_pattern->setCellEffectTwo(editor_row, editor_channel, value);
    }
    if (event.key.scancode == SDL_SCANCODE_DELETE) {
        switch (editor_channel_column) {
        case PatternEditorChannelColumn::NOTE: deleteNote(); break;
        case PatternEditorChannelColumn::INSTRUMENT: deleteInstrument(); break;
        case PatternEditorChannelColumn::VOLUME: deleteVolume(); break;
        case PatternEditorChannelColumn::EFFECT1: deleteEffectType(); break;
        }
    }
    else if (event.key.scancode == SDL_SCANCODE_DOWN) {
        moveDown();
    }
    else if (event.key.scancode == SDL_SCANCODE_UP) {
        moveUp();
    }
    else if (event.key.scancode == SDL_SCANCODE_LEFT) {
        moveLeft();
    }
    else if (event.key.scancode == SDL_SCANCODE_RIGHT) {
        moveRight();
    }
    else if (event.key.scancode == 84) { //Octave down (/)
        editor_octave--;
        if (editor_octave < 0) editor_octave = 0;
    }
    else if (event.key.scancode == 85) { //Octave up (*)
        editor_octave++;
        if (editor_octave > 8) editor_octave = 8;
    }
    else if (event.key.scancode == SDL_SCANCODE_SPACE) {
        if (editor_mode == PatternEditorMode::EDIT) playPattern();
        else stopPlaying();
    }
}

void handlePatternEdtiorMouseButtonDown(SDL_Event& event, Screen& current_screen) {
    if (event.button.button == 1) {
        if (event.button.x >= 760 && event.button.x <= 768 && event.button.y >= 0 && event.button.y <= 8) {
            //Add a pattern
            patterns[patterns_active].active = true;
            patterns_active++;
        }
        if (event.button.x >= 768 && event.button.x <= 774 && event.button.y >= 0 && event.button.y <= 8) {
            //Remove a pattern
            if (patterns_active > 1) {
                patterns[patterns_active - 1].active = false;
                patterns_active--;
            }
        }
        if (event.button.x >= 670 && event.button.x <= 766 && event.button.y >= 0 && event.button.y <= 8) {
            current_screen = Screen::INSTRUMENTEDITOR;
        }
    }
}

void drawPattern(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font, int x, int y) {
    //----Draw current pattern----
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
        for (int r = 0; r < current_pattern->row_count; r++) {
            long long cell = current_pattern->getCell(r, ch);
            int dx = (ch * 80) + x;
            bool same_row = editor_mode == PatternEditorMode::EDIT && r == editor_row;
            bool beat_row = r % 4 == 0;

            //--Note and octave--
            TTF_Text* text = TTF_CreateText(text_engine, font, getNoteName(cell & NOTE_MASK), 6 * sizeof(char));
            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::NOTE))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_DrawRendererText(text, dx, (r * 8) - (first_row_to_render * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);

            //--Instrument--
            char str[3];
            int ins = current_pattern->getCellInstrument(r, ch);
            if (ins < INSTRUMENT_BLANK) sprintf_s(str, "%.2d", ins);
            else  sprintf_s(str, "--");
            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::INSTRUMENT))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str, 3 * sizeof(char));
            TTF_DrawRendererText(text, dx + (24), (r * 8) - (first_row_to_render * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);

            //--Volume--
            char str2[6];
            int vol = current_pattern->getCellVolume(r, ch);
            if (vol != VOLUME_BLANK) sprintf_s(str2, "%.2d", vol);
            else sprintf_s(str2, "--");
            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::VOLUME))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str2, 3 * sizeof(char));
            TTF_DrawRendererText(text, dx + (40), (r * 8) - (first_row_to_render * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);

            //--Effect 1--
            char str3[6];
            sprintf_s(str3, getEffectTypeString((cell & EFFECT_TYPE_MASK) >> 20));

            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT1))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str3, 2);
            TTF_DrawRendererText(text, dx + (56), (r * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);
            //--Effect 2--
            sprintf_s(str3, "%x", current_pattern->getCellEffectOne(r, ch));

            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT2))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str3, 2);
            TTF_DrawRendererText(text, dx + (62), (r * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);

            //--Effect 3--
            sprintf_s(str3, "%x", current_pattern->getCellEffectTwo(r, ch));

            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT3))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str3, 2);
            TTF_DrawRendererText(text, dx + (68), (r * 8) + y);
            TTF_SetTextColor(text, 255, 255, 255, 255);


            //Row number
            if (ch == 0) {
                char str4[3];
                sprintf_s(str4, "%.2d", r);

                if (beat_row)
                    TTF_SetTextColor(text, 255, 255, 100, 255);
                else
                    TTF_SetTextColor(text, 255, 255, 255, 255);
                TTF_SetTextString(text, str4, 3 * sizeof(char));
                TTF_DrawRendererText(text, dx - 16, (r * 8) + y);
            }

            TTF_DestroyText(text);

            //White divider line
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, dx + (78), (r * 8) + y, dx + 78, (r * 8) + 8 + y);
        }
    }

    //---Draw pattern list----
    TTF_Text* text = TTF_CreateText(text_engine, font, "--", 3 * sizeof(char));
    for (int i = 0; i < MAX_PATTERNS; i++) {
        char str[3];
        sprintf_s(str, "%.2d", i);
        TTF_SetTextString(text, str, 3);
        if (patterns[i].active) {
            if (i == current_pattern_index)
                TTF_SetTextColor(text, 0, 255, 0, 255);
            else
                TTF_SetTextColor(text, 255, 255, 255, 255);
        }
        else
            TTF_SetTextColor(text, 128, 128, 128, 255);
        TTF_DrawRendererText(text, 784, i * 8);
    }

    //----TEMPORARY UI----
    //Add and remove a pattern (+ and -)
    TTF_SetTextColor(text, 255, 255, 255, 255);
    TTF_SetTextString(text, "+", 2);
    TTF_DrawRendererText(text, 760, 0);
    TTF_SetTextString(text, "-", 2);
    TTF_DrawRendererText(text, 768, 0);
    
    //Go to the instruments screen
    TTF_SetTextString(text, "Instruments", 12);
    TTF_DrawRendererText(text, 670, 0);
}