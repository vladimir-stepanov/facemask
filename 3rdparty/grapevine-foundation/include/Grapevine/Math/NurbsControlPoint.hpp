/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBSCONTROLPOINT
#define GRAPEVINE_MATH_NURBSCONTROLPOINT

namespace Grapevine
{
    template<typename T, typename controlPointType>
    class NurbsControlPoint
    {
        public:
        NurbsControlPoint()
        :   controlPoint_(0),
            weight_(1)
        {}

        const controlPointType &value() const
        {
            return controlPoint_;
        }

        const controlPointType &value(const controlPointType & newValue)
        {
            return controlPoint_ = newValue;
        }

        T weight() const
        {
            return weight_;
        }

        T weight(T newWeight)
        {
            weight_ = newWeight;
            return weight_;
        }

        private:
        controlPointType controlPoint_;
        T                weight_;
    };

}

#endif
