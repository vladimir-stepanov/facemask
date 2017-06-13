//!

#include "MainView.hpp"
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/SceneGraph/Camera.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Random.hpp>
#include <Grapevine/Math/EasingCurve.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Material/MaterialAsset.hpp>
#include <Grapevine/Math/Ray.hpp>

using namespace Grapevine;

MainView::MainView(unsigned int width, unsigned int height):
    canvas_(width, height),
    camera_(canvas_, Vector3(0.0f, 0.0f, 45.0f), 45.0f),
    scene_(camera_),
    width_(width),
    height_(height),
    inputSource_(InputSource(canvas_)),
    node1_(Node(Vector3(-25.0f, 10.0f, 0.0f), ModelAsset("Youtube.barrel"))),
    node2_(Node(Vector3(-25.0f, 0.0f, 0.0f), ModelAsset("Youtube.barrel"))),
    node3_(Node(Vector3(-25.0f, -10.0f, 0.0f), MeshAsset("Youtube.barrel"))),
    node4_(Node(Vector3(5.0f, 10.0f, 0.0f), MeshAsset("Youtube.barrel"))),
    rotating1_(Vector3(100.0f, 0.0f, 0.0f), MeshAsset("Youtube.barrel")),
    rotating2_(Vector3(0.0f, 0.0f, 100.0f), MeshAsset("Youtube.barrel")),
    node5_(Node(Vector3(5.0f, 0.0f, 0.0f), MeshAsset("Youtube.barrel"))),
    node6_(Node(Vector3(5.0f, -10.0f, 0.0f), MeshAsset("normalmap_box.barrel"), MaterialAsset("red256x256.png", 0.0f, 0.0f))),
    textNode1_(Vector3(-20.0f, 10.0f, 0.0f), RichText(Text("1 material", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1)))),
    textNode2_(Vector3(-20.0f, 0.0f, 0.0f), RichText(Text("2 materials", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1)))),
    textNode3_(Vector3(-20.0f, -10.0f, 0.0f), RichText(Text("glossiness 1", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1)))),
    textNode4_(Vector3(10.0f, 10.0f, 0.0f), RichText(Text("glossiness 10", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1)))),
    textNode5_(Vector3(10.0f, 0.0f, 0.0f), RichText(Text("glossiness 100", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1)))),
    textNode6_(Vector3(10.0f, -10.0f, 0.0f), RichText(Text("normal map", FontAsset("Ubuntu-Light.ttf"), 2, Vector4(1, 1, 1, 1))))
{
    canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    // Splash screen
    {
        // node 1
        node1_.setScale(0.03f);
        scene_.addNode(node1_);
        scene_.addNode(textNode1_);

        // node 2
        node2_.setScale(0.03f);
        scene_.addNode(node2_);
        scene_.addNode(textNode2_);

        // node 3
        node3_.setScale(0.03f);
        scene_.addNode(node3_);
        scene_.addNode(textNode3_);

        // node 4
        node4_.setScale(0.03f);
        scene_.addNode(node4_);
        scene_.addNode(textNode4_);

        rotating1_.setScale(0.5f);
        node4_.addChild(rotating1_);

        rotating2_.setScale(0.5f);
        rotating1_.addChild(rotating2_);

        // node 5
        node5_.setScale(0.03f);
        scene_.addNode(node5_);
        scene_.addNode(textNode5_);

        // node 6
        node6_.setScale(0.03f);
        scene_.addNode(node6_);
        scene_.addNode(textNode6_);
    }
}

MainView::~MainView()
{
}

bool MainView::step()
{
    bool quitApplication = false;
    input(quitApplication);
    physics();
    draw();
    return !quitApplication;
}

void MainView::input(bool& quitApplication)
{
    InputEvent event;

    while (inputSource_.pollEvent(event))
    {
        if (event.type == InputEventType::Touch)
        {
            if (event.touch.flags & TouchFlags::Pressed)
            {
                // TODO
                //LOGD("Press");
                Ray ray = camera_.mouseCoordinatesToRay(event.touch.x, event.touch.y);
                //LOGD("origin:%f, %f, %f", ray.origin().x(), ray.origin().y(), ray.origin().z());
                //LOGD("unit:%f, %f, %f", ray.unit().x(), ray.unit().y(), ray.unit().z());

                std::list<Intersection> intersections;
                scene_.intersections(ray, intersections);

                for (std::list<Intersection>::iterator i = intersections.begin(); i != intersections.end(); i++)
                {
                    LOGD("Intersection (%f, %f)", (*i).rectangleCoordinate().x(), (*i).rectangleCoordinate().y());
                }
            }
        }
        else if (event.type == InputEventType::System)
        {
            LOGD("event.type == InputEventType::System");
            quitApplication = true;
        }
    }
}

void MainView::physics()
{
    static unsigned int lastTime = Timer::milliseconds();
    unsigned int currentTime = Timer::milliseconds();
    unsigned int timeDiff = currentTime - lastTime;
    lastTime = currentTime;
    float rotationAngle = (float)timeDiff / 100.0f;

    Matrix4 rotation = Matrix::rotate(rotationAngle, Vector::yAxis);
    node1_.rotate(rotation);
    node2_.rotate(rotation);
    node3_.rotate(rotation);
    node4_.rotate(rotation);
    node5_.rotate(rotation);
    node6_.rotate(rotation);
    rotating1_.rotate(rotation);
    rotating2_.rotate(rotation);
}

void MainView::draw()
{
    fpsCounter_.startStep();

    canvas_.clear();
    scene_.draw();
    canvas_.update();

    fpsCounter_.endStep();

    return;
}
