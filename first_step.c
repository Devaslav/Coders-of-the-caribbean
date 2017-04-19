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
	int col;
	int row;
	int val;
}Hex_Point;
////////////////////////
typedef struct 
{
	int id;
	Hex_Point hex[3];
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
	Hex_Point hex;
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
	int turn;
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

//////////////////////////////////////////////////////////////////////////
void VAL_HEXS(_MINE_ *Mines, int m_cnt, CANNONBALL *Cores, int c_cnt, SHIP My_Ship, SHIP *En_Ships, int En_Cnt)
{
	int i,j,k;
	Hex_Point Temp_Hex;

	for (j = 0;j < 21; j++)
	for (i = 0;i < 23; i++)
	{	
		HEXS[i][j].val = 1;	
	}




}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ACTION Get_Action(SHIP My_Ship, int My_Ship_Cnt, SHIP *En_Ships, int En_Cnt,  BARREL *Barrels, int Bar_Cnt,
	_MINE_ *Mines, int m_cnt, ACTION *PREV_ACT)
{
	double Dist;
	ACTION ACT;
	int i,j;

	Dist = 100;

	ACT.action = WAIT;
	ACT.turn = 100;


	//fprintf(stderr, "Bar_Cnt = %d\n", Bar_Cnt);
	for (int En_ID = 0; En_ID<En_Cnt; En_ID++)
	if( PREV_ACT[My_Ship_Cnt].action != FIRE
		 //&& (Hex_Dist(My_Ship.hex, En_Ships[En_ID].hex) < Dist )
		 && ((Hex_Dist(My_Ship.hex[1], En_Ships[En_ID].hex[1]) <= 8 && My_Ship.spd > 0)
		 || (Hex_Dist(My_Ship.hex[1], En_Ships[En_ID].hex[1]) <= 4)) )
	{
		ACT.action = FIRE;
		ACT.hex.col = En_Ships[En_ID].hex[1].col;
		ACT.hex.row = En_Ships[En_ID].hex[1].row;

		Dist = Hex_Dist(My_Ship.hex[1], En_Ships[En_ID].hex[1]);

		for (i = 0; i < 8; i++)
			for (j = 0; j < 8; j++)
			{

			}

		
		fprintf(stderr, "FIRE : TRG=%d X=%d Y=%d\n", En_Ships[En_ID].id, ACT.hex.col, ACT.hex.row);
	}
	

	if ( (Bar_Cnt > 0 && ACT.action != FIRE) || My_Ship.rum <=50)
	{
		ACT.action = MOVE;
		Dist = 100;

		for (i = 0; i < Bar_Cnt; i++)
		{
			if (Hex_Dist(My_Ship.hex[1], Barrels[i].hex) < Dist)
			{
				Dist = Hex_Dist(My_Ship.hex[1], Barrels[i].hex);
				ACT.hex.col = Barrels[i].hex.col;
				ACT.hex.row = Barrels[i].hex.row;

				//fprintf(stderr, "TRG_BAR==%d\n", i);
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
	//GET_SQRS();

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
				My_Ships[my_ships_cnt].hex[1].col = x;
				My_Ships[my_ships_cnt].hex[1].row = y;
				My_Ships[my_ships_cnt].dir = arg1;
				My_Ships[my_ships_cnt].hex[0] = Shift_Hex(My_Ships[my_ships_cnt].hex[1], arg1, 0);
				My_Ships[my_ships_cnt].hex[2] = Shift_Hex(My_Ships[my_ships_cnt].hex[1], arg1, 1);

				My_Ships[my_ships_cnt].spd = arg2;
				My_Ships[my_ships_cnt].rum = arg3;
				My_Ships[my_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: FC=%d, FR=%d, CC=%d, CR=%d, BC=%d, BR=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					My_Ships[my_ships_cnt].hex[0].col, My_Ships[my_ships_cnt].hex[0].row,
					My_Ships[my_ships_cnt].hex[1].col, My_Ships[my_ships_cnt].hex[1].row,
					My_Ships[my_ships_cnt].hex[2].col, My_Ships[my_ships_cnt].hex[2].row,
					My_Ships[my_ships_cnt].dir, My_Ships[my_ships_cnt].spd, My_Ships[my_ships_cnt].rum, My_Ships[my_ships_cnt].owner);

				my_ships_cnt++;
			}

			if (strcmp(entityType, "SHIP") == 0 && arg4 == 0) // init En_Ship
			{
				En_Ships[en_ships_cnt].id = i;
				En_Ships[en_ships_cnt].hex[1].col = x;
				En_Ships[en_ships_cnt].hex[2].row = y;
				En_Ships[en_ships_cnt].dir = arg1;
				En_Ships[en_ships_cnt].hex[0] = Shift_Hex(En_Ships[en_ships_cnt].hex[1], arg1, 0);
				En_Ships[en_ships_cnt].hex[2] = Shift_Hex(En_Ships[en_ships_cnt].hex[1], arg1, 1);

				En_Ships[en_ships_cnt].spd = arg2;
				En_Ships[en_ships_cnt].rum = arg3;
				En_Ships[en_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: COL_C=%d, ROW_C=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					En_Ships[en_ships_cnt].hex[1].col, En_Ships[en_ships_cnt].hex[1].row,
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
				Mines[mine_cnt].hex.col = x;
				Mines[mine_cnt].hex.row = y;

				fprintf(stderr, "MINE %d: X=%d, Y=%d\n", i,
					Mines[mine_cnt].hex.col, Mines[mine_cnt].hex.row);

				mine_cnt++;
			}

			if (strcmp(entityType, "CANNONBALL") == 0) // init Core
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