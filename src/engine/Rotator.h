#pragma once

#include "Component.h"
#include "Transform.h"

class Rotator : public Component
{
public:
    float speedRadiansPerSec = 1.0f;

    void onUpdate(float timeSeconds) override
    {
        Transform* t = owner() ? owner()->getComponent<Transform>() : nullptr;
        if (t)
        {
            t->rotationEuler.y = timeSeconds * speedRadiansPerSec;
        }
    }
};


