/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_gyroscope.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi5;

osThreadId Task01Handle;
osThreadId Task02Handle;
osThreadId Task03Handle;
osThreadId Task04Handle;
osThreadId Task05Handle;
/* USER CODE BEGIN PV */

float xyz_rotation[3]; //rotation 3 axis vars
float xyz_ang_rate[3]; //angle rate vars

uint8_t msg_buf[128] = "Hello gyro\n\r";
int height = 0, compare_height = 0;
int row = 0; //Tang kit theo chieu ngang
int rowDirect = 0; //Huong tang trai phai 1:trai 0: phai
int flag = 0;
int count_score = 0,count_height = 25; //Diem tro choi
char str[10]; //temp string
uint8_t display_score[] = "Score:"; //Scored

int RollY_Value = 160; //truc y tren man hinh lcd nhung gia tri tinh toan cho bien nay la angular rate cua X gyro

//TickType_t initial_time = 0, end_time = 0,diff = 0; //get time to control gyroscope drift control angle


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI5_Init(void);
void Task01_Init(void const * argument);
void Task02_Init(void const * argument);
void Task03_Init(void const * argument);
void Task04_Init(void const * argument);
void Task05_Init(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task01 */
  osThreadDef(Task01, Task01_Init, osPriorityAboveNormal, 0, 216);
  Task01Handle = osThreadCreate(osThread(Task01), NULL);

  /* definition and creation of Task02 */
  osThreadDef(Task02, Task02_Init, osPriorityNormal, 0, 216);
  Task02Handle = osThreadCreate(osThread(Task02), NULL);

  /* definition and creation of Task03 */
  osThreadDef(Task03, Task03_Init, osPriorityNormal, 0, 216);
  Task03Handle = osThreadCreate(osThread(Task03), NULL);

  /* definition and creation of Task04 */
  osThreadDef(Task04, Task04_Init, osPriorityAboveNormal, 0, 216);
  Task04Handle = osThreadCreate(osThread(Task04), NULL);

  /* definition and creation of Task04 */
  osThreadDef(Task05, Task05_Init, osPriorityNormal, 0, 216);
  Task05Handle = osThreadCreate(osThread(Task05), NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GREEN_LED_Pin|RED_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GREEN_LED_Pin RED_LED_Pin */
  GPIO_InitStruct.Pin = GREEN_LED_Pin|RED_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task01_Init */
/**
  * @brief  Function implementing the Task01 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task01_Init */
void Task01_Init(void const * argument) //Init + read GYRO
{
  /* init code for USB_DEVICE */
	MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */
  /* Infinite loop */

  //Init Gyro
    BSP_GYRO_Init();

  //Init LCD
	BSP_LCD_Init();

  //LCD default
	BSP_LCD_LayerDefaultInit(1,SDRAM_DEVICE_ADDR);
	BSP_LCD_SelectLayer(1);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
	BSP_LCD_SetBackColor(LCD_COLOR_DARKGREEN);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_SetFont(&Font20); //Font
	BSP_LCD_GetFont();

  for(;;)
  {
	 //read GYROSCOPE
	  BSP_GYRO_GetXYZ(xyz_rotation);

	  for(int i = 0;i<3;i++)
	  {
		  xyz_ang_rate[i] = (xyz_rotation[i]/2000)*70; //angular rate
	  }

	  osDelay(100);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task02_Init */
/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task02_Init */
void Task02_Init(void const * argument)
{
  /* USER CODE BEGIN Task02_Init */

  /* Infinite loop */
  for(;;)
  {

    if(xyz_ang_rate[0] >= 0){ //x

    if(xyz_ang_rate[0] <= 25) //trang thai can bang
    {
    	BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    	BSP_LCD_DrawCircle(BSP_LCD_GetXSize() - 120, 160, 25);
    	BSP_LCD_FillCircle(BSP_LCD_GetXSize() - 120, 160, 25);


    	sprintf(str,"%d",count_score);
    	//hien thi diem dat duoc tren man hinh
    	//noi ket chuoi
    	BSP_LCD_DisplayStringAt(11,13,display_score,LEFT_MODE);
    	BSP_LCD_DisplayStringAt(100,13,(uint8_t*)str,LEFT_MODE);

    	osDelay(50);

    }
    else //truong hop bong da duoc tang len
    {
    	//phu thuoc vao muc do tang bong tuc la lay tu angular rate x quyet dinh do to cua bong
    	//do to cua bong cung chinh la khoang cach giua mat vot voi diem can bang
    	//luc nay cho bong duoc tang len max roi tu tu ha xuong den diem can bang
    	height = fabs(xyz_ang_rate[0])/30 + 25;

    	if (xyz_ang_rate[1] > 0) rowDirect = 0; else rowDirect = 1;
    	row    = fabs(xyz_ang_rate[1])/30 + 20; //gyro lech theo phuong ngang theo truc Y

    	//neu height qua lon chung ta se set max height, tranh truong hop bong vuot ra ngoai pham vi cho phep
    	if(height > 110)
    	{
    		height = 110; //ep height khong vuot tam
    	}
    	if(row > 110)
    	{
    	    height = 110; //ep row khong vuot tam
    	}
    	//-- ket thuc set max

 
    	flag = 1; //flag co de nhan biet khi nao bong duoc tang len va tinh diem
    	while(height!=25){  //Trang thai nang

    		if(count_height == height) break;

    		++count_height;
    		if (rowDirect == 0){
    			BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    			BSP_LCD_DrawCircle(BSP_LCD_GetXSize() + row, RollY_Value, count_height);
    			BSP_LCD_FillCircle(BSP_LCD_GetXSize() + row, RollY_Value, count_height);
    		}
    		else {
    			BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    			BSP_LCD_DrawCircle(BSP_LCD_GetXSize() - row, RollY_Value, count_height);
    			BSP_LCD_FillCircle(BSP_LCD_GetXSize() - row, RollY_Value, count_height);
    		}
    		BSP_LCD_DisplayStringAt(11,13,display_score,LEFT_MODE);
    		BSP_LCD_DisplayStringAt(100,13,(uint8_t*)str,LEFT_MODE);

    		osDelay(10);

    		} //end while

    	while(count_height != 25) //Trang thai ha
    	{
    		count_height--;

    		/*
    		BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    		BSP_LCD_DrawCircle(BSP_LCD_GetXSize() - 120, RollY_Value, count_height);
    		BSP_LCD_FillCircle(BSP_LCD_GetXSize() - 120, RollY_Value, count_height);
			*/
    		if (rowDirect == 0){
    		    BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    		    BSP_LCD_DrawCircle(BSP_LCD_GetXSize() - row, RollY_Value, count_height);
    		    BSP_LCD_FillCircle(BSP_LCD_GetXSize() - row, RollY_Value, count_height);
    		}
    		else {
    		    BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    		    BSP_LCD_DrawCircle(BSP_LCD_GetXSize() + row, RollY_Value, count_height);
    		    BSP_LCD_FillCircle(BSP_LCD_GetXSize() + row, RollY_Value, count_height);
    		    		}
    		sprintf(str,"%d",count_score);
    		//hien thi diem dat duoc tren man hinh
    		BSP_LCD_DisplayStringAt(11,13,display_score,LEFT_MODE);
    		BSP_LCD_DisplayStringAt(100,13,(uint8_t*)str,LEFT_MODE);
    		osDelay(10);
    	}
    	count_height = 25; //reset lai bien dem do do cao bong
    	}
	}
	  osDelay(1);
  }
  /* USER CODE END Task02_Init */
}

/* USER CODE BEGIN Header_Task03_Init */
/**
* @brief Function implementing the Task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task03_Init */
void Task03_Init(void const * argument) //Hien thi do cao bong
{
  /* USER CODE BEGIN Task03_Init */
  /* Infinite loop */
  for(;;)
  {
	sprintf(msg_buf,"Do cao bong:%d\n", count_height);
  	CDC_Transmit_HS(msg_buf,strlen((const char*)msg_buf));
    osDelay(100); // cu 100ms xuat do cao bong 1 lan
  }
  /* USER CODE END Task03_Init */
}

/* USER CODE BEGIN Header_Task04_Init */
/**
* @brief Function implementing the Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task04_Init */
void Task04_Init(void const * argument) //Game over
{
  /* USER CODE BEGIN Task04_Init */
  /* Infinite loop */
  for(;;)
  {

	if(height >= 40 && height <= 110 && flag == 1)
	{
		++count_score;
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
		osDelay(1700);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);

		  flag = 0;
			//get current height
			compare_height = fabs(xyz_ang_rate[0])/30 + 25;

			//neu height qua lon chung ta se set max height, tranh truong hop bong vuot ra ngoai pham vi cho phep
			if(compare_height > 110)
			{
			    compare_height = 110; //ep height khong vuot tam
			}
			//-- ket thuc set max height
			if(compare_height >= 60)
			{
				//---- GAME OVER SCREEN ----
				//BEGIN
				    BSP_LCD_Clear(LCD_COLOR_RED);
				    BSP_LCD_SetBackColor(LCD_COLOR_RED);
				    BSP_LCD_DisplayStringAt(1, 130, "GAME OVER", CENTER_MODE);

				    BSP_LCD_DisplayStringAt(1,150,display_score,CENTER_MODE);
				    BSP_LCD_DisplayStringAt(1,175,(uint8_t*)str,CENTER_MODE);

				 //END

				    //turn on RED LED
				    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);

				    vTaskDelete(Task01_Init); //Xóa đi các task sau khi game over chỉ giữ lại mỗi task4 để hiển thị 'GAME OVER'
				    vTaskDelete(Task02_Init);
				    vTaskDelete(Task03_Init);
			}

	}
	osDelay(1);
  }
  /* USER CODE END Task04_Init */
}

/* USER CODE BEGIN Header_Task05_Init */
/**
* @brief Function implementing the Task05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task05_Init */
void Task05_Init(void const * argument)
{
  /* USER CODE BEGIN Task05_Init */
	//Task05 dung de lan bong theo chieu nang kit
	//giai thuat hom truoc minh tinh angle lay truoc tru sau ra duoc xap xi 100 do co khi lai dung vi neu tinh
	//theo thuoc do do thi neu KIT nam nhu the kha nang cao tu 90 do den 100 do hon co ve chinh xac
	/* Infinite loop */
  for(;;)
  {
	//Version1: sử dụng trực tiếp angular rate tu tinh
	if(xyz_ang_rate >= 180 && xyz_ang_rate[0] <= 190) //trang thai lan can bang
	{
		RollY_Value = 160;
	}

	if(xyz_ang_rate[0]> 190) //phat hien KIT nghieng thi bat dau lan banh
	{
		++RollY_Value;
		if(RollY_Value==295)
		{
			RollY_Value = 160;
		}
	}

    osDelay(50);
  }
  /* USER CODE END Task05_Init */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
