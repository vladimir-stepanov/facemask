#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"
#include "bindings/Canvas.hpp"
#include "bindings/Ray.hpp"
#include "bindings/Vector.hpp"
#include "bindings/Matrix4.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class CameraComponent : public ExportedClass<CameraComponent, Component, Grapevine::Scene::Extensions::CameraComponent>
        {
            public:
            CameraComponent() : ExportedClass("grapevine", "CameraComponent") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> cameraComponentTemplate)
            {
                registerMethod<CameraComponent::mouseCoordinatesToRay>("mouseCoordinatesToRay");
            }

            Scene::Extensions::CameraComponent *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                Grapevine::Canvas *canvas = NULL;
                float fieldOfViewY = 90.0f;
                float nearPlane = 1.0f;
                float farPlane = 10000000.0f;

                switch(args.Length())
                {
                    case 4: farPlane = args[3].As<v8::Number>()->Value();
                    case 3: nearPlane = args[2].As<v8::Number>()->Value();
                    case 2: fieldOfViewY = args[1].As<v8::Number>()->Value();
                    case 1: canvas = cast<Canvas>(args[0]);
                            if(canvas) break;
                    default:
                        LOGE("Invalid arguments to CameraComponent constructor!");
                        return NULL;
                }
                return new Scene::Extensions::CameraComponent(*canvas, fieldOfViewY, nearPlane, farPlane);
            }

            static void mouseCoordinatesToRay(Scene::Extensions::CameraComponent *cameraComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 4)
                {
                    int x = args[0].As<v8::Number>()->Value();
                    int y = args[1].As<v8::Number>()->Value();
                    Vector3 *location = cast<Vector<3> >(args[2]);
                    Grapevine::Matrix4 *rotation = cast<Matrix4>(args[3]);
                    if(location && rotation)
                    {
                        const Grapevine::Ray &ray = cameraComponent->mouseCoordinatesToRay(x, y, *location, *rotation);
                        args.GetReturnValue().Set(Ray::classInstance()->instance(new Grapevine::Ray(ray.origin(), ray.unit())));
                        return;
                    }
                }

                LOGE("Invalid arguments to CameraComponent::mouseCoordinatesToRay");
            }

        };

        static CameraComponent cameraComponent_;

    }
}
