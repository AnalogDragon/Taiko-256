/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"

#define IS_S256		1

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */


extern uint8_t SET_1;
extern uint8_t SET_2;
extern uint8_t SET_3;
extern uint8_t SET_4;


extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern IWDG_HandleTypeDef hiwdg;

extern TIM_HandleTypeDef htim14;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;


uint8_t CloseIrqAndSave(void);
void CloseIrqAndActive(uint8_t data);


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#if (IS_S256 == 1)

#define DE_Pin GPIO_PIN_8
#define DE_GPIO_Port GPIOA
#define SENSE_Pin GPIO_PIN_15
#define SENSE_GPIO_Port GPIOB
	
#define OUT1_Pin GPIO_PIN_14
#define OUT1_GPIO_Port GPIOB
#define OUT2_Pin GPIO_PIN_13
#define OUT2_GPIO_Port GPIOB
#define OUT3_Pin GPIO_PIN_12
#define OUT3_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_11
#define OUT4_GPIO_Port GPIOB
#define OUT5_Pin GPIO_PIN_10
#define OUT5_GPIO_Port GPIOB
#define OUT6_Pin GPIO_PIN_2
#define OUT6_GPIO_Port GPIOB
#define OUT7_Pin GPIO_PIN_1
#define OUT7_GPIO_Port GPIOB
#define OUT8_Pin GPIO_PIN_0
#define OUT8_GPIO_Port GPIOB

#define SET1_Pin GPIO_PIN_0
#define SET1_GPIO_Port GPIOD
#define SET2_Pin GPIO_PIN_1
#define SET2_GPIO_Port GPIOD
#define SET3_Pin GPIO_PIN_2
#define SET3_GPIO_Port GPIOD
#define SET4_Pin GPIO_PIN_3
#define SET4_GPIO_Port GPIOD

#define IN1_Pin GPIO_PIN_0
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_1
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN5_Pin GPIO_PIN_4
#define IN5_GPIO_Port GPIOA
#define IN6_Pin GPIO_PIN_5
#define IN6_GPIO_Port GPIOA
#define IN7_Pin GPIO_PIN_6
#define IN7_GPIO_Port GPIOA
#define IN8_Pin GPIO_PIN_7
#define IN8_GPIO_Port GPIOA

#define LED_STA_Pin GPIO_PIN_15
#define LED_STA_GPIO_Port GPIOA

#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA

#define KEY_TEST_Pin	GPIO_PIN_9
#define KEY_TEST_GPIO_Port	GPIOB
#define KEY_SERVICE_Pin	GPIO_PIN_8
#define KEY_SERVICE_GPIO_Port	GPIOB
#define KEY_ENTER_Pin	GPIO_PIN_7
#define KEY_ENTER_GPIO_Port	GPIOB
#define KEY_SEL_UP_Pin	GPIO_PIN_6
#define KEY_SEL_UP_GPIO_Port	GPIOB
#define KEY_COIN1_Pin	GPIO_PIN_5
#define KEY_COIN1_GPIO_Port	GPIOB
#define KEY_SEL_DOWN_Pin	GPIO_PIN_13
#define KEY_SEL_DOWN_GPIO_Port GPIOC

#else

#define OUT1_Pin GPIO_PIN_9
#define OUT1_GPIO_Port GPIOB
#define SET3_Pin GPIO_PIN_14
#define SET3_GPIO_Port GPIOC
#define SET4_Pin GPIO_PIN_15
#define SET4_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_0
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_1
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN5_Pin GPIO_PIN_4
#define IN5_GPIO_Port GPIOA
#define IN6_Pin GPIO_PIN_5
#define IN6_GPIO_Port GPIOA
#define IN7_Pin GPIO_PIN_6
#define IN7_GPIO_Port GPIOA
#define IN8_Pin GPIO_PIN_7
#define IN8_GPIO_Port GPIOA
#define SET2_Pin GPIO_PIN_0
#define SET2_GPIO_Port GPIOB
#define SET1_Pin GPIO_PIN_1
#define SET1_GPIO_Port GPIOB
#define OUT8_Pin GPIO_PIN_2
#define OUT8_GPIO_Port GPIOB
#define LED_STA_Pin GPIO_PIN_8
#define LED_STA_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define OUT7_Pin GPIO_PIN_3
#define OUT7_GPIO_Port GPIOB
#define OUT6_Pin GPIO_PIN_4
#define OUT6_GPIO_Port GPIOB
#define OUT5_Pin GPIO_PIN_5
#define OUT5_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_6
#define OUT4_GPIO_Port GPIOB
#define OUT3_Pin GPIO_PIN_7
#define OUT3_GPIO_Port GPIOB
#define OUT2_Pin GPIO_PIN_8
#define OUT2_GPIO_Port GPIOB

