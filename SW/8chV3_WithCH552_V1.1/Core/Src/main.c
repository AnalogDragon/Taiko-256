/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "save.h"


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

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM14_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
static void MX_USART1_UART_Init246(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


uint8_t OUT_BUFFER_LEN_D = 2;
uint8_t OUT_BUFFER_LEN_K = 2;

uint8_t with_feq_limit = 0;        //输出频率限制
uint8_t with_input_balance = 0;    //允许咚交叉输入
uint8_t this_is_12ASIA = 0;       //12亚专用

uint8_t output_count[8] = {0};  //输出缓存

uint8_t kb_lmt_count[8] = {0};  //输出冷却倒计


#define ADC_BUFFER_SIZE     10
#define ADC_LIST_SIZE       6
#define ADC_CHANNEL_NUM     8
#define CHANNEL_NUM     		8

uint8_t POWER_ON_EN = 0;


uint8_t KEY_TEST = 0;
uint8_t KEY_SERVICE = 0;
uint8_t KEY_COIN = 0;
uint8_t KEY_UP_SET = 0;
uint8_t KEY_DN_SET = 0;
uint8_t KEY_ENTER = 0;
uint8_t COIN_LOCK = 0;



uint16_t P1_KL = 0;
uint16_t P1_DL = 0;
uint16_t P1_DR = 0;
uint16_t P1_KR = 0;

uint16_t P2_KL = 0;
uint16_t P2_DL = 0;
uint16_t P2_DR = 0;
uint16_t P2_KR = 0;

uint8_t UnknowCMD = 0;
uint8_t JVS_OK = 0;


//
void GPIO_Output(uint8_t IO, uint8_t state){
  switch(IO){
    case 0:if(state)OUT1_GPIO_Port->BSRR = (uint32_t)OUT1_Pin;else OUT1_GPIO_Port->BRR = (uint32_t)OUT1_Pin;break;
    case 1:if(state)OUT2_GPIO_Port->BSRR = (uint32_t)OUT2_Pin;else OUT2_GPIO_Port->BRR = (uint32_t)OUT2_Pin;break;
    case 2:if(state)OUT3_GPIO_Port->BSRR = (uint32_t)OUT3_Pin;else OUT3_GPIO_Port->BRR = (uint32_t)OUT3_Pin;break;
    case 3:if(state)OUT4_GPIO_Port->BSRR = (uint32_t)OUT4_Pin;else OUT4_GPIO_Port->BRR = (uint32_t)OUT4_Pin;break;
    case 4:if(state)OUT5_GPIO_Port->BSRR = (uint32_t)OUT5_Pin;else OUT5_GPIO_Port->BRR = (uint32_t)OUT5_Pin;break;
    case 5:if(state)OUT6_GPIO_Port->BSRR = (uint32_t)OUT6_Pin;else OUT6_GPIO_Port->BRR = (uint32_t)OUT6_Pin;break;
    case 6:if(state)OUT7_GPIO_Port->BSRR = (uint32_t)OUT7_Pin;else OUT7_GPIO_Port->BRR = (uint32_t)OUT7_Pin;break;
    case 7:if(state)OUT8_GPIO_Port->BSRR = (uint32_t)OUT8_Pin;else OUT8_GPIO_Port->BRR = (uint32_t)OUT8_Pin;break;
  }
}

volatile uint16_t I_Buffer = 0;
volatile uint16_t O_Buffer = 0;
volatile uint16_t O_CountDown[CHANNEL_NUM] = {0};

volatile uint16_t ADC_Buffer[ADC_BUFFER_SIZE][ADC_CHANNEL_NUM];
volatile uint16_t ADC_List[ADC_CHANNEL_NUM][ADC_LIST_SIZE];
volatile uint16_t ADC_Now[ADC_CHANNEL_NUM];

volatile uint16_t ADC_Zero[ADC_CHANNEL_NUM] = {0};

volatile uint8_t InputCount[CHANNEL_NUM] = {0};
volatile uint16_t InputLevel_MAX[CHANNEL_NUM] = {0};
volatile uint16_t InputLevel_Now[CHANNEL_NUM] = {0};


uint8_t SystemError = 0;


uint8_t SET_1 = 0;
uint8_t SET_2 = 0;
uint8_t SET_3 = 0;
uint8_t SET_4 = 0;

#if (IS_S256 == 1)

#define KB_LMT_COUNT  32
#define KB_LMT_TRIG   (KB_LMT_COUNT - 10)

#else

#define KB_LMT_COUNT  32
#define KB_LMT_TRIG   (KB_LMT_COUNT - 1)

#endif

//输出函数
void IO_OutputList(void){
	static uint8_t flag[8] = {0};
	
	if(with_feq_limit){
		
		
		//老的函数用来获取敲击数
		for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
			if(O_Buffer & (1 << i)){
				
				if(POWER_ON_EN){
					if(flag[i] == 0){
						flag[i] = 1;
					}
					else flag[i] = 0xFF;
				}
				
				O_CountDown[i]--;
				
				if(O_CountDown[i] == 0){
					O_Buffer &= ~(1 << i);
					I_Buffer &= ~(1 << i);
				}
			}
			else{
				O_CountDown[i] = 0;
				flag[i] = 0;
			}
		}
		
    //计数器
		if(flag[0] == 1 && output_count[0] < OUT_BUFFER_LEN_K)output_count[0]++;
		if(flag[1] == 1 && output_count[1] < OUT_BUFFER_LEN_D)output_count[1]++;
		if(flag[2] == 1 && output_count[2] < OUT_BUFFER_LEN_D)output_count[2]++;
		if(flag[3] == 1 && output_count[3] < OUT_BUFFER_LEN_K)output_count[3]++;
		if(flag[4] == 1 && output_count[4] < OUT_BUFFER_LEN_K)output_count[4]++;
		if(flag[5] == 1 && output_count[5] < OUT_BUFFER_LEN_D)output_count[5]++;
		if(flag[6] == 1 && output_count[6] < OUT_BUFFER_LEN_D)output_count[6]++;
		if(flag[7] == 1 && output_count[7] < OUT_BUFFER_LEN_K)output_count[7]++;

    //修改这个函数为960Hz，则每32个周期为30Hz
		//展开效率高一丢丢

    //1P
    //1 = LK
    if(output_count[0] && kb_lmt_count[0] == 0){
      output_count[0]--;
			P1_KL = 1023;
      kb_lmt_count[0] = KB_LMT_COUNT;
      if(kb_lmt_count[3] == 0)kb_lmt_count[3] = KB_LMT_COUNT/2;
    }
    //4 = RK
    if(output_count[3] && kb_lmt_count[3] == 0){
      output_count[3]--;
			P1_KR = 1023;
      kb_lmt_count[3] = KB_LMT_COUNT;
      if(kb_lmt_count[0] == 0)kb_lmt_count[0] = KB_LMT_COUNT/2;
    }
    //2 = LD
    if(output_count[1] && kb_lmt_count[1] == 0){
      output_count[1]--;
			P1_DL = 1023;
      kb_lmt_count[1] = KB_LMT_COUNT;
      if(kb_lmt_count[2] == 0)kb_lmt_count[2] = KB_LMT_COUNT/2;
    }
    //3 = RD
    if(output_count[2] && kb_lmt_count[2] == 0){
      output_count[2]--;
			P1_DR = 1023;
      kb_lmt_count[2] = KB_LMT_COUNT;
      if(kb_lmt_count[1] == 0)kb_lmt_count[1] = KB_LMT_COUNT/2;
    }


    //2P
    //1 = LK
    if(output_count[4] && kb_lmt_count[4] == 0){
      output_count[4]--;
			P2_KL = 1023;
      kb_lmt_count[4] = KB_LMT_COUNT;
      if(kb_lmt_count[7] == 0)kb_lmt_count[7] = KB_LMT_COUNT/2;
    }
    //4 = RK
    if(output_count[7] && kb_lmt_count[7] == 0){
      output_count[7]--;
			P2_KR = 1023;
      kb_lmt_count[7] = KB_LMT_COUNT;
      if(kb_lmt_count[4] == 0)kb_lmt_count[4] = KB_LMT_COUNT/2;
    }
    //2 = LD
    if(output_count[5] && kb_lmt_count[5] == 0){
      output_count[5]--;
			P2_DL = 1023;
      kb_lmt_count[5] = KB_LMT_COUNT;
      if(kb_lmt_count[6] == 0)kb_lmt_count[6] = KB_LMT_COUNT/2;
    }
    //3 = RD
    if(output_count[6] && kb_lmt_count[6] == 0){
      output_count[6]--;
			P2_DR = 1023;
      kb_lmt_count[6] = KB_LMT_COUNT;
      if(kb_lmt_count[5] == 0)kb_lmt_count[5] = KB_LMT_COUNT/2;
    }


    if(with_input_balance){
      //交叉的部分
      //1P
      if(output_count[1] && kb_lmt_count[2] == 0){
        output_count[1]--;
				P1_DR = 1023;
        kb_lmt_count[2] = KB_LMT_COUNT;
        if(kb_lmt_count[1] == 0)kb_lmt_count[1] = KB_LMT_COUNT/2;
      }
      if(output_count[2] && kb_lmt_count[1] == 0){
        output_count[2]--;
				P1_DL = 1023;
        kb_lmt_count[1] = KB_LMT_COUNT;
        if(kb_lmt_count[2] == 0)kb_lmt_count[2] = KB_LMT_COUNT/2;
      }
      //2P
      if(output_count[5] && kb_lmt_count[6] == 0){
        output_count[5]--;
				P2_DR = 1023;
        kb_lmt_count[6] = KB_LMT_COUNT;
        if(kb_lmt_count[5] == 0)kb_lmt_count[5] = KB_LMT_COUNT/2;
      }
      if(output_count[6] && kb_lmt_count[5] == 0){
        output_count[6]--;
				P2_DL = 1023;
        kb_lmt_count[5] = KB_LMT_COUNT;
        if(kb_lmt_count[6] == 0)kb_lmt_count[6] = KB_LMT_COUNT/2;
      }
    }

    //输出
    GPIO_Output(0, kb_lmt_count[0] > KB_LMT_TRIG);
    GPIO_Output(1, kb_lmt_count[1] > KB_LMT_TRIG);
    GPIO_Output(2, kb_lmt_count[2] > KB_LMT_TRIG);
    GPIO_Output(3, kb_lmt_count[3] > KB_LMT_TRIG);
    GPIO_Output(4, kb_lmt_count[4] > KB_LMT_TRIG);
    GPIO_Output(5, kb_lmt_count[5] > KB_LMT_TRIG);
    GPIO_Output(6, kb_lmt_count[6] > KB_LMT_TRIG);
    GPIO_Output(7, kb_lmt_count[7] > KB_LMT_TRIG);
    
    //count down
    if(kb_lmt_count[0])kb_lmt_count[0]--;
    if(kb_lmt_count[1])kb_lmt_count[1]--;
    if(kb_lmt_count[2])kb_lmt_count[2]--;
    if(kb_lmt_count[3])kb_lmt_count[3]--;
    if(kb_lmt_count[4])kb_lmt_count[4]--;
    if(kb_lmt_count[5])kb_lmt_count[5]--;
    if(kb_lmt_count[6])kb_lmt_count[6]--;
    if(kb_lmt_count[7])kb_lmt_count[7]--;

		return;
	}
 
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    if(O_Buffer & (1 << i)){
			
      if(POWER_ON_EN)
        GPIO_Output(i, SystemSet.OutLoLength < O_CountDown[i]);
			
      O_CountDown[i]--;
			
      if(O_CountDown[i] == 0){
        O_Buffer &= ~(1 << i);
				I_Buffer &= ~(1 << i);
      }
    }
    else{
      O_CountDown[i] = 0;
      GPIO_Output(i, 0);
    }
  }
}


