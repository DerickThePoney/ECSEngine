#pragma once

#include "miniaudio.h"

namespace ECSEngine
{
namespace MACustomVFSMethods
{
void CreateVFS();
void CloseVFS();

ma_result ma_vfs_open(ma_vfs* pVFS, const char* pFilePath, ma_uint32 openMode, ma_vfs_file* pFile);
ma_result ma_vfs_close(ma_vfs* pVFS, ma_vfs_file file);
ma_result ma_vfs_read(ma_vfs* pVFS, ma_vfs_file file, void* pDst, size_t sizeInBytes, size_t* pBytesRead);
ma_result ma_vfs_seek(ma_vfs* pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin);
ma_result ma_vfs_tell(ma_vfs* pVFS, ma_vfs_file file, ma_int64* pCursor);
ma_result ma_vfs_info(ma_vfs* pVFS, ma_vfs_file file, ma_file_info* pInfo);
} // namespace MACustomVFSMethods
} // namespace ECSEngine