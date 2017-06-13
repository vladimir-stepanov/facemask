/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MODEL_MODELASSET
#define GRAPEVINE_MODEL_MODELASSET

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Math/Box.hpp>
#include <string>

namespace Grapevine
{
    class ModelResource;

    /**
     * @class ModelAsset
     *
     * ModelAsset is a class that handles the model file loading.
     */
    class ModelAsset
    {
    public:
        CLASS_COPYABLE(ModelAsset);

        /**
         * Creates model asset. References the resource alive.
         *
         * @note        Duplicate loading is handled internally, so
         *              it is memory vice OK to create two same assets.
         *
         * @param [in]  filename        Filename to load.
         */
        ModelAsset(std::string const& filename);

        /**
         * Unreferences the resource.
         */
        ~ModelAsset();

        /**
         * @note        Ment for engine's internal use only!
         *              Will be deprecating in future versions!
         */
        ModelResource* resource();

        Box boundingBox();

    private:
        ModelResource* resource_;
    };
}

#endif
