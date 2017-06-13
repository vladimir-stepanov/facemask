noinst_LTLIBRARIES = libgrapevine-scene.la

libgrapevine_scene_la_SOURCES = \
    Component.cpp \
    Node.cpp \
    NodePrivate.cpp \
    NodeList.cpp \
    NodeListPrivate.cpp \
    Graph.cpp \
    GraphPrivate.cpp \
    AbstractComponentPrivate.cpp \
    AbstractVisitor.cpp \
    AbstractTreeVisitor.cpp \
    AbstractListVisitor.cpp

libgrapevine_scene_la_CPPFLAGS = -I$(top_srcdir)/../grapevine/include -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)
# Includes
library_includedir=$(includedir)/Grapevine/Scene
library_include_HEADERS = \
    ../../include/Grapevine/Scene/Graph.hpp \
    ../../include/Grapevine/Scene/Node.hpp \
    ../../include/Grapevine/Scene/NodePrivate.hpp \
    ../../include/Grapevine/Scene/NodeList.hpp \
    ../../include/Grapevine/Scene/Component.hpp \
    ../../include/Grapevine/Scene/AbstractComponentPrivate.hpp \
    ../../include/Grapevine/Scene/AbstractComponentPrivateTemplate.hpp \
    ../../include/Grapevine/Scene/AbstractVisitor.hpp \
    ../../include/Grapevine/Scene/AbstractTreeVisitor.hpp \
    ../../include/Grapevine/Scene/AbstractListVisitor.hpp
