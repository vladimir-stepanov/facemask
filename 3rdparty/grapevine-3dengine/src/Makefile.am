SUBDIRS = Scene SceneExtensions
lib_LTLIBRARIES = libgrapevine-3dengine.la
libgrapevine_3dengine_la_SOURCES =
libgrapevine_3dengine_la_LIBADD = $(DEPENDENCIES_LIBS) \
	Scene/libgrapevine-scene.la \
	SceneExtensions/libgrapevine-sceneextensions.la

libgrapevine_3dengine_la_LDFLAGS = -module -avoid-version -shared --disable-static

# Includes
library_includedir=$(includedir)/Grapevine
