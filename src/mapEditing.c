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

/**
 * Allocates memory for a map using its dimensions.
 * @param map The 2-dimensional character array representation of the track.
 */
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

/**
 * Reads the map sent by the race manager, using stdin.
 * @param map The map structure where to store what's read.
 */
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

/**
 * Reads the map from a given file. USED FOR DEBUGGING.
 * @param map The map structure where to store what's read.
 * @param filename The name of the file where is stored the map.
 */
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

/**
 * Saves the map structure in a given file.
 * @param map The 2-dimensional character array representation of the track.
 * @param filename The name of the file where to stored the map structure.
 */
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

/**
 * Returns a map structure which is an exact copy of the given map structure.
 * @param map The map structure to copy.
 * @return The map structure which is the copy.
 */
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

/**
 * Releases the memory used by a given map structure.
 * @param map The 2-dimensional character array representation of the track.
 */
void freeMap(MapStructure* map) {

    int i;

    for (i = 0; i < map->width; i++) {
        free(map->grid[i]);
    }
    free(map->grid);
}

/**
 * Checks if the given position is in the map.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position to checks.
 * @return Results of the test.
 */
int isInGrid(MapStructure map, Vector2D position) {

    if (position.x >= map.width || position.x < 0) {
        return 0;
    }

    if ( position.y >= map.height || position.y < 0) {
        return 0;
    }

    return 1;
}

/**
 * Returns the tile's type of the given position.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position where to read the tile.
 * @return The type of the given tile.
 */
char readMapTile(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return '.';
    }

    return map.grid[position.x][position.y];
}

/**
 * Modifies the tile's type at the given position.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position where to edit the tile.
 * @param characterToWrite The new type to write.
 */
void writeMapTile(MapStructure* map, Vector2D position, char characterToWrite) {

    if (!isInGrid(*map, position)) {
        return;
    }

    map->grid[position.x][position.y] = characterToWrite;
}

/**
 * Modifies the tile's type at the given position, using the tile's type on another map at the same position.
 * @param map The map used as reference.
 * @param editedMap The map where to modify the tile's type.
 * @param position The position where to edit the tile.
 */
void regenMapTile(MapStructure originalMap, MapStructure* editedMap, Vector2D position) {

    if (!isInGrid(originalMap, position)) {
        return;
    }

    editedMap->grid[position.x][position.y] = originalMap.grid[position.x][position.y];
}

/**
 * Checks if the given tile is an arrival tile.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position of the tile to check.
 * @return The result of the test.
 */
int isArrival(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return 0;
    }

    if (map.grid[position.x][position.y] == '=') {
        return 1;
    }

    return 0;
}

/**
 * Checks if the given tile is a sand tile.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position of the tile to check.
 * @return The result of the test.
 */
int isSand(MapStructure map, Vector2D position) {

    if (!isInGrid(map, position)) {
        return 0;
    }

    if (map.grid[position.x][position.y] == '~') {
        return 1;
    }

    return 0;
}

/**
 * Checks if the given tile is a drivable tile.
 * @param map The 2-dimensional character array representation of the track.
 * @param position The position of the tile to check.
 * @return The result of the test.
 */
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

/**
 * Builds a line structure with both points (x1,y1) and (x2,y2).
 * @param x1 X coordinate of the first point.
 * @param y1 Y coordinate of the first point.
 * @param x2 X coordinate of the second point.
 * @param y2 Y coordinate of the second point.
 * @param data The line structure to build.
 */
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

/**
 * Goes to next point from a given line in a given way accordingly to current position.
 * @param data The structure of the line.
 * @param point Position of the next point.
 * @param sens The way in which to look for the next point.
 */
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

/**
 * Checks if the line between departure and arrival is crossable.
 * @param map The 2-dimensional character array representation of the track.
 * @param departure The position of the departure.
 * @param arrival The position of the arrival.
 * @return The result of the test.
 */
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

/**
 * Checks if the given vector is a legal velocity vector.
 * @param velocity The velocity vector to check.
 * @return The result of the test.
 */
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

/**
 * Checks if the given vector is a legal speed vector in a sand tile.
 * @param velocity The speed vector to check.
 * @return The result of the test.
 */
int validSandSpeed(Vector2D speed) {

    if(speed.x == 0){
        if(speed.y >= -1 && speed.y <= 1){
            return 1;
        }
        else {
            return 0;
        }
    }
    if(speed.y == 0){
        if(speed.x >= -1 && speed.x <= 1){
            return 1;
        }
        else {
            return 0;
        }
    }
}
