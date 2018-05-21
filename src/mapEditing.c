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
#include "../include/mathObjects.h"
#include "../include/gpcontrol.h"
#include <stdio.h>
#include <stdlib.h>


void allocateMapGrid (MapStructure* map) {

    int i;

    if (map->height <= 0 || map->width <= 0) {
        fprintf(stderr, "Map of wrong dimensions\n");
        map->grid = NULL;
        return;
    }

    map->grid = (char**)calloc((size_t)map->width, sizeof(char*));
    for (i = 0; i < map->width; i++) {
        map->grid[i] = (char*)calloc((size_t)map->height, sizeof(char));
    }

    if (map->grid == NULL) {
        fprintf(stderr, "Memory for the map cannot be allocated\n");
    }
}

void readMapFromStdin(MapStructure* map) {

    int i, j;
    char charBuffer;

    fscanf(stdin, "%d %d %d", &map->width, &map->height, &map->fuelAvailable);
    while (fread(&charBuffer, sizeof(char), 1, stdin) == 1 && charBuffer != '\n') {}

    allocateMapGrid(map);

    for (j = 0; j < map->height; j++) {
        i = 0;
        while (fread(&charBuffer, sizeof(char), 1, stdin) == 1 && charBuffer != '\n') {
            map->grid[i][j] = charBuffer;
            i++;
        }
    }
}

void readMapFromFile(MapStructure* map, char *filename) {

    int i, j;
    char charBuffer;
    FILE *filepointer;

    filepointer = fopen(filename, "r");

    if(filepointer == NULL) {
        fprintf(stderr, "Can't open the specified file.\n");
        return;
    }

    fscanf(filepointer, "%d %d %d", &map->width, &map->height, &map->fuelAvailable);
    while (fread(&charBuffer, sizeof(char), 1, filepointer) == 1 && charBuffer != '\n') {}

    allocateMapGrid(map);

    for (j = 0; j < map->height; j++) {
        i = 0;
        while (fread(&charBuffer, sizeof(char), 1, filepointer) == 1 && charBuffer != '\n') {
            map->grid[i][j] = charBuffer;
            i++;
        }
    }
    fclose(filepointer);
}

void saveMapAsFile(MapStructure map, char* filename) {

    int i, j;
    FILE* mapFile;

    mapFile = fopen(filename, "w");
    if (mapFile == NULL) {
        fprintf(stderr, "Cannot open file %s\n", filename);
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

    allocateMapGrid(&copiedMap);

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


int isInGrid(MapStructure map, Vector2D position) {

    if (position.x >= map.width || position.x < 0) {
        return 0;
    }

    if ( position.y >= map.height || position.y < 0) {
        return 0;
    }

    return 1;
}


char readMapTile(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return '.';
    }

    return map.grid[position.x][position.y];
}


void writeMapTile(MapStructure* map, Vector2D position, char characterToWrite) {

    if (!isInGrid(*map, position)) {
        return;
    }

    map->grid[position.x][position.y] = characterToWrite;
}


void regenMapTile(MapStructure originalMap, MapStructure* editedMap, Vector2D position) {

    if (!isInGrid(originalMap, position)) {
        return;
    }

    editedMap->grid[position.x][position.y] = originalMap.grid[position.x][position.y];
}


int isArrival(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return 0;
    }

    if (map.grid[position.x][position.y] == '=') {
        return 1;
    }

    return 0;
}

int isSand(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return 0;
    }

    if (map.grid[position.x][position.y] == '~') {
        return 1;
    }

    return 0;
}


int isDrivable(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return 0;
    }

    if (map.grid[position.x][position.y] == '#') {
        return 1;
    }

    if (map.grid[position.x][position.y] == '~') {
        return 1;
    }

    if (map.grid[position.x][position.y] == '=') {
        return 1;
    }

    return 0;
}


void line(int x1, int y1, int x2, int y2, infoLine *data) {
    data->posStart.x = x1;
    data->posStart.y = y1;
    data->posActual.x = x1 + 0.5;
    data->posActual.y = y1 + 0.5;
    data->posEnd.x = x2;
    data->posEnd.y = y2;
    int adxi, adyi, dxi, dyi;
    adxi = dxi = x2 - x1;
    adyi = dyi = y2 - y1;
    if (adxi < 0)
        adxi = -dxi;
    if (adyi < 0)
        adyi = -dyi;
    data->pos = 0;
    data->len = adxi;
    if (adyi > adxi)
        data->len = adyi;
    data->delta.x = ((float)dxi) / data->len;
    data->delta.y = ((float)dyi) / data->len;
}


int nextPoint(infoLine *data, pos2Dint *point, int sens) {
    if (sens > 0) {
        if (data->pos == data->len) {
            point->x = data->posEnd.x;
            point->y = data->posEnd.y;
            return -1; // La fin de la ligne est atteinte
        }
        data->posActual.x += data->delta.x;
        data->posActual.y += data->delta.y;
        point->x = ((int)data->posActual.x);
        point->y = ((int)data->posActual.y);
        data->pos++;
        return 1; // un nouveau point est déterminé.
    }
    if (sens < 0) {
        if (data->pos == 0) {
            point->x = data->posStart.x;
            point->y = data->posStart.y;
            return -1; // La fin de la ligne est atteinte
        }
        data->posActual.x -= data->delta.x;
        data->posActual.y -= data->delta.y;
        point->x = ((int)data->posActual.x);
        point->y = ((int)data->posActual.y);
        data->pos--;

        return 1; // un nouveau point est déterminé.
    }
    return 1; // Avec sens==0, il n'y a pas de déplacement
}


int isCrossable(MapStructure map, Vector2D departure, Vector2D arrival) {

    infoLine lineStructure;
    pos2Dint computedPosition;
    Vector2D positionBuffer;

    if (!isDrivable(map, departure) || !isDrivable(map, arrival)) {
        return 0;
    }

    line(departure.x, departure.y, arrival.x, arrival.y, &lineStructure);

    while (nextPoint(&lineStructure, &computedPosition, 1) == 1) {
      positionBuffer.x = computedPosition.x;
      positionBuffer.y = computedPosition.y;

      if (!isDrivable(map, positionBuffer)) {
          return 0;
      }
    }

    return 1;
}

int isValidVelocity(Vector2D velocity) {

    if (velocity.x >= 2) {
        return 0;
    }
    if (velocity.x <= -2) {
        return 0;
    }
    if (velocity.y >= 2) {
        return 0;
    }
    if (velocity.y <= -2) {
        return 0;
    }

    return 1;
}

int validSandSpeed(Vector2D velocity) {

    if(velocity.x == 0){
        if(velocity.y >= -1 && velocity.y <= 1){
            return 1;
        }
        else {
            return 0;
        }
    }
    if(velocity.y == 0){
        if(velocity.x >= -1 && velocity.x <= 1){
            return 1;
        }
        else {
            return 0;
        }
    }
}
