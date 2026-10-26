/* projectile.c */
#include <stdlib.h>
#include <stdio.h>
#include "../include/types.h"
#include "../include/projectile.h"
#include "../include/joueur.h"

Projectile* creer_projectile(float x, float y, float vitesse, TypeArme type)
{
    Projectile* p = (Projectile*)malloc(sizeof(Projectile));
    if (!p) return NULL;
    p->x       = x;
    p->y       = y;
    p->vitesse = vitesse;
    p->actif   = 1;
    p->type    = (int)type;
    p->suivant = NULL;
    return p;
}

void detruire_liste_projectiles(Projectile* liste)
{
    while (liste) {
        Projectile* tmp = liste->suivant;
        free(liste);
        liste = tmp;
    }
}

void deplacer_projectiles(Projectile** liste, float zone_y, float delta_t)
{
    for (Projectile* p = *liste; p != NULL; p = p->suivant) {
        if (!p->actif) continue;
        p->y -= p->vitesse;   /* monte vers le haut */
        if (p->y < zone_y)    /* sorti de la zone   */
            p->actif = 0;
    }
}

void nettoyer_projectiles(Projectile** liste)
{
    Projectile** cur = liste;
    while (*cur) {
        if (!(*cur)->actif) {
            Projectile* tmp = *cur;
            *cur = (*cur)->suivant;
            free(tmp);
        } else {
            cur = &(*cur)->suivant;
        }
    }
}

void verifier_peut_tirer(Joueur* j, Projectile* liste)
{
    if (!j || j->arme_courante != ARME_SIMPLE) return;
    /* Si aucun projectile actif, le joueur peut re-tirer */
    for (Projectile* p = liste; p != NULL; p = p->suivant) {
        if (p->actif) return;
    }
    j->peut_tirer = 1;
}
