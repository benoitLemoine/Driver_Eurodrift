#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mapEditing.h"
#include "../include/graphEditing.h"
#include "../include/tileQueue.h"

int main() {

    /*DEBUG*/
    FILE* info = fopen("mainLog.txt", "w");

    //Check next tile
    //if next tile impossible, search new path
    //if no new path, stay on the current position

    MapStructure map, baseMap;
    Car car;
    MapGraph *graph;
    TileQueue *path, *path2, *tmpPath;
    Tile t;

    Vector2D tileToReach;

    char c;
    int lap = 0;
    int cpt;

    char positions[100];
    Vector2D firstCompetitorPosition, secondCompetitorPosition;
    char posX[10], posY[10];
    char posXFirstCompetitor[10], posYFirstCompetitor[10];
    char posXSecondCompetitor[10], posYSecondCompetitor[10];

    char action[100];

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

            computeOneByOneGraph(&map, graph, car);
            path = buildBestPath(&map, graph, car);
            removeUselessBoosts(map, path, car);

            //shortenPath(path, &map, car);
            //drawPathOnMap(path, &map);
            //saveMapAsFile(map, "mapMain.txt");

            dequeueTileQueue(path, &t);
        } else {

//            dequeueTileQueue(path, &t);

            tileToReach.x = car.position.x + path->head->value.speed.x;
            tileToReach.y = car.position.y + path->head->value.speed.y;

            //TODO correct path by +1/-1 when unreachable next tile
            if(!isCrossable(map, car.position, tileToReach)) {
                resetCost(&map, graph);
                resetVisited(graph);
                //freeTileQueue(path);

                computeOneByOneGraph(&map, graph, car);
                path2 = buildBestPath(&map, graph, car);
                removeUselessBoosts(map, path2, car);

                if (path2->tail->value.cost < path->tail->value.cost) {
                    free(path);
                    path = path2;
                    dequeueTileQueue(path, &t);
                } else {
                    free(path2);
                    t.speed.x = 0;
                    t.speed.y = 0;
                }
            }
            else {
                    dequeueTileQueue(path, &t);
                }
        }

        sprintf(action, "%d %d", t.speed.x - car.speed.x, t.speed.y - car.speed.y);

        /*Reset competitors positions on the map*/
        regenMapTile(baseMap, &map, firstCompetitorPosition);
        regenMapTile(baseMap, &map, secondCompetitorPosition);

        car.speed.x = t.speed.x;
        car.speed.y = t.speed.y;

        fprintf(stdout, "%s\n", action);
        fflush(stdout);
        fflush(info);
    }

    freeMap(&map);
    freeMap(&baseMap);
    freeGraph(graph);
    freeTileQueue(path);

    return EXIT_SUCCESS;
}