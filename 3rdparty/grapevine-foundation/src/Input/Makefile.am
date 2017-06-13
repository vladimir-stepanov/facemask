noinst_LTLIBRARIES = libgrapevine-input.la

# Common sources
grapevine_common_sources_INPUT = InputSourcePrivate.cpp

# Target specific sources
if USE_XINPUT2
libgrapevine_input_la_SOURCES = \
    $(grapevine_common_sources_INPUT) \
    InputSource-XDisplay.cpp \
    InputSourcePrivateXDisplay.cpp \
    SyncedSlideEvent.cpp
else
libgrapevine_input_la_SOURCES = \
    $(grapevine_common_sources_INPUT) \
    InputSource-NGOS.cpp \
    InputSourcePrivateNGOS.cpp \
    InputSourcePrivateNGOS-Hammerhead.cpp
endif

libgrapevine_input_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/Input
library_include_HEADERS = \
    ../../include/Grapevine/Input/InputSource.hpp \
    ../../include/Grapevine/Input/InputEvent.hpp \
    ../../include/Grapevine/Input/SyncedSlideEvent.hpp

#librarycommon_includedir=$(includedir)/Common/Input
#librarycommon_include_HEADERS = \
#    ../../include/Common/Input/InputEvent.hpp
