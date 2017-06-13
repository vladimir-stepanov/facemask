SUBDIRS = Core Math Resource Benchmark GPU Canvas Image Input Material Model Shader StateMachine Animation Arrange
lib_LTLIBRARIES = libgrapevine-foundation.la
libgrapevine_foundation_la_SOURCES =
libgrapevine_foundation_la_LIBADD = $(DEPENDENCIES_LIBS) \
	Core/libgrapevine-core.la \
	Math/libgrapevine-math.la \
	Resource/libgrapevine-resource.la \
	Benchmark/libgrapevine-benchmark.la \
	GPU/libgrapevine-gpu.la \
	Canvas/libgrapevine-canvas.la \
	Shader/libgrapevine-shader.la \
	Image/libgrapevine-image.la \
	Material/libgrapevine-material.la \
	Input/libgrapevine-input.la \
	Model/libgrapevine-model.la \
	StateMachine/libgrapevine-statemachine.la \
	Animation/libgrapevine-animation.la \
	Arrange/libgrapevine-arrange.la \
	-ljpeg

libgrapevine_foundation_la_LDFLAGS = -module -avoid-version -shared --disable-static

# Includes
library_includedir=$(includedir)/Grapevine/
library_include_HEADERS = ../include/Grapevine/Config.hpp

# Handle Meta.hpp here because it's one header only module
librarymeta_includedir=Grapevine/Meta
librarymeta_include_HEADERS = ../include/Grapevine/Meta/Meta.hpp
