/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Model/DynamicMesh.hpp"
#include "DynamicMeshPrivate.hpp"

namespace Grapevine
{
    DynamicMesh::DynamicMesh(std::vector<std::vector<ControlPoint> > const& rowsColsGrid):
        private_(new DynamicMeshPrivate(rowsColsGrid))
    {
        private_->increment();
        private_->update();
    }

    DynamicMesh::DynamicMesh(DynamicMesh const& r):
        private_(r.private_)
    {
        private_->increment();
    }

    DynamicMesh::DynamicMesh():
        private_(NULL)
    {
    }

    DynamicMesh::~DynamicMesh()
    {
        if (private_ != NULL)
        {
            private_->decrement();
            private_ = NULL;
        }
    }

    void DynamicMesh::update()
    {
        if (private_ != NULL)
        {
            private_->update();
        }
    }

    DynamicMesh& DynamicMesh::operator=(DynamicMesh const& r)
    {
        if (private_ != NULL)
        {
            if (this != &r)
            {
                if (private_ != NULL)
                {
                    private_->decrement();
                    private_ = NULL;
                }

                if (r.private_ != NULL)
                {
                    private_ = r.private_;
                    private_->increment();
                }
            }
        }
        return *this;
    }

    float DynamicMesh::boundingSphereRadius() const
    {
        return private_->boundingSphereRadius();
    }
}
