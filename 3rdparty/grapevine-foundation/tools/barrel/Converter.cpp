// This file is part of Grapevine's Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#include "Converter.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#define FBXSDK_NEW_API
#include <fbxsdk.h>
//#include <fbxsdk_compatibility.h>
#define MAT_HEADER_LENGTH 200
#include "DisplayCommon.hpp"
#include "DisplayHierarchy.hpp"
#include "DisplayAnimation.hpp"
#include "DisplayMarker.hpp"
#include "DisplaySkeleton.hpp"
#include "DisplayMesh.hpp"
#include "DisplayNurb.hpp"
#include "DisplayPatch.hpp"
#include "DisplayLodGroup.hpp"
#include "DisplayCamera.hpp"
#include "DisplayLight.hpp"
#include "DisplayGlobalSettings.hpp"
#include "DisplayPose.hpp"
#include "DisplayPivotsAndLimits.hpp"
#include "DisplayUserProperties.hpp"
#include "DisplayGenericInfo.hpp"
#include "DisplayCommon.hpp"

using namespace Grapevine;

#ifdef IOS_REF
    #undef  IOS_REF
    #define IOS_REF (*(pManager->GetIOSettings()))
#endif

Converter::Converter(int argc, char *argv[]):
    xAxisRotation_(0.0f)
{
    FbxManager* sdkManager = NULL;
    FbxScene* scene = NULL;

    // Prepare the FBX SDK
    initializeSdkObjects(sdkManager, scene);

    // The example can take a FBX file as an argument
    std::string fileIn("");
    std::string fileOut("");
    if (argc != 3)
    {
        throw Grapevine::Exception("\nUsage:\n ./barrel <IN> <OUT>");
    }
    else
    {
        fileIn = argv[1];
        fileOut = argv[2];
    }

    bool result = false;
    LOGI("Opening a file: %s", fileIn.c_str());
    result = loadScene(sdkManager, scene, fileIn.c_str());

    // Coordinate system
    LOGI("Coordinate system...");

    FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem engineAxisSystem(FbxAxisSystem::OpenGL);
    if (sceneAxisSystem != engineAxisSystem)
    {
        LOGI("convert KFbxAxisSystem");
        engineAxisSystem.ConvertScene(scene);     // convert the scene to engine specs
    }

    if (result == false)
    {
        LOGE("An error occurred while loading the scene...");
    }
    else
    {
        //DisplayMetaData(scene);

        //FBXSDK_printf("\n\n---------------------\nGlobal Light Settings\n---------------------\n\n");
        //DisplayGlobalLightSettings(&scene->GetGlobalSettings());

        //FBXSDK_printf("\n\n----------------------\nGlobal Camera Settings\n----------------------\n\n");
        //DisplayGlobalCameraSettings(&scene->GetGlobalSettings());

        //FBXSDK_printf("\n\n--------------------\nGlobal Time Settings\n--------------------\n\n");
        //DisplayGlobalTimeSettings(&scene->GetGlobalSettings());

        //FBXSDK_printf("\n\n---------\nHierarchy\n---------\n\n");
        DisplayHierarchy(scene);

        //FBXSDK_printf("\n\n------------\nNode Content\n------------\n\n");
        DisplayContent(scene);

        //FBXSDK_printf("\n\n----\nPose\n----\n\n");
        DisplayPose(scene);

        //FBXSDK_printf("\n\n---------\nAnimation\n---------\n\n");
        //DisplayAnimation(scene);

        //FBXSDK_printf("\n\n---------\nGeneric Information\n---------\n\n");
        //DisplayGenericInfo(scene);
    }

    // Destroy all objects created by the FBX SDK.
    //destroySdkObjects(sdkManager, result);

    LOGI("Writing to a file: %s", fileOut.c_str());
    exporter_.write(fileOut);
}

Converter::~Converter()
{
}

