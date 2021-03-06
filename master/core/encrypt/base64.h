
#ifndef _AOM_Base64_H
#define _AOM_Base64_H

#ifdef _MSC_VER
#pragma warning(disable:4514)
#endif

#include <stdio.h>
#include <string>

namespace klib{
namespace encode {

class base64
{
public:
    base64();

    void encode(const std::string&, std::string& , bool add_crlf = true);
    void encode(const char *, size_t, std::string& , bool add_crlf = true);
    void encode(const unsigned char *, size_t, std::string& , bool add_crlf = true);

    void decode(const std::string&, std::string& );

    size_t decode_length(const std::string& );

private:
    base64(const base64& ) {}
    base64& operator=(const base64& ) { return *this; }
};

}} // namespace 

#endif