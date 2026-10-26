#ifndef COLLISION_H
#define COLLISION_H

/* ============================================================
 *  collision.h – Détection des collisions (couche métier)
 *  Toutes les détections sont géométriques (cercle / rectangle).
 * ============================================================ */

#include "types.h"

/* Projectile touche une bulle : retourne le pointeur vers la bulle touchée
 * ou NULL si aucune collision. Désactive le projectile si touché.          */
Bulle* collision_projectile_bulle(Projectile* p, Bulle* liste_bulles);

/* Retourne 1 si une bulle touche le joueur (cercle / rectangle)            */
int    collision_bulle_joueur(Bulle* liste_bulles, Joueur* j);

/* Retourne 1 si un éclair touche le joueur                                 */
int    collision_eclair_joueur(Eclair* liste_eclairs, Joueur* j);

/* Retourne 1 si le joueur passe sur une arme au sol                        */
ArmeBonus* collision_joueur_arme(Joueur* j, ArmeBonus* liste_armes);

/* Retourne 1 si un projectile touche le boss                               */
int    collision_projectile_boss(Projectile* liste_proj, Boss* boss);

/* Retourne 1 si le boss touche le joueur                                   */
int    collision_boss_joueur(Boss* boss, Joueur* j);

#endif
