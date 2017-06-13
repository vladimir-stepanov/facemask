#include <fbxsdk.h>
#include "DisplayCommon.hpp"
#include "DisplayCamera.hpp"

void DisplayGlobalLightSettings(FbxGlobalSettings* pGlobalSettings)
{
    DisplayColor("Ambient Color: ", pGlobalSettings->GetAmbientColor());
    DisplayString("");
}

void DisplayGlobalCameraSettings(FbxGlobalSettings* pGlobalSettings)
{
    DisplayString("Default Camera: ", pGlobalSettings->GetDefaultCamera());
    DisplayString("");
}

void DisplayGlobalTimeSettings(FbxGlobalSettings* pGlobalSettings)
{
    char lTimeString[256];

    DisplayString("Time Mode : ", FbxGetTimeModeName(pGlobalSettings->GetTimeMode()));

    FbxTimeSpan lTs;
    FbxTime     lStart, lEnd;
    pGlobalSettings->GetTimelineDefaultTimeSpan(lTs);
    lStart = lTs.GetStart();
    lEnd   = lTs.GetStop();
    DisplayString("Timeline default timespan: ");
    DisplayString("     Start: ", lStart.GetTimeString(lTimeString, FbxUShort(256)));
    DisplayString("     Stop : ", lEnd.GetTimeString(lTimeString, FbxUShort(256)));

    DisplayString("");
}
