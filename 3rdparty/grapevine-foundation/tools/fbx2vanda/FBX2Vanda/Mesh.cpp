// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#include "FBX2Vanda/Mesh.hpp"
#include <stdint.h>
#include <Grapevine/Core/Exception.hpp>

namespace FBX2Vanda
{
    Mesh::Mesh():
        name_(std::string("FBX2Vanda_noNameGiven")),
        materialID_(-1)
    {
    }

    void Mesh::setName(std::string name)
    {
        name_ = name;
    }

    std::string* Mesh::getName(void)
    {
        return &name_;
    }

    void Mesh::addPoint(FBX2Vanda::ControlPoint* point)
    {
        points_.insert(std::make_pair(point->index(), point));
    }

    FBX2Vanda::ControlPoint* Mesh::point(unsigned int index)
    {
        std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator iterator;
        iterator = points_.find(index);
        if (iterator == points_.end())
        {
            throw Grapevine::Exception("FBX2Vanda::Mesh::point(), index overflow");
        }
        return (*iterator).second;
    }

    void Mesh::addPolygon(FBX2Vanda::Polygon* polygon)
    {
        polygons_.push_back(polygon);
    }

    unsigned int Mesh::uvCoordinateChannels()
    {
        unsigned int size = 0;
        for (std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator i = points_.begin(); i != points_.end(); ++i)
        {
            ControlPoint* point = (*i).second;
            if (point->uvCoordinateChannels() > size)
            {
                size = point->uvCoordinateChannels();
            }
        }
        return size;
    }

    unsigned int Mesh::normalChannels()
    {
        unsigned int size = 0;
        for (std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator i = points_.begin(); i != points_.end(); ++i)
        {
            ControlPoint* point = (*i).second;
            if (point->normalChannels() > size)
            {
                size = point->normalChannels();
            }
        }
        return size;
    }

    void Mesh::setMaterialName(std::string materialName)
    {
        materialName_ = materialName;
    }

    std::string* Mesh::getMaterialName(void)
    {
        return &materialName_;
    }

    void Mesh::setMaterialID(int materialID)
    {
        materialID_ = materialID;
    }

    void Mesh::write(std::ofstream& fileStream)
    {
        // write header
        Grapevine::Log::D("write header");

        // 32bit unsigned int: amount of polygons
        uint32_t polygonCount = polygons_.size();
        fileStream.write(reinterpret_cast<char const*>(&polygonCount), sizeof(polygonCount));

        // polygonCount * (8 * 4byte float (position, normal, uv))
        uint32_t polygonIndex = 0;
        Grapevine::Log::D("write polygons, count: %d", polygonCount);
        for (std::vector<FBX2Vanda::Polygon*>::iterator i = polygons_.begin(); i != polygons_.end(); ++i)
        {
            FBX2Vanda::Polygon* polygon = *i;
            if (polygon->size() != 3)
            {
                Grapevine::Log::E("Triangle polygons only supported! Polygon %d had %d indices!", polygonIndex, polygon->size());
            }

            uint32_t p0i = polygon->controlPoint(0)->index();
            uint32_t p1i = polygon->controlPoint(1)->index();
            uint32_t p2i = polygon->controlPoint(2)->index();

            // Digg out the points and write their data to file
            std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator p0 = points_.find(p0i);
            std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator p1 = points_.find(p1i);
            std::map<unsigned int, FBX2Vanda::ControlPoint*>::iterator p2 = points_.find(p2i);
            writePoint((*p0).second, fileStream);
            writePoint((*p1).second, fileStream);
            writePoint((*p2).second, fileStream);

            polygonIndex++;
        }
    }

    void Mesh::writePoint(FBX2Vanda::ControlPoint* point, std::ofstream& fileStream)
    {
        Grapevine::Vector3 location = point->location();
        float x = location.x();
        float y = location.y();
        float z = location.z();
        fileStream.write(reinterpret_cast<char const*>(&x), sizeof(x));
        fileStream.write(reinterpret_cast<char const*>(&y), sizeof(y));
        fileStream.write(reinterpret_cast<char const*>(&z), sizeof(z));

        unsigned int normalChannel = 0;
        {
            Normal* normal = point->normal(normalChannel);
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (normal == NULL)
            {
                Grapevine::Log::W("normalChannel was NULL! Writing 0.0f values.");
            }
            else
            {
                x = normal->x();
                y = normal->y();
                z = normal->z();
            }
            fileStream.write(reinterpret_cast<char const*>(&x), sizeof(x));
            fileStream.write(reinterpret_cast<char const*>(&y), sizeof(y));
            fileStream.write(reinterpret_cast<char const*>(&z), sizeof(z));
            // NOTE: we write one and only channel-> the first one. Other channels are not supported in .vanda format
        }

        unsigned int uvChannel = 0;
        {
            UVCoordinate* uvCoordinate = point->uvCoordinate(uvChannel);
            float u = 0.0f;
            float v = 0.0f;
            if (uvCoordinate == NULL)
            {
                Grapevine::Log::W("uvCoordinate was NULL! Writing 0.0f values.");
            }
            else
            {
                u = uvCoordinate->u();
                v = uvCoordinate->v();
            }

            fileStream.write(reinterpret_cast<char const*>(&u), sizeof(u));
            fileStream.write(reinterpret_cast<char const*>(&v), sizeof(v));
            // NOTE: we write one and only channel-> the first one. Other channels are not supported in .vanda format
        }
    }
}
