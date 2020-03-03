#ifndef HEXSTRING_H
#define HEXSTRING_H

#include <string>

class RawBytes;

class HexString : public std::string {
public:
    HexString();
    HexString( const char* hex );
    HexString( const RawBytes& bytes );
    HexString( const std::string& s );

private:
    void validate() const;
};

#endif // HEXSTRING_H
