//!

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/Scene/AbstractListVisitor.hpp>
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/TransformationComponentPrivate.hpp>

using namespace Grapevine;

// // // // // //       T E S T   V I S I T O R       // // // // // //

class MyListVisitor: public Scene::AbstractListVisitor
{
public:
    CLASS_COPYABLE(MyListVisitor)

    MyListVisitor():
        Scene::AbstractListVisitor("MyVisitor")
    {
    }

    virtual ~MyListVisitor()
    {
    }

    virtual void visitNodeEnter(Scene::NodePrivate* node)
    {
        LOGD("node id:%d", node->id());
    }

    virtual void visitNodeLeave(Scene::NodePrivate* node)
    {
    }
};

// // // // // //       M A I N       // // // // // //

int main(int argc, char *argv[])
{
    LOG("hello-componentowner");

    LOGD("----------------------------------------------------------------------");

    MyListVisitor visitor;

    Scene::NodeList list1 = Scene::Node::componentOwners<Scene::Extensions::TransformationComponentPrivate>();
    list1.accept(visitor);

    Scene::Node node;
    Scene::Node node2;
    Scene::Node node3;

    node.addChild(node3);
    node.addComponent(Scene::Extensions::TransformationComponent(Vector3()));
    node3.addComponent(Scene::Extensions::TransformationComponent(Vector3()));
    node2.addComponent(Scene::Extensions::TransformationComponent(Vector3()));

    Scene::NodeList list2 = Scene::Node::componentOwners<Scene::Extensions::TransformationComponentPrivate>();
    list2.accept(visitor);


    LOGD("----------------------------------------------------------------------");

    return 0;
}
