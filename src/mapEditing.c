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


/**
 * @file mapEditing.c
 */

#include "../include/mapEditing.h"
#include <stdio.h>
#include <stdlib.h>


void readMapFromStdin(MapStructure* map) {

    int i, j;
    char charBuffer;

    fscanf(stdin, "%d %d %d", &map->width, &map->height, &map->fuelAvailable);
    while (fread(&charBuffer, sizeof(char), 1, stdin) == 1 && charBuffer != '\n') {}


    map->grid = (char**)calloc((size_t)map->width, sizeof(char*));
    for (i = 0; i < map->width; i++) {
        map->grid[i] = (char*)calloc((size_t)map->height, sizeof(char));
    }

    if (map->grid == NULL) {
        fprintf(stderr, "Memory for the map cannot be allocated");
        return;
    }


    for (j = 0; j < map->height; j++) {
        i = 0;
        while (fread(&charBuffer, sizeof(char), 1, stdin) == 1 && charBuffer != '\n') {
            map->grid[i][j] = charBuffer;
            i++;
        }
    }
}


void saveMapAsFile(MapStructure map, char* filename) {

    int i, j;
    FILE* mapFile;

    mapFile = fopen(filename, "w");
    if (mapFile == NULL) {
        fprintf(stderr, "Cannot open file %s", filename);
        return;
    }

    fprintf(mapFile,"%d %d %d\n", map.width, map.height, map.fuelAvailable);

    for (j = 0; j < map.height; j++) {
        for (i = 0; i < map.width; i++) {
            fputc(map.grid[i][j], mapFile);
        }
        fputc('\n', mapFile);
    }

    fclose(mapFile);
}


MapStructure copyMap(MapStructure map) {

    int i, j;
    MapStructure copiedMap;

    copiedMap.width = map.width;
    copiedMap.height = map.height;
    copiedMap.fuelAvailable = map.fuelAvailable;

    copiedMap.grid = (char**)calloc((size_t)map.width, sizeof(char*));
    for (i = 0; i < map.width; i++) {
        copiedMap.grid[i] = (char*)calloc((size_t)map.height, sizeof(char));
    }

    for (i = 0; i < map.width; i++) {
        for (j = 0; j < map.height; j++) {
            copiedMap.grid[i][j] = map.grid[i][j];
        }
    }

    return copiedMap;
}


void freeMap(MapStructure* map) {

    int i;

    for (i = 0; i < map->width; i++) {
        free(map->grid[i]);
    }
    free(map->grid);
}


int isInGrid(MapStructure map, int x, int y) {

    if (x > map.width || x < 0) {
        return 0;
    }

    if ( y > map.height || y < 0) {
        return 0;
    }

    return 1;
}


char readMapTile(MapStructure map, int x, int y) {

    if (isInGrid(map, x, y)) {
        return '.';
    }

    return map.grid[x][y];
}


void writeMapTile(MapStructure* map, int x, int y, char characterToWrite) {

    if (isInGrid(*map, x, y)) {
        return;
    }

    map->grid[x][y] = characterToWrite;
}


void regenMapTile(MapStructure originalMap, MapStructure* editedMap, int x, int y) {

    if (isInGrid(originalMap, x, y)) {
        return;
    }

    editedMap->grid[x][y] = originalMap.grid[x][y];
}


int isArrival(MapStructure map, int x, int y) {

    if (isInGrid(map, x, y)) {
        return 0;
    }

    if (map.grid[x][y] == '=') {
        return 1;
    }

    return 0;
}
