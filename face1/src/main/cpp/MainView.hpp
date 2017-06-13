/**
 * Copyright Huawei Technologies Oy 2017
 * author: antti.peuhkurinen@huawei.com
 *
 * DISCLAIMER:
 *
 * !!! PROTOTYPE LEVEL SOURCE CODE !!!

 * This source code is ment only for example and demonstration purposes.
 *
 */

#ifndef FACE1_MAINVIEW
#define FACE1_MAINVIEW

#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/SceneExtensions/ModelComponent.hpp>
#include <Grapevine/SceneExtensions/SimpleScene.hpp>
#include <vector>
#include "Grapevine/SceneExtensions/Renderer.hpp"

class MainView
{
public:
    MainView(unsigned int width, unsigned int height);
    ~MainView();
    bool step(bool haveFace, bool newPose, float px, float py, float pz, float* modelView, float* frustumScale, float* landmarks);
    void surfaceChanged(unsigned int width, unsigned int height);

private:
    void input(bool& quitApplication);
    void draw(bool haveFace, bool newPose, float px, float py, float pz, float* matrix, float* frustumScale, float* landmarks);
    void initScene();
    static void createUVCoordinates(std::vector<std::vector<float>>& uvCoordinates);

    inline float interpolateValue(float current, float target, float t)
    {
        return current + (target-current) * t;
    }

    inline void addTriangle(uint16_t a, uint16_t b, uint16_t c)
    {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
    }

    Grapevine::Canvas canvas_;
    Grapevine::FpsCounter fpsCounter_;
    unsigned int width_;
    unsigned int height_;

    // Camera
    Grapevine::Scene::Extensions::CameraComponent cameraComponent_;
    Grapevine::Scene::Extensions::TransformationComponent cameraTransformationComponent_;
    Grapevine::Scene::Node cameraNode_;

    // Main object transformation
    Grapevine::Scene::Extensions::TransformationComponent transformationComponent_;

    // Identity transform for mask
    Grapevine::Scene::Extensions::TransformationComponent maskTransformationComponent_;

    // Scene
    Grapevine::Scene::Extensions::SimpleScene scene_;

    // Dummy object
    Grapevine::MeshAsset meshAsset_;
    Grapevine::TextureAsset textureAsset_;
    Grapevine::MaterialAsset materialAsset_;
    Grapevine::Scene::Extensions::Renderer renderer_;

    //Grapevine::MeshAsset* maskAsset_;
    Grapevine::Scene::Node maskNode_;

    std::vector<Grapevine::Vector2> uvs;
    std::vector<uint16_t> indices;

    bool haveFace_;
    typedef struct
    {
        float ax, ay, az; // Current orientation angle
        float tx, ty; // Current translation
        float scale;
        float landmarks[2*68];
    } Pose;

    Pose previousPose;
    Pose targetPose;

    Pose pose;  // interpolated pose to use

    float interpolation;
    int interpolationSteps;
    int frameCalls;
};

#endif
