/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBSSURFACE
#define GRAPEVINE_MATH_NURBSSURFACE

#include <vector>
#include <limits>
#include <stdexcept>
#include "Grapevine/Math/Norm.hpp"
#include "Grapevine/Math/NurbsRemapper.hpp"
#include "Grapevine/Math/NurbsBasis.hpp"
#include "Grapevine/Math/NurbsControlPoint.hpp"

namespace Grapevine
{
    template<typename controlPointType>
    struct NurbsSurfaceDerivatives
    {
        NurbsSurfaceDerivatives(controlPointType point, controlPointType du, controlPointType dv)
        : point_(point), du_(du), dv_(dv)
        {
        }

        controlPointType point_;
        controlPointType du_;
        controlPointType dv_;
    };

    template<typename T, typename controlPointType>
    class AbstractNurbsSurface
    {
        public:
        virtual controlPointType eval(T u, T v) = 0;
        virtual NurbsSurfaceDerivatives<controlPointType> evalDerivatives(T u, T v) = 0;
        virtual NurbsControlPoint<T, controlPointType> &controlPoint(unsigned int u, unsigned int v) = 0;
        virtual void updateRemapper(NurbsRemapper<T> &uRemapper, NurbsRemapper<T> &vRemapper, int density) = 0;
    };

    template<typename T, typename controlPointType, int uDegree, int vDegree>
    class NurbsSurface : public AbstractNurbsSurface<T, controlPointType>
    {
        public:
        static const int uOrder_ = NurbsBasis<T, uDegree>::order_;
        static const int vOrder_ = NurbsBasis<T, vDegree>::order_;
        static const int uDegree_ = uDegree;
        static const int vDegree_ = vDegree;

        typedef NurbsControlPoint<T, controlPointType> ControlPoint;

        NurbsSurface(int uControlPoints, int vControlPoints)
        :   controlPoints_(uControlPoints * vControlPoints),
            uKnots_(uControlPoints + uOrder_),
            vKnots_(vControlPoints + vOrder_)
        {
            NurbsBasis<T, uDegree>::generatePinnedUniformKnots(uKnots_);
            NurbsBasis<T, vDegree>::generatePinnedUniformKnots(vKnots_);
        }

        controlPointType eval(T u, T v)
        {
            // TODO: optimize by evaluating only those basis functions that affect the current knot span
            //       and check if basis evaluation itself can be simplified after this has been taken
            //       care of (see isInRange, et al)

            T norm = 0;
            controlPointType result = 0;

            const int vControlPoints = vKnots_.size() - vOrder_;
            const int uControlPoints = uKnots_.size() - uOrder_;

            std::vector<T> uNurbs;
            std::vector<T> vNurbs;

            for(int vi = 0; vi < vControlPoints; vi++)
            {
                vNurbs.push_back(NurbsBasis<T, vDegree>::eval(vKnots_, vi, v));
            }

            for(int ui = 0; ui < uControlPoints; ui++)
            {
                uNurbs.push_back(NurbsBasis<T, uDegree>::eval(uKnots_, ui, u));
            }

            int controlPointIndex = 0;
            for(int vi = 0; vi < vControlPoints; vi++)
            {
                for(int ui = 0; ui < uControlPoints; ui++, controlPointIndex++)
                {
                    const NurbsControlPoint<T, controlPointType> &cp = controlPoints_[controlPointIndex];
                    const T basisValue = uNurbs[ui] * vNurbs[vi] * cp.weight();
                    norm += basisValue;
                    result += cp.value() * basisValue;
                }
            }

            if(norm > std::numeric_limits<T>::epsilon())
            {
                return result / norm;
            }

            return result;
        }

