#include <main.h>
#include <MB1642DButtonController.hpp>
#include <touchgfx/hal/HAL.hpp>

void MB1642DButtonController::init()
{
    previousState = 0;
}

bool MB1642DButtonController::sample(uint8_t& key)
{
    uint8_t state = MB1642DKeyState();
    if (state == previousState)
    {
        return false;
    }
    previousState = state;
    if (state != 0)
    {
        key = state;
        return true;
    }
    return false;
}

uint8_t MB1642DButtonController::MB1642DKeyState(void)
{
    uint8_t keyPressed = 0;

    if (touchgfx::HAL::getInstance()->getDisplayOrientation() == touchgfx::ORIENTATION_PORTRAIT)
    {
        if (HAL_GPIO_ReadPin(KEY_CENTER_GPIO_Port, KEY_CENTER_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '5'; // Corresponds to keyboard keypad enter
        }
        else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '4'; // Corresponds to keyboard keypad left
        }
        else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '6'; // Corresponds to keyboard keypad right
        }
        else if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '8'; // Corresponds to keyboard keypad up
        }
        else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '2'; // Corresponds to keyboard keypad down
        }
        //Blue user button on Nucleo boad
        else if (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_Port, BUTTON_USER_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '0';
        }
    }
    else
    {
        if (HAL_GPIO_ReadPin(KEY_CENTER_GPIO_Port, KEY_CENTER_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '5'; // Corresponds to keyboard keypad enter
        }
        else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '8'; // Corresponds to keyboard keypad up
        }
        else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '2'; // Corresponds to keyboard keypad down
        }
        else if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '6'; // Corresponds to keyboard keypad right
        }
        else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '4'; // Corresponds to keyboard keypad left
        }
        //Blue user button on Nucleo boad
        else if (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_Port, BUTTON_USER_Pin) == GPIO_PIN_RESET)
        {
            keyPressed = '0';
        }
    }

    return keyPressed;
}
