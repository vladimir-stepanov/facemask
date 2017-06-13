/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ARRANGE_MOVE
#define GRAPEVINE_ARRANGE_MOVE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Arrange/Item.hpp>
#include <string>

namespace Grapevine
{
    namespace Arrange
    {
        class Move
        {
        public:
            CLASS_COPYABLE(Move)

            Move(Item const& item, Vector2u const& target);
            virtual ~Move();
            Item const& item() const;
            Vector2u const& target() const;

        private:
            Item item_;
            Vector2u target_;
        };
    }
}

#endif
