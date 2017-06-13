//!

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/TransformationComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/SceneExtensions/CameraComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/ModelComponent.hpp>
#include <Grapevine/SceneExtensions/TextComponent.hpp>
#include <Grapevine/SceneExtensions/AnimationComponent.hpp>
#include <Grapevine/SceneExtensions/AnimationProgressTriggerComponent.hpp>
#include <Grapevine/SceneExtensions/SimpleScene.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Model/ModelAsset.hpp>
#include <Grapevine/Material/MaterialAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <Grapevine/Animation/Animation.hpp>
#include <Grapevine/Animation/LinearPath.hpp>
#include <Grapevine/Animation/BezierQuadraticPath.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Model/DynamicMesh.hpp>
#include <Grapevine/Model/ModelAsset.hpp>
#include <Grapevine/Arrange/Grid.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Input/InputSource.hpp>

using namespace Grapevine;

// // // // // //      T E S T   C L A S S      // // // // // //
static const float maxOmniLightStrength = 1000.0f;
static const unsigned int WAVES_WIDTH = 60;
static const unsigned int WAVES_HEIGHT = 60;

class Main
{
public:
    Main():
        canvas_(1600, 900),
        inputSource_(canvas_),
        resourceManager_("assets/"),
        cameraComponent_(canvas_),
        cameraTransformationComponent_(Vector3(0.0f, 0.0f, 1000.0f)),
        cameraNode_(cameraComponent_, cameraTransformationComponent_),
        scene_(cameraNode_),
        timer_(),
        omniLightPlane_(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
        animationComponentForClock_(
            Animation(
                3000.0f,
                Easing(Easing::Type::InOutQuad),
                LinearPath(
                    ControlPoint(Vector3(0.0f, 1000.0f, 0.0f), Matrix::rotateRadians(1.5f, Vector::yAxis), Vector3(1.0f, 1.0f, 1.0f), 0.0f),
                    ControlPoint(Vector3(-2000.0f, 600.0f, -200.0f), Matrix::rotateRadians(0.2f, Vector::yAxis), Vector3(1.0f, 1.0f, 1.0f), 1.0f)
                )
            )
        ),
        faceTransformationComponent_()
    {
        initScene();
        scene_.addNode(cameraNode_);
    }

    void initScene()
    {
        // clock
        {
            Matrix4 rotation0 = Matrix::rotateRadians(1.5f, Vector::yAxis);
            Matrix4 rotation1 = Matrix::rotateRadians(0.2f, Vector::yAxis);
            animationComponentForClock_.start();

            Scene::Node clockNodeRotator(
                Scene::Extensions::TransformationComponent(),
                animationComponentForClock_
                );

            Scene::Extensions::TextComponent clockText(Grapevine::RichText(Text("20:46",FontAsset("Ubuntu-B.ttf"), 700, Vector4(1, 1, 1, 0.75f))));
            Scene::Node clockNode(
                Scene::Extensions::TransformationComponent(Vector3(0.0f, 0.0f, 0.0f)),
                clockText
                );

            Scene::Extensions::TextComponent adText(Grapevine::RichText(Text("Wave and 3D FX!",FontAsset("Ubuntu-B.ttf"), 100, Vector4(1, 1, 1, 0.75f))));
            Scene::Node adNode(
                Scene::Extensions::TransformationComponent(Vector3(0.0f, -100.0f, 0.0f)),
                adText
                );

            clockNodeRotator.addChild(clockNode);
            clockNodeRotator.addChild(adNode);
            scene_.addNode(clockNodeRotator);

            float radius = -25000.0f;

            // Alpha for whole title text width
            Vector2 boxMin, boxMax;
            clockText.boundingBox(boxMin, boxMax);
            float textWidth = boxMax.x() - boxMin.x();
            float alphaRadians = textWidth / radius; // Rough estimate
            for (unsigned int i = 0; i < clockText.glyphCount(); i++)
            {
                Grapevine::Vector2 glyphPosition = clockText.glyphPosition(i);

                // Calculate rotation and placement
                float x = glyphPosition.x() / textWidth;
                float alphaGlyphStart = -alphaRadians / 2.0f + x * alphaRadians; // /2.0f comes because text is placed at node center

                Vector3 tmpPlacement(glyphPosition.x() - textWidth / 2.0f, 0.0f, radius);
                Matrix4 rotation = Matrix::rotateRadians(alphaGlyphStart, Vector::yAxis);
                Vector3 tmpPlacement2 = Vector::transform(tmpPlacement, rotation);
                Vector3 translate = tmpPlacement - tmpPlacement2;
                float scale = clockText.glyphScale(i);

                Matrix4 m;
                m *= Matrix::translate(textWidth / 2.0f + translate.x(), 0.0f, translate.z());
                m *= rotation;
                m *= Grapevine::Matrix::scale(scale, scale, 0);

                clockText.setGlyphTransform(i, m);
            }
            for (unsigned int i = 0; i < adText.glyphCount(); i++)
            {
                Grapevine::Vector2 glyphPosition = adText.glyphPosition(i);

                // Calculate rotation and placement
                float x = glyphPosition.x() / textWidth;
                float alphaGlyphStart = -alphaRadians / 2.0f + x * alphaRadians; // /2.0f comes because text is placed at node center

                Vector3 tmpPlacement(glyphPosition.x() - textWidth / 2.0f, 0.0f, radius);
                Matrix4 rotation = Matrix::rotateRadians(alphaGlyphStart, Vector::yAxis);
                Vector3 tmpPlacement2 = Vector::transform(tmpPlacement, rotation);
                Vector3 translate = tmpPlacement - tmpPlacement2;
                float scale = adText.glyphScale(i);

                Matrix4 m;
                m *= Matrix::translate(textWidth / 2.0f + translate.x(), 0.0f, translate.z());
                m *= rotation;
                m *= Grapevine::Matrix::scale(scale, scale, 0);

                adText.setGlyphTransform(i, m);
            }
        }

        // waves
        {
            // Create dynamic mesh
            for(unsigned int x = 0; x < WAVES_WIDTH; x++)
            {
                std::vector<ControlPoint> row;
                for(unsigned int y = 0; y < WAVES_HEIGHT; y++)
                {
                    row.push_back(ControlPoint(Vector3(-1000.0f + (float)x * 100.0f, -100.0f, -1000.0f + (float)y * 100.0f)));
                }
                waves_.push_back(row);
            }
            DynamicMesh waveMesh(waves_);

            // Create model asset
            MeshAsset dynamicMeshAsset(waveMesh);
            MaterialAsset materialAsset("borat.png", 0.5f, 0.5f);
            Scene::Extensions::TransformationComponent transformationComponent(Vector3(0.0f, 0.0f, 0.0f));
            Scene::Extensions::ModelComponent dynamicModelComponent(dynamicMeshAsset, materialAsset);
            Scene::Node waveNode(transformationComponent, dynamicModelComponent);
            scene_.addNode(waveNode);
        }

        // facebook
        {
            Scene::Node faceNode(
                faceTransformationComponent_,
                Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            scene_.addNode(faceNode);
        }

        // play button
        {
            Scene::Node playNode(
                Scene::Extensions::TransformationComponent(Vector3(1000.0f, 500.0f, 0.0f)),
                Scene::Extensions::ModelComponent(ModelAsset("play_button.barrel"))
                );
            scene_.addNode(playNode);
        }
    }

    void run()
    {
        static unsigned int startTime = Timer::milliseconds();
        canvas_.setClearColor(Vector4(0.5f, 0.5f, 1.0f, 1.0f));
        FpsCounter fpsCounter("FPS");
        bool quitApplication = false;
        while (quitApplication == false)
        {
            fpsCounter.startStep();
            input(quitApplication);
            processWaves();
            processFaceIcon();
            float rotation = ((float)Timer::milliseconds() - (float)startTime) * 0.1f;
            Matrix4 tmpRotation = Matrix::rotate(rotation, Vector::yAxis);
            scene_.draw(canvas_);
            Timer::sleepMilliseconds(16);
            fpsCounter.endStep();
        }
    }

    void input(bool& quitApplication)
    {
        InputEvent event;
        static bool pressed_ = false;

        // Light management
        static float defaultLightDirectionFactor = 1.0f;
        static Vector3 touchBasedDirection(0.0f, 0.0, 0.0f);
        Vector3 defaultLightDirection(0.23570f, 0.23570f, 0.94281f);
        static float pressedX = 0.0f;
        static float pressedY = 0.0f;
        static Vector3 omniLightColor(1.0f, 1.0f, 1.0f);
        static Vector3 omniLightLocation(0.0f, 0.0f, 0.0f);
        static float omniLightStrength = 0.0f;

        while (inputSource_.pollEvent(event))
        {
            // Touch
            if (event.type == InputEventType::Touch)
            {
                if (event.touch.flags & TouchFlags::Released)
                {
                    pressed_ = false;
                    animationComponentForClock_.start(); // restart clock anim
                }
                else
                {
                    float x = (float)event.touch.x / (float)canvas_.width() - 0.5f;
                    float y = (float)event.touch.y / (float)canvas_.height() - 0.5f;
                    x *= 2.0f;
                    y *= -2.0f;
                    touchBasedDirection = Vector3(x, y, 1.0f);
                    LOGD("%f, %f", x, y);
                    pressed_ = true;
                }

                // pressed
                pressedX = (float)event.touch.x;
                pressedY = (float)event.touch.y;

                Ray tmp = cameraComponent_.mouseCoordinatesToRay(event.touch.x, event.touch.y, cameraTransformationComponent_.location(), cameraTransformationComponent_.rotation());
                RayPlaneIntersection tmp2 = tmp.intersection(omniLightPlane_);
                if (tmp2.type() == RayPlaneIntersection::Type::PointIntersection)
                {
                    // LOGI("p %f, %f", tmp2.p().x(), tmp2.p().y());
                    omniLightLocation = 0.5f * (omniLightLocation + Vector3(tmp2.p().x(), tmp2.p().y(), tmp2.p().z()));
                }
            }
            // System quit
            else if (event.type == InputEventType::System)
            {
                quitApplication = true;
            }
        }

        // Light management
        if (pressed_)
        {
            omniLightStrength += (maxOmniLightStrength - omniLightStrength) / 4.0f; // 4.0f is a hard coded factor
            float x = (float)pressedX / (float)canvas_.width() - 0.5f;
            float y = (float)pressedY / (float)canvas_.height() - 0.5f;
            x *= 2.0f;
            y *= -2.0f;
            touchBasedDirection = Vector3(x, y, 1.0f);
        }

        if (pressed_ == false)
        {
            omniLightStrength /= 1.25f; // 1.25f is a hard coded factor
            defaultLightDirectionFactor += (1.0f - defaultLightDirectionFactor) / 2.0f;
            if (defaultLightDirectionFactor > 1.0f)
                defaultLightDirectionFactor = 1.0f;
        }
        else
        {
            defaultLightDirectionFactor = 0.0f;
        }
        Vector3 totalLight = defaultLightDirectionFactor * defaultLightDirection + (1.0f - defaultLightDirectionFactor) * touchBasedDirection;

        scene_.setOmniLightWorldLocation(omniLightLocation);
        scene_.setOmniLightColor(omniLightColor);
        scene_.setOmniLightStrength(omniLightStrength);
    }

    void processWaves()
    {
        static unsigned int startTime = Timer::milliseconds();
        unsigned int currentTime = Timer::milliseconds();
        unsigned int diff = currentTime - startTime;
        for(unsigned int x = 0; x < WAVES_WIDTH; x++)
        {
            for(unsigned int y = 0; y < WAVES_HEIGHT; y++)
            {
                float phase = sin(((float)(x % 8 + y % 8) * (float)diff / 10.0f) / 180.0f);
                ControlPoint tmp = waves_[x][y];
                tmp.setLocation((Vector3(-6000.0f + (float)x * 200.0f, -2200.0f + phase * 20.0f + (y * 50.0f), -1000.0f -(float)y * 200.0f)));
            }
        }
    }

    void processFaceIcon()
    {
        static unsigned int startTime = Timer::milliseconds();
        unsigned int currentTime = Timer::milliseconds();
        unsigned int diff = currentTime - startTime;
        float phase = sin((float)diff / 1800.0f) * 40.0f;

        //static unsigned int startTime = Timer::milliseconds();
        //bool quitApplication = false;
        float rotation = ((float)Timer::milliseconds() - (float)startTime) * 0.1f;
        Matrix4 tmpRotation = Matrix::rotate(phase, Vector::yAxis);
        faceTransformationComponent_.setLocation(Vector3(1000.0f, phase * 1.0f, 0));
        faceTransformationComponent_.setRotation(tmpRotation);
    }

    void handleTimeout(Timer* timer)
    {
        LOG
    }

    void handleAnimationProgressTrigger()
    {
        LOG
    }

private:
    Canvas canvas_;
    InputSource inputSource_;
    ResourceManager resourceManager_;
    Scene::Extensions::TransformationComponent cameraTransformationComponent_;
    Scene::Extensions::CameraComponent cameraComponent_;
    Scene::Node cameraNode_;
    Scene::Extensions::SimpleScene scene_;
    std::vector<std::vector<ControlPoint> > waves_;
    Scene::Extensions::AnimationComponent animationComponentForClock_;
    Scene::Extensions::TransformationComponent faceTransformationComponent_;

    Timer timer_;
    ControlPoint pX0Y0_;
    ControlPoint pX1Y0_;
    ControlPoint pX0Y1_;
    ControlPoint pX1Y1_;
    Grapevine::Plane omniLightPlane_;
};

// // // // // //       M A I N       // // // // // //

int main(int argc, char *argv[])
{
    LOG("hello-scene");

    Main* tmp = new Main();
    tmp->run();
    delete tmp;

    return 0;
}
