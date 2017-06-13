//!

#include "MainView.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <stdlib.h> /* atoi */
#include <Grapevine/Math/Vector.hpp>

using namespace Grapevine;

void testing();

int main(int argc, char *argv[])
{
    unsigned int x = 4 * 720 / 5;
    unsigned int y = 4 * 1280 / 5;

    //testing();

    if (argc == 3)
    {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
    }

    try
    {
        ResourceManager resourceManager("android/unlock/assets/");
        MainView mainView(x, y);
        while (mainView.step())
        {
        }
    }
    catch (Exception e)
    {
        Log::E("Exception: %s", e.what());
    }
    catch (...)
    {
        Log::E("Exception: unknown exception!");
    }

    return 0;
}

void testing()
{
    // Vector testing
    Vector3 tmp(1.0f, 2.0f, 3.0f);
    Vector3 tmp2 = 5.0f * tmp;
    LOGD("%f, %f, %f", tmp2.x(), tmp2.y(), tmp2.z());
    LOGD("%f, %f, %f", tmp.x(), tmp.y(), tmp.z());
    Vector3 tmp3 = tmp * 5.0f;
    LOGD("%f, %f, %f", tmp3.x(), tmp3.y(), tmp3.z());
    LOGD("%f, %f, %f", tmp.x(), tmp.y(), tmp.z());
}

