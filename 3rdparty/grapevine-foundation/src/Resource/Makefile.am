noinst_LTLIBRARIES = libgrapevine-resource.la

libgrapevine_resource_la_SOURCES = File.cpp Resource.cpp ResourceManager.cpp ResourceManagerPrivate.cpp MemoryFile.cpp

libgrapevine_resource_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

library_includedir = $(includedir)/Grapevine/Resource
library_include_HEADERS = ../../include/Grapevine/Resource/File.hpp \
	../../include/Grapevine/Resource/ResourceManagerPrivate.hpp \
	../../include/Grapevine/Resource/ResourceManager.hpp \
	../../include/Grapevine/Resource/Resource.hpp \
	../../include/Grapevine/Resource/MemoryFile.hpp
