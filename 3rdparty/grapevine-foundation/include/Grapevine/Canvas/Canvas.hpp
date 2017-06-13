/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CANVAS_CANVAS
#define GRAPEVINE_CANVAS_CANVAS

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <cstddef>

namespace Grapevine
{
    /**
     * @class Canvas
     *
     * Canvas is a class that handles the screen surface.
     */
    class Canvas
    {
    public:
        CLASS_UNCOPYABLE(Canvas);

        /**
         * Constructor that creates a screen with given size.
         *
         * @param [in]  width       pixel width of the canvas.
         * @param [in]  height      pixel height of the canvas.
         */
        Canvas(unsigned int width, unsigned int height);

        /**
         * Uninitializes the screen surface.
         */
        ~Canvas();

        /**
         * Get the width of the canvas.
         *
         * @return      Width of the canvas.
         */
        unsigned int width() const;

        /**
         * Get the height of the canvas.
         *
         * @return      Height of the canvas.
         */
        unsigned int height() const;

        /**
         * Clear the canvas.
         * @note Use setClearColor() to change the clear color.
         */
        void clear();

        /**
         * Resize the canvas.
         *
         * @param [in]  width       pixel width of the canvas.
         * @param [in]  height      pixel height of the canvas.
         */
        void resize(unsigned int width, unsigned int height);

        /**
         * Updates the canvas. This means technically a buffer swap.
         */
        void update();

        /**
         * Sets a new clear color in  format.
         *
         * @param [in]  color   clear color in RGBA format.
         */
        void setClearColor(Vector4 const& color);

        /**
         * Set the resolution if supported.
         *
         * @param [in] Resolution which can be smaller than the native width/height.
         */
        void setResolution(Vector2i const& resolution);

        /**
         * Get the resolution of the canvas (can be smaller than the size of the canvas)
         *
         * @return      Resolution of the canvas. Same as width and height when not supported
         */
        Vector2i resolution() const;

    private:

        /**
         * X is asking the inputs from actual display
         * InputSourcePrivate is can then access the private canvas to cast it
         */
        friend class InputSource;
        class CanvasPrivate* private_;
    };
}

#endif
