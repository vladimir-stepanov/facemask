//!

#include <Grapevine/SceneExtensions/SceneExtensions.hpp>

using namespace Grapevine;

// // // // // //      T E S T   C L A S S   M A I N    // // // // // //
static const unsigned int IMAGE_WIDTH = 1000;
static const unsigned int IMAGE_HEIGHT = 1000;

class Main
{
public:
    Main():
        canvas_(1600, 900),
        inputSource_(canvas_),
        resourceManager_("assets/"),
        cameraComponent_(canvas_),
        cameraTransformationComponent_(Vector3(0.0f, 0.0f, 1000.0f)),
        cameraNode_(cameraComponent_, cameraTransformationComponent_),
        burnTransformation_(Vector3(0.0f, 0.0f, 0.0f)),
        burnTexture_("grapevine.png"),
        burnMaterial_(burnTexture_, Vector2(0.2f, 0.2f), 0.5f, 0.05f),
        burnModel_(MeshAsset(IMAGE_WIDTH, IMAGE_HEIGHT), burnMaterial_),
        burnNode_(burnTransformation_, burnModel_),
        backgroundTransformation_(Vector3(0.0f, 0.0f, -100.0f)),
        backgroundMaterial_("borat.png", 0, 0),
        backgroundModel_(MeshAsset(IMAGE_WIDTH, IMAGE_HEIGHT), backgroundMaterial_),
        scene_(cameraNode_),
        timer_(),
        zeroPlane_(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f))
    {
        scene_.addNode(cameraNode_);
        scene_.addNode(burnNode_);

        Scene::Node background(backgroundTransformation_, backgroundModel_);
        scene_.addNode(background);
    }

    void run()
    {
        static unsigned int startTime = Timer::milliseconds();
        canvas_.setClearColor(Vector4(0.5f, 0.5f, 1.0f, 1.0f));
        FpsCounter fpsCounter("FPS");
        bool quitApplication = false;
        while (quitApplication == false)
        {
            fpsCounter.startStep();
            input(quitApplication);
            float rotation = ((float)Timer::milliseconds() - (float)startTime) * 0.1f;
            Matrix4 tmpRotation = Matrix::rotate(rotation, Vector::yAxis);
            scene_.draw(canvas_);
            Timer::sleepMilliseconds(16);
            fpsCounter.endStep();
        }
    }

    void input(bool& quitApplication)
    {
        InputEvent event;
        static bool pressed_ = false;
        static float distance = 0.0f;

        // Light management
        static float pressedX = 0.0f;
        static float pressedY = 0.0f;
        static float textureU = 0.0f;
        static float textureV = 0.0f;
        static Vector3 pressedStart(0.0f, 0.0f, 0.0f);

        while (inputSource_.pollEvent(event))
        {
            // Touch
            if (event.type == InputEventType::Touch)
            {
                Ray tmp = cameraComponent_.mouseCoordinatesToRay(event.touch.x, event.touch.y, cameraTransformationComponent_.location(), cameraTransformationComponent_.rotation());
                RayPlaneIntersection tmp2 = tmp.intersection(zeroPlane_);
                if (tmp2.type() == RayPlaneIntersection::Type::PointIntersection)
                {
                    if(event.touch.flags & TouchFlags::Pressed && pressed_ == false)
                    {
                        pressedStart = tmp2.p();
                        pressed_ = true;
                    }
                    if (event.touch.flags & TouchFlags::Released)
                    {
                        pressed_ = false;
                    }
                    float x = tmp2.p().x();
                    float y = tmp2.p().y();
                    if((x >= -(float)IMAGE_WIDTH/2.0f && x <= (float)IMAGE_WIDTH/2.0f)
                        &&
                        (y >= -(float)IMAGE_HEIGHT/2.0f && y <= (float)IMAGE_HEIGHT/2.0f))
                    {
                        distance = (pressedStart - tmp2.p()).length() * 2.0f;
                        textureU = (x + (float)IMAGE_WIDTH/2.0f) / (float)IMAGE_WIDTH;
                        textureV = (y + (float)IMAGE_HEIGHT/2.0f) / (float)IMAGE_HEIGHT;
                        LOGD("u, v: %f, %f", textureU, textureV);
                    }
                }
            }
            // System quit
            else if (event.type == InputEventType::System)
            {
                quitApplication = true;
            }
        }
        if(pressed_ == false)
        {
            distance *= 0.97f;
        }
        burnMaterial_.setAlphaBurn(Vector2(textureU, textureV), distance/(float)IMAGE_WIDTH, 0.35f);
        scene_.setDirty();
    }

    void handleTimeout(Timer* timer)
    {
        LOG
    }

    void handleAnimationProgressTrigger()
    {
        LOG
    }

private:
    Canvas canvas_;
    InputSource inputSource_;
    ResourceManager resourceManager_;
    Scene::Extensions::TransformationComponent cameraTransformationComponent_;
    Scene::Extensions::CameraComponent cameraComponent_;
    Scene::Node cameraNode_;
    Scene::Extensions::TransformationComponent burnTransformation_;
    Grapevine::TextureAsset burnTexture_;
    Grapevine::MaterialAsset burnMaterial_;
    Scene::Extensions::ModelComponent burnModel_;
    Scene::Node burnNode_;
    Grapevine::MaterialAsset backgroundMaterial_;
    Scene::Extensions::TransformationComponent backgroundTransformation_;
    Scene::Extensions::ModelComponent backgroundModel_;
    Scene::Extensions::SimpleScene scene_;
    Timer timer_;
    Grapevine::Plane zeroPlane_;
};

// // // // // //       M A I N       // // // // // //

int main(int argc, char *argv[])
{
    LOG("hello-alphaburn");

    Main* tmp = new Main();
    tmp->run();
    delete tmp;

    return 0;
}
