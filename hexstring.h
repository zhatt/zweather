/*
 * HexString class
 *
 * This wrapper class is used to handle hex encoded std::strings.  There are
 * constructors to convert from raw bytes and other hex encoded string types.
 */

#ifndef HEXSTRING_H
#define HEXSTRING_H

#include <string>

class RawBytes;

class HexString : public std::string {
public:
    HexString();

    // Construct from a hex encoded C string.
    HexString( const char* hex );

    // Encode raw bytes as hex string.
    HexString( const RawBytes& bytes );

    // Construct from a hex encoded std::string.
    HexString( const std::string& s );

private:
    void validate() const;
};

#endif // HEXSTRING_H
