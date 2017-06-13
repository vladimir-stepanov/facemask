/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Barrel.hpp"

namespace Grapevine
{
    std::string Barrel::FileTagStart    = "[BARRELSTART]";
    std::string Barrel::FileTagEnd      = "[BARRELEND]";
    std::string Barrel::FileTagMesh     = "[MESH]";
    std::string Barrel::FileTagMaterial = "[MATERIAL]";

    Barrel::Barrel()
    {
    }

    Barrel::~Barrel()
    {
    }
}
