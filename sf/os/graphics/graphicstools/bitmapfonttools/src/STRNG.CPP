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
* Header STRNG.CPP
*
*/


#include "STRNG.H"

extern bool OutputUnicode;

ostream& operator << (ostream& out, const String& aString)
	{
	for (int i = 0; i < aString.iLength; i++)
		out << aString.iText[i];
	out << '\n';
	return out;
	}

EXPORT_C void String::Externalize(ostream& out)
	{
	if (OutputUnicode)
		{
		// Convert the string to Unicode, allowing #NNNN (each N is a hex digit)
		// to represent an arbitrary Unicode character. Other values are just
		// extended, so don't use codepage 1252 values in the range 128..159.
		unsigned short* buffer = new unsigned short[iLength];
		int i = 0;
		int j = 0;
		while (i < iLength)
			{
			if (iText[i] == '#')
				{
				i++;
				char hex[5];
				hex[0] = iText[i++];
				hex[1] = iText[i++];
				hex[2] = iText[i++];
				hex[3] = iText[i++];
				hex[4] = 0;
				buffer[j++] = (unsigned short)strtoul(hex, NULL, 16);
				}
			else
				{
				buffer[j] = iText[i];
				buffer[j] &= 0xFF;
				i++;
				j++;
				}
			}
		int unicode_characters = j;
		int32 length = (unicode_characters << 1);	// 16-bit data
		if (length < 0x80)
			{
			unsigned char len = (unsigned char)(length << 1);
			out.write((char*)&len, sizeof(len));
			}
		else if (length < 0x4000)
			{
			uint16 len = (uint16)((length << 2) + 1);
			out.write((char*)&len, sizeof(len));
			}
		else
			{
			// assert len<0x20000000 ?
			uint32 len = (uint32)((length << 3) + 3);
			out.write((char*)&len, sizeof(len));
			}
		// Output Unicode characters using the Standard Compression Scheme for Unicode.
		// To save the bother of doing this properly, use a degenerate form whereby each
		// Unicode character is output as itself. 0x0F selects Unicode mode and 0xF0 quotes
		// characters that would conflict with other tags.
		out << (unsigned char)0x0F;

		for (i = 0; i < unicode_characters; i++)
			{
			unsigned char hi = (unsigned char)(buffer[i] >> 8);
			unsigned char lo = (unsigned char)buffer[i];
			if ((hi >= 0xe0) && (hi <= 0xf2))
				out << 0xf0;
			out << hi;
			out << lo;
			}

		delete [] buffer;
		}
	else
		{
		int32 length = (iLength << 1) + 1;	// 8-bit data
		if (length < 0x80)
			{
			unsigned char len = (unsigned char)(length << 1);
			out.write((char*)&len, sizeof(len));
			}
		else if (length < 0x4000)
			{
			uint16 len = (uint16)((length << 2) + 1);
			out.write((char*)&len, sizeof(len));
			}
		else
			{
			// assert len<0x20000000 ?
			uint32 len = (uint32)((length << 3) + 3);
			out.write((char*)&len, sizeof(len));
			}
		out.write(iText, iLength);
		}
	}

EXPORT_C int String::CreateText(const int aLength)
	{
	if (aLength != iLength)
		{
		char* text = new char[aLength + 1];
		if (text)
			{
			iLength = aLength;
			iText = text;
			}
		else
			{
			iLength = 0;
			delete [] iText;
			iText = NULL;
			}
		}
	return iLength;
	}

EXPORT_C void String::DeleteText(char* aText) const
	{
	if (aText != iText)
		delete [] aText;
	}

EXPORT_C String::~String()
	{
	delete [] iText;
	}
