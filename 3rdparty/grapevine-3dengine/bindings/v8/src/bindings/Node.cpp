#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Node.hpp"
#include "bindings/Component.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        Node::Node() : ExportedClass("grapevine", "Node") {}

        void Node::initializeClass(v8::Local<v8::FunctionTemplate> nodeTemplate)
        {
            registerMethod<Node::addChild>("addChild");
            registerMethod<Node::removeChild>("removeChild");

            registerMethod<Node::addComponent>("addComponent");
            registerMethod<Node::removeComponent>("removeComponent");
        }

        Grapevine::Scene::Node *Node::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            std::vector<Grapevine::Scene::Component *> components;
            if(args.Length() > 3)
            {
                LOGE("Maximum of 3 components can be specified via constructor! Use addComponent instead!");
                return NULL;
            }

            for(int i = 0; i < args.Length(); i++)
            {
                Grapevine::Scene::Component *component = cast<Component>(args[i]);
                if(!component)
                {
                    LOGE("Argument number %i was not a component!", i);
                    return NULL;
                }
                components.push_back(component);
            }

            switch(components.size())
            {
                case 3: return new Grapevine::Scene::Node(*components[0], *components[1], *components[2]);
                case 2: return new Grapevine::Scene::Node(*components[0], *components[1]);
                case 1: return new Grapevine::Scene::Node(*components[0]);
            }
            return new Grapevine::Scene::Node();
        }

        void Node::addChild(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 1)
            {
                v8::Handle<v8::Value> childValue = args[0];

                Grapevine::Scene::Node *child = cast<Node>(childValue);
                if(child)
                {
                    node->addChild(*child);
                    return;
                }
            }

            LOGE("Invalid parameter(s) or none passed to Node::addChild");
        }

        void Node::addComponent(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 1)
            {
                Grapevine::Scene::Component *component = cast<Component>(args[0]);
                if(component)
                {
                    node->addComponent(*component);
                    return;
                }
            }

            LOGE("Invalid parameter(s) or none passed to Node::addComponent");
        }

        void Node::removeComponent(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 1)
            {
                v8::String::Utf8Value componentName(args[0]);
                node->removeComponent(*componentName);
                return;
            }

            LOGE("Invalid parameter(s) or none passed to Node::removeComponent");
        }

        void Node::removeChild(Grapevine::Scene::Node *node, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 1)
            {
                Grapevine::Scene::Node *child = cast<Node>(args[0]);
                if(child)
                {
                    node->removeChild(*child);
                    return;
                }

            }

            LOGE("Invalid parameter(s) or none passed to Node::removeComponent");
        }

        static Node node_;

    }
}
