#ifndef ECLAIR_H
#define ECLAIR_H
#include "types.h"
Eclair* creer_eclair(float x, float y, float vitesse);
void    detruire_liste_eclairs(Eclair* liste);
void    deplacer_eclairs(Eclair** liste, float sol, float delta_t);
void    nettoyer_eclairs(Eclair** liste);
#endif
