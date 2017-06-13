/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBSCURVE
#define GRAPEVINE_MATH_NURBSCURVE

#include <cmath>
#include <vector>
#include <limits>
#include <stdexcept>
#include "Grapevine/Math/Norm.hpp"
#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Math/NurbsBasis.hpp"
#include "Grapevine/Math/NurbsControlPoint.hpp"
#include "Grapevine/Math/NurbsRemapper.hpp"

namespace Grapevine
{
    template<typename T, typename controlPointType, int degree>
    class NurbsCurve
    {
        public:
        static const int order_ = NurbsBasis<T, degree>::order_;
        static const int degree_ = degree;

        typedef NurbsControlPoint<T, controlPointType> ControlPoint;

        NurbsCurve(int points) throw(std::runtime_error)
        :   controlPoints_(points),
            knots_(points + order_)
        {
            NurbsBasis<T, degree>::generatePinnedUniformKnots(knots_);
        }

        controlPointType eval(T u)
        {
            T norm = 0;
            controlPointType result = 0;

            // TODO: optimize by evaluating only those basis functions that affect the current knot span
            //       and check if basis evaluation itself can be simplified after this has been taken
            //       care of (see isInRange, et al)
            for(int i = 0; i < controlPoints_.size(); i++)
            {
                T basisValue = NurbsBasis<T, degree>::eval(knots_, i, u) * controlPoints_[i].weight();
                norm += basisValue;
                result += controlPoints_[i].value() * basisValue;
            }

            if(norm > std::numeric_limits<T>::epsilon())
            {
                return result / norm;
            }

            return result;
        }

        void updateRemapper(NurbsRemapper<T> &remapper, int density)
        {
            std::vector<T> curveLength;
            T totalLength(0);

            controlPointType prev = eval(0);
            curveLength.push_back(0);
            for(int i = 1; i < density; i++)
            {
                T param = T(i) / (density - 1);
                controlPointType result = eval(nextafterf(param, -1));
                T dist = L2Norm(result - prev);

                curveLength.push_back(totalLength += dist);
                prev = result;
            }

            for(int i = 0; i < density; i++)
            {
                curveLength[i] /= totalLength;
            }

            int j = 0;
            T start = 0;
            T end = 0;
            T unitStep = T(1) / (density - 1);

            remapper.paramMap_.clear();
            remapper.paramMap_.push_back(0);
            for(int i = 1; i < density - 1; i++)
            {
                T curveParam = T(i) * unitStep;

                while(curveParam > end)
                {
                    end = curveLength[++j];
                    start = curveLength[j - 1];
                }

                T factor = (curveParam - start) / (end - start);
                T paramStart = T(j - 1) * unitStep;

                remapper.paramMap_.push_back(paramStart + unitStep * factor);
            }
            remapper.paramMap_.push_back(1);
        }

        ControlPoint &controlPoint(unsigned int i) throw(std::range_error)
        {
            if(i >= controlPoints_.size())
            {
                throw std::range_error("Control point index over the range");
            }

            return controlPoints_[i];
        }

        private:

        std::vector<NurbsControlPoint<T, controlPointType> > controlPoints_;
        std::vector<T> knots_;
    };

}
#endif
