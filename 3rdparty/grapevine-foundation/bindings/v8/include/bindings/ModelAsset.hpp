#ifndef __MODELASSET_HPP__
#define __MODELASSET_HPP__

#include <Grapevine/Model/ModelAsset.hpp>

#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        class ModelAsset : public ExportedClass<ModelAsset, void, Grapevine::ModelAsset>
        {
        public:
            ModelAsset();
            void initializeClass(v8::Local<v8::FunctionTemplate> inputTemplate);
            Grapevine::ModelAsset *construct(const v8::FunctionCallbackInfo<v8::Value>& args);

        };
    }
}

#endif