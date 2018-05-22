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
    TileQueue *pathSpeed, *pathDijkstraCurrent, *pathDijkstraNew;
    Tile t;

    char c;
    int lap = 0;
    int cpt, flagChanged;
    int switchAlgorithm = 0;
    int i, j;

    char positions[100];
    Vector2D firstCompetitorPosition, secondCompetitorPosition;
    char posX[10], posY[10];
    char posXFirstCompetitor[10], posYFirstCompetitor[10];
    char posXSecondCompetitor[10], posYSecondCompetitor[10];

    char action[100];

    Vector2D invertedSpeed, nextSpeed, tileToReach, nextPos;
    Vector2D velocityTile;

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
            updateCostTileQueue(map, pathDijkstraCurrent, car);

            resetCost(&map, graph);
            resetVisited(graph);

            //compute fast path

            valueGraphDistancePath(&map, graph, car);

            pathSpeed = bestMove(&map, graph, car.position, car.speed, 5);
            updateSpeedTileQueue(pathSpeed);
            updateCostTileQueue(map, pathSpeed, car);

        } else {
            updateCostTileQueue(map, pathDijkstraCurrent, car);

            resetCost(&map, graph);
            resetVisited(graph);

            //compute Dijkstra path
            nextSpeed.x = car.speed.x;
            nextSpeed.y = car.speed.y;

            car.speed.x = 0;
            car.speed.y = 0;

            //change Dijkstra if better one found

            if (computeOneByOneGraph(&map, graph, car)) {
                pathDijkstraNew = buildBestPath(&map, graph, car);
                if (pathDijkstraNew->tail->value.cost != -1) {
                    removeUselessBoosts(map, pathDijkstraNew, car);
                    updateCostTileQueue(map, pathDijkstraNew, car);

                    if (pathDijkstraCurrent->tail->value.cost >= pathDijkstraNew->tail->value.cost
                        || !isArrival(map, pathDijkstraCurrent->tail->value.position) || !switchAlgorithm) {
                        freeTileQueue(pathDijkstraCurrent);
                        pathDijkstraCurrent = pathDijkstraNew;
                    } else {
                        freeTileQueue(pathDijkstraNew);
                    }
                }
                else {
                    freeTileQueue(pathDijkstraNew);
                }
            }

            car.speed.x = nextSpeed.x;
            car.speed.y = nextSpeed.y;

            if(!switchAlgorithm) {

                resetCost(&map, graph);
                resetVisited(graph);

                //compute fast path

                if (valueGraphDistancePath(&map, graph, car)) {
                    freeTileQueue(pathSpeed);
                    pathSpeed = bestMove(&map, graph, car.position, car.speed, 5);
                    updateSpeedTileQueue(pathSpeed);
                    updateCostTileQueue(map, pathSpeed, car);

                    //Algorithm selection
                    tileToReach.x = pathSpeed->head->value.speed.x - car.speed.x;
                    tileToReach.y = pathSpeed->head->value.speed.y - car.speed.y;

                    nextSpeed.x = tileToReach.x + car.speed.x;
                    nextSpeed.y = tileToReach.y + car.speed.y;

                    nextPos.x = car.position.x + nextSpeed.x;
                    nextPos.y = car.position.y + nextSpeed.y;

                    invertedSpeed.x = 0 - nextSpeed.x;
                    invertedSpeed.y = 0 - nextSpeed.y;

                    if (pathDijkstraCurrent->tail->value.cost >
                        (car.fuelAvailable - computeCost(tileToReach, car.speed, isSand(map, car.position))
                         - computeCost(invertedSpeed, nextSpeed, isSand(map, nextPos)) - 10)) {
                        switchAlgorithm = 1;
                    }
                }
            }
        }

        if (switchAlgorithm) {

            if (!isValidVelocity(car.speed)) {

                t.speed.x = 0 - car.speed.x;
                t.speed.y = 0 - car.speed.y;
                car.fuelAvailable -= computeCost(t.speed, car.speed, isSand(map, car.position));
                sprintf(action, "%d %d", t.speed.x, t.speed.y);
                car.speed.x = 0;
                car.speed.y = 0;

            } else {

                tileToReach.x = car.position.x + pathDijkstraCurrent->head->value.speed.x;
                tileToReach.y = car.position.y + pathDijkstraCurrent->head->value.speed.y;

                if (!isCrossable(map, car.position, tileToReach)) {

                    /*resetCost(&map, graph);
                    resetVisited(graph);

                    //compute Dijkstra path
                    if (computeOneByOneGraph(&map, graph, car)) {
                        pathDijkstraNew = buildBestPath(&map, graph, car);
                        if (pathDijkstraNew->tail->value.cost != -1) {
                            removeUselessBoosts(map, pathDijkstraNew, car);
                            updateCostTileQueue(map, pathDijkstraNew, car);

                            if (pathDijkstraCurrent->tail->value.cost >= pathDijkstraNew->tail->value.cost
                                || !isArrival(map, pathDijkstraCurrent->tail->value.position)) {
                                freeTileQueue(pathDijkstraCurrent);
                                pathDijkstraCurrent = pathDijkstraNew;
                                dequeueTileQueue(pathDijkstraCurrent, &t);
                            } else {
                                freeTileQueue(pathDijkstraNew);
                            }
                        } else {
                            //TODO recompute dijkstra
                            freeTileQueue(pathDijkstraNew);
                            enqueueTileQueue(pathDijkstraCurrent, t);
                            t.speed.x = 0;
                            t.speed.y = 0;
                        }
                    } else {
                    */    //TODO recompute dijkstra
                        t.speed.x = 0;
                        t.speed.y = 0;
                    //}
                } else {
                    dequeueTileQueue(pathDijkstraCurrent, &t);
                }

                velocityTile.x = t.speed.x - car.speed.x;
                velocityTile.y = t.speed.y - car.speed.y;

                car.fuelAvailable -= computeCost(velocityTile, car.speed, isSand(map, car.position));
                sprintf(action, "%d %d", velocityTile.x, velocityTile.y);
                car.speed.x += velocityTile.x;
                car.speed.y += velocityTile.y;
            }
        } else {
            if (isSand(map, car.position) && !validSandSpeed(car.speed)) {
                velocityTile.x = 0 - car.speed.x;
                velocityTile.y = 0 - car.speed.y;
                car.fuelAvailable -= computeCost(velocityTile, car.speed, isSand(map, car.position));
                sprintf(action, "%d %d", velocityTile.x, velocityTile.y);
                car.speed.x = 0;
                car.speed.y = 0;
            } else {
                dequeueTileQueue(pathSpeed, &t);
                flagChanged = 0;
                if (!isCrossable(map, car.position, t.position)) {
                    for (i = -1; i < 1; i++) {
                        for (j = -1; j < 1; j++) {
                            tileToReach.x = t.position.x + i;
                            tileToReach.y = t.position.y + j;
                            nextSpeed.x = t.speed.x + i;
                            nextSpeed.y = t.speed.y + j;
                            if (isCrossable(map, car.position, tileToReach)) {
                                if ((nextSpeed.x < 6 && nextSpeed.x > -6 && nextSpeed.y < 6 && nextSpeed.y > -6) ||
                                    isSand(map, car.position)) {
                                    if (((nextSpeed.x * nextSpeed.x) + (nextSpeed.y * nextSpeed.y)) <= 25) {
                                        if (!isSand(map, car.position) || (i == 0 || j == 0)) {
                                            flagChanged = 1;
                                            t.position = tileToReach;
                                            t.speed.x = nextSpeed.x;
                                            t.speed.y = nextSpeed.y;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    velocityTile.x = t.speed.x - car.speed.x;
                    velocityTile.y = t.speed.y - car.speed.y;
                    car.fuelAvailable -= computeCost(velocityTile, car.speed, isSand(map, car.position));
                    sprintf(action, "%d %d", velocityTile.x, velocityTile.y);
                    if (!flagChanged) {
                        car.speed.x = 0;
                        car.speed.y = 0;
                    } else {
                        car.speed.x += velocityTile.x;
                        car.speed.y += velocityTile.y;
                    }
                } else {
                    velocityTile.x = t.speed.x - car.speed.x;
                    velocityTile.y = t.speed.y - car.speed.y;
                    car.fuelAvailable -= computeCost(velocityTile, car.speed, isSand(map, car.position));
                    sprintf(action, "%d %d", velocityTile.x, velocityTile.y);
                    car.speed.x += velocityTile.x;
                    car.speed.y += velocityTile.y;
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