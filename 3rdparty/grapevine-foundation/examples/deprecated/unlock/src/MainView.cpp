//! This file is just to smoke test 3d engine and physics integration

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
#include <Grapevine/SceneGraph/PhysicsNode.hpp>
#include <Grapevine/SceneGraph/PhysicsTextNode.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <sstream>

using namespace Grapevine;

#define FONTZ 30.0f
#define FONTTHICKNESS 5.0f

MainView::MainView(unsigned int width, unsigned int height):
    canvas_(width, height),
//    camera_(canvas_, Vector3(0.0f, -900.0f, 2000.0f), 45.0f, 1.0f, 15000.0f),
    camera_(canvas_, Vector3(0.0f, 0.0f, 2300.0f), 45.0f, 1.0f, 15000.0f),
    scene_(camera_, Vector3(0.0f, 0.0f, -50)),
    background_(Vector3(), ModelAsset("unlock_background.barrel")),
    ballGlow_(Vector3(0.0f, -625.0f, 150.0f), ModelAsset("ball_glow.barrel")),
    fakeClock0_(Vector3(-200.0f, 500.0f, 150.0f), ModelAsset("unlock_numberbox_1.barrel"), PhysicsNode::Type::BoundingSphere, 5.0f),
    fakeClock1_(Vector3(200.0f, 500.0f, 150.0f), ModelAsset("unlock_numberbox_2.barrel"), PhysicsNode::Type::BoundingSphere, 5.0f),
    fakeDate0_(Vector3(-200.0f, 300.0f, 50.0f), ModelAsset("unlock_day_box.barrel"), PhysicsNode::Type::BoundingBox, 5.0f),
    fakeDate1_(Vector3(0.0f, 300.0f, 50.0f), ModelAsset("unlock_month_box.barrel"), PhysicsNode::Type::BoundingBox, 5.0f),
    fakeDate2_(Vector3(200.0f, 300.0f, 50.0f), ModelAsset("unlock_year_box.barrel"), PhysicsNode::Type::BoundingBox, 5.0f),
    //clock0_(Vector3(-330.0f, 500.0f, FONTZ), RichText(Text("12", FontAsset("font.ttf"), 300, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    //clock1_(Vector3(-30.0f, 500.0f, FONTZ), RichText(Text(":", FontAsset("font.ttf"), 300, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    //clock2_(Vector3(70.0f, 500.0f, FONTZ), RichText(Text("44", FontAsset("font.ttf"), 300, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    //dateWeekday_(Vector3(-315.0f, 400.0f, FONTZ), RichText(Text("Thu;", FontAsset("font.ttf"), 100, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    //dateDayNumber_(Vector3(-120.0f, 398.0f, FONTZ), RichText(Text("Sep 19;", FontAsset("font.ttf"), 100, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    //dateMonth_(Vector3(190.0f, 400.0f, FONTZ), RichText(Text("2013", FontAsset("font.ttf"), 100, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    operator_(Vector3(-80.0f, 300.0f, FONTZ), RichText(Text(" ", FontAsset("font.ttf"), 100, Vector4(1, 1, 1, 1))), 1.0f, FONTTHICKNESS),
    button_(Vector3(0.0f, 0.0f, 5.0f), ModelAsset("button.barrel"), PhysicsNode::Type::BoundingBox, 0.0f),
    swipeToCamera_(Vector3(-385.0f, -635.0f, 50.0f), ModelAsset("camera_icon.barrel"), PhysicsNode::Type::BoundingBox, 1.0f),
    swipeTotargetLock_(Vector3(385.0f, -635.0f, 50.0f), ModelAsset("unlock_icon.barrel"), PhysicsNode::Type::BoundingBox, 1.0f),
    plane_(Vector::origo, Vector::zAxis),
    ball_(Vector3(0.0f, -625.0f, 150.0f), ModelAsset("unlock_ball.barrel"), PhysicsNode::Type::BoundingSphere, 5.0f),
    goundPlane_(Vector3(0.0f, 0.0f, 0.0f), Vector::zAxis),
    ceilingPlane_(Vector3(0.0f, 0.0f, 400.0f), Vector::minusZAxis),
    topPlane_(Vector3(0.0f, 900.0f, 0.0f), Vector::minusYAxis),
    bottomPlane_(Vector3(0.0f, -900.0f, 0.0f), Vector::yAxis),
    leftPlane_(Vector3(-500.0f, 0.0f, 0.0f), Vector::xAxis),
    rightPlane_(Vector3(500.0f, 0.0f, 0.0f), Vector::minusXAxis),
    width_(width),
    height_(height),
    inputSource_(canvas_),
    fpsCounter_("FPS"),
    lastStep_(Timer::milliseconds()),
    physicsEnabled_(false),
    physicsDebugEnabled_(false),
    normalsDebugEnabled_(false),
    impulse_(false),
    impulseVector_(Vector3()),
    pressed_(false)
{
    canvas_.setClearColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    camera_.setRotation(Vector::xAxis, 0.0f);
    //camera_.setRotation(Vector::xAxis, -20.0f);
    initScene();
}

MainView::~MainView()
{
}

InputSource* MainView::inputSource()
{
    return &inputSource_;
}

