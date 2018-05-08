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

void resetCost(MapGraph *graph) {

    int i, j;

    for (i = 0; i < graph->width; i++) {
        for (j = 0; j < graph->height; j++) {
            if (graph->nodes[i][j].type == '.') {
                graph->nodes[i][j].cost = -1;
            } else {
                graph->nodes[i][j].cost = INT_MAX;
            }
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
 * @param map The 2-dimensional character array representation of the track.
 * @param graph The graph representation of the track.
 * @param playerPosition The starting position of the player.
 */
void computeOneByOneGraph(MapStructure map, MapGraph *graph, Car car) {

    int i, j;
    Vector2D velocityForNext, currentSpeed;
    int computedCost;

    int inSand;
    int crossable;
    int impossibleVelocity;

    TileQueue *neighbors = initTileQueue();
    Tile t;
    MapTile *current;
    Vector2D testedNeighbor;

    velocityForNext.x = 0;
    velocityForNext.y = 0;
    current = &(graph->nodes[car.position.x][car.position.y]);
    current->cost = 0;

    t.cost = 0;
    t.speed = car.speed;
    t.position = car.position;

    enqueueByCostTileQueue(neighbors, t);

    while (!isEmptyTileQueue(neighbors) && !isArrival(map, t.position)) {

        dequeueTileQueue(neighbors, &t);

        current = &(graph->nodes[t.position.x][t.position.y]);
        current->visited = 1;

        currentSpeed = t.speed;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                inSand = 0;
                impossibleVelocity = 0;

                testedNeighbor.x = current->position.x + i;
                testedNeighbor.y = current->position.y + j;

                crossable = isCrossable(map, current->position, testedNeighbor);

                if (crossable && !isVisited(graph, testedNeighbor)) {

                    if (current->type == '~') {

                        inSand = 1;
                    }

                    if (!inSand || (i == 0 || j == 0)) {

                        //Velocity required to reach the new tile
                        velocityForNext.x = i - currentSpeed.x;
                        velocityForNext.y = j - currentSpeed.y;

                        if (!isValideVelocity(velocityForNext)) {
                            impossibleVelocity = 1;
                        }

                        computedCost = current->cost + computeCost(velocityForNext, currentSpeed, inSand);

                        if (computedCost < getTileCost(graph, testedNeighbor) && !impossibleVelocity) {

                            graph->nodes[testedNeighbor.x][testedNeighbor.y].cost = computedCost;

                            t.position = testedNeighbor;
                            t.cost = getTileCost(graph, testedNeighbor);
                            t.speed.x = velocityForNext.x + currentSpeed.x;
                            t.speed.y = velocityForNext.y + currentSpeed.y;
                            enqueueByCostTileQueue(neighbors, t);
                            removeDuplicate(neighbors, t.position);
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

    /*Looking for the cheapest arrival tile*/
    minCost = INT_MAX;
    for (i = 0; i < graph->arrivalTileNumber; i++) {
        if (minCost >= getTileCost(graph, graph->arrivalTiles[i])) {
            minCost = getTileCost(graph, graph->arrivalTiles[i]);
            current = graph->arrivalTiles[i];
        }
    }

    /*Computing cheapest path to this arrival tile using Dijkstra algorithm*/
    while (!isEqualVector2D(current, playerPosition)) {
        t.position = current;
        t.cost = getTileCost(graph, current);

        minCost = INT_MAX;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                testedNeighbor.x = t.position.x + i;
                testedNeighbor.y = t.position.y + j;

                inSandArrival = 0;
                if (isInGraph(testedNeighbor, graph)) {

                    if (graph->nodes[testedNeighbor.x][testedNeighbor.y].type == '~') {
                        inSandArrival = 1;
                    }

                    if (minCost >= getTileCost(graph, testedNeighbor) && getTileCost(graph,testedNeighbor) >= 0) {
                        if (!inSandArrival || (i == 0 || j == 0)) {
                            minCost = getTileCost(graph, testedNeighbor);
                            current = testedNeighbor;
                        }
                    }
                }
            }
        }
        enqueueTileQueue(path, t);
    }
    t.position = current;
    t.cost = getTileCost(graph, current);
    enqueueTileQueue(path, t);

    /*Compute speeds to get from a tile to the next one*/
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
    Vector2D velocity, nextVelocity;
    Vector2D newPos;

    cur = path->head;

    while (cur != path->tail) {

        velocity.x = cur->value.speed.x - cur->next->value.speed.x;
        velocity.y = cur->value.speed.y - cur->next->value.speed.y;

        if (velocity.x >= 2 || velocity.x <= -2 || velocity.y >= 2 || velocity.y <= -2) {
            if (cur->next->next != NULL) {
                nextVelocity.x = cur->value.speed.x - cur->next->next->value.speed.x;
                nextVelocity.y = cur->value.speed.y - cur->next->next->value.speed.y;

                if (isValideVelocity(nextVelocity)) {
                    newPos.x = (int) ((cur->value.position.x - cur->next->next->value.position.x) / 2);
                    newPos.y = (int) ((cur->value.position.y - cur->next->next->value.position.y) / 2);

                    newPos.x = cur->value.position.x - newPos.x;
                    newPos.y = cur->value.position.y - newPos.y;

                    //Checker le type de case (joueur, sable...)
                    if (graph->nodes[newPos.x][newPos.y].cost != -1) {

                        cur->next->value.cost = graph->nodes[newPos.x][newPos.y].cost;
                        cur->next->value.position.x = newPos.x;
                        cur->next->value.position.y = newPos.y;

                        cur->value.speed.x = newPos.x - cur->value.position.x;
                        cur->value.speed.y = newPos.y - cur->value.position.y;

                        cur->next->value.speed.x = cur->next->next->value.position.x - newPos.x;
                        cur->next->value.speed.y = cur->next->next->value.position.y - newPos.y;

                    }
                }
            }
        }

        cur = cur->next;
    }

}

int computeCost(Vector2D velocity, Vector2D speed, int inSand) {

    int fuelConsumed = velocity.x * velocity.x + velocity.y * velocity.y;
    fuelConsumed += (int) (sqrt(speed.x * speed.x + speed.y * speed.y) * 3.0 / 2.0);
    if (inSand) {
        fuelConsumed += 1;
    }
    return fuelConsumed;
}

int isInGraph(Vector2D testedVector, MapGraph *graph) {

    if (testedVector.x >= graph->width) {
        return 0;
    }
    if (testedVector.x < 0) {
        return 0;
    }
    if (testedVector.y >= graph->height) {
        return 0;
    }
    if (testedVector.y < 0) {
        return 0;
    }

    return 1;
}
