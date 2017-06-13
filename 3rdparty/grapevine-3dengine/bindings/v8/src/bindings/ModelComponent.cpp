#include <Grapevine/SceneExtensions/ModelComponent.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"
#include "bindings/ModelAsset.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class ModelComponent : public ExportedClass<ModelComponent, Component, Grapevine::Scene::Extensions::ModelComponent>
        {
            public:
            ModelComponent() : ExportedClass("grapevine", "ModelComponent") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> cameraComponentTemplate)
            {
            }

            Grapevine::Scene::Extensions::ModelComponent *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                Grapevine::ModelAsset *modelAsset = NULL;
                switch(args.Length())
                {
                    case 1: modelAsset = cast<ModelAsset>(args[0]);
                            if(modelAsset) break;
                    default:
                        LOGE("Invalid arguments to ModelComponent constructor!");
                        return NULL;
                }
                return new Grapevine::Scene::Extensions::ModelComponent(*modelAsset);
            }

        };

        static ModelComponent modelComponent_;

    }
}
