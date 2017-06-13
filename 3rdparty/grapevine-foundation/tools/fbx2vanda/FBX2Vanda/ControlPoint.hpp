// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef FBX2VANDA_CONTROLPOINT
#define FBX2VANDA_CONTROLPOINT

#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Core/Log.hpp>
#include "FBX2Vanda/Common.hpp"
#include <stdint.h>
#include <vector>

namespace FBX2Vanda
{
    class ControlPoint
    {
    public:
        ControlPoint(uint32_t index, Grapevine::Vector3 const& location);
        ~ControlPoint();
        void addUVCoordinate(UVCoordinate* coordinate, unsigned int channel);
        UVCoordinate* uvCoordinate(unsigned int channel);
        void addNormal(Normal* normal, unsigned int channel);
        Normal* normal(unsigned int channel);
        uint32_t index();
        unsigned int byteSize();
        unsigned int uvCoordinateChannels();
        unsigned int normalChannels();
        void write(std::ofstream& fileStream, unsigned int uvCoordinateChannels, unsigned int normalChannels);
        Grapevine::Vector3 location();

    private:
        uint32_t index_;
        Grapevine::Vector3 location_;
        std::vector<UVCoordinate*> uvChannels_;
        std::vector<Normal*> normalChannels_;
    };
}

#endif
