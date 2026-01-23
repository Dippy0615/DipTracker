#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Audio.h"
#include "InstrumentEditor.h"
#include "Screen.h"

int current_instrument_index = 0;

void handleInstrumentEditorMouseButtonDown(SDL_Event& event, Screen& current_screen) {
	//TEMPORARY UI: Pattern Editor button
	if (event.button.x >= 0 && event.button.x <= 120 && event.button.y >= 0 && event.button.y <= 8) {
		current_screen = Screen::PATTERNEDITOR;
	}
	//TEMPORARY UI: Increase/Decrease envelope length buttons
	if (event.button.x >= 64 && event.button.x <= 71 && event.button.y >= 32 && event.button.y <= 40) {
		instruments[current_instrument_index].setVolumeEnvelopeLength(instruments[current_instrument_index].getVolumeEnvelopeLength() + 1);
	}
	if (event.button.x >= 72 && event.button.x <= 80 && event.button.y >= 32 && event.button.y <= 40) {
		if(instruments[current_instrument_index].getVolumeEnvelopeLength()>0)
			instruments[current_instrument_index].setVolumeEnvelopeLength(instruments[current_instrument_index].getVolumeEnvelopeLength() - 1);
	}
}

void drawInstrumentEditor(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font) {
	//TEMPORARY UI: Pattern Editor button
	TTF_Text* text = TTF_CreateText(text_engine, font, "Pattern Editor", 15);
	TTF_DrawRendererText(text, 0, 0);


	TTF_SetTextString(text, "Volume", 7);
	TTF_DrawRendererText(text, 0, 32);

	//TEMPORARY UI: Increase/Decrease envelope length buttons
	TTF_SetTextString(text, "+", 2);
	TTF_DrawRendererText(text, 64, 32);
	TTF_SetTextString(text, "-", 2);
	TTF_DrawRendererText(text, 72, 32);

	//Instrument name
	Instrument* ins = &instruments[current_instrument_index];
	TTF_SetTextString(text, ins->getName(), 20);
	TTF_DrawRendererText(text, 400, 32);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//Draw envelope
	int len = ins->getVolumeEnvelopeLength();
	for (int i = 0; i < len; i++) {
		int x = 0 + (i * 16);
		SDL_RenderLine(renderer, x, 128, x + 14, 128);
	}

}