#pragma once
#include "Constants.h"
#include "RowData.h"

class Pattern
{
private:
	int* cells;
public:
	Pattern();
	int row_count;
	void setCell(int row, int column, int value);
	int getCell(int row, int column);
	void setCellNote(int row, int column, int note);
	int getCellNote(int row, int column);
	void setCellVolume(int row, int column, int volume);
	int getCellVolume(int row, int column);
	void freePattern();

};

