/* USER CODE BEGIN Header */
/**
  * MAIN LIMPIO Y MODULARIZADO
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drivers_hardware.h" // Driver Pantalla
#include "fisicas.h"          // Motor Gráfico y Físico
// #include "fsm_juego.h"     // <--- DESCOMENTAR CUANDO TENGAS EL ARCHIVO DE TU COMPAÑERO

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
// Flags globales para comunicarse con la FSM
volatile uint8_t flag_boton_j1 = 0;
volatile uint8_t flag_boton_j2 = 0;

// Posiciones de los tanques (Variables de estado)
int t1_x = 1, t1_y = 12;
int t2_x = 28, t2_y = 12;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

/* USER CODE BEGIN 0 */
// (Ya no hay funciones aquí, todo está en los .c externos)
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  // 1. Inicializar Hardware
  MAX7219_Init();

  // 2. Inicializar FSM (Cuando tu compañero te pase el código)
  // Juego_t juego;
  // FSM_Init(&juego);
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
	  // --- LIMPIAR PANTALLA ---
	        ClearScreen();

	        // --- DIBUJAR ESCENARIO FIJO ---
	        DibujarEscenario(t1_x, t1_y, t2_x, t2_y);

	        // --- VARIABLES DE PRUEBA (Estáticas para no perder valor en cada vuelta) ---
	        static int estado_prueba = 0; // 0=Esperar, 1=Volando, 2=Explotando
	        static int turno = 1;         // 1=Jugador1, -1=Jugador2
	        static uint32_t tiempo_inicio_explosion = 0;
	        static int pos_explosion_x, pos_explosion_y;
	        //Variable para contar cuántos tiros llevamos
	              static int contador_turnos = 0;
	        // --- MÁQUINA DE ESTADOS DE PRUEBA ---
	        switch (estado_prueba) {

	            // ESTADO 0: Preparar disparo
	            case 0:
	                HAL_Delay(500); // Pausa de medio segundo entre turnos

	                // Configurar disparo: Jugador, Angulo 45, Fuerza 60
	                int origen_x = (turno == 1) ? t1_x : t2_x;
	                int origen_y = (turno == 1) ? t1_y : t2_y;

	                Fisicas_IniciarDisparo(turno, 45.0f, 35.0f, origen_x, origen_y);

	                estado_prueba = 1; // ¡Fuego! Pasamos a volar
	                break;

	            // ESTADO 1: Bala volando
	            case 1:
	                // Calculamos objetivo (solo para saber si chocamos con tanque)
	                int obj_x = (turno == 1) ? t2_x : t1_x;
	                int obj_y = (turno == 1) ? t2_y : t1_y;

	                // Movemos la bala
	                uint8_t resultado = ProcesarBala(obj_x, obj_y);

	                // Si chocó con algo (1=Pared/Suelo, 2=Tanque)
	                if (resultado != 0) {
	                    // Guardamos DONDE explotó para dibujar la animación ahí
	                    pos_explosion_x = Fisicas_GetBalaX();
	                    pos_explosion_y = Fisicas_GetBalaY();

	                    // Iniciamos cronómetro de explosión
	                    tiempo_inicio_explosion = HAL_GetTick();
	                    estado_prueba = 2; // Pasamos a explotar
	                }
	                break;

	            // ESTADO 2: Animación Explosión
	            case 2:
	                // Dibujamos la explosión usando tus tiempos
	                DibujarExplosion(pos_explosion_x, pos_explosion_y, tiempo_inicio_explosion);

	                // Si ha pasado más de 1.6 segundos (tu tiempo máximo en fisicas.c)
	                if (HAL_GetTick() - tiempo_inicio_explosion > 1600) {
	                    // Cambiamos de turno y volvemos a empezar
	                	// ¡NUEVO! Aumentamos el contador porque el turno ha terminado
	                	                  contador_turnos++;

	                	                  // ¡NUEVO! Comprobamos si ya van 3 turnos
	                	                  if (contador_turnos >= 3) {

	                	                      // 1. Dibujamos Game Over
	                	                      DibujarGameOver();
	                	                      HW_UpdateDisplay(Fisicas_GetBuffer()); // Forzar pintado

	                	                      // 2. Esperamos 3 segundos para verlo bien
	                	                      HAL_Delay(3000);

	                	                      // 3. Reiniciamos el contador para volver a jugar
	                	                      contador_turnos = 0;
	                	                  }
	                    turno = (turno == 1) ? -1 : 1;
	                    estado_prueba = 0;
	                }
	                break;
	        }

	        // --- ENVIAR A PANTALLA ---
	        HW_UpdateDisplay( Fisicas_GetBuffer() );

	        // Velocidad del juego (50ms = 20 FPS)
	        HAL_Delay(50);
  }
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) Error_Handler();
}

static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // <--- OJO: 32
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS_MATRIZ_GPIO_Port, CS_MATRIZ_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  // CONFIGURACIÓN DE TU PIN CS (Chip Select)
  GPIO_InitStruct.Pin = CS_MATRIZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_MATRIZ_GPIO_Port, &GPIO_InitStruct);

  // BOTONES
  GPIO_InitStruct.Pin = BTN_J1_Pin|BTN_J2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // RESTO DE PINES (Audio, USB, LEDs...)
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2S3_MCK_Pin|I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = VBUS_FS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_FS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = OTG_FS_ID_Pin|OTG_FS_DM_Pin|OTG_FS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = Audio_SCL_Pin|Audio_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// INTERRUPCIONES
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // Activamos flags para que la FSM se entere en el bucle principal
    if (GPIO_Pin == BTN_J1_Pin) {
        flag_boton_j1 = 1;
    }
    if (GPIO_Pin == BTN_J2_Pin) {
        flag_boton_j2 = 1;
    }
}

void Error_Handler(void) { __disable_irq(); while (1) {} }
