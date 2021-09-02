#include "customfont.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <utf8utl.h>

bool CustomFont::load()
{
    std::ifstream f(_FileName);
    if(!f)
        return false;

    std::stringstream s;
    std::string line;
    UTF8Utl::UTF8CHAR c;

    unsigned int uc=0; //https://stackoverflow.com/a/6916509/3643138
    _Chars.clear();
    s << std::hex << std::showbase;
    while (getline(f,line))
    {
        char data[8] ={0,0,0,0,0,0,0,0};
        s.clear();
        s.str(line);
        size_t i=0;
        for(i=0;i<9 && !s.eof() ;i++)
        {
            if(i==0)
                s >> c;
            else
            {
                s >> std::hex >> uc ; // Reading hex data. Must read into an unsigned int
                data[i-1] = uc ;
            }
        }

        std::cout << "Char "<<std::hex << c<<" Loaded : " ;
        for(auto i=0;i<8;i++)
            std::cout << std::hex<< (int) data[i] <<" ";

        std::cout<<std::endl;
        if(i==9)
            set(c,data);
    }

    return true;
}

bool CustomFont::save() const
{
    std::ofstream f(_FileName,std::ios_base::trunc);
    if(!f)
        return false;

    dump(f);

    return true;
}

void CustomFont::dump(std::ostream &s) const
{
    std::ios::fmtflags flg( s.flags() );


    s<< std::hex ;//<< std::showbase;
    for(const auto & c : _Chars)
    {
        s << c.first ;
        for(size_t i=0;i<8;i++)
            s << " "<< ((0xFF)&((int) c.second[i]));

        s <<std::endl;
    }
    s.flags( flg );
}

void CustomFont::setFileName(const std::string & f)
{
    _FileName=f;
}

void CustomFont::get(UTF8Utl::UTF8CHAR c, char data[]  )
{
    std::memcpy(data,_Chars[c],8);
}

char CustomFont::getCol(UTF8Utl::UTF8CHAR c, size_t i)
{
    return _Chars[c][i];
}

void CustomFont::set(UTF8Utl::UTF8CHAR c, const char data[8])
{
    std::memcpy(_Chars[c],data,8);
}

bool CustomFont::isSet(UTF8Utl::UTF8CHAR c) const
{
    return _Chars.find(c) != _Chars.end();
}

std::string CustomFont::getTestingString() const
{

    std::string test;

    for(const auto i:_Chars)
        test+= UTF8Utl::Index2UTF8Char(i.first);

    return test;
}
