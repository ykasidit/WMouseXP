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
// TidyVector.cpp An auto memory clearing derivative of CPtrArray
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tidyvector.h"

TidyVector::TidyVector(void)
	{
	}

void TidyVector::RemoveAt( int nIndex )
	{
		delete (this->GetAt(nIndex));		 
		CPtrArray::RemoveAt(nIndex,1);

	}
void TidyVector::RemoveAll( )
	{
		for(int i=0;i<this->GetSize();i++)
			delete (this->GetAt(i));		 
		CPtrArray::RemoveAll();
	}
TidyVector::~TidyVector(void)
	{
		RemoveAll();
	}