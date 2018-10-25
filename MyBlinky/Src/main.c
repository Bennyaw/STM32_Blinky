
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//GPIO mode is at bit 0 and 1
typedef enum {
	GPIO_INPUT = 0,
	GPIO_OUTPUT,
	GPIO_ALT_FUNC,
	GPIO_ANALOG,
}GPIOMode;

//GPIO output driver type is at bit 2
typedef enum {
	GPIO_PUSH_PULL = 0,
	GPIO_OPEN_DRAIN = 1 << 2,
}GPIODriverType;

//GPIO output speed is at bit 3 and 4
typedef enum {
	GPIO_LOW_SPEED = 0,
	GPIO_MED_SPEED = 1 << 3,
	GPIO_HI_SPEED = 2 << 3,
	GPIO_VERY_HI_SPEED = 3 << 3,
}GPIOOutSpeed;

//GPIO pull type is at bit 5 and 6
typedef enum {
	GPIO_NO_PULL = 0,
	GPIO_PULL_UP = 1 << 5,
	GPIO_PULL_DOWN = 2 << 5,
}GPIOPullType;

typedef enum{
	GpioPin0  = 0x0001,
	GpioPin1  = 0x0002,
	GpioPin2  = 0x0004,
	GpioPin3  = 0x0008,
	GpioPin4  = 0x0010,
	GpioPin5  = 0x0020,
	GpioPin6  = 0x0040,
	GpioPin7  = 0x0080,
	GpioPin8  = 0x0100,
	GpioPin9  = 0x0200,
	GpioPin10 = 0x0400,
	GpioPin11 = 0x0800,
	GpioPin12 = 0x1000,
	GpioPin13 = 0x2000,
	GpioPin14 = 0x4000,
	GpioPin15 = 0x8000,
}GpioPin;

typedef volatile uint32_t IoRegister ;

typedef struct GpioRegs GpioRegs;
struct GpioRegs {
	IoRegister mode;
	IoRegister driverType;
	IoRegister outSpeed;
	IoRegister pullType;
	IoRegister inData;
	IoRegister outData;
	IoRegister outBits;
	IoRegister pinLock;
	IoRegister altFuncLow;
	IoRegister altFuncHi;
	};

#define gpioA  ((GpioRegs *)0x40020000)
#define gpioB  ((GpioRegs *)0x40020400)
#define gpioC  ((GpioRegs *)0x40020800)
#define gpioD  ((GpioRegs *)0x40020C00)
#define gpioE  ((GpioRegs *)0x40021000)
#define gpioF  ((GpioRegs *)0x40021400)
#define gpioG  ((GpioRegs *)0x40021800)
#define gpioH  ((GpioRegs *)0x40021C00)
#define gpioI  ((GpioRegs *)0x40022000)
#define gpioJ  ((GpioRegs *)0x40022400)
#define gpioK  ((GpioRegs *)0x40022800)
/**
 * To configure the GPIO pin.
 *
 * Input:
 * 	port 			the port to configure
 * 	pin				the pin to configure
 * 	configuration	the configuration setting for the pin
 *
 * 	E.g:
 * 		configureGPIO_Pin(gpioD,GPIOPin0,GPIO_OUTPUT |	 	\
 * 										 GPIO_OPEN_DRAIN |	\
 * 										 GPIO_HI_SPEED |	\
 * 										 GPIO_NO_PULL);
 *
 * 						 mode	  driverType  outSpeed	 pullType
 * 				e.g		0110101    0110101     0110101    0110101
 * 					  & 0000011	   0000100     0011000    1100000
 * 						0000001	   0000100	   0010000    0100000
 * */

void configureGPIO_Pin(GpioRegs *port, GpioPin pins, int configuration)
{
	uint32_t tempMode, tempDriverType, tempOutSpeed,tempPullType;
	uint16_t pinValue;
	uint16_t pinMask = 0x0001;

	tempMode = configuration & 0x03;
	tempDriverType = (configuration & 0x04) >> 2;
	tempOutSpeed = (configuration & 0x18) >> 3;
	tempPullType = (configuration & 0xC0) >> 5;

	for(int i=0; i<16 ; i++)
	{
		pinValue = pinMask & pins;

		if(pinValue)
		{
			port->mode &= ~(3 << (i * 2));
			port->mode |= tempMode << (i * 2);

			port->driverType &= ~(1 << (i * 2));
			port->driverType |= tempDriverType << i;

			port->outSpeed &= ~(3 << (i * 2));
			port->outSpeed |= tempOutSpeed << (i * 2);

			port->pullType &= ~(3 << (i * 2));
			port->pullType |= tempPullType << (i * 2);
		}

		pinMask = pinMask << 1;

	}


}
/*
void gpioWritePins(GpioRegs *port, GpioPin pins, int state)
{

}

void gpioTogglePins(GpioRegs *port, GpioPin pins)
{

}
*/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))
	  {
		  HAL_GPIO_WritePin(GPIOG, Led3_Pin|Led4_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(GPIOG, Led3_Pin|Led4_Pin, GPIO_PIN_RESET);
	  }


  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, Led3_Pin|Led4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : userButton_Pin */
  GPIO_InitStruct.Pin = userButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(userButton_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Led3_Pin Led4_Pin */
  GPIO_InitStruct.Pin = Led3_Pin|Led4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
