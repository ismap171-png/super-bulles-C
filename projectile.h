#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "types.h"

Projectile* creer_projectile(float x, float y, float vitesse, TypeArme type);
void        detruire_liste_projectiles(Projectile* liste);
/* Déplace tous les projectiles vers le haut ; désactive ceux hors zone */
void        deplacer_projectiles(Projectile** liste, float zone_y, float delta_t);
/* Supprime les projectiles inactifs de la liste */
void        nettoyer_projectiles(Projectile** liste);
/* Réactive le droit de tir du joueur si aucun projectile actif (arme simple) */
void        verifier_peut_tirer(Joueur* j, Projectile* liste);

#endif
