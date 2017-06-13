//!

#ifndef GRAPEVINE_TESTVIEW1
#define GRAPEVINE_TESTVIEW1

#include <Grapevine/Shader/ShaderManager.hpp>
#include <Grapevine/Shader/ShaderProgram.hpp>
#include <Grapevine/SceneGraph/Node.hpp>
#include <Grapevine/SceneGraph/TextNode.hpp>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Core/Thread.hpp>
#include "TestView1Thread.hpp"

class TestView1
{
public:
    static unsigned int const AmountOfThreads = 10;

    TestView1(unsigned int width, unsigned int height);
    ~TestView1();
    bool step();

private:
    void input(bool& quitApplication);
    void physics();
    void draw();

    unsigned int width_;
    unsigned int height_;

    Grapevine::Camera camera_;
    Grapevine::Canvas canvas_;
    Grapevine::Scene scene_;
    Grapevine::InputSource inputSource_;
    Grapevine::FpsCounter fpsCounter_;
    Grapevine::ShaderManager shaderManager_;
    Grapevine::ShaderProgram shaderProgram_;
    TestView1Thread* threads_[AmountOfThreads];
};

#endif
