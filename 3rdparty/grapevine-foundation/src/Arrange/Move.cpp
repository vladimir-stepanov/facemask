/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Arrange/Move.hpp>

namespace Grapevine
{
    namespace Arrange
    {
        Move::Move(Item const& item, Vector2u const& target):
            item_(item),
            target_(target)
        {
        }

        Move::~Move()
        {
        }

        Move::Move(Move const& r):
            item_(r.item_),
            target_(r.target_)
        {
        }

        Move& Move::operator=(Move const& r)
        {
            if (this != &r)
            {
                item_ = r.item_;
                target_ = r.target_;
            }
            return *this;
        }

        Item const& Move::item() const
        {
            return item_;
        }

        Vector2u const& Move::target() const
        {
            return target_;
        }
    }
}
