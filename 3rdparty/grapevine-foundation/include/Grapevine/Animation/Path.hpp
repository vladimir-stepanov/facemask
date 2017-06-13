/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_PATH
#define GRAPEVINE_ANIMATION_PATH

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Math/Matrix.hpp>

namespace Grapevine
{
    /**
     * @class AbstractPathPrivate
     *
     * Component gives functionality to Path. User can inherit
     * AbstractPathPrivate to create new functionality.
     */
    template <typename T>
    class AbstractPathPrivate: public Referenced
    {
    public:
        //CLASS_UNCOPYABLE(AbstractPathPrivate)

        AbstractPathPrivate()
        {
        };

        virtual ~AbstractPathPrivate()
        {
        };

        virtual T value(float offset) const
        {
            return T();
        }
    };

    // @note: This is a base class for or path classes
    template <typename T>
    class Path
    {
    public:
        Path(AbstractPathPrivate<T>* private__):
            private_(ReferencedPointer<AbstractPathPrivate<T> >(private__))
        {
        }

        virtual ~Path()
        {
        }

        Path(Path const& r):
            private_(r.private_)
        {
        }

        Path& operator=(Path const& r)
        {
            if (this != &r)
            {
                private_ = r.private_;
            }
            return *this;
        }

        /**
         * Get path location
         *
         * @param [in]  offset  [0.0, 1.0] value, 0.0 being the start of
         *                      the path and 1.0 the end of the path.
         */
        T value(float offset) const
        {
            return private_.pointer()->value(offset);
        }

    protected:
        ReferencedPointer<AbstractPathPrivate<T> > private_;

    private:
        Path()
        {
            throw Exception("Path() should not be used");
        }
    };
}

#endif
