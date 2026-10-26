/* ============================================================
 *  ihm.c – Menus, saisie, lecture des entrées clavier/souris
 *  C'est ici que se trouvent TOUS les appels à key[] et mouse.
 * ============================================================ */

#include <allegro.h>
#include <string.h>
#include <ctype.h>

#include "../include/types.h"
#include "../include/ihm.h"
#include "../include/affichage.h"
#include "../include/sauvegarde.h"
#include "../include/niveau.h"

/* ------------------------------------------------------------------ */
EtatJeu gerer_menu(void)
{
    /* Options du menu */
    const char* options[] = {
        "Regles du jeu",
        "Nouvelle partie",
        "Reprendre une partie",
        "Quitter"
    };
    const int NB_OPTIONS = 4;
    int selection = 0;

    clear_keybuf();

    while (1) {
        /* Afficher le menu */
        afficher_menu(options, NB_OPTIONS, selection);

        /* Navigation clavier */
        if (key[KEY_UP]   || key[KEY_Z]) {
            selection = (selection - 1 + NB_OPTIONS) % NB_OPTIONS;
            while (key[KEY_UP] || key[KEY_Z]) rest(10);
        }
        if (key[KEY_DOWN] || key[KEY_S]) {
            selection = (selection + 1) % NB_OPTIONS;
            while (key[KEY_DOWN] || key[KEY_S]) rest(10);
        }
        if (key[KEY_ENTER] || key[KEY_SPACE]) {
            while (key[KEY_ENTER] || key[KEY_SPACE]) rest(10);
            break;
        }
        /* Navigation souris */
        poll_mouse();
        if (mouse_b & 1) {
            int clique = menu_clic_souris(mouse_x, mouse_y, NB_OPTIONS);
            if (clique >= 0) { selection = clique; break; }
        }

        rest(16);
    }

    switch (selection) {
    case 0: afficher_regles(); return ETAT_MENU;
    case 1: return ETAT_JEU;
    case 2: {
        /* Reprendre : demander le pseudo puis charger */
        char pseudo[32] = "";
        saisir_pseudo(pseudo);
        int niv = charger_partie(pseudo);
        if (niv <= 0) niv = 1;
        jouer_partie(pseudo, niv);
        return ETAT_MENU;
    }
    case 3: return ETAT_QUITTER;
    default: return ETAT_QUITTER;
    }
}

/* ------------------------------------------------------------------ */
void saisir_pseudo(char* buf_out)
{
    int   len = 0;
    char  buf[32] = "";
    int   done = 0;

    clear_keybuf();

    while (!done) {
        afficher_saisie_pseudo(buf);

        if (keypressed()) {
            int c = readkey();
            int ascii = c & 0xFF;
            int scan  = (c >> 8) & 0xFF;

            if (scan == KEY_ENTER && len > 0) {
                done = 1;
            } else if (scan == KEY_BACKSPACE && len > 0) {
                buf[--len] = '\0';
            } else if (isalnum(ascii) && len < 31) {
                buf[len++] = (char)ascii;
                buf[len]   = '\0';
            }
        }
        rest(16);
    }

    strncpy(buf_out, buf, 31);
    buf_out[31] = '\0';
}

/* ------------------------------------------------------------------ */
int lire_entrees_jeu(int* direction, int* tire)
{
    *direction = 0;
    *tire      = 0;

    if (key[KEY_LEFT]  || key[KEY_Q]) *direction = -1;
    if (key[KEY_RIGHT] || key[KEY_D]) *direction =  1;
    if (key[KEY_SPACE])               *tire       =  1;
    if (key[KEY_ESC])                 return 1;   /* quitter */

    return 0;
}
