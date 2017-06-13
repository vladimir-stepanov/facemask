/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_EASINGCURVE
#define GRAPEVINE_MATH_EASINGCURVE

#include "Grapevine/Math/Math.hpp"
#include "Grapevine/Math/Vector.hpp"
#include <cmath>
#include <complex>

namespace Grapevine
{
    struct EasingCurve
    {
    public:
        /*
          ***
          *******
          **********
          ************
          *************
          **************
         */
        static float convexCirc(float value)
        {
            if (value < 0.0f) {
                value = 0.0f;
            }
            if (value > 1.0f) {
                value = 1.0f;
            }
            value = 1.0f - value;

            return sqrt(1.0f - value * value);
        }

        /*
          *
          ******
          ***********
          ************
          ************
          ***********
          ******
          *
         */
        static float sinFromThreeQuarters(float value)
        {
            float degrees = 270.0f + value * 360.0f;
            float r = sin(degrees * Math::Pi / 180.0f);
            r += 1.0f;
            r /= 2.0f;
            return r;
        }

        /**
         * @param [in]  t       current time [0 ... 1]
         */
        static inline float easeInOutExpo(float t)
        {
            if(t == 0) return 0;
            if(t == 1) return 1;
            t *= 2;
            if(t < 1)
            {
                return 0.5f * pow(2, 10 * (t - 1));
            }
            return 0.5f * (-pow(2, -10 * (t - 1)) + 2);
        };

        /**
         * @param [in]  t       current time [0 ... 1]
         */
        static inline float easeInOutQuad(float t)
        {
            t *= 2;
            if(t < 1)
            {
                return 0.5f * t * t;
            }
            t -= 1;
            return -0.5f * ((t)*(t-2) - 1);
        }

        static inline float easeOutSine(float t)
        {
            return sin(t * (float)Math::Pi / 2);
        }

        static inline float easeInOutQuart(float t)
        {
            t *= 2;
            if( t < 1)
            {
                return 0.5f*t*t*t*t;
            }
            else
            {
                t -= 2;
                return -0.5f * (t*t*t*t - 2);
            }
        }

        static inline float easeOutQuad(float t)
        {
	       return -t*(t-2);
        }

        static inline float easeInOutSine(float t)
        {
            return -0.5f * (cos(Math::Pi*t) - 1) ;
        }

        /**
         *  3 even sinewave bounces from t = 0.25
         */
        static inline float easeOutSineBounce(float t)
        {
            const float start = 0.05f;
            const float bounce = 0.7f;
            const float length = 1.f - start;

            if(t > start)
            {
                const float bounceT = (t - start) / length;
                // bounce around 1 with radius of bounce * 2
                return 1.f + bounce * sinf(5.f * float(Math::Pi) * bounceT) * (1.f - bounceT * bounceT * bounceT * bounceT);

            }
            else
            {
                return t / start; // linear 0 .. 1 until start of the bounce
            }
        }

        /**
         * @brief Easing curve defined as cubic bezier
         * @details First and last control points are 0,0 and 1,1, while second and third
         *          can be supplied by user.
         *
         * @param t value in range 0 to 1. T will be projected to bezier curve to find y
         * @param p1 Second control point
         * @param p2 Third control point
         */
        static inline float easeBezier(float t, Vector2 p1, Vector2 p2)
        {
            static const auto invertBezier = [](float t, float x1, float x2) -> float // x0 == 0, x3 == 1
            {
                const float epsilon = 0.000001f;

                const float c = x1 * 3;
                const float b = 3*x2 - 6*x1;
                const float a = 3*x1 - 3*x2 + 1;
                const float d = -t;

                const float bb = b * b;
                const float ac = a * c;

                const float delta0 = bb - 3*ac;
                const float delta1 = 2*bb*b - 9*ac*b + 27*a*a*d;

                auto r = std::complex<float>(delta1 * delta1 - 4 * delta0 * delta0 * delta0);
                auto sqroot = std::sqrt(r);
                auto rc = 0.5f * (delta1 + (delta1 < 0 ? -1.f : 1.f) * sqroot);
                auto C = std::pow(rc, 1.f/3);

                if (std::abs(C.real()) < epsilon && std::abs(C.imag()) < epsilon) {
                    float r = -b/(3*a);
                    if(r>=0.0f && r<=0.0f){
                        return r;
                    }
                }

                for (auto l:
                    {
                        std::complex<float>( 1.f,  0.f),
                        std::complex<float>(-0.5,  0.5*std::sqrt(3)),
                        std::complex<float>(-0.5, -0.5*std::sqrt(3))
                    })
                {
                    auto alphac = -1.f/(3 * a) * (b + l * C + delta0/(l * C));

                    if (std::abs(alphac.imag()) < epsilon && alphac.real()>=0.0f && alphac.real() <= 1.0f) {
                        return alphac.real();
                    }
                }

                return 0;
            };

            const float alpha = invertBezier(t, p1.x(), p2.x());

            const float ca2 = 1 - alpha;
            const float ca1 = ca2 * ca2;

            const float cb1 = alpha;
            const float cb2 = cb1 * alpha;
            const float cb3 = cb2 * cb1;

            const float c1 = 3 * ca1 * cb1;
            const float c2 = 3 * ca2 * cb2;
            const float c3 = cb3;

            return p1.y()*c1 + p2.y()*c2 + c3;
        }

    };
}

#endif
