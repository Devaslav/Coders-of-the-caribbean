#define _CRT_SECURE_NO_WARNINGS

#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

/// HELPER
///////////////////////////////////////////////////////////////////////////
//convert even - r offset to cube
//	x = col - (row + (row & 1)) / 2
//	z = row
//	y = -x - z

//convert cube to even - r offset
//col = x + (z + (z & 1)) / 2
//row = z














///////////////////////////////////////////////////////////////////////////
// VARS AND STRUCTS
////////////////////////
typedef struct
{
	int x;
	int y;
	int z;
}Cube_Point;

typedef struct
{
	int col;
	int row;
}Hex_Point;
////////////////////////
typedef struct 
{
	int id;
	Hex_Point offset;
	Cube_Point cube;
	int dir; // the ship's rotation orientation (between 0 and 5)
	int spd; // the ship's speed (between 0 and 2)
	int rum; // the ship's stock of rum units
	int owner; // 1 if the ship is controlled by you, 0 otherwise 

} SHIP;

typedef struct
{
	int id;
	Hex_Point offset;
	Cube_Point cube;
	int value; // value of rum units(10-26)

} BARREL;

typedef struct
{
	int id;
	Hex_Point offset;
	Cube_Point cube;
	int owner; // the entityId of the ship that fired this cannon ball
	int turns; // number of turns before impact (1 means the cannon ball will land at the end of the current turn)
} CANNONBALL;

typedef struct
{
	int id;
	Hex_Point offset;
	Cube_Point cube;
} _MINE_;

typedef enum
{
	FIRE,		// 0
	MINE,		// 1
	MOVE,		// 2
	SLOWER,		// 3
	WAIT		// 4
} Actions;

typedef struct
{
	Hex_Point offset;
	Cube_Point cube;
	Actions action;
} ACTION;

typedef struct
{
	int x1;
	int y1;
	int x2;
	int y2;
	double cx;
	double cy;
	int valid; // 1 - valid, 0-???,-1 - invalid
} SQUARE;

////////////////////////
BARREL Barrels[100];
SHIP My_Ships[3];
SHIP En_Ships[3];
_MINE_ Mines[100];
CANNONBALL Cores[100];

ACTION TURN_ACTIONS[3];

#define N_SQR 110//(20 / 2) * (22 / 2)
SQUARE SQRS[N_SQR];

//////////////////////////////////////////////////////////////////////////
// MACROSES
#define GET_DIST(X1, Y1, X2, Y2) ( sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)) ) 
#define GET_TURNS(X) ( round( 1+ X/3 ) )

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
//////////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////////
// Hex_Magic 
Cube_Point Cube_Dirs[6] = 
{
	{+1, -1,  0},
	{ +1, 0, -1},
	{ 0, +1, -1},
	{ -1, +1, 0},
	{ -1, 0, +1},
	{ 0, -1, +1}
};

Hex_Point Offset_Dirs [6] =
{
		{+1, 0}, {0, -1}, {-1, -1},
		{-1, 0}, {-1, +1},{0, +1},
};

Hex_Point HP_Plus(Hex_Point a, Hex_Point b)
{
	Hex_Point c;
	c.col = a.col + b.col;
	c.row = a.row + b.row;
	return c;
}

Cube_Point Hex_to_Cube(Hex_Point Point)
{
	Cube_Point Cube;

	Cube.x = Point.col - (Point.row + (Point.row & 1)) / 2;
	Cube.z = Point.row;
	Cube.y = -Cube.x - Cube.z;

	return Cube;
}

Hex_Point Cube_to_Hex(Cube_Point Cube)
{
	Hex_Point Hex;

	Hex.col = Cube.x + (Cube.z + (Cube.z & 1)) / 2;
	Hex.row = Cube.z;
}

/*
int Cube_Dist1(Cube_Point a, Cube_Point b)
{
	return max(max(abs(a.x - b.x), abs(a.y - b.y)), abs(a.z - b.z));
}
*/

int Cube_Dist(Cube_Point a, Cube_Point b)
{
	return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
}


