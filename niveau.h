#ifndef NIVEAU_H
#define NIVEAU_H

#include "types.h"

Niveau* initialiser_niveau(int numero);
void    detruire_niveau(Niveau* n);

/* Boucle de jeu d'un niveau.
 * Retourne : 1 = victoire, 0 = défaite, -1 = quitter */
int     boucle_de_jeu(Niveau* niv, Joueur* j);

/* Lance la séquence complète (tous les niveaux).
 * Retourne 1 si le boss est vaincu, 0 sinon. */
int     jouer_partie(const char* pseudo, int niveau_depart);

#endif
