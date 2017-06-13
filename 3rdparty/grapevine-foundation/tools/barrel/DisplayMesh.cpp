// This file is based on Autodesk sample which was modified for barrel project

#include "DisplayMesh.hpp"
//#include "DisplayMaterial.hpp"
#include "DisplayTexture.hpp"
#include "DisplayLink.hpp"
#include "DisplayShape.hpp"
#include "Barrel/Common.hpp"
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Resource/File.hpp>
#include <Grapevine/Core/Exception.hpp>
#include "DisplayPivotsAndLimits.hpp"
#include <fbxsdk/core/math/fbxmatrix.h>

#if defined (FBXSDK_ENV_MAC)
// disable the format not a string literal and no format arguments warning since
// the FBXSDK_printf calls made here are all valid calls and there is no secuity risk
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

#define MAT_HEADER_LENGTH 200

void DisplayControlsPoints(FbxMesh* pMesh, Barrel::Mesh* barrelMesh, Grapevine::Matrix4& transformation);
void DisplayPolygons(FbxMesh* pMesh, Barrel::Mesh* barrelMesh, Grapevine::Matrix4& rotation);
void DisplayMaterialMapping(FbxMesh* pMesh);
void DisplayTextureMapping(FbxMesh* pMesh);
std::string DisplayTextureNames(FbxProperty &pProperty);
void DisplayMaterialConnections(FbxMesh* pMesh, Exporter& exporter, Barrel::Mesh* barrelMesh);
void DisplayMaterialTextureConnections(FbxSurfaceMaterial* pMaterial, Exporter& exporter, Barrel::Mesh* barrelMesh, char * header, int pMatId, int l );

std::string textureName(FbxProperty &pProperty)
{
    std::string r("");

    int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();

    if(lLayeredTextureCount > 0)
    {
        LOGE("layered texture not supported!");
    }
    else
    {
        //no layered texture simply get on the property
        int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

        if (lNbTextures == 1)
        {
            for(int j =0; j<lNbTextures; ++j)
            {
                FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);

                //Texture tex;
                FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(lTexture);
                if (pFileTexture)
                {
                    std::string filepath =  pFileTexture->GetFileName();
                    r = Grapevine::File::filenameFromPath(filepath);
                }
            }

            LOGD("texture file: %s", r.c_str());
        }
        else
        {
            LOGE("Texture amount is not 1 (it is: %d)! Material not supported and might corrupt the resulting .barrel!", lNbTextures);
        }
    }

    return r;
}

float fbxFloat(FbxProperty &property)
{
    /*const int count = property.GetSrcObjectCount<FbxFloat>();
    if (count == 1)
    {
        FbxFloat* fbxFloat = property.Get<FbxFloat>();
        LOGW("fbxFloat fbxFloat");
    }*/
    float r = property.Get<FbxFloat>();
    LOGD("float:%f", r);
    return r;
}

void DisplayMesh(FbxScene* scene, FbxNode* node, Exporter& exporter, float xRotation)
{
    Barrel::Mesh* barrelMesh = new Barrel::Mesh();
    FbxMesh* lMesh = (FbxMesh*) node->GetNodeAttribute ();

    // Name
    Grapevine::Log::D("Mesh name: %s", (char*)node->GetName());
    barrelMesh->setName(std::string(node->GetName()));

    // Recalculate normals
//    lMesh->BuildMeshEdgeArray();
    lMesh->SplitPoints(FbxLayerElement::eUV);
//    lMesh->ComputeVertexNormals();

    // Transformations
    FbxMatrix localFbxMatrix = node->EvaluateLocalTransform(0);
    FbxVector4 geometricTranslation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 geometricRotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 geometricScaling = node->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxMatrix geometricFbxMatrix;
    geometricFbxMatrix.SetTRS(geometricTranslation, geometricRotation, geometricScaling);
    localFbxMatrix *= geometricFbxMatrix;

    // Pivot rotation
    LOGD("Pivot xAxis rotation:%f", xRotation);
    FbxVector4 sceneTranslation(0.0f, 0.0f, 0.0f);
    FbxVector4 sceneRotation(xRotation, 0.0f, 0.0f);
    FbxVector4 sceneScaling(1.0f, 1.0f, 1.0f);
    FbxMatrix sceneFbxMatrix(sceneTranslation, sceneRotation, sceneScaling);
    localFbxMatrix = sceneFbxMatrix * localFbxMatrix;

    // Rotation for normals
    FbxMatrix rotationFbxMatrix = localFbxMatrix.Inverse().Transpose();

    float tmpLocalMatrix[16];
    float tmpRotationMatrix[16];
    int i = 0;
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            tmpLocalMatrix[i] = localFbxMatrix.Get(column, row);
            tmpRotationMatrix[i] = rotationFbxMatrix.Get(column, row);
            i++;
        }
    }
    Grapevine::Matrix4 localMatrix(&tmpLocalMatrix[0]);
    Grapevine::Matrix4 rotationMatrix(&tmpRotationMatrix[0]);

