#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include "Audio.h"
#include "Pattern.h"
#include "PatternEditor.h"

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
int editor_channel_column = PatternEditorChannelColumn::NOTE;
int first_row_to_render = 0;

void initializePatternEditor() {

}

void drawPattern(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font, int x) {
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

            TTF_DrawRendererText(text, dx, (r * 8) - (first_row_to_render * 8));
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
            TTF_DrawRendererText(text, dx + (24), (r * 8) - (first_row_to_render * 8));
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
            TTF_DrawRendererText(text, dx + (40), (r * 8) - (first_row_to_render * 8));
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
            TTF_DrawRendererText(text, dx + (56), (r * 8));
            TTF_SetTextColor(text, 255, 255, 255, 255);
            //--Effect 2--
            sprintf_s(str3, "%x", current_pattern->getCellEffectOne(r, ch));

            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT2))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str3, 2);
            TTF_DrawRendererText(text, dx + (62), (r * 8));
            TTF_SetTextColor(text, 255, 255, 255, 255);

            //--Effect 3--
            sprintf_s(str3, "%x", current_pattern->getCellEffectTwo(r, ch));

            //Highlight
            if (same_row)
                TTF_SetTextColor(text, 150, 150, 150, 255);
            if ((editor_mode == PatternEditorMode::PLAY && r == row) || (same_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT3))
                TTF_SetTextColor(text, 255, 0, 0, 255);

            TTF_SetTextString(text, str3, 2);
            TTF_DrawRendererText(text, dx + (68), (r * 8));
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
                TTF_DrawRendererText(text, dx - 16, r * 8);
            }

            TTF_DestroyText(text);

            //White divider line
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, dx + (78), r * 8, dx + 78, (r * 8) + 8);
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

    //Add and remove a pattern (+ and -)
    TTF_SetTextColor(text, 255, 255, 255, 255);
    TTF_SetTextString(text, "+", 2);
    TTF_DrawRendererText(text, 760, 0);
    TTF_SetTextString(text, "-", 2);
    TTF_DrawRendererText(text, 768, 0);
}

void playPattern() {
	resetPlaybackVariablesFull();
	editor_mode = PatternEditorMode::PLAY;
}

void stopPlaying() {
	editor_mode = PatternEditorMode::EDIT;
}