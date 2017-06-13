// This file is part of Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#include "Exporter.hpp"
#include <Grapevine/Core/Exception.hpp>
#include <iostream>
#include <fstream>

Exporter::Exporter()
{
}

Exporter::~Exporter()
{
}

void Exporter::addMesh(Barrel::Mesh* mesh)
{
    meshes_.push_back(mesh);
}

void Exporter::addMaterial(Barrel::Material* material)
{
    materials_.push_back(material);
}

void Exporter::write(std::string const& fileOut)
{
    typedef std::map<std::string, int> MaterialIDs;
    MaterialIDs materialIDs;

    Grapevine::Log::D("Linking materials to meshes...");

    // Assign IDs for materials
    int j = 0;
    for (std::vector<Barrel::Material*>::iterator i = materials_.begin(); i != materials_.end(); ++i)
    {
        Barrel::Material* barrelMaterial = *i;
        Grapevine::Log::D("material '%s', id %d", barrelMaterial->getMaterialName()->c_str(), j);
        barrelMaterial->setMaterialID(j);
        materialIDs[barrelMaterial->getMaterialName()->c_str()] = j++;
    }

    // Find materials and their IDs for every mesh
    for (std::vector<Barrel::Mesh*>::iterator i = meshes_.begin(); i != meshes_.end(); ++i)
    {
        Barrel::Mesh* barrelMesh = *i;
        int materialID = -1;
        MaterialIDs::iterator j = materialIDs.find(barrelMesh->getMaterialName()->c_str());
        if(j == materialIDs.end())
        {
            Grapevine::Log::D("mesh '%s' has no material", barrelMesh->getName()->c_str());
        }
        else
        {
            materialID = j->second;
            Grapevine::Log::D("mesh '%s' linked to material '%s' having id %d", barrelMesh->getName()->c_str(), j->first.c_str(), materialID);
        }
        barrelMesh->setMaterialID(materialID);
    }

    std::ofstream fileStream;
    fileStream.open (fileOut.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    fileStream << "[BARRELSTART]";

    Grapevine::Log::D("Export meshes...");
    for (std::vector<Barrel::Mesh*>::iterator i = meshes_.begin(); i != meshes_.end(); ++i)
    {
        Barrel::Mesh* barrelMesh = *i;
        Grapevine::Log::D("[MESH]");
        fileStream << "[MESH]";
        barrelMesh->write(fileStream);
    }

    Grapevine::Log::D("Export materials...");
    for (std::vector<Barrel::Material*>::iterator i = materials_.begin(); i != materials_.end(); ++i)
    {
        Barrel::Material* barrelMaterial = *i;
        Grapevine::Log::D("[MATERIAL]");
        fileStream << "[MATERIAL]";
        barrelMaterial->write(fileStream);
    }

    Grapevine::Log::D("end write...");
    fileStream << "[BARRELEND]";
    fileStream.close();
    Grapevine::Log::D("end done.");
}

