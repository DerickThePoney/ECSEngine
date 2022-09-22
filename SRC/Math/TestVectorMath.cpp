#include "TestVectorMath.h"
#include "Vector.h"
#include "VectorUtils.h"
#include <iostream>

namespace ECSEngine
{
template<typename VecType>
void TestVector3()
{

    {
        VecType test1(1.f, 2.f, 3.0f);
        VecType test2(2.f, 1.f, 3.0f);

        VecType somme = test1 + test2;
        std::cout << "somme=" << somme.x << "\t" << somme.y << "\t" << somme.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(1.f, 1.f, 3.0f);

        VecType soustraction = test1 - test2;
        std::cout << "soustraction=" << soustraction.x << "\t" << soustraction.y << "\t" << soustraction.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        VecType multiplication = test1 * test2;
        std::cout << "multiplication=" << multiplication.x << "\t" << multiplication.y << "\t" << multiplication.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        VecType div = test1 / test2;
        std::cout << "div=" << div.x << "\t" << div.y << "\t" << div.z << "\n";
    }

    {
        VecType test1(1.f, 2.f, 3.0f);
        VecType test2(2.f, 1.f, 3.0f);

        test1 += test2;
        std::cout << "add test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(1.f, 1.f, 3.0f);

        test1 -= test2;
        std::cout << "sub test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        test1 *= test2;
        std::cout << "mult test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);

        test1 /= test2;
        std::cout << "div test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2 = test1;

        std::cout << "assign test2=" << test2.x << "\t" << test2.y << "\t" << test1.z << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);
        VecType test3(2.f, 1.f, 3.0f);

        const bool equal = test1 == test2;
        const bool diff = test1 == test3;

        std::cout << "test test1==test2 = " << equal << "\n";
        std::cout << "test test1==test3 = " << diff << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f);
        VecType test2(2.f, 2.f, 3.0f);
        VecType test3(2.f, 1.f, 3.0f);

        const bool equal = test1 != test2;
        const bool diff = test1 != test3;

        std::cout << "test test1!=test2 = " << equal << "\n";
        std::cout << "test test1!=test3 = " << diff << "\n";
    }

    {
        VecType test2(2.f, 9.f, 3.0f);

        auto test1 = test2.xx();        
        std::cout << "xx test1=" << test1.x << "\t" << test1.y << "\t" << "\n";

        test1 = test2.yy();
        std::cout << "yy test1=" << test1.x << "\t" << test1.y << "\t" << "\n";

        test1 = test2.yx();
        std::cout << "yx test1=" << test1.x << "\t" << test1.y << "\t"  << "\n";

        test1 = test2.x0();
        std::cout << "x0 test1=" << test1.x << "\t" << test1.y << "\t" << "\n";

        test1 = test2.x1();
        std::cout << "x1 test1=" << test1.x << "\t" << test1.y << "\t" << "\n";

        test1 = test2.y0();
        std::cout << "y0 test1=" << test1.x << "\t" << test1.y << "\t" << "\n";

        test1 = test2.y1();
        std::cout << "y1 test1=" << test1.x << "\t" << test1.y << "\t" << "\n";
    }

    {
        VecType test1(2.f, 2.f, 2.f);

        const float lengthSq = LengthSq(test1);
        const float length = Length(test1);

        std::cout << "LengthSq test1 = " << lengthSq << "\n";
        std::cout << "Length test1 = " << length << "\n";
    }
}


