#define _CRT_SECURE_NO_WARNINGS

#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GET_NUM(x,y,w) (x+w*y)

#define MAX_STEPS	10

// STRUCTS
struct WALKER
{
	int x;
	int y;
	char entityType[21];
	int id;
	int param0;
	int param1;
	int param2;
};

struct DOT
{
	int x;
	int y;
	char SYMB;
	int mass;
	int num;
};

struct DOT DOTS[26 * 20];
int MOVES[20][26];


// FUNCTIONS
// BOF
void CHANGE(int *A, int *B);

// PROGS
void A_STAR(int x, int y, int w, int h, int TYPE, int STEP);	  		// Алгоритм поиска пути А*
char IN_ZONE_X(int x, int w);					  		// Проверка корректности Х координаты
char IN_ZONE_Y(int y, int h);					  		// Проверка корректности Y координаты
char VALID_DOT(int x, int y, int w, int h);	  		// Проверка возможности хода
													//char LOCKED_DOT(int x, int y);			  		// Проверка на запертую со всех сторон клетку

													//void 	GET_EXPL_FIELD(int Rad);              	// Покрытие всего поля опасностью ящеров
													//void 	GET_WANDER_FIELD(int B_NUM);			// Покрытие поля опасностью от ящеров

int My_X;
int My_Y;
int My_BNum;
int My_BRad;

int En_X[3];
int En_Y[3];

int Trg_X;
int Trg_Y;
int Trg_Mass;

int TURN;


/**
* Survive the wrath of Kutulu
* Coded fearlessly by JohnnyYuge & nmahoude (ok we might have been a bit scared by the old god...but don't say anything)
**/
int main()
{
	int width;
	scanf("%d", &width);
	int height;
	scanf("%d", &height); fgetc(stdin);

	fprintf(stderr, "w=%d, h=%d \n", width, height);
	/*
	char line[5][11] =
	{
	'x','x','x','x','x','x','x','x','x','x','x',
	'.','.','.','.','.','.','.','.','.','.','.',
	'.','.','.','.','.','.','.','.','.','.','.',
	'x','x','x','x','x','x','x','x','x','x','x',
	};
	*/
	char line[20][26];

	for (int i = 0; i < height; i++) {
		//char line[26];
		fgets(line[i], 26, stdin);
		//fprintf(stderr, "%s", line[i]);
	}


	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			DOTS[i * width + j].x = j;
			DOTS[i * width + j].y = i;
			DOTS[i * width + j].num = GET_NUM(j, i, width);
			DOTS[i * width + j].mass = -1;
			MOVES[j][i] = 0;
			DOTS[i * width + j].SYMB = line[i][j];
			fprintf(stderr, "%c", DOTS[i * width + j].SYMB);
		}
		fprintf(stderr, "\n");
	}


	int sanityLossLonely; // how much sanity you lose every turn when alone, always 3 until wood 1
	int sanityLossGroup; // how much sanity you lose every turn when near another player, always 1 until wood 1
	int wandererSpawnTime; // how many turns the wanderer take to spawn, always 3 until wood 1
	int wandererLifeTime; // how many turns the wanderer is on map after spawning, always 40 until wood 1
	scanf("%d%d%d%d", &sanityLossLonely, &sanityLossGroup, &wandererSpawnTime, &wandererLifeTime);

	// game loop
	while (1) {
		int entityCount; // the first given entity corresponds to your explorer
		scanf("%d", &entityCount);

		//for (int k = 0; k < entityCount; k++) 
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				MOVES[i][j] = 0;

		for (int i = 0; i < entityCount; i++)
		{
			char entityType[21];
			int id;
			int x;
			int y;
			int param0;
			int param1;
			int param2;
			scanf("%s%d%d%d%d%d%d", entityType, &id, &x, &y, &param0, &param1, &param2);

			if (entityType[0] == 'E')
			{
				A_STAR(x, y, width, height, 0, 0);

				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
					{
						if (DOTS[GET_NUM(j, i, width)].mass != -1) MOVES[i][j] = MOVES[i][j] + MAX_STEPS - DOTS[GET_NUM(j, i, width)].mass;
						DOTS[i * width + j].mass = -1;
					}
			}

			if (entityType[0] == 'W')
			{
				A_STAR(x, y, width, height, 1, 0);

				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
					{
						if (DOTS[GET_NUM(j, i, width)].mass != -1) MOVES[i][j] = MOVES[i][j] - 5 * (MAX_STEPS - DOTS[GET_NUM(j, i, width)].mass);
						DOTS[i * width + j].mass = -1;
					}
			}
			//if(entityType[0] == 'W') A_STAR(x, y, width, height, 1, 0);	 
		}

		/*
		for (int i = 0; i < height; i++)
		{
		fprintf(stderr, "%02d :",i);
		for (int j = 0; j < width; j++)
		{
		fprintf(stderr, " %02d ", DOTS[i * width + j].mass);
		}
		fprintf(stderr, "\n");
		}
		*/
		fprintf(stderr, "\n---------\n");

		for (int i = 0; i < height; i++)
		{
			fprintf(stderr, "%02d :", i);
			for (int j = 0; j < width; j++)
			{
				fprintf(stderr, " %03d ", MOVES[i][j]);
			}
			fprintf(stderr, "\n");
		}

		Trg_Mass = -1000;
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				if (Trg_Mass< MOVES[i][j])
				{
					Trg_X = j;
					Trg_Y = i;
					Trg_Mass = MOVES[i][j];
				}
			}

		// Write an action using printf(). DON'T FORGET THE TRAILING \n
		// To debug: fprintf(stderr, "Debug messages...\n");

		printf("MOVE %d %d\n", Trg_X, Trg_Y); // MOVE <x> <y> | WAIT

	}

	return 0;
}


