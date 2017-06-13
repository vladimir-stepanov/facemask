#ifndef BARREL_DISPLAY_TEXTURE_H
#define BARREL_DISPLAY_TEXTURE_H

#include "DisplayCommon.hpp"

void FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex);
void DisplayTexture(FbxGeometry* pGeometry);
void DisplayTextureInfo(FbxTexture* pTexture, int pBlendMode);

#endif
