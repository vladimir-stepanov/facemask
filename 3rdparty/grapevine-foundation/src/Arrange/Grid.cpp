/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Arrange/Grid.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/String.hpp>
#include <string>
#include <set>

namespace Grapevine
{
    namespace Arrange
    {
        // private re-arrange implemenetation
        namespace
        {
            struct TempGrid
            {
                TempGrid(const Vector2u &size, const std::map<int, Item> &items, const Item &placedItem)
                : size_(size), items_(items)
                {
                    items_.insert(std::make_pair(placedItem.id(), placedItem));
                }

                TempGrid(const Vector2u &size, const std::map<int, Item> &items)
                : size_(size), items_(items)
                {
                }

                bool getOverlaps(int itemId, Vector2u targetLocation, std::set<int> *overlaps = NULL)
                {
                    const Item &item = items_.find(itemId)->second;
                    const Vector2u &itemSize(item.size());
                    if(overlaps) overlaps->clear();

                    for(std::map<int, Vector2u>::const_iterator it = locations_.begin(); it != locations_.end(); ++it)
                    {
                        const Item &item = items_.find(it->first)->second;

                        const Vector2u &size(item.size());
                        const Vector2u &loc(it->second);

                        //LOGD("Check if %ix%i at %i,%i overlaps with %ix%i at %i,%i", size.x(), size.y(), loc.x(), loc.y(), itemSize.x(), itemSize.y(), targetLocation.x(), targetLocation.y());
                        const Vector2u &minA = targetLocation;
                        const Vector2u &minB = loc;
                        const Vector2u &maxA = targetLocation + itemSize - Vector2u(1);
                        const Vector2u &maxB = loc + size - Vector2u(1);

                        if( minA.x() <= maxB.x() &&
                            minB.x() <= maxA.x() &&
                            minA.y() <= maxB.y() &&
                            minB.y() <= maxA.y())
                        {
                            //LOGD("Overlaps with %i", it->first);
                            if(overlaps)
                            {
                                overlaps->insert(it->first);
                            }
                            else
                            {
                                return true;
                            }
                        }

                    }
                    return overlaps ? overlaps->size() > 0 : false;
                }

                Vector2u nearestOffset(const Vector2u &size, const Vector2u &itemSize, const Vector2u &targetLocation, const Vector2i &directionHint)
                {
                    Vector2u loc(0);
                    if(directionHint.x() < 0)
                    {
                        loc.x(targetLocation.x()-itemSize.x());
                    }
                    else if(directionHint.x() > 0)
                    {
                        loc.x(targetLocation.x()+size.x());
                    }

                    if(directionHint.y() < 0)
                    {
                        loc.y(targetLocation.y()-itemSize.y());
                    }
                    else if(directionHint.y() > 0)
                    {
                        loc.y(targetLocation.y()+size.y());
                    }

                    return loc;
                }

