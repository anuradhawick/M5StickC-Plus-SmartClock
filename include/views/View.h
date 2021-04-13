#pragma once

class View
{
public:
    virtual ~View() = default;
    virtual void render();
};
