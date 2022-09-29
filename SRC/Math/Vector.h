#pragma once
#include "VectorTypes.h"

namespace ECSEngine
{
/****************************
 * VECTOR2
 ****************************/
#pragma pack(push, 1)
template<typename T>
struct alignas(4) Vector2
{
public:
    Vector2() = default;
    explicit Vector2(T parValue)
        : x(parValue)
        , y(parValue)
    {
    }
    explicit Vector2(T parX, T parY)
        : x(parX)
        , y(parY)
    {
    }

#define MAKESCALAROP(OP)                                                                                                                                                           \
    inline Vector2 operator OP(const T parA) const                                                                                                                                      \
    {                                                                                                                                                                              \
        return Vector2(x OP parA, y OP parA);                                                                                                                                      \
    }                                                                                                                                                                              \
    inline void operator OP=(const T parA)                                                                                                                                        \
    {                                                                                                                                                                              \
        x OP= parA;                                                                                                                                                                 \
        y OP= parA;                                                                                                                                                                 \
    }

#define MAKEVECOP(OP)                                                                                                                                                              \
    inline Vector2 operator OP(const Vector2& parA) const                                                                                                                               \
    {                                                                                                                                                                              \
        return Vector2(x OP parA.x, y OP parA.y);                                                                                                                                  \
    }                                                                                                                                                                              \
    inline void operator OP=(const Vector2& parA)                                                                                                                                 \
    {                                                                                                                                                                              \
        x OP= parA.x;                                                                                                                                                               \
        y OP= parA.y;                                                                                                                                                               \
    }

#define MAKEOP(OP)                                                                                                                                                                 \
    MAKEVECOP(OP);                                                                                                                                                                 \
    MAKESCALAROP(OP);

    MAKEOP(+);
    MAKEOP(-);
    MAKEOP(/);
    MAKEOP(*);

#undef MAKEVECOP
#undef MAKESCALAROP
#undef MAKEOP

    inline bool operator==(const Vector2& parA) const
    {
        return parA.x == x && parA.y == y;
    }

