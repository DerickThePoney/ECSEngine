// clang-format off
/**
 * Sampler flags.
 *
 */
SAMPLER_FLAG(SAMPLER_U_MIRROR, 0x00000001), //!< Wrap U mode: Mirror
SAMPLER_FLAG(SAMPLER_U_CLAMP, 0x00000002), //!< Wrap U mode: Clamp
SAMPLER_FLAG(SAMPLER_U_BORDER, 0x00000003), //!< Wrap U mode: Border

SAMPLER_FLAG(SAMPLER_V_MIRROR, 0x00000004), //!< Wrap V mode: Mirror
SAMPLER_FLAG(SAMPLER_V_CLAMP, 0x00000008), //!< Wrap V mode: Clamp
SAMPLER_FLAG(SAMPLER_V_BORDER, 0x0000000c), //!< Wrap V mode: Border

SAMPLER_FLAG(SAMPLER_W_MIRROR, 0x00000010), //!< Wrap W mode: Mirror
SAMPLER_FLAG(SAMPLER_W_CLAMP, 0x00000020), //!< Wrap W mode: Clamp
SAMPLER_FLAG(SAMPLER_W_BORDER, 0x00000030), //!< Wrap W mode: Border

SAMPLER_FLAG(SAMPLER_MIN_POINT, 0x00000040), //!< Min sampling mode: Point
SAMPLER_FLAG(SAMPLER_MIN_ANISOTROPIC, 0x00000080), //!< Min sampling mode: Anisotropic

SAMPLER_FLAG(SAMPLER_MAG_POINT, 0x00000100), //!< Mag sampling mode: Point
SAMPLER_FLAG(SAMPLER_MAG_ANISOTROPIC, 0x00000200), //!< Mag sampling mode: Anisotropic

SAMPLER_FLAG(SAMPLER_MIP_POINT, 0x00000400), //!< Mip sampling mode: Point

SAMPLER_FLAG(SAMPLER_COMPARE_LESS, 0x00010000), //!< Compare when sampling depth texture: less.
SAMPLER_FLAG(SAMPLER_COMPARE_LEQUAL, 0x00020000), //!< Compare when sampling depth texture: less or equal.
SAMPLER_FLAG(SAMPLER_COMPARE_EQUAL, 0x00030000), //!< Compare when sampling depth texture: equal.
SAMPLER_FLAG(SAMPLER_COMPARE_GEQUAL, 0x00040000), //!< Compare when sampling depth texture: greater or equal.
SAMPLER_FLAG(SAMPLER_COMPARE_GREATER, 0x00050000), //!< Compare when sampling depth texture: greater.
SAMPLER_FLAG(SAMPLER_COMPARE_NOTEQUAL, 0x00060000), //!< Compare when sampling depth texture: not equal.
SAMPLER_FLAG(SAMPLER_COMPARE_NEVER, 0x00070000), //!< Compare when sampling depth texture: never.
SAMPLER_FLAG(SAMPLER_COMPARE_ALWAYS, 0x00080000), //!< Compare when sampling depth texture: always.
#ifndef DONOTINCLUDELENGTH
SAMPLER_FLAG(SAMPLER_FLAGS_LENGTH, 0x10000000)
#endif
      // clang-format on
