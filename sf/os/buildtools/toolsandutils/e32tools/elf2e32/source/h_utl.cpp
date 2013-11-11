// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the e32 image version for e32 image dump for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include <stdlib.h>
#include <stdio.h>
#include "h_utl.h"

/**
Function to check bracketed hex i.e '{', '}'

@internalComponent
@released

@param aTemp
@param aBrackets
@param aDigits
@param aValue

@return True if the value passed in is a bracketed hex.
*/
TBool IsBracketedHex(const char* aTemp, const char* aBrackets, TInt aDigits, TUint32& aValue)
{
	if (aTemp[0]!=aBrackets[0] || aTemp[1+aDigits]!=aBrackets[1])
		return 0;
	TInt i;
	TUint32 x = 0;
	for (i=1; i<=aDigits; ++i)
	{
		TInt c = aTemp[i];
		if (c>='a' && c<='z') c-=32;
		if (c<'0' || (c>'9' && c<'A') || c>'F')
			return 0;
		c-='0';
		if (c>9)
			c-=7;
		x = (x<<4) | (TUint32)c;
	}
	aValue = x;
	
	return 1;
}

/**
Function to check the decimal version

@internalComponent
@released

@param aBegin
Beginning of the version information
@param aTemp
@param aValue
Holds the hexadecimal value
@return the checked value.
*/
TInt CheckForDecimalVersion(const char* aBegin, const char* aTemp, TUint32& aValue)
{
	aValue = 0;
	if (aTemp <= aBegin || *aTemp != '}')
		return 0;
	TUint32 v[2] = {0,0};
	TUint32 m = 1;
	TInt pos = 0;
	const char* s0 = aTemp + 1;
	for (--aTemp; aTemp >= aBegin; --aTemp)
	{
		int c = *aTemp;
		if (c >= '0' && c <= '9')
		{
			v[pos] += m * (c - '0');
			if (v[pos] >= 65536u)
				return 0;
			m *= 10;
		}
		else if (c == '.')
		{
			m = 1;
			if (++pos >= 2)
				return 0;
		}
		else if (c == '{')
			break;
		else
			return 0;
	}
	if (aTemp < aBegin)
		return 0;
	
	aValue = (v[1] << 16) | v[0];
	
	return s0 - aTemp;
}

/**
Function to Parse a filename and convert decimal version number to hex

@internalComponent
@released

@param aName
Filename to be parsed

@return the converted name wherein the decimal number is converted to hex.
*/
char* NormaliseFileName(const char* aName)
{
	TFileNameInfo f(aName, 0);
	TInt nl = f.iBaseLength;
	TInt el = f.iTotalLength - f.iExtPos;
	TInt tl = nl + el;
	if (f.iFlags & EVerPresent)
		tl += 10;
	char* t = new char[tl + 1];
	if (t)
	{
		memcpy(t, aName, nl);
		if (f.iFlags & EVerPresent)
			sprintf(t + nl, "{%08x}%s", (TInt)f.iModuleVersion, aName + f.iExtPos);
		else if (el)
			memcpy(t + nl, aName + f.iExtPos, el);
		t[tl] = 0;
	}
	
	return t;
}

/**
Constructor for Class TFileNameInfo

@internalComponent
@released

@param aFileName
Filename to be parsed
@param aLookForUid
*/
TFileNameInfo::TFileNameInfo(const char* aFileName, TBool aLookForUid)
{
	iFileName = aFileName;
	TInt l = strlen(aFileName);
	iTotalLength = l;
	TInt remain = l;
	iFlags = 0;
	iUid3 = 0;
	iModuleVersion = 0;
	iBaseLength = l;
	iExtPos = l;
	const char* s = iFileName + l;
	for (; s>=iFileName && *s!='.' && *s!='}' && (!aLookForUid || *s!=']'); --s)
	{
	}
	
	if (s<iFileName)
		return;
	if (*s == '.')
	{
		iExtPos = s - iFileName;
		if (iExtPos == 0)
		{
			iBaseLength = 0;
			return;
		}
		remain = iExtPos;
		--s;
	}
	else if (s != iFileName + l)
		return;
	
	if (aLookForUid && remain>=10 && IsBracketedHex(s-9, "[]", 8, iUid3))
	{
		iFlags |= EUidPresent;
		remain -= 10;
		s -= 10;
	}
	
	if (remain>=10 && IsBracketedHex(s-9, "{}", 8, iModuleVersion))
	{
		iFlags |= EVerPresent;
		remain -= 10;
		s -= 10;
	}
	else
	{
		TInt n = CheckForDecimalVersion(iFileName, s, iModuleVersion);
		if (n>0)
		{
			iFlags |= EVerPresent;
			remain -= n;
			s -= n;
		}
	}
	iBaseLength = remain;
}

/*-------------------------------------------------------------------------
String comparison on Linux seems to be a little half-baked.
-------------------------------------------------------------------------*/
#ifdef __LINUX__

int stricmp(const char *a, const char *b)
	{
	unsigned char ca,cb;

	do {
		ca = *a++;
		cb = *b++;
		ca = tolower(ca);
		cb = tolower(cb);
	}
	while((ca == cb) && (ca));
	return (int) ca-cb;
	}

int strnicmp(const char *a, const char *b, int n)
	{
	unsigned char ca,cb;
	int i = 0;

	do {
		if (++i > n) return 0;
		ca = *a++;
		cb = *b++;
		ca = tolower(ca);
		cb = tolower(cb);
	}
	while((ca == cb) && (ca));
	return (int) ca-cb;
	}

char* strupr(char *a)
	{
		char *ret = a;

		while (*a)
			{
			*a = toupper(*a);
			a++;
			}

		return ret;
	}


#endif