//    for (i=0;i<16;i++)
//    {
//        LOGD("transform: %f", localMatrix.get(i));
//    }

//    for (i=0;i<16;i++)
//    {
//        LOGD("rotation: %f", rotationMatrix.get(i));
//    }

    // Display & export data
    DisplayMetaDataConnections(lMesh);
    DisplayControlsPoints(lMesh, barrelMesh, localMatrix);
    DisplayPolygons(lMesh, barrelMesh, rotationMatrix);
    //DisplayMaterial(lMesh, exporter);

    DisplayMaterialMapping(lMesh);
    DisplayTexture(lMesh);
    DisplayMaterialConnections(lMesh, exporter, barrelMesh);
    //DisplayLink(lMesh);
    //DisplayShape(lMesh);

    exporter.addMesh(barrelMesh);
}

void DisplayControlsPoints(FbxMesh* pMesh, Barrel::Mesh* barrelMesh, Grapevine::Matrix4& transformation)
{
    int i, lControlPointsCount = pMesh->GetControlPointsCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();

    Grapevine::Log::D("Control Points");

    for (i = 0; i < lControlPointsCount; i++)
    {
        //DisplayInt("        Control Point ", i);
        //Display3DVector("            Coordinates: ", lControlPoints[i]);

        FbxVector4 value = lControlPoints[i];
        Grapevine::Vector3 coordinate;
        coordinate.x((float)value[0]);
        coordinate.y((float)value[1]);
        coordinate.z((float)value[2]);

        //Grapevine::Matrix4 transformation2;
        coordinate = transformation.transform(coordinate);

        // NOTE: creates duplicate control points because we are not importing subtree, but only collapsed model
        Barrel::ControlPoint* point = new Barrel::ControlPoint(i, coordinate);
        barrelMesh->addPoint(point);
    }

    DisplayString("");
}

