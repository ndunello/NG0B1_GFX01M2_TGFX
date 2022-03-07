#include "stm32g0xx_hal.h"
#include "st7789vw.h"
#include "main.h"
#include <assert.h>
#include <MB1642DDisplayDriver.h>
//#include <touchgfx/hal/HAL.hpp>

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern TIM_HandleTypeDef htim6;

volatile uint16_t TE = 0;

//Signal TE interrupt to TouchGFX
void touchgfxSignalVSync(void);

static uint32_t OrientationTab[4] =
{
  0x00U , /* Portrait orientation choice of LCD screen               */
  0xC0U , /* Portrait rotated 180° orientation choice of LCD screen  */
  0xA0U , /* Landscape orientation choice of LCD screen              */
  0x60U , /* Landscape rotated 180° orientation choice of LCD screen */
};

static void Display_DCS_Send(uint8_t command)
{
  // Reset the nCS pin
  SPIA_NCS_GPIO_Port->BRR = SPIA_NCS_Pin;
  // Set the DCX pin
  SPIA_DCX_GPIO_Port->BRR = SPIA_DCX_Pin;

  // Send the command
  *((__IO uint8_t*)&hspi1.Instance->DR) = command;

  // Wait until the bus is not busy before changing configuration
  while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

  // Reset the DCX pin
  SPIA_DCX_GPIO_Port->BSRR = SPIA_DCX_Pin;

  // Set the nCS
  SPIA_NCS_GPIO_Port->BSRR = SPIA_NCS_Pin;
}

