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

    MapStructure map;
    int fuel;
    int boostCount = 5;
    int lap = 0;
    int cpt;
    int ddx = 1, ddy = 0;
    int dx = 0, dy = 0;
    char action[100];

    char positions[100];
    MapGraph *graph;
    Vector2D ourPosition;
    int computedGraph = 0;
    TileQueue *path;
    Tile t;

    char posX[10], posY[10];

    /*Reading of the map*/

    readMapFromStdin(&map);
    saveMapAsFile(map, "mapSave.txt");
    graph = allocateMapGraph(&map);

    /*Reading positions at each turn*/

    while (!feof(stdin)) {
        lap++;
        cpt = 0;
        while (fread(&c, sizeof(char), 1, stdin) == 1 && c != '\n') {
            positions[cpt] = c;
            cpt++;
            fprintf(info, "%c", c);
            fflush(info);
        }
        positions[cpt] = '\0';

        sscanf(positions, "%s %s", posX, posY);
        ourPosition.x = atoi(posX);
        ourPosition.y = atoi(posY);

        if(!computedGraph) {
            computedGraph = 1;
            dijkstraAlgorithm(map, graph, ourPosition);
            path = buildBestPath(graph, ourPosition);
        }

        dequeueTileQueue(path, &t);

        sprintf(action, "%d %d", t.speedX - dx, t.speedY - dy);

        fprintf(info, " %d %d", t.speedX, t.speedY);
        fprintf(info, " %d %d", dx, dy);
        fprintf(info, "%c", '\n');

        dx = t.speedX;
        dy = t.speedY;
        fprintf(stdout, "%s\n", action);
        fflush(stdout);
        fflush(info);
    }

    freeMap(&map);
    freeGraph(graph);

    return EXIT_SUCCESS;
}