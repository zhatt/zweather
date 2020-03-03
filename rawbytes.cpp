
#include <cassert>
#include <climits>
#include <iomanip>
#include <sstream>

#include "base64string.h"
#include "hexstring.h"
#include "rawbytes.h"

template class std::allocator<uint8_t>;

RawBytes::RawBytes( const Base64String& in ) {
    for( auto iter = in.begin(); iter != in.end(); iter += 4 ) {
        unsigned numEqualPad = 0;
        uint32_t bits = 0;
        for( unsigned i = 0; i < 4; i++ ) {
            char c = *(iter + i);
            uint8_t nibble;
            if ( c == '=' ) {
                numEqualPad ++;
                nibble = 0;
            } else {
                nibble = Base64String::mappingTable.find( c );
            }
            bits |= nibble << ( 6*(3-i) );
        }

        for ( unsigned i = 0; i < 3-numEqualPad; i++ ) {
            uint8_t value = ( bits >> (16-8*i) );
            this->push_back( value );
        }
    }
}

RawBytes::RawBytes( const HexString& hex ) {
    for( auto iter = hex.begin(); iter != hex.end(); ++iter ) {
        uint8_t bitsUpper;
        if ( *iter >= 'a' && *iter <= 'z' ) {
            bitsUpper = *iter - 'a' + 10;
        } else {
            bitsUpper = *iter - '0' + 0;
        }

        ++iter;

        uint8_t bitsLower;
        if ( *iter >= 'a' && *iter <= 'z' ) {
            bitsLower = *iter - 'a' + 10;
        } else {
            bitsLower = *iter - '0' + 0;
        }

        uint8_t bits = ( bitsUpper << 4 ) | bitsLower;
        this->push_back( bits );
    }
}

RawBytes::RawBytes( const char* c ) :
    RawBytes( std::string( c ) ) {}

RawBytes::RawBytes( const std::string& s ) :
    std::vector<uint8_t>( s.begin(), s.end() ) {}

RawBytes::RawBytes( size_type n, const RawBytes& bytes ) :
    std::vector<uint8_t>( 0 ) {

    assert( bytes.size() != 0 );

    this->reserve( n );

    auto iter = bytes.cbegin();

    while( n-- > 0 ) {
        this->push_back( *iter );
        if ( ++iter == bytes.cend() ) {
            iter = bytes.cbegin();
        }
    }
}

RawBytes::operator std::string() const {
    return std::string( this->begin(), this->end() );
}

void RawBytes::x_or( const RawBytes& bytes ) {
    assert( bytes.size() != 0 );

    auto iter = bytes.cbegin();

    for ( auto& b : *this ) {
        b ^= *iter;
        if ( ++iter == bytes.cend() ) {
            iter = bytes.cbegin();
        }
    }
}

RawBytes RawBytes::extractStepped( size_t start, size_t step ) const {
    if ( step == 0 ) {
        throw std::invalid_argument( "step is zero" );
    }

    if ( ! ( this->cbegin() + start <= this->end() ) ) {
        throw std::invalid_argument( "start is after end" );
    }

    RawBytes r;

    for ( auto iter = this->cbegin() + start; iter < this->end();
          iter += step ) {
        r.push_back( *iter );
    }

    return r;
}

unsigned RawBytes::scoreAsCharacters() const {
    // Score is number of characters, digits, and spaces.
    unsigned score = 0;
    for ( auto c : *this ) {
        if ( isalnum( c ) || isspace( c ) ) score++;
    }
    return score;
}

unsigned RawBytes::HammingDistance( const RawBytes& a, const RawBytes& b ) {
    assert( a.size() == b.size() );
    assert( a.size() * 8 <= UINT_MAX );

    unsigned distance = 0;

    // Brute force bit counting.
    for ( size_t i = 0; i < a.size(); i++ ) {
        uint8_t bits = a[i] ^ b[i];
        for( unsigned j = 0; j < 8; j++ ) {
            distance += bits & 0x01;
            bits >>= 1;
        }
    }

    return distance;
}

