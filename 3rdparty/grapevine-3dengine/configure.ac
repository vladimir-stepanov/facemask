# autoconf
AC_PREREQ(2.59)
AC_INIT([grapevine-3dengine], [2.0], [antti.peuhkurinen@huawei.com])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([config.h])
AM_PROG_AR
AC_PROG_LIBTOOL
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)

### TARGETS - START

DEPENDENCIES="grapevine"

# Set
AC_ARG_ENABLE(desktoplinux,
    [--enable-desktoplinux   Enable xegl + gles2 platform],
    [DESKTOPLINUX_XEGL_GLES2="yes"],
    [DESKTOPLINUX_XEGL_GLES2="no"]
)

# Set flags according to GRAPEVINE_PLATFORM
if test "x$DESKTOPLINUX_XEGL_GLES2" == "xyes"; then
    GRAPEVINE_CFLAGS="-DDESKTOPLINUX_XEGL_GLES2"
    DEPENDENCIES="glib-2.0 >= 2.24 glesv2 egl sigc++-2.0 >= 2.2 libpng12"
    AM_CONDITIONAL(GL_HPP, true)
    AM_CONDITIONAL(USE_GLES2, true)
    AM_CONDITIONAL(USE_XINPUT2, true)
fi
AC_SUBST(GRAPEVINE_CFLAGS)

AM_CONDITIONAL([DESKTOPLINUX_XEGL_GLES2], [test "x$DESKTOPLINUX_XEGL_GLES2" = "xyes"])

AC_ARG_ENABLE(eglnative,
    [--enable-eglnative Enable egl + gles2 platform],
    [NATIVE_EGL_GLES2="yes"],
    [NATIVE_EGL_GLES2="no"]
)
if test "x$NATIVE_EGL_GLES2" == "xyes"; then
    GRAPEVINE_CFLAGS="-DNATIVE_EGL_GLES2"
    DEPENDENCIES="glib-2.0 >= 2.24 glesv2 egl sigc++-2.0 >= 2.2 libpng12"
    AM_CONDITIONAL(GL_HPP, true)
    AM_CONDITIONAL(USE_GLES2, true)
    AM_CONDITIONAL(USE_XINPUT2, false)
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
AC_SUBST(DEPENDENCIES_CFLAGS)
AC_SUBST(DEPENDENCIES_LIBS)

GRAPEVINE_CFLAGS="$GRAPEVINE_CFLAGS $DEPENDENCIES_CFLAGS"
AC_CONFIG_FILES([Makefile
        src/Makefile
        src/Scene/Makefile
        src/SceneExtensions/Makefile
        grapevine-3dengine.pc])
AC_OUTPUT
