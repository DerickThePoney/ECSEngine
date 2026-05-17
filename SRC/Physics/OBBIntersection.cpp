#include "stdafx.h"

#include "OBBIntersection.h"

#include "Common/FixedSizedArray.h"
#include "Common/MemoryView.h"
#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
//--------------------------------------------------------------------------------------------------
inline void EdgesContact(vec3* CA, vec3* CB, const vec3& PA, const vec3& QA, const vec3& PB, const vec3& QB)
{
    vec3 DA = QA - PA;
    vec3 DB = QB - PB;
    vec3 r = PA - PB;
    float a = Dot(DA, DA);
    float e = Dot(DB, DB);
    float f = Dot(DB, r);
    float c = Dot(DA, r);

    float b = Dot(DA, DB);
    float denom = a * e - b * b;

    float TA = (b * f - c * e) / denom;
    float TB = (b * TA + f) / e;

    *CA = PA + DA * TA;
    *CB = PB + DB * TB;
}
//--------------------------------------------------------------------------------------------------
void SupportEdge(const mat4& tx, const vec3& e, vec3 n, vec3* aOut, vec3* bOut)
{
    n = Transpose(GetRotation(tx)) * n;
    vec3 absN = Abs(n);
    vec3 a, b;

    // x > y
    if (absN.x > absN.y)
    {
        // x > y > z
        if (absN.y > absN.z)
        {
            a = vec3(e.x, e.y, e.z);
            b = vec3(e.x, e.y, -e.z);
        }

        // x > z > y || z > x > y
        else
        {
            a = vec3(e.x, e.y, e.z);
            b = vec3(e.x, -e.y, e.z);
        }
    }

    // y > x
    else
    {
        // y > x > z
        if (absN.x > absN.z)
        {
            a = vec3(e.x, e.y, e.z);
            b = vec3(e.x, e.y, -e.z);
        }

        // z > y > x || y > z > x
        else
        {
            a = vec3(e.x, e.y, e.z);
            b = vec3(-e.x, e.y, e.z);
        }
    }

    float signx = Sign(n.x);
    float signy = Sign(n.y);
    float signz = Sign(n.z);

    a.x *= signx;
    a.y *= signy;
    a.z *= signz;
    b.x *= signx;
    b.y *= signy;
    b.z *= signz;

    *aOut = (GetRotation(tx) * a) + tx.Column(3).xyz();
    *bOut = (GetRotation(tx) * b) + tx.Column(3).xyz();
}

