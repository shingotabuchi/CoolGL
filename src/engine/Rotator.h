#pragma once

#include "component.h"
#include "transform.h"

class Rotator : public Component
{
public:
    float speedRadiansPerSec = 1.0f;

    void OnUpdate(float timeSeconds) override
    {
        if (!cached_transform_ && Owner())
        {
            cached_transform_ = Owner()->GetComponent<Transform>();
        }
        Transform* t = cached_transform_;
        if (t)
        {
            t->rotation_euler.y = timeSeconds * speedRadiansPerSec;
        }
    }

    void OnDetach() override { cached_transform_ = nullptr; }
private:
    Transform* cached_transform_ = nullptr;
};