                bool recPlace(const int itemId, const Vector2u targetLocation, const Vector2i directionHint)
                {
                    //LOGD("Place: %i at %ix%i", itemId, targetLocation.x(), targetLocation.y());

                    const Item &item = items_.find(itemId)->second;
                    if(targetLocation.x() > size_.x() || targetLocation.y() > size_.y() ||
                       targetLocation.x() + item.size().x() > size_.x() || targetLocation.y() + item.size().y() > size_.y())
                    {
                        //LOGD("Outside grid");
                        // outside grid
                        return false;
                    }

                    TempGrid newGrid(size_, items_);
                    newGrid.locations_ = locations_;
                    newGrid.locked_ = locked_;

                    std::set<int> overlaps;
                    getOverlaps(itemId, targetLocation, &overlaps);
                    std::map<int, Vector2u> newTargetLocations;
                    for(std::set<int>::iterator it = overlaps.begin(); it != overlaps.end(); ++it)
                    {
                        //LOGD("Move %i", *it);
                        int itemToBeMoved = *it;
                        if(locked_.find(itemToBeMoved) != locked_.end())
                        {
                            //LOGD("Locked item");
                            return false;
                        }

                        const Vector2u &loc = locations_[*it];
                        const Vector2u &size = items_.find(*it)->second.size();
                        const Vector2u &replacementSize = item.size();

                        //LOGD("Moved item loc: %i,%i size %ix%i", loc.x(), loc.y(), size.x(), size.y());
                        //LOGD("Repl item loc: %i,%i size: %ix%i", targetLocation.x(), targetLocation.y(), replacementSize.x(), replacementSize.y());
                        //LOGD("Dir hint      : %i,%i", directionHint.x(), directionHint.y());

                        // calculate new location for the items
                        Vector2u newTargetLocation = nearestOffset(replacementSize, size, targetLocation - loc, directionHint) + loc;

                        newTargetLocations.insert(std::make_pair(*it, newTargetLocation));

                        newGrid.locations_.erase(*it); //remove items that are being moved
                    }

                    newGrid.locations_[itemId] = targetLocation; //place the item
                    newGrid.locked_.insert(itemId);

                    for(std::set<int>::iterator it = overlaps.begin(); it != overlaps.end(); ++it)
                    {
                        const Vector2u newTargetLocation(newTargetLocations[*it]);
                       // LOGD("Trying to place %i to %ix%i", *it, newTargetLocation.x(), newTargetLocation.y());
                        if(!newGrid.recPlace(*it, newTargetLocation, directionHint))
                        {
                            return false;
                        }
                    }
                    locations_ = newGrid.locations_;
                    return true;
                }

                bool nearestPlace(const int itemId, const Vector2u targetLocation)
                {
                    TempGrid newGrid(size_, items_);
                    newGrid.locations_ = locations_;
                    newGrid.locked_ = locked_;

                    std::set<int> overlaps;
                    getOverlaps(itemId, targetLocation, &overlaps);

                    for(std::set<int>::iterator it = overlaps.begin(); it != overlaps.end(); ++it)
                    {
                        //LOGD("Move %i", *it);
                        int itemToBeMoved = *it;
                        if(locked_.find(itemToBeMoved) != locked_.end())
                        {
                            //LOGD("Locked item");
                            return false;
                        }

                        newGrid.locations_.erase(*it); //remove items that are being moved
                    }

                    newGrid.locations_[itemId] = targetLocation; //place the item
                    newGrid.locked_.insert(itemId);

                    for(std::set<int>::iterator it = overlaps.begin(); it != overlaps.end(); ++it)
                    {
                        int itemId = *it;
                        Vector2u closestPosition = Vector2u(-1, -1);
                        unsigned int closestDistance = 0xffffffffu;

                        for(unsigned int y = 0; y < size_.y(); y++)
                        {
                            for(unsigned int x = 0; x < size_.x(); x++)
                            {
                                if(!getOverlaps(itemId, Vector2u(x, y)))
                                {
                                    Vector2u diff(locations_[itemId] - Vector2u(x, y));
                                    unsigned int distance = diff.x() * diff.x() + diff.y() * diff.y();
                                    if(distance < closestDistance)
                                    {
                                        closestPosition = Vector2u(x, y);
                                        closestDistance = distance;
                                    }
                                }
                            }
                        }

                        if(closestDistance == 0xffffffffu)
                        {
                            return false;
                        }

                        newGrid.locations_[itemId] = closestPosition; //place the item
                        newGrid.locked_.insert(itemId);
                    }

                    locations_ = newGrid.locations_;

                    return true;
                }

                const Vector2u size_;
                std::map<int, Item> items_;
                std::map<int, Vector2u> locations_;
                std::set<int> locked_;
            };
        }


        Grid::Grid(Vector2u const& size):
            size_(size),
            idGrid_()
        {
            refreshIdGrid();
        }

        Grid::~Grid()
        {
        }

        bool Grid::hasItem(Vector2u const& location)
        {
            unsigned int id = idGrid_[location.x() + location.y() * size_.x()];

            std::map<int, Item>::iterator i = items_.find(id);
            if (i != items_.end())
            {
                return true;
            }
            return false;
        }

        Item Grid::get(Vector2u const& location)
        {
            unsigned int id = idGrid_[location.x() + location.y() * size_.x()];

            std::map<int, Item>::iterator i = items_.find(id);
            return i->second;
        }

