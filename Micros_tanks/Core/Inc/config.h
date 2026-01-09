/*
 * config.h
 *
 *  Created on: Jan 5, 2026
 *      Author: omarf
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

// --- PANTALLA ---
#define NUM_MATRICES 8

// --- FÍSICA ---
#define GRAVEDAD 0.08f        // Ajustado para que se note en pantalla pequeña
#define VELOCIDAD_DISPARO 2.5f

// --- JUGADORES ---
#define JUGADOR_1 1
#define JUGADOR_2 2

// Posiciones iniciales
#define T1_X_INI  2
#define T1_Y_INI  12
#define T2_X_INI  28
#define T2_Y_INI  12

// --- CÓDIGOS DE COLISIÓN ---
#define COLISION_NADA 0
#define COLISION_OBSTACULO 1
#define COLISION_TANQUE 2

#endif /* INC_CONFIG_H_ */
