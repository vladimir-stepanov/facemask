/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_BARREL
#define GRAPEVINE_MODEL_BARREL

#include <string>

namespace Grapevine
{
    class Barrel
    {

    public:
        static const unsigned int MaximumBlockHeaderLength = 1024;
        static std::string FileTagStart;
        static std::string FileTagEnd;
        static std::string FileTagMesh;
        static std::string FileTagMaterial;

    private:
        Barrel();
        ~Barrel();
    };
}

#endif
