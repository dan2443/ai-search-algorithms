#include "glut.h"
#include <time.h>
#include "Cell.h"
#include <vector>
#include <queue>
#include "CompareCells.h"
#include "CompareCellsF.h"
#include <iostream>

using namespace std;

const int W = 600;
const int H = 600;
const int WALL = 1;
const int SPACE = 0;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4; // visited
const int GRAY = 5;
const int PATH = 6;

const int MSZ = 100;

int maze[MSZ][MSZ] = { 0 };
int targetX, targetY;

bool runBFS = false, runDFS = false, runBestFS = false, runAStar = false;

vector <Cell*> grays; // grays queue

priority_queue <Cell*, vector<Cell*>, CompareCells> pq; //pq of BestFS
priority_queue <Cell*, vector<Cell*>, CompareCellsF> pqA; //pq of AStar

void InitMaze();
int DistanceFromPacman(int row, int col);

void init()
{
	srand(time(0));
	glClearColor(1, 1, 1, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 0, 1);

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++) // frame of WALLS
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for (i =1; i < MSZ-1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1)
			{
				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
			else // the line is even
			{
				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}
	// mark out the start and the target cells

	maze[MSZ / 2][MSZ / 2] = START;
	targetX = rand() % MSZ;
	targetY = rand() % MSZ;
	maze[targetX][targetY] = TARGET;
	Cell* pc = new Cell(MSZ / 2, MSZ / 2, nullptr);
	grays.push_back(pc); // the start cell is in grays
	pq.push(pc); //push to pq
	pqA.push(pc); //push to pqA

}

void DrawMaze()
{
	int i, j;
	int xsz = W / MSZ;
	int ysz = H / MSZ;
	double x, y;
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])   // set color
			{
			case SPACE:
				glColor3d(0.9, 0.9, 0.9);
				break;
			case WALL:
				glColor3d(0.3, 0, 0);
				break;
			case START:
				glColor3d(0.5, 0.5, 1);
				break;
			case TARGET:
				glColor3d(1, 0, 0);
				break;
			case BLACK:
				glColor3d(0.9, 0.9, 0.5);
				break;
			case GRAY:
				glColor3d(0.2, 0.6, 0.2);
				break;
			case PATH:
				glColor3d(1, 0.6, 1);
				break;
			}
			// draw square
			x = 2.0 * (j * xsz) / W - 1; // value in range [-1,1)
			y = 2.0 * (i * ysz) / H - 1; // value in range [-1,1)
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y+(2.0*ysz)/H);
			glVertex2d(x+2.0*xsz/W, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y);
			glEnd();
		}
}

void RestorePath(Cell* pcurrent)
{
	while (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != START)
	{
		maze[pcurrent->GetRow()][pcurrent->GetColumn()] = PATH;
		pcurrent = pcurrent->GetParent();
	}
}

void CheckNeighbor(Cell* pcurrent, int row, int col)
{
	// check the color of the neighbor cell
	if (maze[row][col] == SPACE || maze[row][col] == TARGET)
	{
		if (maze[row][col] == TARGET) // the solution has been found
		{
			cout << "the solution has been found\n";
			runBFS = false;
			runDFS = false;
			runBestFS = false;
			runAStar = false;
			RestorePath(pcurrent);
		}
		else // it is white neighbor, so make it gray
		{
			if (runBFS || runDFS) 
			{ //BFS OR DFS
				Cell* pc = new Cell(row, col, pcurrent);
				grays.push_back(pc);
			}
			else if (runBestFS)
			{ //BestFS
				Cell* pc = new Cell(row, col, pcurrent, DistanceFromPacman(row, col));
				pq.push(pc);
			}
			else
			{ //AStar
				Cell* pc = new Cell(row, col, pcurrent, DistanceFromPacman(row, col), pcurrent->GetG()+1);
				pqA.push(pc);
			}
			maze[row][col] = GRAY;
		}
	}
}

int DistanceFromPacman(int row, int col)
{
	return (abs(row - targetX) + abs(col - targetY));
}

void BFSIteration()
{
	Cell* pcurrent;

	if (grays.empty())
	{
		cout << "There is no solution\n";
		runBFS = false;
	}
	else // there are gray cells
	{
		pcurrent = grays.front();  // save the FIRST element,
		grays.erase(grays.begin()); // remove it from the queue
		if(maze[pcurrent->GetRow()][pcurrent->GetColumn()]!=START)
		maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;   // and paint it black

		// now check the neighbors
		// up
		CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// down
		if(runBFS)
		CheckNeighbor(pcurrent, pcurrent->GetRow()- 1, pcurrent->GetColumn() );
		// right
		if (runBFS)
		CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// left
		if (runBFS)
		CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);
	}
}

void DFSIteration()
{
	Cell* pcurrent;

	if(grays.empty())
	{
		cout << "There is no solution\n";
		runDFS = false;
	}
	else
	{
		// remove the LAST element from grays
		pcurrent = grays[grays.size() - 1]; //  or pcurrent = *grays.back();
		grays.pop_back();
		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != START)
			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;

		// now check the neighbors
		// up
		CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// down
		if (runDFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
		// right
		if (runDFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// left
		if (runDFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);
	}

}

void BestFSIteration() 
{
	Cell* pcurrent;

	if (pq.empty())
	{
		cout << "There is no solution\n";
		runBestFS = false;
	}
	else
	{
		// pop top from q
		pcurrent = pq.top();
		pq.pop();
		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != START)
			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;

		// now check the neighbors
		// up
		CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// down
		if (runBestFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
		// right
		if (runBestFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// left
		if (runBestFS)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);


	}

}


void AStarIteration()
{
	Cell* pcurrent;

	if (pqA.empty())
	{
		cout << "There is no solution\n";
		runAStar = false;
	}
	else
	{
		// pop top from pqA
		pcurrent = pqA.top();
		pqA.pop();
		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != START)
			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;

		// now check the neighbors
		// up
		CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// down
		if (runAStar)
			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
		// right
		if (runAStar)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// left
		if (runAStar)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);

	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
/*
	glLineWidth(3);
	glBegin(GL_POLYGON);// GL_LINE_STRIP); // GL_LINES
		glColor3d(1, 0, 0);
		glVertex2d(x, y);
		glColor3d(0, 0, 1);
		glVertex2d(-x, -y);
		glColor3d(0, 1, 0);
		glVertex2d(x, -y);
	glEnd();
*/
	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
//	y -= 0.001;
//	x += 0.002;
	if (runBFS)
		BFSIteration();
	if (runDFS)
		DFSIteration();
	if (runBestFS)
		BestFSIteration();
	if (runAStar)
		AStarIteration();
	glutPostRedisplay(); // go to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 0: // BFS
		runBFS = true;
		break;
	case 1: // DFS
		runDFS = true;
		break;
	case 2: // BestFirstSearch
		runBestFS = true;
		break;
	case 3: // AStar
		runAStar = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display); // display is window refresh function
	glutIdleFunc(idle);  // idle is background function

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("BFS", 0);
	glutAddMenuEntry("DFS", 1);
	glutAddMenuEntry("BestFirstSearch", 2);
	glutAddMenuEntry("A*", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);



	init();

	glutMainLoop();
}