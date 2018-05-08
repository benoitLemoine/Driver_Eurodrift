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
 * @file mapEditing.h
 */


#ifndef DRIVER_EURODRIFT_MAPEDITING_H
#define DRIVER_EURODRIFT_MAPEDITING_H

#include "../include/mathObjects.h"

typedef struct {

    int fuelAvailable;
    int boostCount;
    Vector2D position;
    Vector2D speed;
}Car;

typedef struct {

    int fuelAvailable;
    int height, width;
    char** grid;

}MapStructure;


void allocateMapGrid (MapStructure* map);

void readMapFromStdin(MapStructure* map);

void saveMapAsFile(MapStructure map, char* filename);

MapStructure copyMap(MapStructure map);

void freeMap(MapStructure* map);


int isInGrid (MapStructure map, Vector2D position);

char readMapTile(MapStructure map, Vector2D position);

void writeMapTile(MapStructure* map, Vector2D position, char characterToWrite);


void regenMapTile(MapStructure originalMap, MapStructure* editedMap, Vector2D position);

int isArrival(MapStructure map, Vector2D position);

int isDrivable(MapStructure map, Vector2D position);

int isCrossable(MapStructure map, Vector2D departure, Vector2D arrival);


void readMapFromFile(MapStructure* map, char *filename);

#endif
