/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "displayST7735.h"
#include "rover_game.h"
#include "joystick.h"
#include "rover_math.h"
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//Timing Flags
extern uint8_t flag3MsPassed;
extern uint8_t flag10MsPassed;
extern uint8_t flag25MsPassed;
extern uint8_t flag100MsPassed;
extern uint8_t flag500MsPassed;
extern uint8_t flag1000MsPassed;

uint8_t ledStatus = false;
//Random Num
uint32_t ranNum = 0;

//Display Testing

uint8_t setDisplay = false;

uint8_t colStart = 0;
uint8_t colEnd = 127;
uint8_t rowStart = 0;
uint8_t rowEnd = 159;

uint8_t prevPlace[2] = {0,0};
uint8_t width = 128;
uint8_t height = 160;
uint32_t rgbColor32 = 0xFCFCFC;

uint16_t angle = 0;
uint16_t objType = 0;

uint8_t line1[10] = {0,0,10,160,76,2,5,1,56,19};

uint8_t rgbColor[3] = {0,0,0};
extern uint32_t carRectColor[144];
extern uint32_t spiderRectColor[150];
//Test timing
extern uint16_t timeDisplayMS;
extern uint8_t runningTrue;
uint16_t timeDisp = 0;

//Analog Joystick
//extern int16_t joystickVal[2];


extern uint8_t adcFlag;

uint16_t joyAngle=0;
uint16_t joyMag=0;

//Display Objects:
Obj_Disp obj1 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};


//Rover Game:
extern uint16_t roverTime;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

void initRNG();//RNG

void testObject(uint16_t angle, ObjectDispType objType);
uint32_t returnAnalog(uint8_t pin);

