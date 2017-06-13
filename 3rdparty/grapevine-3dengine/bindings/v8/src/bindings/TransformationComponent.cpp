#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"
#include "bindings/Vector.hpp"
#include "bindings/Matrix4.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class TransformationComponent : public ExportedClass<TransformationComponent, Component, Scene::Extensions::TransformationComponent>
        {
            public:
            TransformationComponent() : ExportedClass("grapevine", "TransformationComponent") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> transformationComponentTemplate)
            {
                registerMethod<TransformationComponent::location>("location");
                registerMethod<TransformationComponent::scale>("scale");
                registerMethod<TransformationComponent::rotation>("rotation");
                registerMethod<TransformationComponent::setLocation>("setLocation");
                registerMethod<TransformationComponent::setRotation>("setRotation");
                registerMethod<TransformationComponent::setScale>("setScale");
            }

            Scene::Extensions::TransformationComponent *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                bool ok = true;
                if(args.Length() == 1)
                {
                    Vector3 *position = cast<Vector<3> >(args[0]);
                    if(position)
                    {
                        return new Scene::Extensions::TransformationComponent(*position);
                    }
                    else
                    {
                        ok = false;
                    }
                }
                else if(args.Length() > 1)
                {
                    ok = false;
                }

                if(!ok)
                {
                    LOGE("TransformationComponent needs optionally either Vector3, ControlPoint (NYI), or none");
                    return NULL;
                }

                return new Scene::Extensions::TransformationComponent();
            }

            static void location(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                args.GetReturnValue().Set(Vector<3>::classInstance()->instance(new Vector3(transformationComponent->location())));
            }

            static void scale(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                args.GetReturnValue().Set(Vector<3>::classInstance()->instance(new Vector3(transformationComponent->scale())));
            }

            static void rotation(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                args.GetReturnValue().Set(Matrix4::classInstance()->instance(new Grapevine::Matrix4(transformationComponent->rotation())));
            }

            static void setLocation(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 1)
                {
                    Vector3 *loc = cast<Vector<3> >(args[0]);
                    if(loc)
                    {
                        transformationComponent->setLocation(*loc);
                        return;
                    }
                }

                LOGE("Invalid arguments to TransformationComponent::setLocation");
            }

            static void setRotation(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 2)
                {
                    Vector3 *axis = cast<Vector<3> >(args[0]);
                    float angle = args[1].As<v8::Number>()->Value();
                    if(axis)
                    {
                        transformationComponent->setRotation(*axis, angle);
                        return;
                    }
                }

                LOGE("Invalid arguments to TransformationComponent::setRotation");
            }

            static void setScale(Scene::Extensions::TransformationComponent *transformationComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 1)
                {
                    Vector3 *scale = cast<Vector<3> >(args[0]);
                    if(scale)
                    {
                        transformationComponent->setScale(*scale);
                        return;
                    }
                }

                LOGE("Invalid arguments to TransformationComponent::setScale");
            }


        };

        static TransformationComponent transformationComponent_;

    }
}
