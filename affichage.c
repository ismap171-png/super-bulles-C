/* ============================================================
 *  affichage.c – Rendu graphique Allegro 4
 *  Double buffering : on dessine sur 'buffer' puis blit écran.
 * ============================================================ */

#include <allegro.h>
#include <stdio.h>
#include <string.h>

#include "../include/types.h"
#include "../include/affichage.h"
#include "../include/ressources.h"

/* Buffer hors-écran pour éviter le scintillement */
static BITMAP* buffer = NULL;

/* Couleurs pré-calculées (initialisées à la 1ère frame) */
static int COL_NOIR, COL_BLANC, COL_ROUGE, COL_BLEU,
           COL_VERT, COL_JAUNE, COL_ORANGE, COL_CYAN,
           COL_GRIS, COL_VIOLET;

static void init_couleurs(void)
{
    static int init = 0;
    if (init) return;
    init = 1;
    COL_NOIR   = makecol(0,   0,   0);
    COL_BLANC  = makecol(255, 255, 255);
    COL_ROUGE  = makecol(220, 50,  50);
    COL_BLEU   = makecol(50,  100, 220);
    COL_VERT   = makecol(50,  200, 50);
    COL_JAUNE  = makecol(240, 220, 0);
    COL_ORANGE = makecol(255, 140, 0);
    COL_CYAN   = makecol(0,   200, 220);
    COL_GRIS   = makecol(140, 140, 140);
    COL_VIOLET = makecol(160, 50,  200);
}

static BITMAP* get_buffer(void)
{
    if (!buffer)
        buffer = create_bitmap(SCREEN_W, SCREEN_H);
    return buffer;
}

/* Couleur d'une bulle selon sa taille */
static int couleur_bulle(TailleBulle t)
{
    switch (t) {
    case TAILLE_GRANDE:  return COL_ROUGE;
    case TAILLE_MOYENNE: return COL_ORANGE;
    case TAILLE_PETITE:  return COL_JAUNE;
    case TAILLE_MINI:    return COL_CYAN;
    default:             return COL_BLANC;
    }
}

