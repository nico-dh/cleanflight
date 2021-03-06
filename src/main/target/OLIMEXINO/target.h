/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

//#define OLIMEXINO_UNCUT_LED1_E_JUMPER
//#define OLIMEXINO_UNCUT_LED2_E_JUMPER

#ifdef OLIMEXINO_UNCUT_LED1_E_JUMPER
#define LED0_GPIO   GPIOA
#define LED0_PIN    Pin_5 // D13, PA5/SPI1_SCK/ADC5 - "LED1" on silkscreen, Green
#define LED0_PERIPHERAL RCC_APB2Periph_GPIOA
#define LED0
#endif

#ifdef OLIMEXINO_UNCUT_LED2_E_JUMPER
// "LED2" is using one of the PWM pins (CH2/PWM2), so we must not use PWM2 unless the jumper is cut.  @See pwmInit()
#define LED1_GPIO   GPIOA
#define LED1_PIN    Pin_1 // D3, PA1/USART2_RTS/ADC1/TIM2_CH3 - "LED2" on silkscreen, Yellow
#define LED1_PERIPHERAL RCC_APB2Periph_GPIOA
#define LED1
#endif

#define ACC
#define BARO
#define GYRO
#define MAG

#define SENSORS_SET (SENSOR_ACC | SENSOR_BARO | SENSOR_MAG)
