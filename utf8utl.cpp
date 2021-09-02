#include "utf8utl.h"
#include <iostream>

std::string UTF8Utl::Index2UTF8Char(UTF8Utl::UTF8CHAR cnt)
{
    long long int sequence=0;
    std::string utf;
    //xxxx xxxx
    if(cnt <= 0x7F)
    {
        sequence = cnt;
    }
    else if (cnt <= 0x7FF)              // ---xxxxx--xxxxxx // Mask
    {                                   // 00000xxxxxxxxxxx // value
        sequence = 0xC080;              // 000.....xxxxxx-- // value
        sequence |= (cnt)   &0x003F;
        sequence |= (cnt<<2)&0x1F00;
    }
    else if(cnt <= 0xFFFF)
    {
        sequence = 0xE08080;
        sequence |= (cnt)    & 0x00003F;
        sequence |= (cnt<<2) & 0x003F00;
        sequence |= (cnt<<4) & 0x0F0000;
    }
    else if (cnt <= 0x10FFFF)
    {
        sequence = 0xF0808080;
        sequence |= (cnt)    & 0x0000003F;
        sequence |= (cnt<<2) & 0x00003F00;
        sequence |= (cnt<<4) & 0x003F0000;
        sequence |= (cnt<<6) & 0xF8000000;
    }


    for(auto i=4;i>=0;i--)
    {
        char c = (char) ((sequence>>i*8)&0xFF);
        if(c==0x00)
            continue;
        utf+=c;
    }

    return utf;
}

size_t UTF8Utl::Utf8CharSize(const char *c)
{
    //0xxxxxxx                                                   7	 007F hex (127)
    //110xxxxx	10xxxxxx                                 (5+6)=11	 07FF hex (2047)
    //1110xxxx	10xxxxxx	10xxxxxx                    (4+6+6)=16	 FFFF hex (65535)
    //11110xxx	10xxxxxx	10xxxxxx	10xxxxxx        (3+6+6+6)=21 10FFFF hex (1,114,111)

    auto n = ( (*c & 0xF0)>>4 );
    size_t s = 0;

    if(n<0x8)
        return 1;

    while(n & 0x8)
    {
        s++;
        n = n<<1;
    }

    return s;
}

UTF8Utl::UTF8CHAR UTF8Utl::UTF8Char2Index(const char *str)
{

    unsigned int utfchar = 0;
    size_t it = 0;
    char c = str[it] ;

    //0xxxxxxx                                                   7	 007F hex (127)
    //110xxxxx	10xxxxxx                                 (5+6)=11	 07FF hex (2047)
    //1110xxxx	10xxxxxx	10xxxxxx                    (4+6+6)=16	 FFFF hex (65535)
    //11110xxx	10xxxxxx	10xxxxxx	10xxxxxx        (3+6+6+6)=21 10FFFF hex (1,114,111)

    if(c<127)
    {
        utfchar = c&0x7F;
    }
    else if((c&0xE0) == (0xC0) )
    {
        utfchar = (c&0x1F)<< 6;
        c=str[++it];
        utfchar |= c&0x3F;
    }
    else if((c&0xF0) == (0xE0) )
    {
        utfchar = (c&0x0F)<< 2*6;
        c=str[++it];
        utfchar |= (c&0x3F) <<6;
        c=str[++it];
        utfchar |= (c&0x3F) ;
    }
    else if((c&0xF8) == (0xF0) )
    {
        utfchar = (c&0x07)<< 3*6;
        c=str[++it];
        utfchar |= (c&0x3F) <<2*6;
        c=str[++it];
        utfchar |= (c&0x3F) << 6 ;
        c=str[++it];
        utfchar |= (c&0x3F);
    }

    return utfchar;
}

UTF8Utl::UTF8Utl()
{

}
