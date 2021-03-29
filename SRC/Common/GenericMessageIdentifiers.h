#pragma once

namespace ECSEngine
{
namespace GenericMessageId
{
enum Type
{
#define DECLARE_GENERIC_MESSAGE_ID(NAME) NAME,
#include "GenericMessageIdentifiers.inl"
#undef DECLARE_GENERIC_MESSAGE_ID
    LENGTH
};
} // namespace GenericMessageId
} // namespace ECSEngine
