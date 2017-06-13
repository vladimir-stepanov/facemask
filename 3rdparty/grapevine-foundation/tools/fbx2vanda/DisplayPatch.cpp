#include "DisplayPatch.hpp"
#include "DisplayTexture.hpp"
#include "DisplayMaterial.hpp"
#include "DisplayLink.hpp"
#include "DisplayShape.hpp"

void DisplayPatch(FbxNode* pNode)
{
    FbxPatch* lPatch = (FbxPatch*) pNode->GetNodeAttribute();

    DisplayString("Patch Name: ", (char *) pNode->GetName());
    DisplayMetaDataConnections(lPatch);

    const char* lSurfaceModes[] = { "Raw", "Low No Normals", "Low", "High No Normals", "High" };

    DisplayString("    Surface Mode: ", lSurfaceModes[lPatch->GetSurfaceMode()]);

    int i, lControlPointsCount = lPatch->GetControlPointsCount();
    FbxVector4* lControlPoints = lPatch->GetControlPoints();

    for (i = 0; i < lControlPointsCount; i++)
    {
        DisplayInt("    Control Point ", i);
        Display3DVector("        Coordinates: ", lControlPoints[i]);
        DisplayDouble("        Weight: ", lControlPoints[i][3]);
    }

    const char* lPatchTypes[] = { "Bezier", "Bezier Quadric", "Cardinal", "B-Spline", "Linear" };

    DisplayString("    Patch U Type: ", lPatchTypes[lPatch->GetPatchUType()]);
    DisplayInt("    U Count: ", lPatch->GetUCount());
    DisplayString("    Patch V Type: ", lPatchTypes[lPatch->GetPatchVType()]);
    DisplayInt("    V Count: ", lPatch->GetVCount());
    DisplayInt("    U Step: ", lPatch->GetUStep());
    DisplayInt("    V Step: ", lPatch->GetVStep());
    DisplayBool("    U Closed: ", lPatch->GetUClosed());
    DisplayBool("    V Closed: ", lPatch->GetVClosed());
    DisplayBool("    U Capped Top: ", lPatch->GetUCappedTop());
    DisplayBool("    U Capped Bottom: ", lPatch->GetUCappedBottom());
    DisplayBool("    V Capped Top: ", lPatch->GetVCappedTop());
    DisplayBool("    V Capped Bottom: ", lPatch->GetVCappedBottom());

    DisplayString("");

    DisplayTexture(lPatch);
    DisplayMaterial(lPatch);
    DisplayLink(lPatch);
    DisplayShape(lPatch);
}


