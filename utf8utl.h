#ifndef UTF8UTL_H
#define UTF8UTL_H
#include <string>

class UTF8Utl
{

public :

    typedef unsigned int UTF8CHAR;

    static std::string Index2UTF8Char(UTF8CHAR cnt);
    static size_t Utf8CharSize(const char* c);
    static UTF8CHAR UTF8Char2Index(const char *str);

public:
    UTF8Utl();
};

#endif // UTF8UTL_H
