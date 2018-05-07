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


/**
 * Initialize a MapGraph given a MapStructure.
 * @param map The map to initialize the graph from.
 * @return A pointer to the newly allocated graph.
 */
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
                if (isArrival(*map, coordinates)) {
                    arrivalTileNumber++;
                    graph->arrivalTiles = realloc(graph->arrivalTiles, sizeof(Vector2D) * arrivalTileNumber);
                    graph->arrivalTiles[arrivalTileNumber - 1] = coordinates;
                }
            }
        }

        graph->arrivalTileNumber = arrivalTileNumber;

    }
    return graph;
}

void resetVisited(MapGraph *graph) {

    int i, j;

    for (i = 0; i < graph->width; i++) {
        for (j = 0; j < graph->height; j++) {
            graph->nodes[i][j].visited = 0;
        }
    }
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

int isVisited(MapGraph *graph, Vector2D position) {
    return graph->nodes[position.x][position.y].visited;
}

int getTileCost(MapGraph *graph, Vector2D position) {
    return graph->nodes[position.x][position.y].cost;
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

/**
 * Generate a valued graph given a track and a starting position.
 * @param map The 2-dimensional character array representation if the track.
 * @param graph The graph representation if the track.
 * @param playerPosition The starting posiyion of the player.
 */
void dijkstraAlgorithm(MapStructure map, MapGraph *graph, Vector2D playerPosition) {

    int i, j;
    int velocityForNextX, velocityForNextY, currentSpeedX, currentSpeedY;
    int inSand;
    int crossable;
    int computedCost;

    int impossibleVelocity;

    TileQueue *neighbors = initTileQueue();
    Tile t;

    MapTile *current = &(graph->nodes[playerPosition.x][playerPosition.y]);
    Vector2D testedNeighbor;

    currentSpeedX = 0;
    currentSpeedY = 0;
    velocityForNextX = 0;
    velocityForNextY = 0;
    current->cost = 0;
    current->visited = 0;

    t.cost = current->cost;
    t.speedX = currentSpeedX;
    t.speedY = currentSpeedY;
    t.position = playerPosition;

    enqueueByCostTileQueue(neighbors, t);

    while (!isEmptyTileQueue(neighbors) && !isArrival(map, t.position)) {

        dequeueTileQueue(neighbors, &t);

        current = &(graph->nodes[t.position.x][t.position.y]);
        current->visited = 1;

        currentSpeedX = t.speedX;
        currentSpeedY = t.speedY;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                inSand = 0;
                impossibleVelocity = 0;

                testedNeighbor.x = current->position.x + i;
                testedNeighbor.y = current->position.y + j;

                if (current->position.x == playerPosition.x && current->position.y == playerPosition.y) {
                    crossable = 1;
                } else {
                    crossable = isCrossable(map, current->position, testedNeighbor);
                }

                if (isInGrid(map, testedNeighbor)) {
                    if (crossable && isDrivable(map, testedNeighbor) && !isVisited(graph, testedNeighbor)) {

                        if (current->type == '~') {

                            inSand = 1;

                        }

                        if (!inSand || (inSand && (i == 0 || j == 0))) {

                            //Velocity required to reach the new tile
                            velocityForNextX = i - currentSpeedX;
                            velocityForNextY = j - currentSpeedY;

                            if (velocityForNextX >= 2 || velocityForNextX <= -2) {
                                impossibleVelocity = 1;
                            }

                            if (velocityForNextY >= 2 || velocityForNextY <= -2) {
                                impossibleVelocity = 1;
                            }

                            computedCost = current->cost + computeCost(velocityForNextX, velocityForNextY, currentSpeedX, currentSpeedY, inSand);

                            if (computedCost < getTileCost(graph, testedNeighbor) && !impossibleVelocity) {

                                graph->nodes[testedNeighbor.x][testedNeighbor.y].cost = computedCost;

                                t.position = testedNeighbor;
                                t.cost = getTileCost(graph, testedNeighbor);
                                t.speedX = velocityForNextX + currentSpeedX;
                                t.speedY = velocityForNextY + currentSpeedY;
                                enqueueByCostTileQueue(neighbors, t);
                                removeDuplicate(neighbors, t.position);
                            }
                        }
                    }
                }
            }
        }
    }
    freeTileQueue(neighbors);
}

/**
 * Build the path with the lowest cost using a valued graph.
 * @param graph The graph representation of the track.
 * @param playerPosition The starting position of the player car.
 * @return The path to follow to reach the end.
 */