        void Grid::remove(Item const& item)
        {
            std::map<int, Item>::iterator i = items_.find(item.id());
            if (i != items_.end())
            {
                items_.erase(i);

                // Remove locations
                std::map<int, Vector2u>::iterator i = itemLocations_.find(item.id());
                if (i != itemLocations_.end())
                {
                    itemLocations_.erase(i);
                }
                else
                {
                    LOGD("Grid::remove(), did not find item location with id: %d", item.id());
                }
                refreshIdGrid();
            }
            else
            {
                LOGD("Grid::remove(), did not find item with id: %d", item.id());
            }
        }

        std::vector<Move> Grid::arrange(Item const& item, Vector2u const& location, Vector2i const& directionHint, bool& placementDone)
        {
            LOGD("Grid::place(), item id: %d, size: %d x %d", item.id(), item.size().x(), item.size().y());

            placementDone = false;
            if (isInsideGrid(location, item) == false)
            {
                return std::vector<Move>();
            }

            // Items under the placed item
            unsigned int startX = location.x();
            unsigned int endX = location.x() + item.size().x() - 1;
            unsigned int startY = location.y();
            unsigned int endY = location.y() + item.size().y() - 1;
            std::set<int> itemsToMove;
            for(unsigned int y = startY; y <= endY; y++)
            {
                for(unsigned int x = startX; x <= endX; x++)
                {
                    if (idGrid_[x + y * size_.x()] != 0)
                    {
                        LOGD("under id:%d was %d", item.id(), idGrid_[x + y * size_.x()]);
                        itemsToMove.insert(idGrid_[x + y * size_.x()]);
                    }
                }
            }

            // Check if new placement can be found
            std::vector<Move> r;
            if(itemsToMove.size() == 0)
            {
                // We had an empty place which need no moves
                placementDone = true;
            }
            else
            {
                TempGrid newGrid(size_, items_, item);
                newGrid.locations_ = itemLocations_;

                // try re-arranging the items first to the direction of directionHint, then try three other directions
                if(newGrid.recPlace(item.id(), location, directionHint)                                    ||
                   newGrid.recPlace(item.id(), location, Vector2i(directionHint.y(), directionHint.x()))   ||
                   newGrid.recPlace(item.id(), location, Vector2i(-directionHint.y(), -directionHint.x())) ||
                   newGrid.recPlace(item.id(), location, Vector2i(-directionHint.x(), -directionHint.y())) ||
                   newGrid.nearestPlace(item.id(), location)
                   )
                {
                    LOGD("Found new arrangement!");
                    for(std::map<int, Vector2u>::const_iterator it = itemLocations_.begin(); it != itemLocations_.end(); ++it)
                    {
                        const Vector2u &newLocation(newGrid.locations_[it->first]);
                        const Vector2u &oldLocation(it->second);
                        if(newLocation.x() != oldLocation.x() || newLocation.y() != oldLocation.y())
                        {
                            int id = it->first;
                            r.push_back(Move(items_.find(id)->second, newLocation));
                        }
                    }

                    placementDone = true;
                }
                else
                {
                    LOGD("Failed to find new arrangement!");
                }
            }
            return r;
        }

        void Grid::move(std::vector<Move> const& moves)
        {
            for(std::vector<Move>::size_type i = 0; i < moves.size(); i++)
            {
                std::map<int, Vector2u>::iterator location = itemLocations_.find(moves[i].item().id());
                if(location != itemLocations_.end())
                {
                    location->second = moves[i].target();
                }
            }

            refreshIdGrid();
        }

        bool Grid::place(Item const& item, Vector2u const &location)
        {
            Vector2u size = item.size();

            if(checkPosition(location,size))
            {
                insertToItems(location, item);
                refreshIdGrid();
                return true;
            }
            else
            {
                return false;
            }
        }

