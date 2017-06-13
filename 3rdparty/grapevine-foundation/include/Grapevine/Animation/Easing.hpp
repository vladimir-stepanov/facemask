/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_EASING
#define GRAPEVINE_ANIMATION_EASING

#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class Easing
    {
    public:
        CLASS_COPYABLE(Easing)

        struct Type
        {
            enum Enum
            {
                Linear,
                InOutExpo,
                InOutQuad,
                OutSine,
                InOutQuart,
                OutQuad,
                InOutSine,
                OutSineBounce,
            };
        };
        Easing(Type::Enum type);
        virtual ~Easing();

        /**
         * Get eased offset
         *
         * @param [in]  timeDistance    [0.0, 1.0] value, 0.0 being the
         *                              start of the time and 1.0 the
         *                              end of the time.
         */
        virtual float offset(float timeDistance) const;

    private:
        Type::Enum type_;
    };
}

#endif
