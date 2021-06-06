#pragma once
#include "Cell.h"
class CompareCells
{
public:
	bool operator () (Cell* c1, Cell* c2) { return c1->GetH() > c2->GetH(); }
};

