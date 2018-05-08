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
 * @author LEMOINE Benoît <blemoine@ecole.ensicaen.fr>
 * @version 0.0.1 / 27-11-2017
 */


/**
 * @file mathObjects.c
 */

#include "../include/mathObjects.h"

int isEqualVector2D(Vector2D firstVector, Vector2D secondVector) {

    if (firstVector.x != secondVector.x) {
        return 0;
    }

    if (firstVector.y != secondVector.y) {
        return 0;
    }

    return 1;
}
 
 