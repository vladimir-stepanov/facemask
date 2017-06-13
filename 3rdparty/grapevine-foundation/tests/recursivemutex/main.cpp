//!

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/Scene/AbstractVisitor.hpp>

class TestVisitor;
class TestThread;

class TestVisitor: public Grapevine::Scene::AbstractVisitor
{
public:
    // Inherited
    void visitGraphEnter()
    {
    }

    void visitGraphLeave()
    {
    }

    void visitNodeEnter(Grapevine::Scene::NodePrivate* nodePrivate)
    {
        TestVisitor testRecursionVisitor;
        nodePrivate->accept(testRecursionVisitor);
    }

    void visitNodeLeave(Grapevine::Scene::NodePrivate* nodePrivate)
    {
    }
};


class TestThread: public Grapevine::Thread
{
public:

};

// // // // // //       M A I N       // // // // // //

int main(int argc, char *argv[])
{
    LOGD("hello!");

    Grapevine::Scene::Node node1;
    Grapevine::Scene::Node node2;
    Grapevine::Scene::Node node3;
    node1.addChild(node2);
    node2.addChild(node3);

    Grapevine::Scene::Graph graph;
    graph.addNode(node1);

    TestVisitor testRecursionVisitor;
    graph.accept(testRecursionVisitor);

    return 0;
}
