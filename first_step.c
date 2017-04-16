#define _CRT_SECURE_NO_WARNINGS

#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////
// VARS AND STRUCTS
typedef struct 
{
	int id;
	int x; // 0-22
	int y; // 0-20
	int dir; // the ship's rotation orientation (between 0 and 5)
	int spd; // the ship's speed (between 0 and 2)
	int rum; // the ship's stock of rum units
	int owner; // 1 if the ship is controlled by you, 0 otherwise 

} SHIP;

typedef struct
{
	int id;
	int x; // 0-22
	int y; // 0-20
	int value; // value of rum units(10-26)

} BARREL;

typedef struct
{
	int id;
	int x; // 0-22
	int y; // 0-20
	int owner; // the entityId of the ship that fired this cannon ball
	int turns; // number of turns before impact (1 means the cannon ball will land at the end of the current turn)
} CANNONBALL;

typedef struct
{
	int id;
	int x; // 0-22
	int y; // 0-20
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
	int x;
	int y;
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

ACTION TURN_ACTIONS[3];

#define N_SQR 110//(20 / 2) * (22 / 2)
SQUARE SQRS[N_SQR];

//////////////////////////////////////////////////////////////////////////
// MACROSES
#define GET_DIST(X1, Y1, X2, Y2) ( sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)) ) 
#define GET_TURNS(X) ( (1+(X) / 3.0) )

//////////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void GET_SQRS(void)
{
	int i,j;

	for(j=0;j<=10;j++)
		for (i = 0;i <= 11;i++)
		{
			SQRS[j * 11 + i].x1 = i * 2;
			SQRS[j * 11 + i].x2 = i * 2 + 1;
			SQRS[j * 11 + i].y1 = j * 2;
			SQRS[j * 11 + i].y2 = j * 2 + 1;
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
void VAL_SQRS(_MINE_ *Mines, int m_cnt, SHIP My_Ship, SHIP *En_Ships, int En_Cnt)
{
	int i,j;

	for (i = 0;i < N_SQR;i++)
	{
		if(SQRS[i].valid != 0) SQRS[i].valid = 1;
	}

	for(i=0;i<m_cnt;i++)
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], Mines[i].x, Mines[i].y) == TRUE ) SQRS[j].valid = -1;
		}
	 
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], My_Ship.x, My_Ship.y) == TRUE ) SQRS[j].valid = 0;
		}

	for (i = 0;i<En_Cnt;i++)
		for (j = 0;j < N_SQR;j++)
		{
			if (IN_SQR(SQRS[j], En_Ships[i].x, En_Ships[i].y) == TRUE) SQRS[j].valid = -2;
		}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ACTION Get_Action(SHIP My_Ship, int My_Ship_Cnt, SHIP *En_Ships, int En_Cnt,  BARREL *Barrels, int Bar_Cnt, ACTION *PREV_ACT)
{
	double Dist;
	ACTION ACT;
	int i;

	ACT.action = WAIT;

	//fprintf(stderr, "Bar_Cnt = %d\n", Bar_Cnt);
	for (int En_ID = 0; En_ID<En_Cnt; En_ID++)
	if ( PREV_ACT[My_Ship_Cnt].action != FIRE 
		 && (GET_DIST(My_Ship.x, My_Ship.y, En_Ships[En_ID].x, En_Ships[En_ID].y) < 8.0 && My_Ship.spd > 0)
		 || (GET_DIST(My_Ship.x, My_Ship.y, En_Ships[En_ID].x, En_Ships[En_ID].y) < 2.5) )
	{
		ACT.action = FIRE;
		ACT.x = En_Ships[En_ID].x;
		ACT.y = En_Ships[En_ID].y;  
		Dist = GET_DIST(My_Ship.x, My_Ship.y, En_Ships[En_ID].x, En_Ships[En_ID].y);

		if (En_Ships[En_ID].dir == 0) { ACT.x += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 1) { ACT.x += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/2; ACT.y -= En_Ships[En_ID].spd*(int )GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 2) { ACT.x -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/2; ACT.y -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 3) { ACT.x -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 4) { ACT.x -= En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/2; ACT.y += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
		if (En_Ships[En_ID].dir == 5) { ACT.x += En_Ships[En_ID].spd*(int)GET_TURNS(Dist)/2; ACT.y += En_Ships[En_ID].spd*(int)GET_TURNS(Dist); }
	}
	

	if (Bar_Cnt > 0 && ACT.action != FIRE)
	{
		ACT.action = MOVE;
		Dist = 10000.0;

		for (i = 0; i < Bar_Cnt; i++)
		{
			if (GET_DIST(My_Ship.x, My_Ship.y, Barrels[i].x, Barrels[i].y) < Dist
					&& SQRS[GET_SQRS_ID(Barrels[i].x, Barrels[i].y)].valid == 1)
			{
				Dist = GET_DIST(My_Ship.x, My_Ship.y, Barrels[i].x, Barrels[i].y);
				ACT.x = Barrels[i].x;
				ACT.y = Barrels[i].y;

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
			if ( (GET_DIST(My_Ship.x, My_Ship.y, SQRS[i].cx, SQRS[i].cy) < Dist) 
				&& SQRS[i].valid == 1 )
			{
				Dist = GET_DIST(My_Ship.x, My_Ship.y, SQRS[i].cx, SQRS[i].cy);
				ACT.x = (int )SQRS[i].cx;
				ACT.y = (int )SQRS[i].cy;

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
				My_Ships[my_ships_cnt].x = x;
				My_Ships[my_ships_cnt].y = y;
				My_Ships[my_ships_cnt].dir = arg1;
				My_Ships[my_ships_cnt].spd = arg2;
				My_Ships[my_ships_cnt].rum = arg3;
				My_Ships[my_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: X=%d, Y=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					My_Ships[my_ships_cnt].x, My_Ships[my_ships_cnt].y, My_Ships[my_ships_cnt].dir,
					My_Ships[my_ships_cnt].spd, My_Ships[my_ships_cnt].rum, My_Ships[my_ships_cnt].owner);

				my_ships_cnt++;
			}

			if (strcmp(entityType, "SHIP") == 0 && arg4 == 0) // init En_Ship
			{
				En_Ships[en_ships_cnt].id = i;
				En_Ships[en_ships_cnt].x = x;
				En_Ships[en_ships_cnt].y = y;
				En_Ships[en_ships_cnt].dir = arg1;
				En_Ships[en_ships_cnt].spd = arg2;
				En_Ships[en_ships_cnt].rum = arg3;
				En_Ships[en_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: X=%d, Y=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					En_Ships[en_ships_cnt].x, En_Ships[en_ships_cnt].y, En_Ships[en_ships_cnt].dir,
					En_Ships[en_ships_cnt].spd, En_Ships[en_ships_cnt].rum, En_Ships[en_ships_cnt].owner);

				en_ships_cnt++;
			}

			if (strcmp(entityType, "BARREL") == 0) // init Barrel
			{
				Barrels[barrel_cnt].id = i;
				Barrels[barrel_cnt].x = x;
				Barrels[barrel_cnt].y = y;
				Barrels[barrel_cnt].value = arg1;

				//fprintf(stderr, "BARREL %d: X=%d, Y=%d, VALUE=%d\n", i,
				//	Barrels[barrel_cnt].x, Barrels[barrel_cnt].y, Barrels[barrel_cnt].value);

				barrel_cnt++;
			}

			if (strcmp(entityType, "MINE") == 0) // init Mine
			{
				Mines[mine_cnt].id = i;
				Mines[mine_cnt].x = x;
				Mines[mine_cnt].y = y;

				fprintf(stderr, "MINE %d: X=%d, Y=%d\n", i,
					Mines[mine_cnt].x, Mines[mine_cnt].y);

				mine_cnt++;
			}
        }

        for (int i = 0; i < my_ships_cnt; i++)
		//for (int j = 0; j < en_ships_cnt; j++) 
		{
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
			// Any valid action, such as "WAIT" or "MOVE x y"		
			VAL_SQRS(Mines, mine_cnt, My_Ships[i], En_Ships, en_ships_cnt);

			New_Act = Get_Action(My_Ships[i], i, En_Ships, en_ships_cnt, Barrels, barrel_cnt, TURN_ACTIONS);
			TURN_ACTIONS[i] = New_Act;

			if (New_Act.x < 0)	New_Act.x = 0;
			if (New_Act.x > 22) New_Act.x = 22;
			if (New_Act.y < 0)	New_Act.y = 0;
			if (New_Act.y > 20) New_Act.y = 20;

			if (New_Act.action == FIRE)
				printf("FIRE %d %d\n", New_Act.x, New_Act.y);
			else if (New_Act.action == MOVE )	
				printf("MOVE %d %d\n", New_Act.x, New_Act.y);
			else  printf("WAIT\n");
			 
        }

		//free(Ships);
		//free(Barrels);
    }
	 
    return 0;
}