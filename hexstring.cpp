
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

#include "hexstring.h"
#include "rawbytes.h"

HexString::HexString() {}

HexString::HexString( const char* hex ) : std::string( hex ) {
    validate();
}

HexString::HexString( const RawBytes& bytes ) {
    std::ostringstream hex;
    hex << std::hex << std::setfill( '0' );

    for ( auto byte : bytes ) {
        hex << std::setw( 2 ) << static_cast<unsigned>( byte );
    }

    std::string* thisString = this;
    *thisString = hex.str();
}

HexString::HexString( const std::string& s ) : std::string( s ) {
    validate();
}

/*
 * Validate that the characters in this object's string are valid base 64.
 */
void HexString::validate() const {

    if ( this->size() % 2 != 0 ) {
        throw std::invalid_argument( "bad length" );
    }

    for( auto c : *this ) {
        if ( ! ( ( c >= 'a' && c <= 'f' ) ||
                ( c >= '0' && c <= '9' ) ) ) {
            throw std::invalid_argument( "bad value" );
        }
    }
}

