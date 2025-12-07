#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <iostream>
#include "Oscillator.h"
#include "OscillatorType.h"
#include "Constants.h"
#include "Channel.h"
#include "Pattern.h"
#include "Audio.h"
#include "PatternEditor.h"
#include "Keyboard.h"

Pattern pattern;
float samples_per_tick = SAMPLE_RATE / 60.0f;
int ticks_per_row = 6;
bool running = true;
int first_row_to_render = 0;
int preview_time = 0;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* font = NULL;
static TTF_TextEngine* text_engine = NULL;

void audioStreamCallback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount) {
    // Here you are expected to provide `additionalAmount` bytes of audio to the stream
    float* buffer = (float*)SDL_malloc(additionalAmount);
    int samples_to_go = additionalAmount / (sizeof(float) * 2);  // 2 channels
    
    for (int i = 0; i < samples_to_go; i++) {
        float sampleL = 0;
        float sampleR = 0;

        if(editor_mode==PatternEditorMode::PLAY){
            for (int i = 0; i < MAX_CHANNELS; i++) {
               float left = 0.0f;
               float right = 0.0f;
                
               int note = pattern.getCellNote(row, i);
               int volume = pattern.getCellVolume(row, i);
               int instrument = pattern.getCellInstrument(row, i);
               if (volume > -1) {
                   channels[i].my_oscillator.SetVolume((float)volume / MAX_VOLUME);
               }
               if (instrument!=INSTRUMENT_BLANK && instrument != (int)channels[i].my_oscillator.GetOscillatorType()) {
                   channels[i].my_oscillator.SetOscillatorType((OscillatorType)instrument);
               }

               if (note == NOTE_BLANK && channels[i].note == NOTE_BLANK) channels[i].note = NOTE_CUT;
               if(note!=NOTE_BLANK) channels[i].note = note;
               if (channels[i].note != NOTE_CUT) channels[i].PlayOscillator(left, right);
           
               sampleL += left;
               sampleR += right;
            }
        }
        else if (is_editor_jamming) {
            float left = 0.0f;
            float right = 0.0f;
            channels[preview_channel].note = preview_note;
            channels[preview_channel].my_oscillator.SetOscillatorType((OscillatorType)preview_instrument);
            channels[preview_channel].PlayOscillator(left, right);
            sampleL += left;
            sampleR += right;
        }
        sampleL *= 0.5f;
        sampleR *= 0.5f;

        tick_counter++;

        if(editor_mode==PatternEditorMode::PLAY){
            if (tick_counter >= samples_per_tick) {
                tick_counter -= samples_per_tick;
                tick++;
                if (tick == ticks_per_row) {
                    tick = 0;
                    row++;
                    //if(row>pattern.row_count/2) first_row_to_render++;
                    if (row == pattern.row_count) {
                        row = 0;
                        first_row_to_render = 0;
                    }
                }

            }
        }
        
        if (--preview_time < 0) {
            preview_time = 0;
            is_editor_jamming = false;
        }
        buffer[i * 2 + 0] = sampleL; // Left
        buffer[i * 2 + 1] = sampleR; // Right
    }

    SDL_PutAudioStreamData(stream, buffer, additionalAmount);
    SDL_free(buffer);
}

