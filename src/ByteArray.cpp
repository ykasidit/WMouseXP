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
// ByteArray.cpp Simple descriptor-like class that holds both data and length attributes
// Autor: Kasidit Yusuf
// Copyright (c) 2005 Kasidit Yusuf. All rights reserved.
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ByteArray.h"

ByteArray::ByteArray(BYTE* array,int length){this->array = array; this->length = length;};

ByteArrayAlloc::ByteArrayAlloc(int length){this->length = length; this->array = new BYTE[length];};
ByteArrayAlloc::ByteArrayAlloc(BYTE* array,int length) //allocate new sapce and copy from src
{
		int i;

		this->array = new BYTE[length];
		this->length=length;
		for(i=length -1; i>=0; i--)
			*(this->array+i) = *(array+i);	
		
};
ByteArrayAlloc::~ByteArrayAlloc(){delete[] array;};
