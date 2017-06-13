//!

#ifndef GRAPEVINEHELLO_MAINVIEW
#define GRAPEVINEHELLO_MAINVIEW

#include <Grapevine/SceneGraph/Node.hpp>
#include <Grapevine/SceneGraph/TextNode.hpp>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>

class MainView
{
public:
    MainView(unsigned int width, unsigned int height);
    ~MainView();
    bool step();

private:
    void input(bool& quitApplication);
    void physics();
    void draw();

    Grapevine::Camera camera_;
    Grapevine::Canvas canvas_;
    Grapevine::Scene scene_;
    Grapevine::InputSource inputSource_;
    Grapevine::Node node1_;
    Grapevine::Node node2_;
    Grapevine::Node node3_;
    Grapevine::Node node4_;
    Grapevine::Node node5_;
    Grapevine::Node node6_;
    Grapevine::Node rotating1_;
    Grapevine::Node rotating2_;
    Grapevine::TextNode textNode1_;
    Grapevine::TextNode textNode2_;
    Grapevine::TextNode textNode3_;
    Grapevine::TextNode textNode4_;
    Grapevine::TextNode textNode5_;
    Grapevine::TextNode textNode6_;
    Grapevine::FpsCounter fpsCounter_;
    unsigned int width_;
    unsigned int height_;
};

#endif
