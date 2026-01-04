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

bool running = true;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* font = NULL;
static TTF_TextEngine* text_engine = NULL;

void audioStreamCallback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount) {
    // Here you are expected to provide `additionalAmount` bytes of audio to the stream
    float* buffer = (float*)SDL_malloc(additionalAmount);
    int samples_to_go = additionalAmount / (sizeof(float) * 2);  // 2 channels
    processSamples(buffer, samples_to_go);
    SDL_PutAudioStreamData(stream, buffer, additionalAmount);
    SDL_free(buffer);
}

void cleanUp() {
    TTF_CloseFont(font);
    TTF_DestroyRendererTextEngine(text_engine);
    for(int i = 0; i < MAX_PATTERNS; i++)
        patterns[i].freePattern();
    
    SDL_DestroyAudioStream(audio_stream);
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("DipTracker", 800, 520, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
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
    patterns[0].active = true;
    patterns_active++;

    current_pattern_index = 0;
    current_pattern = &patterns[current_pattern_index];
    

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
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == 1) {
                    if (event.button.x >= 760 && event.button.x <= 768 && event.button.y>=0&&event.button.y<=8) {
                        //Add a pattern
                        patterns[patterns_active].active = true;
                        patterns_active++;
                    }
                    if (event.button.x >= 768 && event.button.x <= 774 && event.button.y >= 0 && event.button.y <= 8) {
                        //Remove a pattern
                        if(patterns_active>1){
                            patterns[patterns_active-1].active = false;
                            patterns_active--;
                        }
                    }
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if(editor_channel_column==PatternEditorChannelColumn::NOTE){
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
                    if(value>-1) current_pattern->setCellEffectOne(editor_row, editor_channel, value);
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
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawPattern(renderer, text_engine, font, 16);
        SDL_RenderPresent(renderer);
    }
    cleanUp();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
