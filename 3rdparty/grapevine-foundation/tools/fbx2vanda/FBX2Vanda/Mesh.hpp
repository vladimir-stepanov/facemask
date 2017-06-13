// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef FBX2VANDA_MESH
#define FBX2VANDA_MESH

#include "FBX2Vanda/Common.hpp"
#include "FBX2Vanda/ControlPoint.hpp"
#include "FBX2Vanda/Polygon.hpp"
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

namespace FBX2Vanda
{
    class Mesh
    {
    public:
        Mesh();
        ~Mesh();
        void setName(std::string name);
        std::string* getName(void);
        void addPoint(FBX2Vanda::ControlPoint* point);
        FBX2Vanda::ControlPoint* point(unsigned int index);
        void addPolygon(FBX2Vanda::Polygon* polygon);
        void setMaterialName(std::string name);
        std::string *getMaterialName(void);
        void setMaterialID(int);
        void write(std::ofstream& fileStream);

    private:
        void writePoint(FBX2Vanda::ControlPoint* point, std::ofstream& fileStream);
        unsigned int uvCoordinateChannels();
        unsigned int normalChannels();
        std::string name_;
        std::map<unsigned int, FBX2Vanda::ControlPoint*> points_;
        std::vector<FBX2Vanda::Polygon*> polygons_;
        std::string materialName_;
        int materialID_;
    };
}

#endif
