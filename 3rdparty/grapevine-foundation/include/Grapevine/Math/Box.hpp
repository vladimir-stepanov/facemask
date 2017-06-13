/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_BOX
#define GRAPEVINE_MATH_BOX

#include <cfloat>
#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    class Box
    {
    public:
        CLASS_COPYABLE(Box);

        Box(Vector3 const& min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3 const& max = Vector3(FLT_MIN, FLT_MIN, FLT_MIN));
        Box(Vector3 const& maxFromOrigo);
        void setMin(Vector3 const& min);
        void setMax(Vector3 const& max);
        Vector3 const& min() const;
        Vector3 const& max() const;

    private:
        Vector3 min_;
        Vector3 max_;
    };
}

#endif
