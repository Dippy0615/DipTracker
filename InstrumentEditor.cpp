#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include "Audio.h"
#include "InstrumentEditor.h"
#include "Screen.h"

SDL_Texture* sine_texture;
SDL_Texture* square_texture;
SDL_Texture* saw_texture;
SDL_Texture* tri_texture;
SDL_Texture* envelope_bar_texture;
int current_instrument_index = 0;

void handleInstrumentEditorMouseButtonDown(SDL_Event& event, Screen& current_screen) {
	//TEMPORARY UI: Pattern Editor button
	if (event.button.x >= 0 && event.button.x <= 120 && event.button.y >= 0 && event.button.y <= 8) {
		current_screen = Screen::PATTERNEDITOR;
	}

	//Oscillator type buttons

	//Sine
	if (event.button.x >= 128 && event.button.x <= 192 && event.button.y >= 16 && event.button.y <= 48) {
		instruments[current_instrument_index].setOscillatorType(OscillatorType::Sine);
	}

	//Square
	if (event.button.x >= 224 && event.button.x <= 288 && event.button.y >= 16 && event.button.y <= 48) {
		instruments[current_instrument_index].setOscillatorType(OscillatorType::Square);
	}

	//Saw
	if (event.button.x >= 320 && event.button.x <= 384 && event.button.y >= 16 && event.button.y <= 48) {
		instruments[current_instrument_index].setOscillatorType(OscillatorType::Saw);
	}

	//Tri
	if (event.button.x >= 416 && event.button.x <= 480 && event.button.y >= 16 && event.button.y <= 48) {
		instruments[current_instrument_index].setOscillatorType(OscillatorType::Tri);
	}

	//TEMPORARY UI: Increase/Decrease envelope length buttons
	if (event.button.x >= 144 && event.button.x <= 151 && event.button.y >= 64 && event.button.y <= 72) {
		instruments[current_instrument_index].setVolumeEnvelopeLength(instruments[current_instrument_index].getVolumeEnvelopeLength() + 1);
	}
	if (event.button.x >= 152 && event.button.x <= 160 && event.button.y >= 64 && event.button.y <= 72) {
		if(instruments[current_instrument_index].getVolumeEnvelopeLength()>0)
			instruments[current_instrument_index].setVolumeEnvelopeLength(instruments[current_instrument_index].getVolumeEnvelopeLength() - 1);
	}
	//Increase/Decrease instrument buttons
	if (event.button.x >= 580 && event.button.x <= 588 && event.button.y >= 0 && event.button.y <= 8) {
		current_instrument_index++;
	}
	if (event.button.x >= 588 && event.button.x <= 596 && event.button.y >= 0 && event.button.y <= 8) {
		current_instrument_index--;
		if (current_instrument_index < 0) current_instrument_index = 0;
	}

	//Envelope editor
	int x_limit = instruments[current_instrument_index].getVolumeEnvelopeLength() * 16;
	if (event.button.x >= 0 && event.button.x <= x_limit && event.button.y >= 128 && event.button.y <= 256) {
		int ind = event.button.x / 16;
		int val = (256 - event.button.y)/2;
		instruments[current_instrument_index].setVolumeEnvelope(ind, val);
	}
}

void drawInstrumentEditor(SDL_Renderer* renderer, TTF_TextEngine* text_engine, TTF_Font* font) {
	//TEMPORARY UI: Pattern Editor button
	TTF_Text* text = TTF_CreateText(text_engine, font, "Pattern Editor", 15);
	TTF_DrawRendererText(text, 0, 0);

	//Oscilattor type buttons
	TTF_SetTextString(text, "Oscillator Type:", 16);
	TTF_DrawRendererText(text, 0, 24);

	SDL_FRect rect;
	rect.x = 128;
	rect.y = 16;
	rect.w = 64;
	rect.h = 32;

	Instrument* ins = &instruments[current_instrument_index];

	if (ins->getOscillatorType() == OscillatorType::Sine)
		SDL_SetTextureColorMod(sine_texture, 255, 0, 0);
	else
		SDL_SetTextureColorMod(sine_texture, 255, 255, 255);
	SDL_RenderTexture(renderer, sine_texture, NULL, &rect);

	rect.x += 96;
	if (ins->getOscillatorType() == OscillatorType::Square)
		SDL_SetTextureColorMod(square_texture, 255, 0, 0);
	else
		SDL_SetTextureColorMod(square_texture, 255, 255, 255);
	SDL_RenderTexture(renderer, square_texture, NULL, &rect);

	rect.x += 96;
	if (ins->getOscillatorType() == OscillatorType::Saw)
		SDL_SetTextureColorMod(saw_texture, 255, 0, 0);
	else
		SDL_SetTextureColorMod(saw_texture, 255, 255, 255);
	SDL_RenderTexture(renderer, saw_texture, NULL, &rect);

	rect.x += 96;
	if (ins->getOscillatorType() == OscillatorType::Tri)
		SDL_SetTextureColorMod(tri_texture, 255, 0, 0);
	else
		SDL_SetTextureColorMod(tri_texture, 255, 255, 255);
	SDL_RenderTexture(renderer, tri_texture, NULL, &rect);

	
	TTF_SetTextString(text, "Envelope Length:", 17);
	TTF_DrawRendererText(text, 0, 64);

	//TEMPORARY UI: Increase/Decrease envelope length buttons
	TTF_SetTextString(text, "+", 2);
	TTF_DrawRendererText(text, 144, 64);
	TTF_SetTextString(text, "-", 2);
	TTF_DrawRendererText(text, 152, 64);

	//Instrument name
	TTF_SetTextString(text, ins->getName(), 20);
	TTF_DrawRendererText(text, 400, 48);

	//Increase/Decrease instrument
	char t[9];
	sprintf_s(t, "INS %.2d", current_instrument_index);
	TTF_SetTextString(text, t, 9);
	TTF_DrawRendererText(text, 534, 0);

	TTF_SetTextString(text, "+-", 3);
	TTF_DrawRendererText(text, 580, 0);

	//Draw envelope
	int len = ins->getVolumeEnvelopeLength();
	int* env = ins->getVolumeEnvelope();
	for (int i = 0; i < len; i++) {
		int x = 0 + (i * 16);
		//SDL_RenderLine(renderer, x, 384, x + 14, 384);

		int env_height = env[i]*2;
		if (env_height == 0) env_height++;

		SDL_FRect rect2;
		rect2.x = x;
		rect2.y = 256-env_height;
		rect2.w = 14;
		rect2.h = env_height;
		SDL_RenderTexture(renderer, envelope_bar_texture, NULL, &rect2);
	}

}