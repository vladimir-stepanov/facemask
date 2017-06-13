/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_QUATERNION
#define GRAPEVINE_MATH_QUATERNION

#include "Grapevine/Math/QuaternionTemplate.hpp"

namespace Grapevine
{
    typedef QuaternionTemplate<float> Quaternion;
    typedef QuaternionTemplate<double> QuaternionD;
    typedef QuaternionTemplate<int> QuaternionI;
    typedef QuaternionTemplate<unsigned int> QuaternionU;
    typedef QuaternionTemplate<bool> QuaternionB;

}

#endif
