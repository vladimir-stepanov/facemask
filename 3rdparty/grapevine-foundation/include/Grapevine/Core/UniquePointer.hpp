/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_UNIQUEPOINTER_HPP
#define GRAPEVINE_CORE_UNIQUEPOINTER_HPP

namespace Grapevine
{
    /**
     * @class UniquePointer
     *
     */
    template <class T> class UniquePointer
    {
    public:
        CLASS_UNCOPYABLE(UniquePointer);

        /**
         * Creates rectangular mesh asset.
         *
         * @param [in]  instance    Instance to manage. Instance will be
         *                          deleted when UniquePointer's destrcutor
         *                          is called.
         * @param [in]  isArray     If instance is array (for delete [] usage)
         */
        UniquePointer(T* instance, bool isArray = false):
            instance_(instance),
            isArray_(isArray)
        {
        }

        ~UniquePointer()
        {
            if (instance_ != NULL)
            {
                if (isArray_ == false)
                {
                    delete instance_;
                }
                else
                {
                    delete [] instance_;
                }
                instance_ = NULL;
            }
        }

        T* pointer() const
        {
            return instance_;
        }

    private:
        T* instance_;
        bool isArray_;
    };
}

#endif
