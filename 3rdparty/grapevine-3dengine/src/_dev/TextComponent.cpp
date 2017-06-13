/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/TextComponent.hpp"
#include "Grapevine/SceneExtensions/TextComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            TextComponent::TextComponent(RichText const& richText):
                Component(new TextComponentPrivate(richText))
            {
                componentPrivate_ = dynamic_cast<TextComponentPrivate*>(private_.pointer());
            }

            TextComponent::TextComponent(TextComponent const &r):
                Component(r)
            {
                componentPrivate_ = r.componentPrivate_;
            }

            TextComponent::~TextComponent()
            {
            }

            TextComponent &TextComponent::operator=(TextComponent const &r)
            {
                Component::operator=(r);
                componentPrivate_ = r.componentPrivate_;
                return *this;
            }

            int TextComponent::glyphCount() const
            {
                return componentPrivate_->glyphCount();
            }

            Vector2 TextComponent::glyphPosition(int index) const
            {
                return componentPrivate_->glyphPosition(index);
            }

            float TextComponent::glyphScale(int index) const
            {
                return componentPrivate_->glyphScale(index);
            }

            void TextComponent::setGlyphTransform(int index, Matrix4 const &tm)
            {
                componentPrivate_->setGlyphTransform(index, tm);
            }

            Matrix4 TextComponent::glyphTransform(int index)
            {
                return componentPrivate_->glyphTransform(index);
            }

            void TextComponent::disableGlyphTransform(int index)
            {
                componentPrivate_->disableGlyphTransform(index);
            }

            int TextComponent::lineCount() const
            {
                return componentPrivate_->lineCount();
            }

            float TextComponent::lineOffset(int index) const
            {
                return componentPrivate_->lineOffset(index);
            }

            void TextComponent::boundingBox(Vector2& min, Vector2& max)
            {
                componentPrivate_->boundingBox(min, max);
            }

            void TextComponent::setColor(Vector4 const& color)
            {
                componentPrivate_->setColor(color);
            }
        }
    }
}
