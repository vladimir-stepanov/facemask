//!

#include "TestView1.hpp"
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Shader/ShaderManager.hpp>
#include <Grapevine/Shader/VertexSource.hpp>
#include <Grapevine/Shader/FragmentSource.hpp>
#include <Grapevine/SceneGraph/Camera.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Random.hpp>
#include <Grapevine/Math/EasingCurve.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>

using namespace Grapevine;

TestView1::TestView1(unsigned int width, unsigned int height):
    canvas_(width, height),
    camera_(canvas_, 45.0f),
    scene_(),
    width_(width),
    height_(height),
    inputSource_(InputSource(canvas_)),
    shaderManager_(ShaderManager()),
    shaderProgram_(ShaderProgram())
{
    canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));

    // Load shader files
    VertexSource vertexSource("twosided.vertex");
    FragmentSource fragmentSource("twosided.fragment");

    // Add constants to shaders
    static const Vector4 lightPosition(0.0f, 0.0f, 20.0f, 0.5f);
    static const Vector4 materialDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
    vertexSource.addConst("LightSourcePosition", lightPosition);
    vertexSource.addConst("MaterialDiffuse", materialDiffuse);

    // Load shaders to program
    shaderProgram_.init();
    shaderProgram_.addShader(vertexSource);
    shaderProgram_.addShader(fragmentSource);
    shaderProgram_.build();

    ShaderManager::setDefaultShaderProgram(&shaderProgram_);
    scene_.setCamera(&camera_);

    // Splash screen
    Log::I("Creating scene...");
    {
        for (unsigned int i = 0; i < AmountOfThreads; i++)
        {
            threads_[i] = new TestView1Thread(&scene_);
            threads_[i]->start();
        }
    }
}

TestView1::~TestView1()
{
    for (unsigned int i = 0; i < AmountOfThreads; i++)
    {
        delete threads_[i];
    }
}

bool TestView1::step()
{
    bool quitApplication = false;
    input(quitApplication);
    physics();
    draw();

    unsigned int amountOfEnds = 0;
    for (unsigned int i = 0; i < AmountOfThreads; i++)
    {
        if (threads_[i]->isAtEnd() == true)
        {
            amountOfEnds++;
        }
    }

    if (amountOfEnds == AmountOfThreads)
    {
        quitApplication = true;
    }

    return !quitApplication;
}

void TestView1::input(bool& quitApplication)
{
    InputEvent event;

    while (inputSource_.pollEvent(event))
    {
        if (event.type == InputEventType::Touch)
        {
            if (event.touch.flags & TouchFlags::Pressed)
            {
                // TODO
            }
        }
        else if (event.type == InputEventType::System)
        {
            Log::I("event.type == InputEventType::System");
            quitApplication = true;
        }
    }
}

void TestView1::physics()
{
    static float rotation = 0.0f;
}

void TestView1::draw()
{
    fpsCounter_.drawStartTrigger();
    canvas_.clear();
    scene_.draw();
    canvas_.update();
    fpsCounter_.drawEndTrigger();

    return;
}
