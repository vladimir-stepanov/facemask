/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_TEXTCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_TEXTCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include "Grapevine/Scene/Component.hpp"
#include "Grapevine/SceneExtensions/TextComponentPrivate.hpp"
#include <Grapevine/Text/RichText.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class TextComponent: public Component
            {
            public:
                CLASS_COPYABLE(TextComponent)

                TextComponent(RichText const& richText);
                virtual ~TextComponent();
                int glyphCount() const;
                Vector2 glyphPosition(int index) const;
                float glyphScale(int index) const;
                void setGlyphTransform(int index, Matrix4 const &tm);
                Matrix4 glyphTransform(int index);
                void disableGlyphTransform(int index);
                int lineCount() const;
                float lineOffset(int index) const;
                void boundingBox(Vector2& min, Vector2& max);
                void setColor(Vector4 const& color);

            private:
                TextComponentPrivate* componentPrivate_;
            };
        }
    }
}

#endif
