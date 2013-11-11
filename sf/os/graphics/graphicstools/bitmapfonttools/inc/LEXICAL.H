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
* Header LEXICAL.H
*
*/


#ifndef __LEXICAL_H__
#define __LEXICAL_H__

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#pragma warning( disable : 4511 )	// copy constructor could not be generated
#pragma warning( disable : 4512 )	// assignment operator could not be generated
#pragma warning( disable : 4514 )	// unreferenced inline function has been removed
#pragma warning( disable : 4699 )	// Note: Using precompiled header %s
#pragma warning( disable : 4710 )	// function not inlined
#endif

#include "STRNG.H"
#include <stdlib.h>

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#include <iostream.h>
#include <fstream.h>
#else //!__VC32__ || __MSVCDOTNET__
#include <iostream>
#include <fstream>
using namespace std;
#endif //!__VC32__ || __MSVCDOTNET__

#include "GDR.H"
/**
@publishedAll
WARNING:Enum for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
enum LexType
	{
	ELexEOF,		// end of file
	ELexNL,			// newline (newlines, white-space and comments stripped)
	ELexNumber,		// integer (no optional plus or minus)
	ELexIdent,		// identifier beginning with a..z, A..Z, or _ and continuing with 0..9
	ELexString,		// string delimited at start by "
	ELexOperator	// any other single character
	};

class Lexical
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	Lexical();
	Lexical(const Lexical& aLex);
	Lexical& operator = (const Lexical& aLex);
	int CovertStringToHex();
private:
	int HexDigit(char aDigit, int& decimalEquivalent);
public:
	LexType iType;
	int iNumber;	// for ELexNumber
	char iText[MaxLineLen + 1];	// for ELexIdent, ELexString, ELexOperator
friend ostream& operator << (ostream& out, Lexical& aLex);
	};

class LexAnal
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	LexAnal(const char* aFilename);
	Lexical Read();	// read next lexical into iLex
	Lexical ReadNextLine();	// read first lex on next line
	void Report();
	~LexAnal();
public:
	ifstream iFin;
	Lexical iLex;
	int iLineNo;
	char iLine[MaxLineLen + 1];
	char* iLexPtr;	// Position in current lexical
	char* iCurPtr;	// Position of current lexical in line
private:
	void GetNextLex();
	void GetNextLine();
	void PurgeLastCR(char *aLine);
	Lexical ReadEOF();
	Lexical ReadNewLine(); 
	Lexical ReadNumber();
	Lexical ReadIdent();
	Lexical ReadString();
	Lexical ReadOperator();
	String iFilename;
};

#endif
