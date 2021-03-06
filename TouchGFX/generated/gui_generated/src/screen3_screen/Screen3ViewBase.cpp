/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/screen3_screen/Screen3ViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"

Screen3ViewBase::Screen3ViewBase()
{

    __background.setPosition(0, 0, 240, 320);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));

    image1.setXY(0, 0);
    image1.setBitmap(touchgfx::Bitmap(BITMAP_SPHERE_ID));

    add(__background);
    add(image1);
}

void Screen3ViewBase::setupScreen()
{

}

//Handles when a key is pressed
void Screen3ViewBase::handleKeyEvent(uint8_t key)
{
    if(56 == key)
    {
        //gotoScreen5
        //When hardware button 56 clicked change screen to Screen5
        //Go to Screen5 with no screen transition
        application().gotoScreen5ScreenNoTransition();
    }
    if(53 == key)
    {
        //gotoScreen1
        //When hardware button 53 clicked change screen to Screen1
        //Go to Screen1 with no screen transition
        application().gotoScreen1ScreenNoTransition();
    }
    if(50 == key)
    {
        //gotoScreen4
        //When hardware button 50 clicked change screen to Screen4
        //Go to Screen4 with no screen transition
        application().gotoScreen4ScreenNoTransition();
    }
}
