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

#include "MainView.hpp"

using namespace Grapevine;

MainView::MainView(unsigned int width, unsigned int height) :
        canvas_(width, height),
        cameraComponent_(canvas_, 69.4f, 1.0f, 10000.0f),  // Mate 9 front camera, 26mm equivalent
        cameraTransformationComponent_(Vector3(0.0f, 0.0f, 500.0f)),
        cameraNode_(cameraComponent_, cameraTransformationComponent_),
        transformationComponent_(Vector3(0.0f, 0.0f, 0.0f)),
        width_(width),
        height_(height),
        scene_(cameraNode_),
        meshAsset_("facebook.barrel"),
        //textureAsset_("templatemask.png"),
        textureAsset_("clownmask.png"),
        materialAsset_(textureAsset_, 0.5f, 0.5f),
        renderer_(cameraNode_),
        //maskAsset_(NULL),
        interpolationSteps(1) {
    canvas_.setClearColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    LOGD("Creating scene...");
    initScene();

    std::vector<std::vector<float>> uvCoordinates;
    createUVCoordinates(uvCoordinates);
    for (unsigned i = 0; i < 74; i++) {
        uvs.push_back(Grapevine::Vector2(uvCoordinates[i][0], uvCoordinates[i][1]));
    }

    // Initialize index buffer for mask (directions from front, index is -1 from i-bug 68 values)
    // https://ibug.doc.ic.ac.uk/resources/facial-point-annotations/

    // 17 fore head triangles
    addTriangle(0, 17, 68); // 0
    addTriangle(17, 68, 69); // 1
    addTriangle(17, 18, 69); // 2
    addTriangle(18, 19, 69); // 3
    addTriangle(19, 72, 69); // 4
    addTriangle(19, 20, 72); // 5
    addTriangle(20, 21, 72); // 6
    addTriangle(69, 70, 72); // 7
    addTriangle(21, 22, 72); // 8
    addTriangle(22, 23, 72); // 9
    addTriangle(23, 24, 72); // 10
    addTriangle(70, 71, 72); // 11
    addTriangle(24, 71, 72); // 12
    addTriangle(24, 25, 71); // 13
    addTriangle(25, 26, 71); // 14
    addTriangle(26, 71, 72); // 15
    addTriangle(26, 16, 72); // 16

    // 17 onwards "eye top triangles"
    addTriangle(0, 1, 17);   // 17
    addTriangle(1, 17, 36);  // 18
    addTriangle(17, 18, 36); // 19
    addTriangle(36, 18, 37); // 20
    addTriangle(18, 19, 37); // 21
    addTriangle(19, 20, 37); // 22
    addTriangle(37, 38, 20); // 23
    addTriangle(38, 20, 21); // 24
    addTriangle(38, 39, 21); // 25
    addTriangle(39, 27, 21); // 26
    addTriangle(21, 22, 27); // 27
    addTriangle(27, 22, 42); // 28
    addTriangle(43, 22, 42); // 29
    addTriangle(23, 22, 43); // 30
    addTriangle(43, 44, 23); // 31
    addTriangle(23, 24, 44); // 32
    addTriangle(24, 25, 44); // 33
    addTriangle(44, 45, 25); // 34
    addTriangle(25, 26, 45); // 35
    addTriangle(45, 26, 15); // 36
    addTriangle(15, 16, 26); // 37

    // 38 and onwards "eye and cheek triangles" right side
    addTriangle(14, 15, 45); // 38
    addTriangle(14, 45, 46); // 39
    addTriangle(44, 45, 46); // 40
    addTriangle(44, 46, 47); // 41
    addTriangle(43, 44, 47); // 42
    addTriangle(42, 43, 47); // 43
    addTriangle(42, 29, 47); // 44
    addTriangle(28, 29, 42); // 45
    addTriangle(27, 28, 42); // 46
    addTriangle(29, 35, 47); // 47
    addTriangle(47, 46, 35); // 48
    addTriangle(35, 46, 14); // 49
    addTriangle(30, 35, 29); // 50
    addTriangle(30, 34, 35); // 51
    addTriangle(33, 34, 30); // 52

    // 53 and onwards "eye and cheek triangles" left side
    addTriangle(30, 32, 33); // 53
    addTriangle(30, 31, 32); // 54
    addTriangle(29, 30, 31); // 55
    addTriangle(29, 31, 40); // 56
    addTriangle(29, 40, 39); // 57
    addTriangle(28, 29, 39); // 58
    addTriangle(27, 28, 39); // 59
    addTriangle(38, 39, 40); // 60
    addTriangle(37, 38, 40); // 61
    addTriangle(37, 40, 41); // 62
    addTriangle(31, 40, 41); // 63
    addTriangle(31, 41, 2);  // 64
    addTriangle(36, 37, 41); // 65
    addTriangle(2, 36, 41);  // 66
    addTriangle(1, 2, 36);   // 67

    // 68 and onwards "top of upper lip"
    addTriangle(2, 3, 48);   // 68
    addTriangle(2, 31, 48);  // 69
    addTriangle(48, 49, 31); // 70
    addTriangle(49, 31, 32); // 71
    addTriangle(49, 50, 32); // 72
    addTriangle(32, 33, 50); // 73
    addTriangle(50, 51, 33); // 74
    addTriangle(51, 52, 33); // 75
    addTriangle(33, 34, 52); // 76
    addTriangle(52, 34, 53); // 77
    addTriangle(34, 35, 53); // 78
    addTriangle(53, 54, 35); // 79
    addTriangle(35, 54, 14); // 80
    addTriangle(13, 14, 54); // 81

    // 82 onwards, mouth - left side
    addTriangle(48, 49, 60); // 82
    addTriangle(48, 59, 60); // 83
    addTriangle(49, 60, 59); // 84
    addTriangle(49, 59, 61); // 85
    addTriangle(49, 50, 61); // 86
    addTriangle(61, 59, 67); // 87
    addTriangle(59, 67, 58); // 88
    addTriangle(57, 58, 66); // 89
    addTriangle(58, 67, 66); // 90
    addTriangle(66, 67, 62); // 91
    addTriangle(67, 61, 62); // 92
    addTriangle(61, 62, 51); // 93
    addTriangle(50, 51, 61); // 94

    // 95 onwards, mouth - right side
    addTriangle(51, 52, 63); // 95
    addTriangle(51, 62, 63); // 96
    addTriangle(65, 62, 63); // 97
    addTriangle(62, 65, 66); // 98
    addTriangle(65, 66, 56); // 99
    addTriangle(56, 57, 66); // 100
    addTriangle(55, 56, 65); // 101
    addTriangle(55, 65, 63); // 102
    addTriangle(55, 63, 53); // 103
    addTriangle(63, 52, 53); // 104
    addTriangle(53, 55, 64); // 105
    addTriangle(54, 55, 64); // 106
    addTriangle(53, 54, 64); // 107

    // 108 onwards, chin
    addTriangle(12, 13, 54); // 108
    addTriangle(11, 12, 54); // 109
    addTriangle(54, 55, 11); // 110
    addTriangle(10, 11, 55); // 111
    addTriangle(55, 56, 10); // 112
    addTriangle(9, 10, 56); // 113
    addTriangle(9, 56, 57); // 114
    addTriangle(8, 9, 57);  // 115
    addTriangle(7, 8, 57);  // 116
    addTriangle(7, 57, 58); // 117
    addTriangle(6, 7, 58);  // 118
    addTriangle(6, 58, 59); // 119
    addTriangle(5, 6, 59);  // 120
    addTriangle(48, 59, 5); // 121
    addTriangle(4, 5, 48); // 122
    addTriangle(3, 4, 48); // 123

/*
    Simple triangle set for debugging:

    addTriangle(48, 51, 54); // Mouth upper
    addTriangle(48, 54, 57); // Mouth lower

    addTriangle(36, 38, 39); // Left eye upper
    addTriangle(36, 39, 40); // Left eye lower

    addTriangle(42, 43, 45); // Right eye upper
    addTriangle(42, 45, 47); // Right eye lower

    addTriangle(48, 51, 54); // Mouth upper
    addTriangle(48, 54, 57); // Mouth lower

    addTriangle(17, 19, 21); // Left eyebrow

    addTriangle(22, 24, 26); // Right eyebrow

    addTriangle(31, 27, 35); // Nose
*/

    LOGD("MainView constructor end");
}

