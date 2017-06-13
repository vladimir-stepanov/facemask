//!

#ifndef GRAPEVINE_HELLO4_MAINVIEW
#define GRAPEVINE_HELLO4_MAINVIEW

#include <Grapevine/SceneGraph/Node.hpp>
#include <Grapevine/SceneGraph/TextNode.hpp>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Core/Mutex.hpp>
#include <Grapevine/Core/Lock.hpp>

class MainView
{
public:
    MainView(unsigned int width, unsigned int height);
    ~MainView();
    bool step();
    void surfaceChanged(unsigned int width, unsigned int height);

    Grapevine::InputSource* inputSource();

private:
    Grapevine::Mutex mutex_;

    void input(bool& quitApplication);
    void physics();
    void draw();

    Grapevine::Camera camera_;
    Grapevine::Canvas canvas_;
    Grapevine::Scene scene_;
    Grapevine::InputSource inputSource_;
    Grapevine::FpsCounter fpsCounter_;
    unsigned int width_;
    unsigned int height_;

    Grapevine::TextNode textIconEvo_;
    Grapevine::Node iconEvo0_;
    Grapevine::Node iconEvo1_;
    Grapevine::Node iconEvo2_;
    Grapevine::Node iconEvo3_;

    Grapevine::TextNode textFinland3DEngine_;
    Grapevine::TextNode textIcons_;
    Grapevine::TextNode textMaterials_;
    Grapevine::Node stoneNode2_;
    Grapevine::Node stoneNode3_;
    Grapevine::Node stoneNode4_;
    Grapevine::Node stoneNode5_;
    Grapevine::TextNode textNode2_;
    Grapevine::TextNode textNode3_;
    Grapevine::TextNode textNode4_;
    Grapevine::TextNode textNode5_;

    std::vector<Grapevine::Node> icons_;

    bool pressed_;
    Grapevine::Plane zeroPlane_;
    Grapevine::Vector3 currentMove_;
    Grapevine::Vector3 lastDiff_;
    Grapevine::Vector3 moveUnderPress_;

    Grapevine::Vector3 keyboardMove_;
    Grapevine::Matrix4 keyboardRotation_;

    bool debugNormals_;
};

#endif
