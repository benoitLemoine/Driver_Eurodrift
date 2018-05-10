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
    t.speed.x = 0;
    t.speed.y = 1;
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
    t.speed.x = 0;
    t.speed.y = 1;
    t.cost = 1;

    enqueueTileQueue(queue, t);
    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speed.x == 0);
    mu_check(queue->tail->value.speed.y == 1);
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
    t.speed.x = 0;
    t.speed.y = 1;
    t.cost = 5;

    enqueueTileQueue(queue, t);

    v.x = 7;
    v.y = 0;

    t.position = v;
    t.speed.x = 2;
    t.speed.y = 0;
    t.cost = 1;

    enqueueTileQueue(queue, t);

    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speed.x == 0);
    mu_check(queue->tail->value.speed.y == 1);
    mu_check(queue->tail->value.cost == 5);

    freeTileQueue(queue);
}

MU_TEST(assertCopyTileQueue) {
    TileQueue *queue;
    TileQueue *copy;
    Tile t;
    Tile t2;
    Vector2D v;

    queue = initTileQueue();

    v.x = 5;
    v.y = 6;

    t.position = v;
    t.speed.x = 0;
    t.speed.y = 1;
    t.cost = 5;

    enqueueTileQueue(queue, t);

    v.x = 7;
    v.y = 0;

    t.position = v;
    t.speed.x = 2;
    t.speed.y = 0;
    t.cost = 1;

    enqueueTileQueue(queue, t);

    copy = copyTileQueue(queue);

    displayTileQueue(queue);
    displayTileQueue(copy);

    dequeueTileQueue(queue, &t);
    dequeueTileQueue(copy, &t2);

    mu_check(t.position.x == t2.position.x);
    mu_check(t.position.y == t2.position.y);
    mu_check(t.speed.x == t2.speed.x);
    mu_check(t.speed.y == t2.speed.y);
    mu_check(t.cost == t2.cost);

    dequeueTileQueue(queue, &t);
    dequeueTileQueue(copy, &t2);

    mu_check(t.position.x == t2.position.x);
    mu_check(t.position.y == t2.position.y);
    mu_check(t.speed.x == t2.speed.x);
    mu_check(t.speed.y == t2.speed.y);
    mu_check(t.cost == t2.cost);

    freeTileQueue(queue);
    freeTileQueue(copy);
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
    t->speed.x = 0;
    t->speed.y = 1;
    t->cost = 5;

    enqueueTileQueue(queue, *t);

    v.x = 7;
    v.y = 0;

    t->position = v;
    t->speed.x = 2;
    t->speed.y = 0;
    t->cost = 1;

    enqueueTileQueue(queue, *t);

    res = dequeueTileQueue(queue, t);

    mu_check(res == 1);
    mu_check(queue->tail->value.position.x == 5);
    mu_check(queue->tail->value.position.y == 6);
    mu_check(queue->tail->value.speed.x == 0);
    mu_check(queue->tail->value.speed.y == 1);
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
    MU_RUN_TEST(assertCopyTileQueue);
}

MU_TEST(assertAllocateMapGraph) {

    MapGraph *graph;
    MapStructure map;

    srand(time(NULL));

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/starter_droit_au_but.txt");

    graph = allocateMapGraph(&map);
    mu_assert(graph != NULL, "Error, graph is NULL");

    freeGraph(graph);
    freeMap(&map);

}

MU_TEST(testDijkstra) {

    Car car;
    MapGraph *graph;
    MapStructure map;

    car.position.x = 43;
    car.position.y = 3;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/starter_virage_sable.txt");

    graph = allocateMapGraph(&map);

    computeOneByOneGraph(&map, graph, car);

    displayGraphCost(graph);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST(testbestPath) {

    Car car;
    MapGraph *graph;
    MapStructure map;

    TileQueue *path;

    car.position.x = 86;
    car.position.y = 0;
    car.speed.x = 0;
    car.speed.y = 0;
    car.boostCount = 5;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/f-Zero_Death_Wind.txt");

    writeMapTile(&map, car.position, '#');

    graph = allocateMapGraph(&map);
    car.fuelAvailable = map.fuelAvailable;

    computeOneByOneGraph(&map, graph, car);
    displayGraphCost(graph);

    path = buildBestPath(&map, graph, car);
    updateSpeedTileQueue(path);
    updateCostTileQueue(map, path);

    removeUselessBoosts(map, path, car);
    displayTileQueue(path);

    freeTileQueue(path);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST(assertResetVisited) {

    Car car;
    MapGraph *graph;
    MapStructure map;

    car.position.x = 86;
    car.position.y = 0;
    car.speed.x = 0;
    car.speed.y = 0;
    car.boostCount = 5;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/f-Zero_Death_Wind.txt");

    writeMapTile(&map, car.position, '#');

    graph = allocateMapGraph(&map);
    car.fuelAvailable = map.fuelAvailable;

    computeOneByOneGraph(&map, graph, car);
    displayGraphVisited(graph);

    resetVisited(graph);
    displayGraphVisited(graph);

    freeGraph(graph);
    freeMap(&map);
}

MU_TEST(assertResetCost) {

    Car car;
    MapGraph *graph;
    MapStructure map;

    car.position.x = 86;
    car.position.y = 0;
    car.speed.x = 0;
    car.speed.y = 0;
    car.boostCount = 5;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/f-Zero_Death_Wind.txt");

    writeMapTile(&map, car.position, '#');

    graph = allocateMapGraph(&map);
    car.fuelAvailable = map.fuelAvailable;

    computeOneByOneGraph(&map, graph, car);
    displayGraphCost(graph);

    resetCost(&map, graph);
    displayGraphCost(graph);

    freeGraph(graph);
    freeMap(&map);
}

MU_TEST_SUITE(suiteTestMapGraph){
    MU_RUN_TEST(assertAllocateMapGraph);
    //MU_RUN_TEST(testDijkstra);
    MU_RUN_TEST(testbestPath);
    MU_RUN_TEST(assertResetVisited);
    MU_RUN_TEST(assertResetCost);
}

int main() {

    MU_RUN_SUITE(suiteTestTileQueue);
    MU_RUN_SUITE(suiteTestMapGraph);
    MU_REPORT();

    return 0;
}