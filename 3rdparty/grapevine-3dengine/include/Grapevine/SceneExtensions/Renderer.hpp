/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_RENDERER
#define GRAPEVINE_SCENEEXTENSIONS_RENDERER

#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Scene/AbstractVisitor.hpp"
#include <Grapevine/Shader/ShaderProgram.hpp>
#include <Grapevine/Material/MaterialResource.hpp>
//#include <../src/kaski/text.hpp>

namespace Grapevine
{
    class MeshResource;

    namespace Scene
    {
        namespace Extensions
        {
            class Renderer
            {
            public:
                struct RenderQueueObject
                {
                    RenderQueueObject(Matrix4& transformation, float distanceFromCamera, ModelComponent::MeshMaterialPair* meshMaterialPair, bool hasAlpha, float opacity = 1.0f):
                        transformation(transformation),
                        distanceFromCamera(distanceFromCamera),
                        meshMaterialPair(meshMaterialPair),
#ifndef __NGOS__
                        //text(NULL),
#endif
                        hasAlpha(hasAlpha),
                        opacity(opacity)
                    {
                    };

/*
#ifndef __NGOS__

                    RenderQueueObject(Matrix4& transformation, float distanceFromCamera, kaski::Text* text, bool hasAlpha, float opacity = 1.0f):
                        transformation(transformation),
                        distanceFromCamera(distanceFromCamera),
                        meshMaterialPair(NULL),
#ifndef __NGOS__
                        text(text),
#endif
                        hasAlpha(hasAlpha),
                        opacity(opacity)
                    {
                    };

#endif
*/
                    Matrix4 transformation;
                    float distanceFromCamera;
                    ModelComponent::MeshMaterialPair* meshMaterialPair;

                    void* text; // place holder
#ifndef __NGOS__
                    //kaski::Text* text;
#endif
                    bool hasAlpha;
                    float opacity;


                };


                Renderer(Node& cameraNode);
                ~Renderer();

                void setLightDiffuseDirection(Vector3 const&);
                void setLightSpecularDirection(Vector3 const&);
                void setLightColorAmbient(Vector3 const&);
                void setLightColorDiffuse(Vector3 const&);
                void setLightColorSpecular(Vector3 const&);
                void setOmniLightWorldLocation(Vector3 const&);
                void setOmniLightColor(Vector3 const&);
                void setOmniLightStrength(float);

                void enableDepthWrites();
                void disableDepthWrites();

                void setCameraMatrices(Matrix4 const& view, Matrix4 const& projection);
                void setDebugNormals(bool value);

                // API for the RenderableComponent
                void render(Matrix4 const& model, MaterialResource* material, MeshResource* mesh, float opacity = 1.0f);

#ifndef __NGOS__
//                void render(Matrix4 const& model, kaski::Text* text, int startIndex, int count, float opacity = 1.0f);
#endif
                // API to render physics debug overlay
                void renderWireframeBox(Matrix4 const& model, Vector3 const& minBoundingVector, Vector3 const& maxBoundingVector);

                void initFrame();
                void setDeviceOrientation(Matrix4 const&);
                void testRender(Matrix4 const& model);

            protected:
                Vector3 nDiffuseLight_;
                Vector3 nSpecularLight_;
                Vector3 lightColorAmbient_;
                Vector3 lightColorDiffuse_;
                Vector3 lightColorSpecular_;
                Vector3 omniLightWorldLocation_;
                Vector3 omniLightColor_;
                float omniLightStrength_;
                Node cameraNode_;

                // Will be resetted in start of every frame
                Matrix4 viewProjection_;
                ShaderProgram* shaderProgramInUse_;

                // Engine normal shader programs:
                ShaderProgram* shaderProgramText_;
                ShaderProgram shaderProgramPhong_;
                ShaderProgram shaderProgramPhongBump_;
                ShaderProgram shaderProgramPhongCubeReflection_;
                ShaderProgram shaderProgramPhongNormalMapCubeReflection_;

                // Engine effect shader programs:
                ShaderProgram shaderProgramPhong_planarReflection_;
                ShaderProgram shaderProgramPhongCubeReflection_planarReflection_;
                ShaderProgram shaderProgramGradient_;
                ShaderProgram shaderProgramShadow_;
                ShaderProgram shaderProgramAlphaBurn_;

                // Engine debug shader programs:
                ShaderProgram shaderProgramDebugNormals_; // NOTE: could be behind DEBUG compile flag
                ShaderProgram shaderProgramDebugPhysics_; // NOTE: could be behind DEBUG compile flag

                // Default cubemap
                CubemapResource cubemap_;

                // Device orientation
                Matrix4 deviceOrientation_;

                bool debugNormals_;

                std::vector<Matrix4> matrixStack_;
            };
        }
    }
}

#endif

