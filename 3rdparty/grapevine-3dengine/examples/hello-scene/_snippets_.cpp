
    void initAlphaDrawTest()
    {
        {
            Scene::Extensions::AnimationComponent animationComponent1(
                Animation(
                    5000.0f,
                    Easing(Easing::Type::Linear),
                    LinearPath(
                        ControlPoint(Vector3(-1000.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 1.0f),
                        ControlPoint(Vector3(-1000.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.0f)
                    )
                )
            );

            Scene::Node node1(
                    animationComponent1,
                    Scene::Extensions::TransformationComponent(Vector3(-1000.0f, 600.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            scene_.addNode(node1);

            Scene::Node node2(
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, -200.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            node1.addChild(node2);

            Scene::Node node3(
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, -200.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            node2.addChild(node3);

            animationComponent1.start();
        }

        {
            Scene::Extensions::AnimationComponent animationComponent2(
                Animation(
                    5000.0f,
                    Easing(Easing::Type::Linear),
                    LinearPath(
                        ControlPoint(Vector3(-500.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 1.0f),
                        ControlPoint(Vector3(-500.0f, 0.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.0f)
                    )
                )
            );

            Scene::Node nodeB1(
                    animationComponent2,
                    Scene::Extensions::TransformationComponent(Vector3(-500.0f, 600.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            scene_.addNode(nodeB1);

            Scene::Extensions::ModelComponent modelB(ModelAsset("facebook.barrel"));
            modelB.setCumulative(false);
            Scene::Node nodeB2(
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, -200.0f, 0.0f)),
                    modelB
                );
            nodeB1.addChild(nodeB2);

            Scene::Extensions::AnimationComponent animationComponent3(
                Animation(
                    5000.0f,
                    Easing(Easing::Type::Linear),
                    LinearPath(
                        ControlPoint(Vector3(0.0f, -200.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 1.0f),
                        ControlPoint(Vector3(100.0f, -200.0f, 0.0f), Matrix4(), Vector3(1.0f, 1.0f, 1.0f), 0.5f)
                    )
                )
            );

            Scene::Node nodeB3(
                    animationComponent3,
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, -200.0f, 0.0f))
                );
            nodeB2.addChild(nodeB3);

            Scene::Node nodeB4(
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, 0.0f, 0.0f)),
                    Scene::Extensions::TextComponent(Grapevine::RichText(Text("moro",FontAsset("Ubuntu-Light.ttf"), 100, Vector4(1, 1, 1, 1))))
                );
            nodeB3.addChild(nodeB4);

            animationComponent2.start();
            animationComponent3.start();
        }

        // fabo logo at bottom, "shadow" effect middle, text at top
        {
            Scene::Node node1(
                    Scene::Extensions::TransformationComponent(Vector3(0.0f, 0.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            scene_.addNode(node1);

            // Create model asset
            MeshAsset meshAsset(250.0f, 250.0f);
            MaterialAsset materialAsset(Vector4(1.0f, 0.0f, 0.0f, 0.5f), 0.25f);
            Scene::Node node2(
                    Scene::Extensions::TransformationComponent(Vector3(-50.0f, -50.0f, 200.0f)),
                    Scene::Extensions::ModelComponent(meshAsset, materialAsset)
                );
            node1.addChild(node2);

            Scene::Node node3(
                    Scene::Extensions::TransformationComponent(Vector3(-100.0f, 0.0f, 200.0f)),
                    Scene::Extensions::TextComponent(Grapevine::RichText(Text("sämpylää",FontAsset("Ubuntu-Light.ttf"), 100, Vector4(1, 1, 1, 1))))
                );
            node2.addChild(node3);
        }

        // same as previous but, text at top is totally transparent (should not be seen at all)
        {
            Scene::Node node1(
                    Scene::Extensions::TransformationComponent(Vector3(500.0f, 0.0f, 0.0f)),
                    Scene::Extensions::ModelComponent(ModelAsset("facebook.barrel"))
                );
            scene_.addNode(node1);

            // Create model asset
            MeshAsset meshAsset(250.0f, 250.0f);
            MaterialAsset materialAsset(Vector4(1.0f, 0.0f, 0.0f, 0.5f), 0.25f);
            Scene::Node node2(
                    Scene::Extensions::TransformationComponent(Vector3(-50.0f, -50.0f, 200.0f)),
                    Scene::Extensions::ModelComponent(meshAsset, materialAsset)
                );
            node1.addChild(node2);

            Scene::Node node3(
                    Scene::Extensions::TransformationComponent(Vector3(-100.0f, 0.0f, 200.0f)),
                    Scene::Extensions::TextComponent(Grapevine::RichText(Text("sämpylää",FontAsset("Ubuntu-Light.ttf"), 100, Vector4(1, 1, 1, 0.5))))
                );
            node2.addChild(node3);
        }
    }

    void run()
    {
        static unsigned int startTime = Timer::milliseconds();
        canvas_.setClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
        FpsCounter fpsCounter("FPS");
        bool quitApplication = false;
        while (quitApplication == false)
        {
            input(quitApplication);
            fpsCounter.startStep();
            float rotation = ((float)Timer::milliseconds() - (float)startTime) * 0.1f;
            Matrix4 tmpRotation = Matrix::rotate(rotation, Vector::yAxis);
            transformationComponent_.setRotation(tmpRotation);
            scene_.draw(canvas_);
            Timer::sleepMilliseconds(16);
            fpsCounter.endStep();
        }
    }

    void input(bool& quitApplication)
    {
        InputEvent event;
        static bool pressed_ = false;

        // Light management
        static float defaultLightDirectionFactor = 1.0f;
        static Vector3 touchBasedDirection(0.0f, 0.0, 0.0f);
        Vector3 defaultLightDirection(0.23570f, 0.23570f, 0.94281f);
        static float pressedX = 0.0f;
        static float pressedY = 0.0f;
        static Vector3 omniLightColor(1.0f, 1.0f, 1.0f);
        static Vector3 omniLightLocation(0.0f, 0.0f, 0.0f);
        static float omniLightStrength = 0.0f;

        while (inputSource_.pollEvent(event))
        {
            // Touch
            if (event.type == InputEventType::Touch)
            {
                if (event.touch.flags & TouchFlags::Released)
                {
                    pressed_ = false;
                }
                else
                {
                    float x = (float)event.touch.x / (float)canvas_.width() - 0.5f;
                    float y = (float)event.touch.y / (float)canvas_.height() - 0.5f;
                    x *= 2.0f;
                    y *= -2.0f;
                    touchBasedDirection = Vector3(x, y, 1.0f);
                    LOGD("%f, %f", x, y);
                    pressed_ = true;
                }

                // pressed
                pressedX = (float)event.touch.x;
                pressedY = (float)event.touch.y;

                Ray tmp = cameraComponent_.mouseCoordinatesToRay(event.touch.x, event.touch.y, cameraTransformationComponent_.location(), cameraTransformationComponent_.rotation());
                RayPlaneIntersection tmp2 = tmp.intersection(omniLightPlane_);
                if (tmp2.type() == RayPlaneIntersection::Type::PointIntersection)
                {
                    // LOGI("p %f, %f", tmp2.p().x(), tmp2.p().y());
                    omniLightLocation = 0.5f * (omniLightLocation + Vector3(tmp2.p().x(), tmp2.p().y(), tmp2.p().z()));
                }
            }
            // System quit
            else if (event.type == InputEventType::System)
            {
                quitApplication = true;
            }
        }

        // Light management
        if (pressed_)
        {
            omniLightStrength += (maxOmniLightStrength - omniLightStrength) / 4.0f; // 4.0f is a hard coded factor
            float x = (float)pressedX / (float)canvas_.width() - 0.5f;
            float y = (float)pressedY / (float)canvas_.height() - 0.5f;
            x *= 2.0f;
            y *= -2.0f;
            touchBasedDirection = Vector3(x, y, 1.0f);
        }

        if (pressed_ == false)
        {
            omniLightStrength /= 1.25f; // 1.25f is a hard coded factor
            defaultLightDirectionFactor += (1.0f - defaultLightDirectionFactor) / 2.0f;
            if (defaultLightDirectionFactor > 1.0f)
                defaultLightDirectionFactor = 1.0f;
        }
        else
        {
            defaultLightDirectionFactor = 0.0f;
        }
        Vector3 totalLight = defaultLightDirectionFactor * defaultLightDirection + (1.0f - defaultLightDirectionFactor) * touchBasedDirection;

        scene_.setOmniLightWorldLocation(omniLightLocation);
        scene_.setOmniLightColor(omniLightColor);
        scene_.setOmniLightStrength(omniLightStrength);
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
    Scene::Extensions::SimpleScene scene_;
    Scene::Extensions::TransformationComponent transformationComponent_;
    Scene::Extensions::AnimationComponent animationComponent_;
    Scene::Extensions::AnimationComponent textAnimationComponent_;
    Scene::Extensions::AnimationProgressTriggerComponent animationProgressTriggerComponent_;
    Timer timer_;
    ControlPoint pX0Y0_;
    ControlPoint pX1Y0_;
    ControlPoint pX0Y1_;
    ControlPoint pX1Y1_;
    Grapevine::Plane omniLightPlane_;
};

// // TEST RAY CYLINDER INTERSECTION // //
void testRayCylinder()
{
    LOGD("*");
    LOGD("* TEST RAY CYLINDER INTERSECTION");
    LOGD("*");

    Cylinder cylinder(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), 1.0f);

    // Ray
    Ray ray0(Vector3(1.0f, 0.0f, 2.0f), Vector3(0.0f, 0.0f, -1.0f));
    RayCylinderIntersection i0 = ray0.intersection(cylinder);
    if (i0.type() == RayCylinderIntersection::Type::OnePointIntersection)
    {
        LOGD("one point as should: %f, %f, %f", i0.p0().x(), i0.p0().y(), i0.p0().z());
    }

    Ray ray1(Vector3(0.0f, 0.0f, 2.0f), Vector3(0.0f, 0.0f, -1.0f));
    RayCylinderIntersection i1 = ray1.intersection(cylinder);
    if (i1.type() == RayCylinderIntersection::Type::TwoPointIntersection)
    {
        LOGD("two points as should: %f, %f, %f and  %f, %f, %f",
            i1.p0().x(), i1.p0().y(), i1.p0().z(),
            i1.p1().x(), i1.p1().y(), i1.p1().z());
    }

    Ray ray2(Vector3(1.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f));
    RayCylinderIntersection i2 = ray2.intersection(cylinder);
    if (i2.type() == RayCylinderIntersection::Type::OnePointIntersection)
    {
        LOGD("one point as should: %f, %f, %f", i2.p0().x(), i2.p0().y(), i2.p0().z());
    }

    Ray ray3(Vector3(0.0f, 0.0f, 2.0f), Vector3(0.0f, 0.0f, 1.0f));
    RayCylinderIntersection i3 = ray3.intersection(cylinder);
    if (i3.type() == RayCylinderIntersection::Type::NoIntersection)
    {
        LOGD("n0 intersection as should");
    }
    else if(i3.type() == RayCylinderIntersection::Type::OnePointIntersection)
    {
        LOGE("one point: %f, %f, %f",
            i3.p0().x(), i3.p0().y(), i3.p0().z());
    }
    else if(i3.type() == RayCylinderIntersection::Type::TwoPointIntersection)
    {
        LOGE("two points: %f, %f, %f and  %f, %f, %f",
            i3.p0().x(), i3.p0().y(), i3.p0().z(),
            i3.p1().x(), i3.p1().y(), i3.p1().z());
    }


}

void testArrangeGrid()
{
    LOGD("*");
    LOGD("* TEST ARRANGE GRID");
    LOGD("*");

    Grapevine::Arrange::Grid grid(Vector2u(4, 4));
    LOGD("grid at start:");
    grid.debugPrint();

    // Add couple of more items
    {
        Grapevine::Arrange::Item item3(Vector2u(3, 3));
        bool arrangePossible = false;
        grid.place(item3, Vector2u(0, 0), arrangePossible);
        if (arrangePossible == true)
        {
            LOGD("could place");
        }
        else
        {
            LOGD("could not place");
        }
    }
    grid.debugPrint();

    {
        Grapevine::Arrange::Item item3(Vector2u(1, 1));
        bool arrangePossible = false;
        grid.place(item3, Vector2u(3, 2), arrangePossible);
        if (arrangePossible == true)
        {
            LOGD("could place");
        }
        else
        {
            LOGD("could not place");
        }
    }

    LOGD("grid at end:");
    grid.debugPrint();

}