template<typename VecType>
void TestVector4()
{

    {
        VecType test1(1.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 1.f, 3.0f, 5.0f);

        VecType somme = test1 + test2;
        std::cout << "somme=" << somme.x << "\t" << somme.y << "\t" << somme.z << "\t" << somme.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(1.f, 1.f, 3.0f, 5.0f);

        VecType soustraction = test1 - test2;
        std::cout << "soustraction=" << soustraction.x << "\t" << soustraction.y << "\t" << soustraction.z << "\t" << soustraction.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        VecType multiplication = test1 * test2;
        std::cout << "multiplication=" << multiplication.x << "\t" << multiplication.y << "\t" << multiplication.z << "\t" << multiplication.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        VecType div = test1 / test2;
        std::cout << "div=" << div.x << "\t" << div.y << "\t" << div.z << "\t" << div.w << "\n";
    }

    {
        VecType test1(1.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 1.f, 3.0f, 5.0f);

        test1 += test2;
        std::cout << "add test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\t" << test1.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(1.f, 1.f, 3.0f, 5.0f);

        test1 -= test2;
        std::cout << "sub test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\t" << test1.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        test1 *= test2;
        std::cout << "mult test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\t" << test1.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);

        test1 /= test2;
        std::cout << "div test1=" << test1.x << "\t" << test1.y << "\t" << test1.z << "\t" << test1.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2 = test1;

        std::cout << "assign test2=" << test2.x << "\t" << test2.y << "\t" << test1.z << "\t" << test1.w << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);
        VecType test3(2.f, 1.f, 3.0f, 5.0f);

        const bool equal = test1 == test2;
        const bool diff = test1 == test3;

        std::cout << "test test1==test2 = " << equal << "\n";
        std::cout << "test test1==test3 = " << diff << "\n";
    }

    {
        VecType test1(2.f, 2.f, 3.0f, 5.0f);
        VecType test2(2.f, 2.f, 3.0f, 5.0f);
        VecType test3(2.f, 1.f, 3.0f, 5.0f);

        const bool equal = test1 != test2;
        const bool diff = test1 != test3;

        std::cout << "test test1!=test2 = " << equal << "\n";
        std::cout << "test test1!=test3 = " << diff << "\n";
    }

    {
        VecType test2(2.f, 9.f, 3.0f, 5.0f);

        auto test1 = test2.xx();
        std::cout << "xx test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.yy();
        std::cout << "yy test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.yx();
        std::cout << "yx test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.x0();
        std::cout << "x0 test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.x1();
        std::cout << "x1 test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.y0();
        std::cout << "y0 test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";

        test1 = test2.y1();
        std::cout << "y1 test1=" << test1.x << "\t" << test1.y << "\t"
                  << "\n";
    }

    {
        VecType test1(2.f, 2.f, 2.f, 2.f);

        const float lengthSq = LengthSq(test1);
        const float length = Length(test1);

        std::cout << "LengthSq test1 = " << lengthSq << "\n";
        std::cout << "Length test1 = " << length << "\n";
    }
}

void TestVectorMath()
{
    std::cout << "TESTING VECTOR3<FLOAT>\n";
    TestVector3<vec3>();
    {
        vec3 t(1.f, 1.f, 1.f);
        vec3 t1(1.f, 0.f, 0.f);
        vec3 tn = Normalize(t);
        const float dot = Dot(tn, t1);
        vec3 cross = Cross(tn, t1);
        std::cout << "Normalize tn=" << tn.x << "\t" << tn.y << "\t" << tn.z << "\t"
                  << "dot= " << dot << "\n"
                  << "Cross= " << cross.x << "\t" << cross.y << "\t" << cross.z << "\t"
                  << "\n";

        auto testAdd = [](const vec3& a, const vec3& b) { return a + b; };
        vec3 t2 = testAdd(vec3(1), vec3(1));
        std::cout << "testAdd tn=" << t2.x << "\t" << t2.y << "\t" << t2.z << "\t"
                  << "\n";
    }

    std::cout << "\nTESTING VECTOR3<U32>\n";
    TestVector3<uvec3>();
    std::cout << "\nTESTING VECTOR3<I32>\n";
    TestVector3<ivec3>();

    std::cout << "TESTING VECTOR4<FLOAT>\n";
    TestVector4<vec4>();
    {
        vec4 t(1.f, 1.f, 1.f, 1.f);
        vec4 t1(1.f, 0.f, 0.f, 0.f);
        vec4 tn = Normalize(t);
        const float dot = Dot(tn, t1);
        std::cout << "Normalize tn=" << tn.x << "\t" << tn.y << "\t" << tn.z << "\t" << tn.w << "\t"
                  << "dot= " << dot
                  << "\n";

        auto testAdd = [](const vec4& a, const vec4& b) { return a + b; };
        vec4 t2 = testAdd(vec4(1), vec4(1));
        std::cout << "testAdd tn=" << t2.x << "\t" << t2.y << "\t" << t2.z << "\t" << t2.w << "\t"
                  << "\n";
    }

    std::cout << "\nTESTING VECTOR4<U32>\n";
    TestVector4<uvec4>();
    std::cout << "\nTESTING VECTOR4<I32>\n";
    TestVector4<ivec4>();
}
}

