noinst_LTLIBRARIES = libgrapevine-material.la

# Common sources
grapevine_common_sources_MATERIAL = TextureAsset.cpp CubemapAsset.cpp MaterialAsset.cpp MaterialResource.cpp FramebufferAsset.cpp

# Target specific sources
if USE_GLES2
libgrapevine_material_la_SOURCES = $(grapevine_common_sources_MATERIAL) TextureResource-GLES2.cpp CubemapResource-GLES2.cpp
endif

# Includes
libgrapevine_material_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)
library_includedir=$(includedir)/Grapevine/Material
library_include_HEADERS = ../../include/Grapevine/Material/TextureAsset.hpp \
	../../include/Grapevine/Material/CubemapAsset.hpp \
	../../include/Grapevine/Material/MaterialAsset.hpp \
	../../include/Grapevine/Material/TextureResource.hpp \
	../../include/Grapevine/Material/CubemapResource.hpp \
	../../include/Grapevine/Material/MaterialResource.hpp \
	../../include/Grapevine/Material/FramebufferAsset.hpp

