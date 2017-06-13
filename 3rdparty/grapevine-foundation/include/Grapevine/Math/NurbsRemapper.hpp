/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NURBSREMAPPER
#define GRAPEVINE_MATH_NURBSREMAPPER

#include <vector>
#include <cassert>
#include <stdexcept>

namespace Grapevine
{
    template<typename T>
    class NurbsRemapper
    {
        public:
            template<typename U, typename controlPointType, int degree>
            friend class NurbsCurve;

            template<typename U, typename controlPointType, int uDegree, int vDegree>
            friend class NurbsSurface;

            NurbsRemapper()
            {}

            T remap(T param) const
            {
                if(paramMap_.size())
                {
                    const int lastIndex = paramMap_.size() - 1;
                    T floatingIndex = lastIndex * param;

                    int startIndex = int(floatingIndex);
                    int endIndex = startIndex + 1;

                    T lerpFactor = floatingIndex - startIndex;

                    // clamp indices
                    startIndex = std::min(std::max(startIndex, 0), lastIndex);
                    endIndex = std::min(std::max(endIndex, 0), lastIndex);

                    T start = paramMap_[startIndex];
                    T end = paramMap_[endIndex];

                    return  start + (end - start) * lerpFactor;
                }

                return param;
            }

        private:
            std::vector<T> paramMap_;
    };

}

#endif
