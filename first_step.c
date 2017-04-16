#define _CRT_SECURE_NO_WARNINGS


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////
// VARS
typedef struct 
{
	int id;
	int x; // 0-22
	int y; // 0-20
	int spd; // speed
	int dir; // orientation, direction
	int rum; // value of rum units
	int owner; // 1-I ,0-Enemy

} SHIP;

typedef struct
{
	int id;
	int x; // 0-22
	int y; // 0-20
	int value; // value of rum units(10-26)

} BARREL;

typedef enum
{
	FIRE,
	MINE,
	MOVE,
	SLOWER,
	WAIT
} Actions;

typedef struct
{
	int x;
	int y;
	Actions action;
} ACTION;

////////////////////////
BARREL Barrels[100];
SHIP My_Ships[3];
SHIP En_Ships[3];


//////////////////////////////////////////////////////////////////////////
// MACROSES
#define GET_DIST(X1, Y1, X2, Y2) ( sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)) ) 

//////////////////////////////////////////////////////////////////////////
// FUNCTIONS
ACTION Get_Action(SHIP *Ships, int ID, BARREL *Barrels, int Bar_Cnt)
{
	double Dist;
	ACTION ACT;

	if (Bar_Cnt > 0)
	{
		ACT.action = MOVE;
		ACT.x = Barrels[0].x;
		ACT.y = Barrels[0].y;
		Dist = GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[0].x, Barrels[0].y);

		for (int i = 0; i < Bar_Cnt; i++)
		{
			if (GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[i].x, Barrels[i].y) < Dist)
			{
				Dist = GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[i].x, Barrels[i].y);
				ACT.x = Barrels[i].x;
				ACT.y = Barrels[i].y;
			}
		}

		
	}
	else
	{
		ACT.action = WAIT;
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

	ACTION New_Act;

	//Ships = (SHIP *)calloc(100, sizeof(SHIP)); // Max my ships == 1
    // game loop
    while (1)
	{
		my_ships_cnt = 0;
		en_ships_cnt = 0;
		barrel_cnt = 0;

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

				fprintf(stderr, "BARREL %d: X=%d, Y=%d, VALUE=%d\n", i,
					Barrels[barrel_cnt].x, Barrels[barrel_cnt].y, Barrels[barrel_cnt].value);

				barrel_cnt++;
			}
        }

        for (int i = 0; i < myShipCount; i++) 
		{
            // Write an action using printf(). DON'T FORGET THE TRAILING \n
            // To debug: fprintf(stderr, "Debug messages...\n");
			// Any valid action, such as "WAIT" or "MOVE x y"		
			

			New_Act = Get_Action(My_Ships, My_Ships[i].id, Barrels, barrel_cnt);

			if (New_Act.action == MOVE )	
				printf("MOVE %d %d\n", New_Act.x, New_Act.y);
			else  printf("WAIT\n");
			 
        }

		//free(Ships);
		//free(Barrels);
    }
	 
    return 0;
}