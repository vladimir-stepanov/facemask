//!

#ifndef PHYSICS_MAINVIEW
#define PHYSICS_MAINVIEW

#include <Grapevine/SceneGraph/PhysicsNode.hpp>
#include <Grapevine/SceneGraph/PhysicsPlane.hpp>
#include <Grapevine/SceneGraph/PhysicsTextNode.hpp>
#include <Grapevine/SceneGraph/TextNode.hpp>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Math/Plane.hpp>
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
    void input(bool& quitApplication);
    void draw();
    void initScene();
    Grapevine::Mutex mutex_;
    Grapevine::Camera camera_;
    Grapevine::Canvas canvas_;
    Grapevine::InputSource inputSource_;
    Grapevine::FpsCounter fpsCounter_;
    Grapevine::Scene scene_;
    Grapevine::Node background_;
    Grapevine::Node ballGlow_;


    Grapevine::PhysicsNode fakeClock0_;
    Grapevine::PhysicsNode fakeClock1_;
    Grapevine::PhysicsNode fakeDate0_;
    Grapevine::PhysicsNode fakeDate1_;
    Grapevine::PhysicsNode fakeDate2_;
    //Grapevine::PhysicsTextNode clock0_;
    //Grapevine::PhysicsTextNode clock1_;
    //Grapevine::PhysicsTextNode clock2_;
    //Grapevine::PhysicsTextNode dateWeekday_;
    //Grapevine::PhysicsTextNode dateDayNumber_;
    //Grapevine::PhysicsTextNode dateMonth_;
    Grapevine::PhysicsTextNode operator_;
    Grapevine::PhysicsNode button_;
    Grapevine::PhysicsNode swipeToCamera_;
    Grapevine::PhysicsNode swipeTotargetLock_;
    Grapevine::Plane plane_;
    Grapevine::PhysicsNode ball_;
    Grapevine::PhysicsPlane goundPlane_;
    Grapevine::PhysicsPlane ceilingPlane_;
    Grapevine::PhysicsPlane topPlane_;
    Grapevine::PhysicsPlane bottomPlane_;
    Grapevine::PhysicsPlane leftPlane_;
    Grapevine::PhysicsPlane rightPlane_;
    //Grapevine::PhysicsPlane leftPlane_;
    //Grapevine::PhysicsPlane rightPlane_;
    std::vector< Grapevine::TextNode > dragToUnlock_;
    unsigned int width_;
    unsigned int height_;
    unsigned int lastStep_;
    bool physicsEnabled_;
    bool physicsDebugEnabled_;
    bool normalsDebugEnabled_;
    bool impulse_;
    Grapevine::Vector3 impulseVector_;
    bool pressed_;
    Grapevine::Vector3 currentMove_;
};

#endif
