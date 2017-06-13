#include <Grapevine/Scene/Component.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        Component::Component() : ExportedClass("grapevine", "Component") {}
        void Component::initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate) {}
        Grapevine::Scene::Component *Component::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            return NULL; //no need to be able to instantiate bare components
        }

        Component Component_;
    }
}
