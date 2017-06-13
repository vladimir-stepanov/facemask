/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "CanvasPrivateSDL.hpp"
#include "SDL.h"

namespace Grapevine
{
    CanvasPrivateSDL::CanvasPrivateSDL(unsigned int width, unsigned int height)
    : CanvasPrivate(width, height)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING)) throw std::runtime_error(SDL_GetError());

#if SDL_MAJOR_VERSION == 2
        if(!(window_ = SDL_CreateWindow("", 100, 100, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL)))
        {
            throw std::runtime_error(SDL_GetError());
        }

        if(!(context_ = SDL_GL_CreateContext(window_)))
        {
            throw std::runtime_error(SDL_GetError());
        }
#else
        if(!(surface_ = SDL_SetVideoMode (width, height, 0, SDL_OPENGL)))
        {
            throw std::runtime_error(SDL_GetError());
        }
#endif
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        clear();
    }

    CanvasPrivateSDL::~CanvasPrivateSDL()
    {
#if SDL_MAJOR_VERSION == 2
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window_);
#else
        SDL_Quit();
#endif
    }

    void CanvasPrivateSDL::clear()
    {
        glClearColor(clearColor_.x(), clearColor_.y(), clearColor_.z(), clearColor_.w());
        glClearDepthf(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void CanvasPrivateSDL::resize(unsigned int width, unsigned int height)
    {
#if SDL_MAJOR_VERSION == 2
        SDL_SetWindowSize(window_, width, height);
#endif
    }

    void CanvasPrivateSDL::update()
    {
#if SDL_MAJOR_VERSION == 2
        SDL_GL_SwapWindow(window_);
#else
        SDL_GL_SwapBuffers();
#endif
    }

    void CanvasPrivateSDL::setClearColor(Vector4 const& color)
    {
        clearColor_ = color;
        glClearColor(clearColor_.x(), clearColor_.y(), clearColor_.z(), clearColor_.w());
    }

#if SDL_MAJOR_VERSION == 2
    SDL_Window *CanvasPrivateSDL::window()
    {
        return window_;
    }
#else
    SDL_Surface *CanvasPrivateSDL::surface()
    {
        return surface_;
    }
#endif
}
