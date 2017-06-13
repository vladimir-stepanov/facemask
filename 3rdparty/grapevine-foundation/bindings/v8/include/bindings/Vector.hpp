#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <Grapevine/Math/Vector.hpp>
#include "bindings.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        // map N to the correct vector type
        template<int N> struct GrapevineVector { typedef void * Type; };
        template<> struct GrapevineVector<2>
        {
            typedef Grapevine::Vector2 Type;
            static std::string name() {return "Vector2";}
            static Type *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                return new Type(args[0].As<v8::Number>()->Value(), args[1].As<v8::Number>()->Value());
            }
        };

        template<> struct GrapevineVector<3>
        {
            typedef Grapevine::Vector3 Type;
            static std::string name() {return "Vector3";}
            static Type *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                 return new Type(args[0].As<v8::Number>()->Value(), args[1].As<v8::Number>()->Value(), args[2].As<v8::Number>()->Value());
            }
        };

        template<> struct GrapevineVector<4>
        {
            typedef Grapevine::Vector4 Type;
            static std::string name() {return "Vector4";}
            static Type *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                 return new Type(args[0].As<v8::Number>()->Value(), args[1].As<v8::Number>()->Value(), args[2].As<v8::Number>()->Value(), args[3].As<v8::Number>()->Value());
            }
        };

        template<int N, int I> void GrapevineVector_setter(typename GrapevineVector<N>::Type &vec, float value) {}
        template<> void GrapevineVector_setter<2, 0>(GrapevineVector<2>::Type &vec, float value);// { vec.x(value); }
        template<> void GrapevineVector_setter<2, 1>(GrapevineVector<2>::Type &vec, float value);// { vec.y(value); }
        template<> void GrapevineVector_setter<3, 0>(GrapevineVector<3>::Type &vec, float value);// { vec.x(value); }
        template<> void GrapevineVector_setter<3, 1>(GrapevineVector<3>::Type &vec, float value);// { vec.y(value); }
        template<> void GrapevineVector_setter<3, 2>(GrapevineVector<3>::Type &vec, float value);// { vec.z(value); }
        template<> void GrapevineVector_setter<4, 0>(GrapevineVector<4>::Type &vec, float value);// { vec.x(value); }
        template<> void GrapevineVector_setter<4, 1>(GrapevineVector<4>::Type &vec, float value);// { vec.y(value); }
        template<> void GrapevineVector_setter<4, 2>(GrapevineVector<4>::Type &vec, float value);// { vec.z(value); }
        template<> void GrapevineVector_setter<4, 3>(GrapevineVector<4>::Type &vec, float value);// { vec.w(value); }

        template<int N>
        class Vector : public ExportedClass<Vector<N>, void, typename GrapevineVector<N>::Type>
        {
            typedef Vector<N> Type;
            public:
            Vector() : ExportedClass<Vector<N>, void, typename GrapevineVector<N>::Type>("grapevine", GrapevineVector<N>::name()) {}

            void initializeClass(v8::Local<v8::FunctionTemplate> vectorTemplate)
            {
                switch(N)
                {
                    case 4: this->template registerGetterSetter<Type::getter<3>, Type::setter<3> >("w");
                    case 3: this->template registerGetterSetter<Type::getter<2>, Type::setter<2> >("z");
                    case 2: this->template registerGetterSetter<Type::getter<1>, Type::setter<1> >("y");
                            this->template registerGetterSetter<Type::getter<0>, Type::setter<0> >("x");
                }
            }

            typename GrapevineVector<N>::Type *construct(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                if(args.Length() != N)
                {
                    LOGE("Invalid amount of arguments passed to Vector%i constructor: %i", N, args.Length());
                    return NULL;
                }

                for(int i = 0; i < N; i++)
                {
                    if(!args[0]->IsNumber())
                    {
                        LOGE("Non-number passed as the argument %i", i);
                        return NULL;
                    }
                }

                return GrapevineVector<N>::construct(args);
            }

            template<int I>
            static void getter(typename GrapevineVector<N>::Type *instance, const std::string &name, const v8::PropertyCallbackInfo<v8::Value>& info)
            {
                info.GetReturnValue().Set(v8::Number::New(v8::Isolate::GetCurrent(), (*instance)[I]));
            }

            template<int I>
            static void setter(typename GrapevineVector<N>::Type *instance, const std::string &name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
            {
                GrapevineVector_setter<N, I>(*instance, value.As<v8::Number>()->Value());
            }
        };
    }
}
#endif