/* ------------------------------------------------------------------ */
void afficher_niveau(Niveau* niv, Joueur* j)
{
    init_couleurs();
    BITMAP* buf = get_buffer();

    /* Fond */
    clear_to_color(buf, COL_BLEU);

    /* Sol et bords */
    rectfill(buf, 0, ZONE_INFO_HAUTEUR, SCREEN_W,
             SCREEN_H, makecol(30, 60, 120));

    /* Zone d'informations */
    rectfill(buf, 0, 0, SCREEN_W, ZONE_INFO_HAUTEUR - 1, COL_NOIR);
    char info[128];
    snprintf(info, sizeof(info), "%s | Score: %d | Temps: %d s",
             j->pseudo, j->score, (int)niv->temps_restant);
    textout_ex(buf, font, info, 10, 12, COL_BLANC, -1);

    /* --- Bulles --- */
    for (Bulle* b = niv->liste_bulles; b != NULL; b = b->suivant) {
        int col = couleur_bulle(b->taille);
        circlefill(buf, (int)b->x, (int)b->y, (int)b->rayon, col);
        circle(buf, (int)b->x, (int)b->y, (int)b->rayon, COL_BLANC);
        /* Reflet simulé */
        circlefill(buf,
                   (int)(b->x - b->rayon * 0.3f),
                   (int)(b->y - b->rayon * 0.3f),
                   (int)(b->rayon * 0.25f),
                   makecol(255, 255, 255));
    }

    /* --- Projectiles --- */
    for (Projectile* p = niv->liste_projectiles; p != NULL; p = p->suivant) {
        if (!p->actif) continue;
        vline(buf, (int)p->x, (int)p->y, (int)(p->y + 16), COL_JAUNE);
        circlefill(buf, (int)p->x, (int)p->y, 3, COL_BLANC);
    }

    /* --- Éclairs --- */
    for (Eclair* e = niv->liste_eclairs; e != NULL; e = e->suivant) {
        if (!e->actif) continue;
        line(buf, (int)e->x - 2, (int)e->y,
                  (int)e->x + 2, (int)(e->y + 20), COL_JAUNE);
        line(buf, (int)e->x + 2, (int)(e->y + 20),
                  (int)e->x - 2, (int)(e->y + 40), COL_JAUNE);
    }

    /* --- Armes au sol --- */
    for (ArmeBonus* a = niv->liste_armes_sol; a != NULL; a = a->suivant) {
        if (!a->actif) continue;
        rectfill(buf, (int)a->x - 10, (int)a->y - 6,
                      (int)a->x + 10, (int)a->y + 6, COL_VERT);
        textout_centre_ex(buf, font, "ARM", (int)a->x, (int)a->y - 4,
                          COL_BLANC, -1);
    }

    /* --- Boss --- */
    if (niv->boss && niv->boss->actif) {
        Boss* b = niv->boss;
        rectfill(buf, (int)(b->x - b->largeur/2),
                      (int)(b->y - b->hauteur/2),
                      (int)(b->x + b->largeur/2),
                      (int)(b->y + b->hauteur/2), COL_VIOLET);
        /* Barre de vie */
        int barre_max = 100;
        int barre_cur = b->pv * barre_max / 10;
        rect(buf, (int)(b->x - 40), (int)(b->y - b->hauteur/2 - 12),
                  (int)(b->x + 40), (int)(b->y - b->hauteur/2 - 4), COL_BLANC);
        rectfill(buf, (int)(b->x - 40), (int)(b->y - b->hauteur/2 - 12),
                      (int)(b->x - 40 + barre_cur),
                      (int)(b->y - b->hauteur/2 - 4), COL_ROUGE);
        textout_centre_ex(buf, font, "BOSS",
                          (int)b->x, (int)(b->y - 6), COL_BLANC, -1);
    }

    /* --- Joueur --- */
    if (j->vivant) {
        int px = (int)(j->x - j->largeur / 2.0f);
        int py = (int)(j->y - j->hauteur / 2.0f);
        /* Corps */
        rectfill(buf, px, py + j->hauteur/3,
                      px + j->largeur, py + j->hauteur, COL_CYAN);
        /* Tête */
        circlefill(buf, (int)j->x, py + j->hauteur/4,
                   j->largeur/3, makecol(240, 200, 150));
        /* Arme courante */
        if (j->arme_courante != ARME_SIMPLE) {
            char abuf[32];
            snprintf(abuf, sizeof(abuf), "%.1fs", j->arme_duree_restante);
            textout_centre_ex(buf, font, abuf, (int)j->x,
                              py - 12, COL_VERT, -1);
        }
    }

    /* Blit buffer → écran */
    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

/* ------------------------------------------------------------------ */
void afficher_decompte(int secondes)
{
    init_couleurs();
    BITMAP* buf = get_buffer();

    for (int i = secondes; i >= 1; i--) {
        clear_to_color(buf, COL_NOIR);
        char txt[4];
        snprintf(txt, sizeof(txt), "%d", i);
        /* Grand texte centré */
        textout_centre_ex(buf, font, txt,
                          SCREEN_W / 2, SCREEN_H / 2 - 8,
                          COL_JAUNE, -1);
        blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(700);
    }
    clear_to_color(buf, COL_NOIR);
    textout_centre_ex(buf, font, "READY !",
                      SCREEN_W / 2, SCREEN_H / 2 - 8, COL_VERT, -1);
    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    rest(600);
}

/* ------------------------------------------------------------------ */
int afficher_fin_niveau(int resultat, int score)
{
    init_couleurs();
    BITMAP* buf = get_buffer();

    const char* titre  = (resultat == 1) ? "VICTOIRE !" : "ECHEC...";
    int         couleur_titre = (resultat == 1) ? COL_VERT : COL_ROUGE;

    const char* options[] = { "Continuer", "Sauvegarder et continuer",
                               "Menu principal", "Quitter" };
    int sel = 0, done = 0;
    clear_keybuf();

    while (!done) {
        clear_to_color(buf, COL_NOIR);
        textout_centre_ex(buf, font, titre, SCREEN_W/2, 80, couleur_titre, -1);

        char sc[64];
        snprintf(sc, sizeof(sc), "Score : %d", score);
        textout_centre_ex(buf, font, sc, SCREEN_W/2, 120, COL_BLANC, -1);

        for (int i = 0; i < 4; i++) {
            int col = (i == sel) ? COL_JAUNE : COL_BLANC;
            textout_centre_ex(buf, font, options[i],
                              SCREEN_W/2, 200 + i * 30, col, -1);
        }
        blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (key[KEY_UP])    { sel = (sel - 1 + 4) % 4; while(key[KEY_UP])   rest(10); }
        if (key[KEY_DOWN])  { sel = (sel + 1)     % 4; while(key[KEY_DOWN]) rest(10); }
        if (key[KEY_ENTER]) { done = 1; }
        rest(16);
    }
    return sel + 1;   /* 1..4 */
}

/* ------------------------------------------------------------------ */
void afficher_victoire_finale(void)
{
    init_couleurs();
    BITMAP* buf = get_buffer();
    clear_to_color(buf, COL_NOIR);
    textout_centre_ex(buf, font, "FELICITATIONS !",
                      SCREEN_W/2, SCREEN_H/2 - 20, COL_JAUNE, -1);
    textout_centre_ex(buf, font, "Vous avez vaincu le Boss !",
                      SCREEN_W/2, SCREEN_H/2 + 10, COL_VERT, -1);
    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    rest(3000);
}

/* ------------------------------------------------------------------ */
void afficher_menu(const char** options, int nb, int selection)
{
    init_couleurs();
    BITMAP* buf = get_buffer();
    clear_to_color(buf, makecol(10, 20, 60));

    textout_centre_ex(buf, font, "SUPER BULLES",
                      SCREEN_W/2, 60, COL_JAUNE, -1);
    textout_centre_ex(buf, font, "ECE ING1 – 2025/2026",
                      SCREEN_W/2, 80, COL_GRIS, -1);

    int debut_y = 160;
    int espacement = 40;

    for (int i = 0; i < nb; i++) {
        int y   = debut_y + i * espacement;
        int col = (i == selection) ? COL_JAUNE : COL_BLANC;
        if (i == selection) {
            rectfill(buf, SCREEN_W/2 - 110, y - 2,
                          SCREEN_W/2 + 110, y + 14,
                          makecol(30, 30, 100));
        }
        textout_centre_ex(buf, font, options[i], SCREEN_W/2, y, col, -1);
    }

    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

/* ------------------------------------------------------------------ */
void afficher_saisie_pseudo(const char* texte_actuel)
{
    init_couleurs();
    BITMAP* buf = get_buffer();
    clear_to_color(buf, makecol(10, 20, 60));

    textout_centre_ex(buf, font, "Entrez votre pseudo :",
                      SCREEN_W/2, SCREEN_H/2 - 40, COL_BLANC, -1);

    /* Champ de saisie */
    rect(buf, SCREEN_W/2 - 100, SCREEN_H/2 - 10,
              SCREEN_W/2 + 100, SCREEN_H/2 + 10, COL_BLANC);
    char affiche[36];
    snprintf(affiche, sizeof(affiche), "%s_", texte_actuel);
    textout_centre_ex(buf, font, affiche, SCREEN_W/2, SCREEN_H/2 - 6,
                      COL_JAUNE, -1);

    textout_centre_ex(buf, font, "[Entree pour valider]",
                      SCREEN_W/2, SCREEN_H/2 + 30, COL_GRIS, -1);

    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

/* ------------------------------------------------------------------ */
void afficher_regles(void)
{
    init_couleurs();
    BITMAP* buf = get_buffer();
    clear_to_color(buf, COL_NOIR);

    const char* lignes[] = {
        "REGLES DU JEU",
        "",
        "- Eliminez toutes les bulles de chaque niveau",
        "- Les bulles se divisent quand elles sont touchees",
        "- Ne vous faites pas toucher !",
        "- A partir du niveau 3 : attention aux eclairs !",
        "- Au niveau 5 : affrontez le Boss !",
        "",
        "Controles :",
        "  Fleches gauche/droite  : deplacer le personnage",
        "  Espace                 : tirer",
        "  Echap                  : quitter le niveau",
        "",
        "Appuyez sur une touche pour revenir au menu"
    };
    int nb = sizeof(lignes) / sizeof(lignes[0]);

    for (int i = 0; i < nb; i++) {
        int col = (i == 0) ? COL_JAUNE : COL_BLANC;
        textout_centre_ex(buf, font, lignes[i],
                          SCREEN_W/2, 50 + i * 24, col, -1);
    }
    blit(buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();
    while (!keypressed()) rest(16);
}

/* ------------------------------------------------------------------ */
int menu_clic_souris(int mx, int my, int nb_options)
{
    int debut_y    = 160;
    int espacement = 40;
    for (int i = 0; i < nb_options; i++) {
        int y = debut_y + i * espacement;
        if (mx >= SCREEN_W/2 - 110 && mx <= SCREEN_W/2 + 110 &&
            my >= y - 2 && my <= y + 14)
            return i;
    }
    return -1;
}
