/* ============================================================
 *  niveau.c – Logique de jeu (couche métier)
 *  La boucle de jeu orchestre physique, collisions et états.
 *  Les appels graphiques sont délégués au module affichage.
 * ============================================================ */

#include <allegro.h>   /* uniquement pour rest() et la boucle de temps */
#include <stdlib.h>
#include <stdio.h>

#include "../include/types.h"
#include "../include/niveau.h"
#include "../include/joueur.h"
#include "../include/bulle.h"
#include "../include/projectile.h"
#include "../include/eclair.h"
#include "../include/collision.h"
#include "../include/affichage.h"
#include "../include/ihm.h"
#include "../include/boss.h"
#include "../include/sauvegarde.h"

/* Dimensions de la zone de jeu (calculées depuis les constantes) */
#define ZONE_X  0.0f
#define ZONE_Y  ((float)ZONE_INFO_HAUTEUR)
#define ZONE_W  ((float)FENETRE_LARGEUR)
#define ZONE_H  ((float)(FENETRE_HAUTEUR - ZONE_INFO_HAUTEUR))

/* Fréquence cible : 60 FPS */
#define FPS          60
#define DELTA_T      (1.0f / FPS)
#define MS_PAR_FRAME (1000 / FPS)

/* ------------------------------------------------------------------ */
static void ajouter_bulles_niveau(Niveau* n, int numero)
{
    /* Configuration des bulles selon le numéro de niveau */
    int   nb = 1 + numero;           /* plus de bulles aux niveaux avancés */
    int   eclair = (numero >= 3) ? 1 : 0;
    float vx_init = 2.0f + numero * 0.3f;

    for (int i = 0; i < nb; i++) {
        float x  = ZONE_W * (0.2f + 0.6f * ((float)i / nb));
        float y  = ZONE_Y + 50.0f;
        Bulle* b = creer_bulle(x, y, (i % 2 == 0) ? vx_init : -vx_init,
                               0.0f, TAILLE_GRANDE,
                               (eclair && i == 0) ? 1 : 0);
        if (b) {
            b->suivant = n->liste_bulles;
            n->liste_bulles = b;
        }
    }
    n->nb_bulles_init = nb;
}

/* ------------------------------------------------------------------ */
Niveau* initialiser_niveau(int numero)
{
    Niveau* n = (Niveau*)malloc(sizeof(Niveau));
    if (!n) return NULL;

    n->numero             = numero;
    n->temps_imparti      = 60 + (NB_NIVEAUX_BASE - numero) * 10; /* décroît */
    n->temps_restant      = (float)n->temps_imparti;
    n->liste_bulles       = NULL;
    n->liste_projectiles  = NULL;
    n->liste_eclairs      = NULL;
    n->liste_armes_sol    = NULL;
    n->boss               = NULL;
    n->id_decor           = (numero - 1) % 4;
    n->termine            = 0;
    n->nb_bulles_init     = 0;

    ajouter_bulles_niveau(n, numero);

    /* Le boss n'apparaît qu'après le 4ème niveau */
    if (numero > NB_NIVEAUX_BASE) {
        n->boss = creer_boss(ZONE_W, ZONE_Y, ZONE_H);
    }

    return n;
}

/* ------------------------------------------------------------------ */
void detruire_niveau(Niveau* n)
{
    if (!n) return;
    detruire_liste_bulles(n->liste_bulles);
    detruire_liste_projectiles(n->liste_projectiles);
    detruire_liste_eclairs(n->liste_eclairs);
    if (n->boss) { free(n->boss); }
    free(n);
}

