/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ModelData.hpp"
#include "Grapevine/Model/MeshResource.hpp"
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Material/MaterialAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>
#include <vector>

namespace Grapevine
{
    ModelData::ModelData()
    {
    }

    ModelData::~ModelData()
    {
    }

    void ModelData::I()
    {
        Log::I("vertices:%d, faces:%d", faces_.size(), vertices_.size());
    }

    /**
     * Calculates the normal vectors.
     */
    void ModelData::calculateNormals()
    {
        Vector3 n;

        for (std::vector<Object>::iterator iter = objects_.begin(); iter != objects_.end(); iter++)
        {
            Object &object = *iter;

            for (std::vector<Face>::const_iterator fIter = object.faces.begin();
                 fIter != object.faces.end(); fIter++) {
                const Face &face = *fIter;
                Vertex &a = object.vertices[face.a];
                Vertex &b = object.vertices[face.b];
                Vertex &c = object.vertices[face.c];

                // Calculate normal
                n = Vector3::cross(b.v - a.v, c.v - a.v);
                n.normalize();
                a.n += n;
                b.n += n;
                c.n += n;

                Vector3 q1(b.v - a.v);
                Vector3 q2(c.v - a.v);
                Vector2 u1(b.t - a.t);
                Vector2 u2(c.t - a.t);
                float det = (u1.x() * u2.y() - u2.x() * u1.y());

                // Calculate tangent
                Vector3 nt;
                nt.x(det * (u2.y() * q1.x() - u1.y() * q2.x()));
                nt.y(det * (u2.y() * q1.y() - u1.y() * q2.y()));
                nt.z(det * (u2.y() * q1.z() - u1.y() * q2.z()));
                nt.normalize();
                a.nt += nt;
                b.nt += nt;
                c.nt += nt;

                // Calculate bitangent
                Vector3 nb;
                nb.x(det * (u1.x() * q2.x() - u2.x() * q1.x()));
                nb.y(det * (u1.x() * q2.y() - u2.x() * q1.y()));
                nb.z(det * (u1.x() * q2.z() - u2.x() * q1.z()));
                nb.normalize();
                a.nb += nb;
                b.nb += nb;
                c.nb += nb;
            }

            for (std::vector<Vertex>::iterator vIter = object.vertices.begin(); vIter != object.vertices.end(); vIter++)
            {
                Vertex &v = *vIter;

                // Orthogonalize
                v.nt = (v.nt - v.n * Vector3::dot(v.nt, v.n));
                v.n.normalize();
                v.nt.normalize();
                v.nb.normalize();
            }
        }
    }

    /**
     * Converts a model to a mesh using the default attributes bindings. Default
     * attributes and their order is: Position, Normal, Texcoord
     *
     * @param mesh the mesh to populate
     */
    void ModelData::convertMaterials(MeshResource& mesh)
    {
        // NOTE: now supporting only one material (the first on found from ModelData) TODO: POLISH!
        for (std::vector<Material>::iterator iter = materials_.begin(); iter != materials_.end(); iter++)
        {
            Material &material = materials_.at(0);

            MaterialResource* tmp = new MaterialResource(material.specularFactor, material.specularGlossiness);
            if (material.colorMapName.size() > 0)
            {
                TextureAsset colorMap(material.colorMapName);
                tmp->setColorMap(ReferencedPointer<TextureResource>(colorMap.resource()));
            }

            if (material.normalMapName.size() > 0)
            {
                TextureAsset normalMap(material.normalMapName);
                tmp->setNormalMap(ReferencedPointer<TextureResource>(normalMap.resource()));
            }

            tmp->setReflectionFactor(material.reflectionFactor);

            mesh.setMaterialResource(tmp);

            break; // NOTE: *look above NOTE*
        }
    }

