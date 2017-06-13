// This file is part of Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef BARREL_EXPORTER
#define BARREL_EXPORTER

#define FBXSDK_NEW_API
#include <fbxsdk.h>
#define MAT_HEADER_LENGTH 200
#include "FBX2Vanda/Mesh.hpp"
#include "FBX2Vanda/Material.hpp"

class Exporter
{
public:
    Exporter();
    ~Exporter();
    void addMesh(FBX2Vanda::Mesh*);
    void addMaterial(FBX2Vanda::Material*);
    void write(std::string const& file);

private:
    std::vector<FBX2Vanda::Mesh*> meshes_;
    std::vector<FBX2Vanda::Material*> materials_;
};

#endif