void cleanUp() {
    TTF_CloseFont(font);
    TTF_DestroyRendererTextEngine(text_engine);
    pattern.freePattern();
    SDL_DestroyAudioStream(audio_stream);
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("DipTracker", 800, 450, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetWindowResizable(window, true);
    SDL_SetWindowFocusable(window, true);

    if (!TTF_Init()) {
        SDL_Log("Couldn't initalize SDL_TTF: %s", SDL_GetError());
    }
    font = TTF_OpenFont("protracker.ttf", 8);
    if (font == NULL) {
        SDL_Log("Couldn't open font: %s", SDL_GetError());
    }
    text_engine = TTF_CreateRendererTextEngine(renderer);
    if (text_engine == NULL) {
        SDL_Log("Couldn't create text engine: %s", SDL_GetError());
    }

    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = SAMPLE_RATE;

    SDL_AudioStream* audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audioStreamCallback, nullptr);
    if (!audio_stream) {
        SDL_Log("SDL_OpenAudioDeviceStream failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    populateNoiseTable();
    initializeChannels();
    current_pattern = &pattern;

    //SMB1 TEST
    //pattern = Pattern::Pattern();
    /*
    pattern.setCell(0, 0, 0b0011100); //28 (E)
    pattern.setCell(1, 0, 0b0011100); //28 (E)
    pattern.setCell(2, 0, 0b1111111); //127 (OFF)
    pattern.setCell(3, 0, 0b0011100); //28 (E)
    pattern.setCell(4, 0, 0b1111111); //127 (OFF)
    pattern.setCell(5, 0, 0b0011000); //24 (C)
    pattern.setCell(6, 0, 0b0011100); //28 (E)
    pattern.setCell(7, 0, 0b1111111); //127 (OFF)
    pattern.setCell(8, 0, 0b0011111); //31 (G)
    pattern.setCell(12,0, 0b1111111); //127 (OFF)

    pattern.setCell(0, 1, 0b1100); //12 (C)
    pattern.setCell(1, 1, 0b1100); //12 (C)
    pattern.setCell(2, 1, 0b1100); //12 (C)
    pattern.setCell(3, 1, 0b1100); //12 (C)
    pattern.setCell(4, 1, 0b1100); //12 (C)
    pattern.setCell(5, 1, 0b1100); //12 (C)
    pattern.setCell(6, 1, 0b1100); //12 (C)
    pattern.setCell(7, 1, 0b1100); //12 (C)
    pattern.setCell(8, 1, 0b10011); //19 (G)
    pattern.setCell(12, 1, 0b1111111); //127 (OFF)
    */
    SDL_ResumeAudioStreamDevice(audio_stream);
    
    while (running) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                running = false;
            }
            if (event.type == SDL_EVENT_WINDOW_MINIMIZED) {
                SDL_MinimizeWindow(window);
            }
            if (event.type == SDL_EVENT_WINDOW_MAXIMIZED) {
                SDL_MaximizeWindow(window);
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if(editor_channel_column==PatternEditorChannelColumn::NOTE){
                    //Note inputting
                    int note = keyToNote(event.key.scancode);
                    if (note > -1) {
                        if (note != NOTE_BLANK && note != NOTE_CUT) {
                            note = ((editor_octave * 12) + note);
                            preview_note = note;
                            preview_time = SAMPLE_RATE;
                            is_editor_jamming = true;
                            preview_channel = editor_channel;
                            preview_instrument = pattern.getCellInstrument(editor_row, editor_channel);
                            if (preview_instrument == INSTRUMENT_BLANK) {
                                pattern.setCellInstrument(editor_row, editor_channel, 0);
                                preview_instrument = 0;
                            }
                        }
                        pattern.setCellNote(editor_row, editor_channel, note);
                        
                        row = editor_row;
                    }
                }
                else if (editor_channel_column == PatternEditorChannelColumn::INSTRUMENT) {
                    //Instrument inputting
                    int ins = keyToInstrument(event.key.scancode);
                    if (ins > -1) {
                        pattern.setCellInstrument(editor_row, editor_channel, ins);
                    }
                }
                else if (editor_channel_column == PatternEditorChannelColumn::VOLUME) {
                    //Volume inputting
                    int vol = keyToVolume(event.key.scancode);
                    if (vol > -1) pattern.setCellVolume(editor_row, editor_channel, vol);
                }
                if (event.key.scancode == SDL_SCANCODE_DELETE) {
                    if (pattern.getCellNote(editor_row, editor_channel) != NOTE_BLANK) {
                        pattern.setCellNote(editor_row, editor_channel, NOTE_BLANK);
                        editor_row++;
                        if (editor_row >= pattern.row_count) editor_row = 0;
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_DOWN) {
                    if (editor_mode == PatternEditorMode::EDIT) {
                        editor_row++;
                        if (editor_row >= pattern.row_count) editor_row = 0;
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_UP) {
                    if (editor_mode == PatternEditorMode::EDIT){
                        editor_row--;
                        if (editor_row < 0) editor_row = pattern.row_count-1;
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_LEFT) {
                    if (editor_mode == PatternEditorMode::EDIT) {
                        editor_channel_column--;
                        if (editor_channel_column < 0) {
                            editor_channel_column = PatternEditorChannelColumn::EFFECT1;
                            editor_channel--;
                            if (editor_channel < 0) editor_channel = MAX_CHANNELS-1;
                        }
                    }
                }
                else if (event.key.scancode == SDL_SCANCODE_RIGHT) {
                    if (editor_mode == PatternEditorMode::EDIT) {
                        editor_channel_column++;
                        if(editor_channel_column>=PatternEditorChannelColumn::EFFECT2){
                            editor_channel_column = 0;
                            editor_channel++;
                            if (editor_channel >= MAX_CHANNELS) editor_channel = 0;
                        }
                    }
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
        }

        SDL_RenderClear(renderer);
        
        for(int ch = 0; ch < MAX_CHANNELS; ch++){
            for (int r = 0; r < pattern.row_count; r++) {
                int cell = pattern.getCell(r, ch);
                
                //Note and octave
                TTF_Text* text = TTF_CreateText(text_engine, font, getNoteName(cell & NOTE_MASK), 6 * sizeof(char));
                //Highlight
                if ((editor_mode==PatternEditorMode::PLAY && r == row)||(editor_mode==PatternEditorMode::EDIT&&r==editor_row&&editor_channel==ch&&editor_channel_column==PatternEditorChannelColumn::NOTE))
                    TTF_SetTextColor(text, 255, 0, 0, 255);
                
                TTF_DrawRendererText(text, (ch * 80), (r * 8)-(first_row_to_render*8));
                TTF_SetTextColor(text, 255, 255, 255, 255);
                //Instrument
                char str[3];
                int ins = pattern.getCellInstrument(r, ch);
                if(ins<INSTRUMENT_BLANK) sprintf_s(str, "%.2d", ins);
                    else  sprintf_s(str, "--");
                //Highlight
                if ((editor_mode == PatternEditorMode::PLAY && r == row) || (editor_mode == PatternEditorMode::EDIT && r == editor_row && editor_channel==ch && editor_channel_column == PatternEditorChannelColumn::INSTRUMENT))
                    TTF_SetTextColor(text, 255, 0, 0, 255);

                TTF_SetTextString(text, str, 3 * sizeof(char));
                TTF_DrawRendererText(text, (ch* 80)+(24), (r * 8) - (first_row_to_render * 8));
                TTF_SetTextColor(text, 255, 255, 255, 255);
                //Volume
                char str2[3];
                sprintf_s(str2, "%.2d", pattern.getCellVolume(r, ch));
                //Highlight
                if ((editor_mode == PatternEditorMode::PLAY && r == row) || (editor_mode == PatternEditorMode::EDIT && r == editor_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::VOLUME))
                    TTF_SetTextColor(text, 255, 0, 0, 255);

                TTF_SetTextString(text, str2, 3 * sizeof(char));
                TTF_DrawRendererText(text, (ch * 80) + (40), (r * 8) - (first_row_to_render * 8));
                TTF_SetTextColor(text, 255, 255, 255, 255);
                //Effect
                char str3[4];
                sprintf_s(str3, getEffectString(cell & EFFECT_MASK));
                //Highlight
                if ((editor_mode == PatternEditorMode::PLAY && r == row) || (editor_mode == PatternEditorMode::EDIT && r == editor_row && editor_channel == ch && editor_channel_column == PatternEditorChannelColumn::EFFECT1))
                    TTF_SetTextColor(text, 255, 0, 0, 255);

                TTF_SetTextString(text, str3, 4 * sizeof(char));
                TTF_DrawRendererText(text, (ch * 80)+(56), (r * 8));
                TTF_DestroyText(text);
            }
        }
        SDL_RenderPresent(renderer);
    }
    cleanUp();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
