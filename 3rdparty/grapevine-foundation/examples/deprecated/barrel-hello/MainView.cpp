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
#include <Grapevine/Text/FontManager.hpp>

using namespace Grapevine;

MainView::MainView(unsigned int width, unsigned int height):
    canvas_(width, height),
    camera_(canvas_, Vector3(0.0f, 0.0f, 300.0f), 45.0f),
    scene_(camera_),
    width_(width),
    height_(height),
    inputSource_(canvas_),
    zeroPlane_(Vector::origo, Vector::zAxis),
    lastDiff_(Vector3(0.0f, 0.0f, 0.0f)),
    textFinland3DEngine_(Vector3(-45.0f, 0.0f, -100.0f), RichText(Text("FINLAND 3D ENGINE", FontAsset("Ubuntu-Light.ttf"), 70, Vector4(1, 1, 1, 1)))),
    textIcons_(Vector3(), RichText(Text("3D ICONS", FontAsset("Ubuntu-Light.ttf"), 20, Vector4(1, 1, 1, 1)))),
    textIconEvo_(Vector3(-70.0f, -50.0f, 0.0f), RichText(Text("ICON EVOLUTION", FontAsset("Ubuntu-Light.ttf"), 20, Vector4(1, 1, 1, 1)))),
    iconEvo0_(Vector3(0.0f, -100.0f, 0.0f), MeshAsset("2D_icon_slate.barrel"), MaterialAsset("androidcamera.png", 1.0f, 100.0f)),
    iconEvo1_(Vector3(0.0f, -150.0f, 0.0f), ModelAsset("showboat_white.barrel")),
    iconEvo2_(Vector3(0.0f, -200.0f, 0.0f), ModelAsset("showboat_textured.barrel")),
    iconEvo3_(Vector3(0.0f, -250.0f, 0.0f), ModelAsset("showboat_textured_with_reflection.barrel")),
    textMaterials_(Vector3(), RichText(Text("MATERIALS & SHADERS", FontAsset("Ubuntu-Light.ttf"), 20, Vector4(1, 1, 1, 1)))),
    stoneNode2_(Node(Vector3(0.0f, -400.0f, 0.0f), MeshAsset("stone.3ds"), MaterialAsset("grapevine.png", 1.0f, 100.0f))),
    stoneNode3_(Node(Vector3(0.0f, -500.0f, 0.0f), MeshAsset("stone.3ds"), MaterialAsset("grapevine.png", "tiles_normalmap.png", 1.0f, 100.0f))),
    stoneNode4_(Node(Vector3(0.0f, -600.0f, 0.0f), MeshAsset("stone.3ds"), MaterialAsset("grapevine.png", 0.5f, 1.0f, 100.0f))),
    stoneNode5_(Node(Vector3(0.0f, -700.0f, 0.0f), MeshAsset("stone.3ds"), MaterialAsset("grapevine.png", "tiles_normalmap.png", 0.5f, 1.0f, 100.0f))),
    textNode2_(Vector3(-2.8f, -7.0f, 0.0f), RichText(Text("phong", FontAsset("Ubuntu-Light.ttf"), 2.5, Vector4(1, 1, 1, 1)))),
    textNode3_(Vector3(-5.5f, -7.0f, 0.0f), RichText(Text("phong_bump", FontAsset("Ubuntu-Light.ttf"), 2.5, Vector4(1, 1, 1, 1)))),
    textNode4_(Vector3(-6.0f, -7.0f, 0.0f), RichText(Text("phong_reflect", FontAsset("Ubuntu-Light.ttf"), 2.5, Vector4(1, 1, 1, 1)))),
    textNode5_(Vector3(-8.0f, -7.0f, 0.0f), RichText(Text("phong_bump_reflect", FontAsset("Ubuntu-Light.ttf"), 2.5, Vector4(1, 1, 1, 1)))),
    pressed_(false),
    debugNormals_(false)
{
    canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    LOGD("Creating scene...");

    textFinland3DEngine_.setRotation(Vector::zAxis, 90.0f);
    scene_.addNode(textFinland3DEngine_);

    // Splash screen
    {
        std::vector<std::string> objects;

        objects.push_back("Youtube");

        float y = 50.0f;
        unsigned int xStep = 0;
        for(std::vector<std::string>::iterator i = objects.begin(); i != objects.end(); ++i)
        {
            std::string name = *i;
            std::string barrel = name + ".barrel";
            Node node(Vector3(0.0f + xStep * 50.0f, y, 0.0f), ModelAsset(barrel));
            node.setScale(0.2f);
            icons_.push_back(node);
            scene_.addNode(node);
            xStep++;
            if (xStep >= 2)
            {
                xStep = 0;
                y += 50.0f;
            }
        }
        y += 50.0f;
        textIcons_.setLocation(Vector3(-70.0f, y, 0.0f));
        textIcons_.setRotation(Vector::yAxis, 30.0f);
        scene_.addNode(textIcons_);
    }

    textIconEvo_.setRotation(Vector::yAxis, 30.0f);
    scene_.addNode(textIconEvo_);
    iconEvo0_.setScale(0.2f);
    iconEvo1_.setScale(0.2f);
    iconEvo2_.setScale(0.2f);
    iconEvo3_.setScale(0.2f);
    scene_.addNode(iconEvo0_);
    scene_.addNode(iconEvo1_);
    scene_.addNode(iconEvo2_);
    scene_.addNode(iconEvo3_);

    textMaterials_.setLocation(Vector3(-70.0f, -350.0f, 0.0f));
    textMaterials_.setRotation(Vector::yAxis, 30.0f);
    scene_.addNode(textMaterials_);

    // Material stones
    // node 2
    stoneNode2_.setScale(6.0f);
    scene_.addNode(stoneNode2_);
    stoneNode2_.addChild(textNode2_);
    icons_.push_back(stoneNode2_);

    // node 3
    stoneNode3_.setScale(6.0f);
    scene_.addNode(stoneNode3_);
    stoneNode3_.addChild(textNode3_);
    icons_.push_back(stoneNode3_);

    // node 4
    stoneNode4_.setScale(6.0f);
    scene_.addNode(stoneNode4_);
    stoneNode4_.addChild(textNode4_);
    icons_.push_back(stoneNode4_);

    // node 5
    stoneNode5_.setScale(6.0f);
    scene_.addNode(stoneNode5_);
    stoneNode5_.addChild(textNode5_);
    icons_.push_back(stoneNode5_);

    LOGD("Fonts...");
    std::vector<std::string> fontsAvailable = FontManager::fontFiles();
    for(std::vector<std::string>::iterator i = fontsAvailable.begin(); i != fontsAvailable.end(); ++i)
    {
        /* std::cout << *it; ... */
        LOGD("font found: %s", (*i).c_str());
    }

    LOGD("MainView constructor end");
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

void MainView::surfaceChanged(unsigned int width, unsigned int height)
{
    LOGI("MainView::surfaceChanged(), width:%d, height:%d", width, height);
    canvas_.resize(width, height);
}

void MainView::input(bool& quitApplication)
{
    LOCK

    InputEvent event;

    keyboardRotation_ = Matrix4();

    while (inputSource_.pollEvent(event))
    {
        Ray rayLast = camera_.mouseCoordinatesToRay(event.touch.lastX, event.touch.lastY);
        Ray rayCurrent = camera_.mouseCoordinatesToRay(event.touch.x, event.touch.y);
        float xDiff = event.touch.x - event.touch.lastX;
        float yDiff = event.touch.y - event.touch.lastY;
        RayPlaneIntersection intersectionLast = rayLast.intersection(zeroPlane_);
        RayPlaneIntersection intersectionCurrent = rayCurrent.intersection(zeroPlane_);

        if (event.type == InputEventType::Touch && (event.touch.flags & TouchFlags::Pressed))
        {
            pressed_ = true;
        }
        else if (event.type == InputEventType::Touch && (event.touch.flags & TouchFlags::Released))
        {
            pressed_ = false;
        }

        // Calculate keyboard move
        if (event.type == InputEventType::Keyboard && event.keyboard.pressed == true)
        {
            switch (event.keyboard.symbol)
            {
                case InputKey::Letter_w:
                {
                    keyboardMove_ = camera_.lookAt().unit();
                    break;
                }
                case InputKey::Letter_s:
                {
                    keyboardMove_ = camera_.lookAt().unit() * -1.0f;
                    break;
                }
                case InputKey::Letter_a:
                {
                    keyboardMove_ = camera_.right() * -1.0f;
                    break;
                }
                case InputKey::Letter_d:
                {
                    keyboardMove_ = camera_.right();
                    break;
                }
                case InputKey::Letter_n:
                {
                    debugNormals_ = !debugNormals_;
                    break;
                }
                case InputKey::ArrowUp:
                {
                    keyboardMove_ = camera_.up() * 1.0f;
                    break;
                }
                case InputKey::ArrowDown:
                {
                    keyboardMove_ = camera_.up() * -1.0f;
                    break;
                }
                case InputKey::ArrowLeft:
                {
                    Vector3 tmpRight = camera_.right();
                    Vector3 tmpUp = camera_.up();
                    LOGD("right (%f, %f, %f)", tmpRight.x(), tmpRight.y(), tmpRight.z());
                    LOGD("up (%f, %f, %f)", tmpUp.x(), tmpUp.y(), tmpUp.z());

                    keyboardRotation_ = Matrix::rotate(-10.0f, tmpUp);
                    break;
                }
                case InputKey::ArrowRight:
                {
                    Vector3 tmpRight = camera_.right();
                    Vector3 tmpUp = camera_.up();
                    LOGD("right (%f, %f, %f)", tmpRight.x(), tmpRight.y(), tmpRight.z());
                    LOGD("up (%f, %f, %f)", tmpUp.x(), tmpUp.y(), tmpUp.z());

                    keyboardRotation_ = Matrix::rotate(10.0f, tmpUp);
                    break;
                }
                default:
                {
                }
            }
        }

        if (event.type == InputEventType::Touch && (event.touch.flags & TouchFlags::Moved))
        {
            if (intersectionLast.type() == RayPlaneIntersection::Type::PointIntersection &&
                intersectionCurrent.type() == RayPlaneIntersection::Type::PointIntersection)
            {
                currentMove_ = intersectionLast.p() - intersectionCurrent.p();
            }
            else
            {
                Log::E("MainView::input() Where is your camera pointing to?");
                currentMove_ = Vector::origo;
            }
        }
        else if (event.type == InputEventType::System)
        {
            Log::I("event.type == InputEventType::System");
            quitApplication = true;
        }
    }
}

InputSource* MainView::inputSource()
{
    return &inputSource_;
}

void MainView::physics()
{
    static unsigned int startTime = Timer::milliseconds();
    //static float rotation = 0;

    unsigned int timeDiff = Timer::milliseconds() - startTime;
    float rotation = (float)timeDiff / 30.0f;

    for(std::vector<Grapevine::Node>::iterator i = icons_.begin(); i != icons_.end(); ++i)
    {
        Grapevine::Node tmp = *i;
        tmp.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);
    }

    iconEvo0_.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);
    iconEvo1_.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);
    iconEvo2_.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);
    iconEvo3_.setRotation(Vector3(0.0f, 1.0f, 0.0f), rotation);

    // Flick effect
    currentMove_.x(0.0f);
    currentMove_.y(0.95f * currentMove_.y());

    // "Action"
    static Vector3 moveDiff = Vector3();

    if (pressed_ == true)
    {
        moveDiff = currentMove_;
    }
    else
    {
        // "flick inspired" -effect
        moveDiff.y(moveDiff.y() * 0.95f);
    }

    if (moveDiff.x() > 50.0f)
    {
        moveDiff.x(50.0f);
    }
    else if (moveDiff.x() < -50.0f)
    {
        moveDiff.x(-50.0f);
    }

    if (moveDiff.y() > 50.0f)
    {
        moveDiff.y(50.0f);
    }
    else if (moveDiff.y() < -50.0f)
    {
        moveDiff.y(-50.0f);
    }

    if (fabs(moveDiff.y()) > 0.001f || fabs(moveDiff.x()) > 0.001f)
    {
        Vector3 tmp = camera_.location();
        tmp += moveDiff;
        camera_.setLocation(tmp);
    }

    // Keyboard
    Vector3 tmp = camera_.location();
    tmp += keyboardMove_;
    camera_.setLocation(tmp);
    camera_.rotate(keyboardRotation_);
    keyboardMove_ *= 0.97;
}

void MainView::draw()
{
    fpsCounter_.startStep();
    canvas_.clear();
    scene_.draw(debugNormals_);
    canvas_.update();
    fpsCounter_.endStep();

    return;
}
