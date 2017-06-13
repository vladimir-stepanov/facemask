#include <fbxsdk.h>
#include <Grapevine/Core/Log.hpp>

#if defined (FBXSDK_ENV_MAC)
// disable the “format not a string literal and no format arguments” warning since
// the FBXSDK_printf calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

// Local functions prototype.
void DisplayHierarchy(FbxNode* pNode, int pDepth);

void DisplayHierarchy(FbxScene* pScene)
{
    LOGD("DisplayHierarchy");
    int i;
    FbxNode* lRootNode = pScene->GetRootNode();

    for( i = 0; i < lRootNode->GetChildCount(); i++)
    {
        DisplayHierarchy(lRootNode->GetChild(i), 0);
    }
}

void DisplayHierarchy(FbxNode* pNode, int pDepth)
{
    FbxString lString;
    int i;

    for(i = 0; i < pDepth; i++)
    {
        lString += "     ";
    }

    lString += pNode->GetName();
    lString += "\n";

    FBXSDK_printf(lString.Buffer());

    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayHierarchy(pNode->GetChild(i), pDepth + 1);
    }
}


