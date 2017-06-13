//!

#ifndef GRAPEVINE_HELLO4_MAINVIEW
#define GRAPEVINE_HELLO4_MAINVIEW

#include <Grapevine/SceneGraph/Node.hpp>
#include <Grapevine/SceneGraph/TextNode.hpp>
#include <Grapevine/SceneGraph/Scene.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>
#include <Grapevine/Text/FontManager.hpp>
#include <list>

namespace Grapevine
{
    /**
     * @class TransformNode
     *
     * TransformNode is a helper class for hierarchical transforms in SceneGraph.
     */
    class TransformNode : public AbstractNode
    {
    public:
        CLASS_COPYABLE(TransformNode);

        /**
         * Creates node with given location.
         *
         * @param [in]  location    Location to place node to.
         */
        TransformNode(Vector3 const& location = Vector3());
    };
}

class MainView
{
public:
    MainView(unsigned int width, unsigned int height);
    ~MainView();
    bool step();

    Grapevine::InputSource* inputSource();

private:
    float getKerning(Grapevine::RichText const& left, Grapevine::RichText const& right);
    Grapevine::Text createRandomText(int glyphCount);
    void createTextRow();
    void changeRandomGlyph();
    void animateGlyphs();
    void input(bool& quitApplication);
    void physics();
    void draw();

    Grapevine::Camera camera_;
    Grapevine::Canvas canvas_;
    Grapevine::Scene scene_;
    Grapevine::InputSource inputSource_;
    Grapevine::TransformNode textScroll_;
    Grapevine::FpsCounter fpsCounter_;
    Grapevine::FontManager fontManager_;
    std::vector<std::string> fontFiles_;
    unsigned int width_;
    unsigned int height_;

    float zoom;
    float zoomDelta;
    float scrollSpeed;
    float scrollSpeedDelta;

    struct TextElement
    {
        Grapevine::RichText richText;
        Grapevine::TextNode textNode;
        Grapevine::TransformNode advanceWidth;
    };

    struct RowElement
    {
        Grapevine::TransformNode root;

        std::vector<TextElement> textElements;
    };
    std::list<RowElement> rowElements_;
    Grapevine::Vector3 rowLocation_;
};

#endif
