#ifndef TYPES_H
#define TYPES_H

/* ============================================================
 *  types.h – Structures de données partagées
 *  Projet Super Bulles – ING1 ECE 2025-2026
 * ============================================================ */

/* --- Taille des bulles --- */
typedef enum {
    TAILLE_GRANDE  = 4,
    TAILLE_MOYENNE = 3,
    TAILLE_PETITE  = 2,
    TAILLE_MINI    = 1   /* taille minimale : détruite à l'impact */
} TailleBulle;

/* --- Types d'armes --- */
typedef enum {
    ARME_SIMPLE    = 0,  /* tir de base, 1 projectile à la fois */
    ARME_RAPIDE    = 1,  /* enchaîne plus vite */
    ARME_MULTIPLE  = 2,  /* tirs simultanés */
    ARME_EXPLOSION = 3   /* provoque une explosion */
} TypeArme;

/* --- États du jeu --- */
typedef enum {
    ETAT_MENU      = 0,
    ETAT_JEU       = 1,
    ETAT_VICTOIRE  = 2,
    ETAT_ECHEC     = 3,
    ETAT_QUITTER   = 4
} EtatJeu;

/* ============================================================
 *  Structure Joueur
 * ============================================================ */
typedef struct {
    float x, y;           /* position du personnage */
    float vitesse;        /* vitesse de déplacement horizontal */
    int   largeur;        /* largeur du sprite */
    int   hauteur;        /* hauteur du sprite */
    int   vivant;         /* 1 = en vie, 0 = mort */
    int   score;          /* score courant */
    char  pseudo[32];     /* pseudo saisi par le joueur */
    TypeArme arme_courante;
    float    arme_duree_restante; /* secondes restantes pour l'arme bonus */
    int      peut_tirer;          /* 1 si le tir précédent est terminé (arme simple) */
} Joueur;

/* ============================================================
 *  Structure Bulle (liste chaînée)
 * ============================================================ */
typedef struct Bulle {
    float x, y;           /* centre de la bulle */
    float vx, vy;         /* vitesse horizontale et verticale */
    float rayon;          /* rayon (dépend de la taille) */
    TailleBulle taille;
    int   couleur;        /* indice de couleur (utilisé par l'affichage) */
    int   contient_arme;  /* 1 si la bulle libère une arme quand détruite */
    int   lance_eclair;   /* 1 si la bulle peut lancer des éclairs (niv. 3+) */
    float timer_eclair;   /* temps avant prochain éclair */
    struct Bulle* suivant;
} Bulle;

/* ============================================================
 *  Structure Projectile (liste chaînée)
 * ============================================================ */
typedef struct Projectile {
    float x, y;
    float vitesse;        /* vitesse vers le haut (valeur positive) */
    int   actif;          /* 1 = en vol, 0 = à supprimer */
    int   type;           /* 0 = simple, 1 = multiple */
    struct Projectile* suivant;
} Projectile;

/* ============================================================
 *  Structure Eclair (liste chaînée)
 * ============================================================ */
typedef struct Eclair {
    float x, y;
    float vitesse_chute;
    int   actif;
    struct Eclair* suivant;
} Eclair;

/* ============================================================
 *  Structure Arme au sol (bonus à ramasser)
 * ============================================================ */
typedef struct ArmeBonus {
    float    x, y;
    TypeArme type;
    int      actif;
    struct ArmeBonus* suivant;
} ArmeBonus;

/* ============================================================
 *  Structure Boss
 * ============================================================ */
typedef struct {
    float x, y;
    float vx, vy;
    int   pv;             /* points de vie */
    int   nb_touches;     /* nombre de fois touché */
    float vitesse_base;
    int   largeur, hauteur;
    float timer_bulle;    /* timer pour lâcher des bulles */
    int   actif;
} Boss;

/* ============================================================
 *  Structure Niveau
 * ============================================================ */
typedef struct {
    int   numero;
    int   temps_imparti;       /* secondes */
    float temps_restant;       /* décompte en cours */
    int   nb_bulles_init;
    Bulle*      liste_bulles;
    Projectile* liste_projectiles;
    Eclair*     liste_eclairs;
    ArmeBonus*  liste_armes_sol;
    Boss*       boss;          /* NULL si pas de boss dans ce niveau */
    int   id_decor;            /* index du décor de fond */
    int   termine;             /* 1 si le niveau est fini */
} Niveau;

/* ============================================================
 *  Structure Partie
 * ============================================================ */
typedef struct {
    Joueur* joueur;
    Niveau* niveau_courant;
    int     numero_niveau;     /* niveau en cours (1 à N) */
    int     nb_niveaux;        /* total de niveaux */
    EtatJeu etat;
} Partie;

/* ============================================================
 *  Structure Sauvegarde
 * ============================================================ */
typedef struct {
    char pseudo[32];
    int  dernier_niveau_gagne;
} Sauvegarde;

/* --- Constantes de jeu --- */
#define NB_NIVEAUX_BASE    4      /* 4 niveaux + boss */
#define RAYON_GRANDE       48.0f
#define RAYON_MOYENNE      32.0f
#define RAYON_PETITE       20.0f
#define RAYON_MINI         12.0f
#define VITESSE_JOUEUR     4.0f
#define VITESSE_PROJECTILE 8.0f
#define GRAVITE            0.15f
#define FICHIER_SAVE       "sauvegardes.txt"

/* --- Dimensions de la fenêtre (Allegro lit SCREEN_W / SCREEN_H en runtime) --- */
#define FENETRE_LARGEUR    800
#define FENETRE_HAUTEUR    600
#define ZONE_INFO_HAUTEUR  40   /* hauteur de la barre d'informations en haut */

#endif /* TYPES_H */
