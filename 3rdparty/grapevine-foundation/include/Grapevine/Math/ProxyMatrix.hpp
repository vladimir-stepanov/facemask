/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_PROXYMATRIX
#define GRAPEVINE_MATH_PROXYMATRIX

#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    template<typename T, unsigned int dimension>
    class ProxyMatrix
    {
    public:
        //CLASS_COPYABLE(ProxyMatrix)

        ProxyMatrix(T* data)
        {
            data_ = data;
        }

        ~ProxyMatrix()
        {
            data_ = 0;
        }

        T& operator[](int index)
        {
            return data_[index * dimension];
        }

        const T& operator[](int index) const
        {
            return data_[index * dimension];
        }

    private:
        T* data_;
    };
}

#endif
