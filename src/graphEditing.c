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

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEBUG(x) fprintf(info, x); fflush(info)

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

void valueGraphDistancePath(MapStructure *map, MapGraph *graph, TileQueue *path, Car car) {

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
    current = &(graph->nodes[path->tail->value.position.x][path->tail->value.position.y]);
    current->cost = 0;

    t.cost = 0;
    t.speed.x = 0;
    t.speed.y = 0;
    t.position = path->tail->value.position;

    enqueueByCostTileQueue(neighbors, t);

    while (!isEmptyTileQueue(neighbors)) {
        dequeueTileQueue(neighbors, &t);

        current = &(graph->nodes[t.position.x][t.position.y]);
        current->visited = 1;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {

                testedNeighbor.x = current->position.x + i;
                testedNeighbor.y = current->position.y + j;

                crossable = isCrossable(*map, current->position, testedNeighbor);

                if (crossable && !isVisited(graph, testedNeighbor)) {

                    inSand = isSand(*map, current->position);

                    if (!inSand || (i == 0 || j == 0)) {

                        computedCost = t.cost + 1;

                        if (computedCost < graph->nodes[testedNeighbor.x][testedNeighbor.y].cost) {
                            graph->nodes[testedNeighbor.x][testedNeighbor.y].cost = computedCost;

                            t.cost += 1;
                            t.position = testedNeighbor;
                            enqueueByCostTileQueue(neighbors, t);
                            removeDuplicate(neighbors, testedNeighbor);
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

    //TODO When the arrival Tile and the car position are not joignable (others car blocking the path)
    //TODO Take car initial speed in account

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

TileQueue *bestMove(MapStructure *map, MapGraph *graph, Vector2D position, Vector2D speed, int cpt) {

    int i, j;
    int minCost;
    int inSand = 0;

    Vector2D nextTile;
    Vector2D testedNeighbor;
    Vector2D testedSpeed;

    Tile t;
    TileQueue *t2;
    TileQueue *correctPath;

    if (readMapTile(*map, position) == '~') {
        inSand = 1;
    }
    if (cpt == 0) {
        t.position = position;
        t.speed.x = speed.x;
        t.speed.y = speed.y;
        t.cost = INT_MAX;
        minCost = INT_MAX;
        correctPath = initTileQueue();
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (inSand) {
                    testedNeighbor.x = position.x + i;
                    testedNeighbor.y = position.y + j;
                } else {
                    testedNeighbor.x = position.x + speed.x + i;
                    testedNeighbor.y = position.y + speed.y + j;
                }
                if (((speed.x + i) < 6 && (speed.x + i) > -6 && (speed.y + j) < 6 && (speed.y + j) > -6) || inSand) {
                    if(((speed.x + i)*(speed.x + i) + (speed.y + j)*(speed.y + j)) <= 25){
                        if (isInGraph(testedNeighbor, graph) && isCrossable(*map, position, testedNeighbor)) {
                            if (minCost >= getTileCost(graph, testedNeighbor) &&
                                getTileCost(graph, testedNeighbor) >= 0) {
                                if (!inSand || (i == 0 || j == 0)) {
                                    minCost = getTileCost(graph, testedNeighbor);
                                    t.cost = minCost;
                                    t.speed.x = testedNeighbor.x - position.x;
                                    t.speed.y = testedNeighbor.y - position.y;
                                }
                            }
                        }
                    }
                }
            }
        }
        enqueueTileQueue(correctPath, t);
        return correctPath;
    } else {
        correctPath = initTileQueue();

        minCost = INT_MAX;
        t.speed.x = speed.x;
        t.speed.y = speed.y;
        t.cost = INT_MAX;
        t.position = position;

        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (inSand) {
                    testedNeighbor.x = position.x + i;
                    testedNeighbor.y = position.y + j;
                    testedSpeed.x = 0;
                    testedSpeed.y = 0;
                } else {
                    testedNeighbor.x = position.x + speed.x + i;
                    testedNeighbor.y = position.y + speed.y + j;
                    testedSpeed.x = testedNeighbor.x - position.x;
                    testedSpeed.y = testedNeighbor.y - position.y;
                }
                if (((speed.x + i) < 6 && (speed.x + i) > -6 && (speed.y + j) < 6 && (speed.y + j) > -6) || inSand) {
                    if(((speed.x + i)*(speed.x + i) + (speed.y + j)*(speed.y + j)) <= 25) {
                        if (isInGraph(testedNeighbor, graph) && isCrossable(*map, position, testedNeighbor)) {
                            if (!inSand || (i == 0 || j == 0)) {
                                t2 = bestMove(map, graph, testedNeighbor, testedSpeed, cpt - 1);
                                if (minCost >= t2->tail->value.cost) {
                                    freeTileQueue(correctPath);
                                    correctPath = copyTileQueue(t2);
                                    t.cost = getTileCost(graph, testedNeighbor);
                                    minCost = correctPath->tail->value.cost;
                                    t.speed.x = testedNeighbor.x - position.x;
                                    t.speed.y = testedNeighbor.y - position.y;
                                }
                                freeTileQueue(t2);
                            }
                        }
                    }
                }
            }
        }
        enqueueTileQueue(correctPath, t);
    }
/*Compute speeds to get from a tile to the next one*/
    updateSpeedTileQueue(correctPath);

    return correctPath;
}

/**
 * Remove useless boosts used when the car goes in diagonal in a straight line or an open turn.
 * @param graph Graph representation of the track.
 * @param path The path to correct.
 */
void removeUselessBoosts(MapStructure map, TileQueue *path, Car car) {

    TileQueueNode *cur, *curStart, *curEnd;
    TileQueueNode *cursorChange, *cursorChange2;
    Vector2D velocityToNext, nextVelocity, diffPos;
    Vector2D intermediatePosition, previousPosition, intermediateVelocity, intermediateSpeed;

    TileQueue *line;
    Tile intermediateTile;

    int costDifference;

    cur = path->head;

    while (cur != path->tail) {

        //if a velocity difference of more than 1/-1 occurs between two Tiles, that means a boost is used.
        velocityToNext.x = cur->value.speed.x - cur->next->value.speed.x;
        velocityToNext.y = cur->value.speed.y - cur->next->value.speed.y;

        if (!isValidVelocity(velocityToNext)) {
            if (cur->next->next != NULL) {
                curStart = cur->next;
                curEnd = cur->next;

                diffPos.x = curEnd->value.position.x - curStart->value.position.x;
                diffPos.y = curEnd->value.position.y - curStart->value.position.y;

                while (curStart->prev != NULL && curEnd->next != NULL && (diffPos.x == 0 || diffPos.y == 0)
                       && isCrossable(map, curStart->value.position, curEnd->value.position)) {
                    curStart = curStart->prev;
                    curEnd = curEnd->next;
                    diffPos.x = curEnd->value.position.x - curStart->value.position.x;
                    diffPos.y = curEnd->value.position.y - curStart->value.position.y;
                }

                curStart = curStart->next;
                curEnd = curEnd->prev;

                if (curStart != curEnd) {
                    cursorChange = curStart;

                    line = initTileQueue();
                    enqueueTileQueueAtTail(line, curStart->value);
                    cursorChange2 = line->head;

                    diffPos.x = curEnd->value.position.x - curStart->value.position.x;
                    diffPos.y = curEnd->value.position.y - curStart->value.position.y;

                    //while loop to compute the potential new cost that the boost suppression would generate
                    while (cursorChange != curEnd) {
                        if (diffPos.x != 0) {
                            if (diffPos.x > 0) {
                                intermediateTile.position.x = cursorChange2->value.position.x + 1;
                            } else {
                                intermediateTile.position.x = cursorChange2->value.position.x - 1;
                            }
                        } else {
                            intermediateTile.position.x = cursorChange2->value.position.x;
                        }
                        if (diffPos.y != 0) {
                            if (diffPos.y > 0) {
                                intermediateTile.position.y = cursorChange2->value.position.y + 1;
                            } else {
                                intermediateTile.position.y = cursorChange2->value.position.y - 1;
                            }
                        } else {
                            intermediateTile.position.y = cursorChange2->value.position.y;
                        }

                        intermediateTile.speed.x = intermediateTile.position.x - cursorChange2->value.position.x;
                        intermediateTile.speed.y = intermediateTile.position.y - cursorChange2->value.position.y;

                        //compute the cost of the current Tile and add it to the cost we already have.
                        intermediateTile.cost = 0;

                        enqueueTileQueueAtTail(line, intermediateTile);

                        cursorChange = cursorChange->next;
                        cursorChange2 = cursorChange2->next;
                    }

                    enqueueTileQueueAtTail(line, curEnd->value);

                    updateSpeedTileQueue(line);
                    updateCostTileQueue(map, line);

                    //if the costDifference calculated is smaller than the old ones, we change the path
                    //if it isn't, but the fuel avaiable allow for skipping the boost, we change the path
                    if (line->tail->value.cost <= curEnd->value.cost ||
                        (line->tail->value.cost - curEnd->value.cost + path->tail->value.cost) <= car.fuelAvailable) {
                        cursorChange = curStart;
                        cursorChange2 = line->head;

                        while (cursorChange2 != line->tail) {

                            cursorChange->value.position.x = cursorChange2->value.position.x;
                            cursorChange->value.position.y = cursorChange2->value.position.y;

                            cursorChange = cursorChange->next;
                            cursorChange2 = cursorChange2->next;
                        }

                        //update the speeds and costs after changing the path
                        updateSpeedTileQueue(path);
                        updateCostTileQueue(map, path);
                    }
                    freeTileQueue(line);
                }

            }
        }
        cur = cur->next;
    }
}

void shortenPath(TileQueue *path, MapStructure *map, Car car) {

    /*DEBUG*/
    FILE *info = fopen("shortenPathLog.txt", "w");
    int nbrLine = 0;
    MapStructure copiedMap;

    TileQueueNode *current = path->head;
    TileQueueNode *endOfLine;
    TileQueue *line = NULL;
    int i;
    int lengthOfLine;

    fprintf(info, "base cost %d\n", path->tail->value.cost);
    fflush(info);

    /*All along the path*/
    while (current != NULL) {
        drawPathOnMap(path, &copiedMap);
        saveMapAsFile(copiedMap, "pathIsShorting.txt");
        copiedMap = copyMap(*map);

        line = isLine(current);

        /*If current is the start of a line*/
        if (line != NULL) {
            fprintf(info, "line %d\n", ++nbrLine);
            fflush(info);

            lengthOfLine = lengthOfQueue(line);
            line = shortenLine(line, map, car, path->tail->value.cost);

            /*Move endOfLine at the position of the line's tail on actual path*/
            endOfLine = current;
            for (i = 1; i < lengthOfLine; i++) {
                endOfLine = endOfLine->next;
            }

            /*Linking*/
            if (current == path->head) {
                line->head->prev = NULL;
                path->head = line->head;
            } else {
                line->head->prev = current->prev;
                current->prev->next = line->head;
            }

            if (endOfLine == path->tail) {
                line->tail->next = NULL;
                path->tail = line->tail;
            } else {
                line->tail->next = endOfLine->next;
                endOfLine->next->prev = line->tail;
            }

            updateSpeedTileQueue(path);
            updateCostTileQueue(*map, path);

            fprintf(info, "new cost %d\n", path->tail->value.cost);
            fflush(info);

            /*Skip the rest of the line (dont want to check sub part of a line)*/
            current = line->tail;
        } else {
            current = current->next;
        }
    }
}


TileQueue *shortenLine(TileQueue *line, MapStructure *map, Car car, int baseCost) {

    //FIXME Bugs appear when computed cost is too high

    //TODO take remaining fuel in account
    //TODO take initial speed of the car in account
    //TODO take sand tile in account
    //TODO take arrival tile in account

    /*DEBUG*/
    FILE *info = fopen("shortenLineLog.txt", "w");

    int lengthOfTheLine = lengthOfQueue(line);
    int lengthOfSmallerPerfectLine;

    int initialSpeed = MAX(car.speed.x, car.speed.y);
    int maxSpeed;

    int missingLength;
    int i, speedLength;

    TileQueue *newline = NULL;
    TileQueueNode *current;

    int previousCost = line->head->value.cost - line->tail->value.cost;
    fprintf(info, "Inital speed: %d\n", initialSpeed);
    fprintf(info, "Base cost: %d\n", baseCost);
    fprintf(info, "Car fuels: %d\n", car.fuelAvailable);
    fprintf(info, "Previous cost: %d\n\n", previousCost);
    fflush(info);
    int newCostLine, newCostTotal;
    int numberOfTryToReduce = 0;

    /*Starting with a null speed is the same as with a speed of one*/
    if (initialSpeed == 0) {
        initialSpeed = 1;
    }

    /*Looks for quickest line which respects remaining fuel*/
    do {

        /*Computing parameters with the initial speed and the length of the line*/
        switch (initialSpeed) {
            case 1:
                /*Max speed*/
                if (lengthOfTheLine >= 26) {
                    maxSpeed = 5 - numberOfTryToReduce;
                } else if (lengthOfTheLine >= 17) {
                    maxSpeed = 4 - numberOfTryToReduce;
                } else if (lengthOfTheLine >= 10) {
                    maxSpeed = 3 - numberOfTryToReduce;
                } else if (lengthOfTheLine >= 5) {
                    maxSpeed = 2 - numberOfTryToReduce;
                }
                /*Length*/
                if (maxSpeed == 5) {
                    lengthOfSmallerPerfectLine = 26;
                } else if (maxSpeed == 4) {
                    lengthOfSmallerPerfectLine = 17;
                } else if (maxSpeed == 3) {
                    lengthOfSmallerPerfectLine = 10;
                } else if (maxSpeed == 2) {
                    lengthOfSmallerPerfectLine = 5;
                } else if (maxSpeed == 1) {
                    return line;
                }
                break;
            case 2:
                if (lengthOfTheLine >= 23) {
                    lengthOfSmallerPerfectLine = 23;
                    maxSpeed = 5;
                } else if (lengthOfTheLine >= 14) {
                    lengthOfSmallerPerfectLine = 14;
                    maxSpeed = 4;
                } else if (lengthOfTheLine >= 7) {
                    lengthOfSmallerPerfectLine = 7;
                    maxSpeed = 3;
                } else if (lengthOfTheLine >= 2) {
                    lengthOfSmallerPerfectLine = 2;
                    maxSpeed = 2;
                }
                break;
            case 3:
                if (lengthOfTheLine >= 20) {
                    lengthOfSmallerPerfectLine = 20;
                    maxSpeed = 5;
                } else if (lengthOfTheLine >= 11) {
                    lengthOfSmallerPerfectLine = 11;
                    maxSpeed = 4;
                } else if (lengthOfTheLine >= 4) {
                    lengthOfSmallerPerfectLine = 4;
                    maxSpeed = 3;
                }
                break;
            case 4:
                if (lengthOfTheLine >= 16) {
                    lengthOfSmallerPerfectLine = 16;
                    maxSpeed = 5;
                } else if (lengthOfTheLine >= 7) {
                    lengthOfSmallerPerfectLine = 7;
                    maxSpeed = 4;
                }
                break;
            case 5:
                if (lengthOfTheLine >= 11) {
                    lengthOfSmallerPerfectLine = 11;
                    maxSpeed = 5;
                }
        }
        missingLength = lengthOfTheLine - lengthOfSmallerPerfectLine;


        if (newline != NULL) {
            freeTileQueue(newline);
        }
        newline = initTileQueue();

        /*Here we use current->prev because in fact line->head is the end of the line, but nvm*/
        /*So using enqueueTileQueueAtTail returns it one more time*/

        /*First half of the line : accelerating*/
        current = line->tail;
        enqueueTileQueue(newline, line->tail->value);
        for (speedLength = initialSpeed; speedLength < maxSpeed; speedLength++) {
            for (i = 0; i < speedLength; i++) {
                current = current->prev;
            }
            enqueueTileQueueAtTail(newline, current->value);
        }

        /*Second half of the line : curbing*/
        for (speedLength = maxSpeed; speedLength > 0; speedLength--) {
            /*Jump at least once*/
            for (i = 0; i < speedLength; i++) {
                current = current->prev;
            }
            enqueueTileQueueAtTail(newline, current->value);

            /*Continue to jump as long as the gap is bigger than the speed*/
            while (missingLength >= speedLength) {
                for (i = 0; i < speedLength; i++) {
                    current = current->prev;
                }
                enqueueTileQueueAtTail(newline, current->value);
                missingLength -= speedLength;
            }
        }

        updateSpeedTileQueue(newline);
        updateCostTileQueue(*map, newline);

        newCostLine = newline->tail->value.cost - newline->head->value.cost;
        newCostTotal = baseCost + newCostLine - previousCost;

        fprintf(info, "Max speed: %d\n", maxSpeed);
        fprintf(info, "New cost of the line (%d): %d\n", numberOfTryToReduce, newCostLine);
        fprintf(info, "New cost in total (%d): %d\n", numberOfTryToReduce, newCostTotal);
        fflush(info);

        numberOfTryToReduce++;
        if (numberOfTryToReduce > 5) return line;

    } while (newCostTotal > car.fuelAvailable);

    freeTileQueue(line);
    return newline;
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
                    cur->prev->value.cost +
                    computeCost(velocityNext, speed, isSand(map, cur->prev->value.position));
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

    /*Saving direction of the line with first 2 points*/
    sensOfTheLine.x = current->next->value.position.x - current->value.position.x;
    sensOfTheLine.y = current->next->value.position.y - current->value.position.y;

    /*As long as it follows this direction*/
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

    /*Keep only lines with length superior or equal to 5*/
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
        } else {
            current = current->next;
        }
    }
}


void drawPathOnMap(TileQueue *path, MapStructure *map) {

    TileQueueNode *current = path->head;

    if (isEmptyTileQueue(path)) {
        return;
    }

    do {
        writeMapTile(map, current->value.position, '*');
        current = current->next;

    } while (current != NULL);
}