void Converter::initializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    // The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if( !pManager )
    {
        FBXSDK_printf("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
    else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

    // Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    // Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    pManager->LoadPluginsDirectory(lPath.Buffer());

    // Create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "My Scene");
    if( !pScene )
    {
        FBXSDK_printf("Error: Unable to create FBX scene!\n");
        exit(1);
    }
}

bool Converter::loadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if (!lImportStatus)
    {
        FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        //FBXSDK_printf("Error returned: %s\n\n", lImporter->GetLastErrorString());
        FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
        FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        /*if (lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedYet ||
            lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedAnymore)
        {

        }*/

        return false;
    }

    FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        FBXSDK_printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        FBXSDK_printf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            FBXSDK_printf("    Animation Stack %d\n", i);
            FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported
            // under a different name.
            FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported.
            FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            FBXSDK_printf("\n");
        }

        // Set the import states. By default, the import states are always set to
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if (lStatus == false /* && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError*/)
    {
        FBXSDK_printf("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
        scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END

        FbxString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        //if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
        //{
            FBXSDK_printf("\nPassword is wrong, import aborted.\n");
        //}
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}


void Converter::DisplayContent(FbxScene* scene)
{
    LOGD("KFbxAxisSystem:");
    FbxAxisSystem lAxisSystemReference = scene->GetGlobalSettings().GetAxisSystem();

    int lUpVectorSign = 1;
    int lFrontVectorSign = 1;

    //get upVector and its sign.
    FbxAxisSystem::EUpVector upVector = lAxisSystemReference.GetUpVector( lUpVectorSign );

    //get FrontVector and its sign.
    FbxAxisSystem::EFrontVector frontVector = lAxisSystemReference.GetFrontVector( lFrontVectorSign );

    //get Coordsystem.
    FbxAxisSystem::ECoordSystem coordSystem = lAxisSystemReference.GetCoorSystem();

    FbxAxisSystem OpenGL; // we desire to convert the scene from Y-Up to Z-Up
    OpenGL.ConvertScene(scene);

    switch (upVector)
    {
        case FbxAxisSystem::eXAxis:
            LOGD("upVector: eXAxis"); break;
        case FbxAxisSystem::eYAxis:
            LOGD("upVector: eYAxis"); break;
        case FbxAxisSystem::eZAxis:
            LOGD("upVector: eZAxis"); break;
    }
    switch (frontVector)
    {
        case FbxAxisSystem::eParityEven:
            LOGD("frontVector: eParityEven"); break;
        case FbxAxisSystem::eParityOdd:
            LOGD("frontVector: eParityOdd"); break;
    }
    switch (coordSystem)
    {
        case FbxAxisSystem:: eRightHanded:
            LOGD("coordSystem: eRightHanded"); break;
        case FbxAxisSystem:: eLeftHanded:
            LOGD("coordSystem: eLeftHanded"); break;
    }

    int i;
    FbxNode* node = scene->GetRootNode();

    if (node)
    {
        LOGD("root node");
        DisplayContent(scene, node);
    }
}

void Converter::DisplayContent(FbxScene* scene, FbxNode* node)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    /**/
    //DisplayUserProperties(node);
    //DisplayTarget(node);
    //DisplayPivotsAndLimits(node);
    DisplayGeometricTransform(node);
    //DisplayTransformPropagation(node);

    if (node->GetNodeAttribute() == NULL)
    {
        FBXSDK_printf("NULL Node Attribute\n\n");
    }
    else
    {
        FBXSDK_printf("\n FBX NODE \n");

        // Look node's attribute type
        lAttributeType = (node->GetNodeAttribute()->GetAttributeType());
        if (lAttributeType == FbxNodeAttribute::eUnknown) { LOGD("eUnknown"); }
        else if (lAttributeType == FbxNodeAttribute::eNull) { LOGD("eNull"); }
        else if (lAttributeType == FbxNodeAttribute::eMarker) { LOGD("eMarker"); }
        else if (lAttributeType == FbxNodeAttribute::eSkeleton) { LOGD("eSkeleton"); }
        else if (lAttributeType == FbxNodeAttribute::eMesh) { LOGD("eMesh"); }
        else if (lAttributeType == FbxNodeAttribute::eNurbs) { LOGD("eNurbs"); }
        else if (lAttributeType == FbxNodeAttribute::ePatch) { LOGD("ePatch"); }
        else if (lAttributeType == FbxNodeAttribute::eCamera) { LOGD("eCamera"); }
        else if (lAttributeType == FbxNodeAttribute::eCameraStereo) { LOGD("eCameraStereo"); }
        else if (lAttributeType == FbxNodeAttribute::eCameraSwitcher) { LOGD("eCameraSwitcher"); }
        else if (lAttributeType == FbxNodeAttribute::eLight) { LOGD("eLight"); }
        else if (lAttributeType == FbxNodeAttribute::eOpticalReference) { LOGD("eOpticalReference"); }
        else if (lAttributeType == FbxNodeAttribute::eOpticalMarker) { LOGD("eOpticalMarker"); }
        else if (lAttributeType == FbxNodeAttribute::eNurbsCurve) { LOGD("eNurbsCurve"); }
        else if (lAttributeType == FbxNodeAttribute::eTrimNurbsSurface) { LOGD("eTrimNurbsSurface"); }
        else if (lAttributeType == FbxNodeAttribute::eBoundary) { LOGD("eBoundary"); }
        else if (lAttributeType == FbxNodeAttribute::eNurbsSurface) { LOGD("eNurbsSurface"); }
        else if (lAttributeType == FbxNodeAttribute::eShape) { LOGD("eShape"); }
        else if (lAttributeType == FbxNodeAttribute::eLODGroup) { LOGD("eLODGroup"); }
        else if (lAttributeType == FbxNodeAttribute::eSubDiv) { LOGD("eSubDiv"); }
        else if (lAttributeType == FbxNodeAttribute::eCachedEffect) { LOGD("eCachedEffect"); }
        else if (lAttributeType == FbxNodeAttribute::eLine) { LOGD("eLine"); }

        switch (lAttributeType)
        {
        //case FbxNodeAttribute::eMarker:
        //    DisplayMarker(node);
        //    break;

        //case FbxNodeAttribute::eSkeleton:
        //    DisplaySkeleton(node);
        //    break;

        case FbxNodeAttribute::eMesh:
            Grapevine::Log::D("FbxNodeAttribute::eMesh");

            // Pivot, FBX likes 90 degree x-axis rotation, we don't :)
            FbxVector4 rotation = DisplayPivotsAndLimits(node);
            float xRotationForThisNode = -rotation[0];
            DisplayMesh(scene, node, exporter_, xRotationForThisNode);
            break;

        //case FbxNodeAttribute::eNurbs:
        //    DisplayNurb(pNode);
        //    break;

        //case FbxNodeAttribute::ePatch:
        //    DisplayPatch(pNode);
        //    break;

        //case FbxNodeAttribute::eCamera:
        //    DisplayCamera(pNode);
        //    break;

        //case FbxNodeAttribute::eLight:
        //    DisplayLight(pNode);
        //    break;

        //case FbxNodeAttribute::eLODGroup:
        //    DisplayLodGroup(pNode);
        //    break;
        }
    }

    LOGD("node has %d child(s)", node->GetChildCount());
    for (i = 0; i < node->GetChildCount(); i++)
    {
        DisplayContent(scene, node->GetChild(i));
    }
}

