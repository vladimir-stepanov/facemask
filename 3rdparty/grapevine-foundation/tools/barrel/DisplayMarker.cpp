#include <fbxsdk.h>

#include "DisplayCommon.hpp"

void DisplayMarker(FbxNode* pNode)
{
    FbxMarker* lMarker = (FbxMarker*)pNode->GetNodeAttribute();
    FbxString     lString;

    DisplayString("Marker Name: ", (char *) pNode->GetName());
    DisplayMetaDataConnections(lMarker);

    // Type
    lString = "    Marker Type: ";
    switch (lMarker->GetType())
    {
    case FbxMarker::eStandard:    lString += "Standard";    break;
    case FbxMarker::eOptical:     lString += "Optical";     break;
    case FbxMarker::eEffectorIK: lString += "IK Effector"; break;
    case FbxMarker::eEffectorFK: lString += "FK Effector"; break;
    }
    DisplayString(lString.Buffer());

    // Look
    lString = "    Marker Look: ";
    switch (lMarker->Look.Get())
    {
    case FbxMarker::eCube:         lString += "Cube";        break;
    case FbxMarker::eHardCross:   lString += "Hard Cross";  break;
    case FbxMarker::eLightCross:  lString += "Light Cross"; break;
    case FbxMarker::eSphere:       lString += "Sphere";      break;
    }
    DisplayString(lString.Buffer());

    // Size
    lString = FbxString("    Size: ") + FbxString(lMarker->Size.Get());
    DisplayString(lString.Buffer());

    // Color
    FbxDouble3 c = lMarker->Color.Get();
    FbxColor color(c[0], c[1], c[2]);
    DisplayColor("    Color: ", color);

    // IKPivot
    Display3DVector("    IKPivot: ", lMarker->IKPivot.Get());
}

