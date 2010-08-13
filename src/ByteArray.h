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
// ByteArray.h Simple descriptor-like class that holds both data and length attributes
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#ifndef USHORT 
#define USHORT unsigned short
#endif

#ifndef UWORD
#define UWORD unsigned int
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

class ByteArray
{
	public:
	BYTE* array;
	int length;
	ByteArray(BYTE* array,int length);
	ByteArray(){};
};

class ByteArrayAlloc : public ByteArray
{
	
	public:
	ByteArrayAlloc(BYTE* array,int length); //allocate new sapce and copy from src
	ByteArrayAlloc(int length);	
	~ByteArrayAlloc(); // free the space allocated by the constructor
};



#endif
