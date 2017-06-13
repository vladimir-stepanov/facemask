/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_DYNAMICMESHPRIVATE
#define GRAPEVINE_MODEL_DYNAMICMESHPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Animation/ControlPoint.hpp>
#include <vector>
#include <Grapevine/GPU/GL.hpp>

namespace Grapevine
{
    class DynamicMeshPrivate: public Referenced
    {
    public:
        CLASS_UNCOPYABLE(DynamicMeshPrivate);

        DynamicMeshPrivate(std::vector<std::vector<ControlPoint> > const& rowsColsGrid);
        virtual ~DynamicMeshPrivate();
        void update();
        void draw(GLint positionLocation, GLint normalLocation, GLint textureUVLocation);
        float boundingSphereRadius() const;

    private:
        void checkGrid();
        void createVBO();
        std::vector<std::vector <ControlPoint> > rowsColsGrid_;
        unsigned int vertexCount_;
        GLuint vbo_;
    };
}

#endif
