#ifndef JOUEUR_H
#define JOUEUR_H

/* ============================================================
 *  joueur.h – Gestion du personnage (couche métier)
 *  Aucune dépendance à Allegro dans ce module.
 * ============================================================ */

#include "types.h"

/* Alloue et initialise un joueur */
Joueur* creer_joueur(const char* pseudo);

/* Libère la mémoire du joueur */
void    detruire_joueur(Joueur* j);

/* Déplace le joueur (direction : -1 gauche, 0 immobile, 1 droite) */
/* limite_gauche et limite_droite : bords de la zone de jeu         */
void    deplacer_joueur(Joueur* j, int direction,
                        float limite_gauche, float limite_droite);

/* Tente de créer un projectile ; renvoie 1 si tir effectué, 0 sinon */
int     tirer(Joueur* j, Projectile** liste_proj);

/* Applique une arme bonus au joueur */
void    recuperer_arme(Joueur* j, TypeArme type, float duree);

/* Mise à jour du timer de l'arme (delta_t en secondes) */
void    maj_arme(Joueur* j, float delta_t);

/* Repositionne le joueur au centre bas de la zone de jeu */
void    reinitialiser_position_joueur(Joueur* j,
                                      float zone_largeur, float zone_hauteur,
                                      float zone_y);

#endif /* JOUEUR_H */
