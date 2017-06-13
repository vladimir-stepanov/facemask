/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_META_META
#define GRAPEVINE_META_META

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <typeinfo>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>

//#define M_DEBUG

namespace Grapevine
{

    namespace Meta
    {

        // -------------------------------------------------------------------------
        // Function<T>
        // -------------------------------------------------------------------------

        template <typename T>
        class Function
        {
        public:
            Function()
            {
            }

            virtual ~Function()
            {
            }

            virtual T evaluate() = 0;
        };

        // -------------------------------------------------------------------------
        // FunctionTemplate<T, B>
        // -------------------------------------------------------------------------

        template <typename T, typename B>
        class FunctionTemplate : public Function<T>
        {
        protected:
            B binding_;

        public:
            FunctionTemplate(B&& binding)
            : binding_(binding)
            {
            }

            ~FunctionTemplate()
            {
            }

            T evaluate()
            {
                return binding_();
            }
        };

        // -------------------------------------------------------------------------
        // AbstractValue
        // -------------------------------------------------------------------------

        class AbstractValue
        {
        protected:
            std::string name_;

        public:
            bool require_update_;
            std::set<std::shared_ptr<AbstractValue>> children_;

            AbstractValue(const std::string name)
            : name_(name), require_update_(true)
            {
            }

            virtual ~AbstractValue()
            {
            }

            virtual const std::type_info& type() const = 0;

            const std::string& name() const
            {
                return name_;
            }

            void link(std::shared_ptr<AbstractValue>& child)
            {
                if (child.get() != this)
                {
                    children_.insert(child);
                }
            }

            void unlink(std::shared_ptr<AbstractValue>& child)
            {
                children_.erase(child);
            }

            void requireUpdate()
            {
                if (!require_update_)
                {
                    for (auto child : children_)
                    {
                        child->requireUpdate();
                    }

                    require_update_ = true;
                }
            }
        };

        // -------------------------------------------------------------------------
        // ValueTemplate<T>
        // -------------------------------------------------------------------------

        template <typename T>
        class ValueTemplate : public AbstractValue
        {
        protected:
            T data_;
            std::unique_ptr<Function<T>> function_;

        public:
            ValueTemplate(const std::string& name, const T& data)
            : AbstractValue(name), data_(data)
            {
            }

            ~ValueTemplate()
            {
            }

            const std::type_info& type() const
            {
                return typeid(data_);
            }

            void bind(Function<T>* func)
            {
                require_update_ = false;
                requireUpdate();

                if (func)
                {
                    function_ = std::unique_ptr<Function<T>>(func);
                    require_update_ = true;
                }
                else
                {
                    function_.reset();
                }
            }

            void set(const T& data)
            {
                bind(NULL);
                data_ = data;
                require_update_ = false;
                requireUpdate();
            }

            const T& get()
            {
                if (function_ && require_update_)
                {
                    require_update_ = false;
                    data_ = function_->evaluate();
                }
                return data_;
            }
        };

        // -------------------------------------------------------------------------
        // Class
        // -------------------------------------------------------------------------

        class Class
        {
        protected:
            std::list<std::shared_ptr<AbstractValue>> values_;

            template <typename T>
            friend class Value;

            Class()
            {
            }

            virtual ~Class()
            {
            }

        public:
            auto begin() const -> decltype(values_.begin())
            {
                return values_.begin();
            }

            auto end() const -> decltype(values_.end())
            {
                return values_.end();
            }
        };

        // -------------------------------------------------------------------------
        // Value<T>
        // -------------------------------------------------------------------------

        template <typename T>
        class Value
        {
        protected:
            bool is_reference_;
            std::shared_ptr<AbstractValue> pointer_;
            std::vector<std::shared_ptr<AbstractValue>> parents_;

            template <typename>
            friend class Value;

            ValueTemplate<T>& getTemplate() const
            {
                return *reinterpret_cast<ValueTemplate<T>*>(pointer_.get());
            }

            void process()
            {
            }

            template <typename A>
            void process(Value<A>& argument)
            {
                // terminate recursion / process current argument
                argument.link(pointer_);
                parents_.emplace_back(argument.pointer_);
            }

