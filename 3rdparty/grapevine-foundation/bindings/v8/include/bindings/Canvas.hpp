#include <Grapevine/Canvas/Canvas.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Canvas : public ExportedClass<Canvas, void, Grapevine::Canvas>
        {
        public:
            Canvas();

            void initializeClass(v8::Local<v8::FunctionTemplate> nodeTemplate);
            Grapevine::Canvas *construct(const v8::FunctionCallbackInfo<v8::Value>& args);

        private:
            static void width(Grapevine::Canvas *canvas, const v8::FunctionCallbackInfo<v8::Value>& args);
            static void height(Grapevine::Canvas *canvas, const v8::FunctionCallbackInfo<v8::Value>& args);
        };
    }
}