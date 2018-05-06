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
 * @version 0.0.1 / 30-04-2018
 */


/**
 * @file testGraphEditing.c
 */

#include <stdlib.h>
#include <time.h>
#include "../include/minunit.h"
#include "../include/tileQueue.h"
#include "../include/graphEditing.h"
#include "../include/mapEditing.h"

MU_TEST(assertInitTileQueue) {
    TileQueue *queue;
    queue = initTileQueue();

    mu_check(queue != NULL);

    freeTileQueue(queue);
}

MU_TEST(assertIsEmptyTileQueue) {
    TileQueue *queue;
    queue = initTileQueue();

    mu_check(isEmptyTileQueue(queue) == 1);

    freeTileQueue(queue);
}

MU_TEST(assertIsNotEmptyTileQueue) {
    TileQueue *queue;
    Tile t;
    Vector2D v;

    v.x = 5;
    v.y = 6;

    queue = initTileQueue();
    t.position = v;
    t.speedX = 0;
    t.speedY = 1;
    t.cost = 1;

    enqueueTileQueue(queue, t);
    mu_check(isEmptyTileQueue(queue) == 0);

    freeTileQueue(queue);
}

MU_TEST(assertEnqueueEmptyTileQueue) {
    TileQueue *queue;
    Tile t;
    Vector2D v;

    queue = initTileQueue();

    v.x = 5;
    v.y = 6;

    t.position = v;
    t.speedX = 0;
    t.speedY = 1;
    t.cost = 1;

    enqueueTileQueue(queue, t);
    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speedX == 0);
    mu_check(queue->tail->value.speedY == 1);
    mu_check(queue->tail->value.cost == 1);

    freeTileQueue(queue);
}

MU_TEST(assertEnqueueNotEmptyTileQueue) {
    TileQueue *queue;
    Tile t;
    Vector2D v;

    queue = initTileQueue();

    v.x = 5;
    v.y = 6;

    t.position = v;
    t.speedX = 0;
    t.speedY = 1;
    t.cost = 5;

    enqueueTileQueue(queue, t);

    v.x = 7;
    v.y = 0;

    t.position = v;
    t.speedX = 2;
    t.speedY = 0;
    t.cost = 1;

    enqueueTileQueue(queue, t);

    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speedX == 0);
    mu_check(queue->tail->value.speedY == 1);
    mu_check(queue->tail->value.cost == 5);

    freeTileQueue(queue);
}

MU_TEST(assertDequeueEmptyTileQueue) {
    TileQueue *queue;
    Tile *t;

    int res;

    queue = initTileQueue();

    t = malloc(sizeof(Tile));

    res = dequeueTileQueue(queue, t);

    mu_check(res == 0);

    free(t);
    freeTileQueue(queue);
}

MU_TEST(assertDequeueNotEmptyTileQueue) {
    TileQueue *queue;
    Tile *t;
    Vector2D v;

    int res;

    queue = initTileQueue();

    t = malloc(sizeof(Tile));

    v.x = 5;
    v.y = 6;

    t->position = v;
    t->speedX = 0;
    t->speedY = 1;
    t->cost = 5;

    enqueueTileQueue(queue, *t);

    v.x = 7;
    v.y = 0;

    t->position = v;
    t->speedX = 2;
    t->speedY = 0;
    t->cost = 1;

    enqueueTileQueue(queue, *t);

    res = dequeueTileQueue(queue, t);

    mu_check(res == 1);
    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speedX == 0);
    mu_check(queue->tail->value.speedY == 1);
    mu_check(queue->tail->value.cost == 5);

    free(t);
    freeTileQueue(queue);
}

MU_TEST_SUITE(suiteTestTileQueue) {
    MU_RUN_TEST(assertInitTileQueue);
    MU_RUN_TEST(assertIsEmptyTileQueue);
    MU_RUN_TEST(assertIsNotEmptyTileQueue);
    MU_RUN_TEST(assertEnqueueEmptyTileQueue);
    MU_RUN_TEST(assertEnqueueNotEmptyTileQueue);
    MU_RUN_TEST(assertDequeueEmptyTileQueue);
    MU_RUN_TEST(assertDequeueNotEmptyTileQueue);
}

MU_TEST(assertAllocateMapGraph) {

    MapGraph *graph;
    MapStructure map;

    Vector2D v;

    srand(time(NULL));

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/starter_droit_au_but.txt");

    graph = allocateMapGraph(&map);
    mu_assert(graph != NULL, "Error, graph is NULL");

    v.x = rand()% graph->width;
    v.y = rand()% graph->height;

    printf("Tile[%d][%d]\n",v.x, v.y);
    mu_assert(graph->nodes[v.x][v.y].type == readMapTile(map, v), "Error, tiles differents.");

    v.x = rand()% graph->width;
    v.y = rand()% graph->height;

    printf("Tile[%d][%d]\n",v.x, v.y);
    mu_assert(graph->nodes[v.x][v.y].type == readMapTile(map, v), "Error, tiles differents.");

    v.x = rand()% graph->width;
    v.y = rand()% graph->height;

    printf("Tile[%d][%d]\n",v.x, v.y);
    mu_assert(graph->nodes[v.x][v.y].type == readMapTile(map, v), "Error, tiles differents.");

    displayGraph(graph);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST(testDijkstra) {

    MapGraph *graph;
    MapStructure map;

    Vector2D v;

    v.x = 43;
    v.y = 3;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/starter_virage_sable.txt");

    graph = allocateMapGraph(&map);

    dijkstraAlgorithm(map, graph, v);

    displayGraph(graph);
    displayGraphCost(graph);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST(testbestPath) {

    MapGraph *graph;
    MapStructure map;

    Vector2D v;
    TileQueue *path;

    v.x = 87;
    v.y = 0;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/f-Zero_Death_Wind.txt");

    graph = allocateMapGraph(&map);

    dijkstraAlgorithm(map, graph, v);
    path = buildBestPath(graph, v);
    correctPath(graph, path);

    displayGraphCost(graph);
    displayTileQueue(path);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST_SUITE(suiteTestMapGraph){
    MU_RUN_TEST(assertAllocateMapGraph);
    //MU_RUN_TEST(testDijkstra);
    MU_RUN_TEST(testbestPath);
}

int main(int argc, char *argv[]) {

    MU_RUN_SUITE(suiteTestTileQueue);
    MU_RUN_SUITE(suiteTestMapGraph);
    MU_REPORT();

    return 0;
}