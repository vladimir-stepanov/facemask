//!

#include "TestView1.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>
#include <stdlib.h> /* atoi */

using namespace Grapevine;

int main(int argc, char *argv[])
{
    unsigned int x = 1280;
    unsigned int y = 720;

    if (argc == 3)
    {
        x = atoi(argv[1]);
        y = atoi(argv[2]);
    }

    try
    {
        ResourceManager resourceManager("../../examples/common/");
        TestView1 testView1(x, y);
        while (testView1.step())
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

