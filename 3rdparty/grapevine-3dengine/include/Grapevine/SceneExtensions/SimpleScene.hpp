/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_SIMPLESCENE
#define GRAPEVINE_SCENEEXTENSIONS_SIMPLESCENE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include "Grapevine/SceneExtensions/InputComponent.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            /**
             * @class SimpleScene
             *
             * Extended SceneGraph with next functionality:
             * - animation
             * - drawing
             * - one free omni light and color
             * - diffuse light direction and color
             */
            class SimpleScene: public Graph
            {
            public:
                CLASS_UNCOPYABLE(SimpleScene)

                struct CullingMethod
                {
                    enum Enum
                    {
                        CameraFrustum,
                        Box
                    };
                };

                /**
                 * SimpleScene constructor that creates a scene using
                 * camera frustum as rendered area. This is recommended
                 * to be used in most simple cases.
                 *
                 * @note: camera can exist in root level only currently
                 *        Reason: Camera does not use absolute location, but local only
                 */
                SimpleScene(Node cameraNode);

                /**
                 * SimpleScene constructor that creates a scene using
                 * custom axis aligned box as a rendered area.
                 */
                SimpleScene(Node& cameraNode, Vector3 const& cullerBoxOrigo, Vector3 const& cullerBoxHalfAxis);
                ~SimpleScene();
                bool draw(Canvas& canvas);

                /**
                 * Change light direction. The end light direction will be
                 * normalized vector from given x, y and z values.
                 *
                 * @param [in]  direction   Light direction wanted in eye space
                 */
                void setLightDirection(Vector3 const& direction);

                /**
                 * @param [in]
                 */
                void setOmniLightWorldLocation(Vector3 const&);

                /**
                 * @param [in]
                 */
                void setOmniLightColor(Vector3 const&);

                /**
                 * @param [in]
                 */
                void setOmniLightStrength(float);

                /**
                 * @param [in]
                 */
                void setDeviceOrientation(Vector3 const& currentOrientation);

                /**
                 * @param [in]
                 */
                void setDirty();

                /**
                 * @note affects only with CullingMethod::Box.
                 *
                 * @param [in]
                 */
                void setCullerBoxOrigo(Vector3 const& origo);

                /**
                 * @note affects only with CullingMethod::Box.
                 *
                 * @param [in]
                 */
                void setCullerBoxAxis(Vector3 const& axis);

                /**
                 * @param [in] event    Input event that will be processed in draw()
                 */
                void add(InputComponent::Event const& event);

            private:
                CullingMethod::Enum cullingMethod_;
                Vector3 cullingBoxOrigo_;
                Vector3 cullingBoxAxis_;

                FpsCounter fpsOverRenderQueueSort_;
                FpsCounter fpsOverDraw_;

                float lightXDirection_;
                float lightYDirection_;

                /// Rendered orientation will smoothly follow the current orientation got from hardware sensors
                unsigned int lastOrientationTimestamp_;
                unsigned int lastOrientationTimestampDiff_;
                Vector3 renderedOrientation_;
                Vector3 currentOrientation_;
                Vector3 lastOrientation_;

                Node cameraNode_;
                class Renderer* renderer_;
                bool sceneDirtyDebugFlag_;

                std::vector<InputComponent::Event> inputEvents_;
            };
        }
    }
}

#endif
