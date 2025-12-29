#include "fisicas.h"
#include "drivers_hardware.h" // Necesitamos las definiciones pero NO HW_UpdateDisplay
#include <math.h>

// --- MEMORIA DE VÍDEO (Privada de este módulo) ---
static volatile uint32_t framebuffer[16];

// Variables internas de la bala
static float bala_x, bala_y, bala_vx, bala_vy;
#define GRAVEDAD 0.02f

// --- FUNCIONES DE MEMORIA ---

uint32_t* Fisicas_GetBuffer(void) {
    return (uint32_t*)framebuffer;
}

void ClearScreen(void) {
    for(int i = 0; i < 16; i++) framebuffer[i] = 0;
}

void SetPixel(int x, int y, int color) {
    if (x >= 0 && x < 32 && y >= 0 && y < 16) {
        if (color) framebuffer[y] |= (1UL << x);
        else       framebuffer[y] &= ~(1UL << x);
    }
}

// --- DIBUJO ---

// Funciones para que el main sepa dónde dibujar la explosión
int Fisicas_GetBalaX(void) { return (int)bala_x; }
int Fisicas_GetBalaY(void) { return (int)bala_y; }


void PintarTanque(int x, int y, int direccion) {
    static const int FORMA[3][3] = {
    		{0,0,1},
			{1,1,0},
			{1,1,1}
    };
    for (int f = 0; f < 3; f++) {
        for (int c = 0; c < 3; c++) {
            int col_leida = (direccion == 1) ? c : (2 - c);
            if (FORMA[f][col_leida] == 1) SetPixel(x + c, y + f, 1);
        }
    }
}

// Dibuja suelo, muro y tanques (usando los parámetros que le pase la FSM)
void DibujarEscenario(int t1_x, int t1_y, int t2_x, int t2_y) {
    // Suelo
    for(int x = 0; x < 32; x++) SetPixel(x, 15, 1);

    // Muro (He copiado tu diseño de muros dobles)
    for(int y = 15 - 7; y < 15; y++) {
        SetPixel(13, y, 1); SetPixel(14, y, 1); // Muro 1
        SetPixel(17, y, 1); SetPixel(18, y, 1); // Muro 2
    }

    // Tanques
    PintarTanque(t1_x, t1_y, 1);
    PintarTanque(t2_x, t2_y, -1);
}

// --- FÍSICAS DE LA BALA ---

// Configura el disparo inicial
void Fisicas_IniciarDisparo(int jugador, float angulo, float fuerza, int origen_x, int origen_y) {
    float radianes = angulo * (3.14159f / 180.0f);
    float velocidad = fuerza * 0.02f; // Factor de ajuste

    // Ajustamos origen para que salga del cañón
    if (jugador == 1) {
        bala_x = origen_x + 1;
        bala_y = origen_y - 1;
        bala_vx = cosf(radianes) * velocidad;
        bala_vy = -sinf(radianes) * velocidad;
    } else {
        bala_x = origen_x; // Lado izquierdo del tanque 2
        bala_y = origen_y - 1;
        bala_vx = -cosf(radianes) * velocidad; // Hacia la izquierda
        bala_vy = -sinf(radianes) * velocidad;
    }
}

// Actualiza posición y detecta choques ("Caja Negra")
uint8_t ProcesarBala(int objetivo_x, int objetivo_y) {
    // 1. Física
    bala_vy += GRAVEDAD;
    bala_x += bala_vx;
    bala_y += bala_vy;

    // 2. Dibujar
    int ix = (int)bala_x;
    int iy = (int)bala_y;
    if(ix >= 0 && ix < 32 && iy >= 0 && iy < 16) {
        SetPixel(ix, iy, 1);
    }

    // 3. Colisiones
    //Suelo o Muros o Fuera
    if (iy >= 15 || ix < 0 || ix > 32) return 1; // 1 = CHOQUE OBSTACULO

    //Tanque Enemigo (Hitbox 3x3)
    if (ix >= objetivo_x && ix <= (objetivo_x + 2) &&
        iy >= objetivo_y && iy <= (objetivo_y + 2)) {
        return 2; // 2 = CHOQUE TANQUE (GOL)
    }
     //Muros
    if (iy >= 8 && iy < 15) {
            // Comprobamos si la X coincide con alguno de los dos muros
            if ((ix >= 13 && ix <= 14) || (ix >= 17 && ix <= 18)) {
                return 1; // 1 = Choque Obstáculo (Explosión y cambio de turno)
            }
        }
    return 0; // 0 = SIGUE VOLANDO
}