/////////////////////////////////////////////////////////////////////
void CHANGE(int *A, int *B)
{
	*A = *A^*B;
	*B = *A^*B;
	*A = *A^*B;
}
/////////////////////////////////////////////////////////////////////
int MIN(int a, int b)
{
	return a <= b ? a : b;
}
/////////////////////////////////////////////////////////////////////
int MAX(int a, int b)
{
	return a >= b ? a : b;
}
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char IN_ZONE_X(int x, int w)
{
	if ((x >= 0) && (x<w)) return TRUE;
	else return FALSE;
}
/////////////////////////////////////////////////////////////////////
char IN_ZONE_Y(int y, int h)
{
	if ((y >= 0) && (y<h)) return TRUE;
	else return FALSE;
}
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void A_STAR(int x, int y, int w, int h, int TYPE, int STEP)
{
	if (STEP >= MAX_STEPS) return;

	if (VALID_DOT(x, y, w, h))
	{
		if ((DOTS[GET_NUM(x, y, w)].mass == -1) || (DOTS[GET_NUM(x, y, w)].mass > STEP))
		{
			DOTS[GET_NUM(x, y, w)].mass = STEP;

			//if(TYPE==0)MOVES[x][y] += (MAX_STEPS-DOTS[GET_NUM(x, y, w)].mass); //EXPLORER
			//if(TYPE==1)MOVES[x][y] -= (MAX_STEPS-DOTS[GET_NUM(x, y, w)].mass); // W-SPAWN

			DOTS[GET_NUM(x, y, w)].num = GET_NUM(x, y, w);
			A_STAR(x, y, w, h, TYPE, STEP);
		}
		else
		{
			if (VALID_DOT(x + 1, y, w, h)) 	A_STAR(x + 1, y, w, h, TYPE, STEP + 1);
			if (VALID_DOT(x - 1, y, w, h)) 	A_STAR(x - 1, y, w, h, TYPE, STEP + 1);
			if (VALID_DOT(x, y + 1, w, h))	A_STAR(x, y + 1, w, h, TYPE, STEP + 1);
			if (VALID_DOT(x, y - 1, w, h))	A_STAR(x, y - 1, w, h, TYPE, STEP + 1);
		}
	}
	else return;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

char VALID_DOT(int x, int y, int w, int h)
{
	if
		(
			!IN_ZONE_X(x, w) || !IN_ZONE_Y(y, h) ||
			(DOTS[GET_NUM(x, y, w)].SYMB != '.' && DOTS[GET_NUM(x, y, w)].SYMB != 'w')
			//||IN_BOMBS(x,y, B_NUM)
			//|| LOCKED_DOT(x,y)
			)
		return FALSE;
	else
		return TRUE;
}
/////////////////////////////////////////////////////////////////////
