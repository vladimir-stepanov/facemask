#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Node : public ExportedClass<Node, void, Grapevine::Scene::Node>
        {
            public:
            Node();
            void initializeClass(v8::Local<v8::FunctionTemplate> nodeTemplate);
            Grapevine::Scene::Node *construct(const v8::FunctionCallbackInfo<v8::Value>& args);
            static void addChild(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args);
            static void removeChild(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args);
            static void addComponent(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args);
            static void removeComponent(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args);
        };

    }
}

#endif