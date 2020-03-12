/*
 * RawBytes class
 *
 * This wrapper class is used to handle raw bytes stored in a std::vector.
 */

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

    // Construct from bytes in C string.
    explicit RawBytes( const char* c );

    explicit RawBytes( const Base64String& b64 );
    explicit RawBytes( const HexString& hex );

    // Construct from bytes in std::string.
    explicit RawBytes( const std::string& s );

    // Fill n bytes from bytes.  If bytes.size() < n then keep reusing bytes
    // until this is full.
    RawBytes( size_type n, const RawBytes& bytes );

    // Convert to std::string.
    operator std::string() const;

    // Perform binary XOR with bytes.
    void x_or( const RawBytes& bytes );

    // Create RawBytes by extracting bytes from this object.
    // first:  index of first byte to extract
    // step:  index is incremented by this amount between byte extraction.
    RawBytes extractStepped( size_t first, size_t step ) const;

    // A trivial scoring function.
    // Score is number of characters, digits, and spaces in the raw bytes.
    unsigned scoreAsCharacters() const;

    // Calculate the Hamming distance between two sets of raw bytes.
    static unsigned HammingDistance( const RawBytes& a, const RawBytes& b );
};

#endif // RAWBYTES_H
