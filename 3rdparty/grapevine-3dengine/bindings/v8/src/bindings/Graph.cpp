#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Graph.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        Graph::Graph() : ExportedClass("grapevine", "Graph") {}

        void Graph::initializeClass(v8::Local<v8::FunctionTemplate> simpleSceneTemplate)
        {
            registerMethod<Graph::addNode>("addNode");
            // registerMethod<Graph::location>("setLocation");
        }

        Grapevine::Scene::Graph *Graph::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            return new Grapevine::Scene::Graph();
        }

        void Graph::addNode(Grapevine::Scene::Graph *graph, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 1)
            {
                Grapevine::Scene::Node *node = cast<Node>(args[0]);
                if(node)
                {
                    graph->addNode(*node);
                    return;
                }
            }

            LOGE("Graph::addNode needs exactly one argument, Node");
            return;
        }

        static Graph graph_;

    }
}
