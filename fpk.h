#pragma once
#include <stdint.h>

#if defined(_WIN32) && defined(FPK_DLL_EXPORTS)
  #define FPK_API __declspec(dllexport)
#else
  #define FPK_API
#endif

#define FPK_MAGIC      "FPK2"
#define FPK_MAX_FILES  1024
#define FPK_MAX_NAME   256
#define FPK_MAX_META   256

#pragma pack(push, 1)
typedef struct {
    char     name[FPK_MAX_NAME];
    uint32_t size;
    uint32_t offset;
} FPKEntry;
#pragma pack(pop)

FPK_API int fpk_pack(const char *archive, const char **files, int file_count, const char *meta);
FPK_API int fpk_list(const char *archive);
FPK_API int fpk_extract(const char *archive);
