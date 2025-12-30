#pragma once
#include "Constants.h"
#include "RowData.h"
#include "Audio.h"

class Pattern
{
private:
	long long* cells;
public:
	Pattern();
	int row_count;
	bool active;
	void setCell(int row, int column, long long value);
	long long getCell(int row, int column);
	void setCellNote(int row, int column, int note);
	int getCellNote(int row, int column);
	void setCellVolume(int row, int column, int volume);
	int getCellVolume(int row, int column);
	void setCellInstrument(int row, int column, int instrument);
	int getCellInstrument(int row, int column);
	void setCellEffectType(int row, int column, EffectType effect_type);
	EffectType getCellEffectType(int row, int column);
	void setCellEffectOne(int row, int column, int value);
	int getCellEffectOne(int row, int column);
	void setCellEffectTwo(int row, int column, int value);
	int getCellEffectTwo(int row, int column);
	void freePattern();

};

