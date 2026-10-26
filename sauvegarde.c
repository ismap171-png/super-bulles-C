/* sauvegarde.c */
#include <stdio.h>
#include <string.h>
#include "../include/types.h"
#include "../include/sauvegarde.h"

/* Format du fichier : une ligne par sauvegarde : "pseudo niveau\n" */

void sauvegarder_partie(const char* pseudo, int niveau_gagne)
{
    /* Lire tout le fichier existant en mémoire */
    FILE* f = fopen(FICHIER_SAVE, "r");
    Sauvegarde sauves[64];
    int nb = 0;

    if (f) {
        while (nb < 64 &&
               fscanf(f, "%31s %d", sauves[nb].pseudo,
                      &sauves[nb].dernier_niveau_gagne) == 2) {
            nb++;
        }
        fclose(f);
    }

    /* Mettre à jour ou ajouter */
    int trouve = 0;
    for (int i = 0; i < nb; i++) {
        if (strcmp(sauves[i].pseudo, pseudo) == 0) {
            if (niveau_gagne > sauves[i].dernier_niveau_gagne)
                sauves[i].dernier_niveau_gagne = niveau_gagne;
            trouve = 1;
            break;
        }
    }
    if (!trouve && nb < 64) {
        strncpy(sauves[nb].pseudo, pseudo, 31);
        sauves[nb].dernier_niveau_gagne = niveau_gagne;
        nb++;
    }

    /* Réécrire le fichier */
    f = fopen(FICHIER_SAVE, "w");
    if (f) {
        for (int i = 0; i < nb; i++)
            fprintf(f, "%s %d\n", sauves[i].pseudo,
                    sauves[i].dernier_niveau_gagne);
        fclose(f);
    }
}

int charger_partie(const char* pseudo)
{
    FILE* f = fopen(FICHIER_SAVE, "r");
    if (!f) return 0;

    char buf[32]; int niveau;
    while (fscanf(f, "%31s %d", buf, &niveau) == 2) {
        if (strcmp(buf, pseudo) == 0) {
            fclose(f);
            return niveau;
        }
    }
    fclose(f);
    return 0;
}
