#include <Grapevine/Math/Ray.hpp>
#include "bindings.hpp"
#include "bindings/Vector.hpp"
#include "bindings/Ray.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        Ray::Ray()
        : ExportedClass("grapevine", "Ray")
        {

        }

        void Ray::initializeClass(v8::Local<v8::FunctionTemplate> inputEventTemplate)
        {
            registerMethod<Ray::origin>("origin");
            registerMethod<Ray::unit>("unit");
        }

        Grapevine::Ray *Ray::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            if(args.Length() == 2)
            {
                Vector3 *origin = cast<Vector<3> >(args[0]);
                Vector3 *unit = cast<Vector<3> >(args[1]);
                return new Grapevine::Ray(*origin, *unit);
            }

            LOGE("Invalid arguments to Ray constructor!");
            return NULL;
        }

        void Ray::origin(Grapevine::Ray *ray, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            args.GetReturnValue().Set(Vector<3>::classInstance()->instance(new Vector3(ray->origin())));
        }

        void Ray::unit(Grapevine::Ray *ray, const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            args.GetReturnValue().Set(Vector<3>::classInstance()->instance(new Vector3(ray->unit())));
        }

        static Ray ray_;

    }
}
