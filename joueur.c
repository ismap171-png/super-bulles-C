/* ============================================================
 *  joueur.c – Implémentation de la gestion du joueur
 *  Couche métier pure – AUCUN appel Allegro ici.
 * ============================================================ */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/types.h"
#include "../include/joueur.h"
#include "../include/projectile.h"

/* ------------------------------------------------------------------ */
Joueur* creer_joueur(const char* pseudo)
{
    Joueur* j = (Joueur*)malloc(sizeof(Joueur));
    if (!j) {
        fprintf(stderr, "Erreur allocation joueur\n");
        exit(EXIT_FAILURE);
    }

    strncpy(j->pseudo, pseudo, 31);
    j->pseudo[31] = '\0';

    j->x       = 0.0f;
    j->y       = 0.0f;
    j->vitesse = VITESSE_JOUEUR;
    j->largeur = 32;
    j->hauteur = 48;
    j->vivant  = 1;
    j->score   = 0;

    j->arme_courante        = ARME_SIMPLE;
    j->arme_duree_restante  = 0.0f;
    j->peut_tirer           = 1;

    return j;
}

/* ------------------------------------------------------------------ */
void detruire_joueur(Joueur* j)
{
    if (j) free(j);
}

/* ------------------------------------------------------------------ */
void deplacer_joueur(Joueur* j, int direction,
                     float limite_gauche, float limite_droite)
{
    if (!j || !j->vivant) return;

    j->x += direction * j->vitesse;

    /* Bloquer sur les bords de la zone de jeu */
    float demi = j->largeur / 2.0f;
    if (j->x - demi < limite_gauche)
        j->x = limite_gauche + demi;
    if (j->x + demi > limite_droite)
        j->x = limite_droite - demi;
}

/* ------------------------------------------------------------------ */
int tirer(Joueur* j, Projectile** liste_proj)
{
    if (!j || !j->vivant) return 0;

    /* Arme simple : 1 seul projectile à la fois */
    if (j->arme_courante == ARME_SIMPLE && !j->peut_tirer)
        return 0;

    /* Créer un projectile au-dessus du centre du personnage */
    Projectile* p = creer_projectile(j->x, j->y - j->hauteur / 2.0f,
                                     VITESSE_PROJECTILE, j->arme_courante);
    if (!p) return 0;

    /* Insérer en tête de liste */
    p->suivant  = *liste_proj;
    *liste_proj = p;

    /* Avec l'arme simple, bloquer jusqu'au prochain impact */
    if (j->arme_courante == ARME_SIMPLE)
        j->peut_tirer = 0;

    /* Arme multiple : tirs latéraux supplémentaires */
    if (j->arme_courante == ARME_MULTIPLE) {
        Projectile* gauche = creer_projectile(j->x - 10.0f,
                                              j->y - j->hauteur / 2.0f,
                                              VITESSE_PROJECTILE, ARME_SIMPLE);
        Projectile* droite = creer_projectile(j->x + 10.0f,
                                              j->y - j->hauteur / 2.0f,
                                              VITESSE_PROJECTILE, ARME_SIMPLE);
        if (gauche) { gauche->suivant = *liste_proj; *liste_proj = gauche; }
        if (droite) { droite->suivant = *liste_proj; *liste_proj = droite; }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
void recuperer_arme(Joueur* j, TypeArme type, float duree)
{
    if (!j) return;
    j->arme_courante       = type;
    j->arme_duree_restante = duree;
    j->peut_tirer          = 1;
}

/* ------------------------------------------------------------------ */
void maj_arme(Joueur* j, float delta_t)
{
    if (!j) return;
    if (j->arme_courante != ARME_SIMPLE && j->arme_duree_restante > 0.0f) {
        j->arme_duree_restante -= delta_t;
        if (j->arme_duree_restante <= 0.0f) {
            j->arme_courante       = ARME_SIMPLE;
            j->arme_duree_restante = 0.0f;
            j->peut_tirer          = 1;
        }
    }
}

/* ------------------------------------------------------------------ */
void reinitialiser_position_joueur(Joueur* j,
                                   float zone_largeur, float zone_hauteur,
                                   float zone_y)
{
    if (!j) return;
    j->x = zone_largeur / 2.0f;
    j->y = zone_y + zone_hauteur - j->hauteur / 2.0f - 5.0f;
    j->arme_courante       = ARME_SIMPLE;
    j->arme_duree_restante = 0.0f;
    j->peut_tirer          = 1;
    j->vivant              = 1;
}
