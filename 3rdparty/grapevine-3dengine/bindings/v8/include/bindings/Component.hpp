#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__

#include <Grapevine/Scene/Component.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Component : public ExportedClass<Component, void, Grapevine::Scene::Component>
        {
        public:
            Component();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate);
            Grapevine::Scene::Component *construct(const v8::FunctionCallbackInfo<v8::Value>& args);

        };
    }
}

#endif