
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "base64string.h"
#include "hexstring.h"
#include "rawbytes.h"

#include "base64-test-cases.h"

class RawBytesTest : public ::testing::Test{};

TEST( RawBytesTest, ConstructorEmpty ) {
    RawBytes bytes;

    EXPECT_EQ( 0, bytes.size() );
}

TEST( RawBytesTest, ConstructorCopy ) {
    const RawBytes bytes1 { 0x12, 0x34, 0x56, 0x78, 0x90 };
    const RawBytes bytes2( bytes1 );

    EXPECT_EQ( bytes1, bytes2 );
}

TEST( RawBytesTest, ConstructorMove ) {
    const RawBytes bytes1 { 0x12, 0x34, 0x56, 0x78, 0x90 };
    RawBytes bytes2( bytes1 );
    RawBytes bytes3( std::move( bytes2 ) );

    EXPECT_EQ( bytes1, bytes3 );
    EXPECT_EQ( 0, bytes2.size() );
}

TEST( RawBytesTest, ConstructorRawBytes ) {
    const HexString hex1( "1234567890" );  // digits
    const HexString hex2( "abcdef" ); // hex digits
    const HexString hex3( "0b11c87e" );  // mixed

    // Construct from HexString.
    RawBytes bytes1( hex1 );
    RawBytes bytes2( hex2 );
    RawBytes bytes3( hex3 );

    const RawBytes expected1 { 0x12, 0x34, 0x56, 0x78, 0x90 };
    const RawBytes expected2 { 0xab, 0xcd, 0xef };
    const RawBytes expected3 { 0x0b, 0x11, 0xc8, 0x7e };

    EXPECT_EQ( expected1, bytes1 );
    EXPECT_EQ( expected2, bytes2 );
    EXPECT_EQ( expected3, bytes3 );
}

TEST( RawBytesTest, ConstructorBase64String ) {
    for ( const auto& test : base64TestCases ) {
        Base64String base64( test.second );
        RawBytes bytes( base64 );

        EXPECT_EQ( test.first, std::string( bytes ) );
    }
}

TEST( RawBytesTest, ConstructorString ) {
    {
    std::string data( "Test Data." );

    RawBytes bytes( data );

    EXPECT_EQ( 10, bytes.size() );

    EXPECT_EQ( 'T', bytes[0] );
    EXPECT_EQ( 'e', bytes[1] );
    EXPECT_EQ( 's', bytes[2] );

    EXPECT_EQ( 'a', bytes[8] );
    EXPECT_EQ( '.', bytes[9] );
    }

    // Test with null byte embedded in string.
    {
    std::string data;
    data += "Data1";
    data.push_back( 0 );
    data += "Data2";

    RawBytes bytes( data );

    EXPECT_EQ( 11, bytes.size() );

    EXPECT_EQ( 'D', bytes[0] );
    EXPECT_EQ( 'a', bytes[1] );
    EXPECT_EQ( 't', bytes[2] );
    EXPECT_EQ( 'a', bytes[3] );
    EXPECT_EQ( '1', bytes[4] );
    EXPECT_EQ(   0, bytes[5] );
    EXPECT_EQ( 'D', bytes[6] );
    EXPECT_EQ( 'a', bytes[7] );
    EXPECT_EQ( 't', bytes[8] );
    EXPECT_EQ( 'a', bytes[9] );
    EXPECT_EQ( '2', bytes[10] );
    }
}

TEST( RawBytesTest, ConstructorConstCharPointer ) {
    {
    RawBytes bytes( "Test Data." );

    EXPECT_EQ( 10, bytes.size() );

    EXPECT_EQ( 'T', bytes[0] );
    EXPECT_EQ( 'e', bytes[1] );
    EXPECT_EQ( 's', bytes[2] );

    EXPECT_EQ( 'a', bytes[8] );
    EXPECT_EQ( '.', bytes[9] );
    }

    // Test with null byte embedded in string.  This is allowed for strings but
    // doesn't really work for const char* because the data will be truncated.
    {
    RawBytes bytes( "Data1\0Data2" );

    EXPECT_EQ( 5, bytes.size() );

    EXPECT_EQ( 'D', bytes[0] );
    EXPECT_EQ( 'a', bytes[1] );
    EXPECT_EQ( 't', bytes[2] );
    EXPECT_EQ( 'a', bytes[3] );
    EXPECT_EQ( '1', bytes[4] );
    EXPECT_EQ(   0, bytes[5] );
    }
}

