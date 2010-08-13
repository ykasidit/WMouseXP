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

////////////////////////////////////////////////////////////////////
// Conversions.cpp Implementation for byte conversion classes for use in LayeredBTServer Draft Bluetooth Stack implementation.
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Conversions.h"

BEUS::BEUS(USHORT inVal) // A Conversion would be made
    {
        this->val[0] = ((BYTE*)(&inVal))[1];
        this->val[1] = ((BYTE*)(&inVal))[0];
    };
BEUS::BEUS(BYTE* inVal) // NO CONVERSION MADE: for vals that is intentionally/explecitly set, Byte by Byte, to be Big-Endian       
    {
        this->val[0] = inVal[0];
        this->val[1] = inVal[1];
    };
BYTE BEUS::equals(USHORT inVal)
    {
        if( ((BYTE*)&inVal)[0] == this->val[1] && ((BYTE*)&inVal)[1] == this->val[0])
            return 1;
        return 0;
    };
BYTE BEUS::equals(BYTE* inVal)
    {
        if( inVal[0] == this->val[0] && inVal[1] == this->val[1])
            return 1;
        return 0;
    };
void BEUS::setVal(BYTE* inVal)
    {
        this->val[0] = inVal[0];
        this->val[1] = inVal[1];
    };
void BEUS::setVal(USHORT inVal)
    {
        this->val[0] = ((BYTE*)(&inVal))[1];
        this->val[1] = ((BYTE*)(&inVal))[0];
    };
USHORT BEUS::getValUSHORT()
    {
        USHORT ret;
        ((BYTE*)&ret)[0] = this->val[1];
        ((BYTE*)&ret)[1] = this->val[0];
        return ret;
    };


/*BYTE*  BEUS::getValBPtr()
    {
        BYTE* ret = new BYTE[2];
        ret[0] = this->val[0];
        ret[1] = this->val[1];
        return ret;
    };
*/

void BEUS::copyValTo(BYTE* bptr)
    {
        bptr[0] = this->val[0];
        bptr[1] = this->val[1];
    };

BEUW::BEUW(UWORD inVal)
    {
        this->val[0] = ((BYTE*)(&inVal))[3];
        this->val[1] = ((BYTE*)(&inVal))[2];
        this->val[2] = ((BYTE*)(&inVal))[1];
        this->val[3] = ((BYTE*)(&inVal))[0];
    };
BEUW::BEUW(BYTE* inVal)
    {
        this->val[0] = inVal[0];
        this->val[1] = inVal[1];
        this->val[2] = inVal[2];
        this->val[3] = inVal[3];
    };
BYTE BEUW::equals(UWORD inVal)
    {
        if( ((BYTE*)&inVal)[0] == this->val[3] && 
            ((BYTE*)&inVal)[1] == this->val[2] && 
            ((BYTE*)&inVal)[2] == this->val[1] && 
            ((BYTE*)&inVal)[3] == this->val[0])
            return 1;
        return 0;
    };
BYTE BEUW::equals(BYTE* inVal)
    {
            if( inVal[0] == this->val[0] && 
                inVal[1] == this->val[1] &&
                inVal[2] == this->val[2] && 
                inVal[3] == this->val[3])
            return 1;
        return 0;
        
    };
void BEUW::setVal(BYTE* inVal)
    {
        this->val[0] = val[0];
        this->val[1] = val[1];
        this->val[2] = val[2];
        this->val[3] = val[3];
        
    };
void BEUW::setVal(UWORD inVal)
    {
        this->val[0] = ((BYTE*)(&inVal))[3];
        this->val[1] = ((BYTE*)(&inVal))[2];
        this->val[2] = ((BYTE*)(&inVal))[1];
        this->val[3] = ((BYTE*)(&inVal))[0];
    };
UWORD BEUW::getValUWORD()
    {
        UWORD ret;
        ((BYTE*)&ret)[0] = this->val[3];
        ((BYTE*)&ret)[1] = this->val[2];
        ((BYTE*)&ret)[2] = this->val[1];
        ((BYTE*)&ret)[3] = this->val[0];

        return ret;
    };
/*
BYTE*  BEUW::getValBPtr()
    {
        BYTE* ret = new BYTE[4];
        
        ret[0] = this->val[0];
        ret[1] = this->val[1];
        ret[2] = this->val[2];
        ret[3] = this->val[3];

        return ret;
    };*/


void BEUW::copyValTo(BYTE* bptr)
    {
        bptr[0] = this->val[0];
        bptr[1] = this->val[1];
        bptr[2] = this->val[2];
        bptr[3] = this->val[3];
        
    };
