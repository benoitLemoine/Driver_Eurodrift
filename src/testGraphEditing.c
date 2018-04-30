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
#include "../include/minunit.h"
#include "../include/graphEditing.h"
#include "../include/mapEditing.h"

MU_TEST(assertAllocateMapGraph) {

    MapGraph *graph;
    MapStructure map;

    readMapFromFile(&map, "../GrandPrix2018_3.0.3/tracks/starter_droit_au_but.txt");

    graph = allocateMapGraph(&map);
    mu_assert(graph != NULL, "Error, graph is NULL");
    displayGraph(graph);
    freeGraph(graph);
    freeMap(&map);

}

MU_TEST_SUITE(suiteTestMapGraph){
    MU_RUN_TEST(assertAllocateMapGraph);
}

int main(int argc, char *argv[]) {

    MU_RUN_SUITE(suiteTestMapGraph);

    return 0;
}