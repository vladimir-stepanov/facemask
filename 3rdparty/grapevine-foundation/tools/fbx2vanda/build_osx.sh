#!/bin/sh

c++ -framework Cocoa -I/Applications/Autodesk/FBX\ SDK/2014.2/include \
    -I. -I../../include `pkg-config --cflags sigc++-2.0` \
    Exporter.cpp \
    Converter.cpp \
    Barrel/ControlPoint.cpp \
    Barrel/Mesh.cpp \
    Barrel/Material.cpp \
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
    DisplayUserProperties.cpp \
    ../../src/Core/Log.cpp \
    /Applications/Autodesk/FBX\ SDK/2014.2/lib/clang/ub/debug/libfbxsdk.a \
    -o barrel main.cpp