    /**
     * Converts a model to a mesh using custom attribute bindings.
     *
     * The attribute bindings are pairs of <AttributeType::Enum, dimensionality>.
     *
     * @param mesh the mesh to populate
     * @param attribs the attribute bindings to use
     */
    void ModelData::convertToMesh(MeshResource& mesh,
        const std::vector<std::pair<AttributeType::Enum, int> > &attribs)
    {
        std::vector<int> format;
        int pPos = -1;
        int nPos = -1;
        int tPos = -1;
        int ntPos = -1;
        int nbPos = -1;

        mesh.reset();
        for (std::vector<std::pair<AttributeType::Enum, int> >::const_iterator iter
             = attribs.begin(); iter != attribs.end(); iter++)
         {

            format.push_back(iter->second);
            if (iter->first == AttributeType::Position)
            {
                pPos = iter - attribs.begin();
            }
            else if (iter->first == AttributeType::Normal)
            {
                nPos = iter - attribs.begin();
            }
            else if (iter->first == AttributeType::Texcoord)
            {
                tPos = iter - attribs.begin();
            }
            else if (iter->first == AttributeType::Tangent)
            {
                ntPos = iter - attribs.begin();
            }
            else if (iter->first == AttributeType::Bitangent)
            {
                nbPos = iter - attribs.begin();
            }
        }

        mesh.setVertexFormat(format);
        float tmp = 0.0f;
        for (std::vector<Object>::const_iterator iter = objects_.begin();
             iter != objects_.end(); iter++)
         {
            if ((*iter).boundingSphereRadius > tmp)
            {
                tmp = (*iter).boundingSphereRadius;
            }
            appendObjectToMesh(*iter, mesh, pPos, nPos, tPos, ntPos, nbPos);
        }
        mesh.setBoundingSphereRadius(tmp);
    }

    /**
     * Appends the vertices of a Model::Object to a Mesh.
     *
     * @param object the object to append
     * @param mesh the mesh to append to
     * @param pPos the attribute position for position attribute
     * @param nPos the attribute position for normal attribute
     * @param tPos the attribute position for texcoord attribute
     */
    void ModelData::appendObjectToMesh(const Object& object, MeshResource& mesh,
        int pPos, int nPos, int tPos, int ntPos, int nbPos)
    {
        mesh.setLocalMatrix(object.localTransform_);

        size_t faceCount = object.faces.size();

        for(size_t i = 0; i < 3 * faceCount; i += 3)
        {
            const Face &face = object.faces[i / 3];
            const Vertex &a = object.vertices[face.a];
            const Vertex &b = object.vertices[face.b];
            const Vertex &c = object.vertices[face.c];

            mesh.nextVertex();
            if (pPos >= 0)
                mesh.setAttribute(pPos, a.v);
            if (nPos >= 0)
                mesh.setAttribute(nPos, a.n);
            if (tPos >= 0)
                mesh.setAttribute(tPos, a.t);
            if (ntPos >= 0)
                mesh.setAttribute(ntPos, a.nt);
            if (nbPos >= 0)
                mesh.setAttribute(nbPos, a.nb);

            mesh.nextVertex();
            if (pPos >= 0)
                mesh.setAttribute(pPos, b.v);
            if (nPos >= 0)
                mesh.setAttribute(nPos, b.n);
            if (tPos >= 0)
                mesh.setAttribute(tPos, b.t);
            if (ntPos >= 0)
                mesh.setAttribute(ntPos, b.nt);
            if (nbPos >= 0)
                mesh.setAttribute(nbPos, b.nb);

            mesh.nextVertex();
            if (pPos >= 0)
                mesh.setAttribute(pPos, c.v);
            if (nPos >= 0)
                mesh.setAttribute(nPos, c.n);
            if (tPos >= 0)
                mesh.setAttribute(tPos, c.t);
            if (ntPos >= 0)
                mesh.setAttribute(ntPos, c.nt);
            if (nbPos >= 0)
                mesh.setAttribute(nbPos, c.nb);
        }
    }
}
