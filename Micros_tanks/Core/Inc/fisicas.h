#ifndef FISICAS_H
#define FISICAS_H
#include <stdint.h>
uint32_t* Fisicas_GetBuffer(void);
void ClearScreen(void);
void SetPixel(int x, int y, int color);
void DibujarEscenario(int t1_x, int t1_y, int t2_x, int t2_y);
void Fisicas_IniciarDisparo(int jugador, float angulo, float fuerza, int origen_x, int origen_y);
uint8_t ProcesarBala(int objetivo_x, int objetivo_y);
void DibujarExplosion(int x, int y, uint32_t tiempo);
int Fisicas_GetBalaX(void);
int Fisicas_GetBalaY(void);
void DibujarGameOver(void);
#endif
