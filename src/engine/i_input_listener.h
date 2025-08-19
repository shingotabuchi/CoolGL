#pragma once

class IInputListener
{
public:
    virtual ~IInputListener() = default;

    virtual void OnScroll(double y_offset) {}
};