bool MainView::step()
{
    fpsCounter_.startStep();

    static float rotation = 0.0f;
    static unsigned int lastTime = Timer::milliseconds();
    unsigned int currentTime = Timer::milliseconds();
    unsigned int timeDiff = currentTime - lastTime;
    lastTime = currentTime;
    if (physicsEnabled_ == false)
    {
        rotation += (float)timeDiff / 200.0f;
        ball_.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);
    }

    if (impulse_ == true)
    {
        ball_.applyImpulse(impulseVector_);
        impulse_ = false;
    }

    bool quitApplication = false;
    input(quitApplication);

    // Ball glow to follow the ball
    {
        Vector3 tmp = ball_.location();
        float tmpH = tmp.z();
        tmp.z(100.0f);

        float scale = 1.0f;
        if (tmpH > 1100.0f)
        {
            scale = 0.0;
        }
        else if (tmpH > 100.0f)
        {
            scale = 1.0f - ((tmpH - 100.0f) / 1000.0f);
        }
        ballGlow_.setLocation(tmp);
        ballGlow_.setScale(scale);
        ballGlow_.setOpacity(scale);
    }
    draw();

    fpsCounter_.endStep();

    lastStep_ = Timer::milliseconds();
    return !quitApplication;
}

void MainView::input(bool& quitApplication)
{
    InputEvent event;

    while (inputSource_.pollEvent(event))
    {
        // Calculate keyboard move
        if (event.type == InputEventType::Keyboard && event.keyboard.pressed == true)
        {
            switch (event.keyboard.symbol)
            {
                case InputKey::Letter_n:
                {
                    normalsDebugEnabled_ = !normalsDebugEnabled_;
                    break;
                }
                case InputKey::Letter_p:
                {
                    physicsEnabled_ = !physicsEnabled_;
                    break;
                }
                case InputKey::Letter_b:
                {
                    physicsDebugEnabled_ = !physicsDebugEnabled_;
                    break;
                }
                case InputKey::ArrowUp:
                {
                    impulse_ = true;
                    impulseVector_ = Vector3(0.0f, 10.0f, 0.0f);
                    break;
                }
                case InputKey::ArrowDown:
                {
                    impulse_ = true;
                    impulseVector_ = Vector3(0.0f, -10.0f, 0.0f);
                    break;
                }
                case InputKey::ArrowLeft:
                {
                    impulse_ = true;
                    impulseVector_ = Vector3(-10.0f, 0.0f, 0.0f);
                    break;
                }
                case InputKey::ArrowRight:
                {
                    impulse_ = true;
                    impulseVector_ = Vector3(10.0f, 0.0f, 0.0f);
                    break;
                }
            }
        }

        // Touch
        if (event.type == InputEventType::Touch)
        {
            physicsEnabled_ = true;

            Ray rayLast = camera_.mouseCoordinatesToRay(event.touch.lastX, event.touch.lastY);
            Ray rayCurrent = camera_.mouseCoordinatesToRay(event.touch.x, event.touch.y);
            float xDiff = event.touch.x - event.touch.lastX;
            float yDiff = event.touch.y - event.touch.lastY;
            RayPlaneIntersection intersectionLast = rayLast.intersection(plane_);
            RayPlaneIntersection intersectionCurrent = rayCurrent.intersection(plane_);

            if (event.touch.flags & TouchFlags::Pressed)
            {
                pressed_ = true;
            }
            if (event.touch.flags & TouchFlags::Released)
            {
                pressed_ = false;
                impulse_ = true;
                impulseVector_ = currentMove_* -50.0f;
                LOGD("impulseVector: (%f, %f, %f)", impulseVector_.x(), impulseVector_.y(), impulseVector_.z());
            }

            if (pressed_ == true)
            {
                if (intersectionLast.type() == RayPlaneIntersection::Type::PointIntersection &&
                    intersectionCurrent.type() == RayPlaneIntersection::Type::PointIntersection)
                {
                    Vector3 point = intersectionCurrent.p();
                    currentMove_ = intersectionLast.p() - intersectionCurrent.p();
                }
                else
                {
                    Log::E("MainView::input() Where is your camera pointing to?");
                    //currentMove_ = Vector::origo;
                }
            }
        }

        // System quit
        else if (event.type == InputEventType::System)
        {
            quitApplication = true;
        }
    }
}

void MainView::draw()
{
    canvas_.clear();
    scene_.draw(normalsDebugEnabled_, physicsDebugEnabled_, physicsEnabled_);
    canvas_.update();

    return;
}

void MainView::initScene()
{
    scene_.addNode(goundPlane_);
    scene_.addNode(topPlane_);
    scene_.addNode(bottomPlane_);
    scene_.addNode(ceilingPlane_);
    scene_.addNode(leftPlane_);
    scene_.addNode(rightPlane_);

    scene_.addNode(background_);
    scene_.addNode(ballGlow_);

    scene_.addNode(fakeDate0_);
    scene_.addNode(fakeDate1_);
    scene_.addNode(fakeDate2_);
    scene_.addNode(fakeClock0_);
    scene_.addNode(fakeClock1_);

    scene_.addNode(swipeToCamera_);
    scene_.addNode(swipeTotargetLock_);
    scene_.addNode(button_);
    scene_.addNode(ball_);

    // We place "drag to unlock" letter by letter
    std::string dragToUnlockText("Drag to unlock");
    float dragToUnlockTextPlacement[] = {
        -17.0f, // "d"
        50.0f,  // "r"
        85.0f,  // "a"
        135.0f, // "g"
        180.0f, // " "
        235.0f, // "t"
        275.0f, // "o"
        310.0f, // " "
        360.0f, // "u"
        412.0f, // "n"
        465.0f, // "l"
        490.0f, // "o"
        542.0f, // "c"
        590.0f  // "k"
    };

    for (unsigned int i = 0; i < dragToUnlockText.length(); i++)
    {
        std::string tmp = dragToUnlockText.substr(i, 1);
        TextNode letter(Vector3(-305.0f + dragToUnlockTextPlacement[i], -850.0f, 100.0f), RichText(Text(tmp.c_str(), FontAsset("font.ttf"), 100, Vector4(1, 1, 1, 0.8f))));
        scene_.addNode(letter);
        dragToUnlock_.push_back(letter);
    }
}
