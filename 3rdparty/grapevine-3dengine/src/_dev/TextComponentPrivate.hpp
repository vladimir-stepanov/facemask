
NOTE: NOT IN USE CURRENTLY !

/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_TEXTCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_TEXTCOMPONENTPRIVATE

#ifndef __NGOS__

#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Text/RichText.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <vector>

namespace kaski
{
    class Text;
}

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class TextComponentPrivate: public AbstractComponentPrivateTemplate<TextComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(TextComponentPrivate)

                TextComponentPrivate(RichText const& richText);
                virtual ~TextComponentPrivate();
                int glyphCount() const;
                Vector2 glyphPosition(int index) const;
                float glyphScale(int index) const;
                void setGlyphTransform(int index, Matrix4 const &tm);
                Matrix4 glyphTransform(int index) const;
                void disableGlyphTransform(int index);
                int lineCount() const;
                float lineOffset(int index) const;
                void boundingBox(Vector2& min, Vector2& max) const;
                void setColor(Vector4 const& color);
                bool hasAlpha();

            private:
                // TODO: cullers should have public API from Text to get the "kaski" text
                friend class CullerBoxVisitor;
                friend class CullerVisitor;
                friend class TextComponent;
                float boundingSphereRadius() const; // <- WILL BE DEPRECATED (when Culler upgraded!)

                void init();
                static class ShaderProgram* shaderProgram_;
                RichText richText_;
                class kaski::Text* kaskiText_;
                float opacity_;
                /*struct ColorGroup
                {
                    Vector4 color;
                    int startIndex;
                    int count;
                };
                std::vector<ColorGroup> colorGroups_;*/
            };
        }
    }
}

#endif
#endif
