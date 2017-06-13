noinst_LTLIBRARIES = libgrapevine-model.la

# Common sources
grapevine_common_sources_MODEL = ModelFactory.cpp 

#grapevine_common_sources_MODEL = ModelAsset.cpp \
#	MeshAsset.cpp \
#	ModelResource.cpp \
#	ModelFactory.cpp \
#	ModelData.cpp \
#	Barrel.cpp \
#	DynamicMesh.cpp \
#	DynamicMeshPrivate-GLES2.cpp

# Target specific sources
#if USE_GLES2
#libgrapevine_model_la_SOURCES = $(grapevine_common_sources_MODEL) \
#	MeshResource-GLES2.cpp
#endif

libgrapevine_model_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/Model

library_include_HEADERS = \
	../../include/Grapevine/Model/ModelFactory.hpp

#library_include_HEADERS = \
#	../../include/Grapevine/Model/ModelAsset.hpp \
#	../../include/Grapevine/Model/MeshAsset.hpp \
#	../../include/Grapevine/Model/DynamicMesh.hpp \
#	../../include/Grapevine/Model/ModelResource.hpp \
#	../../include/Grapevine/Model/MeshResource.hpp
