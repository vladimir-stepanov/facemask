//!

#include "MainView.hpp"
#include <Grapevine/Model/MeshAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/SceneGraph/Camera.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Random.hpp>
#include <Grapevine/Math/EasingCurve.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Core/Exception.hpp>
#include "../../src/kaski/font.hpp"
#include <Grapevine/Text/FontManager.hpp>
#include <cmath>

using namespace Grapevine;

#define TEST 4

#if TEST == 1
// test 1
#   define FONT_SIZE 4
#   define GLYPHS_PER_ROW 100
#   define ROWS 50
#   define ROW_HEIGHT 2.5f
#   define DYNAMIC_TEXT false
#elif TEST == 2
// test 2
#   define FONT_SIZE 2
#   define GLYPHS_PER_ROW 200
#   define ROWS 100
#   define ROW_HEIGHT 1.25f
#   define DYNAMIC_TEXT false
#elif TEST == 3
// test 3
#   define FONT_SIZE 8
#   define GLYPHS_PER_ROW 50
#   define ROWS 25
#   define ROW_HEIGHT 5.f
#   define DYNAMIC_TEXT true
#   define CHANGE_GLYPHS_PER_SECOND 1
#elif TEST == 4
// test 4
#   define FONT_SIZE 6
#   define GLYPHS_PER_ROW 66
#   define ROWS 33
#   define ROW_HEIGHT 3.75f
#   define DYNAMIC_TEXT false
#   define ANIMATE_GLYPHS 1
#elif TEST == 5
// test 5
#   define FONT_SIZE 40
#   define GLYPHS_PER_ROW 1
#   define ROWS 1
#   define ROW_HEIGHT 64.f
#   define DYNAMIC_TEXT false
//#   define FORCE_FONT "arial.ttf"
//#   define VALID_GLYPHS "AeHTIO"
//#   define FORCE_FONT "japanese.ttf"
#   define FORCE_FONT "HDZB_75.ttf"
#   define VALID_GLYPHS "J"
#else
#   error "Unknown test case!"
#endif

#define ROWS_PER_SECOND 5

namespace Grapevine
{
    TransformNode::TransformNode(Vector3 const& location):
        AbstractNode(location)
    {
        // do nothing
    }

    TransformNode::TransformNode(TransformNode const& r):
        AbstractNode(r.private_)
    {
        if (this == &r)
        {
            throw Exception("TransformNode constructed from itself!");
        }
    }

    TransformNode& TransformNode::operator=(TransformNode const& r)
    {
        AbstractNode::operator=(r);
        return *this;
    }
}

MainView::MainView(unsigned int width, unsigned int height):
    canvas_(width, height),
    camera_(canvas_, Vector::origo, 45.0f),
    scene_(camera_),
    width_(width),
    height_(height),
    inputSource_(canvas_),
    fontFiles_(FontManager::fontFiles()),
    zoom(0.f),
    zoomDelta(0.f),
    scrollSpeed(1.f),
    scrollSpeedDelta(0.f)
{
    canvas_.setClearColor(Vector4(0.f, 0.f, 0.f, 1.0f));
    scene_.addNode(textScroll_);

    rowLocation_ = Vector3(-100.f, -60.f, -160.f);
    for (int i=0; i<ROWS; ++i)
    {
        createTextRow();
    }
}

MainView::~MainView()
{
}

bool MainView::step()
{
    bool quitApplication = false;
    input(quitApplication);
    physics();
    draw();
    return !quitApplication;
}

InputSource* MainView::inputSource()
{
    return &inputSource_;
}

float MainView::getKerning(Grapevine::RichText const& left, Grapevine::RichText const& right)
{
    if (left.size() > 0 && right.size() > 0)
    {
        Grapevine::Text const& leftText_ = left.text(left.size() - 1);
        Grapevine::Text const& rightText_ = right.text(0);

        FontAsset const& leftFontAsset = leftText_.fontAsset();
        FontAsset const& rightFontAsset = rightText_.fontAsset();

        if (leftFontAsset == rightFontAsset)
        {
            std::wstring const& leftText = leftText_.text();
            std::wstring const& rightText = rightText_.text();

            if (!leftText.empty() && !rightText.empty())
            {
                float fontScale = leftFontAsset.getScale(rightText_.size());

                return fontScale * leftFontAsset.getKerning(leftText[leftText.size() - 1], rightText[0]);
            }
        }
    }
    return 0;
}

