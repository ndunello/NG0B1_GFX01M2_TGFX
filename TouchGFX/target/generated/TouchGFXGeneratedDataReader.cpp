/**
  ******************************************************************************
  * File Name          : TouchGFXGeneratedDataReader.cpp
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <TouchGFXGeneratedDataReader.hpp>
#include <stm32g0xx.h>

/*
 * External Interface description for starting transmissions
 */
extern "C" void DataReader_WaitForReceiveDone();
extern "C" void DataReader_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
extern "C" void DataReader_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length);

/*
 * flashEndAddress is calculated using the FLASH_BASE address provided in stm32g0xx.h
 * and by reading the flash size of the MCU located at address FLASHSIZE_BASE.
 * FLASHSIZE_BASE is in kBytes and is shifted left to translate into an address space.
 */
TouchGFXGeneratedDataReader::TouchGFXGeneratedDataReader() :
    flashEndAddress(FLASH_BASE + ((*(uint16_t*)FLASHSIZE_BASE) << 10)) {}

bool TouchGFXGeneratedDataReader::addressIsAddressable(const void* address)
{
    /* Check if adrress is in internal flash */
    if (address >= (void*)FLASH_BASE && address <(void*)(flashEndAddress))
    {
        return true;
    }

    /* Check if adrress is in internal RAM */
    if (address >= (void*)SRAM_BASE && address <(void*)(SRAM_BASE + SRAM_SIZE_MAX))
    {
        return true;
    }

    // Address is neither internal RAM or internal flash
    return false;
}

void TouchGFXGeneratedDataReader::copyData(const void* src, void* dst, uint32_t bytes)
{
    /* If requested number of bytes are less than 250
     * CPU and polling will be used to read from flash.
     * If number of bytes requested is larger than 250
     * a DMA transfer will be started.
     */
    if (bytes < 250)
    {
        /* Start transfer using the CPU */
        DataReader_ReadData((uint32_t)src, (uint8_t*)dst, bytes);
    }
    else
    {
        /* Start transfer using DMA */
        DataReader_StartDMAReadData((uint32_t)src, (uint8_t*)dst, bytes);

        /* Wait on data transmision complete */
	    DataReader_WaitForReceiveDone();
    }
}

void TouchGFXGeneratedDataReader::startFlashLineRead(const void* src, uint32_t bytes)
{
    /* Start transfer using DMA */
    DataReader_StartDMAReadData((uint32_t)src, (readToBuffer1 ? buffer1 : buffer2), bytes);
}

const uint8_t* TouchGFXGeneratedDataReader::waitFlashReadComplete()
{
  /*
   * Switch line buffer and then wait for transmission to complete before returning new buffer.
   */
    uint8_t* readBuffer = readToBuffer1 ? buffer1 : buffer2;
    DataReader_WaitForReceiveDone();
    readToBuffer1 = !readToBuffer1;
    return readBuffer;
}
