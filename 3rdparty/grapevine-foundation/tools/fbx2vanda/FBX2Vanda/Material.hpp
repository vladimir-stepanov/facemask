// This file is part of FBX2Vanda project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef FBX2VANDA_MATERIAL
#define FBX2VANDA_MATERIAL

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
    class Material
    {
    public:
        Material();
        ~Material();
        void setMaterialName(std::string const& name);  // Material name in 3ds max
        std::string* getMaterialName(void);
        void setMaterialID(int id);
        void setColorMapFile(std::string const& name);  // Ambient texture in 3ds max
        void setNormalMapFile(std::string const& name); // Bump texture in max
        void setSpecularFactor(float factor);
        void setGlossinessFactor(float factor);
        void setReflectionFactor(float factor);
        void write(std::ofstream& fileStream);

    private:
        std::string materialName_;
        std::string colorMapFilename_;
        std::string normalMapFilename_;
        float specularFactor_;
        float glossinessFactor_;
        float reflectionFactor_;
        int materialID_;
    };
}

#endif
