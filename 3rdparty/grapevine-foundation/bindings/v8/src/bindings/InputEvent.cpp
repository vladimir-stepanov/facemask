#include <Grapevine/Input/InputEvents.hpp>

#include "bindings.hpp"
#include "bindings/InputEvent.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class InputEventType : public Exported
        {
        public:

            InputEventType() : Exported("grapevine", "InputEventType") {}

            virtual v8::Local<v8::Data> initialize()
            {
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                v8::EscapableHandleScope handle_scope(isolate);

                v8::Local<v8::ObjectTemplate> inputEventTypeTemplate = v8::ObjectTemplate::New(isolate);
                inputEventTypeTemplate->Set(v8::String::NewFromUtf8(isolate, "Touch"), v8::Number::New(isolate, Grapevine::InputEventType::Touch));
                inputEventTypeTemplate->Set(v8::String::NewFromUtf8(isolate, "System"), v8::Number::New(isolate, Grapevine::InputEventType::System));
                return handle_scope.Escape(inputEventTypeTemplate);
            }
        };

        class TouchFlags : public Exported
        {
        public:

            TouchFlags() : Exported("grapevine", "TouchFlags") {}

            virtual v8::Local<v8::Data> initialize()
            {
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                v8::EscapableHandleScope handle_scope(isolate);

                v8::Local<v8::ObjectTemplate> inputEventTypeTemplate = v8::ObjectTemplate::New(isolate);
                inputEventTypeTemplate->Set(v8::String::NewFromUtf8(isolate, "Pressed"), v8::Number::New(isolate, Grapevine::TouchFlags::Pressed));
                inputEventTypeTemplate->Set(v8::String::NewFromUtf8(isolate, "Moved"), v8::Number::New(isolate, Grapevine::TouchFlags::Moved));
                inputEventTypeTemplate->Set(v8::String::NewFromUtf8(isolate, "Released"), v8::Number::New(isolate, Grapevine::TouchFlags::Released));
                return handle_scope.Escape(inputEventTypeTemplate);
            }
        };

        class TouchEvent : public ExportedClass<TouchEvent, void, Grapevine::TouchEvent>
        {
        public:
            TouchEvent() : ExportedClass("grapevine", "TouchEvent") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate)
            {
                registerGetter<TouchEvent::flags>("flags");
                registerGetter<TouchEvent::x>("x");
                registerGetter<TouchEvent::y>("y");
            }

            Grapevine::TouchEvent *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                return NULL;
            }

        private:
            static void flags(Grapevine::TouchEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), event->flags));
            }

            static void x(Grapevine::TouchEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), event->x));
            }
            static void y(Grapevine::TouchEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), event->y));
            }

        };


        InputEvent::InputEvent() : ExportedClass("grapevine", "InputEvent") {}

        void InputEvent::initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate)
        {
            registerGetter<InputEvent::type>("type");
            registerGetter<InputEvent::touch>("touch");
        }

        Grapevine::InputEvent *InputEvent::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            return new Grapevine::InputEvent();
        }

        void InputEvent::type(Grapevine::InputEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
        {
            info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), event->type));
        }

        void InputEvent::touch(Grapevine::InputEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
        {
            v8::Local<v8::Object> touchEvent = classInstance()->memberInstance<TouchEvent>(info.Holder(), &(event->touch));
            info.GetReturnValue().Set(touchEvent);
        }

        static InputEventType InputEventType_;
        static TouchEvent TouchEvent_;
        static TouchFlags TouchFlags_;
        static InputEvent InputEvent_;
    }
}