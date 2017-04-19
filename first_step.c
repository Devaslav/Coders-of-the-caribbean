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
	Hex_Point hex;
	int dir; // the ship's rotation orientation (between 0 and 5)
	int spd; // the ship's speed (between 0 and 2)
	int rum; // the ship's stock of rum units
	int owner; // 1 if the ship is controlled by you, 0 otherwise 

} SHIP;

typedef struct
{
	int id;
	Hex_Point hex;
	int value; // value of rum units(10-26)

} BARREL;

typedef struct
{
	int id;
	Hex_Point hex;
	int owner; // the entityId of the ship that fired this cannon ball
	int turns; // number of turns before impact (1 means the cannon ball will land at the end of the current turn)
} CANNONBALL;

typedef struct
{
	int id;
	Hex_Point offset;
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
	Hex_Point hex;
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

#define N_HEX 483 // 21*23 
#define N_HEX_Y 21 
#define N_HEX_X 23
Hex_Point HEXS[N_HEX_X][N_HEX_Y];

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
Hex_Point Hex_Dirs [6] =
{
		{+1, 0}, {0, -1}, {-1, -1},
		{-1, 0}, {-1, +1},{0, +1},
};

// Hex_Plus(Hex_Point a, Hex_Point b)
Hex_Point Hex_Plus(Hex_Point a, Hex_Point b)
{
	Hex_Point c;
	c.col = a.col + b.col;
	c.row = a.row + b.row;
	return c;
}

// Offset_Dist(Hex_Point a, Hex_Point b)
Hex_Dist(Hex_Point a, Hex_Point b)
{
	return (abs(a.col - b.col)
		+ abs(a.col + a.row - b.col - b.row)
		+ abs(a.row - b.row)) / 2;
}

// 
Hex_Point Shift_Hex(Hex_Point In_Hex, int DIR, int Revers)
{
	if (Revers == 1) { DIR = (DIR + 3) % 5;	}


	switch (DIR)
	{
	case 0:
		In_Hex.col++;
		break;

	case 1:
		In_Hex.col= In_Hex.col + In_Hex.row % 2;
		In_Hex.row--;
		break;

	case 2:
		if (In_Hex.row % 2 == 0) In_Hex.col--;
		In_Hex.row--;
		break;

	case 3:
		In_Hex.col--;
		break;

	case 4:
		if (In_Hex.row % 2 == 0) In_Hex.col--;
		In_Hex.row++;
		break;

	case 5:
		In_Hex.col = In_Hex.col + In_Hex.row % 2;
		In_Hex.row++;
		break;

	default:
		break;
	}

	return In_Hex;
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
void VAL_HEXS(_MINE_ *Mines, int m_cnt, CANNONBALL *Cores, int c_cnt, SHIP My_Ship, SHIP *En_Ships, int En_Cnt)
{
	int i,j,k;
	Hex_Point Temp_Hex;

	for (j = 0;j < 21; j++)
	{
		if(SQRS[i].valid != 0) SQRS[i].valid = 1;
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


	//fprintf(stderr, "Bar_Cnt = %d\n", Bar_Cnt);
	for (int En_ID = 0; En_ID<En_Cnt; En_ID++)
	if ( PREV_ACT[My_Ship_Cnt].action != FIRE
		 && (Hex_Dist(My_Ship.hex, En_Ships[En_ID].hex) < Dist )
		 && ((Hex_Dist(My_Ship.hex, En_Ships[En_ID].hex) <= 8 && My_Ship.spd > 0)
		 || (Hex_Dist(My_Ship.hex, En_Ships[En_ID].hex) <= 3)) )
	{
		ACT.action = FIRE;
		ACT.hex.col = En_Ships[En_ID].hex.col;
		ACT.hex.row = En_Ships[En_ID].hex.row;

		Dist = Hex_Dist(My_Ship.hex, En_Ships[En_ID].hex);

		fprintf(stderr, "FIRE : TRG=%d X=%d Y=%d\n", En_Ships[En_ID].id, ACT.hex.col, ACT.hex.row);
		 
		if (En_Ships[En_ID].dir == 0) { ACT.hex.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 1) { ACT.hex.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.hex.row -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 2) { ACT.hex.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.hex.row -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 3) { ACT.hex.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }		
		if (En_Ships[En_ID].dir == 4) { ACT.hex.col -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.hex.row += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 5) { ACT.hex.col += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/ 2; ACT.hex.row += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		
	}
	

	if ( (Bar_Cnt > 0 && ACT.action != FIRE) || My_Ship.rum <=50)
	{
		ACT.action = MOVE;
		Dist = 100;

		for (i = 0; i < Bar_Cnt; i++)
		{
			if (Hex_Dist(My_Ship.hex, Barrels[i].hex) < Dist
					&& SQRS[GET_SQRS_ID(Barrels[i].hex.col, Barrels[i].hex.row)].valid == 1)
			{
				Dist = Hex_Dist(My_Ship.hex, Barrels[i].hex);
				ACT.hex.col = Barrels[i].hex.col;
				ACT.hex.row = Barrels[i].hex.row;

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
			if ( (GET_DIST(My_Ship.hex.col, My_Ship.hex.row, SQRS[i].cx, SQRS[i].cy) < Dist)
				&& SQRS[i].valid == 1 )
			{
				Dist = GET_DIST(My_Ship.hex.col, My_Ship.hex.row, SQRS[i].cx, SQRS[i].cy);
				ACT.hex.col = (int )SQRS[i].cx;
				ACT.hex.row = (int )SQRS[i].cy;

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
        for (int i = 0; i < entityCount; i++) 
		{
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
				My_Ships[my_ships_cnt].hex.col = x;
				My_Ships[my_ships_cnt].hex.row = y;

				My_Ships[my_ships_cnt].dir = arg1;
				My_Ships[my_ships_cnt].spd = arg2;
				My_Ships[my_ships_cnt].rum = arg3;
				My_Ships[my_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: COL=%d, ROW=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					My_Ships[my_ships_cnt].hex.col, My_Ships[my_ships_cnt].hex.row,
					My_Ships[my_ships_cnt].dir, My_Ships[my_ships_cnt].spd, My_Ships[my_ships_cnt].rum, My_Ships[my_ships_cnt].owner);

				my_ships_cnt++;
			}

			if (strcmp(entityType, "SHIP") == 0 && arg4 == 0) // init En_Ship
			{
				En_Ships[en_ships_cnt].id = i;
				En_Ships[en_ships_cnt].hex.col = x;
				En_Ships[en_ships_cnt].hex.row = y;

				En_Ships[en_ships_cnt].dir = arg1;
				En_Ships[en_ships_cnt].spd = arg2;
				En_Ships[en_ships_cnt].rum = arg3;
				En_Ships[en_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: COL=%d, ROW=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					En_Ships[en_ships_cnt].hex.col, En_Ships[en_ships_cnt].hex.row,
					En_Ships[en_ships_cnt].dir,En_Ships[en_ships_cnt].spd, En_Ships[en_ships_cnt].rum, En_Ships[en_ships_cnt].owner);

				en_ships_cnt++;
			}

			if (strcmp(entityType, "BARREL") == 0) // init Barrel
			{
				Barrels[barrel_cnt].id = i;
				Barrels[barrel_cnt].hex.col = x;
				Barrels[barrel_cnt].hex.row = y;
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

				fprintf(stderr, "MINE %d: X=%d, Y=%d\n", i,
					Mines[mine_cnt].offset.col, Mines[mine_cnt].offset.row);

				mine_cnt++;
			}

			if (strcmp(entityType, "CANNONBALL") == 0) // init Mine
			{
				Cores[core_cnt].id = i;
				Cores[core_cnt].hex.col = x;
				Cores[core_cnt].hex.row = y;

				Cores[core_cnt].owner = arg1;
				Cores[core_cnt].turns = arg2;

				fprintf(stderr, "CORE %d: X=%d, Y=%d TURN=%d\n", i,
					Cores[core_cnt].hex.col, Cores[core_cnt].hex.row, Cores[core_cnt].turns);

				core_cnt++;
			}
        }

        for (int i = 0; i < my_ships_cnt; i++)
		//for (int j = 0; j < en_ships_cnt; j++) 
		{
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
			// Any valid action, such as "WAIT" or "MOVE x y"		
			VAL_HEXS(Mines, mine_cnt, Cores, core_cnt, My_Ships[i], En_Ships, en_ships_cnt);

			New_Act = Get_Action(My_Ships[i], i, En_Ships, en_ships_cnt, Barrels, barrel_cnt, 
				Mines, mine_cnt, TURN_ACTIONS);
			TURN_ACTIONS[i] = New_Act;

			if (New_Act.hex.col < 0)	New_Act.hex.col = 0;
			if (New_Act.hex.col > 22)	New_Act.hex.col = 22;
			if (New_Act.hex.row < 0)	New_Act.hex.row = 0;
			if (New_Act.hex.row > 20)	New_Act.hex.row = 20;

			if (New_Act.action == FIRE)
				printf("FIRE %d %d\n", New_Act.hex.col, New_Act.hex.row);
			else if (New_Act.action == MOVE )	
				printf("MOVE %d %d\n", New_Act.hex.col, New_Act.hex.row);
			else  printf("WAIT\n");
			 
        }

		//free(Ships);
		//free(Barrels);
    }
	 
    return 0;
}