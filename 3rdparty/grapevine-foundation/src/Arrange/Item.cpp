/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Arrange/Item.hpp>

namespace Grapevine
{
    namespace Arrange
    {
        unsigned int Item::lastId_ = 1;

        Item::Item(Vector2u const& size):
            id_(lastId_++),
            size_(size)
        {
        }

        Item::~Item()
        {
        }

        Item::Item(Item const& r):
            id_(r.id_),
            size_(r.size_)
        {
        }

        Item& Item::operator=(Item const& r)
        {
            if (this != &r)
            {
                id_ = r.id_;
                size_ = r.size_;
            }
            return *this;
        }

        bool Item::operator==(Item const& r)
        {
            if (id_ == r.id())
            {
                return true;
            }
            return false;
        }

        Vector2u const& Item::size() const
        {
            return size_;
        }

        unsigned int Item::id() const
        {
            return id_;
        }
    }
}
