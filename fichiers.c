#include "lancement.h"

void sauvegarderPartie(Partie *p, char *pseudo) {
    char nomFichier[100];
    sprintf(nomFichier, "%s.txt", pseudo);
    FILE *f = fopen(nomFichier, "w");
    if (f) {
        fprintf(f, "%d %d %d %d %d\n", p->niveauActuel, p->vies, p->score, p->coupsRestants, p->objectifScore);
        for(int i=0; i<LIGNES; i++){
            for(int j=0; j<COLONNES; j++){
                fprintf(f, "%d ", p->grille[i][j]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }
}

int chargerPartie(Partie *p, char *pseudo) {
    char nomFichier[100];
    sprintf(nomFichier, "%s.txt", pseudo);
    FILE *f = fopen(nomFichier, "r");
    if (f) {
        if (fscanf(f, "%d %d %d %d %d", &p->niveauActuel, &p->vies, &p->score, &p->coupsRestants, &p->objectifScore) >= 5) {
            for(int i=0; i<LIGNES; i++){
                for(int j=0; j<COLONNES; j++){
                    fscanf(f, "%d", &p->grille[i][j]);
                }
            }
            fclose(f);
            return 1;
        }
        fclose(f);
    }
    return 0;
}