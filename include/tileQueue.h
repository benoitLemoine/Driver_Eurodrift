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
 * @file tileQueue.h
 */

#ifndef DRIVER_EURODRIFT_TILEQUEUE_H
#define DRIVER_EURODRIFT_TILEQUEUE_H

#include "mathObjects.h"
#include <stdlib.h>

typedef struct {
    int cost;
    Vector2D speed;
    Vector2D position;
} Tile;

typedef struct TileQueueNode TileQueueNode;

struct TileQueueNode{
    Tile value;
    TileQueueNode *prev;
    TileQueueNode *next;
};

typedef struct {
    TileQueueNode *head;
    TileQueueNode *tail;
} TileQueue;

TileQueue *initTileQueue();

TileQueue *copyTileQueue(TileQueue *queue);

int isEmptyTileQueue(TileQueue *queue);

void enqueueByCostTileQueue(TileQueue *queue, Tile t);

void enqueueTileQueue(TileQueue *queue, Tile t);

void enqueueTileQueueAtTail(TileQueue *queue, Tile t);

int dequeueTileQueue(TileQueue *queue, Tile *t);

void displayTileQueue(TileQueue *queue);

void removeDuplicate(TileQueue *queue, Vector2D position);

void updateSpeedTileQueue(TileQueue *queue);

void freeTileQueue(TileQueue *queue);

int lengthOfQueue(TileQueue *queue);

#endif //DRIVER_EURODRIFT_TILEQUEUE_H
