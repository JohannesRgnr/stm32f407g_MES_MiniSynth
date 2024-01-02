/*
 * MIDI_application.c
 *
 *  Created on: 6 dec. 2014
 *      Author: Xavier Halgand
 */


/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "MIDI_application.h"

extern USBH_HandleTypeDef hUSBHost;
extern MIDI_ApplicationTypeDef Appli_state;
extern int8_t midi1, midi2;



/* Private define ------------------------------------------------------------*/

#define RX_BUFF_SIZE   64  /* Max Received data 64 bytes */


uint8_t MIDI_RX_Buffer[RX_BUFF_SIZE]; // MIDI reception buffer

/* Private function prototypes -----------------------------------------------*/
void ProcessReceivedMidiDatas(void);


/*-----------------------------------------------------------------------------*/
/**
 * @brief  Main routine for MIDI application, looped in main.c
 * @param  None
 * @retval none
 */
void MIDI_Application(void)
{
	if(Appli_state == MIDI_APPLICATION_READY)
	{
		USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // just once at the beginning, start the first reception
		Appli_state = MIDI_APPLICATION_RUNNING;
	}
	if(Appli_state == MIDI_APPLICATION_RUNNING)
	{
			//....pffff......grrrrr......
	}

	if(Appli_state == MIDI_APPLICATION_DISCONNECT)
	{
		Appli_state = MIDI_APPLICATION_IDLE;
		USBH_MIDI_Stop(&hUSBHost);
	}

}


/*-----------------------------------------------------------------------------*/
void ProcessReceivedMidiDatas(void)
{
	uint16_t numberOfPackets;
	uint8_t *ptr = MIDI_RX_Buffer;
	midi_package_t pack;

	numberOfPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost) / 4; //each USB midi package is 4 bytes long
	if (numberOfPackets != 0) // seems useless...
	{
		//BSP_LED_Toggle(LED_Blue);

		while(numberOfPackets--)
		{
			pack.cin_cable = *ptr ; ptr++ ;
			pack.evnt0 = *ptr ; ptr++ ;
			pack.evnt1 = *ptr ; ptr++ ;
			pack.evnt2 = *ptr ; ptr++ ;

			if(pack.cin_cable != 0)
				HAL_GPIO_WritePin(GPIOD, 0x8000, GPIO_PIN_SET);
			ProcessMIDI(pack);

		}
	}
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief  MIDI data receive callback.
 * @param  phost: Host handle
 * @retval None
 */
void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	ProcessReceivedMidiDatas();

	USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // start a new reception
}
/*-----------------------------------------------------------------------------*/
