// This file is based on Autodesk sample which was modified for barrel project

#ifndef BARREL_DISPLAY_MESH_H
#define BARREL_DISPLAY_MESH_H

#include "DisplayCommon.hpp"
#include "Exporter.hpp"
#include "Barrel/Mesh.hpp"

void DisplayMesh(FbxScene* scene, FbxNode* pNode, Exporter& exporter, float xRotation);

#endif
