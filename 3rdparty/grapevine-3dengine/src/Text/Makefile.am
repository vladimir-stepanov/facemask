noinst_LTLIBRARIES = libgrapevine-text.la

libgrapevine_text_la_SOURCES = RichText.cpp \
    Text.cpp \
    FontAsset.cpp \
    FontResource.cpp \
    FontManager.cpp \
    FontManagerPrivate.cpp

libgrapevine_text_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

library_includedir=$(includedir)/Grapevine/Text
library_include_HEADERS = ../../include/Grapevine/Text/RichText.hpp \
    ../../include/Grapevine/Text/Text.hpp \
    ../../include/Grapevine/Text/FontAsset.hpp \
    ../../include/Grapevine/Text/FontManager.hpp