//清除操作
void DoMute(uint8_t ID){
	I_Buffer &= ~(1 << ID);
	InputCount[ID] = 0;
	InputLevel_MAX[ID] = 0;
}

//操作输出
void DoOutput(uint8_t ID){
  
  UartDebug();
  
	//清除整组异端
  if(SystemSet.ADC_Mute_Side){
    
    if(ID > 3){
      DoMute(4);
      DoMute(5);
      DoMute(6);
      DoMute(7);
    }
    else{
      DoMute(0);
      DoMute(1);
      DoMute(2);
      DoMute(3);
    }
    
  }
  else{
    
    if(ID == 0 || ID == 3){
      DoMute(1);
      DoMute(2);
    }
    else if(ID == 1 || ID == 2){
      DoMute(0);
      DoMute(3);
    }
    else if(ID == 4 || ID == 7){
      DoMute(5);
      DoMute(6);
    }
    else if(ID == 5 || ID == 6){
      DoMute(4);
      DoMute(7);
    }
    else return;
    
    DoMute(ID);
    
  }
	
  //allow output
  O_Buffer |= 1 << ID;
  O_CountDown[ID] = SystemSet.OutHiLength + SystemSet.OutLoLength;
	//here need to make all channel mute
}


void REG_Output(uint8_t ID,uint16_t Level){
	
	//首次登记
	if(((I_Buffer>>ID)&1) == 0){
		I_Buffer |= 1<<ID;
		InputCount[ID] = 0;
	}
	
	//记录最大值
	if(Level > InputLevel_MAX[ID]){
		InputLevel_MAX[ID] = Level;
	}
	
}