        NurbsSurfaceDerivatives<controlPointType> evalDerivatives(T u, T v)
        {
            //TODO: figure out if the delta needs to be adjusted based the local curvature

            static const T delta = 0.001;

            T uDelta = u + delta < uKnots_.back() ? delta : -delta;
            T vDelta = v + delta < vKnots_.back() ? delta : -delta;

            T norms[3];
            controlPointType results[3];
            for(int i = 0; i < 3; i++)
            {
                norms[i] = 0;
            }

            const int vControlPoints = vKnots_.size() - vOrder_;
            const int uControlPoints = uKnots_.size() - uOrder_;

            std::vector<T> uNurbs;
            std::vector<T> duNurbs;
            std::vector<T> vNurbs;
            std::vector<T> dvNurbs;

            uNurbs.reserve(uControlPoints);
            duNurbs.reserve(uControlPoints);
            vNurbs.reserve(vControlPoints);
            dvNurbs.reserve(vControlPoints);

            for(int ui = 0; ui < uControlPoints; ui++)
            {
                uNurbs.push_back(NurbsBasis<T, uDegree>::eval(uKnots_, ui, u));
                duNurbs.push_back(NurbsBasis<T, uDegree>::eval(uKnots_, ui, u + uDelta));
            }

            for(int vi = 0; vi < vControlPoints; vi++)
            {
                vNurbs.push_back(NurbsBasis<T, vDegree>::eval(vKnots_, vi, v));
                dvNurbs.push_back(NurbsBasis<T, vDegree>::eval(vKnots_, vi, v + vDelta));
            }

            int controlPointIndex = 0;
            for(int vi = 0; vi < vControlPoints; vi++)
            {
                for(int ui = 0; ui < uControlPoints; ui++, controlPointIndex++)
                {
                    T basisValues[3];
                    basisValues[0] = uNurbs[ui] * vNurbs[vi];
                    basisValues[1] = duNurbs[ui] * vNurbs[vi];
                    basisValues[2] = uNurbs[ui] * dvNurbs[vi];

                    const NurbsControlPoint<T, controlPointType> &cp = controlPoints_[controlPointIndex];
                    T weight = cp.weight();
                    controlPointType value = cp.value();

                    for(int i = 0; i < 3; i++)
                    {
                        basisValues[i] *= weight;
                        norms[i] += basisValues[i];
                        results[i] += value * basisValues[i];
                    }
                }
            }

            for(int i = 0; i < 3; i++)
            {
                if(norms[i] > std::numeric_limits<T>::epsilon())
                {
                    results[i] /= norms[i];
                }
            }

            return NurbsSurfaceDerivatives<controlPointType>(results[0],
                                                            (results[1] - results[0])/uDelta,
                                                            (results[2] - results[0])/vDelta);
        }

        void updateRemapper(NurbsRemapper<T> &uRemapper, NurbsRemapper<T> &vRemapper, int density)
        {
            updateRemapper(uRemapper, density, 0, 1, T(1)/2, 0);
            updateRemapper(vRemapper, density, T(1)/2, 0, 0, 1);
        }

        ControlPoint &controlPoint(unsigned int u, unsigned int v) throw(std::range_error)
        {
            const unsigned int vControlPoints = vKnots_.size() - vOrder_;
            const unsigned int uControlPoints = uKnots_.size() - uOrder_;

            if(u >= uControlPoints || v >= vControlPoints)
            {
                throw std::range_error("Control point index over the range");
            }

            return controlPoints_[v * uControlPoints + u];
        }

        private:
        void updateRemapper(NurbsRemapper<T> &remapper, int density, T uStart, T uStep, T vStart, T vStep)
        {
            std::vector<T> curveLength;
            T totalLength(0);

            controlPointType prev = eval(uStart, vStart);
            curveLength.push_back(0);
            for(int i = 1; i < density; i++)
            {
                T param = T(i) / (density - 1);
                controlPointType result = eval(nextafterf(uStart + uStep * param, -1), nextafterf(vStart + vStep * param, -1));
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
        std::vector<NurbsControlPoint<T, controlPointType> > controlPoints_;
        std::vector<T> uKnots_;
        std::vector<T> vKnots_;
    };

}

#endif
