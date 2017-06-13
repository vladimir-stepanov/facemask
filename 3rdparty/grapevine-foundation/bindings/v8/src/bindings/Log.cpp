#include <Grapevine/Core/Log.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Log : public Exported
        {
            public:
            Log() : Exported("grapevine", "log") {}

            v8::Local<v8::Data> initialize()
            {
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                v8::EscapableHandleScope handle_scope(isolate);

                v8::Local<v8::FunctionTemplate> logTemplate = v8::FunctionTemplate::New(isolate, log);
                return handle_scope.Escape(logTemplate);
            }

            static void log(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if (args.Length() < 2) return;

                v8::HandleScope scope(args.GetIsolate());
                v8::Handle<v8::Value> level = args[0];
                v8::Handle<v8::Value> string = args[1];
                char levelId = (*v8::String::Utf8Value(level))[0];

                switch(levelId)
                {
                    default:
                    case 'D': LOGD("%s", *v8::String::Utf8Value(string)); break;
                    case 'I': LOGI("%s", *v8::String::Utf8Value(string)); break;
                    case 'W': LOGW("%s", *v8::String::Utf8Value(string)); break;
                    case 'E': LOGE("%s", *v8::String::Utf8Value(string)); break;
                }
            }
        };

        static Log log_;

    }
}