//查找最大值，仅用于登记后
uint8_t CheckAdcMax(uint8_t ID, uint16_t* pValueList){
	
  uint16_t ADC_Value = pValueList[ID];
	
  if(SystemSet.ADC_Mute_Side){    
    
    switch(ID){
      //1P
      case 0:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 1:
        if(
          ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 2:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 3:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[0]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
        //2P
      case 4:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 5:
        if(
          ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 6:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 7:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[4]
          ){
          return 0;
        }
        else{
          return 1;
        }
    }

  }
  
  else{    
    
    switch(ID){
      //1P
      case 0:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 1:
        if(
          ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 2:
        if(
         ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 3:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
        //2P
      case 4:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 5:
        if(
          ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 6:
        if(
         ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 7:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
          ){
          return 0;
        }
        else{
          return 1;
        }
    }

  }
	
	return 0;
  
}


/*
原理：收到自己被触发后，等待设置时间到
在时间段内有高于自己值的就屏蔽自己
*/

void CalPxMute_V2(uint8_t ID){
	
	//查找组内最大是否为本通道
	if(CheckAdcMax(ID, (uint16_t*)InputLevel_MAX)){
		DoOutput(ID);
	}
	else{
		DoMute(ID);
	}
	
}


#define AUTO_ZERO_LEN		2048

void AutoZero(void){
	static uint32_t ADC_ADD[CHANNEL_NUM] = {0}; 
	static uint16_t Count = 0;
	 
	if(I_Buffer){
		memset(ADC_ADD, 0, sizeof(ADC_ADD));
		Count = 0;
	}
	else{
		for(uint8_t i=0; i<CHANNEL_NUM; i++){
			ADC_ADD[i] += ADC_Now[i];
		}
		Count++;
		
		if(Count >= AUTO_ZERO_LEN){
			for(uint8_t i=0; i<CHANNEL_NUM; i++){
				ADC_Zero[i] = ADC_ADD[i] / Count;
			}
			memset(ADC_ADD, 0, sizeof(ADC_ADD));
			Count = 0;
      if(POWER_ON_EN == 0){
        memset((uint8_t*)InputLevel_MAX, 0, sizeof(InputLevel_MAX));
        POWER_ON_EN = 1;
      }
		}
	}
}


 
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
  uint32_t temp;
	
	//calc adc value
	for(uint8_t i=0;i<CHANNEL_NUM;i++){

		//move list
		for(uint8_t j=0;j < (ADC_LIST_SIZE - 1);j++){
			ADC_List[i][(ADC_LIST_SIZE - 1) - j] = ADC_List[i][(ADC_LIST_SIZE - 1) - j - 1];
		}
	
		//cal avg
		temp = 0;
		for(uint8_t j=0;j<ADC_BUFFER_SIZE;j++){
			temp += ADC_Buffer[j][SystemSet.ADC_ChannelRemap[i]];
		}
		ADC_Now[i] = (uint16_t)(temp * SystemSet.ADC_Mul[i] / ADC_BUFFER_SIZE / ADC_DIV_DEF);
		
		//Set Zero
		if(ADC_Zero[i] < ADC_Now[i])
			ADC_List[i][0] = ADC_Now[i] - ADC_Zero[i];
		else
			ADC_List[i][0] = 0;
		InputLevel_Now[i] = ADC_List[i][0];
		
	}
  
	
	//cale trigger
  if(SystemSet.MuteEnable == DISABLE){
		//without mute func
    
    for(uint8_t i=0;i<CHANNEL_NUM;i++){
      
      //cal trigger
      for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
        if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){
          
          //REG OUTPUT
          I_Buffer |= 1<<i;
          if(((O_Buffer>>i)&1) == 0){
						//这里注册输出
						UartDebug();
            O_CountDown[i] = SystemSet.OutHiLength + SystemSet.OutLoLength;
            O_Buffer |= 1 << i;
          }
          break;
        }
      }
    }
  }
	
	else if(SystemSet.ADC_WaitMuteTime == 0){
		//mute and time first
		uint16_t max_dt = 0;
		uint8_t max_id = 0;
		
		if(O_Buffer == 0){
			for(uint8_t i=0;i<CHANNEL_NUM;i++){
				for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
					if(ADC_List[i][0] > ADC_List[i][j+1]){
						if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){	//do trigger
							
							if(max_dt < ADC_List[i][0] - ADC_List[i][j+1]){
								max_dt = ADC_List[i][0] - ADC_List[i][j+1];
								max_id = i;
							}
						}
					}
				}
			}
		}
		if(max_dt != 0){
			DoOutput(max_id);
		}
		
		//end of time first
	}
	
	else{
		//With Mute
		
		for(uint8_t i=0;i<CHANNEL_NUM;i++){
			//cal trigger
			if((I_Buffer>>i) & 1){	//This channel has been trigged , GetMax
					REG_Output(i, ADC_List[i][0]);
			}
			else{
				//寻找高脉冲边缘
				for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
					
					if(ADC_List[i][0] > ADC_List[i][j+1]){
						if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){//do trigger
							
							//REG OUTPUT
							if(CheckAdcMax(i,(uint16_t*)InputLevel_Now))
								if(((O_Buffer>>i) & 1) == 0)  //This channel no output Now
									REG_Output(i, ADC_List[i][0]);//GetMax
								
						}
					}
					
				}
			}
		}
		
		//calculate output
		for(uint8_t i=0;i<CHANNEL_NUM;i++){
			if((I_Buffer >> i) & 1){
				InputCount[i]++;
				if(InputCount[i] >= SystemSet.ADC_WaitMuteTime){
					//time out
					CalPxMute_V2(i);
				}
			}
		}
		
		//end of mute
		
	}
	
	AutoZero();
	
  
}

 
void ReadSet(void){
  SET_1 = HAL_GPIO_ReadPin(SET1_GPIO_Port,SET1_Pin) == 0; //Debug EN
  SET_2 = HAL_GPIO_ReadPin(SET2_GPIO_Port,SET2_Pin) == 0; //output limit
  SET_3 = HAL_GPIO_ReadPin(SET3_GPIO_Port,SET3_Pin) == 0; //output banlace
  SET_4 = HAL_GPIO_ReadPin(SET4_GPIO_Port,SET4_Pin) == 0; //12ASIA
}


