noinst_LTLIBRARIES = libgrapevine-animation.la

libgrapevine_animation_la_SOURCES = \
    Path.cpp \
    BezierQuadraticPathPrivate.cpp \
    BezierQuadraticPath.cpp \
    BezierComplexPath.cpp \
    BezierComplexPathPrivate.cpp \
    LinearPathPrivate.cpp \
    LinearPath.cpp \
    Easing.cpp \
    AnimationManager.cpp \
    AnimationPrivate.cpp \
    Animation.cpp \
    ControlPoint.cpp \
    ControlPointPrivate.cpp

libgrapevine_animation_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/Animation
library_include_HEADERS = \
    ../../include/Grapevine/Animation/Path.hpp \
    ../../include/Grapevine/Animation/Easing.hpp \
    ../../include/Grapevine/Animation/AnimationManager.hpp \
    ../../include/Grapevine/Animation/AnimationPrivate.hpp \
    ../../include/Grapevine/Animation/Animation.hpp \
    ../../include/Grapevine/Animation/BezierQuadraticPath.hpp \
    ../../include/Grapevine/Animation/BezierComplexPath.hpp \
    ../../include/Grapevine/Animation/ControlPoint.hpp \
    ../../include/Grapevine/Animation/LinearPath.hpp