static void Display_DCS_Send_With_Data(uint8_t command, uint8_t* data, uint8_t size)
{
  // Reset the nCS pin
  SPIA_NCS_GPIO_Port->BRR = SPIA_NCS_Pin;
  // Set the DCX pin
  SPIA_DCX_GPIO_Port->BRR = SPIA_DCX_Pin;

  *((__IO uint8_t*)&hspi1.Instance->DR) = command;

  // Wait until the bus is not busy before changing configuration
  while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);
  SPIA_DCX_GPIO_Port->BSRR = SPIA_DCX_Pin;

  while (size > 0U)
  {
    *((__IO uint8_t*)&hspi1.Instance->DR) = *data;
    data++;
    size--;
    /* Wait until TXE flag is set to send data */
    while(((hspi1.Instance->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
  }

  // Wait until the bus is not busy before changing configuration
  while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);

  // Set the nCS
  SPIA_NCS_GPIO_Port->BSRR = SPIA_NCS_Pin;
}

void MB1642DDisplayDriver_DisplayOn(void)
{
  // Display ON
  Display_DCS_Send(ST7789H2_DISPLAY_ON);
  HAL_Delay(100);
}

void Display_OFF(void)
{
  // Display OFF
  Display_DCS_Send(ST7789H2_DISPLAY_OFF);
  HAL_Delay(100);
}

static uint16_t old_x0=0xFFFF, old_x1=0xFFFF, old_y0=0xFFFF, old_y1=0xFFFF;

void Display_Set_Area(uint16_t x0, uint16_t y0,
                      uint16_t x1, uint16_t y1)
{
  uint8_t arguments[4];

  // Set columns, if changed
  if (x0 != old_x0 || x1 != old_x1)
  {
    arguments[0] = x0 >> 8;
    arguments[1] = x0 & 0xFF;
    arguments[2] = x1 >> 8;
    arguments[3] = x1 & 0xFF;
    Display_DCS_Send_With_Data(ST7789H2_CASET, arguments, 4);

    old_x0 = x0;
    old_x1 = x1;
  }

  // Set rows, if changed
  if (y0 != old_y0 || y1 != old_y1)
  {
    arguments[0] = y0 >> 8;
    arguments[1] = y0 & 0xFF;
    arguments[2] = y1 >> 8;
    arguments[3] = y1 & 0xFF;
    Display_DCS_Send_With_Data(ST7789H2_RASET, arguments, 4);

    old_y0 = y0;
    old_y1 = y1;
  }
}

volatile uint8_t IsTransmittingBlock_;
void Display_Bitmap(const uint16_t *bitmap, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey)
{
  IsTransmittingBlock_ = 1;
  __HAL_SPI_ENABLE(&hspi1); // Enables SPI peripheral
  uint8_t command = ST7789H2_WRITE_RAM;

  // Define the display area
  Display_Set_Area(posx, posy, posx+sizex-1, posy+sizey-1);

  // Reset the nCS pin
  SPIA_NCS_GPIO_Port->BRR = SPIA_NCS_Pin;
  // Set the DCX pin
  SPIA_DCX_GPIO_Port->BRR = SPIA_DCX_Pin;

  *((__IO uint8_t*)&hspi1.Instance->DR) = command;

  // Wait until the bus is not busy before changing configuration
  while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET);
  SPIA_DCX_GPIO_Port->BSRR = SPIA_DCX_Pin;

  // Set the SPI in 16-bit mode to match endianess
  hspi1.Instance->CR2 = SPI_DATASIZE_16BIT;

  // Disable spi peripherals
  __HAL_SPI_DISABLE(&hspi1);
  __HAL_DMA_DISABLE(&hdma_spi1_tx);

  CLEAR_BIT(hspi1.Instance->CR2, SPI_CR2_LDMATX);

  /* Clear all flags */
  __HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, (DMA_FLAG_GI1 << (hdma_spi1_tx.ChannelIndex & 0x1cU)));

  /* Configure DMA Channel data length */
  hdma_spi1_tx.Instance->CNDTR = sizex*sizey;
  /* Configure DMA Channel destination address */
  hdma_spi1_tx.Instance->CPAR = (uint32_t)&hspi1.Instance->DR;

  /* Configure DMA Channel source address */
  hdma_spi1_tx.Instance->CMAR = (uint32_t)bitmap;

  /* Disable the transfer half complete interrupt */
  __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_HT);
  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(&hdma_spi1_tx, (DMA_IT_TC | DMA_IT_TE));

  /* Enable the Peripherals */
  __HAL_DMA_ENABLE(&hdma_spi1_tx);
  __HAL_SPI_ENABLE(&hspi1);

  /* Enable Tx DMA Request */
  SET_BIT(hspi1.Instance->CR2, SPI_CR2_TXDMAEN);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);

  TE++;
  (&htim6)->Instance->CR1 &= ~(TIM_CR1_CEN);
  (&htim6)->Instance->CNT = 0;

  touchgfxSignalVSync();
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);
  (&htim6)->Instance->CR1 = (TIM_CR1_CEN);
}