Grapevine::Text MainView::createRandomText(int glyphCount)
{
#ifdef FORCE_FONT
    FontAsset fontAsset(FORCE_FONT);
#else
    // select random font from the available font files
    int selectedFont = rand() % fontFiles_.size();
    FontAsset fontAsset(fontFiles_[selectedFont]);
#endif

    float fontScale = fontAsset.getScale(FONT_SIZE);

    // randomize our color
    float r = static_cast<float>(rand()) / RAND_MAX;
    float g = static_cast<float>(rand()) / RAND_MAX;
    float b = static_cast<float>(rand()) / RAND_MAX;
    switch (rand() %3)
    {
        case 0:
            r = r * 0.5 + 0.5;
            break;
        case 1:
            g = g * 0.5 + 0.5;
            break;
        case 2:
            b = b * 0.5 + 0.5;
            break;
    }
    float a = static_cast<float>(rand()) / RAND_MAX * 0.5 + 0.5;

    // generate random text
    struct Language
    {
        int index;
        std::string text;
    };

    static Language languages[2] =
    {
      { 0, "Lorem ipsum dolor sit amet, per omnis nullam mollis id, sit paulo ullamcorper intellegebat ei. Pri id dolorum ancillae. Facilisi adipiscing vix ne, cu suas case qualisque cum, aeterno eloquentiam per id. Aliquip quaestio eam te. Cu mel primis iuvaret prodesset, ad per sale altera voluptatum." } ,
      { 0, "新摂民聞球情罪秋記氷選育改示岡集川覧本。告化属査荻会記先営原並野視負当。点持済歓激団治件勤県過月。無企半連十湯受回独覧負試予正説堀置。躍温卓各連面査提指頭味広光。流録民済聞球番来更合復馬治比合。文機図線健集皇重地可索全融男月。神字現宮写策情人条更易際装迷案度一。京描考子語政妊従料前予次禁同誘映。" },
    };

    Language& language = languages[fontAsset.name().find("HDZB_") == std::string::npos ? 0 : 1];

    std::string text;
    for (int i=0; i<glyphCount; ++i)
    {
        unsigned char code;

        code = language.text[(language.index++) % language.text.size()];
        text += code;

        // calculate tota bytes for UTF-8 multibyte characters
        int totalBytes = 0;
        for (int j=7; j>0; --j)
        {
            if (!(code & (1 << j)))
            {
                break;
            }
            ++totalBytes;
        }

        // include rest of the bytes for this multibyte character
        while (--totalBytes > 0)
        {
            code = language.text[(language.index++) % language.text.size()];
            text += code;
        }
    }

    return Grapevine::Text(text, fontAsset, FONT_SIZE, Vector4(r, g, b, a));
}

void MainView::createTextRow()
{
    RowElement rowElement;

    // create root node for our new row, where we link our TextNode(s)
    rowElement.root = TransformNode(rowLocation_);

    // link to scrollable text root
    textScroll_.addChild(rowElement.root);

    // update y location
    //rowElement.y = rowLocation_.y;
    rowLocation_.y(rowLocation_.y() + ROW_HEIGHT);

    // generate text elements for this row
    int fontCount = (rand() % 3) + 1;
    int glyphCount = GLYPHS_PER_ROW / fontCount;

#if !DYNAMIC_TEXT
    RichText richText;
#else
    int prev = 0;
    std::string prevFontName;
#endif

    TransformNode root = rowElement.root;

    for (int j=0; j<fontCount; ++j)
    {
        if (j == fontCount - 1)
        {
            // make sure we have exact number of glyphs per row
            glyphCount += GLYPHS_PER_ROW - glyphCount * fontCount;
        }

        Grapevine::Text text = createRandomText(glyphCount);

#if DYNAMIC_TEXT
        // create new richtext for each glyph
        std::wstring const &str = text.text();
        FontAsset const& fontAsset = text.fontAsset();

        if (prevFontName != fontAsset.name())
        {
            prevFontName = fontAsset.name();

            // reset prev character if fonts don't match
            prev = 0;
        }

        float fontScale = fontAsset.getScale(text.size());

        for (int k=0; k<str.size(); ++k)
        {
            int code = str[k];

            // calculate kerning and advancewidth
            float kerning = fontScale * fontAsset.getKerning(prev, code);
            float advanceWidth = fontScale * fontAsset.getAdvanceWidth(code);

            // create new text element with all necessary references so we can modify the data later on
            TextElement textElement;
            textElement.richText = RichText(Grapevine::Text(std::wstring(1, code), fontAsset, text.size(), text.color()));
            textElement.textNode = TextNode(Vector3(kerning, 0, 0), textElement.richText);
            textElement.advanceWidth = TransformNode(Vector3(advanceWidth, 0, 0));

            root.addChild(textElement.textNode);
            textElement.textNode.addChild(textElement.advanceWidth);

            // add newly created textelement to list of available textelements
            rowElement.textElements.push_back(textElement);

            // use this node's advancewidth translation helper as a root for next node
            root = textElement.advanceWidth;

            prev = code;
        }
#else
        richText.addText(text);
#endif
    }

#if !DYNAMIC_TEXT
    // create new TextElement, no need for advanceWidth
    TextElement textElement;
    textElement.richText = richText;
    textElement.textNode = TextNode(Vector3(0, 0, 0), richText);

    rowElement.root.addChild(textElement.textNode);

    // add newly created textelement to list of available textelements
    rowElement.textElements.push_back(textElement);
#endif

    // add new row element to list of RowElements and clamp row count to a desired value
    rowElements_.push_back(rowElement);
    if (rowElements_.size() > ROWS)
    {
        textScroll_.removeChild(rowElements_.front().root);
        rowElements_.pop_front();
    }
}

