#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mapEditing.h"


int fuelConsumedThisTurn(int ddx, int ddy, int dx, int dy, int inSand) {

    int fuelConsumed = ddx * ddx + ddy * ddy;
    fuelConsumed += (int)(sqrt(dx * dx + dy * dy) * 3.0 / 2.0);

    if (inSand) {
        fuelConsumed += 1;
    }
    return -fuelConsumed;
}

int main() {

    char c;
    int i;
    FILE* info = fopen("raceLog.txt", "w");

    MapStructure map;
    int fuel;
    int boostCount = 5;
    int lap = 0;
    int ddx = 1, ddy = 0;
    int dx = 0, dy = 0;
    char action[100];

    /*Reading of the map*/

    readMapFromStdin(&map);
    saveMapAsFile(map, "mapSave.txt");
    freeMap(&map);


    /*Reading positions at each turn*/

    while (!feof(stdin)) {
        lap++;

        while (fread(&c, sizeof(char), 1, stdin) == 1 && c != '\n') {
            fprintf(info, "%c", c);
        }

        fuel += fuelConsumedThisTurn(ddx, ddy, dx, dy, 0);
        dx += ddx;
        dy += ddy;

        sprintf(action, "%d %d", ddx, ddy);
        fprintf(stdout, "%s\n", action);
        fflush(stdout);
        fflush(info);
    }

    return EXIT_SUCCESS;
}