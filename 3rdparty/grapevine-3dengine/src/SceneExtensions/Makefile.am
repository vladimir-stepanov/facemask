noinst_LTLIBRARIES = libgrapevine-sceneextensions.la


# Common sources
grapevine_common_sources_SCENEEXTENSIONS = \
    InfoComponent.cpp \
    InfoComponentPrivate.cpp \
    InputVisitor.cpp \
    InputComponent.cpp \
    InputComponentPrivate.cpp \
    TransformationComponent.cpp \
    TransformationComponentPrivate.cpp \
    CameraComponent.cpp \
    CameraComponentPrivate.cpp \
    AbstractRenderableComponent.cpp \
    AbstractRenderableComponentPrivate.cpp \
    ModelComponent.cpp \
    ModelComponentPrivate.cpp \
    BoundingVolumeComponent.cpp \
    BoundingVolumeComponentPrivate.cpp \
    TransformVisitor.cpp \
    CullerVisitor.cpp \
    SimpleScene.cpp

# Target specific sources
if USE_GLES2
libgrapevine_sceneextensions_la_SOURCES = $(grapevine_common_sources_SCENEEXTENSIONS) \
    Renderer-GLES2.cpp
endif

libgrapevine_sceneextensions_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

# Includes
library_includedir=$(includedir)/Grapevine/SceneExtensions
library_include_HEADERS = \
    ../../include/Grapevine/SceneExtensions/InfoComponent.hpp \
    ../../include/Grapevine/SceneExtensions/InfoComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/InputComponent.hpp \
    ../../include/Grapevine/SceneExtensions/InputComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/TransformationComponent.hpp \
    ../../include/Grapevine/SceneExtensions/TransformationComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/CameraComponent.hpp \
    ../../include/Grapevine/SceneExtensions/CameraComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/AbstractRenderableComponent.hpp \
    ../../include/Grapevine/SceneExtensions/AbstractRenderableComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/ModelComponent.hpp \
    ../../include/Grapevine/SceneExtensions/ModelComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/BoundingVolumeComponent.hpp \
    ../../include/Grapevine/SceneExtensions/BoundingVolumeComponentPrivate.hpp \
    ../../include/Grapevine/SceneExtensions/SimpleScene.hpp \
    ../../include/Grapevine/SceneExtensions/SceneExtensions.hpp


