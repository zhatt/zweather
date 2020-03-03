#ifndef RAWBYTES_H
#define RAWBYTES_H

#include <cstdint>
#include <string>
#include <vector>

#include "base64string.h"
#include "hexstring.h"

class RawBytes : public std::vector<uint8_t> {
public:
    using vector::vector;

    explicit RawBytes( const char* c );
    explicit RawBytes( const Base64String& b64 );
    explicit RawBytes( const HexString& hex );
    explicit RawBytes( const std::string& s );

    // Fill n bytes from bytes.  If bytes.size() < n then keep reusing bytes
    // until this is full.
    RawBytes( size_type n, const RawBytes& bytes );

    operator std::string() const;

    void x_or( const RawBytes& bytes );

    RawBytes extractStepped( size_t first, size_t step ) const;
    unsigned scoreAsCharacters() const;

    static unsigned HammingDistance( const RawBytes& a, const RawBytes& b );
};

#endif // RAWBYTES_H
