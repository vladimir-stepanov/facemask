// This file is part of Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef BARREL_EXPORTER
#define BARREL_EXPORTER

#define FBXSDK_NEW_API
#include <fbxsdk.h>
#define MAT_HEADER_LENGTH 200
#include "Barrel/Mesh.hpp"
#include "Barrel/Material.hpp"

class Exporter
{
public:
    Exporter();
    ~Exporter();
    void addMesh(Barrel::Mesh*);
    void addMaterial(Barrel::Material*);
    void write(std::string const& file);

private:
    std::vector<Barrel::Mesh*> meshes_;
    std::vector<Barrel::Material*> materials_;
};

#endif
