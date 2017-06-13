// This file is part of Barrel project. Read README for more information.
// Author: peuhkura@gmail.com

#include "Converter.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>

using namespace Grapevine;

int main(int argc, char *argv[])
{
    try
    {
        Log::I("Grapevine FBX2Barrel");
        Converter converter(argc, argv);
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
