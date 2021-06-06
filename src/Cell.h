#pragma once
class Cell
{
private:
	int row, column, h, g, f;
	Cell* parent;

public:
	Cell(int r, int c, Cell* p);
	Cell(int r, int c, Cell* p, int d);
	Cell(int r, int c, Cell* p, int d, int j);
	int GetRow() { return row; }
	int GetColumn() { return column; }
	int GetH() { return h; }
	int GetG() { return g; }
	int GetF() { return f; }
	void SetParent(Cell* p) { parent = p; }
	void SetF(int x) { f = x; }
	void SetG(int x) { g = x; }
	Cell* GetParent() { return parent; }

};

