/* collision.c – Détection des collisions par formes géométriques */
#include <math.h>
#include "../include/types.h"
#include "../include/collision.h"
#include "../include/joueur.h"
#include "../include/bulle.h"

/* Utilitaire : distance entre deux points */
static float distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1, dy = y2 - y1;
    return sqrtf(dx*dx + dy*dy);
}

/* ------------------------------------------------------------------ */
/* Projectile (rectangle étroit) vs Bulle (cercle)                     */
Bulle* collision_projectile_bulle(Projectile* p, Bulle* liste_bulles)
{
    if (!p || !p->actif) return NULL;
    for (Bulle* b = liste_bulles; b != NULL; b = b->suivant) {
        /* On simplifie : on traite le projectile comme un point */
        if (distance(p->x, p->y, b->x, b->y) < b->rayon) {
            p->actif = 0;   /* le projectile est consommé */
            return b;
        }
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* Bulle (cercle) vs Joueur (rectangle)                                */
int collision_bulle_joueur(Bulle* liste_bulles, Joueur* j)
{
    if (!j || !j->vivant) return 0;

    float jx  = j->x - j->largeur / 2.0f;
    float jy  = j->y - j->hauteur / 2.0f;
    float jx2 = jx + j->largeur;
    float jy2 = jy + j->hauteur;

    for (Bulle* b = liste_bulles; b != NULL; b = b->suivant) {
        /* Point le plus proche du centre du cercle dans le rectangle */
        float px = fmaxf(jx, fminf(b->x, jx2));
        float py = fmaxf(jy, fminf(b->y, jy2));
        if (distance(px, py, b->x, b->y) < b->rayon)
            return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Éclair (segment vertical, traité comme un point) vs Joueur          */
int collision_eclair_joueur(Eclair* liste_eclairs, Joueur* j)
{
    if (!j || !j->vivant) return 0;
    float jx  = j->x - j->largeur / 2.0f;
    float jy  = j->y - j->hauteur / 2.0f;
    float jx2 = jx + j->largeur;
    float jy2 = jy + j->hauteur;

    for (Eclair* e = liste_eclairs; e != NULL; e = e->suivant) {
        if (!e->actif) continue;
        if (e->x >= jx && e->x <= jx2 && e->y >= jy && e->y <= jy2)
            return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
ArmeBonus* collision_joueur_arme(Joueur* j, ArmeBonus* liste_armes)
{
    if (!j) return NULL;
    for (ArmeBonus* a = liste_armes; a != NULL; a = a->suivant) {
        if (!a->actif) continue;
        if (fabsf(a->x - j->x) < j->largeur / 2.0f + 16.0f &&
            fabsf(a->y - j->y) < j->hauteur / 2.0f + 16.0f) {
            a->actif = 0;
            return a;
        }
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
int collision_projectile_boss(Projectile* liste_proj, Boss* boss)
{
    if (!boss || !boss->actif) return 0;
    float bx  = boss->x - boss->largeur / 2.0f;
    float by  = boss->y - boss->hauteur / 2.0f;
    float bx2 = bx + boss->largeur;
    float by2 = by + boss->hauteur;

    for (Projectile* p = liste_proj; p != NULL; p = p->suivant) {
        if (!p->actif) continue;
        if (p->x >= bx && p->x <= bx2 && p->y >= by && p->y <= by2) {
            p->actif = 0;
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
int collision_boss_joueur(Boss* boss, Joueur* j)
{
    if (!boss || !boss->actif || !j || !j->vivant) return 0;
    float bx  = boss->x - boss->largeur / 2.0f;
    float by  = boss->y - boss->hauteur / 2.0f;
    float bx2 = bx + boss->largeur;
    float by2 = by + boss->hauteur;
    float jx  = j->x - j->largeur / 2.0f;
    float jy  = j->y - j->hauteur / 2.0f;
    float jx2 = jx + j->largeur;
    float jy2 = jy + j->hauteur;

    return !(bx2 < jx || jx2 < bx || by2 < jy || jy2 < by);
}
