#ifndef MB1642ABUTTONCONTROLLER_HPP_
#define MB1642ABUTTONCONTROLLER_HPP_

#include <platform/driver/button/ButtonController.hpp>

class MB1642DButtonController : public touchgfx::ButtonController
{
    virtual void init();
    virtual bool sample(uint8_t& key);

private:
    uint8_t MB1642DKeyState(void);

    uint8_t previousState;
};

#endif /* MB1642ABUTTONCONTROLLER_HPP_ */
