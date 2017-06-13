/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBS
#define GRAPEVINE_MATH_NURBS

#include "Grapevine/Math/NurbsRemapper.hpp"
#include "Grapevine/Math/NurbsControlPoint.hpp"
#include "Grapevine/Math/NurbsCurve.hpp"
#include "Grapevine/Math/NurbsSurface.hpp"

namespace Grapevine
{
    template<typename controlPointType>
    struct Nurbs
    {
        typedef NurbsRemapper<float>                       Remapper;
        typedef NurbsControlPoint<float, controlPointType> ControlPoint;
        typedef AbstractNurbsSurface<float, controlPointType> Surface;
    };

    template<typename controlPointType>
    struct LinearNurbs
    {
        typedef NurbsCurve<float, controlPointType, 1>      Curve;
        typedef NurbsSurface<float, controlPointType, 1, 1> Surface;
    };

    template<typename controlPointType>
    struct QuadraticNurbs
    {
        typedef NurbsCurve<float, controlPointType, 2>      Curve;
        typedef NurbsSurface<float, controlPointType, 2, 2> Surface;
    };

    template<typename controlPointType>
    struct CubicNurbs
    {
        typedef NurbsCurve<float, controlPointType, 3>      Curve;
        typedef NurbsSurface<float, controlPointType, 3, 3> Surface;
    };

}
#endif