void DisplayPolygons(FbxMesh* pMesh, Barrel::Mesh* barrelMesh, Grapevine::Matrix4& rotation)
{
    int i, j, lPolygonCount = pMesh->GetPolygonCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
    char header[100];

    Grapevine::Log::D("Polygons");

    int vertexId = 0;
    for (i = 0; i < lPolygonCount; i++)
    {
        Barrel::Polygon* barrelPolygon = new Barrel::Polygon();

        DisplayInt("        Polygon ", i);
        int l;

        for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
        {
            FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
            switch (lePolgrp->GetMappingMode())
            {
            case FbxGeometryElement::eByPolygon:
                if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
                {
                    FBXSDK_sprintf(header, 100, "        Assigned to group: ");
                    int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
                    DisplayInt(header, polyGroupId);
                    break;
                }
            default:
                // any other mapping modes don't make sense
                DisplayString("        \"unsupported group assignment\"");
                break;
            }
        }

        int lPolygonSize = pMesh->GetPolygonSize(i);

        for (j = 0; j < lPolygonSize; j++)
        {
            int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

            Barrel::ControlPoint* barrelPoint = barrelMesh->point(lControlPointIndex);
            if (barrelPoint == NULL)
            {
                Grapevine::Log::E("no point found!");
                throw Grapevine::Exception("no point found, corrupted .fbx data or Bad Implementation (tm)");
            }
            barrelPolygon->addPoint(barrelPoint);
            Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

            for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
            {
                FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor( l);
                FBXSDK_sprintf(header, 100, "            Color vertex: ");

                switch (leVtxc->GetMappingMode())
                {
                case FbxGeometryElement::eByControlPoint:
                    switch (leVtxc->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
                            DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                        }
                        break;
                    default:
                        break; // other reference modes not shown here!
                    }
                    break;

                case FbxGeometryElement::eByPolygonVertex:
                    {
                        switch (leVtxc->GetReferenceMode())
                        {
                        case FbxGeometryElement::eDirect:
                            DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
                            break;
                        case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = leVtxc->GetIndexArray().GetAt(vertexId);
                                DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                            }
                            break;
                        default:
                            break; // other reference modes not shown here!
                        }
                    }
                    break;

                case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
                case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
                case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
                    break;
                }
            }
            for (l = 0; l < pMesh->GetElementUVCount(); ++l)
            {
                FbxGeometryElementUV* leUV = pMesh->GetElementUV( l);
                FBXSDK_sprintf(header, 100, "                UV: ");

                switch (leUV->GetMappingMode())
                {
                case FbxGeometryElement::eByControlPoint:
                    switch (leUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                    {
                        float tmpU = (float)leUV->GetDirectArray().GetAt(lControlPointIndex)[0];
                        float tmpV = (float)leUV->GetDirectArray().GetAt(lControlPointIndex)[1];
                        Grapevine::Vector2 tmp(tmpU, tmpV);
//                        Grapevine::Log::D("UVCoordinate creation! 1");
                        barrelPoint->addUVCoordinate(new Barrel::UVCoordinate(tmp), l);

                        Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
                        break;
                    }
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leUV->GetIndexArray().GetAt(lControlPointIndex);

                        float tmpU = (float)leUV->GetDirectArray().GetAt(id)[0];
                        float tmpV = (float)leUV->GetDirectArray().GetAt(id)[1];
                        Grapevine::Vector2 tmp(tmpU, tmpV);
//                        Grapevine::Log::D("UVCoordinate creation! 2");
                        barrelPoint->addUVCoordinate(new Barrel::UVCoordinate(tmp), l);

                        Display2DVector(header, leUV->GetDirectArray().GetAt(id));
                        break;
                    }
                    default:
                        LOGE("FAIL: UVCoordinate creation!");
                        break; // other reference modes not shown here!
                    }
                    break;

                case FbxGeometryElement::eByPolygonVertex:
                    {
                        int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
                        switch (leUV->GetReferenceMode())
                        {
                        case FbxGeometryElement::eDirect:
                        case FbxGeometryElement::eIndexToDirect:
                            {

                                float tmpU = (float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[0];
                                float tmpV = (float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[1];
                                Grapevine::Vector2 tmp(tmpU, tmpV);
                                //Grapevine::Log::D("UVCoordinate creation! 3");
                                barrelPoint->addUVCoordinate(new Barrel::UVCoordinate(tmp), l);

                                Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
                            }
                            break;
                        default:
                            break; // other reference modes not shown here!
                        }
                    }
                    break;

                case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
                case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
                case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
                    break;
                }
            }

            // Get normals
            FbxVector4 value;
            pMesh->GetPolygonVertexNormal(i, j, value);
            Display3DVector("            Normal: ", value);
            Grapevine::Vector3 tmp;
            tmp.x((float)value[0]);
            tmp.y((float)value[1]);
            tmp.z((float)value[2]);

            tmp = rotation.transform(tmp);
            tmp.normalize();

            Barrel::Normal* barrelNormal = new Barrel::Normal(tmp);
            barrelPoint->addNormal(barrelNormal, 0);

//            for( l = 0; l < pMesh->GetElementNormalCount(); ++l)
//            {
//                FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l);
//                FBXSDK_sprintf(header, 100, "            Normal: ");

//                if(leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
//                {
//                    switch (leNormal->GetReferenceMode())
//                    {
//                    case FbxGeometryElement::eDirect:
//                        {
//                            FbxVector4 value = leNormal->GetDirectArray().GetAt(vertexId);
//                            Grapevine::Vector3 tmp;
//                            tmp.x((float)value[0]);
//                            tmp.y((float)value[1]);
//                            tmp.z((float)value[2]);

//                            tmp = rotation.transform(tmp);

//                            Barrel::Normal* barrelNormal = new Barrel::Normal(tmp);
//                            barrelPoint->addNormal(barrelNormal, l);

//                            Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
//                        }
//                        break;
//                    case FbxGeometryElement::eIndexToDirect:
//                        {
//                            int id = leNormal->GetIndexArray().GetAt(vertexId);

//                            FbxVector4 value = leNormal->GetDirectArray().GetAt(id);
//                            Grapevine::Vector3 tmp;
//                            tmp.x((float)value[0]);
//                            tmp.y((float)value[1]);
//                            tmp.z((float)value[2]);

//                            tmp = rotation.transform(tmp);

//                            Barrel::Normal* barrelNormal = new Barrel::Normal(tmp);
//                            barrelPoint->addNormal(barrelNormal, l);

//                            Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
//                        }
//                        break;
//                    default:
//                        break; // other reference modes not shown here!
//                    }
//                }
//            }
            for( l = 0; l < pMesh->GetElementTangentCount(); ++l)
            {
                FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
                FBXSDK_sprintf(header, 100, "            Tangent: ");

                if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    switch (leTangent->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Grapevine::Log::D("Tangent creation! 1");

                        Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            Grapevine::Log::D("Tangent creation! 2");

                            int id = leTangent->GetIndexArray().GetAt(vertexId);
                            Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
                        }
                        break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }

            }
            for( l = 0; l < pMesh->GetElementBinormalCount(); ++l)
            {

                FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);

                FBXSDK_sprintf(header, 100, "            Binormal: ");
                if(leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    switch (leBinormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Grapevine::Log::D("Binormal creation! 1");

                        Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            Grapevine::Log::D("Binormal creation! 2");

                            int id = leBinormal->GetIndexArray().GetAt(vertexId);
                            Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
                        }
                        break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }
            }
            vertexId++;
        } // for polygonSize

        barrelMesh->addPolygon(barrelPolygon);

    } // for polygonCount


    //check visibility for the edges of the mesh
    for(int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
    {
        FbxGeometryElementVisibility* leVisibility=pMesh->GetElementVisibility(l);
        FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
        DisplayString(header);
        switch(leVisibility->GetMappingMode())
        {
            //should be eByEdge
        case FbxGeometryElement::eByEdge:
            //should be eDirect
            for(int j=0; j!=pMesh->GetMeshEdgeCount();++j)
            {
                DisplayInt("        Edge ", j);
                DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
            }

            break;
        }
    }
    DisplayString("");
}

