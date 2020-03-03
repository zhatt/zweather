
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "hexstring.h"
#include "rawbytes.h"

class HexStringTest : public ::testing::Test{};

TEST( HexStringTest, ConstructorEmpty ) {
    HexString h;

    EXPECT_EQ( 0, h.size() );

}

TEST( HexStringTest, ConstructorCopy ) {
    const HexString hex1;
    const HexString hex2( "1234567890" );

    const HexString hex3( hex1 );
    EXPECT_EQ( "", hex3 );

    const HexString hex4( hex2 );
    EXPECT_EQ( "1234567890", hex4 );
}

TEST( HexStringTest, ConstructorMove ) {
    // hex1 and hex2 can't be const if we plan to move them.
    HexString hex1;
    HexString hex2( "1234567890" );

    const HexString hex3( std::move( hex1 ) );
    EXPECT_EQ( "", hex3 );
    EXPECT_EQ( "", hex1 );

    const HexString hex4( std::move( hex2 ) );
    EXPECT_EQ( "1234567890", hex4 );
    EXPECT_EQ( "", hex2 );
}

TEST( HexStringTest, ConstructorCharPtr ) {
    const HexString hex1( "1234567890" );  // digits
    const HexString hex2( "abcdef" ); // hex digits
    const HexString hex3( "0b11c87e" );  // mixed

    EXPECT_EQ( "1234567890", hex1 );
    EXPECT_EQ( "abcdef", hex2 );
    EXPECT_EQ( "0b11c87e", hex3 );
}

TEST( HexStringTest, ConstructorFromRawBytes ) {
    const RawBytes bytes1{ 0x12, 0x34, 0x56, 0x78, 0x90 };
    const RawBytes bytes2{ 0xab, 0xcd, 0xef };
    const RawBytes bytes3{ 0x0b, 0x11, 0xc8, 0x7e };

    const HexString hex1( bytes1 );
    const HexString hex2( bytes2 );
    const HexString hex3( bytes3 );

    EXPECT_EQ( "1234567890", hex1 );
    EXPECT_EQ( "abcdef", hex2 );
    EXPECT_EQ( "0b11c87e", hex3 );

    EXPECT_EQ( 5, bytes1.size() );
    EXPECT_EQ( 3, bytes2.size() );
    EXPECT_EQ( 4, bytes3.size() );
}

TEST( HexStringTest, ConstructorString ) {
    const HexString hex1( std::string( "1234567890" ) );  // digits
    const HexString hex2( std::string( "abcdef" ) ); // hex digits
    const HexString hex3( std::string( "0b11c87e" ) );  // mixed

    EXPECT_EQ( "1234567890", hex1 );
    EXPECT_EQ( "abcdef", hex2 );
    EXPECT_EQ( "0b11c87e", hex3 );
}

TEST( HexStringTest, BadString ) {
    EXPECT_THROW( const HexString h1( "123" ), std::invalid_argument );
    EXPECT_THROW( const HexString h2( "123g" ), std::invalid_argument );
}
