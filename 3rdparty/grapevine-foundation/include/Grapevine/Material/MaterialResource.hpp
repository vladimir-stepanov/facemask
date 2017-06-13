/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_MATERIALRESOURCE
#define GRAPEVINE_MATERIAL_MATERIALRESOURCE

#include <Grapevine/Core/ReferencedPointer.hpp>
#include "Grapevine/Material/MaterialAsset.hpp"
#include "Grapevine/Material/TextureAsset.hpp"
#include "Grapevine/Material/CubemapAsset.hpp"
#include "Grapevine/Material/TextureResource.hpp"
#include "Grapevine/Material/CubemapResource.hpp"
#include <Grapevine/Math/Vector.hpp>

namespace Grapevine
{
    class TextureResource;
    class CubemapResource;

    class MaterialResource: public Referenced
    {
    public:
        struct Effect
        {
            enum Enum
            {
                NoEffect,
                PlanarReflection,
                Gradient,
                Shadow,
                AlphaBurn
            };
        };

        ReferencedPointer<TextureResource> colorMap() const;
        ReferencedPointer<TextureResource> normalMap() const;
        float specularFactor() const;
        float specularGlossiness() const;
        float reflectionFactor() const;
        void setReflectionFactor(float value);

        // TODO: later separate effect class is needed for next effect related lines:
        Effect::Enum effect() const;
        float startOpacity() const;
        float yUsage() const;
        Vector4 const& gradientStart() const;
        Vector4 const& gradientEnd() const;
        Vector4 const& shadowColor() const;
        float shadowGradientUsage() const;
        void setAlphaBurn(Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth);
        Vector2 alphaBurnCenter() const;
        float alphaBurnRadius() const;
        float alphaBurnGradientWidth() const;
        void setBlendMode(MaterialAsset::BlendMode::Enum blendMode);
        MaterialAsset::BlendMode::Enum blendMode();
        ShaderProgram* shaderProgram() const;

    private:
        friend class MaterialAsset;
        friend class ModelData;

        MaterialResource(float specularFactor = 0.0f, float specularGlossiness = 0.0f);
        MaterialResource(std::string const& name, class ModelData* modelData);
        ~MaterialResource();

        void setColorMap(ReferencedPointer<TextureResource> colorMap);
        void setNormalMap(ReferencedPointer<TextureResource> normalMap);
        void setSpecularFactor(float value);
        void setSpecularGlossiness(float value);

        // TODO: later separate effect class is needed for next effect related lines:
        void setEffect(Effect::Enum effect);
        void setStartOpacity(float);
        void setYUsage(float);
        void setGradient(Vector4 const& start, Vector4 const& end);
        void setShadow(Vector4 const& color, float gradientUsage);

        void setShaderProgram(ShaderProgram& shaderProgram);

        void setHWLayerId(int64_t id);
        int64_t hwLayerId() const;

        void setDepthWrites(bool enabled);
        bool isDepthWrites() const;

        ReferencedPointer<TextureResource> colorMap_;
        ReferencedPointer<TextureResource> normalMap_;
        float specularFactor_;
        float specularGlossiness_;
        float reflectionFactor_;
        float opacity_;
        float highlight_;

        // TODO: later separate effect class is needed for next effect related lines:
        Effect::Enum effect_;
        float startOpacity_;
        float yUsage_;
        Vector4 gradientStart_;
        Vector4 gradientEnd_;
        Vector4 shadowColor_;
        float shadowGradientUsage_;
        Vector2 alphaBurnCenter_;
        float alphaBurnRadius_;
        float alphaBurnGradientWidth_;

        MaterialAsset::BlendMode::Enum blendMode_;

        ShaderProgram* shaderProgram_;
        int64_t hwLayerId_ = 0;
        bool depthWrites_ = false;
    };
}

#endif
