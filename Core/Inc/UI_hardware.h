/**
 * @file UI_hardware.c
 * @author johannes regnier
 * @brief hardware user interface
 * *****  UI is for the moment:
 * *****  3 potentiometers (on PC4. PC5 and PC2).. Data resolution is 10bits [0, 1023], smoothed using a lowpass filter
 * *****  1 encoder (PB4 / PB5) 
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef _UI_HARDWARE_H
#define _UI_HARDWARE_H

#define ONE_OVER_1023   0.0009775171065

void poll_ADCs(void);
void poll_Encoder(void);
void poll_EncoderBtn(void);

#endif // !_UI_HARDWARE_H
