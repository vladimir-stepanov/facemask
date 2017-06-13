//!

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/SceneExtensions/SimpleScene.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Model/ModelResource.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Scene/AbstractComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/CameraComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/TransformationComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/SceneExtensions/CameraComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/ModelComponent.hpp>

using namespace Grapevine;

class Main
{
public:
    Main():
        canvas_(1280, 720),
        inputSource_(canvas_),
        resourceManager_("assets/"),
        cameraComponent_(canvas_),
        cameraTransformationComponent_(Vector3(0.0f, 0.0f, 1000.0f)),
        cameraNode_(cameraComponent_, cameraTransformationComponent_)
    {
    }

    void run()
    {
        Scene::Extensions::SimpleScene scene(cameraNode_);

        /*{
            Grapevine::Scene::Extensions::TransformationComponent transformationComponent(Vector3(200.0f, 400.0f, 0.0f));

            Grapevine::MeshAsset meshAsset(100.0f, 100.0f);
            Grapevine::TextureAsset texture("facebook_diffuse.png");
            Grapevine::MaterialAsset materialAsset(texture, 0.5f, 0.5f);
            Grapevine::Scene::Extensions::ModelComponent modelComponent(meshAsset, materialAsset);

            Scene::Node tmpNode(
                transformationComponent,
                modelComponent
            );
            scene.addNode(tmpNode);
        }

        Grapevine::Log::I("loading model(s) ...");

        Grapevine::ModelResource tmp("facebook.barrel");

        Scene::Node tmpNode(
            Scene::Extensions::TransformationComponent(Vector3(200.0f, 400.0f, 0.0f)),
            Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
        );
        scene.addNode(tmpNode);
        */

        canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
        bool quitApplication = false;
        while (quitApplication == false)
        {
            input(quitApplication);
            scene.draw(canvas_);
        }
    }

    void input(bool& quitApplication)
    {
        InputEvent event;

        while (inputSource_.pollEvent(event))
        {
            if (event.type == InputEventType::System)
            {
                quitApplication = true;
            }
        }
    }

private:
    Canvas canvas_;
    InputSource inputSource_;
    ResourceManager resourceManager_;
    Scene::Extensions::TransformationComponent cameraTransformationComponent_;
    Scene::Extensions::CameraComponent cameraComponent_;
    Scene::Node cameraNode_;
};

// // // // // //       M A I N       // // // // // //

int main(int argc, char *argv[])
{
    Grapevine::Log::I("hello-scene");

    Main* tmp = new Main();
    tmp->run();
    delete tmp;

    return 0;
}
