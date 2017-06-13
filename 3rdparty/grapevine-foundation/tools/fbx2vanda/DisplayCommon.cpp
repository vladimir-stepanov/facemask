// This file is based on Autodesk sample which was modified for barrel project

#include "DisplayCommon.hpp"
#if defined (FBXSDK_ENV_MAC)
// disable the format not a string literal and no format arguments warning since
// the FBXSDK_printf calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif
#include <Grapevine/Core/Log.hpp>

void DisplayMetaDataConnections(FbxObject* pObject)
{
#ifdef DEBUG
    int nbMetaData = pObject->GetSrcObjectCount<FbxObjectMetaData>();
    if (nbMetaData > 0)
        DisplayString("    MetaData connections ");

    for (int i = 0; i < nbMetaData; i++)
    {
        FbxObjectMetaData* metaData = pObject->GetSrcObject<FbxObjectMetaData>(i);
        DisplayString("        Name: ", (char*)metaData->GetName());
    }
#endif
}

void DisplayString(const char* pHeader, const char* pValue /* = "" */, const char* pSuffix /* = "" */)
{
#ifdef DEBUG
    FbxString lString;

    lString = pHeader;
    lString += pValue;
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}

void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix /* = "" */)
{
#ifdef DEBUG
    FbxString lString;

    lString = pHeader;
    lString += pValue ? "true" : "false";
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}

void DisplayInt(const char* pHeader, int pValue, const char* pSuffix /* = "" */)
{
#ifdef DEBUG
    FbxString lString;

    lString = pHeader;
    lString += pValue;
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}

void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix /* = "" */)
{
    /*FbxString lString;
    FbxString lFloatValue = (float) pValue;
    lFloatValue = pValue <= -HUGE_VAL ? "-INFINITY" : lFloatValue.Buffer();
    lFloatValue = pValue >=  HUGE_VAL ?  "INFINITY" : lFloatValue.Buffer();
    lString = pHeader;
    lString += lFloatValue;
    lString += pSuffix;*/
    LOGD("%s: %.2lf", pHeader, pValue);
}

void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix  /* = "" */)
{
#ifdef DEBUG
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}


void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix /* = "" */)
{
#ifdef DEBUG
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];
    FbxString lFloatValue3 = (float)pValue[2];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
    lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
    lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += ", ";
    lString += lFloatValue3;
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}

void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix /* = "" */)
{
#ifdef DEBUG
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];
    FbxString lFloatValue3 = (float)pValue[2];
    FbxString lFloatValue4 = (float)pValue[3];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >=  HUGE_VAL ?  "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >=  HUGE_VAL ?  "INFINITY" : lFloatValue2.Buffer();
    lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
    lFloatValue3 = pValue[2] >=  HUGE_VAL ?  "INFINITY" : lFloatValue3.Buffer();
    lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
    lFloatValue4 = pValue[3] >=  HUGE_VAL ?  "INFINITY" : lFloatValue4.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += ", ";
    lString += lFloatValue3;
    lString += ", ";
    lString += lFloatValue4;
    lString += pSuffix;
    lString += "\n";
    FBXSDK_printf(lString);
#endif
}


void DisplayColor(const char* pHeader, FbxPropertyT<FbxDouble3> pValue, const char* pSuffix /* = "" */)
{
//#ifdef DEBUG
   FbxString lString;

    lString = pHeader;
    //lString += (float) pValue.mRed;
    //lString += (double)pValue.GetArrayItem(0);
    lString += " (red), ";
    //lString += (float) pValue.mGreen;
    //lString += (double)pValue.GetArrayItem(1);
    lString += " (green), ";
    //lString += (float) pValue.mBlue;
    //lString += (double)pValue.GetArrayItem(2);
    lString += " (blue)";
    lString += pSuffix;
    LOGD("%s", lString.Buffer());
    //FBXSDK_printf(lString);
//#endif
}


void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix /* = "" */)
{
    FbxString lString;

    /*lString = pHeader;
    lString += (float) pValue.mRed;
    lString += " (red), ";
    lString += (float) pValue.mGreen;
    lString += " (green), ";
    lString += (float) pValue.mBlue;
    lString += " (blue)";
    lString += pSuffix;*/
    LOGD("%s R %.2f, G %.2f, B %.2f", pHeader, (float)pValue.mRed, (float)pValue.mGreen, (float)pValue.mBlue);
}

