// This file is part of Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#ifndef BARREL_CONVERTER
#define BARREL_CONVERTER

#define FBXSDK_NEW_API
#include <fbxsdk.h>
#define MAT_HEADER_LENGTH 200
#include "Exporter.hpp"

class Converter
{
public:
    Converter(int argc, char *argv[]);
    ~Converter();

private:
    Exporter exporter_;

    void displayMetaData(FbxScene* scene);
    void initializeSdkObjects(FbxManager*&, FbxScene*&);
    bool loadScene(FbxManager*, FbxDocument*, const char* filename);

    void DisplayContent(FbxScene* pScene);
    void DisplayContent(FbxScene* scene, FbxNode* pNode);
    void DisplayTarget(FbxNode* pNode);
    void DisplayTransformPropagation(FbxNode* pNode);
    void DisplayGeometricTransform(FbxNode* pNode);
    void DisplayMetaData(FbxScene* pScene);

    float xAxisRotation_;
};

#endif
