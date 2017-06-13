#include <Grapevine/Canvas/Canvas.hpp>

#include "bindings.hpp"
#include "bindings/Canvas.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;

        Canvas::Canvas() : ExportedClass("grapevine", "Canvas") {}

        void Canvas::initializeClass(v8::Local<v8::FunctionTemplate> nodeTemplate)
        {
            registerMethod<Canvas::width>("width");
            registerMethod<Canvas::height>("height");
        }

        Grapevine::Canvas *Canvas::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() != 2)
            {
                LOGE("%i arguments passed to Canvas constructor, should be exactly 2!", args.Length());
                return NULL;
            }

            if(!args[0]->IsNumber())
            {
                LOGE("First argument to Canvas should be number!");
                return NULL;
            }

            if(!args[1]->IsNumber())
            {
                LOGE("Second argument to Canvas should be number!");
                return NULL;
            }

            return new Grapevine::Canvas(args[0].As<v8::Number>()->Value(), args[1].As<v8::Number>()->Value());
        }

        void Canvas::width(Grapevine::Canvas *canvas, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), canvas->width()));
        }

        void Canvas::height(Grapevine::Canvas *canvas, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            args.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), canvas->height()));
        }


        static Canvas sCanvas_;
    }
}