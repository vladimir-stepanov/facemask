/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ARRANGE_ITEM
#define GRAPEVINE_ARRANGE_ITEM

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <string>

namespace Grapevine
{
    namespace Arrange
    {
        class Item
        {
        public:
            CLASS_COPYABLE(Item)

            Item(Vector2u const& size);
            ~Item();
            bool operator==(Item const& r);
            Vector2u const& size() const;
            unsigned int id() const;

        private:
            static unsigned int lastId_;
            unsigned int id_;
            Vector2u size_;
        };
    }
}

#endif
