/**
******************************************************************************
* File Name : TouchGFXConfiguration.cpp
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
#include <texts/TypedTextDatabase.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <BitmapDatabase.hpp>
#include <TouchGFXDataReader.hpp>
#include <platform/driver/lcd/LCD16bppSerialFlash.hpp>
#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/hal/NoDMA.hpp>
#include <TouchGFXHAL.hpp>
#include <STM32TouchController.hpp>
#include <stm32g0xx_hal.h>

extern "C" void touchgfx_init();
extern "C" void touchgfx_taskEntry();

static STM32TouchController tc;
static NoDMA dma;
static TouchGFXDataReader dataReader;
static LCD16bppSerialFlash display(dataReader);
static ApplicationFontProvider fontProvider;
static Texts texts;
static TouchGFXHAL hal(dma, display, tc, 240, 320);

void touchgfx_init()
{
    Bitmap::registerBitmapDatabase(BitmapDatabase::getInstance(), BitmapDatabase::getInstanceSize());
    TypedText::registerTexts(&texts);
    Texts::setLanguage(0);

    /*
     * Parse TouchGFXDataReader instance pointer to TouchGFXHAL and ApplicationFontProvider
     * in order to enable external data access.
     */
    hal.setDataReader(&dataReader);
    fontProvider.setFlashReader(&dataReader);
    FontManager::setFontProvider(&fontProvider);

    FrontendHeap& heap = FrontendHeap::getInstance();
    /*
     * we need to obtain the reference above to initialize the frontend heap.
     */
    (void)heap;

    /*
     * Initialize TouchGFX
     */
    hal.initialize();
}

void touchgfx_taskEntry()
{
    /*
     * Main event loop will check for VSYNC signal, and then process next frame.
     *
     * Note This function returns immediately if there is no VSYNC signal.
     */
     if (OSWrappers::isVSyncAvailable())
     {
         hal.backPorchExited();
     }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