std::string DisplayTextureNames(FbxProperty &pProperty)
{
    std::string r;

    int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();

    if(lLayeredTextureCount > 0)
    {
        for(int j=0; j<lLayeredTextureCount; ++j)
        {
            std::string tmp("DisplayTextureNames:layered:");

            FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
            int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();

            for(int k =0; k<lNbTextures; ++k)
            {
                //lConnectionString += k;
                tmp += "\"";
                tmp += (char*)lLayeredTexture->GetName();
                tmp += "\"";
                tmp += " ";
            }
            tmp += "of ";
            tmp += pProperty.GetName();
            tmp += " on layer ";
            tmp += j;
            LOGD("%s", tmp.c_str());
        }
    }
    else
    {
        //no layered texture simply get on the property
        int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

        if(lNbTextures > 0)
        {
            std::string tmp("DisplayTextureNames:property:");

            for(int j =0; j<lNbTextures; ++j)
            {
                FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
                if(lTexture)
                {
                    tmp += "\"";
                    tmp += (char*)lTexture->GetName();
                    tmp += "\"";
                    tmp += " ";
                }

                //Texture tex;
                FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(lTexture);
                if (pFileTexture)
                {
                    std::string filepath =  pFileTexture->GetFileName();
                    LOGD("    filepath: %s", filepath.c_str());
                    r = Grapevine::File::filenameFromPath(filepath);

                    /*if( !FileExists(filepath.c_str() ) )
                    {
                        std::string relativePath = StripFilename(params.fbxFilename) + "\\" + filepath;
                        if( FileExists(relativePath.c_str() ) )
                        {
                            filepath = relativePath;
                        }
                        else
                        {
                            FxOgreFBXLog("Warning! Could not locate texture %s for material %s.\n.", filepath.c_str(), m_name.c_str());
                        }
                    }


                    for( size_t i = 0; i < filepath.length(); ++i )
                    {

                        if( filepath.at(i) == '/' )
                        {
                            filepath[i] = toascii('\\');

                        }
                        else
                        {
                            filepath[i] = toascii(filepath[i]);
                        }
                    }

                    tex.absFilename = filepath;
                    tex.filename = FxOgreFBX::StripToTopParent(filepath);
                    tex.bCreateTextureUnit = true;*/
                }
            }
            tmp += "of ";
            tmp += pProperty.GetName();
            LOGD("%s", tmp.c_str());
        }
    }

    return r;
}

