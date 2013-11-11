/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Header STRNG.H
*
*/


#ifndef __STRNG_H__
#define __STRNG_H__

#include <string.h>
#include <stdlib.h>

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#include <iostream.h>
#else //!__VC32__ || __MSVCDOTNET__
#include <iostream>
using namespace std;
#endif //!__VC32__ || __MSVCDOTNET__

#include "GDR.H"

class String	
/**
NB only byte strings supported, no Unicode yet
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	inline String();
	inline String(const char* aText);
	inline String(int aLength, char* aText);
	inline String(const String& aString);
	inline String& operator = (const char* aText);
	inline String& operator = (const String& aString);
	inline String& operator += (const char aChar);
	inline String& operator += (const String& aString);
	inline int operator == (const String& aString) const;
	inline char& operator [] (const int aNum) const;
	inline int Length() const;
	inline const char* Text();
	IMPORT_C virtual void Externalize(ostream& out);
protected:
	inline void CopyText(char* aDest, const char* aSource, int aLength) const;
	IMPORT_C int CreateText(const int aLength);
	IMPORT_C void DeleteText(char* aText) const;
public:
	IMPORT_C ~String();
protected:
	int32 iLength;	// length of string
	char* iText;	// data
friend ostream& operator << (ostream& out, const String& aString);
	};

#include "STRNG.INL"

#endif
