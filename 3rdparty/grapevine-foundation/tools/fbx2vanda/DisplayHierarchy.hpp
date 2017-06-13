#ifndef BARREL_DISPLAY_HIERARCHY_H
#define BARREL_DISPLAY_HIERARCHY_H

#include "DisplayCommon.hpp"

void DisplayHierarchy(FbxScene* pScene);
void DisplayHierarchy(FbxNode* pNode, int pDepth);

#endif