        bool Grid::checkPosition(Vector2u const &position, Vector2u const &size)
        {
            if(isInsideGrid(position, size) == false)
            {
                return false;
            }

            unsigned int startX = position.x();
            unsigned int endX = position.x() + size.x();
            unsigned int startY = position.y();
            unsigned int endY = position.y() + size.y();

            for(unsigned int y = startY; y < endY; y++)
            {
                for(unsigned int x = startX; x < endX; x++)
                {
                    if (idGrid_[x + y * size_.x()] != 0)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        bool Grid::findFreePlace(Vector2u &location, Vector2u const &size)
        {
            for(int y = (size_.y()-1); y >= 0; y--)
            {
                for(unsigned int x = 0; x < size_.x(); x++)
                {
                    Vector2u pos(x,y);

                    if(checkPosition(pos,size))
                    {
                        location = pos;
                        return true;
                    }
                }
            }

            return false;
        }


        void Grid::refreshIdGrid()
        {
            idGrid_.resize(size_.x() * size_.y());
            for(unsigned int i = 0; i < size_.x() * size_.y(); i++)
            {
                idGrid_[i] = 0;
            }

            // Place items to grid
            for(std::map<int, Item>::iterator i = items_.begin(); i != items_.end(); i++)
            {
                Item item(i->second);
                std::map<int, Vector2u>::iterator iLocation = itemLocations_.find(item.id());
                if (iLocation == itemLocations_.end())
                {
                    LOGE("Grid::refreshIdGrid(), item id:%d has no location in grid.", item.id());
                    throw Exception("Grid::refreshIdGrid(), grid corrputed!");
                }
                Vector2u location(iLocation->second);

                unsigned int startX = location.x();
                unsigned int endX = location.x() + item.size().x() - 1;
                unsigned int startY = location.y();
                unsigned int endY = location.y() + item.size().y() - 1;
                for(unsigned int x = startX; x <= endX; x++)
                {
                    for(unsigned int y = startY; y <= endY; y++)
                    {
                        idGrid_[x + y * size_.x()] = item.id();
                    }
                }
            }

            // If we got here, given items were placed sanely to the grid.
        }

        bool Grid::isInsideGrid(Vector2u const& location, Item const& item)
        {
            bool r = true;

            unsigned int endX = location.x() + item.size().x() - 1;
            unsigned int endY = location.y() + item.size().y() - 1;

            if (endX >= size_.x())
            {
                LOGE("Item %d x was over the grid size.", item.id());
                r = false;
            }
            if (endY >= size_.y())
            {
                LOGE("Item %d y was over the grid size.", item.id());
                r = false;
            }

            return r;
        }

        bool Grid::isInsideGrid(Vector2u const& location, Vector2u const& size)
        {
            bool r = true;

            unsigned int endX = location.x() + size.x() - 1;
            unsigned int endY = location.y() + size.y() - 1;

            if (endX >= size_.x())
            {
                r = false;
            }
            if (endY >= size_.y())
            {
                r = false;
            }

            return r;
        }

        void Grid::insertToItems(Vector2u const& location, Item const& item)
        {
            std::pair<unsigned int, Vector2u> tmpLocation(item.id(), location);
            itemLocations_.insert(tmpLocation);

            std::pair<unsigned int, Item> tmpItem(item.id(), item);
            items_.insert(tmpItem);
        }

        void Grid::debugPrint()
        {
            LOGD("%d, %d", size_.x(), size_.y());
            for(unsigned int y = 0; y < size_.y(); y++)
            {
                std::string lineText;
                for(unsigned int x = 0; x < size_.x(); x++)
                {
                    lineText += std::string(" |");
                    if (idGrid_[x + y * size_.x()] != 0)
                    {
                        std::string tmp = String::toString(idGrid_[x + y * size_.x()]);
                        unsigned int tmpSize = 0;
                        if (tmp.size() <= 3)
                        {
                            tmpSize = 3 - tmp.size();
                        }
                        tmp.insert(0, "     ", tmpSize);
                        lineText += tmp;
                    }
                    else
                    {
                        lineText += std::string("  -");
                    }
                }
                LOGD("%s |", lineText.c_str());
            }
        }

        std::map<int, Vector2u> &Grid::itemLocations()
        {
            return itemLocations_;
        }
    }
}