void DisplayMaterialTextureConnections(FbxSurfaceMaterial* pMaterial, Exporter& exporter, Barrel::Mesh* barrelMesh, char * header, int pMatId, int l )
{
    Grapevine::Log::D("DisplayMaterialTextureConnections");

    FbxProperty lProperty;
    Barrel::Material* material = new Barrel::Material();

    // Material name
    barrelMesh->setMaterialName(pMaterial->GetName());
    material->setMaterialName(pMaterial->GetName());

    // Colormap
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    material->setColorMapFile(textureName(lProperty));

    // Normalmap
    LOG
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    material->setNormalMapFile(textureName(lProperty));
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    DisplayTextureNames(lProperty);
    LOG

    // Specular factor
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
    material->setSpecularFactor(fbxFloat(lProperty));

    // Glossiness factor
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    material->setGlossinessFactor(fbxFloat(lProperty));

    // ReflectionFactor factor
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
    std::string reflectionTextureName = textureName(lProperty);
    if (reflectionTextureName.size() == 0)
    {
        LOGI("No reflectionTextureName set, using reflectionFactor 0.0f");
        material->setReflectionFactor(0.0f);
    }
    else
    {
        LOG
        lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
        material->setReflectionFactor(fbxFloat(lProperty));
    }
    exporter.addMaterial(material);

    /*
    if(!pMaterial)
        return;

    FbxString lConnectionString = "            Material %d -- ";
    //Show all the textures

    FbxProperty lProperty;
    //Diffuse Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    std::string tmp = DisplayTextureNames(lProperty);
    LOGD("DisplayMaterialTextureConnections: tmp, sDiffuse:%s", tmp.c_str());

    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    std::string diffureTextureFile = DisplayTextureNames(lProperty);
    LOGD("DisplayMaterialTextureConnections: color map:%s", diffureTextureFile.c_str());

    //DiffuseFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
    DisplayTextureNames(lProperty);

    //Emissive Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
    DisplayTextureNames(lProperty);

    //EmissiveFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
    DisplayTextureNames(lProperty);

    //Ambient Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
    DisplayTextureNames(lProperty);

    //AmbientFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
    DisplayTextureNames(lProperty);

    //Specular Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
    DisplayTextureNames(lProperty);

    //SpecularFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
    DisplayTextureNames(lProperty);

    //Shininess Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    DisplayTextureNames(lProperty);

    //Bump Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);
    DisplayTextureNames(lProperty);

    //Normal Map Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    DisplayTextureNames(lProperty);

    //Transparent Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
    DisplayTextureNames(lProperty);

    //TransparencyFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
    DisplayTextureNames(lProperty);

    //Reflection Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
    DisplayTextureNames(lProperty);

    //ReflectionFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
    DisplayTextureNames(lProperty);*/
}