MainView::~MainView() {
}

bool MainView::step(bool haveFace, bool newPose, float px, float py, float pz, float *modelView,
                    float *frustumScale, float *landmarks) {
    bool quitApplication = false;
    input(quitApplication);
    draw(haveFace, newPose, px, py, pz, modelView, frustumScale, landmarks);

    return !quitApplication;
}

void MainView::surfaceChanged(unsigned int width, unsigned int height) {
    LOGI("MainView::surfaceChanged(), width:%d, height:%d", width, height);
    canvas_.resize(width, height);
}

void MainView::input(bool &quitApplication) {
    InputEvent event;
}

void MainView::draw(bool haveFace, bool newPose, float px, float py, float pz, float *matrix,
                    float *frustumScale, float *landmarks) {
    if (newPose) {
        previousPose = pose; // Save current intepolated pose as previous
        targetPose.ax = px;
        targetPose.ay = py;
        targetPose.az = pz;
        targetPose.tx = frustumScale[0];
        targetPose.ty = frustumScale[1];
        targetPose.scale = frustumScale[2];
        memcpy(targetPose.landmarks, landmarks, 2 * 68 * sizeof(float));

        interpolationSteps = frameCalls + 1;
        frameCalls = 0;
        interpolation = 0;
    } else {
        frameCalls++;
    }

    if (haveFace_ != haveFace && haveFace) {
        // Use values as such as the face just became visible
        previousPose = targetPose;
    }

    // Interpolate current pose
    interpolation += 1.0f / interpolationSteps;
    float t = std::min(1.0f, interpolation);
    pose.ax = interpolateValue(previousPose.ax, targetPose.ax, t);
    pose.ay = interpolateValue(previousPose.ay, targetPose.ay, t);
    pose.az = interpolateValue(previousPose.az, targetPose.az, t);
    pose.tx = interpolateValue(previousPose.tx, targetPose.tx, t);
    pose.ty = interpolateValue(previousPose.ty, targetPose.ty, t);
    pose.scale = interpolateValue(previousPose.scale, targetPose.scale, t);

    for (unsigned i = 0; i < 2 * 68; i++)
        pose.landmarks[i] = interpolateValue(previousPose.landmarks[i], targetPose.landmarks[i], t);

    float viewScale = (float) canvas_.width() / canvas_.height();
    // Generate rudimentary mesh
    const int scale = 1440;
    std::vector<Vector3> positions;
    for (unsigned i = 0; i < 68; i++) {
        positions.push_back(Vector3(pose.landmarks[i * 2] * scale * viewScale / 2.0f,
                                    pose.landmarks[i * 2 + 1] * scale / -2.0f, 0));
        //positions.push_back(Vector3(landmarks[i*2], landmarks[i*2+1], 0));
    }

    // Create new forehead vertices
    Vector2 diff30to27 =
            Vector2(0.0f, pose.landmarks[27 * 2 + 1]) - Vector2(pose.landmarks[30 * 2 + 1]);
    Vector3 diff30to27_3D = Vector3(0.0f, diff30to27.y() * scale / -2.0f, 0);

    positions.push_back(Vector3(pose.landmarks[0 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[0 * 2 + 1] * scale / -2, 0) +
                        2.0f * diff30to27_3D); // 68
    positions.push_back(Vector3(pose.landmarks[18 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[18 * 2 + 1] * scale / -2, 0) +
                        2.0f * diff30to27_3D); // 69
    positions.push_back(Vector3(pose.landmarks[27 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[27 * 2 + 1] * scale / -2, 0) +
                        3.0f * diff30to27_3D); // 70
    positions.push_back(Vector3(pose.landmarks[25 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[25 * 2 + 1] * scale / -2, 0) +
                        2.0f * diff30to27_3D); // 71
    positions.push_back(Vector3(pose.landmarks[16 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[16 * 2 + 1] * scale / -2, 0) +
                        2.0f * diff30to27_3D); // 72
    positions.push_back(Vector3(pose.landmarks[27 * 2] * scale * viewScale / 2.f,
                                pose.landmarks[27 * 2 + 1] * scale / -2, 0) +
                        2.0f * diff30to27_3D); // 73

    std::vector<Vector3> normals;  // TODO construct from vertices

    scene_.remove(maskNode_);
    Grapevine::MeshAsset maskAsset(positions, uvs, normals, indices);
    Grapevine::Scene::Extensions::ModelComponent modelComponent(maskAsset, materialAsset_);
    maskNode_ = Scene::Node(maskTransformationComponent_, modelComponent);
    scene_.addNode(maskNode_);

//    LOGI("MainView::draw() %f %f", frustumScale[0], frustumScale[1]);
//    transformationComponent_.rotate(Matrix::rotate(1.0f, Vector3(0, 1, 0)));
    transformationComponent_.setOpacity(haveFace ? 1.0f : 0.0f);
//    transformationComponent_.setRotation(Matrix::eulerAngles(pose.ax, pose.ay, pose.az));
    Matrix4 rotationMatrix;
    for (unsigned j = 0; j < 4; j++)
        for (unsigned i = 0; i < 4; i++)
            rotationMatrix[j][i] = matrix[j * 4 + i];
    transformationComponent_.setRotation(rotationMatrix);

//    float scale = pose.scale * 5.0f;
//    transformationComponent_.setScale(Vector3(scale, scale, scale));
//    transformationComponent_.setLocation(
//            Vector3(pose.tx * width_ / 2, pose.ty * height_ / 2, 0));
    transformationComponent_.setLocation(
            Vector3(pose.tx / 10, pose.ty / 10, (pose.scale / 10) + 540));
    scene_.draw(canvas_);

    haveFace_ = haveFace;
    //Matrix4 tmp(matrix);
    //renderer_.render(tmp, materialAsset_.resource(), meshAsset_.resource(), 0.5f);
}

void MainView::initScene() {
    LOGI("MainView::initScene()");

    {
        if (0) {
            Grapevine::MeshAsset meshAsset("facebook.barrel");
            Grapevine::TextureAsset texture("facebook_diffuse.png");
            Grapevine::MaterialAsset materialAsset(texture, 0.5f, 0.5f);
            Grapevine::Scene::Extensions::ModelComponent modelComponent(meshAsset, materialAsset);

            Scene::Node tmpNode(
                    transformationComponent_,
                    modelComponent
            );
            scene_.addNode(tmpNode);
        }

        // new face-like asset: "dummy smiley"
        {
            Grapevine::MeshAsset meshAsset("dummy_smiley_blacks.3DS");
            Grapevine::TextureAsset texture("dummy_smiley_black.png");
            Grapevine::MaterialAsset materialAsset(texture, 0.5f, 0.5f);
            Grapevine::Scene::Extensions::ModelComponent modelComponent(meshAsset, materialAsset);

            Scene::Node tmpNode(
                    transformationComponent_,
                    modelComponent
            );
            //scene_.addNode(tmpNode);
        }

        {
            Grapevine::MeshAsset meshAsset("dummy_smiley_yellows.3DS");
            Grapevine::TextureAsset texture("dummy_smiley_yellow.png");
            Grapevine::MaterialAsset materialAsset(texture, 0.5f, 0.5f);
            Grapevine::Scene::Extensions::ModelComponent modelComponent(meshAsset, materialAsset);

            Scene::Node tmpNode(
                    transformationComponent_,
                    modelComponent
            );
            //scene_.addNode(tmpNode);
        }
    }
}

void MainView::createUVCoordinates(std::vector<std::vector<float>> &uvCoordinates) {
    // NOTE: these coordinates are generated from average face. See doc faceuv and
    // separate facemask-skinningprototyping/dlib/example/face_pose*cpp for more details.
    uvCoordinates.push_back({0.1f, 0.619393f});
    uvCoordinates.push_back({0.103604f, 0.531308f});
    uvCoordinates.push_back({0.110811f, 0.443224f});
    uvCoordinates.push_back({0.125225f, 0.35514f});
    uvCoordinates.push_back({0.157658f, 0.271612f});
    uvCoordinates.push_back({0.218919f, 0.203271f});
    uvCoordinates.push_back({0.301802f, 0.150117f});
    uvCoordinates.push_back({0.393694f, 0.110631f});
    uvCoordinates.push_back({0.507207f, 0.1f});
    uvCoordinates.push_back({0.606306f, 0.110631f});
    uvCoordinates.push_back({0.698198f, 0.150117f});
    uvCoordinates.push_back({0.781081f, 0.203271f});
    uvCoordinates.push_back({0.842342f, 0.271612f});
    uvCoordinates.push_back({0.874775f, 0.35514f});
    uvCoordinates.push_back({0.889189f, 0.443224f});
    uvCoordinates.push_back({0.896396f, 0.531308f});
    uvCoordinates.push_back({0.9f, 0.619393f});
    uvCoordinates.push_back({0.172072f, 0.689252f});
    uvCoordinates.push_back({0.218919f, 0.733294f});
    uvCoordinates.push_back({0.289189f, 0.75f});
    uvCoordinates.push_back({0.359459f, 0.743925f});
    uvCoordinates.push_back({0.427928f, 0.724182f});
    uvCoordinates.push_back({0.572072f, 0.724182f});
    uvCoordinates.push_back({0.640541f, 0.743925f});
    uvCoordinates.push_back({0.710811f, 0.75f});
    uvCoordinates.push_back({0.781081f, 0.733294f});
    uvCoordinates.push_back({0.827928f, 0.689252f});
    uvCoordinates.push_back({0.5f, 0.65736f});
    uvCoordinates.push_back({0.5f, 0.608762f});
    uvCoordinates.push_back({0.5f, 0.560164f});
    uvCoordinates.push_back({0.5f, 0.508528f});
    uvCoordinates.push_back({0.417117f, 0.462967f});
    uvCoordinates.push_back({0.46036f, 0.455374f});
    uvCoordinates.push_back({0.5f, 0.449299f});
    uvCoordinates.push_back({0.53964f, 0.455374f});
    uvCoordinates.push_back({0.582883f, 0.462967f});
    uvCoordinates.push_back({0.258559f, 0.636098f});
    uvCoordinates.push_back({0.301802f, 0.65736f});
    uvCoordinates.push_back({0.354054f, 0.65736f});
    uvCoordinates.push_back({0.399099f, 0.633061f});
    uvCoordinates.push_back({0.352252f, 0.623949f});
    uvCoordinates.push_back({0.3f, 0.623949f});
    uvCoordinates.push_back({0.600901f, 0.633061f});
    uvCoordinates.push_back({0.645946f, 0.65736f});
    uvCoordinates.push_back({0.698198f, 0.65736f});
    uvCoordinates.push_back({0.741441f, 0.636098f});
    uvCoordinates.push_back({0.7f, 0.623949f});
    uvCoordinates.push_back({0.647748f, 0.623949f});
    uvCoordinates.push_back({0.336036f, 0.347547f});
    uvCoordinates.push_back({0.393694f, 0.379439f});
    uvCoordinates.push_back({0.458559f, 0.394626f});
    uvCoordinates.push_back({0.5f, 0.388551f});
    uvCoordinates.push_back({0.541441f, 0.394626f});
    uvCoordinates.push_back({0.606306f, 0.379439f});
    uvCoordinates.push_back({0.663964f, 0.347547f});
    uvCoordinates.push_back({0.608108f, 0.291355f});
    uvCoordinates.push_back({0.545045f, 0.268575f});
    uvCoordinates.push_back({0.5f, 0.265537f});
    uvCoordinates.push_back({0.454955f, 0.268575f});
    uvCoordinates.push_back({0.391892f, 0.291355f});
    uvCoordinates.push_back({0.361261f, 0.344509f});
    uvCoordinates.push_back({0.458559f, 0.362734f});
    uvCoordinates.push_back({0.5f, 0.359696f});
    uvCoordinates.push_back({0.541441f, 0.362734f});
    uvCoordinates.push_back({0.638739f, 0.344509f});
    uvCoordinates.push_back({0.541441f, 0.318692f});
    uvCoordinates.push_back({0.5f, 0.314136f});
    uvCoordinates.push_back({0.458559f, 0.318692f});
    uvCoordinates.push_back({0.1f, 0.827757f});
    uvCoordinates.push_back({0.218919f, 0.941659f});
    uvCoordinates.push_back({0.5f, 0.969907f});
    uvCoordinates.push_back({0.781081f, 0.941659f});
    uvCoordinates.push_back({0.9f, 0.827757f});
    uvCoordinates.push_back({0.5f, 0.865724f});
}
