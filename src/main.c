#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mapEditing.h"
#include "../include/graphEditing.h"
#include "../include/tileQueue.h"


int fuelConsumedThisTurn(int ddx, int ddy, int dx, int dy, int inSand) {

    int fuelConsumed = ddx * ddx + ddy * ddy;
    fuelConsumed += (int)(sqrt(dx * dx + dy * dy) * 3.0 / 2.0);

    if (inSand) {
        fuelConsumed += 1;
    }
    return -fuelConsumed;
}

int main() {

    char c;
    int i;
    FILE* info = fopen("raceLog.txt", "w");

    MapStructure map, baseMap;
    int fuel;
    int boostCount = 5;
    int lap = 0;
    int cpt;
    int ddx = 1, ddy = 0;
    int dx = 0, dy = 0;
    char action[100];


    MapGraph *graph;
    TileQueue *path;
    Tile t;

    char positions[100];
    Vector2D ourPosition, firstCompetitorPosition, secondCompetitorPosition;
    char posX[10], posY[10];
    char posXFirstCompetitor[10], posYFirstCompetitor[10];
    char posXSecondCompetitor[10], posYSecondCompetitor[10];

    /*Reading of the map*/

    readMapFromStdin(&map);
    baseMap = copyMap(map);
    graph = allocateMapGraph(&map);

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

        ourPosition.x = atoi(posX);
        ourPosition.y = atoi(posY);
        firstCompetitorPosition.x = atoi(posXFirstCompetitor);
        firstCompetitorPosition.y = atoi(posYFirstCompetitor);
        secondCompetitorPosition.x = atoi(posXSecondCompetitor);
        secondCompetitorPosition.y = atoi(posYSecondCompetitor);

        fprintf(info, "%d %d %d %d %d %d\n", ourPosition.x, ourPosition.y, firstCompetitorPosition.x,
                firstCompetitorPosition.y, secondCompetitorPosition.x, secondCompetitorPosition.y);


//        writeMapTile(&map, firstCompetitorPosition, '.');
//        writeMapTile(&map, secondCompetitorPosition, '.');
//        graph->nodes[firstCompetitorPosition.x][firstCompetitorPosition.y].type = '.';
//        graph->nodes[secondCompetitorPosition.x][secondCompetitorPosition.y].type = '.';


        dijkstraAlgorithm(map, graph, ourPosition);
        path = buildBestPath(graph, ourPosition);
        correctPath(graph, path);


//        regenMapTile(baseMap, &map, firstCompetitorPosition);
//        regenMapTile(baseMap, &map, secondCompetitorPosition);
//        graph->nodes[firstCompetitorPosition.x][firstCompetitorPosition.y].type = readMapTile(baseMap, firstCompetitorPosition);
//        graph->nodes[secondCompetitorPosition.x][secondCompetitorPosition.y].type = readMapTile(baseMap, secondCompetitorPosition);

        dequeueTileQueue(path, &t);

        sprintf(action, "%d %d", t.speedX - dx, t.speedY - dy);

//        fprintf(info, " %d %d", t.speedX, t.speedY);
//        fprintf(info, " %d %d", dx, dy);
//        fprintf(info, "%c", '\n');

        dx = t.speedX;
        dy = t.speedY;
        fprintf(stdout, "%s\n", action);
        fflush(stdout);
        fflush(info);
    }

    freeMap(&map);
    freeMap(&baseMap);
    freeGraph(graph);

    return EXIT_SUCCESS;
}