float gauss(float x, float y, float s)
{
    return std::exp((x * x + y * y) / (-2.0f * s * s));
}

void normalize(float *v)
{
    float s = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (s)
    {
        s = 1.0f / float(sqrt(s));
        v[0] *= s;
        v[1] *= s;
        v[2] *= s;
    }
}

void computeNormal(float *dest, float dx, float dy)
{
    float s = 1.0f / float(sqrt(dx * dx + dy * dy + 1.0f));
    dest[0] = s * dx;
    dest[1] = s * dy;
    dest[2] = s;
}

void gauss(GLfloat *m, float x, float y)
{
    float s = 1.0f;
    float d = 1.0f; // todo: scale
    float gx = x / 30.0f;
    float gy = y / 30.0f;

    float z0 = gauss(gx + 0, gy + 0, s);
    float z1 = gauss(gx + d, gy + 0, s);
    float z2 = gauss(gx + 0, gy - d, s);
    float z3 = gauss(gx - d, gy + 0, s);
    float z4 = gauss(gx + 0, gy + d, s);
    float dx = z1 - z3; // todo: scale result
    float dy = z4 - z2; // todo: scale result

    // x: right
    m[0] = d; // todo: scale?
    m[1] = 0.0f;
    m[2] = dx;
    m[3] = 0.0f;
    normalize(m + 0);

    // y: up
    m[4] = 0.0f;
    m[5] = d; // todo: scale?
    m[6] = dy;
    m[7] = 0.0f;
    normalize(m + 4);

    // z: ahead
    m[8] = 0; // computed
    m[9] = 0; // computed
    m[10] = 0; // computed
    m[11] = 0.0f;
    //cross(m + 8, m + 0, m + 4);
    //normalize(m + 8);
    computeNormal(m + 8, dx, dy);

    m[12] = 0;//x;
    m[13] = 0;//y;
    m[14] = 100.f * z0;//z0;
    m[15] = 1.0f;
}

void MainView::changeRandomGlyph()
{
    #if !DYNAMIC_TEXT
        return;
    #endif

    // select random row
    std::list<RowElement>::iterator it = rowElements_.begin();
    int selectedRow = rand() % rowElements_.size();
    for (int i=0; i<selectedRow; ++i)
    {
        ++it;
    }
    RowElement& rowElement = *it;

    if (rowElement.textElements.size() < 2)
    {
        // if there is not enough elements
        return;
    }

    // select random glyph
    int selectedGlyph = rand() % rowElement.textElements.size();

    // create new 1 char random text
    Grapevine::Text text = createRandomText(1);

    // grab our font
    FontAsset const& fontAsset = text.fontAsset();
    float fontScale = fontAsset.getScale(text.size());

    char code = text.text()[0];
    float advanceWidth = fontScale * fontAsset.getAdvanceWidth(code);

    // create new text element with all necessary references so we can modify the data later on
    TextElement newTextElement;
    newTextElement.richText = RichText(text);
    newTextElement.textNode = TextNode(Vector3(0, 0, 0), newTextElement.richText);
    newTextElement.advanceWidth = TransformNode(Vector3(advanceWidth, 0, 0));

    newTextElement.textNode.addChild(newTextElement.advanceWidth);

    // take reference to element being replaced
    TextElement &oldTextElement = rowElement.textElements[selectedGlyph];

    if (selectedGlyph == 0)
    {
        // we want to replace first glyph
        TransformNode& prev = rowElement.root;
        TextElement& next = rowElement.textElements[selectedGlyph + 1];

        // update kerning
        next.textNode.setLocation(getKerning(newTextElement.richText, next.richText));

        // replace TextElement in hierarchy
        prev.removeChild(oldTextElement.textNode);
        prev.addChild(newTextElement.textNode);
        oldTextElement.advanceWidth.removeChild(next.textNode);
        newTextElement.advanceWidth.addChild(next.textNode);
    }
    else
    if (selectedGlyph == rowElement.textElements.size() - 1)
    {
        // we want to replace last glyph
        TextElement& prev = rowElement.textElements[selectedGlyph - 1];

        // update kerning
        newTextElement.textNode.setLocation(getKerning(prev.richText, newTextElement.richText));

        // replace TextElement in hierarchy
        prev.advanceWidth.removeChild(oldTextElement.textNode);
        prev.advanceWidth.addChild(newTextElement.textNode);
    }
    else
    {
        // any other glyph in the text
        TextElement& prev = rowElement.textElements[selectedGlyph - 1];
        TextElement& next = rowElement.textElements[selectedGlyph + 1];

        // update kerning
        newTextElement.textNode.setLocation(getKerning(prev.richText, newTextElement.richText));
        next.textNode.setLocation(getKerning(newTextElement.richText, next.richText));

        // replace TextElement in hierarchy
        prev.advanceWidth.removeChild(oldTextElement.textNode);
        prev.advanceWidth.addChild(newTextElement.textNode);
        oldTextElement.advanceWidth.removeChild(next.textNode);
        newTextElement.advanceWidth.addChild(next.textNode);
    }

    // replace old element with new
    oldTextElement = newTextElement;
}

