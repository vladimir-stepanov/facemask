// This file is part of Grapevine project. See README file for more information.

#ifndef GRAPEVINE_TESTVIEW1THREAD
#define GRAPEVINE_TESTVIEW1THREAD

#include <pthread.h>
#include <string>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/SceneGraph/Node.hpp>
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Random.hpp>

class TestView1Thread: public Grapevine::Thread
{
public:
    TestView1Thread(Grapevine::Scene* scene):
        Grapevine::Thread(),
        scene_(scene),
        node_(Grapevine::Vector3(0.0f, 0.0f, -50.0f), Grapevine::MeshAsset("stone.3ds"), Grapevine::TextureAsset("grapevine.png")),
        end_(false),
        startTime_(Grapevine::Timer::milliseconds())
    {
    }

    ~TestView1Thread()
    {
    }

    bool isAtEnd()
    {
        return end_;
    };

protected:
    int run()
    {
        unsigned int cyclesRun = 0;
        for (unsigned int i = 0; i < 1000; i++)
        {
            scene_->addNode(node_);
            Grapevine::Timer::sleep(Grapevine::Random::integerWithMod(5));
            scene_->removeNode(node_);
            cyclesRun++;
        }

        LOGD("cyclesRun:%d, in time:%d", cyclesRun, Grapevine::Timer::milliseconds() - startTime_);
        end_ = true;
    }

private:
    Grapevine::Scene* scene_;
    Grapevine::Node node_;
    bool end_;
    unsigned int startTime_;
};

#endif
