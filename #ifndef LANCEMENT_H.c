#ifndef LANCEMENT_H
#define LANCEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <math.h>

// --- CONSTANTES ---
#define LIGNES 15
#define COLONNES 20
#define TYPES_ITEMS 5

// --- STRUCTURES ---
typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int grille[LIGNES][COLONNES];
    int score;
    int coupsRestants;
    int vies;
    int niveauActuel;
    int objectifScore;
} Partie;

// --- PROTOTYPES ---
void initialiserPartie(Partie *p, int niveau);
void afficherInterface(Partie *p, Position curseur, Position selection);
int gererEntree(Position *curseur, Position *selection, Partie *p);
void permuter(Partie *p, Position pos1, Position pos2);
int verifierAlignements(Partie *p);
void appliquerGravite(Partie *p);
void sauvegarderPartie(Partie *p, char *pseudo);
int chargerPartie(Partie *p, char *pseudo);
void menuPrincipal();
void jouerNiveau(Partie *p);
void gotoxy(int x, int y);
void color(int couleur);

#endif