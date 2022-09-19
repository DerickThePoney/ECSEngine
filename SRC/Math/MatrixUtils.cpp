#include "MatrixUtils.h"
#include "Matrix.h"

namespace ECSEngine
{

mat2 Mul(const mat2& parA, const mat2& parB)
{
    mat2 result;
    result.FValues[0] = parA.FValues[0] * parB.FValues[0] + parA.FValues[1] * parB.FValues[2];
    result.FValues[1] = parA.FValues[0] * parB.FValues[1] + parA.FValues[1] * parB.FValues[3];
    result.FValues[2] = parA.FValues[2] * parB.FValues[0] + parA.FValues[3] * parB.FValues[2];
    result.FValues[3] = parA.FValues[2] * parB.FValues[1] + parA.FValues[3] * parB.FValues[3];
    return result;
}

}
