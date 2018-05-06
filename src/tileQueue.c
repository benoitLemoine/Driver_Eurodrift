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
 * @file tileQueue.c
 */

#include "../include/tileQueue.h"
#include <stdio.h>

TileQueue *initTileQueue() {
    TileQueue *queue;

    queue = malloc(sizeof(TileQueue));

    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void freeTileQueue(TileQueue *queue) {
    Tile *t;

    t = malloc(sizeof(TileQueueNode));
    while (!isEmptyTileQueue(queue)) {
        dequeueTileQueue(queue, t);
    }
    free(t);
    free(queue);
}

int isEmptyTileQueue(TileQueue *queue) {
    if (queue->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void enqueueByCostTileQueue(TileQueue *queue, Tile t) {
    TileQueueNode *node;

    TileQueueNode *cur;

    node = malloc(sizeof(TileQueueNode));
    node->value = t;
    node->prev = NULL;
    node->next = NULL;

    if (isEmptyTileQueue(queue)) {
        queue->head = node;
        queue->tail = node;
    } else {
        cur = queue->tail;
        while (cur->value.cost >= node->value.cost && cur->prev != NULL) {
            cur = cur->prev;
        }
        if (cur->value.cost < node->value.cost) {
            if (cur->next == NULL) {
                queue->tail = node;
            } else {
                cur->next->prev = node;
            }
            node->next = cur->next;
            node->prev = cur;
            cur->next = node;
        } else {
            queue->head = node;
            node->next = cur;
            cur->prev = node;
        }
    }
}

void enqueueTileQueue(TileQueue *queue, Tile t) {

    TileQueueNode *node;
    TileQueueNode *cur;

    node = malloc(sizeof(TileQueueNode));
    node->value = t;
    node->prev = NULL;
    node->next = NULL;

    if (isEmptyTileQueue(queue)) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->head->prev = node;
        node->next = queue->head;
        queue->head = node;
    }
}

int dequeueTileQueue(TileQueue *queue, Tile *t) {
    TileQueueNode *tmp;

    if (isEmptyTileQueue(queue)) {
        t->speedX = -1;
        t->speedY = -1;
        t->position.x = -1;
        t->position.y = -1;
        t->cost = -1;
        return 0;
    } else {
        t->speedX = queue->head->value.speedX;
        t->speedY = queue->head->value.speedY;
        t->position.x = queue->head->value.position.x;
        t->position.y = queue->head->value.position.y;
        t->cost = queue->head->value.cost;
        tmp = queue->head;
        queue->head = queue->head->next;
        if (queue->head == NULL) {
            queue->tail = NULL;
        } else {
            queue->head->prev = NULL;
        }
        free(tmp);
        return 1;
    }
}

void removeDuplicate(TileQueue *queue, Vector2D position) {

    TileQueueNode *cur;
    TileQueueNode *tmp;
    int found = 0;

    if (isEmptyTileQueue(queue)) {
        return;
    } else if (queue->head == queue->tail) {
        return;
    } else {
        cur = queue->head;

        while (cur != queue->tail) {
            if (cur->value.position.x == position.x && cur->value.position.y == position.y) {
                if (!found) {
                    found = 1;
                    cur = cur->next;
                } else {
                    tmp = cur;

                    cur->prev->next = cur->next;
                    cur->next->prev = cur->prev;

                    cur = cur->next;
                    free(tmp);
                }
            } else {
                cur = cur->next;
            }
        }

        if (cur->value.position.x == position.x && cur->value.position.y == position.y && found) {
            cur->prev->next = cur->next;
            queue->tail = cur->prev;
            free(cur);
        }
    }
}

//Work only when having a queue built from finish to start
void updateSpeedTileQueue(TileQueue *queue) {

    TileQueueNode *cur;

    if (isEmptyTileQueue(queue)) {
        return;
    } else if (queue->head == queue->tail) {
        return;
    } else {
        cur = queue->head;

        while (cur != queue->tail) {
            cur->value.speedX += cur->next->value.position.x - cur->value.position.x;
            cur->value.speedY += cur->next->value.position.y - cur->value.position.y;
            cur = cur->next;
        }
    }
}

void displayTileQueue(TileQueue *queue) {

    TileQueueNode *cur;

    if (isEmptyTileQueue(queue)) {
        printf("TileQueue empty.\n");
    } else {
        cur = queue->head;

        while (cur != queue->tail) {
            printf("-> [%3d][%3d]: Vx %3d Vy %3d : %3d\n", cur->value.position.x, cur->value.position.y,
                   cur->value.speedX,
                   cur->value.speedY, cur->value.cost);
            cur = cur->next;
        }
        printf("-> [%3d][%3d]: Vx %3d Vy %3d : %3d\n", cur->value.position.x, cur->value.position.y, cur->value.speedX,
               cur->value.speedY, cur->value.cost);
    }
}
