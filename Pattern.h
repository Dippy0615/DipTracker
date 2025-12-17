#pragma once
#include "Constants.h"
#include "RowData.h"

class Pattern
{
private:
	long long* cells;
public:
	Pattern();
	int row_count;
	void setCell(int row, int column, long long value);
	long long getCell(int row, int column);
	void setCellNote(int row, int column, int note);
	int getCellNote(int row, int column);
	void setCellVolume(int row, int column, int volume);
	int getCellVolume(int row, int column);
	void setCellInstrument(int row, int column, int instrument);
	int getCellInstrument(int row, int column);
	void freePattern();

};