void DisplayMaterialConnections(FbxMesh* pMesh, Exporter& exporter, Barrel::Mesh* barrelMesh)
{
    Grapevine::Log::D("DisplayMaterialConnections");

    int i, l, lPolygonCount = pMesh->GetPolygonCount();

    char header[MAT_HEADER_LENGTH];

    Grapevine::Log::D("    Polygons Material Connections");

    //check whether the material maps with only one mesh
    bool lIsAllSame = true;
    for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
    {

        FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
        if( lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
        {
            lIsAllSame = false;
            break;
        }
    }

    //For eAllSame mapping type, just out the material and texture mapping info once
    if(lIsAllSame)
    {
        for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
        {

            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( l);
            if( lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
            {
                FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));
                int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
                if(lMatId >= 0)
                {
                    Grapevine::Log::D("        All polygons share the same material \'%s\' in mesh %d", lMaterial->GetName(), l);
                    //DisplayInt("        All polygons share the same material in mesh ", l);
                    DisplayMaterialTextureConnections(lMaterial, exporter, barrelMesh, header, lMatId, l);
                }
            }
        }

        //no material
        if (l == 0)
        {
            Grapevine::Log::D("        no material applied");
        }
    }

    //For eByPolygon mapping type, just out the material and texture mapping info once
    else
    {
        for (i = 0; i < lPolygonCount; i++)
        {
            DisplayInt("        Polygon ", i);

            for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
            {

                FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial( l);
                FbxSurfaceMaterial* lMaterial = NULL;
                int lMatId = -1;
                lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(i));
                lMatId = lMaterialElement->GetIndexArray().GetAt(i);

                if(lMatId >= 0)
                {
                    DisplayMaterialTextureConnections(lMaterial, exporter, barrelMesh, header, lMatId, l);
                }
            }
        }
    }
}


void DisplayMaterialMapping(FbxMesh* pMesh)
{
    Grapevine::Log::D("DisplayMaterialMapping");
    const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
    const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct"};

    int lMtrlCount = 0;
    FbxNode* lNode = NULL;
    if(pMesh){
        lNode = pMesh->GetNode();
        if(lNode)
            lMtrlCount = lNode->GetMaterialCount();
    }

    for (int l = 0; l < pMesh->GetElementMaterialCount(); l++)
    {
        FbxGeometryElementMaterial* leMat = pMesh->GetElementMaterial( l);
        if (leMat)
        {
            char header[100];
            FBXSDK_sprintf(header, 100, "    Material Element %d: ", l);
            DisplayString(header);

            LOGD(" * header: %s", header);
            LOGD("           Mapping: %s", lMappingTypes[leMat->GetMappingMode()]);
            LOGD("           ReferenceMode: %s", lReferenceMode[leMat->GetReferenceMode()]);

            int lMaterialCount = 0;
            FbxString lString;

            if (leMat->GetReferenceMode() == FbxGeometryElement::eDirect ||
                leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
            {
                lMaterialCount = lMtrlCount;
            }

            if (leMat->GetReferenceMode() == FbxGeometryElement::eIndex ||
                leMat->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
            {
                int i;

                //lString = "           Indices: ";

                int lIndexArrayCount = leMat->GetIndexArray().GetCount();
                for (i = 0; i < lIndexArrayCount; i++)
                {
                    lString += leMat->GetIndexArray().GetAt(i);

                    if (i < lIndexArrayCount - 1)
                    {
                        lString += ", ";
                    }
                }

                lString += "\n";

                //FBXSDK_printf(lString);
            }
        }
    }

    DisplayString("");
}
