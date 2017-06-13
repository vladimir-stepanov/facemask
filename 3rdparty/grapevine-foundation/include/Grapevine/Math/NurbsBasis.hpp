/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBSBASIS
#define GRAPEVINE_MATH_NURBSBASIS

#include <vector>
#include <cassert>
#include <stdexcept>

namespace Grapevine
{
    template<typename T, int degree>
    class NurbsBasis
    {
        public:
        static const int order_ = degree + 1;

        static T eval(const std::vector<T> &knots, int controlPoint, T param)
        {
            if(degree)
            {
                const T f = WeightF(knots, controlPoint, param);
                const T g = WeightG(knots, controlPoint + 1, param);

                const T N1 = NurbsBasis<T, degree - 1>::eval(knots, controlPoint, param);
                const T N2 = NurbsBasis<T, degree - 1>::eval(knots, controlPoint + 1, param);

                return f * N1 + g * N2;
            }
            else
            {
                // 0th-degree basis function is 1 in the current knot span and zero elsewhere
                return param >= knots[controlPoint] && param < knots[controlPoint + 1] ? 1 : 0;
            }

        }

        static void generatePinnedUniformKnots(std::vector<T> &knots) throw(std::runtime_error)
        {
            if(knots.size() < order_ * 2)
            {
                throw std::runtime_error("Invalid knot vector size. Knot vector has to have at least Nurbs order * 2 entries, "
                                         "or in other words, control point count has to be at least equal to the order of "
                                         "the curve.");
            }

            int i;
            for(i = 0; i < order_; i++)
            {
                knots[i] = 0;
            }

            int linear_knots = knots.size() - order_ * 2;
            for(; i < linear_knots + order_; i++)
            {
                knots[i] = T((i - order_ + 1)) / (linear_knots + 1);
            }

            for(; i < int(knots.size()); i++)
            {
                knots[i] = 1;
            }
        }

        private:
        static bool isInRange(const std::vector<T> &knots, int controlPoint, T param)
        {
            const T knot_i = knots[controlPoint];
            const T knot_id = knots[controlPoint + degree];

            return param >= knot_i && param < knot_id;
        }

        static T WeightF(const std::vector<T> &knots, int controlPoint, T param)
        {
            const T knot_i = knots[controlPoint];
            const T knot_id = knots[controlPoint + degree];

            return isInRange(knots, controlPoint, param) ? (param - knot_i) / (knot_id - knot_i) : 0;
        }

        static T WeightG(const std::vector<T> &knots, int controlPoint, T param)
        {
            const T knot_i = knots[controlPoint];
            const T knot_id = knots[controlPoint + degree];

            return isInRange(knots, controlPoint, param) ? (knot_id - param) / (knot_id - knot_i) : 0;
        }
    };

    template<typename T>
    class NurbsBasis<T, -1>
    {
        public:
        static T eval(const std::vector<T> &knots, int controlPoint, T param)
        {
            assert(!"This should never get called");
            return 0;
        }
    };
}

#endif
