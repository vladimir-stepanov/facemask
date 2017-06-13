/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/EasingCurve.hpp>
#include <Grapevine/Animation/Easing.hpp>

namespace Grapevine
{
    Easing::Easing(Easing::Type::Enum type):
        type_(type)
    {
    }

    Easing::Easing(Easing const& r):
        type_(r.type_)
    {
    }

    Easing::~Easing()
    {
    }

    Easing& Easing::operator=(Easing const& r)
    {
        if (this != &r)
        {
            type_ = r.type_;
        }
        return *this;
    }

    float Easing::offset(float t) const
    {
        float r = 0;

        // TODO: polish with private?
        switch(type_)
        {
            case Easing::Type::Linear:
                r = t;
                break;
            case Easing::Type::InOutExpo:
                r = EasingCurve::easeInOutExpo(t);
                break;
            case Easing::Type::InOutQuad:
                r = EasingCurve::easeInOutQuad(t);
                break;
            case Easing::Type::OutSine:
                r = EasingCurve::easeOutSine(t);
                break;
            case Easing::Type::InOutQuart:
                r = EasingCurve::easeInOutQuart(t);
                break;
            case Easing::Type::OutQuad:
                r = EasingCurve::easeOutQuad(t);
                break;
            case Easing::Type::InOutSine:
                r = EasingCurve::easeInOutSine(t);
                break;
            case Easing::Type::OutSineBounce:
                r = EasingCurve::easeOutSineBounce(t);
                break;
        }
        return r;
    }
}
