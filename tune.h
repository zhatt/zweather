// Helper class to get tuning values from environment variables.

#ifndef TUNE_H
#define TUNE_H

#include <map>
#include <string>

class Tune {
    const std::string prefix;
    std::map<std::string,std::string> variables;

    std::string add_prefix( const std::string& variable_name ) const;

  public:
    // The prefix will be combined with the variable name to create the
    // environment variable name.  For example if the prefix is PROG1_ and
    // the variable name is MAX_SIZE then the environment variable will be
    // PROG1_MAX_SIZE.  Note that you must include a underbar in the prefix if
    // you want the environment variable to have one.
    Tune( std::string prefix );

    // You must add all of the names before you can get them.  This allows you
    // to set a default value if to use if the environment variable wasn't set.
    void add_variable( const std::string& variable_name,
                       const std::string& default_value );

    // Get a value.  Throws an error if the variable wasn't previously added.
    std::string get( const std::string& variable_name ) const;
};

#endif // TUNE_H
