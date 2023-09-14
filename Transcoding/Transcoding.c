#include "Transcoding.h"
#include <iconv.h>
#include <string.h>


//-------------------------------------------------------------------------------------------------------------------//
int GBKToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("UTF-8", "GBK");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);

    if (iconv(cd, &inptr, &srcLen, &outptr, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int GBKToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GB2312", "GBK");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &outptr, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int GBKToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("Unicode", "GBK");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &outptr, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}
//-------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------//
int GB2312ToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("UTF-8", "GB2312");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);
    
    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int GB2312ToGBK(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GBK", "GB2312");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);
    
    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int GB2312ToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("Unicode", "GB2312");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);
    
    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}
//-------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------//
int UTF8ToGBK(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GBK", "UTF-8");
    if(cd == (iconv_t)(-1))
    {
        return -1;
    }

    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int UTF8ToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GB2312", "UTF-8");

    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int UTF8ToUnicode(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("Unicode", "UTF-8");
    
    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}
//-------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------//
int UnicodeToUTF8(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("UTF-8", "Unicode");
    
    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int UnicodeToGBK(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GBK", "Unicode");
    
    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}

int UnicodeToGB2312(char* src, size_t srcLen, char* dst, size_t dstLen)
{
    char *inptr = src, *outptr = dst;

    iconv_t cd = iconv_open("GB2312", "Unicode");
    
    memset(dst, 0, dstLen);

    if(iconv(cd, &inptr, &srcLen, &dst, &dstLen) == -1)
    {
        return -1;
    }

    iconv_close(cd);

    return 0;
}
//-------------------------------------------------------------------------------------------------------------------//