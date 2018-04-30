/**
 * ENSICAEN
 * 6 Boulevard Marechal Juin 
 * F-14050 Caen Cedex 
 *
 * This file is owned by ENSICAEN students.
 * No portion of this document may be reproduced, copied
 * or revised without written permission of the authors.
*/


/**
 * @author LEMOINE Benoît <blemoine@ecole.ensicaen.fr>
 * @version 0.0.1 / 27-11-2017
 */


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/mapEditing.h"
#include "../include/mathObjects.h"

/**
 * @file mapEditingTest.c
 */

void allocateMapGrid_Test() {

    MapStructure map;


    srand(time(NULL));

    map.width = rand()% 500 + 1;
    map.height = rand()% 500 + 1;

    allocateMapGrid(&map);
    if (map.grid == NULL) {
        printf("Warning : allocateMapGrid_Test failed !\n");
        printf("\tCannot allocate memory with size of %d * %d\n",
                map.width, map.height);
        freeMap(&map);
        return;
    }
    freeMap(&map);

    map.width = 0;

    allocateMapGrid(&map);
    if (map.grid != NULL) {
        printf("Warning : allocateMapGrid_Test failed !\n");
        printf("\tTry to allocate memory with size of %d * %d\n",
               map.width, map.height);
        freeMap(&map);
        return;
    }

    map.width = -(rand() +1);
    if (map.grid != NULL) {
        printf("Warning : allocateMapGrid_Test failed !\n");
        printf("\tTry to allocate memory with size of %d * %d\n",
               map.width, map.height);
        freeMap(&map);
        return;
    }

    printf("- allocateMapGrid_Test succeeded !\n");
}

void copyMap_TestOnSmallMap() {

    int i, j;
    MapStructure map;
    MapStructure copiedMap;

    srand(time(NULL));
    map.width = rand() % 500 + 1;
    map.height = rand() % 500 + 1;

    map.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        map.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
        for (j = 0; j < map.height; j++) {
            map.grid[i][j] = (char)rand();
        }
    }

    copiedMap = copyMap(map);

    for (i = 0; i < copiedMap.width; i++) {
        for (j = 0; j < copiedMap.height; j++) {
            if (map.grid[i][j] != copiedMap.grid[i][j]) {
                printf("Warning : copyMap_TestOnSmallMap failed !\n");
                printf("\tIndex [%d][%d] : %d found, %d expected\n", i, j,
                       copiedMap.grid[i][j], map.grid[i][j]);
                freeMap(&map);
                freeMap(&copiedMap);
                return;
            }
        }
    }

    printf("- copyMap_TestOnSmallMap succeeded !\n");
    freeMap(&map);
    freeMap(&copiedMap);
}


void isInGrid_Test() {

    Vector2D position;
    MapStructure map;


    srand(time(NULL));
    map.width = rand();
    map.height = rand();

    position.x = -rand();
    position.y = -rand();

    if (isInGrid(map, position)) {
        printf("Warning : isInGrid_Test failed !\n");
        printf("\t %d %d wasn't on a grid of %d * %d",
               position.x, position.y, map.width, map.height);
        return;
    }

    position.x = rand() % map.width;
    position.y = rand() % map.height;

    if (!isInGrid(map, position)) {
        printf("Warning : isInGrid_Test failed !\n");
        printf("\t %d %d was on a grid of %d * %d",
               position.x, position.y, map.width, map.height);
        return;
    }

    position.x = 0;
    position.y = 0;

    if (!isInGrid(map, position)) {
        printf("Warning : isInGrid_Test failed !\n");
        printf("\t %d %d was on a grid of %d * %d",
               position.x, position.y, map.width, map.height);
        return;
    }

    printf("- isInGrid_Test succeeded !\n");
}


void readMapTile_Test() {

    int i, j;
    Vector2D position;
    MapStructure map;

    srand(time(NULL));

    map.width = rand() % 500 + 1;
    map.height = rand() % 500 + 1;

    position.x = rand() % map.width;
    position.y = rand () % map.height;

    map.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        map.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
    }

    map.grid[position.x][position.y] = '=';

    if (readMapTile(map, position) != '=') {
        printf("Warning : readMapTile_Test failed !\n");
        printf("\t Tile %d %d wasn't correct, on map of %d %d",
               position.x, position.y, map.width, map.height);
        return;
    }

    printf("- readMapTile_Test succeeded !\n");
}


void writeMapTile_Test() {

    int i, j;
    Vector2D position;
    MapStructure map;

    srand(time(NULL));

    map.width = rand() % 500 + 1;
    map.height = rand() % 500 + 1;

    position.x = rand() % map.width;
    position.y = rand() % map.height;

    map.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        map.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
    }

    writeMapTile(&map, position, '*');

    if (map.grid[position.x][position.y] != '*') {
        printf("Warning : writeMapTile_Test failed !\n");
        printf("\t Tile %d %d wasn't correct, on map of %d %d",
               position.x, position.y, map.width, map.height);
        return;
    }

    printf("- writeMapTile_Test succeeded !\n");
}