TEST( RawBytesTest, ConstructorLoopBytes ) {
    std::string keyString( "ABC" );
    RawBytes key( keyString );

    {
    RawBytes bytes( 0, key );
    EXPECT_EQ( 0, bytes.size() );
    }

    {
    RawBytes bytes( 1, key );
    EXPECT_EQ( "A", std::string( bytes ) );
    }

    {
    RawBytes bytes( 2, key );
    EXPECT_EQ( "AB", std::string( bytes ) );
    }

    {
    RawBytes bytes( 3, key );
    EXPECT_EQ( "ABC", std::string( bytes ) );
    }

    {
    RawBytes bytes( 4, key );
    EXPECT_EQ( "ABCA", std::string( bytes ) );
    }

    {
    RawBytes bytes( 5, key );
    EXPECT_EQ( "ABCAB", std::string( bytes ) );
    }

    {
    RawBytes bytes( 6, key );
    EXPECT_EQ( "ABCABC", std::string( bytes ) );
    }

    {
    RawBytes bytes( 7, key );
    EXPECT_EQ( "ABCABCA", std::string( bytes ) );
    }
}

TEST( RawBytesTest, RawBytesOperatorString ) {
    {
    RawBytes bytes{ 'T', 'e', 's', 't' };

    EXPECT_EQ( "Test", std::string( bytes ) );
    }

    // Test with null byte embedded in data.
    {
    RawBytes bytes{ 'T', 'e', 's', 't', 0, 'I', 't' };

    std::string s( bytes );
    EXPECT_EQ( 'T', s[0] );
    EXPECT_EQ( 'e', s[1] );
    EXPECT_EQ( 's', s[2] );
    EXPECT_EQ( 't', s[3] );
    EXPECT_EQ(   0, s[4] );
    EXPECT_EQ( 'I', s[5] );
    EXPECT_EQ( 't', s[6] );
    }
}

TEST( RawBytesTest, RawBytesXor ) {
    {
    const RawBytes key{ 0x01 };
    RawBytes result{ 0xFF, 0xFF };

    const RawBytes expect{ 0xFE, 0xFE };

    result.x_or( key );

    EXPECT_EQ( expect, result );
    }

    {
    const RawBytes key{ 0x01, 0x02 };
    RawBytes result{ 0xFF, 0xFF };

    const RawBytes expect{ 0xFE, 0xFD };

    result.x_or( key );

    EXPECT_EQ( expect, result );
    }

    {
    const RawBytes key{ 0x01, 0x02, 0x55 };
    RawBytes result{ 0xFF, 0xFF };

    const RawBytes expect{ 0xFE, 0xFD };

    result.x_or( key );

    EXPECT_EQ( expect, result );
    }

    {
    const RawBytes key{ 0x01, 0x02 };
    RawBytes result{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    const RawBytes expect{ 0xFE, 0xFD, 0xFE, 0xFd, 0xFE };

    result.x_or( key );

    EXPECT_EQ( expect, result );
    }
}

TEST( RawBytesTest, RawBytesHammingDistance ) {
    {
    RawBytes a { 0x00 };
    RawBytes b { 0x00 };
    unsigned distance = RawBytes::HammingDistance( a, b );
    EXPECT_EQ( 0, distance );
    }

    {
    RawBytes a { 0x00 };
    RawBytes b { 0xff };
    unsigned distance = RawBytes::HammingDistance( a, b );
    EXPECT_EQ( 8, distance );
    }

    {
    RawBytes a { 0x34 }; //0011 0100
    RawBytes b { 0x29 }; //0010 1001
    unsigned distance = RawBytes::HammingDistance( a, b );
    EXPECT_EQ( 4, distance );
    }

    {
    RawBytes a( "this is a test" );
    RawBytes b( "wokka wokka!!!" );
    unsigned distance = RawBytes::HammingDistance( a, b );
    EXPECT_EQ( 37, distance );
    }
}

TEST( RawBytesTest, ExtractBytes ) {
    const RawBytes a( "abcdefghijklmnop" );

    EXPECT_EQ( "abcdefghijklmnop", std::string( a.extractStepped( 0, 1 ) ) );
    EXPECT_EQ( "bcdefghijklmnop",  std::string( a.extractStepped( 1, 1 ) ) );
    EXPECT_EQ( "cdefghijklmnop",   std::string( a.extractStepped( 2, 1 ) ) );

    EXPECT_EQ( "acegikmo", std::string( a.extractStepped( 0, 2 ) ) );
    EXPECT_EQ( "bdfhjlnp", std::string( a.extractStepped( 1, 2 ) ) );
    EXPECT_EQ( "cegikmo",  std::string( a.extractStepped( 2, 2 ) ) );

    EXPECT_EQ( "adgjmp", std::string( a.extractStepped( 0, 3 ) ) );
    EXPECT_EQ( "behkn", std::string( a.extractStepped( 1, 3 ) ) );
    EXPECT_EQ( "cfilo", std::string( a.extractStepped( 2, 3 ) ) );
}

TEST( RawBytesTest, ScoreAsCharacters ) {
    RawBytes a( "abcdefghij" );
    EXPECT_EQ( 10, a.scoreAsCharacters() );

    RawBytes b( "R#l$fg-hij" );
    EXPECT_EQ( 7, b.scoreAsCharacters() );

    RawBytes c{ 2, 3, 4, 5};
    EXPECT_EQ( 0, c.scoreAsCharacters() );

    RawBytes d{ 2, 'A', 4, 'C' };
    EXPECT_EQ( 2, d.scoreAsCharacters() );

}
