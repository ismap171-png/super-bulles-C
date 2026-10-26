/* ============================================================
 *  main.c – Point d'entrée du programme Super Bulles
 *  Initialise Allegro 4 et lance la boucle de menu.
 * ============================================================ */

#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/types.h"
#include "../include/ressources.h"
#include "../include/ihm.h"
#include "../include/affichage.h"
#include "../include/niveau.h"
#include "../include/joueur.h"
#include "../include/sauvegarde.h"

/* Prototypes internes */
static void initialiser_allegro(void);
static void boucle_principale(void);

/* ------------------------------------------------------------------ */
int main(void)
{
    initialiser_allegro();
    charger_ressources();      /* charge les bitmaps / sons */
    boucle_principale();
    liberer_ressources();
    allegro_exit();
    return 0;
}
END_OF_MAIN()   /* macro obligatoire Allegro 4 (gère WinMain sur Windows) */

/* ------------------------------------------------------------------ */
static void initialiser_allegro(void)
{
    if (allegro_init() != 0) {
        fprintf(stderr, "Erreur initialisation Allegro : %s\n", allegro_error);
        exit(EXIT_FAILURE);
    }

    install_keyboard();
    install_mouse();
    install_timer();

    /* Mode graphique – fenêtre 800×600 */
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, FENETRE_LARGEUR, FENETRE_HAUTEUR, 0, 0) != 0) {
        fprintf(stderr, "Impossible d'ouvrir la fenêtre graphique : %s\n", allegro_error);
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    set_window_title("Super Bulles – ECE ING1");

    /* Double buffering : on travaille sur un bitmap en mémoire */
    /* puis on blitte vers l'écran pour éviter le scintillement */
    set_color_depth(desktop_color_depth());
}

/* ------------------------------------------------------------------ */
/* Boucle principale : menu → jeu → retour menu                       */
static void boucle_principale(void)
{
    EtatJeu etat = ETAT_MENU;
    char pseudo[32] = "";
    int  niveau_depart = 1;

    while (etat != ETAT_QUITTER) {

        switch (etat) {

        /* ---- Menu principal ---- */
        case ETAT_MENU:
            etat = gerer_menu();
            break;

        /* ---- Lancement d'une nouvelle partie ---- */
        case ETAT_JEU: {
            /* Saisie du pseudo si pas encore fait */
            if (pseudo[0] == '\0') {
                saisir_pseudo(pseudo);
                niveau_depart = 1;
            }

            /* Boucle sur les niveaux */
            int resultat = jouer_partie(pseudo, niveau_depart);

            if (resultat == 1) {
                /* Victoire totale */
                afficher_victoire_finale();
            }
            /* Dans tous les cas on revient au menu */
            pseudo[0]     = '\0';
            niveau_depart = 1;
            etat = ETAT_MENU;
            break;
        }

        default:
            etat = ETAT_QUITTER;
            break;
        }
    }
}
