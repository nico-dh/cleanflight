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

#include <stdbool.h>
#include <stdint.h>

#include <platform.h>

#include "gpio.h"

#include "bus_spi.h"

// SPI2 Driver
// PB15     28      SPI2_MOSI
// PB14     27      SPI2_MISO
// PB13     26      SPI2_SCK
// PB12     25      SPI2_NSS

static bool spiTest(void);

bool spiInit(void)
{
    gpio_config_t gpio;
    SPI_InitTypeDef spi;

    // Enable SPI2 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);

    // MOSI + SCK as output
    gpio.mode = Mode_AF_PP;
    gpio.pin = Pin_13 | Pin_15;
    gpio.speed = Speed_50MHz;
    gpioInit(GPIOB, &gpio);
    // MISO as input
    gpio.pin = Pin_14;
    gpio.mode = Mode_IN_FLOATING;
    gpioInit(GPIOB, &gpio);
    // NSS as gpio slave select
    gpio.pin = Pin_12;
    gpio.mode = Mode_Out_PP;
    gpioInit(GPIOB, &gpio);

    // Init SPI2 hardware
    SPI_I2S_DeInit(SPI2);
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    spi.SPI_CPOL = SPI_CPOL_High;
    spi.SPI_CPHA = SPI_CPHA_2Edge;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_Init(SPI2, &spi);
    SPI_Cmd(SPI2, ENABLE);

    return spiTest();
}

void spiSelect(bool select)
{
    if (select) {
        digitalLo(GPIOB, Pin_12);
    } else {
        digitalHi(GPIOB, Pin_12);
    }
}

uint8_t spiTransferByte(uint8_t in)
{
    uint8_t rx;
    SPI2->DR;
    SPI2->DR = in;
    while (!(SPI2->SR & SPI_I2S_FLAG_RXNE));
    rx = SPI2->DR;
    while (!(SPI2->SR & SPI_I2S_FLAG_TXE));
    while (SPI2->SR & SPI_I2S_FLAG_BSY);
    return rx;
}

bool spiTransfer(uint8_t *out, uint8_t *in, int len)
{
    uint8_t b;
    SPI2->DR;
    while (len--) {
        b = in ? *(in++) : 0xFF;
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
#ifdef STM32F303xC
        SPI_I2S_SendData16(SPI2, b);
#endif
#ifdef STM32F10X_MD
        SPI_I2S_SendData(SPI2, b);
#endif
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
#ifdef STM32F303xC
        b = SPI_I2S_ReceiveData16(SPI2);
#endif
#ifdef STM32F10X_MD
        b = SPI_I2S_ReceiveData(SPI2);
#endif
        if (out)
            *(out++) = b;
    }

    return true;
}

static bool spiTest(void)
{
    uint8_t out[] = { 0x9F, 0, 0, 0 };
    uint8_t in[4];

    spiSelect(true);
    spiTransfer(in, out, sizeof(out));
    spiSelect(false);
    if (in[1] == 0xEF)
        return true;

    return false;
}
