#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Node.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Graph : public ExportedClass<Graph, void, Grapevine::Scene::Graph>
        {
            public:
            Graph();
            void initializeClass(v8::Local<v8::FunctionTemplate> simpleSceneTemplate);
            Grapevine::Scene::Graph *construct(const v8::FunctionCallbackInfo<v8::Value>& args);
            static void addNode(Grapevine::Scene::Graph *graph, const v8::FunctionCallbackInfo<v8::Value>& args);
        };
    }
}
