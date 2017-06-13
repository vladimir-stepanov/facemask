#include <Grapevine/SceneExtensions/InputComponent.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class InputComponentEvent : public ExportedClass<InputComponentEvent, void, Grapevine::Scene::Extensions::InputComponent::Event>
        {
            public:
            InputComponentEvent();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputEventTemplate);
            Grapevine::Scene::Extensions::InputComponent::Event *construct(const v8::FunctionCallbackInfo<v8::Value>& args);
        };
    }
}
