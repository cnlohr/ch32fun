/* This shows how to use the option bytes.  I.e. how do you disable NRST?
   WARNING Portions of this code are under the following copyright.
*/
/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32v00x_flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : This file provides all the FLASH firmware functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "ch32fun.h"
#include <stdio.h>

uint32_t count;

int FLASH_WaitForLastOperation(uint32_t Timeout);

int main()
{
	SystemInit();

	Delay_Ms( 100 );

	FLASH->OBKEYR = FLASH_KEY1;
	FLASH->OBKEYR = FLASH_KEY2;
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	FLASH->MODEKEYR = FLASH_KEY1;
	FLASH->MODEKEYR = FLASH_KEY2;

	printf( "Option bytes started as:%04x\n", OB->USER );

	uint16_t rdptmp = RDP_Key;


	int status = FLASH_WaitForLastOperation(EraseTimeout);
	if(status == FLASH_COMPLETE)
	{
		FLASH->OBKEYR = FLASH_KEY1;
		FLASH->OBKEYR = FLASH_KEY2;

		FLASH->CTLR |= CR_OPTER_Set;
		FLASH->CTLR |= CR_STRT_Set;
		status = FLASH_WaitForLastOperation(EraseTimeout);

		if(status == FLASH_COMPLETE)
		{
			FLASH->CTLR &= CR_OPTER_Reset;
			FLASH->CTLR |= CR_OPTPG_Set;
			OB->RDPR = (uint16_t)rdptmp;
			status = FLASH_WaitForLastOperation(ProgramTimeout);

			if(status != FLASH_TIMEOUT)
			{
				FLASH->CTLR &= CR_OPTPG_Reset;
			}
		}
		else
		{
			if(status != FLASH_TIMEOUT)
			{
				FLASH->CTLR &= CR_OPTPG_Reset;
			}
		}
	}


	printf( "After Clear:%04x\n", OB->USER );

/* Notes from flash document:
 * @param   OB_IWDG - Selects the IWDG mode
 *            OB_IWDG_SW - Software IWDG selected
 *            OB_IWDG_HW - Hardware IWDG selected
 *          OB_STOP - Reset event when entering STOP mode.
 *            OB_STOP_NoRST - No reset generated when entering in STOP
 *            OB_STOP_RST - Reset generated when entering in STOP
 *          OB_STDBY - Reset event when entering Standby mode.
 *            OB_STDBY_NoRST - No reset generated when entering in STANDBY
 *            OB_STDBY_RST - Reset generated when entering in STANDBY
 *          OB_RST - Selects the reset IO mode and Ignore delay time
 *            OB_RST_NoEN - Reset IO disable (PD7)
 *            OB_RST_EN_DT12ms - Reset IO enable (PD7) and  Ignore delay time 12ms
 *            OB_RST_EN_DT1ms - Reset IO enable (PD7) and  Ignore delay time 1ms
 *            OB_RST_EN_DT128us - Reset IO enable (PD7) and  Ignore delay time 128us
 *          OB_BOOT - Selects bootloader or usercode for on start.
 *            OB_STARTMODE_USER - Boot directly to flash
 *            OB_STARTMODE_BOOT - Boot to bootloader
*/
	uint16_t OB_STOP = OB_STOP_NoRST;
	uint16_t OB_IWDG = OB_IWDG_SW;
	uint16_t OB_STDBY = OB_STDBY_NoRST;
	uint16_t OB_RST = OB_RST_EN_DT1ms;
	uint16_t OB_BOOT = OB_STARTMODE_BOOT;

    FLASH->OBKEYR = FLASH_KEY1;
    FLASH->OBKEYR = FLASH_KEY2;
    status = FLASH_WaitForLastOperation(10000);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_OPTPG_Set;
        OB->USER = OB_BOOT | OB_IWDG | (uint16_t)(OB_STOP | (uint16_t)(OB_STDBY | (uint16_t)(OB_RST | (uint16_t)0xc0)));

        status = FLASH_WaitForLastOperation(10000);
        if(status != FLASH_TIMEOUT)
        {
            FLASH->CTLR &= CR_OPTPG_Reset;
        }
    }

	printf( "After Write:%04x\n", OB->USER );

	while(1);
}


int FLASH_GetBank1Status(void)
{
	int flashstatus = FLASH_COMPLETE;

	if((FLASH->STATR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY)
	{
		flashstatus = FLASH_BUSY;
	}
	else
	{
		if((FLASH->STATR & FLASH_FLAG_BANK1_WRPRTERR) != 0)
		{
			flashstatus = FLASH_ERROR_WRP;
		}
		else
		{
			flashstatus = FLASH_COMPLETE;
		}
	}
	return flashstatus;
}


int FLASH_WaitForLastOperation(uint32_t Timeout)
{
	int status = FLASH_COMPLETE;

	status = FLASH_GetBank1Status();
	while((status == FLASH_BUSY) && (Timeout != 0x00))
	{
		status = FLASH_GetBank1Status();
		Timeout--;
	}
	if(Timeout == 0x00)
	{
		status = FLASH_TIMEOUT;
	}
	return status;
}

