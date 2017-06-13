bin_PROGRAMS = fbx2vanda
fbx2vanda_SOURCES = main.cpp \
    Exporter.cpp \
    Converter.cpp \
    FBX2Vanda/ControlPoint.cpp \
    FBX2Vanda/Mesh.cpp \
    FBX2Vanda/Material.cpp \ \
    DisplayAnimation.cpp \
    DisplayCamera.cpp \
    DisplayCommon.cpp \
    DisplayGenericInfo.cpp \
    DisplayGlobalSettings.cpp \
    DisplayHierarchy.cpp \
    DisplayLight.cpp \
    DisplayLink.cpp \
    DisplayLodGroup.cpp \
    DisplayMarker.cpp \
    DisplayMesh.cpp \
    DisplayPivotsAndLimits.cpp \
    DisplayPose.cpp \
    DisplayShape.cpp \
    DisplaySkeleton.cpp \
    DisplayTexture.cpp \
    DisplayUserProperties.cpp

fbx2vanda_LDADD = $(GRAPEVINE_DEPENDENCIES_LIBS) -L/usr/lib/gcc4/x64/release -lfbxsdk
fbx2vanda_CPPFLAGS = $(GRAPEVINE_DEPENDENCIES_CFLAGS)
