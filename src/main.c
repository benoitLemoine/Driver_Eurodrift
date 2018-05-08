#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mapEditing.h"
#include "../include/graphEditing.h"
#include "../include/tileQueue.h"

int main() {

    MapStructure map, baseMap;
    Car car;
    MapGraph *graph;
    TileQueue *path;
    Tile t;

    char c;
    int lap = 0;
    int cpt;

    char positions[100];
    Vector2D firstCompetitorPosition, secondCompetitorPosition;
    char posX[10], posY[10];
    char posXFirstCompetitor[10], posYFirstCompetitor[10];
    char posXSecondCompetitor[10], posYSecondCompetitor[10];

    char action[100];

    FILE* info = fopen("raceLog.txt", "w");

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

            computeOneByOneGraph(map, graph, car);
            path = buildBestPath(graph, car.position);
            correctPath(graph, path);

            dequeueTileQueue(path, &t);
        }
        else {

            dequeueTileQueue(path, &t);

            if(!isCrossable(map, car.position, t.position)) {
                resetCost(graph);
                resetVisited(graph);

                computeOneByOneGraph(map, graph, car);
                path = buildBestPath(graph, car.position);
                correctPath(graph, path);

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

    return EXIT_SUCCESS;
}