#pragma once
#include "Cell.h"
class CompareCellsF
{
public:
	bool operator () (Cell* c1, Cell* c2) { return c1->GetF() > c2->GetF(); }
};

