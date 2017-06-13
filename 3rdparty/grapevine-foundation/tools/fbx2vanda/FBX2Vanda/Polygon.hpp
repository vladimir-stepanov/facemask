// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef FBX2VANDA_POLYGON
#define FBX2VANDA_POLYGON

#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Core/Log.hpp>
#include "FBX2Vanda/ControlPoint.hpp"
#include "FBX2Vanda/Common.hpp"

namespace FBX2Vanda
{
    class Polygon
    {
    public:
        Polygon()
        {
        }

        ~Polygon()
        {
        }

        void addPoint(ControlPoint* point)
        {
            points_.push_back(point);
        }

        unsigned int size()
        {
            return points_.size();
        }

        ControlPoint* controlPoint(unsigned int index)
        {
            if (index > points_.size() - 1)
            {
                Grapevine::Log::E("Asking point from a polygon from an index that does not exist.");
                return NULL;
            }
            return points_.at(index);
        }

        ControlPoint* operator[](unsigned int index)
        {
            if (index > points_.size() - 1)
            {
                Grapevine::Log::E("Asking point from a polygon from an index that does not exist.");
                return NULL;
            }
            return points_.at(index);
        }

    private:
        std::vector<ControlPoint*> points_;
    };
}

#endif
