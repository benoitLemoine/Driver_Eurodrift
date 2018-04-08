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

typedef struct {

    int fuelAvailable;
    int height, width;
    char** grid;

}MapStructure;


void readMapFromStdin(MapStructure* map);

void saveMapAsFile(MapStructure map, char* filename);

MapStructure copyMap(MapStructure map);

void freeMap(MapStructure* map);


int isInGrid (MapStructure map, int x, int y);

char readMapTile(MapStructure map, int x, int y);

void writeMapTile(MapStructure* map, int x, int y, char characterToWrite);


void regenMapTile(MapStructure originalMap, MapStructure* editedMap, int x, int y);

int isArrival(MapStructure map, int x, int y);

#endif
