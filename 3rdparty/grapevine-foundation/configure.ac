# autoconf
AC_PREREQ(2.59)
AC_INIT([grapevine-foundation], [2.0], [antti.peuhkurinen@huawei.com])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([config.h])
AM_PROG_AR
AC_PROG_LIBTOOL
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)

### TARGETS - START

AC_ARG_ENABLE([desktoplinux],
  [AS_HELP_STRING([--disable-desktoplinux], [Enable xegl + gles2 platform @<:@check@:>@])],
  [:],
  [enable_gui=check])

# Set flags according to GRAPEVINE_PLATFORM
if [test "$enable_desktoplinux" != "no"]; then
    GRAPEVINE_CFLAGS="-DDESKTOPLINUX_XEGL_GLES2"
    DEPENDENCIES="glib-2.0 >= 2.24 glesv2 egl sigc++-2.0 >= 2.2 libpng"
    AM_CONDITIONAL(GL_HPP, true)
    AM_CONDITIONAL(USE_GLES2, true)
    AM_CONDITIONAL(USE_XINPUT2, true)
    PKG_LDFLAGS="-lgrapevine-foundation -lpthread -lglib-2.0 -lrt -lpng -ljpeg -lsigc-2.0 -lrt -lGLESv2 -lX11"
    PKG_CFLAGS=""
else
    AM_CONDITIONAL(GL_HPP, false)
    AM_CONDITIONAL(USE_GLES2, false)
    AM_CONDITIONAL(USE_XINPUT2, false)
    PKG_LDFLAGS="-lgrapevine-foundation -lpthread -lglib-2.0 -lrt -lpng -ljpeg -lsigc-2.0 -lrt"
    PKG_CFLAGS=""
fi
AC_SUBST(GRAPEVINE_CFLAGS)

AM_CONDITIONAL([DESKTOPLINUX_XEGL_GLES2], [test "$enable_desktoplinux" = "yes"])

AC_ARG_ENABLE(eglnative,
    [--enable-eglnative Enable egl + gles2 platform],
    [NATIVE_EGL_GLES2="yes"],
    [NATIVE_EGL_GLES2="no"]
)
if test "x$NATIVE_EGL_GLES2" == "xyes"; then
    GRAPEVINE_CFLAGS="-DNATIVE_EGL_GLES2 -DTSAIKKA"
    DEPENDENCIES="glib-2.0 >= 2.24 glesv2 egl sigc++-2.0 >= 2.2 libpng hwcmanager hwcomposer-egl hybris-egl-platform"
    AM_CONDITIONAL(GL_HPP, true)
    AM_CONDITIONAL(USE_GLES2, true)
    AM_CONDITIONAL(USE_XINPUT2, false)
    PKG_LDFLAGS="-lgrapevine-foundation -lpthread -lglib-2.0 -lrt -lpng -ljpeg -lsigc-2.0 -lrt -lGLESv2"
    PKG_CFLAGS="-DTSAIKKA"
fi
AC_SUBST(GRAPEVINE_CFLAGS)
AM_CONDITIONAL([NATIVE_EGL_GLES2], [test "x$NATIVE_EGL_GLES2" = "xyes"])
### TARGETS - END

# For Makefile.am lib_LTLIBRARIES
LT_INIT

# C++ source
AC_PROG_CXX

# Automake
AM_INIT_AUTOMAKE([1.11 -Wall foreign no-define])
AM_SILENT_RULES([yes])

# Get the dependencies, use _CFLAGS and _LIBS later Makefile.ams
PKG_CHECK_MODULES(DEPENDENCIES, "$DEPENDENCIES")
AC_SUBST(DEPENDENCIES)
AC_SUBST(DEPENDENCIES_CFLAGS)
AC_SUBST(DEPENDENCIES_LIBS)

GRAPEVINE_CFLAGS="$GRAPEVINE_CFLAGS $DEPENDENCIES_CFLAGS"
AC_CONFIG_FILES([Makefile
        src/Makefile
        src/Core/Makefile
        src/Resource/Makefile
        src/Math/Makefile
        src/Benchmark/Makefile
        src/Canvas/Makefile
        src/GPU/Makefile
        src/Image/Makefile
        src/Input/Makefile
        src/Material/Makefile
        src/Model/Makefile
        src/Shader/Makefile
        src/StateMachine/Makefile
        src/Animation/Makefile
        src/Arrange/Makefile
        grapevine-foundation.pc
        grapevine-foundation-xegl_gles2.pc])
AC_SUBST(DEPENDENCIES)
AC_SUBST(PKG_LDFLAGS)
AC_SUBST(PKG_CFLAGS)
AC_OUTPUT
