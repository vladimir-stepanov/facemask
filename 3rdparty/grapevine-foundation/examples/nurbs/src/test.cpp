#include <cstdio>

#include "Grapevine/Math/Nurbs.hpp"

using namespace Grapevine;

void printBasisFunctions()
{
    static const float k[] = {  0, 1, 2, 3, 4, 4.1, 5.1, 6.1, 7.1};
    std::vector<float> knots(k, k + sizeof(k) / sizeof(k[0]));


    int max = 2000;
    int controlPoints = knots.size() - NurbsBasis<float, 2>::order_;
    for(int cp = 0; cp < controlPoints; cp++)
    {
        for(int i = 0; i < max + 1; i++)
        {
            float u = float(i * knots.back()) / max;

            printf("%f  %f\n", u, NurbsBasis<float, 2>::eval(knots, cp, u));
        }
    }
}

void printCurve()
{
    std::vector<Vector2> controlPoints;
    controlPoints.push_back(1);
    controlPoints.push_back(2);
    controlPoints.push_back(3);
    controlPoints.push_back(4);
    controlPoints.push_back(Vector2(5, 0));
    controlPoints.push_back(6);
    controlPoints.push_back(7);
    controlPoints.push_back(8);

    QuadraticNurbs<Vector2>::Curve curve(controlPoints.size());

    for(int i = 0; i < controlPoints.size(); i++)
    {
        Nurbs<Vector2>::ControlPoint &cp = curve.controlPoint(i);
        cp.value(controlPoints[i]);
        //if(i == controlPoints.size() - 1) cp.weight(25.f);
    }

    int max = 40;

    //use remapper to equalize the spacing of the evaluated points along the curve
    Nurbs<Vector2>::Remapper remapper;
    curve.updateRemapper(remapper, 16);

    for(int i = 0; i < max; i++)
    {
        float u = float(i)/max;
        Vector2 v = curve.eval(remapper.remap(u));
        printf("%f  %f\n", v.x(), v.y());
    }
}

void printSurface()
{
    QuadraticNurbs<float>::Surface surface(5, 5);

    surface.controlPoint(2, 2).value(1);
    surface.controlPoint(2, 0).value(-1);
    surface.controlPoint(0, 2).value(-1);

    int max = 20;
    for(int vi = 0; vi < max + 1; vi++)
    for(int ui = 0; ui < max + 1; ui++)
    {
        float u = float(ui)/max, v = float(vi)/max;
        printf("%f %f %f\n", u, v, surface.eval(2*u-0.5, 2*v-0.5));
    }
}

int main(int argc, char *argv[])
{
    int testCase = 0;
    if(argc > 1) testCase = atoi(argv[1]);
    switch(testCase)
    {
        case 0: printSurface(); break;
        case 1: printCurve(); break;
        case 2: printBasisFunctions(); break;
    }
    return 0;
}
