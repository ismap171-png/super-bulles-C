#ifndef IHM_H
#define IHM_H

/* ============================================================
 *  ihm.h – Interface Homme-Machine
 *  Gère clavier, souris, menus, saisie du pseudo.
 *  C'est le seul module qui lit directement les touches Allegro.
 * ============================================================ */

#include "types.h"

/* Affiche le menu principal et retourne l'état suivant */
EtatJeu gerer_menu(void);

/* Saisie du pseudo via clavier (affichage graphique) */
void    saisir_pseudo(char* buf_out);

/* Lit les entrées pendant une frame de jeu.
 * direction : -1 / 0 / 1 ; tire : 0 ou 1
 * Retourne 1 si le joueur veut quitter (Echap).            */
int     lire_entrees_jeu(int* direction, int* tire);

#endif
