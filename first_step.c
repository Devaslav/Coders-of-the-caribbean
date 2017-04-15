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
	MOVE,
	SLOWER,
	WAIT
} Actions;

typedef struct
{
	int x;
	int y;
	Actions Act;
};


BARREL Barrels[100];
SHIP *Ships;

//////////////////////////////////////////////////////////////////////////
// MACROSES
#define GET_DIST(X1, Y1, X2, Y2) ( sqrt((X1-X2)*(X1-X2)+(Y1-Y2)*(Y1-Y2)) ) 

//////////////////////////////////////////////////////////////////////////
// FUNCTIONS
int Get_Action(SHIP *Ships, int ID, BARREL *Barrels, int Bar_Cnt)
{
	int BAR_ID;
	double Dist;

	BAR_ID = 0;
	Dist = GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[0].x, Barrels[0].y);

	for (int i = 1;i < Bar_Cnt;i++)
	{
		if (GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[i].x, Barrels[i].y) < Dist)
		{
			Dist = GET_DIST(Ships[ID].x, Ships[ID].y, Barrels[i].x, Barrels[i].y);
			BAR_ID = i;
		}
	}

	return BAR_ID;
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
	Ships = (SHIP *)calloc(100, sizeof(SHIP)); // Max my ships == 1

    // game loop
    while (1) {
        static int myShipCount; // the number of remaining ships
        scanf("%d", &myShipCount);
		
		int my_ships_cnt=0;
		int en_ships_cnt=0;

		int barrel_cnt = 0;

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

			if (strcmp(entityType, "SHIP") == 0) // init Ship
			{
				Ships[my_ships_cnt].id = i;
				Ships[my_ships_cnt].x = x;
				Ships[my_ships_cnt].y = y;
				Ships[my_ships_cnt].dir = arg1;
				Ships[my_ships_cnt].spd = arg2;
				Ships[my_ships_cnt].rum = arg3;
				Ships[my_ships_cnt].owner = arg4;

				fprintf(stderr, "SHIP %d: X=%d, Y=%d, DIR=%d, SPD=%d, RUM=%d, OWN=%d\n", i,
					Ships[my_ships_cnt].x, Ships[my_ships_cnt].y, Ships[my_ships_cnt].dir, 
					Ships[my_ships_cnt].spd, Ships[my_ships_cnt].rum, Ships[my_ships_cnt].owner);

				my_ships_cnt++;
			}
			else
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
			int Bar_ID;
			Bar_ID = Get_Action(Ships, i, Barrels, barrel_cnt);
			
			if (barrel_cnt > 0) 
            printf("MOVE %d %d\n", Barrels[Bar_ID].x, Barrels[Bar_ID].y); // Any valid action, such as "WAIT" or "MOVE x y"
			else  printf("WAIT");

        }

		//free(Ships);
		//free(Barrels);
    }

    return 0;
}