//检测配置开关变化后，就重启ic
void checkSet(void){
	if(SET_1 != (HAL_GPIO_ReadPin(SET1_GPIO_Port,SET1_Pin) == 0)){
		HAL_NVIC_SystemReset();
	}
	if(SET_2 != (HAL_GPIO_ReadPin(SET2_GPIO_Port,SET2_Pin) == 0)){
		HAL_NVIC_SystemReset();
	}
	if(SET_3 != (HAL_GPIO_ReadPin(SET3_GPIO_Port,SET3_Pin) == 0)){
		HAL_NVIC_SystemReset();
	}
	if(SET_4 != (HAL_GPIO_ReadPin(SET4_GPIO_Port,SET4_Pin) == 0)){
		HAL_NVIC_SystemReset();
	}
}


uint8_t SciBuffer[260];
uint16_t SciLength = 0;
uint8_t SciTime = 0;

uint8_t SciCount = 0;
uint8_t SciErrTime = 0;

void UsartStartRec(void){
  HAL_UART_AbortReceive_IT(&huart1);
  HAL_UART_Receive_IT(&huart1, SciBuffer, sizeof(SciBuffer));
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
}


uint8_t SciCheck(uint8_t* data, uint16_t len){
  if(len < 3)return 1;
  uint16_t CRCData1;
  uint16_t CRCData2;
  
  CRCData1 = CRCCheck(data, len-2);
  CRCData2 = data[len-1] | (data[len-2]<<8);
  
  if(CRCData1 == CRCData2)return 0;
  return 1;
}



