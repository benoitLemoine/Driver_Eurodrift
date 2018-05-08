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

#include "mathObjects.h"
#include "tileQueue.h"
#include "mapEditing.h"

typedef struct {

    Vector2D position;
    int cost;
    int visited;

}MapTile;

typedef struct {

    MapTile **nodes;
    int height, width;
    int arrivalTileNumber;
    Vector2D *arrivalTiles;

}MapGraph;

MapGraph *allocateMapGraph(MapStructure *map);

void displayGraphCost(MapGraph *graph);

void freeGraph(MapGraph *graph);


int computeCost(Vector2D velocity, Vector2D speed, int inSand);

void computeOneByOneGraph(MapStructure *map, MapGraph *graph, Car car);


int isVisited(MapGraph *graph, Vector2D position);

int getTileCost(MapGraph *graph, Vector2D position);


TileQueue *buildBestPath(MapStructure *map, MapGraph *graph, Vector2D playerPosition);

void correctPath(MapGraph *graph, TileQueue *path);


void resetVisited(MapGraph *graph);

void resetCost(MapStructure *map, MapGraph *graph);

int isInGraph(Vector2D testedVector, MapGraph *graph);

#endif //DRIVER_EURODRIFT_GRAPHEDITING_H
