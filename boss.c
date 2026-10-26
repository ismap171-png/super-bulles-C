/* boss.c */
#include <stdlib.h>
#include "../include/types.h"
#include "../include/boss.h"
#include "../include/bulle.h"

Boss* creer_boss(float zone_w, float zone_y, float zone_h)
{
    Boss* b = (Boss*)malloc(sizeof(Boss));
    if (!b) return NULL;
    b->x           = zone_w / 2.0f;
    b->y           = zone_y + 80.0f;
    b->vx          = 2.0f;
    b->vy          = 0.0f;
    b->pv          = 10;
    b->nb_touches  = 0;
    b->vitesse_base = 2.0f;
    b->largeur     = 80;
    b->hauteur     = 80;
    b->timer_bulle = 3.0f;
    b->actif       = 1;
    return b;
}

void maj_boss(Boss* b, Bulle** liste_bulles,
              float zx, float zy, float zw, float zh, float dt)
{
    if (!b || !b->actif) return;

    /* Vitesse croît avec les touches reçues */
    float v = b->vitesse_base + b->nb_touches * 0.3f;
    b->x += (b->vx > 0 ? v : -v);

    if (b->x - b->largeur/2 < zx)          { b->x = zx + b->largeur/2;  b->vx = 1; }
    if (b->x + b->largeur/2 > zx + zw)     { b->x = zx+zw-b->largeur/2; b->vx = -1;}

    /* Lâcher des bulles périodiquement */
    b->timer_bulle -= dt;
    if (b->timer_bulle <= 0.0f) {
        Bulle* bul = creer_bulle(b->x, b->y + b->hauteur/2,
                                 2.0f, -3.0f, TAILLE_PETITE, 0);
        if (bul) { bul->suivant = *liste_bulles; *liste_bulles = bul; }
        b->timer_bulle = 2.0f;
    }
}
