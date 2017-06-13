/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/Component.hpp"

namespace Grapevine
{
    namespace Scene
    {
        Component::Component(AbstractComponentPrivate* priva):
            private_(ReferencedPointer<AbstractComponentPrivate>(priva))
        {
        }

        Component::Component(Component const& r):
            private_(r.private_)
        {
        }

        Component::Component():
            private_(NULL)
        {
            throw Exception("Component() should not be used");
        }

        Component::~Component()
        {
        }

        Component& Component::operator=(Component const& r)
        {
            if (this != &r)
            {
                private_ = r.private_;
            }
            return *this;
        }

        std::string const& Component::type() const
        {
            return private_.pointer()->type();
        }

        bool Component::isNull() const
        {
            return private_.isNull();
        }

        void Component::setCumulative(bool value)
        {
            private_.pointer()->setCumulative(value);
        }

        bool Component::cumulative()
        {
            return private_.pointer()->cumulative();
        }
    }
}
