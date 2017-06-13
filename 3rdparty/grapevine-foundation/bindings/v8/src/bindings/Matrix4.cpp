#include <Grapevine/Math/Matrix.hpp>

#include "bindings.hpp"
#include "bindings/Vector.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class Matrix4 : public ExportedClass<Matrix4, void, Grapevine::Matrix4>
        {
            public:
            Matrix4()
            : ExportedClass("grapevine", "Matrix4")
            {
            }

            void initializeClass(v8::Local<v8::FunctionTemplate> matrixTemplate)
            {
                registerMethod<Matrix4::setRotation>("setRotation");
            }

            Grapevine::Matrix4 *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                return new Grapevine::Matrix4();
            }

            static void setRotation(Grapevine::Matrix4 *matrix, const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() == 2)
                {
                    float angle = args[0].As<v8::Number>()->Value();
                    Vector3 *axis = cast<Vector<3> >(args[1]);

                    Grapevine::Matrix4 m = Grapevine::Matrix::rotate(angle,*axis);

                    *matrix = m;
                }
            }
        };

        Matrix4 matrix4_;
    }
}
