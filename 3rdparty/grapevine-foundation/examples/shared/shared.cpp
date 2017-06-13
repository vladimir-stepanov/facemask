#include <memory>

namespace Works
{
    template <typename T>
    class Shared : public std::shared_ptr<T>
    {
    public:

        template <typename ...A>
        static Shared<T> make(A&&... a)
        {
            return new T(std::forward<A>(a)...);
        }

        Shared(T *ptr)
        : std::shared_ptr<T>(ptr)
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        template <typename U>
        Shared(const Shared<U> &shared)
        : std::shared_ptr<T>(std::dynamic_pointer_cast<T>(shared))
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        Shared(const Shared& shared)
        {
            *this = shared;
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        ~Shared()
        {
            //printf("  ~Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        const Shared& operator = (const Shared& shared)
        {
            std::shared_ptr<T>::operator = (shared);
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        template <typename U>
        const Shared& operator = (const Shared<U>& shared)
        {
            std::shared_ptr<T>::operator = (std::static_pointer_cast<T>(shared));
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        unsigned int count() const
        {
            return int(this->use_count());
        }

        unsigned int id() const
        {
            return int(this->id());
        }

        bool isNull() const
        {
            return !std::shared_ptr<T>::get();
        }
    };
}

namespace Fails
{
    template <typename T>
    class Shared : public std::shared_ptr<T>
    {
    public:

        template <typename ...A>
        Shared<T>(A&&... a)
        : std::shared_ptr<T>(new T(std::forward<A>(a)...))
        {
        }

        Shared(T *ptr)
        : std::shared_ptr<T>(ptr)
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        template <typename U>
        Shared(const Shared<U> &shared)
        : std::shared_ptr<T>(std::dynamic_pointer_cast<T>(shared))
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        Shared(const Shared& shared)
        {
            *this = shared;
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        ~Shared()
        {
            //printf("  ~Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        const Shared& operator = (const Shared& shared)
        {
            std::shared_ptr<T>::operator = (shared);
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        template <typename U>
        const Shared& operator = (const Shared<U>& shared)
        {
            std::shared_ptr<T>::operator = (std::static_pointer_cast<T>(shared));
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        unsigned int count() const
        {
            return int(this->use_count());
        }

        unsigned int id() const
        {
            return int(this->id());
        }

        bool isNull() const
        {
            return !std::shared_ptr<T>::get();
        }
    };
}

class Base
{
public:
    Base() {}
    virtual ~Base() {}
};

class Derived
{
public:
    Derived() {}
    virtual ~Derived() {}

};

class UnrelatedClass
{
public:
    UnrelatedClass(Works::Shared<Base> a) {}
    UnrelatedClass(Fails::Shared<Base> a) {}
    virtual ~UnrelatedClass() {}

};

int main(int argc, char *argv[])
{
    {
        auto derived = Works::Shared<Derived>::make();
        Works::Shared<Base> base = Works::Shared<Derived>::make();
        Works::Shared<UnrelatedClass> unrelated1(base);
        Works::Shared<UnrelatedClass> unrelated2(derived);
    }

    {
        Fails::Shared<Derived> derived;
        Fails::Shared<Base> base = Fails::Shared<Derived>();
        Fails::Shared<UnrelatedClass> unrelated1(base);
        Fails::Shared<UnrelatedClass> unrelated2(derived);
    }

}