void MainView::animateGlyphs()
{
    #if DYNAMIC_TEXT
        return;
    #endif

    float gaussTM[16];

    // go through all rows
    std::list<RowElement>::iterator it = rowElements_.begin();
    while (it != rowElements_.end())
    {
        RowElement &rowElement = *it++;

        Vector3 location = textScroll_.location() + rowElement.root.location();

        for (int i=0; i<rowElement.textElements.size(); ++i)
        {
            TextNode &textNode = rowElement.textElements[i].textNode;

            int glyphCount = textNode.glyphCount();
            for (int j=0; j<glyphCount; ++j)
            {
                Grapevine::Vector2 position = textNode.glyphPosition(j);
                float scale = textNode.glyphScale(j);

                gauss(gaussTM, location.x() + position.x(), location.y() + position.y());

                Matrix4 tm;
                tm *= Matrix::translate(position.x(), position.y(), 0);
                tm *= Matrix4(gaussTM);
                tm *= Matrix::scale(scale, scale, 0);

                textNode.setGlyphTransform(j, tm);
            }
        }
    }
}

void MainView::input(bool& quitApplication)
{
    InputEvent event;

    while (inputSource_.pollEvent(event))
    {
        if (event.type == InputEventType::Touch)
        {
            if (event.touch.flags & TouchFlags::Pressed)
            {
                zoomDelta = 0.f;
                scrollSpeedDelta = 0.f;
            }
            if (event.touch.flags & TouchFlags::Moved)
            {
                float dx = event.touch.x - event.touch.lastX;
                float dy = event.touch.y - event.touch.lastY;
                zoomDelta = zoomDelta + (dx / width_ - zoomDelta) * 0.1f;
                scrollSpeedDelta = scrollSpeedDelta + (dy / height_ - scrollSpeedDelta) * 0.1f;
            }
        }
        else if (event.type == InputEventType::System)
        {
            Log::I("event.type == InputEventType::System");
            quitApplication = true;
        }
    }
}

void MainView::physics()
{
    static float deltaTime = 0.01f;
    static float time = 0.f;
    static float scrollTime = 0.f;
    static float rowSpawnTime = 0.f;
    static float glyphChangeTime = 0.f;

    zoom += zoomDelta * 100.f * deltaTime;
    zoomDelta *= 0.9f;
    if (zoom < 0) zoom = 0;
    if (zoom > 1) zoom = 1;

    scrollSpeed += scrollSpeedDelta * 1000.f * deltaTime;
    scrollSpeedDelta *= 0.9f;
    if (scrollSpeed < 0) scrollSpeed = 0;
    if (scrollSpeed > 10) scrollSpeed = 10;

    textScroll_.setLocation(Vector3(0, -scrollTime * ROW_HEIGHT * ROWS_PER_SECOND, zoom * 150.f));

#if ROWS_PER_SECOND > 0
    float singleRowSpawnTime = 1.f / ROWS_PER_SECOND;
    while (rowSpawnTime >= singleRowSpawnTime)
    {
        createTextRow();
        rowSpawnTime -= singleRowSpawnTime;
    }
#endif

#if CHANGE_GLYPHS_PER_SECOND > 0
    float singleGlyphChangeTime = 1.f / CHANGE_GLYPHS_PER_SECOND;
    while (glyphChangeTime >= singleGlyphChangeTime)
    {
        changeRandomGlyph();
        glyphChangeTime -= singleGlyphChangeTime;
    }
#endif

#ifdef ANIMATE_GLYPHS
    animateGlyphs();
#endif

    time += deltaTime;
    scrollTime += deltaTime * scrollSpeed;
    rowSpawnTime += deltaTime * scrollSpeed;
    glyphChangeTime += deltaTime;
}

void MainView::draw()
{
    fpsCounter_.startStep();

    canvas_.clear();
    scene_.draw();
    canvas_.update();

    fpsCounter_.endStep();

    return;
}