/* ------------------------------------------------------------------ */
/* BOUCLE DE JEU PRINCIPALE D'UN NIVEAU                                */
int boucle_de_jeu(Niveau* niv, Joueur* j)
{
    /* Positionnement initial du joueur */
    reinitialiser_position_joueur(j, ZONE_W, ZONE_H, ZONE_Y);

    /* Décompte 3-2-1 avant le niveau */
    afficher_decompte(3);

    int    resultat   = 0;   /* 0 = en cours */
    long   dernier    = 0;
    int    quitter    = 0;

    while (resultat == 0 && !quitter) {

        /* --- Limiter à 60 FPS --- */
        long maintenant = clock();
        long delta_ms   = (maintenant - dernier) * 1000 / CLOCKS_PER_SEC;
        if (delta_ms < MS_PAR_FRAME) {
            rest(MS_PAR_FRAME - (int)delta_ms);
        }
        dernier = clock();

        /* --- Lecture des entrées (IHM) --- */
        int direction = 0;
        int tire      = 0;
        quitter = lire_entrees_jeu(&direction, &tire);

        /* --- Logique métier --- */

        /* Déplacement joueur */
        deplacer_joueur(j, direction, ZONE_X, ZONE_X + ZONE_W);

        /* Tir */
        if (tire) {
            tirer(j, &niv->liste_projectiles);
        }

        /* Déplacements */
        deplacer_bulles(niv->liste_bulles,
                        ZONE_X, ZONE_Y, ZONE_W, ZONE_H, DELTA_T);
        deplacer_projectiles(&niv->liste_projectiles, ZONE_Y, DELTA_T);
        deplacer_eclairs(&niv->liste_eclairs,
                         ZONE_Y + ZONE_H, DELTA_T);

        /* Mise à jour des éclairs lancés par les bulles */
        maj_eclairs_bulles(niv->liste_bulles, &niv->liste_eclairs,
                           DELTA_T, niv->numero);

        /* Mise à jour de l'arme temporaire */
        maj_arme(j, DELTA_T);

        /* Réactiver le tir (arme simple) si aucun projectile actif */
        verifier_peut_tirer(j, niv->liste_projectiles);

        /* Décompte du temps */
        niv->temps_restant -= DELTA_T;

        /* --- Collisions --- */

        /* Projectile → bulle */
        for (Projectile* p = niv->liste_projectiles; p != NULL; p = p->suivant) {
            if (!p->actif) continue;
            Bulle* touchee = collision_projectile_bulle(p, niv->liste_bulles);
            if (touchee) {
                float ax = 0.0f, ay = 0.0f;
                int arme = diviser_bulle(&niv->liste_bulles, touchee, &ax, &ay);
                j->score += 100;
                if (arme) {
                    /* Créer une arme au sol */
                    ArmeBonus* a = (ArmeBonus*)malloc(sizeof(ArmeBonus));
                    if (a) {
                        a->x    = ax; a->y = ay;
                        a->type = ARME_RAPIDE + (rand() % 2);
                        a->actif   = 1;
                        a->suivant = niv->liste_armes_sol;
                        niv->liste_armes_sol = a;
                    }
                }
            }
        }

        /* Projectile → boss */
        if (niv->boss && collision_projectile_boss(niv->liste_projectiles, niv->boss)) {
            j->score += 500;
            niv->boss->pv--;
            niv->boss->nb_touches++;
            if (niv->boss->pv <= 0) {
                resultat = 1;   /* boss vaincu → victoire */
            }
        }

        /* Bulle → joueur */
        if (collision_bulle_joueur(niv->liste_bulles, j)) {
            j->vivant = 0;
            resultat  = -1;   /* défaite */
        }

        /* Éclair → joueur */
        if (collision_eclair_joueur(niv->liste_eclairs, j)) {
            j->vivant = 0;
            resultat  = -1;
        }

        /* Boss → joueur */
        if (niv->boss && collision_boss_joueur(niv->boss, j)) {
            j->vivant = 0;
            resultat  = -1;
        }

        /* Joueur ramasse une arme */
        ArmeBonus* arme_prise = collision_joueur_arme(j, niv->liste_armes_sol);
        if (arme_prise) {
            recuperer_arme(j, arme_prise->type, 8.0f);
        }

        /* --- Conditions de fin --- */
        if (niv->boss == NULL && niv->liste_bulles == NULL) {
            resultat = 1;   /* toutes les bulles éliminées */
        }
        if (niv->temps_restant <= 0.0f && resultat == 0) {
            resultat = -1;  /* temps écoulé */
        }

        /* --- Mise à jour boss --- */
        if (niv->boss && niv->boss->actif) {
            maj_boss(niv->boss, &niv->liste_bulles,
                     ZONE_X, ZONE_Y, ZONE_W, ZONE_H, DELTA_T);
        }

        /* --- Nettoyage des listes --- */
        nettoyer_projectiles(&niv->liste_projectiles);
        nettoyer_eclairs(&niv->liste_eclairs);

        /* --- Affichage --- */
        afficher_niveau(niv, j);
    }

    if (quitter) return -2;   /* code spécial "quitter" */
    return (resultat == 1) ? 1 : 0;
}

/* ------------------------------------------------------------------ */
int jouer_partie(const char* pseudo, int niveau_depart)
{
    Joueur* j = creer_joueur(pseudo);

    for (int num = niveau_depart; num <= NB_NIVEAUX_BASE + 1; num++) {
        Niveau* niv = initialiser_niveau(num);
        int res = boucle_de_jeu(niv, j);
        detruire_niveau(niv);

        int choix = afficher_fin_niveau(res, j->score);
        /* choix : 1=continuer, 2=sauvegarder+continuer, 3=menu, 4=quitter */

        if (choix == 2 || choix == 3) {
            sauvegarder_partie(pseudo, (res == 1) ? num : num - 1);
        }
        if (choix == 3 || choix == 4 || res == -2) break;
        if (res == 0) {
            num--;   /* recommencer le même niveau si perdu */
        }
        if (res == 1 && num > NB_NIVEAUX_BASE) {
            /* Victoire finale */
            detruire_joueur(j);
            return 1;
        }
    }

    detruire_joueur(j);
    return 0;
}
