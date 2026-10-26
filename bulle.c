/* ============================================================
 *  bulle.c – Physique et cycle de vie des bulles
 *  Couche métier pure – AUCUN appel Allegro ici.
 * ============================================================ */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "../include/types.h"
#include "../include/bulle.h"
#include "../include/eclair.h"

/* --- Vitesses initiales selon la taille --- */
static float vitesse_h_pour_taille(TailleBulle t)
{
    switch (t) {
    case TAILLE_GRANDE:  return 2.0f;
    case TAILLE_MOYENNE: return 2.5f;
    case TAILLE_PETITE:  return 3.0f;
    case TAILLE_MINI:    return 3.5f;
    default:             return 2.0f;
    }
}

/* ------------------------------------------------------------------ */
float rayon_pour_taille(TailleBulle t)
{
    switch (t) {
    case TAILLE_GRANDE:  return RAYON_GRANDE;
    case TAILLE_MOYENNE: return RAYON_MOYENNE;
    case TAILLE_PETITE:  return RAYON_PETITE;
    case TAILLE_MINI:    return RAYON_MINI;
    default:             return RAYON_MINI;
    }
}

/* ------------------------------------------------------------------ */
Bulle* creer_bulle(float x, float y, float vx, float vy,
                   TailleBulle taille, int lance_eclair)
{
    Bulle* b = (Bulle*)malloc(sizeof(Bulle));
    if (!b) {
        fprintf(stderr, "Erreur allocation bulle\n");
        return NULL;
    }
    b->x            = x;
    b->y            = y;
    b->vx           = vx;
    b->vy           = vy;
    b->rayon        = rayon_pour_taille(taille);
    b->taille       = taille;
    b->couleur      = (int)taille;      /* couleur liée à la taille */
    b->contient_arme  = (rand() % 5 == 0) ? 1 : 0; /* 20% de chance */
    b->lance_eclair   = lance_eclair;
    b->timer_eclair   = 3.0f + (rand() % 300) / 100.0f; /* 3-6 s */
    b->suivant        = NULL;
    return b;
}

/* ------------------------------------------------------------------ */
void detruire_liste_bulles(Bulle* liste)
{
    while (liste) {
        Bulle* tmp = liste->suivant;
        free(liste);
        liste = tmp;
    }
}

/* ------------------------------------------------------------------ */
/* Physique : gravité + rebonds sur les 4 murs                         */
void deplacer_bulles(Bulle* liste,
                     float zone_x, float zone_y,
                     float zone_w, float zone_h,
                     float delta_t)
{
    float mur_gauche = zone_x;
    float mur_droite = zone_x + zone_w;
    float plafond    = zone_y;
    float sol        = zone_y + zone_h;

    for (Bulle* b = liste; b != NULL; b = b->suivant) {

        /* Appliquer la gravité */
        b->vy += GRAVITE;

        /* Déplacer */
        b->x += b->vx;
        b->y += b->vy;

        /* Rebond sur le mur gauche */
        if (b->x - b->rayon < mur_gauche) {
            b->x  = mur_gauche + b->rayon;
            b->vx = fabsf(b->vx);
        }
        /* Rebond sur le mur droit */
        if (b->x + b->rayon > mur_droite) {
            b->x  = mur_droite - b->rayon;
            b->vx = -fabsf(b->vx);
        }
        /* Rebond sur le plafond */
        if (b->y - b->rayon < plafond) {
            b->y  = plafond + b->rayon;
            b->vy = fabsf(b->vy);
        }
        /* Rebond sur le sol – vitesse de rebond selon la taille */
        if (b->y + b->rayon > sol) {
            b->y  = sol - b->rayon;
            /* Rebond élastique : la vitesse verticale est inversée
             * et légèrement réduite pour simuler une perte d'énergie */
            float vh_min = vitesse_h_pour_taille(b->taille) * 3.0f;
            if (-b->vy < vh_min) b->vy = -vh_min;
            else                  b->vy = b->vy * -0.95f;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Divise (ou détruit) la bulle touchée et met à jour la liste         */
int diviser_bulle(Bulle** liste, Bulle* bulle_touchee,
                  float* arme_x_out, float* arme_y_out)
{
    int arme_tombe = 0;

    /* Récupérer info avant suppression */
    float x           = bulle_touchee->x;
    float y           = bulle_touchee->y;
    int   a_arme      = bulle_touchee->contient_arme;
    int   eclair      = bulle_touchee->lance_eclair;
    TailleBulle taille = bulle_touchee->taille;

    /* Retirer la bulle de la liste chaînée */
    Bulle** cur = liste;
    while (*cur && *cur != bulle_touchee) {
        cur = &(*cur)->suivant;
    }
    if (*cur) {
        Bulle* tmp = *cur;
        *cur = (*cur)->suivant;
        free(tmp);
    }

    /* Si taille minimale : destruction pure */
    if (taille == TAILLE_MINI) {
        if (a_arme) {
            *arme_x_out = x;
            *arme_y_out = y;
            arme_tombe  = 1;
        }
        return arme_tombe;
    }

    /* Sinon : créer 2 bulles de taille inférieure */
    TailleBulle taille_fille = (TailleBulle)(taille - 1);
    float vh = vitesse_h_pour_taille(taille_fille);
    float vy_init = -5.0f;  /* impulsion vers le haut à la division */

    Bulle* gauche = creer_bulle(x, y, -vh, vy_init, taille_fille, eclair);
    Bulle* droite = creer_bulle(x, y,  vh, vy_init, taille_fille, eclair);

    if (gauche) { gauche->suivant = *liste; *liste = gauche; }
    if (droite) { droite->suivant = *liste; *liste = droite; }

    /* Arme tombe de la bulle détruite */
    if (a_arme) {
        *arme_x_out = x;
        *arme_y_out = y;
        arme_tombe  = 1;
    }

    return arme_tombe;
}

/* ------------------------------------------------------------------ */
void maj_eclairs_bulles(Bulle* liste, Eclair** liste_eclairs,
                        float delta_t, int niveau_actif)
{
    /* Les éclairs n'apparaissent qu'à partir du niveau 3 */
    if (niveau_actif < 3) return;

    for (Bulle* b = liste; b != NULL; b = b->suivant) {
        if (!b->lance_eclair) continue;

        b->timer_eclair -= delta_t;
        if (b->timer_eclair <= 0.0f) {
            /* Lancer un éclair depuis le bas de la bulle */
            Eclair* e = creer_eclair(b->x, b->y + b->rayon, 5.0f);
            if (e) {
                e->suivant    = *liste_eclairs;
                *liste_eclairs = e;
            }
            /* Réarmer le timer */
            b->timer_eclair = 3.0f + (rand() % 300) / 100.0f;
        }
    }
}