// Offset_Dist(Hex_Point a, Hex_Point b)
Offset_Dist(Hex_Point a, Hex_Point b)
{
	Cube_Point ac = Hex_to_Cube(a);
	Cube_Point bc = Hex_to_Cube(b);
	
	return Cube_Dist(ac, bc);
}
////////////////////////////////////////////////////////////////////////////
void GET_SQRS(void)
{
	int i,j;

	for(j=0;j<=10;j++)
		for (i = 0;i <= 11;i++)
		{
			SQRS[j * 11 + i].x1 = i * 3;
			SQRS[j * 11 + i].x2 = i * 3 + 2;
			SQRS[j * 11 + i].y1 = j * 3;
			SQRS[j * 11 + i].y2 = j * 3 + 2;
			SQRS[j * 11 + i].cx = (SQRS[j * 11 + i].x1 + SQRS[j * 11 + i].x2) / 2.0;
			SQRS[j * 11 + i].cy = (SQRS[j * 11 + i].y1 + SQRS[j * 11 + i].y2) / 2.0;

			SQRS[j * 11 + i].valid = 1;
		}
}
//////////////////////////////////////////////////////////////////////////
char IN_SQR(SQUARE SQR, int x, int y)
{
	if (x >= SQR.x1 && x <= SQR.x2 && y >= SQR.y1 && y <= SQR.y2)
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////////
int GET_SQRS_ID(int x, int y)
{
	int i;
	for (i = 0;i < N_SQR;i++)
		if (x >= SQRS[i].x1 && x <= SQRS[i].x2 && y >= SQRS[i].y1 && y <= SQRS[i].y2) return i;

	return -1;
}
//////////////////////////////////////////////////////////////////////////
void VAL_SQRS(_MINE_ *Mines, int m_cnt, CANNONBALL *Cores, int c_cnt, SHIP My_Ship, SHIP *En_Ships, int En_Cnt)
{
	int i,j;

	for (i = 0;i < N_SQR;i++)
	{
		if(SQRS[i].valid != 0) SQRS[i].valid = 1;
	}

	for(i=0;i<m_cnt;i++)
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], Mines[i].offset.col, Mines[i].offset.row) == TRUE ) SQRS[j].valid = -1;
		}

	for (i = 0;i<c_cnt;i++)
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], Cores[i].offset.col, Cores[i].offset.row) == TRUE) SQRS[j].valid = -2;
		}
	 
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], My_Ship.offset.col, My_Ship.offset.row) == TRUE ) SQRS[j].valid = 0;
		}

	for (i = 0;i<En_Cnt;i++)
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], En_Ships[i].offset.col, En_Ships[i].offset.row) == TRUE) SQRS[j].valid = -3;
		}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ACTION Get_Action(SHIP My_Ship, int My_Ship_Cnt, SHIP *En_Ships, int En_Cnt,  BARREL *Barrels, int Bar_Cnt,
	_MINE_ *Mines, int m_cnt, ACTION *PREV_ACT)
{
	double Dist;
	ACTION ACT;
	int i;

	Dist = 100;

	ACT.action = WAIT;

	if (m_cnt > 0 && PREV_ACT[My_Ship_Cnt].action != FIRE && ACT.action != FIRE)
	{
		for (i = 0; i<m_cnt; i++)
			if (Offset_Dist(My_Ship.offset, Mines[i].offset) < 3* My_Ship.spd)
			{
				ACT.action = FIRE;
				ACT.offset.col = Mines[i].offset.col;
				ACT.offset.row = Mines[i].offset.row;
				ACT.cube = Hex_to_Cube(ACT.offset);
			}
	}

	//fprintf(stderr, "Bar_Cnt = %d\n", Bar_Cnt);
	for (int En_ID = 0; En_ID<En_Cnt; En_ID++)
	if ( PREV_ACT[My_Ship_Cnt].action != FIRE && ACT.action != FIRE 
		 && (Offset_Dist(My_Ship.offset, En_Ships[En_ID].offset) <= Dist )
		 && (Offset_Dist(My_Ship.offset, En_Ships[En_ID].offset) <= 10 && My_Ship.spd > 0)
		 || (Offset_Dist(My_Ship.offset, En_Ships[En_ID].offset) <= 3) )
	{
		ACT.action = FIRE;
		ACT.offset.col = En_Ships[En_ID].offset.col;
		ACT.offset.row = En_Ships[En_ID].offset.row;
		ACT.cube = Hex_to_Cube(ACT.offset);

		Dist = Offset_Dist(My_Ship.offset, En_Ships[En_ID].offset);

		 
		if (En_Ships[En_ID].dir == 0) { ACT.offset.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		if (En_Ships[En_ID].dir == 1) { ACT.offset.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.offset.row -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		if (En_Ships[En_ID].dir == 2) { ACT.offset.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.offset.row -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		if (En_Ships[En_ID].dir == 3) { ACT.offset.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		if (En_Ships[En_ID].dir == 4) { ACT.offset.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.offset.row += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		if (En_Ships[En_ID].dir == 5) { ACT.offset.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.offset.row += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; }
		
	}
	

	if (Bar_Cnt > 0 && ACT.action != FIRE)
	{
		ACT.action = MOVE;
		Dist = 100;

		for (i = 0; i < Bar_Cnt; i++)
		{
			if (Offset_Dist(My_Ship.offset, Barrels[i].offset) < Dist
					&& SQRS[GET_SQRS_ID(Barrels[i].offset.col, Barrels[i].offset.row)].valid == 1)
			{
				Dist = Offset_Dist(My_Ship.offset, Barrels[i].offset);
				ACT.offset.col = Barrels[i].offset.col;
				ACT.offset.row = Barrels[i].offset.row;

				//fprintf(stderr, "TRG_BAR==%d\n", i);
			}
		}
	}

	if(Bar_Cnt == 0 && ACT.action != FIRE)
	{
		ACT.action = MOVE;
		Dist = 10000.0;
		
		for (i = 0; i < 110; i++)
		{
			if ( (GET_DIST(My_Ship.offset.col, My_Ship.offset.row, SQRS[i].cx, SQRS[i].cy) < Dist)
				&& SQRS[i].valid == 1 )
			{
				Dist = GET_DIST(My_Ship.offset.col, My_Ship.offset.row, SQRS[i].cx, SQRS[i].cy);
				ACT.offset.col = (int )SQRS[i].cx;
				ACT.offset.row = (int )SQRS[i].cy;

				//fprintf(stderr, "TRG_SQR==%d\n", i);
			}
		}
		
	}

	return ACT;
}








//////////////////////////////////////////////////////
/****************************************************/
//////////////////////////////////////////////////////
/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	int my_ships_cnt = 0;
	int en_ships_cnt = 0;
	int barrel_cnt = 0;
	int mine_cnt = 0;
	int core_cnt = 0;

	ACTION New_Act;
	GET_SQRS();

	TURN_ACTIONS[0].action = WAIT;
	TURN_ACTIONS[1].action = WAIT;
	TURN_ACTIONS[2].action = WAIT;

	//Ships = (SHIP *)calloc(100, sizeof(SHIP)); // Max my ships == 1
    // game loop
    while (1)
	{
		my_ships_cnt = 0;
		en_ships_cnt = 0;
		barrel_cnt = 0;
		mine_cnt = 0;
		core_cnt = 0;

        static int myShipCount; // the number of remaining ships
        scanf("%d", &myShipCount);

        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        scanf("%d", &entityCount);
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            char entityType[11];
            int x;
            int y;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            scanf("%d%s%d%d%d%d%d%d", &entityId, entityType, &x, &y, &arg1, &arg2, &arg3, &arg4);

			if (strcmp(entityType, "SHIP") == 0 && arg4 == 1) // init My_Ship
			{
				My_Ships[my_ships_cnt].id = i;
				My_Ships[my_ships_cnt].offset.col = x;
				My_Ships[my_ships_cnt].offset.row = y;
				My_Ships[my_ships_cnt].cube = Hex_to_Cube(My_Ships[my_ships_cnt].offset);

				My_Ships[my_ships_cnt].dir = arg1;
				My_Ships[my_ships_cnt].spd = arg2;
				My_Ships[my_ships_cnt].rum = arg3;
				My_Ships[my_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: COL=%d, ROW=%d, X=%d, Y=%d, Z=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					My_Ships[my_ships_cnt].offset.col, My_Ships[my_ships_cnt].offset.row, 
					My_Ships[my_ships_cnt].cube.x, My_Ships[my_ships_cnt].cube.y, My_Ships[my_ships_cnt].cube.z,
					My_Ships[my_ships_cnt].dir, My_Ships[my_ships_cnt].spd, My_Ships[my_ships_cnt].rum, My_Ships[my_ships_cnt].owner);

				my_ships_cnt++;
			}

			if (strcmp(entityType, "SHIP") == 0 && arg4 == 0) // init En_Ship
			{
				En_Ships[en_ships_cnt].id = i;
				En_Ships[en_ships_cnt].offset.col = x;
				En_Ships[en_ships_cnt].offset.row = y;
				En_Ships[en_ships_cnt].cube = Hex_to_Cube(En_Ships[en_ships_cnt].offset);

				En_Ships[en_ships_cnt].dir = arg1;
				En_Ships[en_ships_cnt].spd = arg2;
				En_Ships[en_ships_cnt].rum = arg3;
				En_Ships[en_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: X=%d, Y=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					En_Ships[en_ships_cnt].offset.col, En_Ships[en_ships_cnt].offset.row, En_Ships[en_ships_cnt].dir,
					En_Ships[en_ships_cnt].spd, En_Ships[en_ships_cnt].rum, En_Ships[en_ships_cnt].owner);

				en_ships_cnt++;
			}

			if (strcmp(entityType, "BARREL") == 0) // init Barrel
			{
				Barrels[barrel_cnt].id = i;
				Barrels[barrel_cnt].offset.col = x;
				Barrels[barrel_cnt].offset.row = y;
				Barrels[barrel_cnt].cube = Hex_to_Cube(Barrels[barrel_cnt].offset);
				Barrels[barrel_cnt].value = arg1;

				//fprintf(stderr, "BARREL %d: X=%d, Y=%d, VALUE=%d\n", i,
				//	Barrels[barrel_cnt].x, Barrels[barrel_cnt].y, Barrels[barrel_cnt].value);

				barrel_cnt++;
			}

			if (strcmp(entityType, "MINE") == 0) // init Mine
			{
				Mines[mine_cnt].id = i;
				Mines[mine_cnt].offset.col = x;
				Mines[mine_cnt].offset.row = y;
				Mines[mine_cnt].cube = Hex_to_Cube(Mines[mine_cnt].offset);

				//fprintf(stderr, "MINE %d: X=%d, Y=%d\n", i,
				//	Mines[mine_cnt].x, Mines[mine_cnt].y);

				mine_cnt++;
			}

			if (strcmp(entityType, "CANNONBALL") == 0) // init Mine
			{
				Cores[core_cnt].id = i;
				Cores[core_cnt].offset.col = x;
				Cores[core_cnt].offset.row = y;
				Cores[core_cnt].cube = Hex_to_Cube(Cores[core_cnt].offset);

				Cores[core_cnt].owner = arg1;
				Cores[core_cnt].turns = arg2;

				fprintf(stderr, "CORE %d: X=%d, Y=%d TURN=%d\n", i,
					Cores[core_cnt].offset.col, Cores[core_cnt].offset.row, Cores[core_cnt].turns);

				core_cnt++;
			}
        }

        for (int i = 0; i < my_ships_cnt; i++)
		//for (int j = 0; j < en_ships_cnt; j++) 
		{
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
			// Any valid action, such as "WAIT" or "MOVE x y"		
			VAL_SQRS(Mines, mine_cnt, Cores, core_cnt, My_Ships[i], En_Ships, en_ships_cnt);

			New_Act = Get_Action(My_Ships[i], i, En_Ships, en_ships_cnt, Barrels, barrel_cnt, 
				Mines, mine_cnt, TURN_ACTIONS);
			TURN_ACTIONS[i] = New_Act;

			if (New_Act.offset.col < 0)		New_Act.offset.col = 0;
			if (New_Act.offset.col > 22)	New_Act.offset.col = 22;
			if (New_Act.offset.row < 0)		New_Act.offset.row = 0;
			if (New_Act.offset.row > 20)	New_Act.offset.row = 20;

			if (New_Act.action == FIRE)
				printf("FIRE %d %d\n", New_Act.offset.col, New_Act.offset.row);
			else if (New_Act.action == MOVE )	
				printf("MOVE %d %d\n", New_Act.offset.col, New_Act.offset.row);
			else  printf("WAIT\n");
			 
        }

		//free(Ships);
		//free(Barrels);
    }
	 
    return 0;
}