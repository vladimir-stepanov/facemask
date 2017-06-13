// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef FBX2VANDA_COMMON
#define FBX2VANDA_COMMON

#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Core/Log.hpp>

namespace FBX2Vanda
{
    class UVCoordinate
    {
    public:
        UVCoordinate(Grapevine::Vector2 const& uv):
            uv_(uv)
        {
        }

        float u()
        {
            return uv_.x();
        }

        float v()
        {
            return uv_.y();
        }

    private:
        Grapevine::Vector2 uv_;
    };

    class Normal
    {
    public:
        Normal(Grapevine::Vector3 const& normal):
            normal_(normal)
        {
        }

        float x()
        {
            return normal_.x();
        }

        float y()
        {
            return normal_.y();
        }

        float z()
        {
            return normal_.z();
        }

    private:
        Grapevine::Vector3 normal_;
    };

};

#endif
