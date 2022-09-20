#include "TestMatrixMath.h"
#include "Matrix.h"
#include "Vector.h"
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

        std::cout << "cool\n";

    }
}



void TestMatrixMath()
{
    TestMatrix2x2f();
}
}