#endif
/* USER CODE BEGIN Private defines */



#define SYNC 0xE0
#define ESCAPE 0xD0
#define BROADCAST 0xFF
#define BUS_MASTER 0x00
#define DEVICE_ADDR_START 0x01

/* Status for the entire packet */
#define STATUS_SUCCESS 0x01
#define STATUS_UNSUPPORTED 0x02      // an unsupported command was sent
#define STATUS_CHECKSUM_FAILURE 0x03 // the checksum on the command packet did not match a computed checksum
#define STATUS_OVERFLOW 0x04         // an overflow occurred while processing the command

/* Reporting for each individual command */
#define REPORT_SUCCESS 0x01          // all went well
#define REPORT_PARAMETER_ERROR1 0x02 // TODO: work out difference between this one and the next
#define REPORT_PARAMETER_ERROR2 0x03
#define REPORT_BUSY 0x04 // some attached hardware was busy, causing the request to fail

/* All of the commands */
#define CMD_RESET 0xF0            // reset bus
#define CMD_RESET_ARG 0xD9        // fixed argument to reset command
#define CMD_ASSIGN_ADDR 0xF1      // assign address to slave
#define CMD_SET_COMMS_MODE 0xF2   // switch communications mode for devices that support it, for compatibility
#define CMD_REQUEST_ID 0x10       // requests an ID string from a device
#define CMD_COMMAND_VERSION 0x11  // gets command format version as two BCD digits packed in a byte
#define CMD_JVS_VERSION 0x12      // gets JVS version as two BCD digits packed in a byte
#define CMD_COMMS_VERSION 0x13    // gets communications version as two BCD digits packed in a byte
#define CMD_CAPABILITIES 0x14     // gets a special capability structure from the device
#define CMD_CONVEY_ID 0x15        // convey ID of main board to device
#define CMD_READ_SWITCHES 0x20    // read switch inputs
#define CMD_READ_COINS 0x21       // read coin inputs
#define CMD_READ_ANALOGS 0x22     // read analog inputs
#define CMD_READ_ROTARY 0x23      // read rotary encoder inputs
#define CMD_READ_KEYPAD 0x24      // read keypad inputs
#define CMD_READ_LIGHTGUN 0x25    // read light gun inputs
#define CMD_READ_GPI 0x26         // read general-purpose inputs
#define CMD_RETRANSMIT 0x2F       // ask device to retransmit data
#define CMD_DECREASE_COINS 0x30   // decrease number of coins
#define CMD_WRITE_GPO 0x32        // write to general-purpose outputs
#define CMD_WRITE_ANALOG 0x33     // write to analog outputs
#define CMD_WRITE_DISPLAY 0x34    // write to an alphanumeric display
#define CMD_WRITE_COINS 0x35      // add to coins
#define CMD_REMAINING_PAYOUT 0x2E // read remaining payout
#define CMD_SET_PAYOUT 0x31       // write remaining payout
#define CMD_SUBTRACT_PAYOUT 0x36  // subtract from remaining payout
#define CMD_WRITE_GPO_BYTE 0x37   // write single gpo byte
#define CMD_WRITE_GPO_BIT 0x38    // write single gpo bit

/* Manufacturer specific commands */
#define CMD_MANUFACTURER_START 0x60 // start of manufacturer-specific commands
#define CMD_NAMCO_SPECIFIC 0x70
#define CMD_MANUFACTURER_END 0x7F // end of manufacturer-specific commands

/* Capabilities of the IO board */
#define CAP_END 0x00        // end of structure
#define CAP_PLAYERS 0x01    // player/switch info
#define CAP_COINS 0x02      // coin slot info
#define CAP_ANALOG_IN 0x03  // analog info
#define CAP_ROTARY 0x04     // rotary encoder info
#define CAP_KEYPAD 0x05     // keypad info
#define CAP_LIGHTGUN 0x06   // light gun info
#define CAP_GPI 0x07        // general purpose input info
#define CAP_CARD 0x10       // card system info
#define CAP_HOPPER 0x11     // token hopper info
#define CAP_GPO 0x12        // general purpose output info
#define CAP_ANALOG_OUT 0x13 // analog output info
#define CAP_DISPLAY 0x14    // character display info
#define CAP_BACKUP 0x15     // backup memory



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