bool TrackFaceAxis(i32* axis, i32 n, float Penetration, float* PenetrationMax, const vec3& normal, vec3* axisNormal)
{
    if (Penetration > 0.f)
        return true;

    if (Penetration > *PenetrationMax)
    {
        *PenetrationMax = Penetration;
        *axis = n;
        *axisNormal = normal;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
inline bool TrackEdgeAxis(i32* axis, i32 n, float s, float* sMax, const vec3& normal, vec3* axisNormal)
{
    if (s > 0.f)
        return true;

    float l = 1.f / Length(normal);
    s *= l;

    if (s > *sMax)
    {
        *sMax = s;
        *axis = n;
        *axisNormal = normal * l;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
struct ClipVertex
{
    ClipVertex() { f.key = ~0; }

    vec3 v;
    FeaturePair f;
};

//--------------------------------------------------------------------------------------------------
void ComputeReferenceEdgesAndBasis(const vec3& eR, const mat4& rtx, vec3 n, i32 axis, u8* out, mat3* basis, vec3* e)
{
    n = Transpose(GetRotation(rtx)) * n;

    if (axis >= 3)
        axis -= 3;

    switch (axis)
    {
    case 0:
        if (n.x > 0.f)
        {
            out[0] = 1;
            out[1] = 8;
            out[2] = 7;
            out[3] = 9;

            *e = vec3(eR.y, eR.z, eR.x);

            basis->SetColumn(0, rtx.Column(1).xyz());
            basis->SetColumn(1, rtx.Column(2).xyz());
            basis->SetColumn(2, rtx.Column(0).xyz());
        }

        else
        {
            out[0] = 11;
            out[1] = 3;
            out[2] = 10;
            out[3] = 5;

            *e = vec3(eR.z, eR.y, eR.x);

            basis->SetColumn(0, rtx.Column(2).xyz());
            basis->SetColumn(1, rtx.Column(1).xyz());
            basis->SetColumn(2, Invert(rtx.Column(0).xyz()));
        }
        break;

    case 1:
        if (n.y > 0.f)
        {
            out[0] = 0;
            out[1] = 1;
            out[2] = 2;
            out[3] = 3;

            *e = vec3(eR.z, eR.x, eR.y);

            basis->SetColumn(0, rtx.Column(2).xyz());
            basis->SetColumn(1, rtx.Column(0).xyz());
            basis->SetColumn(2, rtx.Column(1).xyz());
        }

        else
        {
            out[0] = 4;
            out[1] = 5;
            out[2] = 6;
            out[3] = 7;

            *e = vec3(eR.z, eR.x, eR.y);

            basis->SetColumn(0, rtx.Column(2).xyz());
            basis->SetColumn(1, Invert(rtx.Column(0).xyz()));
            basis->SetColumn(2, Invert(rtx.Column(1).xyz()));
        }
        break;

    case 2:
        if (n.z > 0.f)
        {
            out[0] = 11;
            out[1] = 4;
            out[2] = 8;
            out[3] = 0;

            *e = vec3(eR.y, eR.x, eR.z);

            basis->SetColumn(0, Invert(rtx.Column(1).xyz()));
            basis->SetColumn(1, rtx.Column(0).xyz());
            basis->SetColumn(2, rtx.Column(2).xyz());
        }

        else
        {
            out[0] = 6;
            out[1] = 10;
            out[2] = 2;
            out[3] = 9;

            *e = vec3(eR.y, eR.x, eR.z);

            basis->SetColumn(0, Invert(rtx.Column(1).xyz()));
            basis->SetColumn(1, Invert(rtx.Column(0).xyz()));
            basis->SetColumn(2, Invert(rtx.Column(2).xyz()));
        }
        break;
    }
}

//--------------------------------------------------------------------------------------------------
void ComputeIncidentFace(const mat4& itx, const vec3& e, vec3 n, ClipVertex* out)
{
    n = Invert(Transpose(GetRotation(itx)) * n);
    vec3 absN = Abs(n);

    if (absN.x > absN.y && absN.x > absN.z)
    {
        if (n.x > 0.f)
        {
            out[0].v = vec3(e.x, e.y, -e.z);
            out[1].v = vec3(e.x, e.y, e.z);
            out[2].v = vec3(e.x, -e.y, e.z);
            out[3].v = vec3(e.x, -e.y, -e.z);

            out[0].f.inI = 9;
            out[0].f.outI = 1;
            out[1].f.inI = 1;
            out[1].f.outI = 8;
            out[2].f.inI = 8;
            out[2].f.outI = 7;
            out[3].f.inI = 7;
            out[3].f.outI = 9;
        }

        else
        {
            out[0].v = vec3(-e.x, -e.y, e.z);
            out[1].v = vec3(-e.x, e.y, e.z);
            out[2].v = vec3(-e.x, e.y, -e.z);
            out[3].v = vec3(-e.x, -e.y, -e.z);

            out[0].f.inI = 5;
            out[0].f.outI = 11;
            out[1].f.inI = 11;
            out[1].f.outI = 3;
            out[2].f.inI = 3;
            out[2].f.outI = 10;
            out[3].f.inI = 10;
            out[3].f.outI = 5;
        }
    }

    else if (absN.y > absN.x && absN.y > absN.z)
    {
        if (n.y > 0.f)
        {
            out[0].v = vec3(-e.x, e.y, e.z);
            out[1].v = vec3(e.x, e.y, e.z);
            out[2].v = vec3(e.x, e.y, -e.z);
            out[3].v = vec3(-e.x, e.y, -e.z);

            out[0].f.inI = 3;
            out[0].f.outI = 0;
            out[1].f.inI = 0;
            out[1].f.outI = 1;
            out[2].f.inI = 1;
            out[2].f.outI = 2;
            out[3].f.inI = 2;
            out[3].f.outI = 3;
        }

        else
        {
            out[0].v = vec3(e.x, -e.y, e.z);
            out[1].v = vec3(-e.x, -e.y, e.z);
            out[2].v = vec3(-e.x, -e.y, -e.z);
            out[3].v = vec3(e.x, -e.y, -e.z);

            out[0].f.inI = 7;
            out[0].f.outI = 4;
            out[1].f.inI = 4;
            out[1].f.outI = 5;
            out[2].f.inI = 5;
            out[2].f.outI = 6;
            out[3].f.inI = 6;
            out[3].f.outI = 7;
        }
    }

    else
    {
        if (n.z > 0.f)
        {
            out[0].v = vec3(-e.x, e.y, e.z);
            out[1].v = vec3(-e.x, -e.y, e.z);
            out[2].v = vec3(e.x, -e.y, e.z);
            out[3].v = vec3(e.x, e.y, e.z);

            out[0].f.inI = 0;
            out[0].f.outI = 11;
            out[1].f.inI = 11;
            out[1].f.outI = 4;
            out[2].f.inI = 4;
            out[2].f.outI = 8;
            out[3].f.inI = 8;
            out[3].f.outI = 0;
        }

        else
        {
            out[0].v = vec3(e.x, -e.y, -e.z);
            out[1].v = vec3(-e.x, -e.y, -e.z);
            out[2].v = vec3(-e.x, e.y, -e.z);
            out[3].v = vec3(e.x, e.y, -e.z);

            out[0].f.inI = 9;
            out[0].f.outI = 6;
            out[1].f.inI = 6;
            out[1].f.outI = 10;
            out[2].f.inI = 10;
            out[2].f.outI = 2;
            out[3].f.inI = 2;
            out[3].f.outI = 9;
        }
    }

    for (i32 i = 0; i < 4; ++i)
        out[i].v = (itx * vec4::MakeHomogeneousPositionVec4(out[i].v)).xyz();
}
//--------------------------------------------------------------------------------------------------
#define InFront(a) ((a) < 0.f)

#define Behind(a) ((a) >= 0.f)

#define On(a) ((a) < 0.005f && (a) > -0.005f)

i32 Orthographic(float sign, float e, i32 axis, i32 clipEdge, ClipVertex* in, i32 inCount, ClipVertex* out)
{
    i32 outCount = 0;
    ClipVertex a = in[inCount - 1];

    for (i32 i = 0; i < inCount; ++i)
    {
        ClipVertex b = in[i];

        float da = sign * a.v[axis] - e;
        float db = sign * b.v[axis] - e;

        ClipVertex cv;

        // B
        if (((InFront(da) && InFront(db)) || On(da) || On(db)))
        {
            AlwaysCheckedAssert(outCount < 8);
            out[outCount++] = b;
        }

        // I
        else if (InFront(da) && Behind(db))
        {
            cv.f = b.f;
            cv.v = a.v + (b.v - a.v) * (da / (da - db));
            cv.f.outR = clipEdge;
            cv.f.outI = 0;
            AlwaysCheckedAssert(outCount < 8);
            out[outCount++] = cv;
        }

        // I, B
        else if (Behind(da) && InFront(db))
        {
            cv.f = a.f;
            cv.v = a.v + (b.v - a.v) * (da / (da - db));
            cv.f.inR = clipEdge;
            cv.f.inI = 0;
            AlwaysCheckedAssert(outCount < 8);
            out[outCount++] = cv;

            AlwaysCheckedAssert(outCount < 8);
            out[outCount++] = b;
        }

        a = b;
    }

    return outCount;
}

//--------------------------------------------------------------------------------------------------
// Resources (also see q3BoxtoBox's resources):
// http://www.randygaul.net/2013/10/27/sutherland-hodgman-clipping/
i32 Clip(const vec3& rPos, const vec3& e, u8* clipEdges, const mat3& basis, ClipVertex* incident, ClipVertex* outVerts, float* outDepths)
{
    i32 inCount = 4;
    i32 outCount;
    ClipVertex in[8];
    ClipVertex out[8];

    for (i32 i = 0; i < 4; ++i)
        in[i].v = Transpose(basis) * (incident[i].v - rPos);

    outCount = Orthographic(1.f, e.x, 0, clipEdges[0], in, inCount, out);

    if (!outCount)
        return 0;

    inCount = Orthographic(1.f, e.y, 1, clipEdges[1], out, outCount, in);

    if (!inCount)
        return 0;

    outCount = Orthographic(-1.f, e.x, 0, clipEdges[2], in, inCount, out);

    if (!outCount)
        return 0;

    inCount = Orthographic(-1.f, e.y, 1, clipEdges[3], out, outCount, in);

    // Keep incident vertices behind the reference face
    outCount = 0;
    for (i32 i = 0; i < inCount; ++i)
    {
        float d = in[i].v.z - e.z;

        if (d <= 0.f)
        {
            outVerts[outCount].v = basis * in[i].v + rPos;
            outVerts[outCount].f = in[i].f;
            outDepths[outCount++] = d;
        }
    }

    assert(outCount <= 8);

    return outCount;
}

//--------------------------------------------------------------------------------------------------
bool OBBIntersection(Contact* C, const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB)
{
    mat3 RotationA = GetRotation(parTransformA);
    mat3 RotationB = GetRotation(parTransformB);

    vec3 ExtentsA = parBoundingBoxA.HalfExtent();
    vec3 ExtentsB = parBoundingBoxB.HalfExtent();

    // Extract B in A's Frame
    mat3 BinAFrame = Transpose(RotationA) * RotationB;

    bool bParallel = false;
    constexpr float kCosTol = 1e-6f;
    mat3 AbsBinAFrame;
    forrange(i, 0, 3)
    {
        vec3 ithColumn = BinAFrame.Column(i);
        ithColumn.x = abs(ithColumn.x);
        if (ithColumn.x + kCosTol >= 1.f)
            bParallel = true;

        ithColumn.y = abs(ithColumn.y);
        if (ithColumn.y + kCosTol >= 1.f)
            bParallel = true;

        ithColumn.z = abs(ithColumn.z);
        if (ithColumn.z + kCosTol >= 1.f)
            bParallel = true;

        AbsBinAFrame.SetColumn(i, ithColumn);
    }

    // Get the separation vector in A's frame
    vec3 SeparationVector = Transpose(RotationA) * (parTransformB.Column(3) - parTransformA.Column(3)).xyz();

    // Query states
    float Penetration = 0.f;
    float PenetrationAMax = -std::numeric_limits<float>().max();
    float PenetrationBMax = -std::numeric_limits<float>().max();
    float PenetrationEMax = -std::numeric_limits<float>().max();
    i32 AAxis = ~0;
    i32 BAxis = ~0;
    i32 EAxis = ~0;
    vec3 NormalA;
    vec3 NormalB;
    vec3 NormalE;

    // checking face axis
    // A's X axis
    Penetration = abs(SeparationVector.x) - (ExtentsA.x + Dot(AbsBinAFrame.Row(0), ExtentsB));
    if (TrackFaceAxis(&AAxis, 0, Penetration, &PenetrationAMax, RotationA.Column(0), &NormalA))
        return false;

    // A's Y axis
    Penetration = abs(SeparationVector.y) - (ExtentsA.y + Dot(AbsBinAFrame.Row(1), ExtentsB));
    if (TrackFaceAxis(&AAxis, 1, Penetration, &PenetrationAMax, RotationA.Column(1), &NormalA))
        return false;

    // A's Z axis
    Penetration = abs(SeparationVector.z) - (ExtentsA.z + Dot(AbsBinAFrame.Row(2), ExtentsB));
    if (TrackFaceAxis(&AAxis, 2, Penetration, &PenetrationAMax, RotationA.Column(2), &NormalA))
        return false;

    // B's X axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(0))) - (ExtentsB.x + Dot(AbsBinAFrame.Column(0), ExtentsA));
    if (TrackFaceAxis(&BAxis, 3, Penetration, &PenetrationBMax, RotationB.Column(0), &NormalB))
        return false;

    // B's Y axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(1))) - (ExtentsB.y + Dot(AbsBinAFrame.Column(1), ExtentsA));
    if (TrackFaceAxis(&BAxis, 4, Penetration, &PenetrationBMax, RotationB.Column(1), &NormalB))
        return false;

    // B's Z axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(2))) - (ExtentsB.z + Dot(AbsBinAFrame.Column(2), ExtentsA));
    if (TrackFaceAxis(&BAxis, 5, Penetration, &PenetrationBMax, RotationB.Column(2), &NormalB))
        return false;

    if (!bParallel)
    {
        // Edge axis checks
        float rA;
        float rB;

        // Cross( a.x, b.x )
        rA = ExtentsA.y * AbsBinAFrame.Column(0).z + ExtentsA.z * AbsBinAFrame.Column(0).y;
        rB = ExtentsB.y * AbsBinAFrame.Column(2).x + ExtentsB.z * AbsBinAFrame.Column(1).x;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(0).y - SeparationVector.y * BinAFrame.Column(0).z) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 6, Penetration, &PenetrationEMax, vec3(0.f, -BinAFrame.Column(0).z, BinAFrame.Column(0).y), &NormalE))
            return false;

        // Cross( a.x, b.y )
        rA = ExtentsA.y * AbsBinAFrame.Column(1).z + ExtentsA.z * AbsBinAFrame.Column(1).y;
        rB = ExtentsB.x * AbsBinAFrame.Column(2).x + ExtentsB.z * AbsBinAFrame.Column(0).x;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(1).y - SeparationVector.y * BinAFrame.Column(1).z) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 7, Penetration, &PenetrationEMax, vec3(0.f, -BinAFrame.Column(1).z, BinAFrame.Column(1).y), &NormalE))
            return false;

        // Cross( a.x, b.z )
        rA = ExtentsA.y * AbsBinAFrame.Column(2).z + ExtentsA.z * AbsBinAFrame.Column(2).y;
        rB = ExtentsB.x * AbsBinAFrame.Column(1).x + ExtentsB.y * AbsBinAFrame.Column(0).x;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(2).y - SeparationVector.y * BinAFrame.Column(2).z) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 8, Penetration, &PenetrationEMax, vec3(0.f, -BinAFrame.Column(2).z, BinAFrame.Column(2).y), &NormalE))
            return false;

        // Cross( a.y, b.x )
        rA = ExtentsA.x * AbsBinAFrame.Column(0).z + ExtentsA.z * AbsBinAFrame.Column(0).x;
        rB = ExtentsB.y * AbsBinAFrame.Column(2).y + ExtentsB.z * AbsBinAFrame.Column(1).y;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(0).z - SeparationVector.z * BinAFrame.Column(0).x) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 9, Penetration, &PenetrationEMax, vec3(BinAFrame.Column(0).z, 0.f, -BinAFrame.Column(0).x), &NormalE))
            return false;

        // Cross( a.y, b.y )
        rA = ExtentsA.x * AbsBinAFrame.Column(1).z + ExtentsA.z * AbsBinAFrame.Column(1).x;
        rB = ExtentsB.x * AbsBinAFrame.Column(2).y + ExtentsB.z * AbsBinAFrame.Column(0).y;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(1).z - SeparationVector.z * BinAFrame.Column(1).x) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 10, Penetration, &PenetrationEMax, vec3(BinAFrame.Column(1).z, 0.f, -BinAFrame.Column(1).x), &NormalE))
            return false;

        // Cross( a.y, b.z )
        rA = ExtentsA.x * AbsBinAFrame.Column(2).z + ExtentsA.z * AbsBinAFrame.Column(2).x;
        rB = ExtentsB.x * AbsBinAFrame.Column(1).y + ExtentsB.y * AbsBinAFrame.Column(0).y;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(2).z - SeparationVector.z * BinAFrame.Column(2).x) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 11, Penetration, &PenetrationEMax, vec3(BinAFrame.Column(2).z, 0.f, -BinAFrame.Column(2).x), &NormalE))
            return false;

        // Cross( a.z, b.x )
        rA = ExtentsA.x * AbsBinAFrame.Column(0).y + ExtentsA.y * AbsBinAFrame.Column(0).x;
        rB = ExtentsB.y * AbsBinAFrame.Column(2).z + ExtentsB.z * AbsBinAFrame.Column(1).z;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(0).x - SeparationVector.x * BinAFrame.Column(0).y) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 12, Penetration, &PenetrationEMax, vec3(-BinAFrame.Column(0).y, BinAFrame.Column(0).x, 0.f), &NormalE))
            return false;

        // Cross( a.z, b.y )
        rA = ExtentsA.x * AbsBinAFrame.Column(1).y + ExtentsA.y * AbsBinAFrame.Column(1).x;
        rB = ExtentsB.x * AbsBinAFrame.Column(2).z + ExtentsB.z * AbsBinAFrame.Column(0).z;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(1).x - SeparationVector.x * BinAFrame.Column(1).y) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 13, Penetration, &PenetrationEMax, vec3(-BinAFrame.Column(1).y, BinAFrame.Column(1).x, 0.f), &NormalE))
            return false;

        // Cross( a.z, b.z )
        rA = ExtentsA.x * AbsBinAFrame.Column(2).y + ExtentsA.y * AbsBinAFrame.Column(2).x;
        rB = ExtentsB.x * AbsBinAFrame.Column(1).z + ExtentsB.y * AbsBinAFrame.Column(0).z;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(2).x - SeparationVector.x * BinAFrame.Column(2).y) - (rA + rB);
        if (TrackEdgeAxis(&EAxis, 14, Penetration, &PenetrationEMax, vec3(-BinAFrame.Column(2).y, BinAFrame.Column(2).x, 0.f), &NormalE))
            return false;
    }

    // Artificial axis bias
    static constexpr float kRelTol = 0.95f;
    static constexpr float kAbsTol = 0.01f;
    i32 Axis;
    float MaxPenetration;
    vec3 Normal;

    float FacePenetrationMax = Max(PenetrationAMax, PenetrationBMax);
    if (kRelTol * PenetrationEMax > FacePenetrationMax + kAbsTol)
    {
        Axis = EAxis;
        MaxPenetration = PenetrationEMax;
        Normal = NormalE;
    }
    else
    {
        if (kRelTol * PenetrationBMax > PenetrationAMax + kAbsTol)
        {
            Axis = BAxis;
            MaxPenetration = PenetrationBMax;
            Normal = NormalB;
        }
        else
        {
            Axis = AAxis;
            MaxPenetration = PenetrationAMax;
            Normal = NormalA;
        }
    }

    if (Dot(Normal, (parTransformB.Column(3) - parTransformA.Column(3)).xyz()) < 0.f)
    {
        Normal = Invert(Normal);
    }

    if (Axis == ~0)
    {
        return false;
    }

    if (Axis < 6)
    {
        mat4 ReferenceTransform;
        mat4 IncidentTransform;
        vec3 ExtentsReference;
        vec3 ExtentsIncident;
        bool flip;

        if (Axis < 3)
        {
            ReferenceTransform = parTransformA;
            IncidentTransform = parTransformB;
            ExtentsReference = ExtentsA;
            ExtentsIncident = ExtentsB;
            flip = false;
        }

        else
        {
            ReferenceTransform = parTransformB;
            IncidentTransform = parTransformA;
            ExtentsReference = ExtentsB;
            ExtentsIncident = ExtentsA;
            flip = true;
            Normal = Invert(Normal);
        }

        // Compute reference and incident edge information necessary for clipping
        ClipVertex incident[4];
        ComputeIncidentFace(IncidentTransform, ExtentsIncident, Normal, incident);
        u8 clipEdges[4];
        mat3 basis;
        vec3 e;
        ComputeReferenceEdgesAndBasis(ExtentsReference, ReferenceTransform, Normal, Axis, clipEdges, &basis, &e);

        // Clip the incident face against the reference face side planes
        ClipVertex out[8];
        float depths[8];
        i32 outNum;
        outNum = Clip(ReferenceTransform.Column(3).xyz(), e, clipEdges, basis, incident, out, depths);

        if (outNum)
        {
            C->FContactNormal = (flip) ? Invert(Normal) : Normal;

            C->FManifold.FContactPoints.reserve(outNum);

            for (i32 i = 0; i < outNum; ++i)
            {
                FeaturePair pair = out[i].f;
                if (flip)
                {
                    std::swap(pair.inI, pair.inR);
                    std::swap(pair.outI, pair.outR);
                }

                ContactPoint& CP = C->FManifold.FContactPoints.emplace_back();
                CP.FPosition = out[i].v;
                CP.FPenetration = depths[i];
                CP.FP = pair;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        // Edge cases
        Normal = RotationA * Normal;

        if (Dot(Normal, (parTransformB.Column(3) - parTransformA.Column(3)).xyz()) < 0.f)
            Normal = Invert(Normal);

        vec3 PA, QA;
        vec3 PB, QB;
        SupportEdge(parTransformA, ExtentsA, Normal, &PA, &QA);
        SupportEdge(parTransformB, ExtentsB, Invert(Normal), &PB, &QB);

        vec3 CA, CB;
        EdgesContact(&CA, &CB, PA, QA, PB, QB);

        C->FContactNormal = Normal;
        /*m->contactCount = 1;*/

        FeaturePair pair;
        pair.key = Axis;

        ContactPoint CP;
        CP.FPosition = (CA + CB) * 0.5f;
        CP.FPenetration = MaxPenetration;
        CP.FP = pair;
        C->FManifold.FContactPoints.push_back(CP);
    }

    C->FFlags.SetBit(EContactFlag::CT_CONTACT_INFO, true);

    return true;
}

} // namespace Physics
} // namespace ECSEngine