#ifndef GRAPEVINE_ANIMATION_BEZIERCOMPLEXPATH
#define GRAPEVINE_ANIMATION_BEZIERCOMPLEXPATH

#include <vector>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Animation/Path.hpp>

namespace Grapevine
{

    template <typename T>
    class BezierComplexPathPrivate: public AbstractPathPrivate<T>
    {
    public:
        //CLASS_UNCOPYABLE(BezierComplexPathPrivate)

        BezierComplexPathPrivate(std::vector<T> controlPoints):
            AbstractPathPrivate<T>(),
            controlPoints_(controlPoints)
        {
        }

        virtual ~BezierComplexPathPrivate()
        {
        }

        virtual T value(float offset) const
        {
            float t = Util::clamp(offset, 0.0f, 1.0f);
            float oneMinusT = (1 - t);
            std::vector<T> temp(controlPoints_);
            std::vector<T> res;
            while(temp.size()>1)
            {
                for(typename std::vector<T>::size_type i=0;i<temp.size()-1;i++)
                {
                    T v=T(oneMinusT * temp[i]+t*temp[i+1]);
                    res.push_back(v);
                }
                temp.erase(temp.begin());
                if(res.size()>1)
                {
                    temp.clear();
                    temp.swap(res);
                }
            }
            return res[0];
        }

    private:
        std::vector<T> controlPoints_;
    };

    template <typename T>
    class BezierComplexPath: public Path<T>
    {
    public:
        //CLASS_COPYABLE(BezierComplexPath)

        BezierComplexPath(std::vector<T> controlPoints):
            Path<T>(new BezierComplexPathPrivate<T>(controlPoints))
        {
        }

        ~BezierComplexPath()
        {
        }
    };
}

#endif
