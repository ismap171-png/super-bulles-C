#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H
#include "types.h"
void sauvegarder_partie(const char* pseudo, int niveau_gagne);
int  charger_partie(const char* pseudo);   /* retourne le niveau, 0 si absent */
#endif
