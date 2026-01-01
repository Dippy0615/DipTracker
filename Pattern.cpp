#include <vector>
#include "Constants.h"
#include "Pattern.h"
#include "Audio.h"

/*
*           
* XXHG GGGF FFFE EEEE VVVV VVVI IIII INNN NNNN
* 
* First 7 bits are for the note (0-119 for notes, 127 for note off) [N]
* Next 6 bits are for the instrument (0-63) [I]
* Next 7 bits are for the volume (0-64, 127 for blank) [V]
* Next 5 bits are for the effect type (0-31) [E] 
* Next 4 bits are for the first effect column [F]
* Next 4 bits are for the second effect column [G]
* Next bit determines what digit of the volume paremeter you're editing (editor only) [H]
*/

Pattern::Pattern() {
	row_count = 64;
	int size = row_count * MAX_CHANNELS;
	cells = new long long[size];
	for (int i = 0; i < size; i++) {
		cells[i] = (NOTE_BLANK | (INSTRUMENT_BLANK << 7) | (VOLUME_BLANK << 13) | (EFFECT_TYPE_BLANK << 20)); 
	}
	active = false;
}

void Pattern::freePattern() {
	delete[] cells;
}

void Pattern::setCell(int row, int column, long long value) {
	cells[row * MAX_CHANNELS + column] = value;
}

long long Pattern::getCell(int row, int column) {
	return cells[row * MAX_CHANNELS + column];
}

void Pattern::setCellNote(int row, int column, int note) {
	cells[row * MAX_CHANNELS + column] &= ~(NOTE_MASK); //clear the bits in the destination number
	cells[row * MAX_CHANNELS + column] |= note; //bitwise OR the bits into the number
}

int Pattern::getCellNote(int row, int column) {
	return cells[row * MAX_CHANNELS + column] & NOTE_MASK;
}

void Pattern::setCellVolume(int row, int column, int volume) {
	cells[row * MAX_CHANNELS + column] &= ~(VOLUME_MASK);
	cells[row * MAX_CHANNELS + column] |= (volume << 13);
}
int Pattern::getCellVolume(int row, int column) {
	return (cells[row * MAX_CHANNELS + column] & VOLUME_MASK) >> 13;
}

void Pattern::setCellInstrument(int row, int column, int instrument) {
	cells[row * MAX_CHANNELS + column] &= ~(INSTRUMENT_MASK);
	cells[row * MAX_CHANNELS + column] |= (instrument << 7);
}
int Pattern::getCellInstrument(int row, int column) {
	return (cells[row * MAX_CHANNELS + column] & INSTRUMENT_MASK) >> 7;
}

void Pattern::setCellEffectType(int row, int column, EffectType effect_type) {
	cells[row * MAX_CHANNELS + column] &= ~(EFFECT_TYPE_MASK);
	cells[row * MAX_CHANNELS + column] |= (effect_type << 20);
}

EffectType Pattern::getCellEffectType(int row, int column) {
	return (EffectType)((cells[row * MAX_CHANNELS + column] & EFFECT_TYPE_MASK) >> 20);
}

void Pattern::setCellEffectOne(int row, int column, int value) {
	cells[row * MAX_CHANNELS + column] &= ~(EFFECT_ONE_MASK);
	cells[row * MAX_CHANNELS + column] |= (value << 25);
}
int Pattern::getCellEffectOne(int row, int column) {
	return (cells[row * MAX_CHANNELS + column] & EFFECT_ONE_MASK) >> 25;
}
void Pattern::setCellEffectTwo(int row, int column, int value) {
	cells[row * MAX_CHANNELS + column] &= ~(EFFECT_TWO_MASK);
	cells[row * MAX_CHANNELS + column] |= ((long long)value << 29);
}
int Pattern::getCellEffectTwo(int row, int column) {
	return (cells[row * MAX_CHANNELS + column] & EFFECT_TWO_MASK) >> 29;
}