void DibujarExplosion(int x, int y, uint32_t tiempo_explosion) {
	//tiempo_explosion se inicia el momento en el que se produce la explosion
     SetPixel(x, y, 1);
     if(HAL_GetTick()-tiempo_explosion>900) {
             	     SetPixel(x+1, y+1, 1);
             	     SetPixel(x-1, y-1, 1);
             	     SetPixel(x+1, y-1, 1);
             	     SetPixel(x-1, y+1, 1);
                 	 SetPixel(x, y, 1);
                      SetPixel(x+2, y, 1);
                      SetPixel(x-2, y, 1);
                      SetPixel(x, y-2, 1);
                      SetPixel(x, y+2, 1);
                      SetPixel(x+2, y+2, 1);
                      SetPixel(x-2, y-2, 1);
                      SetPixel(x+2, y-2, 1);
                      SetPixel(x-2, y+2, 1);

                  }
     else if(HAL_GetTick()-tiempo_explosion>600) {
             	 SetPixel(x, y, 1);
                  SetPixel(x+1, y, 1);
                  SetPixel(x-1, y, 1);
                  SetPixel(x, y-1, 1);
                  SetPixel(x, y+1, 1);
                  SetPixel(x+2, y+2, 1);
                  SetPixel(x-1, y-2, 1);
                  SetPixel(x+2, y-2, 1);
                  SetPixel(x-2, y+2, 1);
              }
     else if(HAL_GetTick()-tiempo_explosion>300) {
    	 SetPixel(x, y, 1);
         SetPixel(x+1, y+1, 1);
         SetPixel(x-1, y-1, 1);
         SetPixel(x+1, y-1, 1);
         SetPixel(x-1, y+1, 1);
     }
}
// --- PANTALLA DE GAME OVER ---

// Función auxiliar para dibujar un carácter de 8x8 en una posición concreta
// Recibe el array de 8 bytes (bitmap) que define la letra
static void DibujarLetra8x8(int x_offset, int y_offset, const uint8_t *letra) {
    for (int fila = 0; fila < 8; fila++) {
        uint8_t linea = letra[fila];
        for (int col = 0; col < 8; col++) {
            // Leemos el bit correspondiente (MSB a la izquierda)
            if (linea & (1 <<  col)) {
                SetPixel(x_offset + col, y_offset + fila, 1);
            }
        }
    }
}

void DibujarGameOver(void) {
    // 1. Borramos todo lo que hubiera antes
    ClearScreen();

    // 2. Definición de las letras (Mapas de bits 8x8)
    // Cada byte es una fila, de arriba a abajo.

    const uint8_t LETRA_G[] = { 0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0xC6, 0x66, 0x3E };
    const uint8_t LETRA_A[] = { 0x18, 0x3C, 0x66, 0xC3, 0xFF, 0xC3, 0xC3, 0xC3 };
    const uint8_t LETRA_M[] = { 0xC3, 0xE7, 0xFF, 0xDB, 0xC3, 0xC3, 0xC3, 0xC3 };
    const uint8_t LETRA_E[] = { 0xFF, 0xC0, 0xC0, 0xFF, 0xC0, 0xC0, 0xC0, 0xFF };

    const uint8_t LETRA_O[] = { 0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x3C };
    const uint8_t LETRA_V[] = { 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x3C, 0x18 };
    const uint8_t LETRA_R[] = { 0xF8, 0xFC, 0xC6, 0xC6, 0xFC, 0xF8, 0xC6, 0xC6 };
    // Reutilizamos la E de arriba

    // 3. Dibujar "GAME" en la fila superior (Y=0)
    // Matriz 0 (X=0), Matriz 1 (X=8), Matriz 2 (X=16), Matriz 3 (X=24)
    DibujarLetra8x8(0,  0, LETRA_E);
    DibujarLetra8x8(8,  0, LETRA_M);
    DibujarLetra8x8(16, 0, LETRA_A);
    DibujarLetra8x8(24, 0, LETRA_G);

    // 4. Dibujar "OVER" en la fila inferior (Y=8)
    DibujarLetra8x8(0,  8, LETRA_R);
    DibujarLetra8x8(8,  8, LETRA_E);
    DibujarLetra8x8(16, 8, LETRA_V);
    DibujarLetra8x8(24, 8, LETRA_O);
}
