#include <Grapevine/SceneExtensions/SimpleScene.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Node.hpp"
#include "bindings/Canvas.hpp"
#include "bindings/Graph.hpp"
#include "bindings/InputComponentEvent.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class SimpleScene : public ExportedClass<SimpleScene, Graph, Grapevine::Scene::Extensions::SimpleScene>
        {
            public:
            SimpleScene() : ExportedClass("grapevine", "SimpleScene") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> simpleSceneTemplate)
            {
                registerMethod<SimpleScene::draw>("draw");
                registerMethod<SimpleScene::add>("add");
            }

            Grapevine::Scene::Extensions::SimpleScene *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 1)
                {
                    Grapevine::Scene::Node *cameraNode = cast<Node>(args[0]);
                    if(cameraNode)
                    {
                        return new Grapevine::Scene::Extensions::SimpleScene(*cameraNode);
                    }
                }
                LOGE("SimpleScene needs Camera Node as its sole parameter to constructor");
                return NULL;
            }

            static void draw(Grapevine::Scene::Extensions::SimpleScene *scene, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 1)
                {
                    Grapevine::Canvas *canvas = cast<Canvas>(args[0]);
                    if(canvas)
                    {
                        bool dirty = scene->draw(*canvas);
                        args.GetReturnValue().Set(v8::Boolean::New(v8::Isolate::GetCurrent(), dirty));
                        return;
                    }
                }


                LOGE("SimpleScene::draw needs exactly one argument, Canvas");
                return;
            }

            static void add(Grapevine::Scene::Extensions::SimpleScene *scene, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 1)
                {
                    Scene::Extensions::InputComponent::Event *event = cast<InputComponentEvent>(args[0]);
                    if(event)
                    {
                        scene->add(*event);
                        return;
                    }
                }

                LOGE("SimpleScene::add needs exactly one argument, InputComponent::Event");
                return;
            }

        };

        static SimpleScene simpleScene_;

    }
}
