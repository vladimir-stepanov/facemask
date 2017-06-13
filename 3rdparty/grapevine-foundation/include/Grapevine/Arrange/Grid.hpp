/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ARRANGE_GRID
#define GRAPEVINE_ARRANGE_GRID

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Arrange/Item.hpp>
#include <Grapevine/Arrange/Move.hpp>
#include <vector>
#include <map>
#include <set>

namespace Grapevine
{
    namespace Arrange
    {
        class Grid
        {
        public:
            CLASS_UNCOPYABLE(Grid)

            /**
             * Constructs grid from given items.
             *
             * @param [in]  size            Grid size.
             */
            Grid(Vector2u const& size);
            virtual ~Grid();

            /**
             * Calculate moves required to fit item to grid.
             *
             * @param [in]  item            Item to place.
             * @param [in]  location        Location to place item to.
             * @param [in]  placementDone   Set to TRUE if arrange was
             *                              succesful, FALSE otherwise.
             * @return                      Moves that give new locations
             *                              to arranged items.
             */
            std::vector<Move> arrange(Item const& item, Vector2u const& location, Vector2i const &directionHint, bool& placementDone);

            /**
             * Move items
             *
             * @param [in]  moves           Vector of moves from arrange.
             *
             */
            void move(std::vector<Move> const& moves);

            /**
             * Place item to grid.
             *
             * @param [in]  item            Item to place.
             * @param [in]  location        Location to place item to.
             * @return                      return TRUE if placement was
             *                              succesful, FALSE otherwise.
             */
            bool place(Item const& item, Vector2u const &location);

            /**
             * See if space is free on grid for given size.
             *
             * @param [in]  item            Item to place.
             * @param [in]  size            Size of place needed
             * @return                      return TRUE if placement could be
             *                              made, FALSE otherwise.
             */
            bool findFreePlace(Vector2u &location, Vector2u const &size);

            /**
             * See if location is taken
             *
             * @param [in]  location        Location to look at.
             * @return                      TRUE if location has item,
             *                              FALSE otherwise.
             */
            bool hasItem(Vector2u const& location);

            /**
             * @param [in]  location        Location to look at.
             * @return                      Item at given location
             *
             * @note        use hasItem() to check first that grid has
             *              item in given location!
             */
            Item get(Vector2u const& location);

            /**
             * @param [in]  item            Removes item from grid.
             *
             * @note        Does nothing if item is not in the grid.
             */
            void remove(Item const& item);

            /**
             * Prints grid to console (y axis is downwards).
             */
            void debugPrint();


            /*
             * Items' locations
             */
            std::map<int, Vector2u> &itemLocations();

        private:
            // Call only when items are all checked with isInsideGrid() == true
            void refreshIdGrid();

            bool isInsideGrid(Vector2u const& location, Item const& item);
            bool isInsideGrid(Vector2u const& location, Vector2u const& size);
            void insertToItems(Vector2u const& location, Item const& item);
            bool checkPosition(Vector2u const& position, Vector2u const& size);
            Vector2u size_;
            std::map<int, Item> items_;
            std::map<int, Vector2u> itemLocations_;
            std::vector<int> idGrid_;
        };
    }
}

#endif
