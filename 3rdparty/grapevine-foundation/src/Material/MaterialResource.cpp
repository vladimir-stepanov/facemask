/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Material/MaterialResource.hpp"
#include "../Model/ModelData.hpp"
#include <cinttypes>

namespace Grapevine
{
    MaterialResource::MaterialResource(float specularFactor, float specularGlossiness):
        colorMap_(ReferencedPointer<TextureResource>()),
        normalMap_(ReferencedPointer<TextureResource>()),
        specularFactor_(specularFactor),
        specularGlossiness_(specularGlossiness),
        reflectionFactor_(0.0f),
        opacity_(1.0f),
        highlight_(0.0f),
        effect_(Effect::NoEffect),
        startOpacity_(1.0f),
        yUsage_(1.0f),
        gradientStart_(Vector4()),
        gradientEnd_(Vector4()),
        blendMode_(MaterialAsset::BlendMode::SourceAlpha_OneMinusSourceAlpha_One_OneMinusSourceAlpha),
        shaderProgram_(NULL)
    {
    }

    MaterialResource::MaterialResource(const std::string& name, ModelData* modelData):
        colorMap_(ReferencedPointer<TextureResource>()),
        normalMap_(ReferencedPointer<TextureResource>()),
        specularFactor_(0.0f),
        specularGlossiness_(0.0f),
        reflectionFactor_(0.0f),
        opacity_(1.0f),
        highlight_(0.0f),
        effect_(Effect::NoEffect),
        startOpacity_(1.0f),
        yUsage_(1.0f),
        gradientStart_(Vector4()),
        gradientEnd_(Vector4()),
        blendMode_(MaterialAsset::BlendMode::SourceAlpha_OneMinusSourceAlpha_One_OneMinusSourceAlpha),
        shaderProgram_(NULL)
    {
        ModelData::Material &material = modelData->materials_[0];
        if (!material.colorMapName.empty())
        {
            TextureAsset colorMapAsset(material.colorMapName);
            setColorMap(ReferencedPointer<TextureResource>(colorMapAsset.resource()));
        }
        if (!material.normalMapName.empty())
        {
            TextureAsset normalMapAsset(material.normalMapName);
            setNormalMap(ReferencedPointer<TextureResource>(normalMapAsset.resource()));
        }
        setSpecularFactor(material.specularFactor);
        setSpecularGlossiness(material.specularGlossiness);
        setReflectionFactor(material.reflectionFactor);
    }

    MaterialResource::~MaterialResource()
    {
    }

    void MaterialResource::setColorMap(ReferencedPointer<TextureResource> colorMap)
    {
        colorMap_ = colorMap;
    }

    void MaterialResource::setNormalMap(ReferencedPointer<TextureResource> normalMap)
    {
        normalMap_ = normalMap;
    }

    void MaterialResource::setReflectionFactor(float value)
    {
        reflectionFactor_ = value;
    }

    void MaterialResource::setSpecularFactor(float value)
    {
        specularFactor_ = value;
    }

    void MaterialResource::setSpecularGlossiness(float value)
    {
        specularGlossiness_ = value;
    }

    void MaterialResource::setEffect(Effect::Enum effect)
    {
        effect_ = effect;
    }

    void MaterialResource::setStartOpacity(float value)
    {
        startOpacity_ = value;
    }

    void MaterialResource::setYUsage(float value)
    {
        yUsage_ = value;
    }

    void MaterialResource::setGradient(Vector4 const& start, Vector4 const& end)
    {
        gradientStart_ = start;
        gradientEnd_ = end;
    }

    void MaterialResource::setShadow(Vector4 const& color, float gradientUsage)
    {
        shadowColor_ = color;
        shadowGradientUsage_ = gradientUsage;
    }

    ReferencedPointer<TextureResource> MaterialResource::colorMap() const
    {
        return colorMap_;
    }

    ReferencedPointer<TextureResource> MaterialResource::normalMap() const
    {
        return normalMap_;
    }

    float MaterialResource::specularFactor() const
    {
        return specularFactor_;
    }

    float MaterialResource::specularGlossiness() const
    {
        return specularGlossiness_;
    }

    float MaterialResource::reflectionFactor() const
    {
        return reflectionFactor_;
    }

    MaterialResource::Effect::Enum MaterialResource::effect() const
    {
        return effect_;
    }

    float MaterialResource::startOpacity() const
    {
        return startOpacity_;
    }

    float MaterialResource::yUsage() const
    {
        return yUsage_;
    }

    Vector4 const& MaterialResource::gradientStart() const
    {
        return gradientStart_;
    }

    Vector4 const& MaterialResource::gradientEnd() const
    {
        return gradientEnd_;
    }

    Vector4 const& MaterialResource::shadowColor() const
    {
        return shadowColor_;
    }

    float MaterialResource::shadowGradientUsage() const
    {
        return shadowGradientUsage_;
    }

    void MaterialResource::setAlphaBurn(Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth)
    {
        alphaBurnCenter_ = uvBurnCenter;
        alphaBurnRadius_ = uvRadius;
        alphaBurnGradientWidth_ = uvGradientWidth;
    }

    Vector2 MaterialResource::alphaBurnCenter() const
    {
        return alphaBurnCenter_;
    }

    float MaterialResource::alphaBurnRadius() const
    {
        return alphaBurnRadius_;
    }

    float MaterialResource::alphaBurnGradientWidth() const
    {
        return alphaBurnGradientWidth_;
    }

    void MaterialResource::setBlendMode(MaterialAsset::BlendMode::Enum blendMode)
    {
        blendMode_ = blendMode;
    }

    MaterialAsset::BlendMode::Enum MaterialResource::blendMode()
    {
        return blendMode_;
    }

    void MaterialResource::setShaderProgram(ShaderProgram& shaderProgram)
    {
        shaderProgram_ = &shaderProgram;
    }

    ShaderProgram* MaterialResource::shaderProgram() const
    {
        return shaderProgram_;
    }

    void MaterialResource::setHWLayerId(int64_t id)
    {
        LOGD("MaterialResource::setHWLayerId %" PRIi64, id);
        hwLayerId_ = id;
    }

    int64_t MaterialResource::hwLayerId() const
    {
        return hwLayerId_;
    }

    void MaterialResource::setDepthWrites(bool enabled)
    {
        depthWrites_ = enabled;
    }

    bool MaterialResource::isDepthWrites() const
    {
        return depthWrites_;
    }
}
