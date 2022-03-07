#ifndef SCREEN5PRESENTER_HPP
#define SCREEN5PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen5View;

class Screen5Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen5Presenter(Screen5View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Screen5Presenter() {};

private:
    Screen5Presenter();

    Screen5View& view;
};

#endif // SCREEN5PRESENTER_HPP
