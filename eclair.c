/* eclair.c */
#include <stdlib.h>
#include "../include/types.h"
#include "../include/eclair.h"

Eclair* creer_eclair(float x, float y, float vitesse)
{
    Eclair* e = (Eclair*)malloc(sizeof(Eclair));
    if (!e) return NULL;
    e->x            = x;
    e->y            = y;
    e->vitesse_chute = vitesse;
    e->actif        = 1;
    e->suivant      = NULL;
    return e;
}

void detruire_liste_eclairs(Eclair* liste)
{
    while (liste) { Eclair* t = liste->suivant; free(liste); liste = t; }
}

void deplacer_eclairs(Eclair** liste, float sol, float delta_t)
{
    for (Eclair* e = *liste; e != NULL; e = e->suivant) {
        if (!e->actif) continue;
        e->y += e->vitesse_chute;
        if (e->y > sol) e->actif = 0;
    }
}

void nettoyer_eclairs(Eclair** liste)
{
    Eclair** cur = liste;
    while (*cur) {
        if (!(*cur)->actif) {
            Eclair* tmp = *cur;
            *cur = (*cur)->suivant;
            free(tmp);
        } else {
            cur = &(*cur)->suivant;
        }
    }
}
