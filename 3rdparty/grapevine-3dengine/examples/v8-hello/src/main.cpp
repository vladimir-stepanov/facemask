#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Resource/ResourceManager.hpp>

#include <v8.h>

#include "bindings.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage:\n\t"\
               "./v8_hello <script>\n");
        return -1;
    }

    try
    {
        Grapevine::ResourceManager resourceManager("assets/");
        V8::VM vm("./");
        V8::VM::Scope scope(vm);
        scope.runScript("scripts/grapevine.js");
        scope.runScript(argv[1]);
    }
    catch(std::exception &e)
    {
        printf("Error: %s\n", e.what());
        return -1;
    }
    return 0;
}
