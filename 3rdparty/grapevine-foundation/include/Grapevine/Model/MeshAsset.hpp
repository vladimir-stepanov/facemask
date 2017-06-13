/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MESHASSET
#define GRAPEVINE_MODEL_MESHASSET

#include <Grapevine/Math/Box.hpp>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Buffer.hpp>
#include <Grapevine/Model/DynamicMesh.hpp>
#include <string>
#include <stdint.h>

namespace Grapevine
{
    class MeshResource;

    /**
     * @class MeshAsset
     *
     * MeshAsset is a class that handles the mesh file or mesh data given
     * by other ways.
     */
    class MeshAsset
    {
    public:
        CLASS_COPYABLE(MeshAsset);

        /**
         * Creates mesh asset. Holds the resource alive.
         *
         * @note        Duplicate loading is handled internally, so
         *              it is memory vice OK to create two same assets.
         *
         * @param [in]  filename        Filename to load.
         */
        MeshAsset(std::string const& filename);

        /**
         * Creates rectangular mesh asset.
         *
         * @param [in]  width           Width of the asset.
         * @param [in]  height          Height of the asset.
         * @param [in]  textureUsageX   Texture usage starting from upper left corner.
         * @param [in]  textureUsageY   Texture usage starting from upper left corner.
         * @param [in]  flipTextureX    Flip texture in X direction.
         * @param [in]  flipTextureY    Flip texture in Y direction.
         */
        MeshAsset(float width, float height, float textureUsageX = 1.0f, float textureUsageY = 1.0f, bool flipTextureX = false, bool flipTextureY = false);

        /**
         * Creates rectangular mesh asset.
         *
         * @param [in]  width           Width of the asset.
         * @param [in]  height          Height of the asset.
         * @param [in]  startPoint      Texture usage starting from upper left corner.
         * @param [in]  rectSize        Texture usage rect size
         * @param [in]  flipTextureX    Flip texture in X direction.
         * @param [in]  flipTextureY    Flip texture in Y direction.
         */
        MeshAsset(float width, float height, Vector2 const& startPoint, Vector2 const& rectSize, bool flipTextureX = false, bool flipTextureY = false);

        /**
         * Creates rectangular mesh asset.
         *
         * @param [in]  width           Width of the asset.
         * @param [in]  height          Height of the asset.
         * @param [in]  positionOffset  Offset vertices to move mesh origin.
         * @param [in]  startPoint      Texture usage starting from upper left corner.
         * @param [in]  rectSize        Texture usage rect size
         * @param [in]  flipTextureX    Flip texture in X direction.
         * @param [in]  flipTextureY    Flip texture in Y direction.
         */
        MeshAsset(float width, float height, Vector2 const& positionOffset, Vector2 const& startPoint, Vector2 const& rectSize, bool flipTextureX = false, bool flipTextureY = false);

        /**
         * Creates user data mesh asset.
         *
         * @param [in]  position        List of vertex positions (required)
         * @param [in]  uv              List of texture coordinates (optional, but if available size() must match position.size())
         * @param [in]  normal          List of normal (optional, but if available size() must match position.size())
         * @param [in]  indices         List of triangle indices (optional)
         */
        MeshAsset(std::vector<Vector3> const& position, std::vector<Vector2> const& uv, std::vector<Vector3> const& normal, std::vector<uint16_t> const &indices);

        /**
         * Creates dynamic mesh asset.
         *
         * @param [in]  dynamicMesh     Dynamic mesh to use as mesh.
         */
        MeshAsset(DynamicMesh const& dynamicMesh);

        /**
         * Creates mesh from location, normal, uv data (triangle array, 3+3+2 4byte floats per vertex)
         * @note data is copied.
         *
         * @param [in]  data
         * @param [in]  byteLength
         */
        MeshAsset(char* data, unsigned int byteSize);

        /**
         * Unreferences the resource.
         */
        ~MeshAsset();

        float boundingSphereRadius() const;
        Box const& boundingBox() const;

        /**
         * Force intialize GPU data. GPU data initialization is
         * handled automatically by drawing normally.
         *
         * @note Use this only when in need of syncronous loading in
         * thread with GL context.
         */
        void initializeGPUData();

        /**
         * @note        Ment for engine's internal use only!
         *              Will be deprecated in future!
         */
        MeshResource* resource();

    private:

        MeshAsset(std::string const& name, class ModelData* modelData);
        MeshResource* resource_;

        friend class ModelResource;
    };
}

#endif
