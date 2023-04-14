#pragma once
#include <iosfwd>

namespace ECSEngine
{

using Address = uint64_t;
enum class FrameFlags : uint16_t
{
    HAS_ADDRESS = 0b0001, // Should always be set
    HAS_FUNC_NAME = 0b0010,
    HAS_LINE_INFO = 0b0100,
    HAS_MODULE_INFO = 0b1000,

    NONE = 0
};

// clang-format off
inline FrameFlags operator~(FrameFlags a) { return static_cast<FrameFlags>(~static_cast<uint16_t>(a)); } //!< \brief operator for FrameFlags
inline FrameFlags operator|(FrameFlags a, FrameFlags b) { return static_cast<FrameFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
inline FrameFlags operator&(FrameFlags a, FrameFlags b) { return static_cast<FrameFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
inline FrameFlags operator^(FrameFlags a, FrameFlags b) { return static_cast<FrameFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
inline FrameFlags& operator|=(FrameFlags& a, FrameFlags b) { return reinterpret_cast<FrameFlags&>(reinterpret_cast<uint16_t&>(a) |= static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
inline FrameFlags& operator&=(FrameFlags& a, FrameFlags b) { return reinterpret_cast<FrameFlags&>(reinterpret_cast<uint16_t&>(a) &= static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
inline FrameFlags& operator^=(FrameFlags& a, FrameFlags b) { return reinterpret_cast<FrameFlags&>(reinterpret_cast<uint16_t&>(a) ^= static_cast<uint16_t>(b)); } //!< \brief operator for FrameFlags
// clang-format on

//! \brief Describes a frame
struct Frame
{
    FrameFlags flags = FrameFlags::NONE; //!< \brief Describes which fields are set

    Address frameAddr = 0; //!< \brief The instruction pointer of the stack frame
    std::string funcName; //!< \brief The name of the function
    std::string moduleName; //!< \brief The name / path of the binary file
    std::string fileName; //!< \brief The name / path of the source file
    int line = 0; //!< \brief The line number in the source file
    int column = 0; //!< \brief The column in the source file
};

struct CallStackTrace
{
    CallStackTrace();
    ~CallStackTrace();

    void PrintToStream(std::ostringstream& oss);

    std::vector<Frame> callstackFrames;
};
} // namespace ECSEngine