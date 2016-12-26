#include "main.h"
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"

void SystemClock_Config(void);
void Error_Handler(void);

FATFS SDFatFs; /* File system object for SD card logical drive */
FIL MyFile; /* File object */
char SDPath[4]; /* SD card logical drive path */

int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_SDIO_SD_Init();
//	MX_FATFS_Init();

	FRESULT res; /* FatFs function common result code */
	uint32_t byteswritten, bytesread; /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];

	/*##-1- Link the micro SD disk I/O driver ##################################*/
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
		/*##-2- Register the file system object to the FatFs module ##############*/
		if (f_mount(&SDFatFs, (TCHAR const*) SDPath, 0) != FR_OK) {
			/* FatFs Initialization Error */
			Error_Handler();
		} else {
			/*##-3- Create a FAT file system (format) on the logical drive #########*/
			/* WARNING: Formatting the uSD card will delete all content on the device */
//			if (f_mkfs((TCHAR const*) SDPath, 0, 0) != FR_OK) {
			if (0) {
				/* FatFs Format Error */
				Error_Handler();
			} else {
				/*##-4- Create and Open a new text file object with write access #####*/
				if (f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE)
						!= FR_OK) {
					/* 'STM32.TXT' file Open for write Error */
					Error_Handler();
				} else {
					/*##-5- Write data to the text file ################################*/
					res = f_write(&MyFile, wtext, sizeof(wtext),
							(void *) &byteswritten);

					if ((byteswritten == 0) || (res != FR_OK)) {
						/* 'STM32.TXT' file Write or EOF Error */
						Error_Handler();
					} else {
						/*##-6- Close the open text file #################################*/
						f_close(&MyFile);

						/*##-7- Open the text file object with read access ###############*/
						if (f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK) {
							/* 'STM32.TXT' file Open for read Error */
							Error_Handler();
						} else {
							/*##-8- Read data from the text file ###########################*/
							res = f_read(&MyFile, rtext, sizeof(rtext),
									(UINT*) &bytesread);

							if ((bytesread == 0) || (res != FR_OK)) {
								/* 'STM32.TXT' file Read or EOF Error */
								Error_Handler();
							} else {
								/*##-9- Close the open text file #############################*/
								f_close(&MyFile);

								/*##-10- Compare read data with the expected data ############*/
								if ((bytesread != byteswritten)) {
									/* Read data is different from the expected data */
									Error_Handler();
								}
							}
						}
					}
				}
			}
		}
	}

	/*##-11- Unlink the RAM disk I/O driver ####################################*/
	FATFS_UnLinkDriver(SDPath);

	while (1) {
	}
}

void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE()
	;

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 24;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void Error_Handler(void) {
	while (1) {
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
}

#endif
