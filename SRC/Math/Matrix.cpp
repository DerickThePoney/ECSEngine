#include "Matrix.h"
#include <cstring>

namespace ECSEngine
{
Matrix2x2f::Matrix2x2f(float parValue)
{
    FValues[0] = parValue;
    FValues[1] = parValue;
    FValues[2] = parValue;
    FValues[3] = parValue;
}

Matrix2x2f::Matrix2x2f(const vec2& A, const vec2& B)
{
    // TODO DECIDE THE MEMORY ORDER HERE. THESE SHOULD BE COLUMNS
}

}

