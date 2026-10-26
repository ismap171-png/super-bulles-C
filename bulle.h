#ifndef BULLE_H
#define BULLE_H

/* ============================================================
 *  bulle.h – Gestion des bulles (couche métier)
 *  Aucune dépendance à Allegro dans ce module.
 * ============================================================ */

#include "types.h"

/* Crée une nouvelle bulle et la retourne (non insérée dans une liste) */
Bulle* creer_bulle(float x, float y, float vx, float vy,
                   TailleBulle taille, int lance_eclair);

/* Libère toute la liste chaînée de bulles */
void   detruire_liste_bulles(Bulle* liste);

/* Met à jour positions + rebonds de toutes les bulles
 * zone_x, zone_y  : coin supérieur gauche de la zone de jeu
 * zone_w, zone_h  : dimensions de la zone de jeu
 * delta_t         : temps écoulé en secondes depuis la dernière frame        */
void   deplacer_bulles(Bulle* liste,
                       float zone_x, float zone_y,
                       float zone_w, float zone_h,
                       float delta_t);

/* Supprime la bulle pointée par *ref et insère les bulles filles.
 * Retourne 1 si une arme doit tomber (contient_arme était vrai).
 * La bulle mini est simplement détruite (aucune division).               */
int    diviser_bulle(Bulle** liste, Bulle* bulle_touchee,
                     float* arme_x_out, float* arme_y_out);

/* Retourne le rayon correspondant à une taille */
float  rayon_pour_taille(TailleBulle t);

/* Met à jour les timers d'éclair des bulles.
 * Si une bulle déclenche un éclair, l'ajoute à liste_eclairs.           */
void   maj_eclairs_bulles(Bulle* liste, Eclair** liste_eclairs,
                          float delta_t, int niveau_actif);

#endif /* BULLE_H */