    inline bool operator!=(const Vector2& parA) const
    {
        return !(*this == parA);
    }

public:
	T x = T(0);
	T y = T(0);
};

#define MAKE_EXTERN_OP(OP)                                                                                                                                                     \
    template<typename T>                                                                                                                                                           \
    inline Vector2<T> operator OP(const Vector2<T>& parA, const Vector2<T>& parB)                                                                                                  \
    {                                                                                                                                                                              \
        return Vector2<T>(parA.x OP parB.x, parA.y OP parB.y);                                                                                                                                                                      \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector2<T> operator OP(const Vector2<T>& parA, const T parB)                                                                                                            \
    {                                                                                                                                                                              \
        return Vector2<T>(parA.x OP parB, parA.y OP parB);                                                                                                         \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector2<T> operator OP(const T parB, const Vector2<T>& parA)                                                                                                            \
    {                                                                                                                                                                              \
        return parA OP parB;                                                                                                                                                       \
    }

MAKE_EXTERN_OP(+);
MAKE_EXTERN_OP(-);
MAKE_EXTERN_OP(/);
MAKE_EXTERN_OP(*);

#undef MAKE_EXTERN_OP

/****************************
 * VECTOR3
 ****************************/

template<typename T>
struct alignas(4) Vector3
{
public:
    Vector3() = default;
    explicit Vector3(T parValue)
        : x(parValue)
        , y(parValue)
        , z(parValue)
    {
    }
    explicit Vector3(T parX, T parY)
        : x(parX)
        , y(parY)
    {
    }
    explicit Vector3(T parX, T parY, T parZ)
        : x(parX)
        , y(parY)
        , z(parZ)
    {
    }
    explicit Vector3(const Vector2<T>& parVec2)
        : x(parVec2.x)
        , y(parVec2.y)
    {
    }

#define MAKESCALAROP(OP)                                                                                                                                                           \
    inline Vector3 operator OP(const T parA) const                                                                                                                                      \
    {                                                                                                                                                                              \
        return Vector3(x OP parA, y OP parA, z OP parA);                                                                                                                                      \
    }                                                                                                                                                                              \
    inline void operator OP=(const T parA)                                                                                                                                        \
    {                                                                                                                                                                              \
        x OP= parA;                                                                                                                                                                 \
        y OP= parA;                                                                                                                                                                 \
        z OP= parA;                                                                                                                                                                 \
    }

#define MAKEVECOP(OP)                                                                                                                                                              \
    inline Vector3 operator OP(const Vector3& parA) const                                                                                                                               \
    {                                                                                                                                                                              \
        return Vector3(x OP parA.x, y OP parA.y, z OP parA.z);                                                                                                                     \
    }                                                                                                                                                                              \
    inline void operator OP=(const Vector3& parA)                                                                                                                                \
    {                                                                                                                                                                              \
        x OP= parA.x;                                                                                                                                                               \
        y OP= parA.y;                                                                                                                                                               \
        z OP= parA.z;                                                                                                                                                               \
    }

#define MAKEOP(OP)                                                                                                                                                                 \
    MAKEVECOP(OP);                                                                                                                                                                 \
    MAKESCALAROP(OP);

    MAKEOP(+);
    MAKEOP(-);
    MAKEOP(/);
    MAKEOP(*);

#undef MAKEVECOP
#undef MAKESCALAROP
#undef MAKEOP

#define MAKE_SWIZZLE_OP_2(OP, A, B)                                                                                                                                                \
    inline Vector2<T> OP() const                                                                                                                                                        \
    {                                                                                                                                                                              \
        return Vector2<T>(A, B);                                                                                                                                                   \
    }

    MAKE_SWIZZLE_OP_2(xx, x, x);
    MAKE_SWIZZLE_OP_2(yy, y, y);
    MAKE_SWIZZLE_OP_2(zz, z, z);
    MAKE_SWIZZLE_OP_2(xz, x, z);
    MAKE_SWIZZLE_OP_2(yx, y, x);
    MAKE_SWIZZLE_OP_2(x0, x, T(0));
    MAKE_SWIZZLE_OP_2(x1, x, T(1));
    MAKE_SWIZZLE_OP_2(y0, y, T(0));
    MAKE_SWIZZLE_OP_2(y1, y, T(1));

#undef MAKE_SWIZZLE_OP_2

#define MAKE_SWIZZLE_OP_3(OP, A, B, C)                                                                                                                                                \
    inline Vector3 OP() const                                                                                                                                                        \
    {                                                                                                                                                                              \
        return Vector3(A, B, C);                                                                                                                                                   \
    }

    MAKE_SWIZZLE_OP_3(xxx, x, x, x);
    MAKE_SWIZZLE_OP_3(yyy, y, y, y);
    MAKE_SWIZZLE_OP_3(zzz, z, z, z);
    MAKE_SWIZZLE_OP_3(x00, x, T(0), T(0));
    MAKE_SWIZZLE_OP_3(x01, x, T(0), T(1));
    MAKE_SWIZZLE_OP_3(y00, y, T(0), T(0));
    MAKE_SWIZZLE_OP_3(y01, y, T(0), T(1));
    MAKE_SWIZZLE_OP_3(z00, z, T(0), T(0));
    MAKE_SWIZZLE_OP_3(z01, z, T(0), T(1));

    MAKE_SWIZZLE_OP_3(xy0, x, y, T(0));
    MAKE_SWIZZLE_OP_3(xy1, x, y, T(1));
    MAKE_SWIZZLE_OP_3(xz0, x, z, T(0));
    MAKE_SWIZZLE_OP_3(xz1, x, z, T(1));
          

#undef MAKE_SWIZZLE_OP_3

    inline bool operator==(const Vector3& parA) const
    {
        return parA.x == x && parA.y == y && parA.z == z;
    }

    inline bool operator!=(const Vector3& parA) const
    {
        return !(*this == parA);
    }

public:
    T x = T(0);
    T y = T(0);
    T z = T(0);
};

#define MAKE_EXTERN_OP(OP)                                                                                                                                                         \
    template<typename T>                                                                                                                                                           \
    inline Vector3<T> operator OP(const Vector3<T>& parA, const Vector3<T>& parB)                                                                                                  \
    {                                                                                                                                                                              \
        return Vector3<T>(parA.x OP parB.x, parA.y OP parB.y, parA.z OP parB.z);                                                                                                                     \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector3<T> operator OP(const Vector3<T>& parA, const T parB)                                                                                                            \
    {                                                                                                                                                                              \
        return Vector3<T>(parA.x OP parB, parA.y OP parB, parA.z OP parB);                                                                                         \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector3<T> operator OP(const T parB, const Vector3<T>& parA)                                                                                                            \
    {                                                                                                                                                                              \
        return parA OP parB;                                                                                                                                                       \
    }

MAKE_EXTERN_OP(+);
MAKE_EXTERN_OP(-);
MAKE_EXTERN_OP(/);
MAKE_EXTERN_OP(*);

#undef MAKE_EXTERN_OP


/****************************
 * VECTOR4
 ****************************/

template<typename T>
struct alignas(4) Vector4
{
public:
    Vector4() = default;
    explicit Vector4(T parValue)
        : x(parValue)
        , y(parValue)
        , z(parValue)
        , w(parValue)
    {
    }
    explicit Vector4(T parX, T parY)
        : x(parX)
        , y(parY)
    {
    }
    explicit Vector4(T parX, T parY, T parZ)
        : x(parX)
        , y(parY)
        , z(parZ)
    {
    }
    explicit Vector4(T parX, T parY, T parZ, T parW)
        : x(parX)
        , y(parY)
        , z(parZ)
        , w(parW)
    {
    }
    explicit Vector4(const Vector2<T>& parVec2)
        : x(parVec2.x)
        , y(parVec2.y)
    {
    }
    explicit Vector4(const Vector2<T>& parVec21, const Vector2<T>& parVec22)
        : x(parVec21.x)
        , y(parVec21.y) 
        , z(parVec22.x)
        , w(parVec22.y)
    {
    }
    explicit Vector4(const Vector3<T>& parVec3)
        : x(parVec3.x)
        , y(parVec3.y)
        , z(parVec3.z)
    {
    }

    explicit Vector4(const Vector3<T>& parVec3, float parW)
        : x(parVec3.x)
        , y(parVec3.y)
        , z(parVec3.z)
        , w(parW)
    {
    }

    static inline Vector4 MakeHomogeneousVec4(const Vector2<T>& parVec2) { return Vector4(parVec2.x, parVec2.y, T(0), T(1)); }
    static inline Vector4 MakeHomogeneousVec4(const Vector3<T>& parVec3) { return Vector4(parVec3.x, parVec3.y, parVec3.z, T(1)); }

#define MAKESCALAROP(OP)                                                                                                                                                           \
    inline Vector4 operator OP(const T parA) const                                                                                                                                 \
    {                                                                                                                                                                              \
        return Vector4(x OP parA, y OP parA, z OP parA, w OP parA);                                                                                                                                      \
    }                                                                                                                                                                              \
    inline void operator OP=(const T parA)                                                                                                                                        \
    {                                                                                                                                                                              \
        x OP= parA;                                                                                                                                                               \
        y OP= parA;                                                                                                                                                               \
        z OP= parA;                                                                                                                                                               \
        w OP= parA;                                                                                                                                                               \
    }

#define MAKEVECOP(OP)                                                                                                                                                              \
    inline Vector4 operator OP(const Vector4& parA) const                                                                                                                                \
    {                                                                                                                                                                              \
        return Vector4(x OP parA.x, y OP parA.y, z OP parA.z, w OP parA.w);                                                                                                                     \
    }                                                                                                                                                                              \
    inline void operator OP=(const Vector4& parA)                                                                                                                                 \
    {                                                                                                                                                                              \
        x OP= parA.x;                                                                                                                                                             \
        y OP= parA.y;                                                                                                                                                             \
        z OP= parA.z;                                                                                                                                                             \
        w OP= parA.w;                                                                                                                                                             \
    }

#define MAKEOP(OP)                                                                                                                                                                 \
    MAKEVECOP(OP);                                                                                                                                                                 \
    MAKESCALAROP(OP);

    MAKEOP(+);
    MAKEOP(-);
    MAKEOP(/);
    MAKEOP(*);

#undef MAKEVECOP
#undef MAKESCALAROP
#undef MAKEOP

#define MAKE_SWIZZLE_OP_2(OP, A, B)                                                                                                                                                \
    inline Vector2<T> OP() const                                                                                                                                                        \
    {                                                                                                                                                                              \
        return Vector2<T>(A, B);                                                                                                                                                   \
    }

    MAKE_SWIZZLE_OP_2(xx, x, x);
    MAKE_SWIZZLE_OP_2(yy, y, y);
    MAKE_SWIZZLE_OP_2(zz, z, z);
    MAKE_SWIZZLE_OP_2(xz, x, z);
    MAKE_SWIZZLE_OP_2(yx, y, x);
    MAKE_SWIZZLE_OP_2(zw, z, w);
    MAKE_SWIZZLE_OP_2(x0, x, T(0));
    MAKE_SWIZZLE_OP_2(x1, x, T(1));
    MAKE_SWIZZLE_OP_2(y0, y, T(0));
    MAKE_SWIZZLE_OP_2(y1, y, T(1));

#undef MAKE_SWIZZLE_OP_2

#define MAKE_SWIZZLE_OP_3(OP, A, B, C)                                                                                                                                             \
    inline Vector3<T> OP() const                                                                                                                                                           \
    {                                                                                                                                                                              \
        return Vector3<T>(A, B, C);                                                                                                                                                   \
    }

    MAKE_SWIZZLE_OP_3(xyz, x, y, z);
    MAKE_SWIZZLE_OP_3(xxx, x, x, x);
    MAKE_SWIZZLE_OP_3(yyy, y, y, y);
    MAKE_SWIZZLE_OP_3(zzz, z, z, z);
    MAKE_SWIZZLE_OP_3(www, w, w, w);
    MAKE_SWIZZLE_OP_3(x00, x, T(0), T(0));
    MAKE_SWIZZLE_OP_3(x01, x, T(0), T(1));
    MAKE_SWIZZLE_OP_3(y00, y, T(0), T(0));
    MAKE_SWIZZLE_OP_3(y01, y, T(0), T(1));
    MAKE_SWIZZLE_OP_3(z00, z, T(0), T(0));
    MAKE_SWIZZLE_OP_3(z01, z, T(0), T(1));

    MAKE_SWIZZLE_OP_3(xy0, x, y, T(0));
    MAKE_SWIZZLE_OP_3(xy1, x, y, T(1));
    MAKE_SWIZZLE_OP_3(xz0, x, z, T(0));
    MAKE_SWIZZLE_OP_3(xz1, x, z, T(1));

#undef MAKE_SWIZZLE_OP_3

#define MAKE_SWIZZLE_OP_4(OP, A, B, C, D)                                                                                                                                             \
    inline Vector4 OP() const                                                                                                                                                           \
    {                                                                                                                                                                              \
        return Vector4(A, B, C, D);                                                                                                                                                   \
    }

    MAKE_SWIZZLE_OP_4(xxxx, x, x, x, x);
    MAKE_SWIZZLE_OP_4(yyyy, y, y, y, y);
    MAKE_SWIZZLE_OP_4(zzzz, z, z, z, z);
    MAKE_SWIZZLE_OP_4(wwww, w, w, w, w);

#undef MAKE_SWIZZLE_OP_4

    inline bool operator==(const Vector4& parA) const
    {
        return parA.x == x && parA.y == y && parA.z == z && parA.w == w;
    }

    inline bool operator!=(const Vector4& parA) const
    {
        return !(*this == parA);
    }

public:
    T x = T(0);
    T y = T(0);
    T z = T(0);
    T w = T(0);
};

#define MAKE_EXTERN_OP(OP)                                                                                                                                                         \
    template<typename T>                                                                                                                                                           \
    inline Vector4<T> operator OP(const Vector4<T>& parA, const Vector4<T>& parB)                                                                                                  \
    {                                                                                                                                                                              \
        return Vector4<T>(parA.x OP parB.x, parA.y OP parB.y, parA.z OP parB.z, parA.w OP parB.w);                                                                                                   \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector4<T> operator OP(const Vector4<T>& parA, const T parB)                                                                                                  \
    {                                                                                                                                                                              \
        return Vector4<T>(parA.x OP parB, parA.y OP parB, parA.z OP parB, parA.w OP parB);                                                                                 \
    }                                                                                                                                                                              \
    template<typename T>                                                                                                                                                           \
    inline Vector4<T> operator OP(const T parB, const Vector4<T>& parA)                                                                                                            \
    {                                                                                                                                                                              \
        return parA OP parB;                                                                                         \
    }

MAKE_EXTERN_OP(+);
MAKE_EXTERN_OP(-);
MAKE_EXTERN_OP(/);
MAKE_EXTERN_OP(*);

#undef MAKE_EXTERN_OP
#pragma pack(pop)
}