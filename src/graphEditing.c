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
                if (readMapTile(*map, coordinates) == '.') {
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

void resetCost(MapStructure *map, MapGraph *graph) {

    int i, j;
    Vector2D coordinates;

    for (i = 0; i < graph->width; i++) {
        for (j = 0; j < graph->height; j++) {

            coordinates.x = i;
            coordinates.y = j;

            if (readMapTile(*map, coordinates) == '.') {
                graph->nodes[i][j].cost = -1;
            } else {
                graph->nodes[i][j].cost = INT_MAX;
            }
        }
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

void displayGraphVisited(MapGraph *graph) {
    int i, j;

    for (j = 0; j < graph->height; j++) {
        for (i = 0; i < graph->width; i++) {
            printf("%d ", graph->nodes[i][j].visited);
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
void computeOneByOneGraph(MapStructure *map, MapGraph *graph, Car car) {

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

    while (!isEmptyTileQueue(neighbors) && !isArrival(*map, t.position)) {

        dequeueTileQueue(neighbors, &t);

        current = &(graph->nodes[t.position.x][t.position.y]);
        current->visited = 1;

        currentSpeed = t.speed;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                impossibleVelocity = 0;

                testedNeighbor.x = current->position.x + i;
                testedNeighbor.y = current->position.y + j;

                crossable = isCrossable(*map, current->position, testedNeighbor);

                if (crossable && !isVisited(graph, testedNeighbor)) {

                    inSand = isSand(*map, current->position);

                    if (!inSand || (i == 0 || j == 0)) {

                        //Velocity required to reach the new tile
                        velocityForNext.x = i - currentSpeed.x;
                        velocityForNext.y = j - currentSpeed.y;

                        if (!isValideVelocity(velocityForNext)) {
                            //    impossibleVelocity = 1;
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
TileQueue *buildBestPath(MapStructure *map, MapGraph *graph, Car car) {

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
        if (minCost >= getTileCost(graph, graph->arrivalTiles[i]) && isArrival(*map, graph->arrivalTiles[i])) {
            minCost = getTileCost(graph, graph->arrivalTiles[i]);
            current = graph->arrivalTiles[i];
        }
    }

    /*Computing cheapest path to this arrival tile using Dijkstra algorithm*/
    while (!isEqualVector2D(current, car.position)) {
        t.position = current;
        t.cost = getTileCost(graph, current);
        t.speed.x = 0;
        t.speed.y = 0;

        minCost = INT_MAX;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                testedNeighbor.x = t.position.x + i;
                testedNeighbor.y = t.position.y + j;

                inSandArrival = 0;
                if (isInGraph(testedNeighbor, graph)) {

                    if (readMapTile(*map, testedNeighbor) == '~') {
                        inSandArrival = 1;
                    }
                    if (minCost >= getTileCost(graph, testedNeighbor) && getTileCost(graph, testedNeighbor) >= 0) {
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
void removeUselessBoosts(MapStructure map, TileQueue *path, Car car) {

    //TODO Make it correct path for up to 3 positions further

    TileQueueNode *cur;
    TileQueueNode *cursorChange;
    Vector2D velocity, nextVelocity, diffPos;
    Vector2D intermediatePosition, previousPosition, intermediateVelocity, intermediateSpeed;

    int costDifference;

    cur = path->head;

    while (cur != path->tail) {

        //if a velocity difference of more than 1/-1 occurs between two Tiles, that means a boost is used.
        velocity.x = cur->value.speed.x - cur->next->value.speed.x;
        velocity.y = cur->value.speed.y - cur->next->value.speed.y;

        if (velocity.x >= 2 || velocity.x <= -2 || velocity.y >= 2 || velocity.y <= -2) {
            if (cur->next->next != NULL) {

                //if the velocity difference doesn't exceed 1/-1 when comparing
                // the current Tile with the Tile 2 step ahead, that means that the boost used was unecessary.
                nextVelocity.x = cur->value.speed.x - cur->next->next->value.speed.x;
                nextVelocity.y = cur->value.speed.y - cur->next->next->value.speed.y;

                if (isValideVelocity(nextVelocity)) {

                    //if a line exists between the two Tiles and it is crossable, the path is corrected to prevents the usage of a boost.
                    if (isCrossable(map, cur->value.position, cur->next->next->value.position)) {
                        cursorChange = cur;

                        diffPos.x = cur->next->next->value.position.x - cur->value.position.x;
                        diffPos.y = cur->next->next->value.position.y - cur->value.position.y;

                        costDifference = 0;

                        intermediatePosition.x = cursorChange->value.position.x;
                        intermediatePosition.y = cursorChange->value.position.y;
                        previousPosition.x = cursorChange->value.position.x;
                        previousPosition.y = cursorChange->value.position.y;
                        intermediateSpeed.x = cursorChange->value.speed.x;
                        intermediateSpeed.y = cursorChange->value.speed.y;

                        //while loop to compute the potential new cost that the boost suppression would generate
                        while (cursorChange != cur->next->next) {

                            if (cursorChange != cur) {
                                if (diffPos.x != 0) {
                                    if (diffPos.x > 0) {
                                        intermediatePosition.x = intermediatePosition.x + 1;
                                    } else {
                                        intermediatePosition.x = intermediatePosition.x - 1;
                                    }
                                } else {
                                    intermediatePosition.x = cur->value.position.x;
                                }
                                if (diffPos.y != 0) {
                                    if (diffPos.y > 0) {
                                        intermediatePosition.y = intermediatePosition.y + 1;
                                    } else {
                                        intermediatePosition.y = intermediatePosition.y - 1;
                                    }
                                } else {
                                    intermediatePosition.y = cur->value.position.y;
                                }
                            }

                            intermediateVelocity.x = intermediatePosition.x - previousPosition.x;
                            intermediateVelocity.y = intermediatePosition.y - previousPosition.y;

                            //compute the cost of the current Tile and add it to the cost we already have.
                            costDifference += computeCost(intermediateVelocity, intermediateSpeed,
                                                          isSand(map, intermediatePosition));

                            intermediateSpeed.x = intermediateVelocity.x;
                            intermediateSpeed.y = intermediateVelocity.y;
                            previousPosition.x = intermediatePosition.x;
                            previousPosition.y = intermediatePosition.y;

                            cursorChange = cursorChange->next;
                        }

                        intermediateVelocity.x = cursorChange->value.position.x - previousPosition.x;
                        intermediateVelocity.y = cursorChange->value.position.y - previousPosition.y;

                        costDifference += computeCost(intermediateVelocity, intermediateSpeed,
                                                      isSand(map, cursorChange->value.position));

                        //if the costDifference calculated is smaller than the old ones, we change the path
                        //if it isn't, but the fuel avaiable allow for skipping the boost, we change the path
                        if (costDifference <= (cursorChange->value.cost - cur->value.cost)
                            || (path->tail->value.cost + costDifference) <= car.fuelAvailable) {
                            cursorChange = cur;

                            diffPos.x = cur->next->next->value.position.x - cur->value.position.x;
                            diffPos.y = cur->next->next->value.position.y - cur->value.position.y;

                            while (cursorChange != cur->next->next) {

                                if (cursorChange != cur) {
                                    if (diffPos.x != 0) {
                                        if (diffPos.x > 0) {
                                            cursorChange->value.position.x = cursorChange->prev->value.position.x + 1;
                                        } else {
                                            cursorChange->value.position.x = cursorChange->prev->value.position.x - 1;
                                        }
                                    } else {
                                        cursorChange->value.position.x = cur->value.position.x;
                                    }
                                    if (diffPos.y != 0) {
                                        if (diffPos.y > 0) {
                                            cursorChange->value.position.y = cursorChange->prev->value.position.y + 1;
                                        } else {
                                            cursorChange->value.position.y = cursorChange->prev->value.position.y - 1;
                                        }
                                    } else {
                                        cursorChange->value.position.y = cur->value.position.y;
                                    }
                                }

                                cursorChange = cursorChange->next;
                            }

                            //update the speeds and costs after changing the path
                            updateSpeedTileQueue(path);
                            updateCostTileQueue(map, path);
                        }
                    }
                }
            }
        }
        cur = cur->next;
    }
}

void updateCostTileQueue(MapStructure map, TileQueue *queue) {

    TileQueueNode *cur;
    Vector2D speed;
    Vector2D velocityNext;

    if (isEmptyTileQueue(queue)) {
        return;
    } else if (queue->head == queue->tail) {
        return;
    } else {

        cur = queue->head;

        speed.x = 0;
        speed.y = 0;
        cur->value.cost = 0;

        cur = cur->next;

        while (cur != queue->tail) {
            velocityNext.x = cur->value.position.x - cur->prev->value.position.x - speed.x;
            velocityNext.y = cur->value.position.y - cur->prev->value.position.y - speed.y;
            cur->value.cost =
                    cur->prev->value.cost + computeCost(velocityNext, speed, isSand(map, cur->prev->value.position));
            speed.x += velocityNext.x;
            speed.y += velocityNext.y;
            cur = cur->next;
        }

        velocityNext.x = 0;
        velocityNext.y = 0;
        cur->value.cost =
                cur->prev->value.cost + computeCost(velocityNext, speed, isSand(map, cur->prev->value.position));
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


TileQueue *isLine(TileQueueNode *start) {

    TileQueue *line;
    TileQueueNode *current = start;
    Vector2D sensOfTheLine, buffer;
    int lengthOfTheLine = 1;

    if (start == NULL || start->next == NULL) {
        return 0;
    }

    line = initTileQueue();
    enqueueTileQueue(line, start->value);

    sensOfTheLine.x = current->next->value.position.x - current->value.position.x;
    sensOfTheLine.y = current->next->value.position.y - current->value.position.y;

    do {
        current = current->next;
        enqueueTileQueue(line, current->value);
        lengthOfTheLine++;

        if (current->next == NULL) {
            break;
        }

        buffer.x = current->next->value.position.x - current->value.position.x;
        buffer.y = current->next->value.position.y - current->value.position.y;

    } while (isEqualVector2D(sensOfTheLine, buffer));

    if (lengthOfTheLine < 5) {
        freeTileQueue(line);
        line = NULL;
    }
    return line;

}

void drawLineOnMap(TileQueue *path, MapStructure *map) {

    TileQueueNode *cursorDraw;
    TileQueueNode *current = path->head;
    TileQueue *line = NULL;

    int i;
    int length;

    while (current != NULL) {

        line = isLine(current);

        if (line != NULL) {
            cursorDraw = line->head;
            length = 0;

            do {
                length++;
                writeMapTile(map, cursorDraw->value.position, '*');
                cursorDraw = cursorDraw->next;

            } while (cursorDraw != line->tail);

            for (i = 0; i < length; i++) {
                current = current->next;
            }
        }
        else {
            current = current->next;
        }
    }
}

