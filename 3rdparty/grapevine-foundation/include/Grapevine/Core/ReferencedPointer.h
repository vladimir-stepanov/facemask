/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_REFERENCEDPOINTER
#define GRAPEVINE_CORE_REFERENCEDPOINTER

#include "Grapevine/Core/ReferencedPointer.hpp"

/**
 * NOTE: this is a wrapper class for the .hpp file. Please use this one instead of the .hpp file in the Atelier OS project.
 */

/**
 * "::rp", the shortened version from ReferencedPointer using C++11 standard.
 *
 * @example Grapevine::rp<MyClass> myVariable(new MyClass(myParameter));
 */
namespace Grapevine
{
    template<typename T>
    using rp = ReferencedPointer<T>;
}

#endif
