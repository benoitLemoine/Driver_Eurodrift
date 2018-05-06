/**
 * @brief Gestionnaire de course de voitures. Projet 1A Ensicaen.
 *
 * @authors Julien Gosme (2012)
 *          Modifi� par G. Lebrun (2015), S. Fourey (2016)
 */
#ifndef GPCONTROL_H
#define GPCONTROL_H


/**
 * @brief Structure pour le position d'un element sur la map avec des
 * coordonnees entieres
 */
typedef struct {
  int x;
  int y;
} pos2Dint;

/**
 * @brief Structure pour une position 2D avec des coordonnees flottantes
 */
typedef struct {
  float x;
  float y;
} pos2Dfloat;

/**
 * @brief Structure permettant de definir un segment et de se deplacer le long
 */
typedef struct {
  pos2Dint posStart;    /*< Debut du segment */
  pos2Dint posEnd;      /*< Fin du segment */
  pos2Dfloat posActual; /*< Position Actuel sur le segment */
  pos2Dfloat delta;     /*< Variation en x et en y pour le deplacement */
  int len;              /*< Longueur suivant l'axe principale */
  int pos;              /*< Position de reference sur la ligne */
} infoLine;

#endif //GPCONTROL_H
