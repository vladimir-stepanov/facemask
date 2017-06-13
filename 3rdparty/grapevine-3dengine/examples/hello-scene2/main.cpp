//!

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/TransformationComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/SceneExtensions/CameraComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/ModelComponent.hpp>
//#include <Grapevine/SceneExtensions/TextComponent.hpp>
//#include <Grapevine/SceneExtensions/AnimationComponent.hpp>
//#include <Grapevine/SceneExtensions/AnimationProgressTriggerComponent.hpp>
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
#include <Grapevine/Input/InputEvent.hpp>
#include <Grapevine/Input/InputSource.hpp>

using namespace Grapevine;

// // // // // //      T E S T   C L A S S      // // // // // //
static const float maxOmniLightStrength = 200.0f;

class Main
{
public:
    Main():
        canvas_(1280, 720),
        inputSource_(canvas_),
        resourceManager_("assets/"),
        transformationComponent_(Vector3(200.0f, 0.0f, 500.0f)),
/*        animationComponent_(
            Animation(
                5000.0f,
                Easing(Easing::Type::InOutQuad),
                LinearPath(
                    ControlPoint(Vector3(0.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.5f),
                    ControlPoint(Vector3(1000.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.1f)
                )
            )
        ),
        textAnimationComponent_(
            Animation(
                10000.0f,
                Easing(Easing::Type::InOutQuad),
                BezierQuadraticPath(
                    ControlPoint(Vector3(0.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 1.0f),
                    ControlPoint(Vector3(500.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.0f),
                    ControlPoint(Vector3(1000.0f, -1000.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 1.0f)
                )
            )
        ),*/
        //animationProgressTriggerComponent_(0.5f),
        cameraComponent_(canvas_),
        cameraTransformationComponent_(Vector3(0.0f, 0.0f, 1000.0f)),
        cameraNode_(cameraComponent_, cameraTransformationComponent_),
        scene_(cameraNode_),
        timer_(),
        pX0Y0_(Vector3(0, 0.0f, 0.0f)),
        pX1Y0_(Vector3(1000, 0.0f, 0.0f)),
        pX0Y1_(Vector3(0, 1000.0f, 0.0f)),
        pX1Y1_(Vector3(1000, 1000.0f, 0.0f)),
        omniLightPlane_(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f))
    {
        LOGD("moro");

        initAnimationTest();
        scene_.addNode(cameraNode_);

        // Testing API
        std::vector<ReferencedPointer<Scene::NodePrivate> > tmp1 = Scene::NodePrivate::componentOwners<Scene::Extensions::CameraComponent>();
        LOGD("camera component owners: %d", tmp1.size());
        std::vector<ReferencedPointer<Scene::NodePrivate> > tmp2 = Scene::NodePrivate::componentOwners<Scene::Extensions::TransformationComponent>();
        LOGD("transformation component owners: %d", tmp2.size());

        //std::vector<Scene::Node> tmp = nodesWithComponent<TransformationComponent>();
    }

    void initAnimationTest()
    {
        // Dynamic stuff
        {
            // Create dynamic mesh
            std::vector<ControlPoint> upperRow;
            upperRow.push_back(pX0Y0_);
            upperRow.push_back(pX1Y0_);
            std::vector<ControlPoint> lowerRow;
            lowerRow.push_back(pX0Y1_);
            lowerRow.push_back(pX1Y1_);
            std::vector< std::vector<ControlPoint> > grid;
            grid.push_back(upperRow);
            grid.push_back(lowerRow);
            DynamicMesh dynamicMesh(grid);

            // Create model asset, smoke testing...
            MeshAsset dynamicMeshAsset(dynamicMesh);
            MaterialAsset materialAsset(Vector4(1.0f, 0.0f, 0.0f, 0.5f), 0.25f);

            Scene::Extensions::TransformationComponent transformationComponent(Vector3(0.0f, 0.0f, 0.0f));
            Scene::Extensions::ModelComponent dynamicModelComponent(dynamicMeshAsset, materialAsset);
            Scene::Node dynamicNode(transformationComponent, dynamicModelComponent);
            ReferencedPointer<Scene::Extensions::TransformationComponentPrivate> tmp2 = dynamicNode.component<Scene::Extensions::TransformationComponentPrivate>();
            if(tmp2.isNull() == false)
            {
                LOGD("OK 2");
            }
            ReferencedPointer<Scene::Extensions::CameraComponentPrivate> tmp3 = dynamicNode.component<Scene::Extensions::CameraComponentPrivate>();
            if(tmp3.isNull() == true)
            {
                LOGD("OK 3");
            }
            dynamicNode.removeComponent<Scene::Extensions::TransformationComponentPrivate>();
            ReferencedPointer<Scene::Extensions::TransformationComponentPrivate> tmp4 = dynamicNode.component<Scene::Extensions::TransformationComponentPrivate>();
            if(tmp4.isNull() == true)
            {
                LOGD("OK 4");
            }
            dynamicNode.addComponent(Scene::Extensions::TransformationComponent(Vector3(0.0f, 0.0f, 0.0f)));
            ReferencedPointer<Scene::Extensions::TransformationComponentPrivate> tmp5 = dynamicNode.component<Scene::Extensions::TransformationComponentPrivate>();
            if(tmp5.isNull() == false)
            {
                LOGD("OK 5");
            }
            scene_.addNode(dynamicNode);
        }

        // Simple animation test
        {
            /*
            Scene::Node node1(
                Scene::Extensions::TransformationComponent(Vector3(-200.0f, 0.0f, 0.0f)),
                textAnimationComponent_);
            textAnimationComponent_.start();
            Scene::Node node1b;
            node1.addChild(node1b);

            Scene::Node node1c(
                Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            node1b.addChild(node1c);
            scene_.addNode(node1);
*/
            Scene::Node node2(
                transformationComponent_,
                Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel")));

            Scene::Node node3(
                Scene::Extensions::TransformationComponent(Vector3(200.0f, 400.0f, 0.0f)),
                Scene::Extensions::ModelComponent(ModelAsset("unlock_ball.barrel")));
            scene_.addNode(node2);
            scene_.addNode(node3);
        }
/*
        Scene::Node nodeAnimSurface(
            Scene::Extensions::TransformationComponent(pX0Y0_),
            animationComponent_,
            animationProgressTriggerComponent_);
        scene_.addNode(nodeAnimSurface);

        animationComponent_.start();*/
    }

    void run()
    {
        static unsigned int startTime = Timer::milliseconds();
        canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
        FpsCounter fpsCounter("FPS");
        bool quitApplication = false;
        while (quitApplication == false)
        {
            input(quitApplication);
            fpsCounter.startStep();
            float rotation = ((float)Timer::milliseconds() - (float)startTime) * 0.1f;
            Matrix4 tmpRotation = Matrix::rotate(rotation, Vector::yAxis);
            transformationComponent_.setRotation(tmpRotation);
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
                }
                else
                {
                    /*
                    float x = (float)event.touch.x / (float)canvas_.width() - 0.5f;
                    float y = (float)event.touch.y / (float)canvas_.height() - 0.5f;
                    x *= 2.0f;
                    y *= -2.0f;
                    touchBasedDirection = Vector3(x, y, 1.0f);
                    LOGD("%f, %f", x, y);
                    pressed_ = true;
                    */
                }

                // pressed
                    /*
                pressedX = (float)event.touch.x;
                pressedY = (float)event.touch.y;


                Ray tmp = cameraComponent_.mouseCoordinatesToRay(event.touch.x, event.touch.y, cameraTransformationComponent_.location(), cameraTransformationComponent_.rotation());
                RayPlaneIntersection tmp2 = tmp.intersection(omniLightPlane_);
                if (tmp2.type() == RayPlaneIntersection::Type::PointIntersection)
                {
                    // LOGI("p %f, %f", tmp2.p().x(), tmp2.p().y());
                    omniLightLocation = 0.5f * (omniLightLocation + Vector3(tmp2.p().x(), tmp2.p().y(), tmp2.p().z()));
                } */
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

private:
    Canvas canvas_;
    InputSource inputSource_;
    ResourceManager resourceManager_;
    Scene::Extensions::TransformationComponent cameraTransformationComponent_;
    Scene::Extensions::CameraComponent cameraComponent_;
    Scene::Node cameraNode_;
    Scene::Extensions::SimpleScene scene_;
    Scene::Extensions::TransformationComponent transformationComponent_;
    //Scene::Extensions::AnimationComponent animationComponent_;
    //Scene::Extensions::AnimationComponent textAnimationComponent_;
    //Scene::Extensions::AnimationProgressTriggerComponent animationProgressTriggerComponent_;
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
