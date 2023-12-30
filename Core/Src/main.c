/* USER CODE BEGIN Header */
/**
 * @file main.c
 * @author johannes regnier
 * @brief MES MiniSynth - Main program body
 * @version 0.1
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oscillators.h"
#ifndef __USBH_MIDI_CORE_H
#include "usbh_MIDI.h"
#endif
#include "MIDI_application.h"
#include "MIDI_lut.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct synthVoice
{
	int active;
	int note;
	int velocity;
	float volume;
	float frequency;
	float phase;
} synthVoice;

typedef enum
{
	INACTIVE = 0,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
} voiceState;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
USBH_HandleTypeDef hUSBHost; /* USB Host handle */
MIDI_ApplicationTypeDef Appli_state = MIDI_APPLICATION_IDLE;

int pitchbend;
int keyboard[128];
uint8_t runningStatus;
float mtoinc[128];
synthVoice voice[16];

extern oscillator_t osc1, osc2, osc3, osc4, osc5, osc6, osc7;
void OpSetFreq(oscillator_t *op, float f)
{
	op->freq = f;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
// void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
static void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId);
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
	pitchbend = 8192;
	for (int i = 0; i < 128; i++)
	{
		mtoinc[i] = mtof[i] * 0.0000208333f; // 1/48000
		keyboard[i] = -1;
	}
	for (int i = 0; i < 16; i++)
	{
		voice[i].active = INACTIVE;
		voice[i].phase = 0.0f;
		voice[i].volume = 0.0f;
	}
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_I2S3_Init();
	MX_SPI1_Init();
	// MX_USB_HOST_Init();
	/* USER CODE BEGIN 2 */
	SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

	/*## Init Host Library ################################################*/
	USBH_Init(&hUSBHost, USBH_UserProcess_callback, 0);

	/*## Add Supported Class ##############################################*/
	USBH_RegisterClass(&hUSBHost, USBH_MIDI_CLASS);

	/*## Start Host Process ###############################################*/
	USBH_Start(&hUSBHost);

	AUDIO_Init();

	ConsoleInit();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		// MX_USB_HOST_Process();
		// ConsoleProcess();
		// OpSetFreq(&osc1, 1000);
		/* USER CODE BEGIN 3 */
		MIDI_Application();

		// USBH_Delay(1);

		/* USBH_Background Process */
		USBH_Process(&hUSBHost);
	}
	/* USER CODE END 3 */
}

void ProcessMIDI(midi_package_t pack)
{
	int i;
	uint8_t status;

	// Status messages that start with F, for all MIDI channels
	// None of these are implemented - though we will flash an LED
	// for the MIDI clock
	status = pack.evnt0 & 0xF0;
	if (status == 0xF0)
	{
		switch (pack.evnt0)
		{
		case 0xF0: // Start of System Exclusive
		case 0xF1: // MIDI Time Code Quarter Fram
		case 0xF2: // Song Position Pointer
		case 0xF3: // Song Select
		case 0xF4: // Undefined
		case 0xF5: // Undefined
		case 0xF6: // Tune Request
		case 0xF7: // End of System Exclusive
			status = runningStatus = 0x00;
			break;
		case 0xF8:										// Timing Clock (24 times a quarter note)
			HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin); // RED LED
			break;
		case 0xF9: // Undefined
		case 0xFA: // Start Sequence
		case 0xFB: // Continue Sequence
		case 0xFC: // Pause Sequence
		case 0xFD: // Undefined
		case 0xFE: // Active Sensing
		case 0xFF: // Reset all synthesizers to power up
			break;
		}
	}

	// MIDI running status (same status as last message) doesn't seem to work over this USB driver
	// code commented out.

	//	else if((pack.evnt0 & 0x80) == 0x00)
	//		status = runningStatus;
	else
		runningStatus = status = pack.evnt0 & 0xF0;

	switch (status)
	{
	case 0x80: // Note Off
		// turn off all voices that match the note off note
		for (i = 0; i < 16; i++)
		{
			if (voice[i].note == pack.evnt1)
			{
				voice[i].active = RELEASE;
				keyboard[voice[i].note] = -1;
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			}
		}
		break;
	case 0x90:				 // Note On
		if (pack.evnt2 == 0) // velocity 0 means note off
			// turn off all voices that match the note off note
			for (i = 0; i < 16; i++)
			{
				if ((voice[i].note == pack.evnt1) && (voice[i].active != INACTIVE))
				{
					voice[i].active = RELEASE;
					keyboard[voice[i].note] = -1;
					HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
				}
			}
		else
		{
			// if this key is already on, end the associated note and turn it off
			if (keyboard[pack.evnt1] != -1)
			{
				voice[keyboard[pack.evnt1]].active = RELEASE;
				keyboard[pack.evnt1] = -1;
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			}
			// find an inactive voice and assign this key to it
			for (i = 0; i < 16; i++)
			{
				if (voice[i].active == INACTIVE)
				{
					voice[i].active = ATTACK;
					voice[i].note = pack.evnt1;
					float frequency;
					frequency = mtof[voice[i].note];
					OpSetFreq(&osc1, frequency);
					voice[i].velocity = pack.evnt2;
					keyboard[pack.evnt1] = i;
					HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
					SEGGER_RTT_printf(0, "Note ON, pitch %u\r\n", (int)frequency);
					break;
				}
			}
		}
		break;
	case 0xA0: // Polyphonic Pressure
		break;
	case 0xB0:				// Control Change
		switch (pack.evnt1) // CC number
		{
		case 20:

			break;
		case 7:
			break; // master volume
		}
		break;
	case 0xC0: // Program Change
		break;
	case 0xD0: // After Touch
		break;
	case 0xE0: // Pitch Bend
		pitchbend = pack.evnt2 * 128 + pack.evnt1;
		break;
	}
}

/*====================================================================================================*/
/**
 * @brief  User Process function callback
 * @param  phost: Host Handle
 * @param  id: Host Library user message ID
 * @retval none
 */
static void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId)
{
	switch (vId)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = MIDI_APPLICATION_DISCONNECT;
		// 4 - green, 3 - orange, 5 - red, 6 - blue
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = MIDI_APPLICATION_READY;
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		break;

	case HOST_USER_CONNECTION:
		Appli_state = MIDI_APPLICATION_START;
		HAL_GPIO_WritePin(GPIOD, 0x8000, GPIO_PIN_SET);
		break;

	default:
		break;
	}
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
