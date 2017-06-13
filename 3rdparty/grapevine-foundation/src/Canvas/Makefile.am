noinst_LTLIBRARIES = libgrapevine-canvas.la

# Common sources
grapevine_common_sources_canvas = CanvasPrivate.cpp

# Target specific sources
if DESKTOPLINUX_XEGL_GLES2
libgrapevine_canvas_la_SOURCES = $(grapevine_common_sources_canvas) Canvas-XEGL.cpp CanvasPrivateXEGL.cpp
endif

if NATIVE_EGL_GLES2
libgrapevine_canvas_la_SOURCES = $(grapevine_common_sources_canvas) Canvas-EGL.cpp
endif

# Includes
libgrapevine_canvas_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)
library_includedir=$(includedir)/Grapevine/Canvas
library_include_HEADERS = ../../include/Grapevine/Canvas/Canvas.hpp
