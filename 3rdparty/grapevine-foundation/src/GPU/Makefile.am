noinst_LTLIBRARIES = libgrapevine-gpu.la

grapevine_common_sources_GPU =

if GL_HPP
libgrapevine_gpu_la_SOURCES = $(grapevine_common_sources_GPU) GL.cpp
endif

libgrapevine_gpu_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/GPU
library_include_HEADERS = \
	../../include/Grapevine/GPU/GL.hpp
