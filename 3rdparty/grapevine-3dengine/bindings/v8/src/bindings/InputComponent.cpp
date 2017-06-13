#include <Grapevine/SceneExtensions/InputComponent.hpp>
#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"
#include "bindings/Component.hpp"

#include "bindings/InputComponentEvent.hpp"
#include "bindings/Ray.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;

        namespace
        {

            void InputComponentSlot(const Scene::Extensions::InputComponent::Event& event, const ReferencedPointer<Referenced> &userData)
            {
                v8::HandleScope scope(v8::Isolate::GetCurrent());
                Scene::Extensions::InputComponent::Event *heapAllocatedEvent = new Scene::Extensions::InputComponent::Event();
                *heapAllocatedEvent = event;

                v8::Local<v8::Value> argv[1];
                argv[0] = InputComponentEvent::classInstance()->instance(heapAllocatedEvent);
                static_cast<CallbackProxy *>(userData.pointer())->call(sizeof(argv)/sizeof(argv[0]), argv);
            }

            void InputComponentEvent_getFlags(Scene::Extensions::InputComponent::Event *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), event->flags));
            }

            void InputComponentEvent_setFlags(Scene::Extensions::InputComponent::Event *event, const std::string &name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
            {
                event->flags = value.As<v8::Number>()->Value();
            }

            void InputComponentEvent_getRay(Scene::Extensions::InputComponent::Event *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(Ray::classInstance()->instance(new Grapevine::Ray(event->ray.origin(), event->ray.unit())));
            }

            void InputComponentEvent_setRay(Scene::Extensions::InputComponent::Event *event, const std::string &name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
            {
                Grapevine::Ray *ray = cast<Ray>(value);
                if(ray)
                {
                    event->ray = *ray;
                }
                else
                {
                    LOGE("InputComponentEvent::ray set with invalid value. Should be of type Ray!");
                }
            }
        }


        class InputComponentStateFlag : public Exported
        {
        public:

            InputComponentStateFlag() : Exported("grapevine", "InputComponentStateFlag") {}

            virtual v8::Local<v8::Data> initialize()
            {
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                v8::EscapableHandleScope handle_scope(isolate);

                v8::Local<v8::ObjectTemplate> stateFlagTemplate = v8::ObjectTemplate::New(isolate);
                stateFlagTemplate->Set(v8::String::NewFromUtf8(isolate, "Pressed"), v8::Number::New(isolate, Scene::Extensions::InputComponent::StateFlag::Pressed));
                stateFlagTemplate->Set(v8::String::NewFromUtf8(isolate, "Moving"), v8::Number::New(isolate, Scene::Extensions::InputComponent::StateFlag::Moving));
                stateFlagTemplate->Set(v8::String::NewFromUtf8(isolate, "Released"), v8::Number::New(isolate, Scene::Extensions::InputComponent::StateFlag::Released));
                return handle_scope.Escape(stateFlagTemplate);
            }
        };



        InputComponentEvent::InputComponentEvent()
        : ExportedClass("grapevine", "InputComponentEvent")
        {
        }

        void InputComponentEvent::initializeClass(v8::Local<v8::FunctionTemplate> inputEventTemplate)
        {
            registerGetterSetter<InputComponentEvent_getFlags,
                                 InputComponentEvent_setFlags>("flags");

            registerGetterSetter<InputComponentEvent_getRay,
                                 InputComponentEvent_setRay>("ray");
        }

        Scene::Extensions::InputComponent::Event *InputComponentEvent::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            return new Scene::Extensions::InputComponent::Event();
        }



        class InputComponent : public ExportedClass<InputComponent, Component, Grapevine::Scene::Extensions::InputComponent>
        {
            public:
            InputComponent() : ExportedClass("grapevine", "InputComponent") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> inputComponentTemplate)
            {
                registerMethod<InputComponent::setCallback>("setCallback");
            }

            Grapevine::Scene::Extensions::InputComponent *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                Grapevine::Scene::Extensions::InputComponent *inputComponent = new Grapevine::Scene::Extensions::InputComponent(new CallbackProxy());
                inputComponent->connectInput(sigc::ptr_fun(InputComponentSlot));
                return inputComponent;
            }

            static void setCallback(Grapevine::Scene::Extensions::InputComponent *inputComponent, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                v8::EscapableHandleScope scope(v8::Isolate::GetCurrent());
                if(args.Length() != 1 || !args[0]->IsFunction())
                {
                    LOGE("Invalid arguments to InputComponent::setCallback!");
                    return;
                }

                static_cast<CallbackProxy *>(inputComponent->userData().pointer())->setCallback(scope.Escape(args.Holder()), args[0].As<v8::Function>());
            }

        };


        static InputComponentStateFlag inputComponentStateFlag_;
        static InputComponentEvent inputComponentEvent_;
        static InputComponent inputComponent_;

    }
}
