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
 * @file graphEditing.c
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "../include/graphEditing.h"
#include "../include/mapEditing.h"
#include "../include/tileQueue.h"

MapGraph *allocateMapGraph(MapStructure *map) {

    int i, j;
    int arrivalTileNumber;

    MapGraph *graph;
    Vector2D coordinates;

    if (map->height <= 0 || map->width <= 0) {
        graph = NULL;
    } else {
        graph = malloc(sizeof(MapGraph));

        graph->height = map->height;
        graph->width = map->width;

        graph->nodes = calloc((size_t) map->width, sizeof(MapTile *));
        arrivalTileNumber = 0;
        graph->arrivalTiles = malloc(sizeof(Vector2D) * arrivalTileNumber);

        for (i = 0; i < map->width; i++) {
            graph->nodes[i] = calloc((size_t) map->height, sizeof(MapTile));
            for (j = 0; j < map->height; j++) {
                coordinates.x = i;
                coordinates.y = j;
                graph->nodes[i][j].position = coordinates;
                graph->nodes[i][j].visited = 0;
                graph->nodes[i][j].type = readMapTile(*(map), coordinates);
                if (graph->nodes[i][j].type == '.') {
                    graph->nodes[i][j].cost = -1;
                } else {
                    graph->nodes[i][j].cost = INT_MAX;
                }
                if(isArrival(*map, coordinates)) {
                    arrivalTileNumber++;
                    graph->arrivalTiles = realloc(graph->arrivalTiles, sizeof(Vector2D) * arrivalTileNumber);
                    graph->arrivalTiles[arrivalTileNumber-1] = coordinates;
                }
            }
        }

        graph->arrivalTileNumber = arrivalTileNumber;

    }
    return graph;
}

void displayGraph(MapGraph *graph) {
    int i, j;

    for (j = 0; j < graph->height; j++) {
        for (i = 0; i < graph->width; i++) {
            printf("%c ", graph->nodes[i][j].type);
        }
        printf("\n");
    }
}

void displayGraphCost(MapGraph *graph) {
    int i, j;

    for (j = 0; j < graph->height; j++) {
        for (i = 0; i < graph->width; i++) {
            printf("%3d ", graph->nodes[i][j].cost);
        }
        printf("\n");
    }
}

void freeGraph(MapGraph *graph) {

    int i;

    if (graph != NULL) {
        for (i = 0; i < graph->width; i++) {
            free(graph->nodes[i]);
        }
        free(graph->arrivalTiles);
        free(graph->nodes);
        free(graph);
    }
}

void dijkstraAlgorithm(MapStructure map, MapGraph *graph, Vector2D playerPosition) {

    //TODO Refaire quand le meilleur chemin est le sable -> norme de vitesse <= 1

    int i, j;
    int dvx, dvy, vx, vy;
    int inSand;
    int crossable;
    int computedCost;

    TileQueue *neighbors = initTileQueue();
    Tile t;

    MapTile current = graph->nodes[playerPosition.x][playerPosition.y];
    Vector2D testedNeighbor;

    vx = 0;
    vy = 0;
    dvx = 0;
    dvy = 0;
    current.cost = 0;
    graph->nodes[playerPosition.x][playerPosition.y].cost = 0;
    current.visited = 1;

    t.cost = current.cost;
    t.speedX = vx;
    t.speedY = vy;
    t.position = playerPosition;

    enqueueTileQueue(neighbors, t);

    while (!isEmptyTileQueue(neighbors) && !isArrival(map, t.position)) {

        dequeueTileQueue(neighbors, &t);
        current = graph->nodes[t.position.x][t.position.y];
        current.visited = 1;
        vx = t.speedX;
        vy = t.speedY;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                inSand = 0;
                testedNeighbor.x = current.position.x + i;
                testedNeighbor.y = current.position.y + j;

                if(current.position.x == playerPosition.x && current.position.y == playerPosition.y) {
                    crossable = 1;
                }
                else {
                    crossable = isCrossable(map, current.position, testedNeighbor);
                }

                if (isInGrid(map, testedNeighbor) && crossable && isDrivable(map, testedNeighbor) &&
                    graph->nodes[testedNeighbor.x][testedNeighbor.y].visited == 0) {

                    graph->nodes[testedNeighbor.x][testedNeighbor.y].visited = 1;
                    dvx = i - vx;
                    dvy = j - vy;

                    if (current.type == '~') {

                        inSand = 1;

                    }

                    computedCost = current.cost + computeCost(dvx, dvy, vx, vy, inSand);
                    if(computedCost < graph->nodes[testedNeighbor.x][testedNeighbor.y].cost) {
                        graph->nodes[testedNeighbor.x][testedNeighbor.y].cost = computedCost;
                    }

                    t.position = testedNeighbor;
                    t.cost = graph->nodes[testedNeighbor.x][testedNeighbor.y].cost;
                    t.speedX = dvx;
                    t.speedY = dvy;
                    enqueueTileQueue(neighbors, t);
                }
            }
        }
    }
    freeTileQueue(neighbors);
}

TileQueue *buildBestPath(MapGraph *graph, Vector2D playerPosition) {

    int i, j;
    int minCost;
    Vector2D finish;
    Vector2D testedNeighbor;

    TileQueue *path = initTileQueue();
    Tile t;

    minCost = INT_MAX;
    for(i = 0; i < graph->arrivalTileNumber; i++) {
        if(minCost >= graph->nodes[graph->arrivalTiles[i].x][graph->arrivalTiles[i].y].cost) {
            minCost = graph->nodes[graph->arrivalTiles[i].x][graph->arrivalTiles[i].y].cost;
            finish = graph->arrivalTiles[i];
        }
    }

    while(finish.x != playerPosition.x || finish.y != playerPosition.y) {
        t.position = finish;
        t.cost = graph->nodes[finish.x][finish.y].cost;

        minCost = INT_MAX;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                testedNeighbor.x = t.position.x + i;
                testedNeighbor.y = t.position.y + j;

                if (minCost >= graph->nodes[testedNeighbor.x][testedNeighbor.y].cost
                    && graph->nodes[testedNeighbor.x][testedNeighbor.y].cost >= 0) {
                    minCost = graph->nodes[testedNeighbor.x][testedNeighbor.y].cost;
                    finish = testedNeighbor;
                }
            }
        }
        t.speedX = 0;
        t.speedY = 0;
        enqueueTileQueue(path, t);
    }
    t.position = finish;
    t.cost = graph->nodes[finish.x][finish.y].cost;
    t.speedX = 0;
    t.speedY = 0;
    enqueueTileQueue(path, t);

    updateSpeedTileQueue(path);

    return path;
}

int computeCost(int ddx, int ddy, int dx, int dy, int inSand) {

    int fuelConsumed = ddx * ddx + ddy * ddy;
    fuelConsumed += (int) (sqrt(dx * dx + dy * dy) * 3.0 / 2.0);
    if (inSand) {
        fuelConsumed += 1;
    }
    return fuelConsumed;
}
