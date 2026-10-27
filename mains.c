#include "lancement.h"

int main() {
    srand((unsigned)time(NULL));
    SetConsoleTitle("ECE MATCH-3 HEROES - VERSION FINALE");
    
    // Maximiser la compatibilité de la console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    menuPrincipal();
    return 0;
}

void menuPrincipal() {
    int choix = 0;
    Partie partie;
    char pseudo[50];

    do {
        system("cls"); 
        color(15); // Blanc
        printf("=== ECE MATCH-3 HEROES ===\n");
        printf("1. Lire les regles\n");
        printf("2. Nouvelle partie\n");
        printf("3. Reprendre une partie\n");
        printf("4. Quitter\n");
        printf("Votre choix : ");
        
        if (scanf("%d", &choix) != 1) {
            while (getchar() != '\n'); 
            choix = 0;
        }

        switch(choix) {
            case 1:
                printf("\nRegles: Alignez 3 lettres identiques.\n");
                printf("CONTROLES: ZQSD ou FLECHES pour bouger, ESPACE pour selectionner.\n");
                printf("Appuyez sur une touche...");
                _getch();
                break;
            case 2:
                partie.vies = 3;
                partie.niveauActuel = 1;
                initialiserPartie(&partie, partie.niveauActuel);
                jouerNiveau(&partie);
                break;
            case 3:
                printf("Pseudo de la sauvegarde : ");
                scanf("%49s", pseudo);
                if(chargerPartie(&partie, pseudo)) {
                    printf("Partie chargee !");
                    Sleep(1000);
                    jouerNiveau(&partie);
                } else {
                    printf("Sauvegarde introuvable. Appuyez sur une touche.");
                    _getch();
                }
                break;
        }
    } while (choix != 4);
}