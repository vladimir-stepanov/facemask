/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Material/MaterialAsset.hpp"
#include "Grapevine/Material/MaterialResource.hpp"

namespace Grapevine
{
    MaterialAsset::MaterialAsset(const char* colorMap, ShaderProgram& shaderProgram):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        TextureAsset colorMapAsset(colorMap);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        resource_.pointer()->setShaderProgram(shaderProgram);
    }

    MaterialAsset::MaterialAsset(uint32_t colorMapEGLTextureId, ShaderProgram& shaderProgram):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        TextureAsset colorMapAsset(colorMapEGLTextureId);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        resource_.pointer()->setShaderProgram(shaderProgram);
    }

    MaterialAsset::MaterialAsset(ShaderProgram& shaderProgram):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        resource_.pointer()->setShaderProgram(shaderProgram);
    }

    MaterialAsset::MaterialAsset(const char* colorMap, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        TextureAsset colorMapAsset(colorMap);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
    }

    MaterialAsset::MaterialAsset(const char* colorMap, const char* normalMap, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        TextureAsset colorMapAsset(colorMap);
        TextureAsset normalMapAsset(normalMap);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        resource_.pointer()->setNormalMap(ReferencedPointer<TextureResource>(normalMapAsset.resource()));
    }

    MaterialAsset::MaterialAsset(const char* colorMap, float reflectionFactor, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        TextureAsset colorMapAsset(colorMap);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        resource_.pointer()->setReflectionFactor(reflectionFactor);
    }

    MaterialAsset::MaterialAsset(const char* colorMap, const char* normalMap, float reflectionFactor, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        TextureAsset colorMapAsset(colorMap);
        TextureAsset normalMapAsset(normalMap);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        resource_.pointer()->setNormalMap(ReferencedPointer<TextureResource>(normalMapAsset.resource()));
        resource_.pointer()->setReflectionFactor(reflectionFactor);
    }

    MaterialAsset::MaterialAsset(TextureAsset& colorMap, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
    }

    MaterialAsset::MaterialAsset(TextureAsset& colorMap, float reflectionFactor, float specularFactor, float specularGlossiness):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
        resource_.pointer()->setReflectionFactor(reflectionFactor);
    }

    MaterialAsset::MaterialAsset(std::string const& name, class ModelData* modelData):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(name, modelData)))
    {
    }

    MaterialAsset::MaterialAsset(TextureAsset& colorMap, float reflectionFactor, float specularFactor, float specularGlossiness, float opacityStart, float yUsage):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(specularFactor, specularGlossiness)))
    {
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
        resource_.pointer()->setEffect(MaterialResource::Effect::PlanarReflection);
        resource_.pointer()->setReflectionFactor(reflectionFactor);
        resource_.pointer()->setStartOpacity(opacityStart);
        resource_.pointer()->setYUsage(yUsage);
    }

    MaterialAsset::MaterialAsset(Vector4 const& startColor, Vector4 const& endColor):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        resource_.pointer()->setEffect(MaterialResource::Effect::Gradient);
        resource_.pointer()->setGradient(startColor, endColor);
    }

    MaterialAsset::MaterialAsset(Vector4 const& color, float gradientUsage):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        resource_.pointer()->setEffect(MaterialResource::Effect::Shadow);
        resource_.pointer()->setShadow(color, gradientUsage);
    }

    MaterialAsset::MaterialAsset(TextureAsset& colorMap, Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth):
        resource_(ReferencedPointer<MaterialResource>(new MaterialResource(0.0f, 0.0f)))
    {
        resource_.pointer()->setEffect(MaterialResource::Effect::AlphaBurn);
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
        resource_.pointer()->setAlphaBurn(uvBurnCenter, uvRadius, uvGradientWidth);
    }

    MaterialAsset::MaterialAsset(MaterialAsset const& r):
        resource_(r.resource_)
    {
    }

    MaterialAsset::~MaterialAsset()
    {
    }

    MaterialAsset& MaterialAsset::operator=(MaterialAsset const& r)
    {
        if (this != &r)
        {
            resource_ = r.resource_;
        }
        return *this;
    }

    MaterialResource* MaterialAsset::resource()
    {
        return resource_.pointer();
    }

    void MaterialAsset::setColorMap(TextureAsset &colorMap)
    {
        resource_.pointer()->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
    }

    void MaterialAsset::setReflectionFactor(float value)
    {
        resource_.pointer()->setReflectionFactor(value);
    }

    void MaterialAsset::setShadow(Vector4 const& color, float gradientUsage)
    {
        resource_.pointer()->setShadow(color, gradientUsage);
    }

    void MaterialAsset::setAlphaBurn(Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth)
    {
        resource_.pointer()->setAlphaBurn(uvBurnCenter, uvRadius, uvGradientWidth);
    }

    void MaterialAsset::setBlendMode(BlendMode::Enum blendMode)
    {
        resource_.pointer()->setBlendMode(blendMode);
    }

    void MaterialAsset::setHWLayerId(int64_t id)
    {
        resource_.pointer()->setHWLayerId(id);
    }

    int64_t MaterialAsset::hwLayerId() const
    {
        return resource_.pointer()->hwLayerId();
    }

    void MaterialAsset::setDepthWrites(bool enable)
    {
        resource_.pointer()->setDepthWrites(enable);
    }

    bool MaterialAsset::isDepthWrites()
    {
        return resource_.pointer()->isDepthWrites();
    }
}
