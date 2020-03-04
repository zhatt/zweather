
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "tune.h"


class TuneTest : public ::testing::Test{};

TEST( TuneTest, ConstructorEmpty ) {
    ASSERT_ANY_THROW( Tune("") );
}

TEST( TuneTest, TestDefault ) {
    Tune tune( "UNIT_TEST_" );
    tune.add_variable( "VALUE1", "default1" );

    ASSERT_EQ( "default1", tune.get( "VALUE1" ) );
}

TEST( TuneTest, TestValue1 ) {
    putenv( const_cast<char *>("UNIT_TEST_VALUE1=fromenv") );
    Tune tune( "UNIT_TEST_" );
    tune.add_variable( "VALUE1", "default1" );

    ASSERT_EQ( "fromenv", tune.get( "VALUE1" ) );
}
