/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/TextComponentPrivate.hpp"
#include <Grapevine/Shader/ShaderProgram.hpp>
#include <../src/kaski/font.hpp>
#include <../src/kaski/math.hpp>
#include <../src/kaski/text.hpp>
#include <Grapevine/Animation/AnimationManager.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            ShaderProgram* TextComponentPrivate::shaderProgram_ = NULL;

            TextComponentPrivate::TextComponentPrivate(RichText const& richText):
                AbstractComponentPrivateTemplate<TextComponentPrivate>("TextComponent"),
                richText_(richText),
                kaskiText_(new kaski::Text())
            {
                init();
            };

            TextComponentPrivate::~TextComponentPrivate()
            {
                delete kaskiText_;
                kaskiText_ = NULL;
            }

            int TextComponentPrivate::glyphCount() const
            {
                return kaskiText_->getGlyphCount();
            }

            Vector2 TextComponentPrivate::glyphPosition(int index) const
            {
                math::vec2 const& position = kaskiText_->getGlyphPosition(index);
                return Vector2(position.x, position.y);
            }

            float TextComponentPrivate::glyphScale(int index) const
            {
                GlyphInstance const& gi = kaskiText_->getGlyphInstance(index);
                return gi.scale;
            }

            void TextComponentPrivate::setGlyphTransform(int index, Matrix4 const &tm)
            {
                kaskiText_->setTransform(index, tm);
                AnimationManager::setSceneDirty();
            }

            Matrix4 TextComponentPrivate::glyphTransform(int index) const
            {
                float data[16];
                kaskiText_->getTransform(index, data);

                return Matrix4(data);
            }

            void TextComponentPrivate::disableGlyphTransform(int index)
            {
                kaskiText_->setTransform(index, NULL);
                AnimationManager::setSceneDirty();
            }

            int TextComponentPrivate::lineCount() const
            {
                return kaskiText_->getLineCount();
            }

            float TextComponentPrivate::lineOffset(int index) const
            {
                return kaskiText_->getLineOffset(index);
            }

            void TextComponentPrivate::init()
            {
                size_t count = richText_.size();
                if (count == 0)
                {
                    // no text
                    return;
                }

                // use color from first text
                setColor(richText_.text(0).color());

                for (size_t i = 0; i < count; ++i)
                {
                    Text const& lText = richText_.text(i);

                    std::wstring const& text = lText.text();
                    FontAsset const& fontAsset = lText.fontAsset();
                    unsigned int size = lText.size();
                    //Vector4 const& color = lText.color();

                    kaskiText_->write(fontAsset, size, text);

                    /*if (colorGroups_.empty())
                    {
                        // no entries yet, add first entry
                        ColorGroup cg;
                        cg.startIndex = 0;
                        cg.count = kaskiText_->getIndex();
                        cg.color = color;
                        colorGroups_.push_back(cg);
                    }
                    else
                    {
                        ColorGroup &cg = colorGroups_.back();
                        if (cg.color[0] != color[0] || cg.color[1] != color[1] || cg.color[2] != color[2] || cg.color[3] != color[3])
                        {
                            // add new entry
                            ColorGroup cgNew;
                            cgNew.startIndex = cg.startIndex + cg.count;
                            cgNew.count = kaskiText_->getIndex() - cgNew.startIndex;
                            cgNew.color = color;
                            colorGroups_.push_back(cgNew);
                        }
                        else
                        {
                            // update previous entry's count to include newly added text
                            cg.count = kaskiText_->getIndex() - cg.startIndex;
                        }
                    }*/
                }

                kaskiText_->linefeed();
                AnimationManager::setSceneDirty();
            }

            void TextComponentPrivate::boundingBox(Vector2& min, Vector2& max) const
            {
                math::vec2 vmin, vmax;
                kaskiText_->getBoundingBox(vmin, vmax);

                min = Vector2(vmin.x, vmin.y);
                max = Vector2(vmax.x, vmax.y);
            }

            float TextComponentPrivate::boundingSphereRadius() const
            {
                // Font component bounding sphere
                int count = glyphCount();
                Vector2 position = glyphPosition(count - 1);
                float r = position.length();
                return r;
            }

            void TextComponentPrivate::setColor(Vector4 const& color)
            {
                // reset new color to all of the glyphs
                kaskiText_->setColor(color.r(), color.g(), color.b(), color.a());
                AnimationManager::setSceneDirty();
            }

            bool TextComponentPrivate::hasAlpha()
            {
                return true;
            }
        }
    }
}
