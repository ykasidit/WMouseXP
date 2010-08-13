/*
Copyright (C) 2007-2010 Kasidit Yusuf.

This file is part of WMouseXP.

WMouseXP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WMouseXP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WMouseXP. If not, see <http://www.gnu.org/licenses/>.
*/


/////////////////////////////////////////////////////////////////////
// Conversions.h Header for byte conversion classes for use in LayeredBTServer Draft Bluetooth Stack implementation.
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#ifndef _CONVERSION_H
#define _CONVERSION_H

#ifndef USHORT 
#define USHORT unsigned short
#endif

#ifndef UWORD
#define UWORD unsigned int
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

class Converter
    {
    public:
        
        static  void swapUSHORT(unsigned short *u) //16 bits //Change between Big-Endian and Little-Endian Formats
        {
            unsigned char tmp = *((unsigned char*)u);   
            *((unsigned char*)u) = *(((unsigned char*)u)+1);
            *(((unsigned char*)u)+1) = tmp; 
        };
        static  void swapUWORD(unsigned int *i) //32 bits //Change between Big-Endian and Little-Endian Formats
        {
            unsigned char tmp = *((unsigned char*)i);   

            *((unsigned char*)i) = *(((unsigned char*)i)+3);

            *(((unsigned char*)i)+3) = tmp; 
            
            tmp = *(((unsigned char*)i)+1);

            *(((unsigned char*)i)+1) = *(((unsigned char*)i)+2);

            *(((unsigned char*)i)+2) = tmp;              
      
        };
        
    };


class BEUS //Big-Endian USHORT
    {
    
    public:

        BEUS(USHORT inVal); // A Conversion would be made
        BEUS(BYTE* inVal); // NO CONVERSION MADE: for vals that is intentionally/explecitly set, Byte by Byte, to be Big-Endian       
        BYTE equals(USHORT val);
        BYTE equals(BYTE* val);
        void setVal(BYTE* val);
        void setVal(USHORT val);
        USHORT getValUSHORT();
        
        void copyValTo(BYTE* bptr);
        static void BEUSCopy(BYTE* dest, BYTE* src)
            {
                dest[0] = src[0];
                dest[1] = src[1];
            };
    


        BYTE val[2];
        
        
    };

class BEUW //Big-Endain UWORD
    {
    public:
        BEUW(UWORD inVal);
        BEUW(BYTE* inVal);        
        
        BYTE equals(UWORD inVal);
        BYTE equals(BYTE* inVal);
        void setVal(BYTE* inVal);
        void setVal(UWORD inVal);
        UWORD getValUWORD();
        
        void copyValTo(BYTE* bptr);
        static void BEUWCopy(BYTE* dest, BYTE* src)
            {
                dest[0] = src[0];
                dest[1] = src[1];
                dest[2] = src[2];
                dest[3] = src[3];
            };

    
        BYTE val[4];
    };


#endif