//Rover Game:
void DisplayTime();
void DisplayScore();

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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  
  //Turn off reset pin
  HAL_GPIO_WritePin(RST_PIN,GPIO_PIN_SET);
  initDisplay();
  initRoverGame();
  //writeRectangle(0,0,128, 160, DISP_AQUA);
  initADC();
  InitializeObjDisp(6);
  //Initialize RNG:
  initRNG();
  
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    if(flag3MsPassed){
      flag3MsPassed=false;
      //Every 3 ms Task:
            
    }
    if(flag10MsPassed){
      flag10MsPassed=false;
      //Every 10 ms Task:
      
      RunGamePlay();
//      writeRectangle(leftTopCorner[0],leftTopCorner[1],12,1,DISP_WHITE);
//      INCRE_CIRC_COUNTER(leftTopCorner[1],160-11);
      
//      testObject(angle, objType);
//          writeRectangle(leftTopCorner[0],leftTopCorner[1],12,1,DISP_WHITE);
//          leftTopCorner[1]++;
//          leftTopCorner[1]++;
//          leftTopCorner[1]++;
//          testObject(angle, objType); 
      
      
                
    }
    if(flag25MsPassed){
      
      flag25MsPassed=false;
      //Every 25 ms Task:
      runJoystick(joystickVal);
      DisplayScore();
      
      
//      timeDisplayMS=10;
//      HAL_ADC_Start(&hadc1);
//      
//      
//      while(((hadc1.Instance->ISR >>2) & 0x1) == 0){
//      }
//      joystickX=HAL_ADC_GetValue(&hadc1);
//      while(((hadc1.Instance->ISR >>2) & 0x1) == 0){
//      }
//      joystickY=HAL_ADC_GetValue(&hadc1);  
//      joystickY=HAL_ADC_GetValue(&hadc1);


     
                
    }

    if(flag100MsPassed){
      flag100MsPassed=false;
      
      //Every 100 ms Task:
      
      
      
//      writeRectangle(leftTopCorner[0],leftTopCorner[1],12,1,DISP_WHITE);
//      leftTopCorner[1]++;
//      testObject(angle, objType);
//      SetCarArr();
//      writeRectangle(prevPlace[0],prevPlace[1],12, 12, rgbColor32);
//      setColumnRowRange(leftTopCorner[0],leftTopCorner[0]+11,leftTopCorner[1],leftTopCorner[1]+11);      
//      writeColorArray(carRectColor,144);
//      
      
      
//      prevPlace[0]=leftTopCorner[0];
//      prevPlace[1]=leftTopCorner[1];
                
    }
    if(flag500MsPassed){
      flag500MsPassed=false;
      //Every 500 ms Task:
      
      
                
    }
    if(flag1000MsPassed){
      flag1000MsPassed=false;
      //Every 1000 ms Task:
      ranNum=getRandNum(20,163);
      roverTime--;
      DisplayTime();
      
//      leftTopCorner[1]++;
//
//      if(leftTopCorner[1]>=147)
//        leftTopCorner[1]=0;
      
//      setColumnRowRange(80,80+14,80 ,80+9);
//      SetSpiderArr();
//      writeColorArray(spiderRectColor,150);
      
    
        
               
    }
    
    if(setDisplay){
      timeDisplayMS=0;
      runningTrue=true;
      setDisplay=false;
      for(uint16_t i = 0; i<1000;i++){
//          writeRectangle(leftTopCorner[0],leftTopCorner[1],12,12,DISP_WHITE);
//          HAL_SPI_Transmit(&hspi1,line1,10,1000);
//        writeRectangle(leftTopCorner[0],leftTopCorner[1],12,1,DISP_WHITE);
//        writeRectangle(leftTopCorner[0]-3,leftTopCorner[1],1,12,DISP_WHITE);
//        leftTopCorner[1]++;
//        testObject(angle, objType);
      }
      
//      testObject(angle, objType);
      
//      writeRectangle(prevPlace[0],prevPlace[1],12, 12, rgbColor32);
//      setColumnRowRange(leftTopCorner[0],leftTopCorner[0]+11,leftTopCorner[1],leftTopCorner[1]+11);
//      prevPlace[0]=leftTopCorner[0];
//      prevPlace[1]=leftTopCorner[1];
//      SetCarArr();
//      writeColorArray(carRectColor,144);
      
      
//      writeRectangle(0,0,128, 160, rgbColor32);
//      setColumnRowRange(leftTopCorner[0],leftTopCorner[0]+11,leftTopCorner[1],leftTopCorner[1]+11);
//      SetCarArr();
//      writeColorArray(carRectColor,144);
//      
//      drawLine((int16_t) line1[0],(int16_t) line1[1],(int16_t) line1[2],(int16_t) line1[3],DISP_PURPLE);
//      
//      writeRectangle(leftTopCorner[0],leftTopCorner[1],width, height, rgbColor32);
//      writePixel(leftTopCorner[0],leftTopCorner[1], 255<<16);
      
//      setColumnRowRange(colStart, colEnd, rowStart, rowEnd);
//      setColor(rgbColor[0], rgbColor[1], rgbColor[2], (1+colEnd-colStart)*(1+rowEnd-rowStart));
      timeDisp = timeDisplayMS;
      runningTrue=false;
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration 
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB3 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void initRNG(){
  //Reset RNG:
  RCC->AHB2RSTR|=0x1<<18;
  RCC->AHB2RSTR&=~(0x1<<18);
  
  //Enable PLL48M2 Clock:
  RCC->PLLSAI1CFGR|=0x1<<20;
  
  //Select PLL48M2 Clock as clock source for CLK48 (RNG)
  RCC->CCIPR&=~(0x3<<26);
  RCC->CCIPR|=(0x3<<26);
  //Enable RNG Clock:
  RCC->AHB2ENR|=0x1<<18;
  RCC->AHB2SMENR|=0x1<<18; //Enable clock even in sleep/stop mode
  
  HAL_Delay(2);//Wait to access RNG again
  //Enable Clock detection, diable interrupt and RNG
  RNG->CR&=~(0x2C);
}

void testObject(uint16_t angle, ObjectDispType objType){
  obj1.angle=angle;
//  obj1.objType =objType;
//  obj1.active=true;
//  allObjects[0]=obj1;
//    
//  InitializeObjDisp(1);
//  SetObjectColor(objPixelSpace, &allObjects[0]);
//  uint8_t x = 5;//leftTopCorner[0];
//  uint8_t y = 1;//leftTopCorner[1];
//  setColumnRowRange(x,x+allObjects[0].width-1,y ,y+allObjects[0].height-1);
//  writeColorArray(objPixelSpace,allObjects[0].width*allObjects[0].height);
  
  
//  printMatrix(objPixelSpace,obj1.height,obj1.width);

}


uint32_t returnAnalog(uint8_t pin){
//  ADC_ChannelConfTypeDef sConfig = {0};
//  sConfig.Channel = ADC_CHANNEL_9*pin+(pin==0)*ADC_CHANNEL_10;
//  sConfig.Rank = ADC_REGULAR_RANK_1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
//  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//  sConfig.Offset = 0;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
//    Error_Handler();
//  }
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 1000);
  HAL_ADC_PollForConversion(&hadc1, 1000);
  
  return HAL_ADC_GetValue(&hadc1);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  timeDisp=10;
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
