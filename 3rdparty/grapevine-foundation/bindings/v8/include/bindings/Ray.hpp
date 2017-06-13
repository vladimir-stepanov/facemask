#include <Grapevine/Math/Ray.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Ray : public ExportedClass<Ray, void, Grapevine::Ray>
        {
            public:
            Ray();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputEventTemplate);
            Grapevine::Ray *construct(const v8::FunctionCallbackInfo<v8::Value>& args);
            static void origin(Grapevine::Ray *ray, const v8::FunctionCallbackInfo<v8::Value>& args);
            static void unit(Grapevine::Ray *ray, const v8::FunctionCallbackInfo<v8::Value>& args);
        };
    }
}