void regenMapTile_Test() {

    int i;
    Vector2D position;
    MapStructure referenceMap, modifiedMap;


    srand(time(NULL));

    referenceMap.width = rand() % 500 + 1;
    referenceMap.height = rand() % 500 + 1;
    modifiedMap.width = referenceMap.width;
    modifiedMap.height = referenceMap.height;

    position.x = rand() % referenceMap.width;
    position.y = rand() % referenceMap.height;


    referenceMap.grid = (char**)calloc((size_t)referenceMap.width, sizeof(char*));
    for (i = 0; i < referenceMap.width; i++) {
        referenceMap.grid[i] = (char*)calloc((size_t)referenceMap.height, sizeof(char));
    }

    modifiedMap.grid = (char**)calloc((size_t)modifiedMap.width, sizeof(char*));
    for (i = 0; i < referenceMap.width; i++) {
        modifiedMap.grid[i] = (char*)calloc((size_t)modifiedMap.height, sizeof(char));
    }


    referenceMap.grid[position.x][position.y] = '=';
    regenMapTile(referenceMap, &modifiedMap, position);

    if (referenceMap.grid[position.x][position.y] != modifiedMap.grid[position.x][position.y]) {
        printf("Warning : regenMapTile_Test failed !\n");
        printf("\t Tile %d %d wasn't correct, found %c, expected %c", position.x, position.y,
               modifiedMap.grid[position.x][position.y], referenceMap.grid[position.x][position.y]);
        freeMap(&referenceMap);
        freeMap(&modifiedMap);
        return;
    }

    printf("- regenMapTile_Test succeeded !\n");
    freeMap(&referenceMap);
    freeMap(&modifiedMap);
}


void isArrival_Test() {

    int i;
    Vector2D position;
    MapStructure map;


    srand(time(NULL));

    map.width = rand() % 500 + 1;
    map.height = rand() % 500 + 1;

    position.x = rand() % map.width;
    position.y = rand() % map.height;

    map.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        map.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
    }

    map.grid[position.x][position.y] = '=';

    if (!isArrival(map, position)) {
        printf("Warning : isArrival_Test failed !\n");
        printf("\t Tile %d %d was the arrival", position.x, position.y);
        freeMap(&map);
        return;
    }

    map.grid[position.x][position.y] = '*';

    if (isArrival(map, position)) {
        printf("Warning : isArrival_Test failed !\n");
        printf("\t Tile %d %d wasn't the arrival", position.x, position.y);
        freeMap(&map);
        return;
    }

    printf("- isArrival_Test succeeded !\n");
    freeMap(&map);
}

void isCrossable_Test() {

    int i;
    MapStructure map;
    Vector2D departure, arrival;


    map.height = 1;
    map.width = 3;

    map.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        map.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
    }

    departure.x = 0;
    departure.y = 0;
    arrival.x = 2;
    arrival.y = 0;


    map.grid[0][0] = '#';
    map.grid[1][0] = '#';
    map.grid[2][0] = '#';

    if(isCrossable(map, departure, arrival) != 1) {
        printf("Warning : isCrossable_Test failed !\n");
        printf("Segment was crossable !");
        freeMap(&map);
        return;
    }


    map.grid[0][0] = '#';
    map.grid[1][0] = '.';
    map.grid[2][0] = '#';

    if(isCrossable(map, departure, arrival) != 0) {
        printf("Warning : isCrossable_Test failed !\n");
        printf("Segment wasn't crossable !");
        freeMap(&map);
        return;
    }


    map.grid[0][0] = '.';
    map.grid[1][0] = '#';
    map.grid[2][0] = '#';

    if(isCrossable(map, departure, arrival) != 0) {
        printf("Warning : isCrossable_Test failed !\n");
        printf("Departure wasn't drivable !");
        freeMap(&map);
        return;
    }

    map.grid[0][0] = '#';
    map.grid[1][0] = '#';
    map.grid[2][0] = '.';

    if(isCrossable(map, departure, arrival) != 0) {
        printf("Warning : isCrossable_Test failed !\n");
        printf("Arrival wasn't drivable !");
        freeMap(&map);
        return;
    }

    printf("- isCrossable_Test succeeded !\n");
    freeMap(&map);
}

int main() {

    allocateMapGrid_Test();
    copyMap_TestOnSmallMap();
    isInGrid_Test();
    readMapTile_Test();
    writeMapTile_Test();
    regenMapTile_Test();
    isArrival_Test();
    isCrossable_Test();

    return 0;
}
 
 