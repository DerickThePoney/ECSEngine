#pragma once

namespace ECSEngine
{
#ifdef ENABLE_DEBUG_PARAMETERS
void CreateAdjustables();
void DestroyAdjustables();

void DrawAdjustables();

u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue);
float GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, float parDefaultValue, float parMinValue, float parMaxValue);
double GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, double parDefaultValue, double parMinValue, double parMaxValue);
bool GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, bool parDefaultValue);

#define ADJUSTABLE_DEBUG_PARAMETER_UNSIGNED(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) const u32 VARNAME = GetOrCreateAdjustableDebugParameter(NAME, FAMILY, DEFAULT, MIN, MAX);
#define ADJUSTABLE_DEBUG_PARAMETER_SINGLE(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) const float VARNAME = GetOrCreateAdjustableDebugParameter(NAME, FAMILY, DEFAULT, MIN, MAX);
#define ADJUSTABLE_DEBUG_PARAMETER_DOUBLE(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) const double VARNAME = GetOrCreateAdjustableDebugParameter(NAME, FAMILY, DEFAULT, MIN, MAX);
#define ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(VARNAME, DEFAULT, NAME, FAMILY) const bool VARNAME = GetOrCreateAdjustableDebugParameter(NAME, FAMILY, DEFAULT);

#else
void CreateAdjustables();
void DestroyAdjustables();
void DrawAdjustables();
#define ADJUSTABLE_DEBUG_PARAMETER_UNSIGNED(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) static constexpr u32 VARNAME = DEFAULT;
#define ADJUSTABLE_DEBUG_PARAMETER_SINGLE(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) static constexpr float VARNAME = DEFAULT;
#define ADJUSTABLE_DEBUG_PARAMETER_DOUBLE(VARNAME, DEFAULT, NAME, FAMILY, MIN, MAX) static constexpr double VARNAME = DEFAULT;
#define ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(VARNAME, DEFAULT, NAME, FAMILY) static constexpr bool VARNAME = DEFAULT;
#endif // ENABLE_DEBUG_PARAMETERS
} // namespace ECSEngine
