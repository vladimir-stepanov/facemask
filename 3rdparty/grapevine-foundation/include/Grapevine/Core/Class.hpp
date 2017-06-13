/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_CLASS_HPP
#define GRAPEVINE_CORE_CLASS_HPP

namespace Grapevine
{
    /**
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     */

    /**
     * Class has copy constructor and assigment operator implemented
     */
    #define CLASS_COPYABLE(ClassName) \
        ClassName(ClassName const& instance);   \
        ClassName& operator=(ClassName const& r);

    /**
     * Class has private copy constructor and assigment operator so
     * that no copy and assign can happen.
     */
    #define CLASS_UNCOPYABLE(ClassName) \
    private: \
        ClassName(const ClassName&); \
        ClassName& operator=(const ClassName&); \
    public:

    /**
     * Used to make clear the usage the PIMPL pattern
     */
    #define CREATE_PRIVATE(ClassName)    \
        private_(new ClassName)

    /**
     * Used to make clear the usage the PIMPL pattern
     */
    #define FREE_PRIVATE        \
        if (private_ != NULL)   \
        {                       \
            delete private_;    \
            private_ = NULL;    \
        }

    /**
     * When your class is plainly owning referenced pointer and it is
     * copyable (CLASS_COPYABLE) you can use next lines:
     */
    #define CLASS_REFERENCEDPOINTER_OWNER_BASIC_COPYCONSTRUCTOR(ClassName) \
        ClassName::ClassName(ClassName const& r): \
            private_(r.private_) \
        { \
        }

    #define CLASS_REFERENCEDPOINTER_OWNER_BASIC_ASSIGN(ClassName) \
        ClassName& ClassName::operator=(ClassName const& r) \
        { \
            if (this != &r) \
            { \
                private_ = r.private_; \
            } \
            return *this; \
        }
}

#endif
