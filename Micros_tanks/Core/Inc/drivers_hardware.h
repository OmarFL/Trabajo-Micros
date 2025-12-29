
#ifndef DRIVERS_HARDWARE_H
#define DRIVERS_HARDWARE_H
#include <stdint.h>
void MAX7219_Init(void);
void HW_UpdateDisplay(uint32_t *buffer_pantalla);
#endif
