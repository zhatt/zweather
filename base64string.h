#ifndef BASE64STRING_H
#define BASE64STRING_H

#include <string>

#include "hexstring.h"

class RawBytes;

class Base64String : public std::string {

public:
    static const std::string mappingTable;

    Base64String();
    Base64String( const char* s );
    Base64String( const RawBytes& bytes );
    Base64String( const std::string& s );
    Base64String( std::istream& is );

private:
    void validate() const;
};


#endif // BASE64STRING_H