void GetSciData(void){
  
  SciCount++;
	
  if(0 == SciCheck(SciBuffer,SciLength)){
    //check data cplt
    //do cmd
    SciDoCMD(SciBuffer, SciLength);
  }
	
}


uint8_t CloseIrqAndSave(void){
  uint8_t temp;
	
  HAL_TIM_Base_Stop_IT(&htim14);
  HAL_ADC_Stop_DMA(&hadc1);
	HAL_Delay(5);
	
  temp = DoSave();
	
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Buffer,ADC_BUFFER_SIZE*ADC_CHANNEL_NUM);
	
  return temp;
}

//改为软件断帧
#define UART_FREAM_TIME		2

void UsartTask(void){
  static uint16_t SciLength_buf = 0xFFFF;
  
  if(SciLength){
    
    if(SciLength_buf != SciLength){
      SciLength_buf = SciLength;
      SciTime = 0;
    }
    else{
      SciTime ++;
      if(SciTime > UART_FREAM_TIME){
				
				if(SET_4){
				}
				else if(SET_1){
					GetSciData();
				}
				SciLength_buf = 0xFFFF;
				SciLength = 0;
        SciTime = 0;
      }
    }
  }
  
}

uint8_t SciBufferTx[260];

void SciSendSucc(uint8_t* data, uint8_t size){
	uint8_t SUM = 0;
//	return;
	
	DE_GPIO_Port->BSRR = (uint32_t)DE_Pin;
	
	SciBufferTx[0] = 0xE0;
	SciBufferTx[1] = 0x00;
	SciBufferTx[2] = size+3;
	SciBufferTx[3] = STATUS_SUCCESS;
	SciBufferTx[4] = REPORT_SUCCESS;
	if(size)memcpy(&SciBufferTx[5],data,size);
	for(uint16_t i=1;i<SciBufferTx[2]+2;i++){
		SUM += SciBufferTx[i];
	}
	SciBufferTx[size + 5] = SUM;
	
	DE_GPIO_Port->BSRR = (uint32_t)DE_Pin;
	
	HAL_UART_Transmit_DMA(&huart1, SciBufferTx,size + 6);
}

