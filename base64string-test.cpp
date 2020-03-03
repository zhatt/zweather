
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "base64string.h"
#include "rawbytes.h"

#include "base64-test-cases.h"

class Base64StringTest : public ::testing::Test{};

TEST( Base64StringTest, ConstructorEmpty ) {
    const Base64String b;
    EXPECT_EQ( 0, b.size() );
}

TEST( Base64StringTest, ConstructorCopy ) {
    const Base64String b1( "cGxlYXN1cmUu" );
    const Base64String b2( b1 );
    EXPECT_EQ( "cGxlYXN1cmUu", b2 );
}

TEST( Base64StringTest, ConstructorMove ) {
    Base64String b1( "cGxlYXN1cmUu" );
    const Base64String b2( std::move( b1 ) );
    EXPECT_EQ( "cGxlYXN1cmUu", b2 );
    EXPECT_EQ( "", b1 );
}

TEST( Base64StringTest, ConstructorCharPtr ) {
    const Base64String b1( "cGxlYXN1cmUu" );
    EXPECT_EQ( "cGxlYXN1cmUu", b1 );
}

TEST( Base64StringTest, ConstructorString ) {
    const std::string s1( "cGxlYXN1cmUu" );
    const Base64String b1( s1 );
    EXPECT_EQ( "cGxlYXN1cmUu", b1 );
}

TEST( Base64StringTest, ConstructorFromRawBytes ) {
    /*
     * Run each test case in the test cases array.
     */
    for ( const auto& test : base64TestCases ) {
        RawBytes bytes( test.first );
        Base64String base64( bytes );
        std::string expect( test.second );
        EXPECT_EQ( expect, base64 );
    }
}

TEST( Base64StringTest, ConstructorIstream ) {
    std::istringstream s( "cGxlYXN1cmUu" );
    const Base64String b1( s );
    EXPECT_EQ( "cGxlYXN1cmUu", b1 );
}

TEST( Base64StringTest, BadString ) {
    const std::string s1( "GxlYXN1cmUu" );
    EXPECT_THROW( const Base64String b1( s1 ), std::invalid_argument );

    const std::string s2( "cGxlYXN^cmUu" );
    EXPECT_THROW( const Base64String b1( s2 ), std::invalid_argument );
}
