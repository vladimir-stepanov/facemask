#include <Grapevine/Math/Matrix.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Matrix4 : public ExportedClass<Matrix4, void, Grapevine::Matrix4>
        {
            public:
            Matrix4();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputEventTemplate);
            Grapevine::Matrix4 *construct(const v8::FunctionCallbackInfo<v8::Value>& args);
            static void setRotation(Grapevine::Matrix4 *matrix, const v8::FunctionCallbackInfo<v8::Value>& args);
        };
    }
}