            template <typename A>
            void process(A& argument)
            {
                // terminate recursion / process current argument
                // do nothing for non-value arguments
            }

            template <typename A, typename ...As>
            void process(A&& argument, As&&... arguments)
            {
                // process current argument
                process(std::forward<A>(argument));

                // recursively process remaining arguments
                process(std::forward<As>(arguments)...);
            }

            void operator = (const Value&)
            {
            }

        public:
            explicit Value(Class* container, const std::string& name, const T& data)
            : is_reference_(false)
            {
#ifdef M_DEBUG
                printf("Constructor: \"%s\".\n", name.c_str());
#endif
                AbstractValue* p = new ValueTemplate<T>(name, data);
                pointer_ = std::shared_ptr<AbstractValue>(p);
                if (container)
                {
                    container->values_.push_back(pointer_);
                }
            }

            explicit Value(Class* container, const Value& value)
            : is_reference_(true)
            {
#ifdef M_DEBUG
                printf("Constructor: Value:\"%s\" (reference).\n", value.name().c_str());
#endif
                pointer_ = value.pointer_;
                if (container)
                {
                    container->values_.push_back(pointer_);
                }
            }

            Value(const Value& value)
            {
#ifdef M_DEBUG
                printf("Constructor: COPY.\n");
#endif
                is_reference_ = value.is_reference_;
                pointer_ = value.pointer_;
                parents_ = value.parents_;
            }

            ~Value()
            {
#ifdef M_DEBUG
                printf("Destructor: \"%s\" %s\n", name().c_str(), is_reference_ ? "(reference)" : "");
#endif

                for (auto& parent : parents_)
                {
#ifdef M_DEBUG
                    printf("  - %s\n", parent->name().c_str());
#endif
                    parent->unlink(pointer_);
                }
                parents_.clear();
            }

            void link(std::shared_ptr<AbstractValue>& p)
            {
                pointer_->link(p);
            }

            const std::type_info& type() const
            {
                return typeid(T);
            }

            const std::string& name() const
            {
                return pointer_->name();
            }

            bool isReference() const
            {
                return is_reference_;
            }

            template <typename F, typename ...A>
            void bind(F f, A&&... a)
            {
                // process arguments recursively
                process(a...);

                auto binding = std::bind(f, a...);
                Function<T>* func = new FunctionTemplate<T, decltype(binding)>(std::move(binding));
                getTemplate().bind(func);
            }

            void unbind()
            {
                getTemplate().bind(NULL);

                for (auto& parent : parents_)
                {
                    parent->unlink(pointer_);
                }
                parents_.clear();
            }

            void operator = (const T& data)
            {
                getTemplate().set(data);
            }

            operator const T& () const
            {
                return getTemplate().get();
            }

            const T* operator -> () const
            {
                return &getTemplate().get();
            }
        };

        // -------------------------------------------------------------------------
        // meta_typeid
        // -------------------------------------------------------------------------

        inline const std::type_info& meta_typeid(const std::shared_ptr<AbstractValue>& value)
        {
            return value->type();
        }

        // -------------------------------------------------------------------------
        // meta_cast
        // -------------------------------------------------------------------------

        template <typename T>
        const T& meta_cast(const std::shared_ptr<AbstractValue>& value)
        {
            if (meta_typeid(value) != typeid(T))
                throw Grapevine::Exception("Incorrect type.");

            Meta::ValueTemplate<T>* x = reinterpret_cast<Meta::ValueTemplate<T>*>(value.get());
            return x->get();
        }

    } // namespace Meta

    using Meta::meta_cast;

    typedef Meta::Value<int> MetaInt;
    typedef Meta::Value<float> MetaFloat;
    typedef Meta::Value<Grapevine::Vector2> MetaVector2;
    typedef Meta::Value<Grapevine::Vector3> MetaVector3;
    typedef Meta::Value<Grapevine::Vector4> MetaVector4;
    typedef Meta::Value<Grapevine::Matrix2> MetaMatrix2;
    typedef Meta::Value<Grapevine::Matrix3> MetaMatrix3;
    typedef Meta::Value<Grapevine::Matrix4> MetaMatrix4;

} // namespace Grapevine

#endif
