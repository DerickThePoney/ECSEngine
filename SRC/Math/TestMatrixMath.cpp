#include "TestMatrixMath.h"
#include "Matrix.h"
#include "Vector.h"
#include "MatrixUtils.h"
#include <iostream>



namespace ECSEngine
{
void TestMatrix2x2f()
{
    {
        mat2 t0;
        mat2 t1(2.f);
        mat2 t2(vec2(0.f, -1.f), vec2(-1.f, 0.f));

        mat2 t3 = mat2::Identity();

        mat2 t4 = t3 * t2;
        t4 *= t3;

        t4 = t4 * 2.f;

        std::cout << "test over\n";
    }
}

void TestMatrix3x3f()
{
    {
        mat3 t0(vec3(1.f, 0.f, 5.f), vec3(2.f, 1.f, 6.f), vec3(3.f,4.f,0.f));

        mat3 t0_i = Invert(t0);

        mat3 t1 = mat3::Identity() * t0_i;
        
        std::cout << "test over\n";
    }
}



void TestMatrixMath()
{
    TestMatrix2x2f();
    TestMatrix3x3f();
}
}