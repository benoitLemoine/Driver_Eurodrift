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
 * @author Leplanquois Maxime <maxime.leplanquois@ecole.ensicaen.fr>
 * @version 0.0.1 / 29-04-2018
 */


/**
 * @file graphEditing.h
 */

#ifndef DRIVER_EURODRIFT_GRAPHEDITING_H
#define DRIVER_EURODRIFT_GRAPHEDITING_H

#include "mapEditing.h"

typedef struct {

    Vector2D position;
    int cost;
    char type;

}MapTile;

typedef struct {

    MapTile **nodes;
    int height, width;

}MapGraph;

typedef struct {
    MapTile **pathTile;
    int totalCost;
}Path;

typedef struct {
    Path **pathlist;
}PathList;

MapGraph *allocateMapGraph(MapStructure *map);

void displayGraph(MapGraph *graph);

void freeGraph(MapGraph *graph);

#endif //DRIVER_EURODRIFT_GRAPHEDITING_H
