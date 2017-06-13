noinst_LTLIBRARIES = libgrapevine-image.la

libgrapevine_image_la_SOURCES = ImageAsset.cpp \
	ImageResource.cpp \
	ImagePng.cpp \
	ImageJpeg.cpp \
	ImageKtx.cpp \
	Bitmap.cpp \
    GeneralImageResource.cpp

libgrapevine_image_la_CPPFLAGS = -I$(top_srcdir)/include  $(GRAPEVINE_CFLAGS) -ljpeg

library_includedir=$(includedir)/Grapevine/Image
library_include_HEADERS = ../../include/Grapevine/Image/ImageAsset.hpp \
	../../include/Grapevine/Image/Bitmap.hpp \
	../../include/Grapevine/Image/ImagePng.hpp \
	../../include/Grapevine/Image/ImageJpeg.hpp \
	../../include/Grapevine/Image/ImageKtx.hpp
