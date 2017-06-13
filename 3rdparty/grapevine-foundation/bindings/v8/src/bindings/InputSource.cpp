#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>

#include "bindings.hpp"
#include "bindings/Canvas.hpp"
#include "bindings/InputEvent.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Canvas;
        class InputSource : public ExportedClass<InputSource, void, Grapevine::InputSource>
        {
        public:
            InputSource() : ExportedClass("grapevine", "InputSource") {}

            void initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate)
            {
                registerMethod<InputSource::pollEvent>("pollEvent");
            }

            Grapevine::InputSource *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() != 1)
                {
                    LOGE("InputSource needs exactly one parameter, Canvas");
                    return NULL;
                }
                Grapevine::Canvas *canvas = cast<Canvas>(args[0]);

                if(!canvas)
                {
                    LOGE("InputSource needs exactly one parameter, Canvas");
                    return NULL;
                }
                return new Grapevine::InputSource(*canvas);
            }

        private:
            static void pollEvent(Grapevine::InputSource *source, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() != 1)
                {
                    LOGE("InputSource::pollEvent needs exactly one parameter, InputEvent");
                    return;
                }

                Grapevine::InputEvent *inputEvent = cast<InputEvent>(args[0]);

                if(!inputEvent)
                {
                    LOGE("InputSource::pollEvent needs exactly one parameter, InputEvent");
                    return;
                }

                args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), source->pollEvent(*inputEvent)));
            }
        };

        static InputSource InputSource_;
    }
}