void MB1642DDisplayDriver_DisplayInit(uint8_t orientation)
{
	uint8_t arguments[15];
	__HAL_SPI_ENABLE(&hspi1);

	/* Sleep In Command */
	Display_DCS_Send(ST7789H2_SLEEP_IN);
	/* Wait for 10ms */
	HAL_Delay(10);

	/* SW Reset Command */
	Display_DCS_Send(ST7789V_SWRESET);
	/* Wait for 200ms */
	HAL_Delay(200);

	/* Sleep Out Command */
	Display_DCS_Send(ST7789H2_SLEEP_OUT);
	/* Wait for 120ms */
	HAL_Delay(120);

	/* Set Display Orientation */
	if (orientation == 0)
	{
		/* ORIENTATION_PORTRAIT */
		arguments[0] = OrientationTab[0];
	}
	else
	{
		/* ORIENTATION_LANDSCAPE */
		arguments[0] = OrientationTab[2];
	}
  	Display_DCS_Send_With_Data(ST7789H2_NORMAL_DISPLAY, arguments, 1);

  	/* Set Full Screen Display Window */
  	/* Column addr set, 4 args, no delay: XSTART = Xpos, XEND = (Xpos + Width - 1) */
  	arguments[0] = 0x00;
  	arguments[1] = 0x00;
  	arguments[2] = 0x00;
  	arguments[3] = 0xEF;
  	Display_DCS_Send_With_Data(ST7789H2_CASET, arguments, 4);
  	/* Row addr set, 4 args, no delay: YSTART = Ypos, YEND = (Ypos + Height - 1) */
  	arguments[0] = 0x00;
  	arguments[1] = 0x00;
  	arguments[2] = 0x01;
  	arguments[3] = 0x3F;
  	Display_DCS_Send_With_Data(ST7789H2_RASET, arguments, 4);
  	Display_DCS_Send(ST7789H2_WRITE_RAM);

  	/* Set Pixel Format */
  	arguments[0] = 0x55;
  	Display_DCS_Send_With_Data(ST7789H2_COLOR_MODE, arguments, 1);

  	/* Frame Rate Control in normal mode */
  	arguments[0] = 0x0A; /* Default is 72 Hz */
  	Display_DCS_Send_With_Data(ST7789H2_FR_CTRL, arguments, 1);

  	/* PORCH control setting */
  	arguments[0] = 0x01;
  	arguments[1] = 0x01;
  	arguments[2] = 0x00;
  	arguments[3] = 0x01;
  	arguments[4] = 0x01;
  	Display_DCS_Send_With_Data(ST7789H2_PORCH_CTRL, arguments, 5);

  	/* Set TE scan line VSYNC Only */
  	arguments[0] = 0x00;
  	arguments[1] = 0x00;
  	Display_DCS_Send_With_Data(ST7789V_SET_TEAR_SCANLINE, arguments, 2);
  	arguments[0] = 0x00;
  	Display_DCS_Send_With_Data(ST7789H2_TEARING_EFFECT, arguments, 1);
}

void MB1642DDisplayDriver_DisplayReset(void)
{
  HAL_GPIO_WritePin(DISP_NRESET_GPIO_Port, DISP_NRESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(DISP_NRESET_GPIO_Port, DISP_NRESET_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
}

void MB1642DDisplayDriver_Init(void)
{
  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

int touchgfxDisplayDriverTransmitActive(void)
{
  return IsTransmittingBlock_;
}

void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  Display_Bitmap((uint16_t*)pixels, x, y, w, h);
}

void MB1642DDisplayDriver_DMACallback(void)
{
  /* Transfer Complete Interrupt management ***********************************/
  if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (hdma_spi1_tx.Instance->CCR & DMA_IT_TC)))
  {
    /* Disable the transfer complete and error interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, DMA_IT_TE | DMA_IT_TC);

    /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, DMA_FLAG_TC1);

    IsTransmittingBlock_ = 0;

    // Wait until the bus is not busy before changing configuration
    // SPI is busy in communication or Tx buffer is not empty
    while(((hspi1.Instance->SR) & SPI_FLAG_BSY) != RESET) { }

    // Set the nCS
    SPIA_NCS_GPIO_Port->BSRR = SPIA_NCS_Pin;

    // Go back to 8-bit mode
    hspi1.Instance->CR2 = SPI_DATASIZE_8BIT;

    // Signal Transfer Complete to TouchGFX
    DisplayDriver_TransferCompleteCallback();
  }
    /* Transfer Error Interrupt management **************************************/
  else if ((0U != (DMA1->ISR & (DMA_FLAG_TC1))) && (0U != (hdma_spi1_tx.Instance->CCR & DMA_IT_TE)))
  {
    /* When a DMA transfer error occurs */
    /* A hardware clear of its EN bits is performed */
    /* Disable ALL DMA IT */
    __HAL_DMA_DISABLE_IT(&hdma_spi1_tx, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, DMA_FLAG_GI1 );

    assert(0);  // Halting program - Transfer Error Interrupt received.
  }
}
int touchgfxDisplayDriverShouldTransferBlock(uint16_t bottom)
{
  //return (bottom < getCurrentLine());
  return (bottom < (TE > 0 ? 0xFFFF : ((__IO uint16_t)htim6.Instance->CNT)));
}

