/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_DYNAMICMESH
#define GRAPEVINE_MODEL_DYNAMICMESH

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Animation/ControlPoint.hpp>
#include <string>
#include <vector>

namespace Grapevine
{
    /**
     * @class DynamicMesh
     *
     * DynamicMesh is a class that handles dynamically changing meshes.
     */
    class DynamicMesh
    {
    public:
        CLASS_COPYABLE(DynamicMesh);

        /**
         * Contructs dynamic mesh using given control points as grid.
         *
         * @param [in]  rowsColsGrid    2 dimensional vector, first dimension being
         *                              row and second dimension col
         *                              @note You must have at least grid of 2x2 to
         *                              contruct this class.
         */
        DynamicMesh(std::vector<std::vector<ControlPoint> > const& rowsColsGrid);
        ~DynamicMesh();

        /**
         * Contructs dynamic mesh using given control points as grid.
         *
         * @note    Must be called in main thread!
         */
        void update();

        DynamicMesh();

        float boundingSphereRadius() const;

    private:
        friend class MeshResource;
        class DynamicMeshPrivate* private_;
    };
}

#endif
