#pragma once

#include "component.h"
#include "transform.h"

class Rotator : public Component
{
public:
    float speedRadiansPerSec = 1.0f;

    void OnUpdate(float timeSeconds) override
    {
        Transform* t = Owner() ? Owner()->GetComponent<Transform>() : nullptr;
        if (t)
        {
            t->rotation_euler.y = timeSeconds * speedRadiansPerSec;
        }
    }
};


