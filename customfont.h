#ifndef CUSTOMFONT_H
#define CUSTOMFONT_H

#include <map>
#include <string>
#include <utf8utl.h>

class CustomFont
{
public:
    bool load();
    bool save() const ;
    void dump(std::ostream & s)const;
    void setFileName(const std::string &);
    void get(UTF8Utl::UTF8CHAR c, char data[8] ) ;
    char getCol(UTF8Utl::UTF8CHAR c, size_t i) ;
    void set(UTF8Utl::UTF8CHAR c, const char data[8]);
    bool isSet(UTF8Utl::UTF8CHAR c) const ;
    std::string getTestingString() const ;
protected:
    std::map<UTF8Utl::UTF8CHAR,char[8]> _Chars;
    std::string _FileName;
};

#endif // CUSTOMFONT_H
