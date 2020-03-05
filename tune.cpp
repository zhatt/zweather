
#include <iostream>
#include "tune.h"

Tune::Tune( const std::string prefix ) :
    prefix( prefix ) {
        if ( prefix.size() == 0 ) {
            throw( std::logic_error( "Error prefix cannot be empty" ) );
        }
    }


std::string
Tune::add_prefix( const std::string& variable_name ) const {
    return prefix + variable_name;
}


void
Tune::add_variable(const std::string& variable_name,
                   const std::string& default_value) {
    std::string name_with_prefix = add_prefix( variable_name );

    char* value_ptr = getenv( name_with_prefix.c_str() );
    if ( value_ptr ) {
        variables[ name_with_prefix ] = std::string( value_ptr );
    } else {
        variables[ name_with_prefix ] = default_value;
    }
}

std::string
Tune::get( const std::string& variable_name ) const {

    std::string name_with_prefix = add_prefix( variable_name );

    auto iter = variables.find( name_with_prefix );
    if ( iter == variables.end() ) {
        throw( std::logic_error( "Unknown tuning variable " + name_with_prefix ) );
    }

    return iter->second;
}
