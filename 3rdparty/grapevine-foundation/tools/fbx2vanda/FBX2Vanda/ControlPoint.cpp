// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#include "FBX2Vanda/ControlPoint.hpp"

namespace FBX2Vanda
{
    ControlPoint::ControlPoint(uint32_t index, Grapevine::Vector3 const& location):
        index_(index),
        location_(location)
    {
    }

    ControlPoint::~ControlPoint()
    {
    }

    void ControlPoint::addUVCoordinate(UVCoordinate* coordinate, unsigned int channel)
    {
        // Channel creation if needed
        while ((channel + 1) > uvChannels_.size())
        {
            uvChannels_.push_back(NULL);
        }

        // Delete old one if exists
        if (uvChannels_[channel] != NULL)
        {
            delete uvChannels_[channel];
        }
        uvChannels_[channel] = coordinate;
    }

    UVCoordinate* ControlPoint::uvCoordinate(unsigned int channel)
    {
        UVCoordinate* r = NULL;

        if (channel >= uvChannels_.size())
        {
            Grapevine::Log::W("Asking UVCoordinate from channel that does not exist.");
        }
        else
        {
            r = uvChannels_[channel];
        }

        return r;
    }

    void ControlPoint::addNormal(Normal* normal, unsigned int channel)
    {
        // Channel creation if needed
        while ((channel + 1) > normalChannels_.size())
        {
            normalChannels_.push_back(NULL);
        }

        // Delete old one if exists
        if (normalChannels_[channel] != NULL)
        {
            delete normalChannels_[channel];
        }

        normalChannels_[channel] = normal;
    }

    Normal* ControlPoint::normal(unsigned int channel)
    {
        Normal* r = NULL;

        if (channel >= normalChannels_.size())
        {
            Grapevine::Log::W("Asking Normal from channel that does not exist.");
        }
        else
        {
            r = normalChannels_[channel];
        }

        return r;
    }

    uint32_t ControlPoint::index()
    {
        return index_;
    }

    unsigned int ControlPoint::byteSize()
    {
        return index_;
    }

    unsigned int ControlPoint::uvCoordinateChannels()
    {
        return uvChannels_.size();
    }

    unsigned int ControlPoint::normalChannels()
    {
        return normalChannels_.size();
    }

    void ControlPoint::write(std::ofstream& fileStream, unsigned int uvCoordinateChannels, unsigned int normalChannels)
    {
        ;
    }

    Grapevine::Vector3 ControlPoint::location()
    {
        return location_;
    }
}
