/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/TransformationComponent.hpp"
#include "Grapevine/SceneExtensions/ModelComponent.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/CameraComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/Renderer.hpp"
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/GPU/GL.hpp>
#include <Grapevine/Model/MeshResource.hpp>
#include <Grapevine/Material/TextureResource.hpp>
#include <Grapevine/Material/CubemapAsset.hpp>
#include <Grapevine/Shader/VertexSource.hpp>
#include <Grapevine/Shader/FragmentSource.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            Renderer::Renderer(Node& cameraNode):
                nDiffuseLight_(0.23570f, 0.23570f, 0.94281f),
                nSpecularLight_(0.23570f, 0.23570f, 0.94281f),
                lightColorAmbient_(1.0f, 1.0f, 1.0f),
                lightColorDiffuse_(1.0f, 1.0f, 1.0f),
                lightColorSpecular_(1.0f, 1.0f, 1.0f),
                omniLightWorldLocation_(0.0f, 0.0f, 0.0f),
                omniLightColor_(0.0f, 0.0f, 0.0f),
                omniLightStrength_(0.0f),
                cameraNode_(cameraNode),
                shaderProgramInUse_(NULL),
                shaderProgramText_(NULL),
                shaderProgramPhong_("shaders/n0_r0.vertex", "shaders/n0_r0.fragment"),
                shaderProgramPhongBump_("shaders/n1_r0.vertex", "shaders/n1_r0.fragment"),
                shaderProgramPhongCubeReflection_("shaders/n0_r1.vertex", "shaders/n0_r1.fragment"),
                shaderProgramPhongNormalMapCubeReflection_("shaders/n1_r1.vertex", "shaders/n1_r1.fragment"),
                shaderProgramPhong_planarReflection_("shaders/n0_r0_planarreflection.vertex", "shaders/n0_r0_planarreflection.fragment"),
                shaderProgramPhongCubeReflection_planarReflection_("shaders/n0_r1_planarreflection.vertex", "shaders/n0_r1_planarreflection.fragment"),
                shaderProgramGradient_("shaders/gradient.vertex", "shaders/gradient.fragment"),
                shaderProgramShadow_("shaders/shadow.vertex", "shaders/shadow.fragment"),
                shaderProgramAlphaBurn_("shaders/fx_alphaburn.vertex", "shaders/fx_alphaburn.fragment"),
                shaderProgramDebugNormals_("shaders/debug_normals.vertex", "shaders/debug_normals.fragment"),
                shaderProgramDebugPhysics_("shaders/debug_linedraw.vertex", "shaders/debug_linedraw.fragment"),
                cubemap_("default_cubemap.png", CubemapAsset::Type::Strip),
                debugNormals_(false)
            {
                // Load shader files
                VertexSource vertexSource("shaders/fontshader.vertex");
                FragmentSource fragmentSource("shaders/fontshader.fragment");

                // patch vertex shader
                std::stringstream header;
                header << "#define MAX_MATRICES 8" << std::endl;
                // HACK header << "#define MAX_MATRICES " << getMaxVertexUniformMatrices() << std::endl;

                vertexSource.add(header.str(), "");

                // Load shaders to program
                shaderProgramText_ = new ShaderProgram();
                shaderProgramText_->addShader(vertexSource);
                shaderProgramText_->addShader(fragmentSource);
                shaderProgramText_->build();
            }

            Renderer::~Renderer()
            {
            }

            void Renderer::setLightDiffuseDirection(Vector3 const& direction)
            {
                nDiffuseLight_ = direction;
            }

            void Renderer::setLightSpecularDirection(Vector3 const& direction)
            {
                nSpecularLight_ = direction;
            }

            void Renderer::setLightColorAmbient(Vector3 const& color)
            {
                lightColorAmbient_ = color;
            }

            void Renderer::setLightColorDiffuse(Vector3 const& color)
            {
                lightColorDiffuse_ = color;
            }

            void Renderer::setLightColorSpecular(Vector3 const& color)
            {
                lightColorSpecular_ = color;
            }

            void Renderer::setCameraMatrices(Matrix4 const& view, Matrix4 const& projection)
            {
                viewProjection_ = projection * view;
            }

            void Renderer::setOmniLightWorldLocation(Vector3 const& worldLocation)
            {
                omniLightWorldLocation_ = worldLocation;
            }

            void Renderer::setOmniLightColor(Vector3 const& color)
            {
                omniLightColor_ = color;
            }

            void Renderer::setOmniLightStrength(float strength)
            {
                omniLightStrength_ = strength;
            }

            void Renderer::enableDepthWrites()
            {
                glDepthMask(GL_TRUE);
            }

            void Renderer::disableDepthWrites()
            {
                glDepthMask(GL_FALSE);
            }

            // TODO: polish, additional functions needed to Renderer API
            void Renderer::render(Matrix4 const& model, MaterialResource* material, MeshResource* mesh, float opacity)
            {
                ShaderProgram* oldShaderProgram = shaderProgramInUse_;

                if (!debugNormals_)
                {
                    if (material == NULL)
                    {
                        LOGE("Renderer::render(), material not provided, cannot draw the given mesh!");
                        return;
                    }

                    ReferencedPointer<TextureResource> colorMap = material->colorMap();
                    ReferencedPointer<TextureResource> normalMap = material->normalMap();

                    // Select shader
                    bool hasColorMap = colorMap.exists();
                    bool hasNormalMap = normalMap.exists();
                    bool hasCubeMap = material->reflectionFactor() > 0.01f;
                    if (material->shaderProgram())
                    {
                        shaderProgramInUse_ = material->shaderProgram();
                    }
                    else if (material->effect() == MaterialResource::Effect::NoEffect)
                    {
                        if (!hasNormalMap && !hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhong_;
                        }
                        else if (!hasNormalMap && hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhongCubeReflection_;
                        }
                        else if (hasNormalMap && !hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhongBump_;
                        }
                        else if (hasNormalMap && hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhongNormalMapCubeReflection_;
                        }
                    }
                    else if (material->effect() == MaterialResource::Effect::PlanarReflection)
                    {
                        if (!hasNormalMap && !hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhong_;
                        }
                        else if (!hasNormalMap && hasCubeMap)
                        {
                            shaderProgramInUse_ = &shaderProgramPhongCubeReflection_planarReflection_;
                        }
                        else if (hasNormalMap && !hasCubeMap)
                        {
                            throw Exception("Only (!hasNormalMap && hasCubeMap) combination currenly supported in planar reflections!");
                            shaderProgramInUse_ = &shaderProgramPhongBump_;
                        }
                        else if (hasNormalMap && hasCubeMap)
                        {
                            throw Exception("Only (!hasNormalMap && hasCubeMap) combination currenly supported in planar reflections!");
                            shaderProgramInUse_ = &shaderProgramPhongNormalMapCubeReflection_;
                        }
                    }
                    else if (material->effect() == MaterialResource::Effect::Gradient)
                    {
                        shaderProgramInUse_ = &shaderProgramGradient_;
                    }
                    else if (material->effect() == MaterialResource::Effect::Shadow)
                    {
                        shaderProgramInUse_ = &shaderProgramShadow_;
                    }
                    else if (material->effect() == MaterialResource::Effect::AlphaBurn)
                    {
                        shaderProgramInUse_ = &shaderProgramAlphaBurn_;
                    }

                    // Change shader
                    if(oldShaderProgram != NULL)
                    {
                        if(oldShaderProgram->id() != shaderProgramInUse_->id())
                        {
                            oldShaderProgram->stop();
                            shaderProgramInUse_->start();
                        }
                        else
                        {
                            // same shader as the old one-> do nothing
                        }
                    }
                    else
                    {
                        shaderProgramInUse_->start();
                    }

                    // TODO: omni support to other than n0_r0 shader
                    if ((material->effect() == MaterialResource::Effect::NoEffect))
                    {
                        (*shaderProgramInUse_)["ModelMatrix"] = model;
                        (*shaderProgramInUse_)["OmniLightLocation"] = omniLightWorldLocation_;
                        (*shaderProgramInUse_)["OmniLightStrength"] = omniLightStrength_;
                        (*shaderProgramInUse_)["OmniLightColor"] = omniLightColor_;
                    }

                    // Set textures
                    if (hasColorMap)
                    {
                        unsigned int baseNumber = 0;

                        GLint location = (*shaderProgramInUse_)["ColorMap"].location();
                        glUniform1i(location, baseNumber);
                        glActiveTexture(GL_TEXTURE0 + baseNumber);
                        glBindTexture(GL_TEXTURE_2D, colorMap.pointer()->id());
                    }
                    if (hasNormalMap)
                    {
                        unsigned int baseNumber = 2;

                        GLint location = (*shaderProgramInUse_)["NormalMap"].location();
                        glUniform1i(location, baseNumber);
                        glActiveTexture(GL_TEXTURE0 + baseNumber);
                        glBindTexture(GL_TEXTURE_2D, normalMap.pointer()->id());
                    }
                    if (hasCubeMap)
                    {
                        unsigned int baseNumber = 4;

                        GLint location = (*shaderProgramInUse_)["ReflectionMap"].location();
                        glUniform1i(location, baseNumber);
                        glActiveTexture(GL_TEXTURE0 + baseNumber);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_.textureId());
                        (*shaderProgramInUse_)["ReflectionFactor"] = material->reflectionFactor();
                        (*shaderProgramInUse_)["DeviceOrientation"] = deviceOrientation_;
                    }

                    // Set uniforms and GL flags
                    {
                        if (material->effect() == MaterialResource::Effect::PlanarReflection)
                        {
                            (*shaderProgramInUse_)[ShaderProgram::Opacity] = opacity;
                            (*shaderProgramInUse_)[ShaderProgram::Highlight] = 0.0f;
                            (*shaderProgramInUse_)[ShaderProgram::SpecularFactor] = material->specularFactor();
                            (*shaderProgramInUse_)[ShaderProgram::SpecularGlossiness] = material->specularGlossiness();
                            (*shaderProgramInUse_)["StartOpacity"] = material->startOpacity();
                            (*shaderProgramInUse_)["YUsage"] = material->yUsage();
                        }
                        else if (material->effect() == MaterialResource::Effect::Gradient)
                        {
                            (*shaderProgramInUse_)["GradientStart"] = material->gradientStart();
                            (*shaderProgramInUse_)["GradientEnd"] = material->gradientEnd();
                        }
                        else if (material->effect() == MaterialResource::Effect::Shadow)
                        {
                            (*shaderProgramInUse_)[ShaderProgram::Opacity] = opacity;
                            (*shaderProgramInUse_)["ShadowColor"] = material->shadowColor();
                            (*shaderProgramInUse_)["ShadowGradientUsage"] = material->shadowGradientUsage();
                        }
                        else if (material->effect() == MaterialResource::Effect::AlphaBurn)
                        {
                            (*shaderProgramInUse_)["AlphaBurnCenter"] = material->alphaBurnCenter();
                            (*shaderProgramInUse_)["AlphaBurnRadius"] = material->alphaBurnRadius();
                            (*shaderProgramInUse_)["AlphaBurnGradientWidth"] = material->alphaBurnGradientWidth();
                        }
                        else if (material->effect() == MaterialResource::Effect::NoEffect)
                        {
                            (*shaderProgramInUse_)[ShaderProgram::Opacity] = opacity;
                            (*shaderProgramInUse_)[ShaderProgram::Highlight] = 0.0f;
                            (*shaderProgramInUse_)[ShaderProgram::SpecularFactor] = material->specularFactor();
                            (*shaderProgramInUse_)[ShaderProgram::SpecularGlossiness] = material->specularGlossiness();
                            (*shaderProgramInUse_)["nDiffuseLight"] = nDiffuseLight_;
                            (*shaderProgramInUse_)["nSpecularLight"] = nSpecularLight_;
                            (*shaderProgramInUse_)["LightColorAmbient"] = lightColorAmbient_;
                            (*shaderProgramInUse_)["LightColorDiffuse"] = lightColorDiffuse_;
                            (*shaderProgramInUse_)["LightColorSpecular"] = lightColorSpecular_;
                        }

                        glEnable(GL_BLEND);
                        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable(GL_DEPTH_TEST);
                        glDisable(GL_CULL_FACE);
                    }
                }
                else
                {
                    // Select shader
                    shaderProgramInUse_ = &shaderProgramDebugNormals_;
                    shaderProgramInUse_->start();
                    glEnable(GL_DEPTH_TEST);
                    glDisable(GL_CULL_FACE);
                }

                // Shader matrices
                Matrix4 mvp = viewProjection_;
                mvp *= model;
                mvp *= mesh->localMatrix();
                (*shaderProgramInUse_)[ShaderProgram::ModelViewProjectionMatrix] = mvp;

                Matrix4 normalMatrix(model);
                normalMatrix.inverse().transpose();
                (*shaderProgramInUse_)[ShaderProgram::NormalMatrix] = normalMatrix;

                // Set buffer data format
                unsigned int position = (*shaderProgramInUse_)[ShaderProgram::Position].location();
                unsigned int normal = (*shaderProgramInUse_)[ShaderProgram::Normal].location();
                unsigned int texCoord = (*shaderProgramInUse_)[ShaderProgram::TexCoord].location();
                std::vector<GLint> attributeLocations;
                attributeLocations.push_back(position);
                attributeLocations.push_back(normal);
                attributeLocations.push_back(texCoord);
                mesh->setAttributeLocations(attributeLocations);

                // Create data (if not created earlier) + render
                mesh->createGPUSpecificData();
                if (mesh->type() == MeshResource::Type::DynamicMesh)
                {
                    mesh->renderDynamicMesh(position, normal, texCoord);
                }
                else
                {
                    mesh->render();
                }
            }

            void Renderer::renderWireframeBox(Matrix4 const& modelTransformation, Vector3 const& min, Vector3 const& max)
            {
                if (shaderProgramInUse_ != NULL)
                {
                    shaderProgramInUse_->stop();
                }

                shaderProgramInUse_ = &shaderProgramDebugPhysics_;
                shaderProgramInUse_->start();

                // mvp to shader
                Matrix4 mvp = viewProjection_;
                mvp *= modelTransformation;
                (*shaderProgramInUse_)[ShaderProgram::ModelViewProjectionMatrix] = mvp;

                unsigned int position = (*shaderProgramInUse_)[ShaderProgram::Position].location();
                //LOGD("pos: %d", position);

                //
                // Create data
                GLfloat vertices[] =
                {
                    // Box "back", -Z plane rectangle
                    min.x(), min.y(), min.z(),
                    max.x(), min.y(), min.z(),

                    max.x(), min.y(), min.z(),
                    max.x(), max.y(), min.z(),

                    max.x(), max.y(), min.z(),
                    min.x(), max.y(), min.z(),

                    min.x(), max.y(), min.z(),
                    min.x(), min.y(), min.z(),

                    // Box "front", +Z plane rectangle
                    min.x(), min.y(), max.z(),
                    max.x(), min.y(), max.z(),

                    max.x(), min.y(), max.z(),
                    max.x(), max.y(), max.z(),

                    max.x(), max.y(), max.z(),
                    min.x(), max.y(), max.z(),

                    min.x(), max.y(), max.z(),
                    min.x(), min.y(), max.z(),

                    // Box "corners", connecting lines from front rectangle to back rectangle corners
                    min.x(), min.y(), min.z(),
                    min.x(), min.y(), max.z(),

                    min.x(), max.y(), min.z(),
                    min.x(), max.y(), max.z(),

                    max.x(), min.y(), min.z(),
                    max.x(), min.y(), max.z(),

                    max.x(), max.y(), min.z(),
                    max.x(), max.y(), max.z(),
                };
                unsigned int numberOfLines = 12;

                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, numberOfLines * 6 /* #lines * 2 points having each 3 floats */ * sizeof(float), vertices, GL_STATIC_DRAW);

                // Draw
                glEnableVertexAttribArray(position);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                unsigned int vertexStride = 0; // separate VBOs
                glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, vertexStride, 0);

                glDrawArrays(GL_LINES, 0, numberOfLines * 2);

                // Delete data
                glDeleteBuffers(1, &vbo);
            }

            void Renderer::testRender(Matrix4 const& model)
            {
                shaderProgramInUse_ = &shaderProgramDebugPhysics_;
                shaderProgramInUse_->start();

                // mvp to shader
                Matrix4 mvp = viewProjection_;
                mvp *= model;
                model.print();
                (*shaderProgramInUse_)[ShaderProgram::ModelViewProjectionMatrix] = mvp;

                Matrix4 normalMatrix(model);
                normalMatrix.inverse().transpose();
                (*shaderProgramInUse_)[ShaderProgram::NormalMatrix] = normalMatrix;

                unsigned int position = (*shaderProgramInUse_)[ShaderProgram::Position].location();
                //LOGD("pos: %d", position);

                //
                // Create data
                GLfloat vertices[] =
                {
                    -100.0f, -100.0f, 0.0f,
                    100.0f, 100.0f, 0.0f,

                    -100.0f, 100.0f, 0.0f,
                    100.0f, -100.0f, 0.0f
                };
                unsigned int numberOfLines = 2;

                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, numberOfLines * 6 /* #lines * 2 points having each 3 floats */ * sizeof(float), vertices, GL_STATIC_DRAW);

                // Draw
                glEnableVertexAttribArray(position);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                unsigned int vertexStride = 0; // separate VBOs
                glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, vertexStride, 0);

                glDrawArrays(GL_LINES, 0, numberOfLines * 2);

                // Delete data
                glDeleteBuffers(1, &vbo);
            }

            void Renderer::setDebugNormals(bool value)
            {
                debugNormals_ = value;
            }

            void Renderer::initFrame()
            {
                // Reset matrix stack
                matrixStack_.clear();

                // Reset camera
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = cameraNode_.component<TransformationComponentPrivate>();
                TransformationComponentPrivate* transformationComponentPrivate = transformationComponent.pointer();
                ReferencedPointer<CameraComponentPrivate> cameraComponent = cameraNode_.component<CameraComponentPrivate>();
                CameraComponentPrivate* cameraComponentPrivate = cameraComponent.pointer();
                viewProjection_ = cameraComponentPrivate->viewProjection(transformationComponentPrivate->location(), transformationComponentPrivate->rotation());
            }

#ifndef ANDROID
/*
            void Renderer::render(Matrix4 const& model, kaski::Text* text, int startIndex, int count, float opacity)
            {
                if (shaderProgramInUse_ != NULL)
                {
                    shaderProgramInUse_->stop();
                }
                shaderProgramInUse_ = shaderProgramText_;
                shaderProgramInUse_->start();
                glEnable(GL_BLEND);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                Matrix4 mvp = viewProjection_;
                mvp *= model;
                (*shaderProgramInUse_)[ShaderProgram::ModelViewProjectionMatrix] = mvp;
                (*shaderProgramInUse_)[ShaderProgram::Opacity] = opacity;
                (*shaderProgramInUse_)[ShaderProgram::Highlight] = 0.0f;
                text->draw(shaderProgramInUse_->id(), startIndex, count, opacity);
            }
*/
#endif

            void Renderer::setDeviceOrientation(Matrix4 const& matrix)
            {
                deviceOrientation_ = matrix;
            }
        }
    }
}
