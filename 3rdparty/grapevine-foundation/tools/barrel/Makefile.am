bin_PROGRAMS = barrel
barrel_SOURCES = main.cpp \
    Exporter.cpp \
    Converter.cpp \
    Barrel/ControlPoint.cpp \
    Barrel/Mesh.cpp \
    Barrel/Material.cpp \ \
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

#    DisplayMaterial.cpp
#    DisplayNurb.cpp
#    DisplayPatch.cpp
# /usr/local/fbxsdk/lib/gcc4/x86/libfbxsdk-2013.3-static.a
 barrel_LDADD = $(GRAPEVINE_DEPENDENCIES_LIBS) -L/usr/lib/gcc4/x64/release -lfbxsdk
#barrel_LDADD = $(GRAPEVINE_DEPENDENCIES_LIBS) -L/usr/lib/gcc4/x86/release -lfbxsdk
barrel_CPPFLAGS = $(GRAPEVINE_DEPENDENCIES_CFLAGS)
