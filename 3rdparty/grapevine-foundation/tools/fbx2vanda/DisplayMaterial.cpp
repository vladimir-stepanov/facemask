#include <fbxsdk.h>
#include "DisplayMaterial.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Resource/File.hpp>

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

void DisplayMaterial(FbxGeometry* pGeometry, Exporter& exporter)
{
    LOGD("\n* material");

    int materialCount = 0;
    FbxNode* node = NULL;
    if(pGeometry)
    {
        node = pGeometry->GetNode();
        if(node)
        {
            materialCount = node->GetMaterialCount();
        }
    }

    if (materialCount > 0)
    {
        LOGD(" * count: %d", materialCount);

        FbxPropertyT<FbxDouble3> lKFbxDouble3;
        FbxPropertyT<FbxDouble> lKFbxDouble1;
        FbxColor theColor;

        for (int lCount = 0; lCount < materialCount; lCount ++)
        {
            Barrel::Material* material = new Barrel::Material();
            exporter.addMaterial(material);
            LOGD("  * index: %d ", lCount);

            FbxSurfaceMaterial *lMaterial = node->GetMaterial(lCount);

            FbxPropertyT<FbxString> lString;
            lString = lMaterial->ShadingModel;
            LOGD("   * shading model: %s", lString.Get().Buffer());

            // DisplayString("            Name: \"", (char *) lMaterial->GetName(), "\"");
            LOGD("   * name: \"%s\"",  (char*)lMaterial->GetName());
            material->setMaterialName(lMaterial->GetName());

            // Get the implementation to see if it's a hardware shader.
            const FbxImplementation* lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_HLSL);
            FbxString lImplemenationType = "HLSL";
            if (!lImplementation)
            {
                lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_CGFX);
                lImplemenationType = "CGFX";
            }
            if (lImplementation)
            {
                //Now we have a hardware shader, let's read it
                //FBXSDK_printf("            Hardware Shader Type: %s\n", lImplemenationType.Buffer());
                LOGD("   * HW shader type: \"%s\"", (char*)lImplemenationType.Buffer());
                FbxBindingTable const* lRootTable = lImplementation->GetRootTable();
                FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
                FbxString lTechniqueName = lRootTable->DescTAG.Get();
                FbxBindingTable const* lTable = lImplementation->GetRootTable();
                size_t lEntryNum = lTable->GetEntryCount();

                for (int i=0; i<(int)lEntryNum; ++i)
                {
                    const FbxBindingTableEntry& lEntry = lTable->GetEntry(i);
                    const char* lEntrySrcType = lEntry.GetEntryType(true);
                    FbxProperty lFbxProp;
                    FbxString lTest = lEntry.GetSource();
                    //FBXSDK_printf("            Entry: %s\n", lTest.Buffer());
                    LOGD("   * entry: \"%s\"", (char*)lTest.Buffer());

                    if (strcmp(FbxPropertyEntryView::sEntryType, lEntrySrcType ) == 0 )
                    {
                        lFbxProp = lMaterial->FindPropertyHierarchical(lEntry.GetSource());
                        if(!lFbxProp.IsValid())
                        {
                            lFbxProp = lMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
                        }
                    }
                    else if (strcmp(FbxConstantEntryView::sEntryType, lEntrySrcType ) == 0 )
                    {
                        lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
                    }
                    if (lFbxProp.IsValid())
                    {
                        if( lFbxProp.GetSrcObjectCount<FbxTexture>() > 0 )
                        {
                            //do what you want with the textures
                            for(int j=0; j<lFbxProp.GetSrcObjectCount<FbxFileTexture>(); ++j)
                            {
                                FbxFileTexture *lTex = lFbxProp.GetSrcObject<FbxFileTexture>(j);
                                FBXSDK_printf("           File Texture: %s\n", lTex->GetFileName());
                            }
                            for(int j=0; j<lFbxProp.GetSrcObjectCount<FbxLayeredTexture>(); ++j)
                            {
                                FbxLayeredTexture *lTex = lFbxProp.GetSrcObject<FbxLayeredTexture>(j);
                                FBXSDK_printf("        Layered Texture: %s\n", lTex->GetName());
                            }
                            for(int j=0; j<lFbxProp.GetSrcObjectCount<FbxProceduralTexture>(); ++j)
                            {
                                FbxProceduralTexture *lTex = lFbxProp.GetSrcObject<FbxProceduralTexture>(j);
                                FBXSDK_printf("     Procedural Texture: %s\n", lTex->GetName());
                            }
                        }
                        else
                        {
                            FbxDataType lFbxType = lFbxProp.GetPropertyDataType();
                            FbxString blah = lFbxType.GetName();
                            if(FbxBoolDT == lFbxType)
                            {
                                DisplayBool("                Bool: ", lFbxProp.Get<FbxBool>() );
                            }
                            else if ( FbxIntDT == lFbxType ||  FbxEnumDT  == lFbxType )
                            {
                                DisplayInt("                Int: ", lFbxProp.Get<FbxInt>());
                            }
                            else if ( FbxFloatDT == lFbxType)
                            {
                                DisplayDouble("                Float: ", lFbxProp.Get<FbxFloat>());

                            }
                            else if ( FbxDoubleDT == lFbxType)
                            {
                                DisplayDouble("                Double: ", lFbxProp.Get<FbxDouble>());
                            }
                            else if ( FbxStringDT == lFbxType
                                ||  FbxUrlDT  == lFbxType
                                ||  FbxXRefUrlDT  == lFbxType )
                            {
                                DisplayString("                String: ", lFbxProp.Get<FbxString>().Buffer());
                            }
                            else if ( FbxDouble2DT == lFbxType)
                            {
                                FbxDouble2 lDouble2 = lFbxProp.Get<FbxDouble2>();
                                FbxVector2 lVect;
                                lVect[0] = lDouble2[0];
                                lVect[1] = lDouble2[1];

                                Display2DVector("                2D vector: ", lVect);
                            }
                            else if ( FbxDouble3DT == lFbxType || FbxColor3DT == lFbxType)
                            {
                                FbxDouble3 lDouble3 = lFbxProp.Get<FbxDouble3>();


                                FbxVector4 lVect;
                                lVect[0] = lDouble3[0];
                                lVect[1] = lDouble3[1];
                                lVect[2] = lDouble3[2];
                                Display3DVector("                3D vector: ", lVect);
                            }

                            else if ( FbxDouble4DT == lFbxType || FbxColor4DT == lFbxType)
                            {
                                FbxDouble4 lDouble4 = lFbxProp.Get<FbxDouble4>();
                                FbxVector4 lVect;
                                lVect[0] = lDouble4[0];
                                lVect[1] = lDouble4[1];
                                lVect[2] = lDouble4[2];
                                lVect[3] = lDouble4[3];
                                Display4DVector("                4D vector: ", lVect);
                            }
                            else if ( FbxDouble4x4DT == lFbxType)
                            {
                                FbxDouble4x4 lDouble44 = lFbxProp.Get<FbxDouble4x4>();
                                for(int j=0; j<4; ++j)
                                {

                                    FbxVector4 lVect;
                                    lVect[0] = lDouble44[j][0];
                                    lVect[1] = lDouble44[j][1];
                                    lVect[2] = lDouble44[j][2];
                                    lVect[3] = lDouble44[j][3];
                                    Display4DVector("                4x4D vector: ", lVect);
                                }

                            }
                        }

                    }
                }
            }
            else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                // We found a Phong material.  Display its properties.

                // Display the Ambient Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Ambient;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Ambient:  ", theColor);

                // Display the Diffuse Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Diffuse;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Diffuse:  ", theColor);

                // Display the SpecularFactor
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->SpecularFactor;
                DisplayDouble("            SpecularFactor: ", lKFbxDouble1.Get());
                material->setSpecularFactor(lKFbxDouble1.Get());

                // Display the Specular Color (unique to Phong materials)
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Specular;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Specular: ", theColor);

                // Display the Emissive Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Emissive;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Emissive: ", theColor);

                //Opacity is Transparency factor now
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->TransparencyFactor;
                DisplayDouble("            TransparencyFactor: ", 1.0-lKFbxDouble1.Get());

                // Display the Shininess
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->Shininess;
                DisplayDouble("            Shininess: ", lKFbxDouble1.Get());

                // Display the Reflectivity
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->ReflectionFactor;
                DisplayDouble("            ReflectionFactor: ", lKFbxDouble1.Get());
            }
            else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                LOGE("blinn!");
            }
            else if(lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) )
            {
                // We found a Lambert material. Display its properties.
                // Display the Ambient Color
                lKFbxDouble3=((FbxSurfaceLambert *)lMaterial)->Ambient;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Ambient: ", theColor);

                // Display the Diffuse Color
                lKFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Diffuse;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Diffuse: ", theColor);

                // Display the Emissive
                lKFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Emissive;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                DisplayColor("            Emissive: ", theColor);

                // Display the Opacity
                lKFbxDouble1 =((FbxSurfaceLambert *)lMaterial)->TransparencyFactor;
                DisplayDouble("            Opacity: ", 1.0-lKFbxDouble1.Get());
            }
            else
                DisplayString("Unknown type of Material");


            FbxProperty lProperty;

            // Colormap
            lProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            material->setColorMapFile(textureName(lProperty));

            // Normalmap
            lProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sBumpMap);
            material->setNormalMapFile(textureName(lProperty));

            // Specular factor
            lProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
            material->setSpecularFactor(fbxFloat(lProperty));

            // ReflectionFactor factor
            lProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
            material->setReflectionFactor(fbxFloat(lProperty));

/*
            DisplayTextureNames(lMaterial->FindProperty(FbxSurfaceMaterial::sAmbient));

            //Normal Map Textures
            DisplayTextureNames(lMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap));
*/
            //DisplayString("            Shading Model: ", lString.Get().Buffer());
            //DisplayString("");
        }
    }
}

