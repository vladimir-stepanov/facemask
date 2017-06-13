/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATRIXSTACK
#define GRAPEVINE_MATH_MATRIXSTACK

#include "Grapevine/Math/Matrix.hpp"
#include <vector>

namespace Grapevine
{
    template<typename T> class MatrixStack
    {
    public:
        // CLASS_COPYABLE

        // Identity matrix on the top of stack
        MatrixStack()
        {
            stack_.push_back(T());
        }

        MatrixStack(const T& matrix)
        {
            stack_.push_back(matrix);
        }

        ~MatrixStack() {}

        const T& getCurrent() const { return stack_.back(); }

        void push()
        {
            stack_.push_back(stack_.back());
        }

        void pop()
        {
            stack_.pop_back();
        }

        void loadIdentity()
        {
            stack_.back().setIdentity();
        }

        T& operator*=(const T& rhs)
        {
            T& currentMatrix = stack_.back();
            currentMatrix *= rhs;
            return currentMatrix;
        }

        void print() const
        {
            const T& currentMatrix = stack_.back();
            currentMatrix.print();
        }

        unsigned int getDepth() const
        {
            return stack_.size();
        }

    private:
        std::vector<T> stack_;
    };

    class MatrixStack4 : public MatrixStack<Matrix4>
    {
    public:
        void translate(float x, float y, float z)
        {
            *this *= Matrix::translate(x, y, z);
        }

        void scale(float x, float y, float z)
        {
            *this *= Matrix::scale(x, y, z);
        }

        void rotate(float angle, float x, float y, float z)
        {
            *this *= Matrix::rotate(angle, x, y, z);
        }

        void rotate(Matrix4 const& m)
        {
            *this *= m;
        }

        void frustum(float left, float right, float bottom, float top, float near,
            float far)
        {
            *this *= Matrix::frustum(left, right, bottom, top, near, far);
        }

        void ortho(float left, float right, float bottom, float top, float near,
            float far)
        {
            *this *= Matrix::ortho(left, right, bottom, top, near, far);
        }

        void perspective(float fovy, float aspect, float zNear, float zFar)
        {
            *this *= Matrix::perspective(fovy, aspect, zNear, zFar);
        }

        void lookAt(float eyeX, float eyeY, float eyeZ, float centerX,
            float centerY, float centerZ, float upX, float upY, float upZ)
        {
            *this *= Matrix::lookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ,
                upX, upY, upZ);
        }
    };
}

#endif
