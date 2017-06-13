/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CANVAS_CANVASPRIVATESDL
#define GRAPEVINE_CANVAS_CANVASPRIVATESDL

#include <SDL.h>

#include "CanvasPrivate.hpp"
#include "Grapevine/GPU/GL.hpp"

namespace Grapevine
{
    class CanvasPrivateSDL: public CanvasPrivate
    {
    public:
        CanvasPrivateSDL(unsigned int width, unsigned int height);
        virtual ~CanvasPrivateSDL();
        void clear();
        void resize(unsigned int width, unsigned int height);
        void update();
        void setClearColor(Vector4 const& color);

#if SDL_MAJOR_VERSION == 2
        SDL_Window *window();
    private:
        SDL_Window *window_;
        SDL_GLContext context_;
#else
        SDL_Surface *surface();
    private:
        SDL_Surface *surface_;
#endif
    };
}

#endif
