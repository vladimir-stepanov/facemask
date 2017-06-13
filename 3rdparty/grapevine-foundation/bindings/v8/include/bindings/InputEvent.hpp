#include <Grapevine/Input/InputEvents.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class InputEvent : public ExportedClass<InputEvent, void, Grapevine::InputEvent>
        {
        public:
            InputEvent();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate);
            Grapevine::InputEvent *construct(const v8::FunctionCallbackInfo<v8::Value>& args);

        private:
            static void type(Grapevine::InputEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info);
            static void touch(Grapevine::InputEvent *event, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info);

        };
    }
}