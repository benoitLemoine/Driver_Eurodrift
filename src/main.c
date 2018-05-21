#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mapEditing.h"
#include "../include/graphEditing.h"
#include "../include/tileQueue.h"

int main() {

    /*DEBUG*/
    FILE *info = fopen("mainLog.txt", "w");

    //Check next tile
    //if next tile impossible, search new path
    //if no new path, stay on the current position

    MapStructure map, baseMap;
    Car car;
    MapGraph *graph;
    TileQueue *path, *path2, *pathSpeed, *pathDijkstraCurrent, *pathDijkstraNew;
    Tile t, currentTile;

    Vector2D tileToReach;

    char c;
    int lap = 0;
    int cpt, flagChanged;
    int switchAlgorithm = 0;

    char positions[100];
    Vector2D firstCompetitorPosition, secondCompetitorPosition;
    char posX[10], posY[10];
    char posXFirstCompetitor[10], posYFirstCompetitor[10];
    char posXSecondCompetitor[10], posYSecondCompetitor[10];

    char action[100];

    Vector2D tmp;

    /*Reading of the map*/

    readMapFromStdin(&map);
    baseMap = copyMap(map);
    graph = allocateMapGraph(&map);

    car.fuelAvailable = map.fuelAvailable;
    car.speed.x = 0;
    car.speed.y = 0;

    /*Reading positions at each turn*/

    while (!feof(stdin)) {
        lap++;
        cpt = 0;
        while (fread(&c, sizeof(char), 1, stdin) == 1 && c != '\n') {
            positions[cpt] = c;
            cpt++;
        }
        positions[cpt] = '\0';

        sscanf(positions, "%s %s %s %s %s %s\n", posX, posY, posXFirstCompetitor,
               posYFirstCompetitor, posXSecondCompetitor, posYSecondCompetitor);


        /*Convert position from string to int*/
        car.position.x = atoi(posX);
        car.position.y = atoi(posY);
        firstCompetitorPosition.x = atoi(posXFirstCompetitor);
        firstCompetitorPosition.y = atoi(posYFirstCompetitor);
        secondCompetitorPosition.x = atoi(posXSecondCompetitor);
        secondCompetitorPosition.y = atoi(posYSecondCompetitor);

        /*Put competitors on the map*/
        writeMapTile(&map, firstCompetitorPosition, '.');
        writeMapTile(&map, secondCompetitorPosition, '.');

        if (lap == 1) {

            writeMapTile(&map, car.position, '#');
            writeMapTile(&baseMap, car.position, '#');

            //compute Dijkstra path

            computeOneByOneGraph(&map, graph, car);
            pathDijkstraCurrent = buildBestPath(&map, graph, car);
            removeUselessBoosts(map, pathDijkstraCurrent, car);
            updateCostTileQueue(map, pathDijkstraCurrent);

            resetCost(&map, graph);
            resetVisited(graph);

            //compute fast path

            valueGraphDistancePath(&map, graph, pathDijkstraCurrent, car);

            pathSpeed = bestMove(&map, graph, car.position, car.speed, 5);
            updateSpeedTileQueue(pathSpeed);
            updateCostTileQueue(map, pathSpeed);

        } else {
            updateCostTileQueue(map, pathDijkstraCurrent);
            freeTileQueue(pathSpeed);

            resetCost(&map, graph);
            resetVisited(graph);

            //compute Dijkstra path

            computeOneByOneGraph(&map, graph, car);
            pathDijkstraNew = buildBestPath(&map, graph, car);
            removeUselessBoosts(map, pathDijkstraNew, car);
            updateCostTileQueue(map, pathDijkstraNew);

            //change Dijkstra if better one found

            //if (pathDijkstraCurrent->tail->value.cost >= pathDijkstraNew->tail->value.cost ||
            //  !isArrival(map, pathDijkstraCurrent->tail->value.position)) {
            freeTileQueue(pathDijkstraCurrent);
            pathDijkstraCurrent = pathDijkstraNew;
            //} else {
            //   freeTileQueue(pathDijkstraNew);
            //}

            resetCost(&map, graph);
            resetVisited(graph);

            //compute fast path

            valueGraphDistancePath(&map, graph, pathDijkstraCurrent, car);

            pathSpeed = bestMove(&map, graph, car.position, car.speed, 5);
            updateSpeedTileQueue(pathSpeed);
            updateCostTileQueue(map, pathSpeed);
        }

        //Algorithm selection

        //TODO Find the correct if to change algorithm
        if (car.fuelAvailable <= (map.fuelAvailable / 3)) {
            switchAlgorithm = 1;
        }

        //TODO Better heuristic for stopping to use the fast path
        if (switchAlgorithm) {

            if (!isValidVelocity(car.speed)) {

                if (isSand(map, car.position) && !validSandSpeed(car.speed)) {
                    sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                    car.speed.x = 0;
                    car.speed.y = 0;
                    car.fuelAvailable -= computeCost(t.speed, car.speed, isSand(map, car.position));
                } else {
                    dequeueTileQueue(pathSpeed, &t);

                    tileToReach.x = car.position.x + t.speed.x;
                    tileToReach.y = car.position.y + t.speed.y;

                    if (t.speed.x >= 2) {
                        tileToReach.x -= 1;
                        if (!isCrossable(map, car.position, tileToReach)) {
                            tileToReach.x += 1;
                        }
                    } else if (t.speed.x <= -2) {
                        tileToReach.x += 1;
                        if (!isCrossable(map, car.position, tileToReach)) {
                            tileToReach.x -= 1;
                        }
                    }
                    if (t.speed.y >= 2) {
                        tileToReach.y -= 1;
                        if (!isCrossable(map, car.position, tileToReach)) {
                            tileToReach.y += 1;
                        }
                    } else if (t.speed.y <= -2) {
                        tileToReach.y += 1;
                        if (!isCrossable(map, car.position, tileToReach)) {
                            tileToReach.y -= 1;
                        }
                    }

                    t.speed.x = tileToReach.x - car.position.x;
                    t.speed.y = tileToReach.y - car.position.y;

                    sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                    car.speed.x = t.speed.x;
                    car.speed.y = t.speed.y;
                    car.fuelAvailable -= t.cost;
                }

            } else {

                tileToReach.x = car.position.x + pathDijkstraCurrent->head->value.speed.x;
                tileToReach.y = car.position.y + pathDijkstraCurrent->head->value.speed.y;

                if (!isCrossable(map, car.position, tileToReach)) {

                    resetCost(&map, graph);
                    resetVisited(graph);

                    //compute Dijkstra path

                    computeOneByOneGraph(&map, graph, car);
                    pathDijkstraNew = buildBestPath(&map, graph, car);
                    removeUselessBoosts(map, pathDijkstraNew, car);
                    updateCostTileQueue(map, pathDijkstraNew);

                    if (pathDijkstraCurrent->tail->value.cost > pathDijkstraNew->tail->value.cost + 30) {
                        freeTileQueue(pathDijkstraCurrent);
                        pathDijkstraCurrent = pathDijkstraNew;
                        dequeueTileQueue(pathDijkstraCurrent, &t);
                    } else {
                        freeTileQueue(pathDijkstraNew);
                        t.speed.x = 0;
                        t.speed.y = 0;
                    }
                } else {
                    dequeueTileQueue(pathDijkstraCurrent, &t);
                }

                sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                car.speed.x = t.speed.x;
                car.speed.y = t.speed.y;
                car.fuelAvailable -= t.cost;
            }
        } else {
            if (isSand(map, car.position) && !validSandSpeed(car.speed)) {
                sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                car.fuelAvailable -= computeCost(t.speed, car.speed, isSand(map, car.position));
                car.speed.x = 0;
                car.speed.y = 0;
            } else {
                dequeueTileQueue(pathSpeed, &t);
                flagChanged = 0;
                if (!isCrossable(map, car.position, t.position)) {
                    for (int i = -1; i < 0; i++) {
                        for (int j = -1; j < 0; j++) {
                            tileToReach.x = t.position.x + i;
                            tileToReach.y = t.position.y + j;
                            if (isCrossable(map, car.position, tileToReach)) {
                                flagChanged = 1;
                                t.position = tileToReach;
                                t.speed.x = t.position.x - car.position.x;
                                t.speed.y = t.position.y - car.position.y;
                            }
                        }
                    }
                    car.fuelAvailable -= computeCost(t.speed, car.speed, isSand(map, car.position));
                    sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                    if (!flagChanged) {
                        car.speed.x = 0;
                        car.speed.y = 0;
                    }
                } else {
                    car.fuelAvailable -= computeCost(t.speed, car.speed, isSand(map, car.position));
                    sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);
                    car.speed.x = t.speed.x;
                    car.speed.y = t.speed.y;
                }
            }
        }
        /*Reset competitors positions on the map*/
        regenMapTile(baseMap, &map, firstCompetitorPosition);
        regenMapTile(baseMap, &map, secondCompetitorPosition);

        fprintf(stdout, "%s\n", action);
        fflush(stdout);
        fflush(info);
    }

    freeMap(&map);
    freeMap(&baseMap);
    freeGraph(graph);
    freeTileQueue(pathDijkstraCurrent);
    freeTileQueue(pathSpeed);

    return EXIT_SUCCESS;
}