void SciSendFail(void){
	
	DE_GPIO_Port->BSRR = (uint32_t)DE_Pin;
	
	SciBufferTx[0] = 0xE0;
	SciBufferTx[1] = 0x00;
	SciBufferTx[2] = 2;//size
	SciBufferTx[3] = STATUS_UNSUPPORTED;
	SciBufferTx[4] = STATUS_UNSUPPORTED + 2;
	DE_GPIO_Port->BSRR = (uint32_t)DE_Pin;
	
	HAL_UART_Transmit_DMA(&huart1, SciBufferTx,5);
}

const uint8_t ACK_ID[] = "namco ltd.;MINI-JV;Ver1.00;JPN,Multipurpose]\n\n!!F**K namco!!\n[Analog Dragon";
const uint8_t ACK_CAL[21] = {0x01,0x01,0x10,0x00,0x02,0x01,0x00,0x00,0x03,0x08,0x10,0x00,0x07,0x00,0x08,0x00,0x12,0x08,0x00,0x00,0x00};

	
uint8_t ACK_SW[24] = {0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
uint8_t ACK_SW_Buf[24];

void Get246Input(void){
	
	memcpy(ACK_SW_Buf,ACK_SW,sizeof(ACK_SW_Buf));
	
	if(KEY_TEST){
		ACK_SW_Buf[0] |= 0x80;
	}
	if(KEY_SERVICE){
		ACK_SW_Buf[1] |= 0x40;
	}
	if(KEY_DN_SET){
		ACK_SW_Buf[1] |= 0x10;
	}
	if(KEY_UP_SET){
		ACK_SW_Buf[1] |= 0x20;
	}
	if(KEY_ENTER){
		ACK_SW_Buf[1] |= 0x02;
	}
	if(KEY_COIN){
		if(!COIN_LOCK)
			ACK_SW_Buf[5] |= 0x01;
	}
	else{
		COIN_LOCK = 0;
	}
	
	if(P1_KL > 1023)P1_KL = 1023;
	if(P1_DL > 1023)P1_DL = 1023;
	if(P1_DR > 1023)P1_DR = 1023;
	if(P1_KR > 1023)P1_KR = 1023;
	
	if(P2_KL > 1023)P2_KL = 1023;
	if(P2_DL > 1023)P2_DL = 1023;
	if(P2_DR > 1023)P2_DR = 1023;
	if(P2_KR > 1023)P2_KR = 1023;
	
	ACK_SW_Buf[7] = (P1_DL>>2) & 0xFF;
	ACK_SW_Buf[8] = (P1_DL<<6) & 0xFF;
	
	ACK_SW_Buf[9] = (P2_KL>>2) & 0xFF;
	ACK_SW_Buf[10] = (P2_KL<<6) & 0xFF;
	
	ACK_SW_Buf[11] = (P2_DL>>2) & 0xFF;
	ACK_SW_Buf[12] = (P2_DL<<6) & 0xFF;
	
	ACK_SW_Buf[13] = (P1_DR>>2) & 0xFF;
	ACK_SW_Buf[14] = (P1_DR<<6) & 0xFF;
	
	ACK_SW_Buf[15] = (P1_KR>>2) & 0xFF;
	ACK_SW_Buf[16] = (P1_KR<<6) & 0xFF;
	
	ACK_SW_Buf[17] = (P1_KL>>2) & 0xFF;
	ACK_SW_Buf[18] = (P1_KL<<6) & 0xFF;
	
	ACK_SW_Buf[19] = (P2_KR>>2) & 0xFF;
	ACK_SW_Buf[20] = (P2_KR<<6) & 0xFF;
	
	ACK_SW_Buf[21] = (P2_DR>>2) & 0xFF;
	ACK_SW_Buf[22] = (P2_DR<<6) & 0xFF;
	
	if(P1_KL)P1_KL = P1_KL>>1;
	if(P1_DL)P1_DL = P1_DL>>1;
	if(P1_DR)P1_DR = P1_DR>>1;
	if(P1_KR)P1_KR = P1_KR>>1;
	if(P2_KL)P2_KL = P2_KL>>1;
	if(P2_DL)P2_DL = P2_DL>>1;
	if(P2_DR)P2_DR = P2_DR>>1;
	if(P2_KR)P2_KR = P2_KR>>1;
	
}

void Get246Data(void)
{
	if(!SET_4)return;
	
	if(SciLength){
		if(SciBuffer[0] != SYNC && SciBuffer[0] != ESCAPE){
			SciLength = 0;
		}
	}
	
	if(SciLength >= 5 && SciLength >= (SciBuffer[2]+3) && SciBuffer[0] == SYNC){
		//getdata
		uint8_t SUM = 0;
		for(uint16_t i=1;i<SciBuffer[2]+2;i++){
			SUM += SciBuffer[i];
		}
		if(SUM == SciBuffer[SciLength-1]){
			SciCount++;
			JVS_OK = 250;
			
			if(SciBuffer[1] == 0xFF){//	BROADCAST 
				switch(SciBuffer[3]){
					case 0xF0: //RESET
						break;
					case 0xF1: //CMD_ASSIGN_ADDR 
						if(SciLength == 6 && SciBuffer[4] == 1){
							SENSE_GPIO_Port->BSRR = (uint32_t)SENSE_Pin;
							SciSendSucc(0,0);
						}
						break;
				}
			}
			else if(SciBuffer[1] == 0x01){//address = 01
				switch(SciBuffer[3]){
					case CMD_REQUEST_ID:
						SciSendSucc((uint8_t*)ACK_ID,sizeof(ACK_ID));
						break;
					case CMD_COMMAND_VERSION:
						SciSendSucc((uint8_t*)"\x13\x01\x30\x01\x10",5);
						break;
					case CMD_CAPABILITIES:
						SciSendSucc((uint8_t*)ACK_CAL,sizeof(ACK_CAL));
						break;
					case CMD_CONVEY_ID:
						SciSendSucc(0,0);
						break;
					case CMD_READ_SWITCHES:
						Get246Input();
						SciSendSucc(ACK_SW_Buf,sizeof(ACK_SW_Buf));
						break;
					case CMD_DECREASE_COINS:
						SciSendSucc((uint8_t*)"\x01\0\x01",3);
						COIN_LOCK = 1;
						break;
					case CMD_NAMCO_SPECIFIC:
						SciSendSucc((uint8_t*)'\x01',1);
						break;
					
					//下面的都是从OPEN JVS里抄的
					case CMD_JVS_VERSION:
						SciSendSucc((uint8_t*)'\x10',1);
						break;
					case CMD_READ_KEYPAD:
						SciSendSucc((uint8_t*)'\0',1);
						break;
					case CMD_REMAINING_PAYOUT:
						SciSendSucc((uint8_t*)"\0\0\0\0",4);
						break;
					case CMD_READ_LIGHTGUN:
						SciSendSucc((uint8_t*)"\0\0\0\0",4);
						break;
					
					case CMD_READ_COINS:
					case CMD_READ_ANALOGS:
					case CMD_READ_ROTARY:
					case CMD_READ_GPI:
						UnknowCMD = 1;
						SciSendFail();
						break;
					
					default:
						UnknowCMD = 1;
						SciSendSucc(0,0);
				}
			}
		}
		SciLength = 0; //clear rec
	}
}


//开机动画
void ON_Play(void){
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    GPIO_Output(i, 1);
    HAL_Delay(30);
  }
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    GPIO_Output(i, 0);
    HAL_Delay(30);
  }
}


