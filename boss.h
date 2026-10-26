#ifndef BOSS_H
#define BOSS_H
#include "types.h"
Boss* creer_boss(float zone_w, float zone_y, float zone_h);
void  maj_boss(Boss* b, Bulle** liste_bulles,
               float zx, float zy, float zw, float zh, float dt);
#endif
