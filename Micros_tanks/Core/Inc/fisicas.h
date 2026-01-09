/*
 * fisicas.h
 *
 *  Created on: Jan 5, 2026
 *      Author: omarf
 */

#ifndef INC_FISICAS_H_
#define INC_FISICAS_H_

#include <stdint.h>

void Fisicas_Init(void);
uint32_t* Fisicas_GetBuffer(void);
void Fisicas_LimpiarPantalla(void);

// Dibujado
void Fisicas_DibujarEscenario(int t1_x, int t1_y, int t2_x, int t2_y);
void Fisicas_DibujarExplosion(int x, int y, uint32_t tiempo_explosion);
void Fisicas_DibujarGameOver(void);
void Fisicas_PintarBala(void);

// Lógica
void Fisicas_PrepararDisparo(int jugador, float angulo, int origen_x, int origen_y);
void Fisicas_CalcularSiguientePosicion(void);
int Fisicas_DetectarColision(int objetivo_x, int objetivo_y);
int Fisicas_GetBalaX(void);
int Fisicas_GetBalaY(void);

#endif /* INC_FISICAS_H_ */