void ReadServiceKey(void){
	KEY_TEST = HAL_GPIO_ReadPin(KEY_TEST_GPIO_Port,KEY_TEST_Pin) == GPIO_PIN_RESET;
	KEY_SERVICE = HAL_GPIO_ReadPin(KEY_SERVICE_GPIO_Port,KEY_SERVICE_Pin) == GPIO_PIN_RESET;
	KEY_COIN = HAL_GPIO_ReadPin(KEY_COIN1_GPIO_Port,KEY_COIN1_Pin) == GPIO_PIN_RESET;
	KEY_UP_SET = HAL_GPIO_ReadPin(KEY_SEL_UP_GPIO_Port,KEY_SEL_UP_Pin) == GPIO_PIN_RESET;
	KEY_DN_SET = HAL_GPIO_ReadPin(KEY_SEL_DOWN_GPIO_Port,KEY_SEL_DOWN_Pin) == GPIO_PIN_RESET;
	KEY_ENTER = HAL_GPIO_ReadPin(KEY_ENTER_GPIO_Port,KEY_ENTER_Pin) == GPIO_PIN_RESET;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t time = 0;
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
  MX_IWDG_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  
  HAL_FLASH_Lock();
	
  HAL_Delay(10);
  
  ReadSet();
	
	SaveInit();
	
  if(SET_2){
		with_feq_limit = 1;
  }
	
	
	if(SET_3){
		with_input_balance = 1;
    OUT_BUFFER_LEN_D = 4;
	}
	
  HAL_IWDG_Refresh(&hiwdg);
  ON_Play();
	
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Buffer,ADC_BUFFER_SIZE*ADC_CHANNEL_NUM);
  HAL_Delay(500);
	
	
	if(SET_4){
		MX_USART1_UART_Init246();//115200
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"Power On In 246 Mode\r\n",sizeof("Power On In 246 Mode\r\n")-1);
    UsartStartRec();
		this_is_12ASIA = 1;
	}
	else if(SET_1){//38400
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"Power On\r\n",sizeof("Power On\r\n")-1);
    UsartStartRec();
  }
	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    HAL_IWDG_Refresh(&hiwdg);
		UsartTask();
    
    time++;
		if(time >= 10000){
      time = 0;
			checkSet();
		}
		if(time%10 == 0){
			ReadServiceKey();
		}
		
		if(SystemError){
			HAL_GPIO_WritePin(LED_STA_GPIO_Port,LED_STA_Pin,(GPIO_PinState)(time < 2));
		}
		else if(UnknowCMD){
			if(time<500 && time % 10 == 0)
				HAL_GPIO_TogglePin(LED_STA_GPIO_Port,LED_STA_Pin);
		}
		else if(!JVS_OK && SET_4){
			if(time % 10 == 0)
				HAL_GPIO_TogglePin(LED_STA_GPIO_Port,LED_STA_Pin);
		}
		else if(POWER_ON_EN){
			if((time % 100) == 0)
				HAL_GPIO_TogglePin(LED_STA_GPIO_Port,LED_STA_Pin);
		}
		else{
        HAL_GPIO_WritePin(LED_STA_GPIO_Port,LED_STA_Pin,GPIO_PIN_SET);
		}
		
		if(JVS_OK)JVS_OK--;
    
    HAL_Delay(1);
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 8;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 33333;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */
//  htim14.Init.Period = 2 * OutListFerq - 1;
  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/* USER CODE BEGIN 4 */



