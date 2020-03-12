/*
 * Base64String class
 *
 * This wrapper class is used to handle base 64 encoded std::strings.
 * There are constructors to convert from raw bytes and other base 64 encoded
 * string types.
 */

#ifndef BASE64STRING_H
#define BASE64STRING_H

#include <string>

#include "hexstring.h"

class RawBytes;

class Base64String : public std::string {

public:
    static const std::string mappingTable;

    Base64String();

    // Construct from a base 64 encoded C string.
    Base64String( const char* s );

    // Encode raw bytes as base 64 string.
    Base64String( const RawBytes& bytes );

    // Construct from a base 64 encoded std::string.
    Base64String( const std::string& s );

    // Print encoded string.
    Base64String( std::istream& is );

private:
    void validate() const;
};


#endif // BASE64STRING_H
