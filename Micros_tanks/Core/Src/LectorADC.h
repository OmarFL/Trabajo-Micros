#ifndef SRC_LECTORADC_H_
#define SRC_LECTORADC_H_

#include "main.h"   // HAL, uint16_t, ADC_HandleTypeDef

typedef struct {
	float grados;
	float potencia;
	uint16_t adc_val[2];
} LectorADC_t;

/* Inicializa la estructura */
void LectorADC_Init(LectorADC_t *lector);

/* Actualiza las lecturas del ADC */
void LectorADC_Actualizar(LectorADC_t *lector);

/* Getters */
float LectorADC_GetGrados(LectorADC_t *lector);
float LectorADC_GetPotencia(LectorADC_t *lector);

#endif /* SRC_LECTORADC_H_ */
