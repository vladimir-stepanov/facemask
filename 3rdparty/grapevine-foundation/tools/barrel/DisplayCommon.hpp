// This file is based on Autodesk sample which was modified for barrel project

#ifndef BARREL_DISPLAY_COMMON
#define BARREL_DISPLAY_COMMON

#include <fbxsdk.h>

void DisplayMetaDataConnections(FbxObject* pNode);
void DisplayString(const char* pHeader, const char* pValue  = "", const char* pSuffix  = "");
void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix  = "");
void DisplayInt(const char* pHeader, int pValue, const char* pSuffix  = "");
void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix  = "");
void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix  = "");
void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix  = "");
void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix  = "");
void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix  = "");

#endif