TileQueue *buildBestPath(MapGraph *graph, Vector2D playerPosition) {

    //TODO Deux queues : queue périodique et queue sûre
    //Arrêt quand case ajoutée = playerPosition
    //Testez les cases en changeant les visited à 0


    int i, j;
    int minCost;
    int inSandArrival;

    Vector2D current;
    Vector2D testedNeighbor;

    TileQueue *path = initTileQueue();
    Tile t;

    minCost = INT_MAX;
    for (i = 0; i < graph->arrivalTileNumber; i++) {
        if (minCost >= getTileCost(graph, graph->arrivalTiles[i])) {
            minCost = getTileCost(graph, graph->arrivalTiles[i]);
            current = graph->arrivalTiles[i];
        }
    }

    while (current.x != playerPosition.x || current.y != playerPosition.y) {
        t.position = current;
        t.cost = getTileCost(graph, current);

        minCost = INT_MAX;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                testedNeighbor.x = t.position.x + i;
                testedNeighbor.y = t.position.y + j;

                inSandArrival = 0;
                if (testedNeighbor.x < graph->width && testedNeighbor.x >= 0 && testedNeighbor.y < graph->height &&
                    testedNeighbor.y >= 0) {
                    if (graph->nodes[testedNeighbor.x][testedNeighbor.y].type == '~') {
                        inSandArrival = 1;
                    }

                    if (minCost >= getTileCost(graph, testedNeighbor) && getTileCost(graph,testedNeighbor) >= 0) {
                        if (!inSandArrival || (inSandArrival && (i == 0 || j == 0))) {
                            minCost = getTileCost(graph, testedNeighbor);
                            current = testedNeighbor;
                        }
                    }
                }
            }
        }
        t.speedX = 0;
        t.speedY = 0;
        enqueueTileQueue(path, t);
    }
    t.position = current;
    t.cost = getTileCost(graph, current);
    t.speedX = 0;
    t.speedY = 0;
    enqueueTileQueue(path, t);

    updateSpeedTileQueue(path);

    return path;
}

/**
 * Remove useless boosts used when the car goes in diagonal in a straight line or an open turn.
 * @param graph Graph representation of the track.
 * @param path The path to correct.
 */
void correctPath(MapGraph *graph, TileQueue *path) {

    TileQueueNode *cur;
    int velocityX, velocityY;
    int nextVelocityX, nextVelocityY;
    int newPosX, newPosY;

    cur = path->head;

    while (cur != path->tail) {

        velocityX = cur->value.speedX - cur->next->value.speedX;
        velocityY = cur->value.speedY - cur->next->value.speedY;

        if (velocityX >= 2 || velocityX <= -2 || velocityY >= 2 || velocityY <= -2) {
            if (cur->next->next != NULL) {
                nextVelocityX = cur->value.speedX - cur->next->next->value.speedX;
                nextVelocityY = cur->value.speedY - cur->next->next->value.speedY;

                if ((nextVelocityX >= -1 && nextVelocityX <= 1) && (nextVelocityY >= -1 && nextVelocityY <= 1)) {
                    newPosX = (int) ((cur->value.position.x - cur->next->next->value.position.x) / 2);
                    newPosY = (int) ((cur->value.position.y - cur->next->next->value.position.y) / 2);

                    newPosX = cur->value.position.x - newPosX;
                    newPosY = cur->value.position.y - newPosY;

                    //Checker le type de case (joueur, sable...)
                    if (graph->nodes[newPosX][newPosY].cost != -1) {

                        cur->next->value.cost = graph->nodes[newPosX][newPosY].cost;
                        cur->next->value.position.x = newPosX;
                        cur->next->value.position.y = newPosY;

                        cur->value.speedX = newPosX - cur->value.position.x;
                        cur->value.speedY = newPosY - cur->value.position.y;

                        cur->next->value.speedX = cur->next->next->value.position.x - newPosX;
                        cur->next->value.speedY = cur->next->next->value.position.y - newPosY;

                    }
                }
            }
        }

        cur = cur->next;
    }

}

int computeCost(int ddx, int ddy, int dx, int dy, int inSand) {

    int fuelConsumed = ddx * ddx + ddy * ddy;
    fuelConsumed += (int) (sqrt(dx * dx + dy * dy) * 3.0 / 2.0);
    if (inSand) {
        fuelConsumed += 1;
    }
    return fuelConsumed;
}
