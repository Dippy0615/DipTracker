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
#include "Instrument.h"
#include "PatternEditor.h"
#include "InstrumentEditor.h"
#include "Keyboard.h"
#include "Screen.h"

bool running = true;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static TTF_Font* font = NULL;
static TTF_TextEngine* text_engine = NULL;

Screen current_screen = Screen::PATTERNEDITOR;

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
    initializeInstruments();
    patterns[0].active = true;
    patterns_active++;

    current_pattern_index = 0;
    current_pattern = &patterns[current_pattern_index];
    
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
                if(current_screen == Screen::PATTERNEDITOR){
                    handlePatternEdtiorMouseButtonDown(event, current_screen);
                }
                if (current_screen == Screen::INSTRUMENTEDITOR) {
                    handleInstrumentEditorMouseButtonDown(event, current_screen);
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if(current_screen==Screen::PATTERNEDITOR) handlePatternEditorKeyDown(event);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        if (current_screen == Screen::PATTERNEDITOR) drawPattern(renderer, text_engine, font, 16, 0);
        if (current_screen == Screen::INSTRUMENTEDITOR) drawInstrumentEditor(renderer, text_engine, font);
        SDL_RenderPresent(renderer);
    }
    cleanUp();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
