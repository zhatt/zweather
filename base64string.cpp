
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "base64string.h"
#include "rawbytes.h"

const std::string Base64String::mappingTable( "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              "abcdefghijklmnopqrstuvwxyz"
                                              "0123456789+/" );
Base64String::Base64String() {}

Base64String::Base64String( const char* s ) : std::string( s ) {
    validate();
}

Base64String::Base64String( const std::string& s ) : std::string( s ) {
    validate();
}

Base64String::Base64String( const RawBytes& bytes ) {

    for ( size_t i = 0; i < bytes.size(); i += 3 ) {
        uint32_t bits = 0;

        for ( size_t j = 0; j < 3; j++ ) {
            bits <<= 8;
            if ( i + j < bytes.size() ) {
                bits |= bytes[ i + j ];
            }
        }

        for ( ssize_t shiftAmount = 6*3; shiftAmount >= 0; shiftAmount -= 6 ) {
            unsigned mapIndex = ( bits >> shiftAmount ) & 0x3f;
            this->push_back( Base64String::mappingTable[ mapIndex ] );
        }
    }

    switch( ( bytes.size() ) % 3 ) {
        case 0:
            // No padding.
            break;
        case 1:
            // Pad 2
            (*this)[ this->size() - 2 ] = '=';
            // fallthrough
        case 2:
            // Pad 1
            (*this)[ this->size() - 1 ] = '=';
            break;
    }
}

Base64String::Base64String( std::istream& is ):
    Base64String( std::string ( std::istream_iterator<char>( is ),
                                std::istream_iterator<char>() ) ) {
    if ( is.bad() || ! is.eof() ) {
        throw std::runtime_error( "bad input" );
    }
}

/*
 * Validate that the characters in this object's string are valid base 64.
 */
void Base64String::validate() const {

    if ( this->size() % 4 != 0 ) {
        throw std::invalid_argument( "bad length" );
    }

    for( auto c : *this ) {
        if (  mappingTable.find( c ) == mappingTable.npos && c != '=' ) {
            throw std::invalid_argument( "bad value" );
        }
    }
}

