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
* Header STRNG.INL
*
*/


/**
 @file
 @publishedAll 
*/

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String::String()
 :	iLength(0), iText(NULL)
	{
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String::String(const char* aText)
 :	iLength(0), iText(NULL)
	{
	if (aText && CreateText(strlen(aText)))
		{
		CopyText(iText, aText, iLength);
		iText[iLength] = '\0';
		}
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String::String(int aLength,char* aText)
 :	iLength(0), iText(NULL)
	{
	if (aText && CreateText(aLength))
		{
		CopyText(iText, aText, aLength);
		iText[iLength] = '\0';
		}
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String::String(const String& aString)
 :	iLength(0), iText(NULL)
	{
	if (CreateText(aString.iLength))
		{
		CopyText(iText, aString.iText, iLength);
		iText[iLength] = '\0';
		}
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String& String::operator = (const char* aText)
	{
	char* text = iText;
	if (CreateText(strlen(aText)))
		{
		CopyText(iText, aText, iLength);
		iText[iLength] = '\0';
		DeleteText(text);
		}
	return *this;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String& String::operator = (const String& aString)
	{
	String string(aString);
	char* text = iText;
	if (CreateText(string.iLength))
		{
		CopyText(iText, string.iText, iLength);
		iText[iLength] = '\0';
		DeleteText(text);
		}
	return *this;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String& String::operator += (const char aChar)
	{
	char* text = iText;
	if (CreateText(iLength + 1))
		{
		CopyText(iText, text, iLength - 1);
		iText[iLength - 1] = aChar;
		iText[iLength] = '\0';
		DeleteText(text);
		}
	return *this;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline String& String::operator += (const String& aString)
	{
	char* text = iText;
	int length = iLength;
	if (CreateText(length + aString.iLength))
		{
		CopyText(iText, text, length);
		CopyText(iText + length, aString.iText, aString.iLength);
		iText[iLength] = '\0';
		DeleteText(text);
		}
	return *this;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline int String::operator == (const String& aString) const
	{
	boolean same = etrue;
	if (iLength != aString.iLength)
		same = efalse;
	else
		{
		for (int i = 0; i < iLength; i++)
			same = same && ((*this)[i] == aString[i]);
		}
	return same;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline char& String::operator [] (const int aNum) const
	{
	return iText[aNum];
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline int String::Length() const
	{
	return iLength;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline const char* String::Text()
	{
	return iText;
	}

/** WARNING:  Functon for internal use ONLY.  Compatibility is not guaranteed in future releases. */
inline void String::CopyText(char* aDest, const char* aSource, int aLength) const
	{
	for (int i = 0; i < aLength; i++)
		aDest[i] = aSource[i];
	}
