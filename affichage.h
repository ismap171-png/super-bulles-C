#ifndef AFFICHAGE_H
#define AFFICHAGE_H

/* ============================================================
 *  affichage.h – Rendu graphique avec Allegro 4
 *  SEUL ce module (et ressources.c) dépend d'Allegro.
 * ============================================================ */

#include "types.h"

/* --- Niveau / Jeu --- */
void afficher_niveau(Niveau* niv, Joueur* j);
void afficher_decompte(int secondes);
int  afficher_fin_niveau(int resultat, int score);  /* retourne le choix joueur */
void afficher_victoire_finale(void);

/* --- Menus --- */
void afficher_menu(const char** options, int nb, int selection);
void afficher_saisie_pseudo(const char* texte_actuel);
void afficher_regles(void);
int  menu_clic_souris(int mx, int my, int nb_options);  /* retourne index ou -1 */

#endif