/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init246(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
#if (IS_S256 == 1)
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STA_GPIO_Port, LED_STA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DE_GPIO_Port, DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SENSE_GPIO_Port, SENSE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT1_Pin OUT8_Pin OUT7_Pin OUT6_Pin
                           OUT5_Pin OUT4_Pin OUT3_Pin OUT2_Pin */
  GPIO_InitStruct.Pin = OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SET1-4_Pin */
  GPIO_InitStruct.Pin = SET3_Pin|SET4_Pin|SET2_Pin|SET1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_STA_Pin */
  GPIO_InitStruct.Pin = LED_STA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_STA_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DE_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SENSE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : service_Pin */
  GPIO_InitStruct.Pin = KEY_TEST_Pin|KEY_SERVICE_Pin|KEY_ENTER_Pin|KEY_SEL_UP_Pin|KEY_COIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = KEY_SEL_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY_SEL_DOWN_GPIO_Port, &GPIO_InitStruct);

}
#else
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STA_GPIO_Port, LED_STA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT1_Pin OUT8_Pin OUT7_Pin OUT6_Pin
                           OUT5_Pin OUT4_Pin OUT3_Pin OUT2_Pin */
  GPIO_InitStruct.Pin = OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SET3_Pin SET4_Pin */
  GPIO_InitStruct.Pin = SET3_Pin|SET4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SET2_Pin SET1_Pin */
  GPIO_InitStruct.Pin = SET2_Pin|SET1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_STA_Pin */
  GPIO_InitStruct.Pin = LED_STA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_STA_GPIO_Port, &GPIO_InitStruct);

}
#endif
/* USER CODE END 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
