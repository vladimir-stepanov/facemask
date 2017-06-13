#include <Grapevine/Model/ModelAsset.hpp>

#include "bindings.hpp"
#include "bindings/ModelAsset.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        ModelAsset::ModelAsset() : ExportedClass("grapevine", "ModelAsset") {}

        void ModelAsset::initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate)
        {
        }

        Grapevine::ModelAsset *ModelAsset::construct(const v8::FunctionCallbackInfo<v8::Value>& args)
        {
            std::string name;
            switch(args.Length())
            {
                case 1:
                {
                    v8::HandleScope scope(args.GetIsolate());
                    v8::Handle<v8::Value> v8name = args[0];
                    name = *v8::String::Utf8Value(v8name);
                    if(!name.empty() && name[0] != '\0') break;
                }
                default:
                    LOGE("Invalid argument(s) to ModelAsset constructor!");
                    return NULL;
            }
            return new Grapevine::ModelAsset(name.c_str());

        }
        static ModelAsset ModelAsset_;
    }
}
