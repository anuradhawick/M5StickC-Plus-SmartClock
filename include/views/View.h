#pragma once
#include "event.h"

class View
{
public:
    virtual ~View() = default;
    virtual void render();
    virtual bool receive_event(EVENTS::event event);
};
