#include "drivers_hardware.h"
#include "main.h" // Importante: Aquí están definidos CS_MATRIZ_Pin y hspi1

// --- CONFIGURACIÓN INTERNA ---
#define NUM_DEVICES 8
#define MAX7219_DECODE_MODE  0x09
#define MAX7219_INTENSITY    0x0A
#define MAX7219_SCAN_LIMIT   0x0B
#define MAX7219_SHUTDOWN     0x0C
#define MAX7219_TEST         0x0F

// Importamos el SPI que creaste en el main
extern SPI_HandleTypeDef hspi1;

// Función privada (solo se usa aquí)
static void MAX7219_SendAll(uint8_t address, uint8_t data) {
    uint8_t paquete[2] = {address, data};

    // Usamos TUS nombres de pines definidos en main.h
    HAL_GPIO_WritePin(CS_MATRIZ_GPIO_Port, CS_MATRIZ_Pin, GPIO_PIN_RESET);

    for(int i = 0; i < NUM_DEVICES; i++) {
        HAL_SPI_Transmit(&hspi1, paquete, 2, 10);
    }

    HAL_GPIO_WritePin(CS_MATRIZ_GPIO_Port, CS_MATRIZ_Pin, GPIO_PIN_SET);
}

void MAX7219_Init(void) {
    MAX7219_SendAll(MAX7219_SHUTDOWN, 0x01); // Despertar
    MAX7219_SendAll(MAX7219_DECODE_MODE, 0x00);
    MAX7219_SendAll(MAX7219_SCAN_LIMIT, 0x07);
    MAX7219_SendAll(MAX7219_INTENSITY, 0x01); // Brillo mínimo
    MAX7219_SendAll(MAX7219_TEST, 0x00);      // Test apagado
}

// Esta función recibe el buffer desde 'Fisicas' y lo manda a los LEDs
void HW_UpdateDisplay(uint32_t *buffer_pantalla) {
    for (int row = 0; row < 8; row++) {
        HAL_GPIO_WritePin(CS_MATRIZ_GPIO_Port, CS_MATRIZ_Pin, GPIO_PIN_RESET);

        // 1. Datos de ABAJO (Filas 8-15)
        uint32_t linea_abajo = buffer_pantalla[row + 8];
        for(int m = 3; m >= 0; m--) {
            uint8_t byte = (linea_abajo >> (m * 8)) & 0xFF;
            uint8_t paq[2] = { row + 1, byte };
            HAL_SPI_Transmit(&hspi1, paq, 2, 10);
        }

        // 2. Datos de ARRIBA (Filas 0-7)
        uint32_t linea_arriba = buffer_pantalla[row];
        for(int m = 3; m >= 0; m--) {
            uint8_t byte = (linea_arriba >> (m * 8)) & 0xFF;
            uint8_t paq[2] = { row + 1, byte };
            HAL_SPI_Transmit(&hspi1, paq, 2, 10);
        }

        HAL_GPIO_WritePin(CS_MATRIZ_GPIO_Port, CS_MATRIZ_Pin, GPIO_PIN_SET);
    }
}
