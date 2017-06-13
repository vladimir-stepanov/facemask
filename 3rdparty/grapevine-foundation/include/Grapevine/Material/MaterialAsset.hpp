/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATERIAL_MATERIALASSET
#define GRAPEVINE_MATERIAL_MATERIALASSET

#include "Grapevine/Core/ReferencedPointer.hpp"
#include "Grapevine/Material/TextureAsset.hpp"
#include "Grapevine/Material/CubemapAsset.hpp"

namespace Grapevine
{
    class MaterialResource;
    class ShaderProgram;

    /**
     * @class MaterialAsset
     *
     * MaterialAsset combines textures, shading etc. options for surfaces.
     *
     * @note some of the constructors create "effect" material (see below)
     */
    class MaterialAsset
    {
    public:
        CLASS_COPYABLE(MaterialAsset);

        struct BlendMode
        {
            enum Enum
            {
                One_SourceAlpha,
                SourceAlpha_OneMinusSourceAlpha_One_OneMinusSourceAlpha
            };
        };

        /**
         * Custom shader program
         *
         * Render using custom shader program
         *
         * @param [in]  shaderProgram
         */
        MaterialAsset(ShaderProgram& shaderProgram);
        explicit MaterialAsset(const char* colorMap, ShaderProgram& shaderProgram);
        explicit MaterialAsset(uint32_t colorMapEGLTextureId, ShaderProgram& shaderProgram);

        /**
         * "Normal" materials.
         *
         * @note: These constructors assume that you are defining a shader in your Renderer!
         */
        MaterialAsset(const char* colorMap, float specularFactor, float specularGlossiness);
        MaterialAsset(const char* colorMap, const char* normalMap, float specularFactor, float specularGlossiness);
        MaterialAsset(const char* colorMap, float reflectionStrength, float specularFactor, float specularGlossiness);
        MaterialAsset(const char* colorMap, const char* normalMap, float reflectionStrength, float specularFactor, float specularGlossiness);
        MaterialAsset(TextureAsset& colorMap, float specularFactor, float specularGlossiness);
        MaterialAsset(TextureAsset& colorMap, float reflectionStrength, float specularFactor, float specularGlossiness);

        /**
         * PlanarReflection
         *
         * "Reflective floor" like effect can be done using this material.
         *
         * @note: These constructors assume that you are defining a shader in your Renderer!
         *
         * @param [in]  colorMap
         * @param [in]  reflectionStrength
         * @param [in]  specularFactor
         * @param [in]  specularGlossiness
         * @param [in]  opacityStart        Opacity goes down linearly. This is the start value for the opacity.
         * @param [in]  yUsage              yUsage from the texture. For example, if your reflection is only half from the original image the yUsage is then 0.3f.
         */
        MaterialAsset(TextureAsset& colorMap, float reflectionStrength, float specularFactor, float specularGlossiness, float opacityStart, float yUsage);

        /**
         * Gradient
         *
         * "Gradient" effect in texture y direction using given colors.
         *
         * @note: These constructors assume that you are defining a shader in your Renderer!
         *
         * @param [in]  startColor
         * @param [in]  endColor
         */
        MaterialAsset(Vector4 const& startColor, Vector4 const& endColor);

        /**
         * Shadow
         *
         * @note: This constructor assumes that you are defining a shader in your Renderer!
         *
         *  "Shadow" effect can be used for shadows or glow effects.
         * Use of rectangular mesh is recommended with this material.
         *
         * 'a' is the amount from total side length that is having
         * the gradient from 1.0 to 0.0 (at the edge)
         *
         * The 'b' is area where the color is used with factor 1.0
         *
         * <a>______\/
         * |_|___|_| a
         * | |   | |/\
         * | | b | |
         * |_|___|_|
         * |_|___|_|
         *
         *
         * @param [in]  color
         * @param [in]  gradientSize is the 'a' described above.
         */
        MaterialAsset(Vector4 const& color, float gradientUsage = 0.5f);

        /**
         * Alpha burn
         *
         * @note: This constructor assumes that you are defining a shader in your Renderer!
         *
         * "Alpha burn" effect to given point with given radius
         *
         * @param [in]  colorMap
         * @param [in]  uvBurnCenter    center uv coordinate of the burn                            (set 0.0f, 0.0f for starters if no effect wanted at start)
         * @param [in]  uvRadius        radius of the burn (outer edge where "solid" starts)        (set 0.0f for starters if no effect wanted at start)
         * @param [in]  uvGradientWidth width of the smoothening at burn edge (inside burn radius)  (set 0.0f for starters if no effect wanted at start)
         */
        MaterialAsset(TextureAsset& colorMap, Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth);

        ~MaterialAsset();
        MaterialResource* resource();
        void setColorMap(TextureAsset &colorMap);

        /**
         * Set reflection factor to material.
         *
         * @param [in]  value   Value between 0.0 and 1.0 to be the opacity.
         */
        void setReflectionFactor(float value);

        /**
         * Reset shadow for the material.
         */
        void setShadow(Vector4 const& color, float gradientUsage);

        /**
         * Reset alpha burn for the material.
         */
        void setAlphaBurn(Vector2 const& uvBurnCenter, float uvRadius, float uvGradientWidth);

        void setBlendMode(BlendMode::Enum blendMode);

        void setHWLayerId(int64_t id);
        int64_t hwLayerId() const;

        void setDepthWrites(bool enable);
        bool isDepthWrites();
    private:
        ReferencedPointer<MaterialResource> resource_;
        friend class ModelResource;
        MaterialAsset(std::string const& name, class ModelData* modelData);
    };
}

#endif
