noinst_LTLIBRARIES = libgrapevine-shader.la

grapevine_common_sources_SHADER =  ShaderManager.cpp \
    ShaderManagerPrivate.cpp \
    FragmentSource.cpp \
    VertexSource.cpp

if USE_GLES2
libgrapevine_shader_la_SOURCES = $(grapevine_common_sources_SHADER) \
    Shader-GLES2.cpp \
    ShaderProgram-GLES2.cpp \
    ShaderSource-GLES2.cpp \
    ShaderProgramSymbol-GLES2.cpp
endif

libgrapevine_shader_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

library_includedir=$(includedir)/Grapevine/Shader
library_include_HEADERS = \
    ../../include/Grapevine/Shader/FragmentSource.hpp \
    ../../include/Grapevine/Shader/ShaderType.hpp \
    ../../include/Grapevine/Shader/Shader.hpp \
    ../../include/Grapevine/Shader/ShaderProgram.hpp \
    ../../include/Grapevine/Shader/ShaderProgramSymbol.hpp \
    ../../include/Grapevine/Shader/ShaderSource.hpp \
    ../../include/Grapevine/Shader/VertexSource.hpp
