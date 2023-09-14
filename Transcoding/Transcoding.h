#ifdef __cplusplus
extern "C"
{
#include <cstddef>
#include <iconv.h>
#include <stddef.h>
#include <string.h>

//-----------------------------------------------------------------------//
// GBK     --> UTF8
int GBKToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen);
// GBK     --> GB2312
int GBKToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen);
// GBK     --> Unicode
int GBKToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen);
//-----------------------------------------------------------------------//

//-----------------------------------------------------------------------//
// GB2312  --> UTF8
int GB2312ToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen);
// GB2312  --> GBK
int GB2312ToGBK(char* src, size_t srcLen, char* dst, size_t dstLen);
// GB2312  --> Unicode
int GB2312ToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen);
//-----------------------------------------------------------------------//

//-----------------------------------------------------------------------//
// UTF8    --> GBK
int UTF8ToGBK(char* src, size_t srcLen, char* dst, size_t dstLen);
// UTF8    --> GB2312
int UTF8ToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen);
// UTF8    --> Unicode
int UTF8ToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen);
//-----------------------------------------------------------------------//

//-----------------------------------------------------------------------//
// Unicode --> UTF8
int UnicodeToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen);
// Unicode --> GBK
int UnicodeToGBK(char* src, size_t srcLen, char* dst, size_t dstLen);
// Unicode --> Unicode
int UnicodeToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen);
//------------------------------------------------------------------------//
}

#endif