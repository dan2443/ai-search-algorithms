#include "Cell.h"


Cell::Cell(int r, int c, Cell* p)
{
	row = r;
	column = c;
	parent = p;
	h = 1000;
	g = 0;
	f = 1000;

}

Cell::Cell(int r, int c, Cell* p,int d)
{
	row = r;
	column = c;
	parent = p;
	h = d;
	g = 0;
	f = 1000;
}

Cell::Cell(int r, int c, Cell* p, int d, int j)
{
	row = r;
	column = c;
	parent = p;
	h = d;
	g = j;
	f = d + j;
}
