#include "bindings/Vector.hpp"

namespace V8
{
    namespace GrapevineV8
    {
        using namespace Grapevine;
        template<> void GrapevineVector_setter<2, 0>(GrapevineVector<2>::Type &vec, float value) { vec.x(value); }
        template<> void GrapevineVector_setter<2, 1>(GrapevineVector<2>::Type &vec, float value) { vec.y(value); }
        template<> void GrapevineVector_setter<3, 0>(GrapevineVector<3>::Type &vec, float value) { vec.x(value); }
        template<> void GrapevineVector_setter<3, 1>(GrapevineVector<3>::Type &vec, float value) { vec.y(value); }
        template<> void GrapevineVector_setter<3, 2>(GrapevineVector<3>::Type &vec, float value) { vec.z(value); }
        template<> void GrapevineVector_setter<4, 0>(GrapevineVector<4>::Type &vec, float value) { vec.x(value); }
        template<> void GrapevineVector_setter<4, 1>(GrapevineVector<4>::Type &vec, float value) { vec.y(value); }
        template<> void GrapevineVector_setter<4, 2>(GrapevineVector<4>::Type &vec, float value) { vec.z(value); }
        template<> void GrapevineVector_setter<4, 3>(GrapevineVector<4>::Type &vec, float value) { vec.w(value); }

        //instantiate the 3 different floating point vector types
        static Vector<2> vector2_;
        static Vector<3> vector3_;
        static Vector<4> vector4_;

    }
}