void Converter::DisplayTarget(FbxNode* pNode)
{
    if(pNode->GetTarget() != NULL)
    {
        DisplayString("    Target Name: ", (char *) pNode->GetTarget()->GetName());
    }
}

void Converter::DisplayTransformPropagation(FbxNode* pNode)
{
    FBXSDK_printf("    Transformation Propagation\n");

    //
    // Rotation Space
    //
    EFbxRotationOrder lRotationOrder;
    pNode->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);

    FBXSDK_printf("        Rotation Space: ");

    switch (lRotationOrder)
    {
    case eEulerXYZ:
        FBXSDK_printf("Euler XYZ\n");
        break;
    case eEulerXZY:
        FBXSDK_printf("Euler XZY\n");
        break;
    case eEulerYZX:
        FBXSDK_printf("Euler YZX\n");
        break;
    case eEulerYXZ:
        FBXSDK_printf("Euler YXZ\n");
        break;
    case eEulerZXY:
        FBXSDK_printf("Euler ZXY\n");
        break;
    case eEulerZYX:
        FBXSDK_printf("Euler ZYX\n");
        break;
    case eSphericXYZ:
        FBXSDK_printf("Spheric XYZ\n");
        break;
    }

    //
    // Use the Rotation space only for the limits
    // (keep using eEulerXYZ for the rest)
    //
    FBXSDK_printf("        Use the Rotation Space for Limit specification only: %s\n",
        pNode->GetUseRotationSpaceForLimitOnly(FbxNode::eSourcePivot) ? "Yes" : "No");


    //
    // Inherit Type
    //
    FbxTransform::EInheritType lInheritType;
    pNode->GetTransformationInheritType(lInheritType);

    FBXSDK_printf("        Transformation Inheritance: ");

    switch (lInheritType)
    {
    case FbxTransform::eInheritRrSs:
        FBXSDK_printf("RrSs\n");
        break;
    case FbxTransform::eInheritRSrs:
        FBXSDK_printf("RSrs\n");
        break;
    case FbxTransform::eInheritRrs:
        FBXSDK_printf("Rrs\n");
        break;
    }
}

void Converter::DisplayGeometricTransform(FbxNode* pNode)
{
    FbxVector4 lTmpVector;

    FBXSDK_printf("    Geometric Transformations\n");

    //
    // Translation
    //
    lTmpVector = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FBXSDK_printf("        Translation: %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

    //
    // Rotation
    //
    lTmpVector = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FBXSDK_printf("        Rotation:    %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

    //
    // Scaling
    //
    lTmpVector = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    FBXSDK_printf("        Scaling:     %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);
}


void Converter::DisplayMetaData(FbxScene* pScene)
{
    FbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
    if (sceneInfo)
    {
        FBXSDK_printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
        FBXSDK_printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
        FBXSDK_printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
        FBXSDK_printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
        FBXSDK_printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
        FBXSDK_printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
        FBXSDK_printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

        FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
        if (thumbnail)
        {
            FBXSDK_printf("    Thumbnail:\n");

            switch (thumbnail->GetDataFormat())
            {
            case FbxThumbnail::eRGB_24:
                FBXSDK_printf("        Format: RGB\n");
                break;
            case FbxThumbnail::eRGBA_32:
                FBXSDK_printf("        Format: RGBA\n");
                break;
            }

            switch (thumbnail->GetSize())
            {
            case FbxThumbnail::eNotSet:
                FBXSDK_printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e64x64:
                FBXSDK_printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e128x128:
                FBXSDK_printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
            }
        }
    }
}
