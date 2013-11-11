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
* Header LEXICAL.CPP
*
*/


#include "LEXICAL.H"

Lexical::Lexical()
 :	iType(ELexNL), iNumber(0)
	{
	iText[0] = '\0';
	}

Lexical::Lexical(const Lexical& aLex)
	{
	iType = aLex.iType;
	iNumber = aLex.iNumber;
	strcpy(iText, aLex.iText);
	}

Lexical& Lexical::operator = (const Lexical& aLex)
	{
	iType = aLex.iType;
	iNumber = aLex.iNumber;
	strcpy(iText, aLex.iText);
	return *this;
	}

int Lexical::CovertStringToHex()
	{
	char* curPtr = iText; // Position of current lexical in line
	int hexDigit;
	int number = 0;

	while (HexDigit(*curPtr, hexDigit))
		{
		number = (16 * number) + hexDigit;
		curPtr++;
		}
	return number;
	}

int Lexical::HexDigit(char aDigit, int& decimalEquivalent)
	{
	boolean validDigit = efalse;
	if ((aDigit >= '0') && (aDigit <= '9'))
		{
		decimalEquivalent = (aDigit - '0');
		validDigit = etrue;
		}
	else if ((aDigit >= 'a') && (aDigit <= 'f'))
		{
		decimalEquivalent = 10 + (aDigit - 'a');
		validDigit = etrue;
		}
	else if ((aDigit >= 'A') && (aDigit <= 'F'))
		{
		decimalEquivalent = 10 + (aDigit - 'A');
		validDigit = etrue;
		}
	return validDigit;
	}

ostream& operator << (ostream& out, const Lexical& aLex)
	{
	switch (aLex.iType)
		{
		case ELexEOF:
			{
			out << "EOF";
			break;
			}
		case ELexNL:
			{
			out << "NL";
			break;
			}
		case ELexNumber:
			{
			out << aLex.iNumber;
			break;
			}
		case ELexOperator:
			{
			out << aLex.iText[0];
			break;
			}
		default:
			{
			out << aLex.iText;
			}
		}	 
	return out;
	}

LexAnal::LexAnal(const char* aFilename)
 :	iFilename(aFilename)
	{
	iFin.open(aFilename);
	iLex.iType = ELexNL;
	iLineNo = 0;
	}

Lexical LexAnal::Read() // read next lexical into iLex
	{
	if (iLex.iType == ELexNL)
		{
		do
			{
			GetNextLex();
			}
		while (iLex.iType == ELexNL);
		}
	else
		GetNextLex();
	return iLex;
	}

Lexical LexAnal::ReadNextLine() // read first lex on next line
	{
	GetNextLine();
	return iLex;
	}

void LexAnal::Report()
	{
	cerr << iFilename.Text() << '(' << iLineNo << "): \n";
	cerr << iLine << '\n';
	for	(char* p = iLine; p < iLexPtr; p++)
		cerr << ' ';
	cerr << "^\n";
	}

LexAnal::~LexAnal()
	{
	iFin.close();
	}

void LexAnal::GetNextLex()
	{
	char ch;
	if (iLex.iType == ELexNL)
		{
		iFin.getline(iLine, MaxLineLen);
		// Remove any CR character that appear at the end when
		// reading a dos file on unix.
		PurgeLastCR(iLine);
		iCurPtr = iLine;
		iLineNo++;
		}

	while ((*iCurPtr == ' ') || (*iCurPtr == '\t'))
		iCurPtr++;
	ch = *iCurPtr;
	iLexPtr = iCurPtr;

	if ((ch == '\0') && (iFin.eof()))	// finds lexical type
		iLex = ReadEOF();
	else if ((ch == '\0') || (ch == '!'))	// ! is a comment
		iLex = ReadNewLine();
	else if ((ch == '-') || ((ch >= '0') && (ch <= '9')))
		iLex = ReadNumber();
	else if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_'))
		iLex = ReadIdent();
	else if	(ch == '"')
		iLex = ReadString();
	else
		iLex = ReadOperator();	
	}

void LexAnal::GetNextLine()
	{
	iFin.getline(iLine, MaxLineLen);
	// Remove any CR character that appear at the end when
	// reading a dos file on unix.
	PurgeLastCR(iLine);
	iCurPtr = iLine;
	iLineNo++;

	char ch;
	while ((*iCurPtr == ' ') || (*iCurPtr == '\t'))
		iCurPtr++;
	ch = *iCurPtr;
	iLexPtr = iCurPtr;

	if ((ch == '\0') && (iFin.eof()))	// finds lexical type
		iLex = ReadEOF();
	else if ((ch == '\0') || (ch == '!'))
		iLex = ReadNewLine();
	else if ((ch == '-') || ((ch >= '0') && (ch <= '9')))
		iLex=ReadNumber();
	else if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_'))
		iLex = ReadIdent();
	else if	(ch == '"')
		iLex = ReadString();
	else
		iLex = ReadOperator();	
	}

void LexAnal::PurgeLastCR(char *aLine)
	{
	int len = strlen(aLine) - 1;
	if (len >= 0 && aLine[len] == '\r')
		{
		aLine[len] = '\0';
		}
	}

Lexical LexAnal::ReadEOF()
	{
	Lexical lex;
	lex.iType = ELexEOF;
	return lex;
	}

Lexical LexAnal::ReadNewLine()
	{
	Lexical lex;
	lex.iType = ELexNL;
	while (*iCurPtr != '\0')
		iCurPtr++;
	return lex;
	}

Lexical LexAnal::ReadNumber()
	{
	Lexical lex;
	char ch;
	boolean negative = efalse;
	lex.iType = ELexNumber;
	if (*iCurPtr == '-')
		{
		negative = etrue;
		iCurPtr++;
		}
	ch = *iCurPtr;
	while ((ch >= '0') && (ch <= '9'))
		{
		if (negative)
			lex.iNumber = (10 * lex.iNumber) - (*iCurPtr - '0');
		else
			lex.iNumber=(10 * lex.iNumber) + (*iCurPtr - '0');
		iCurPtr++;
		ch = *iCurPtr;
		}
	return lex;
	}


Lexical LexAnal::ReadIdent()
	{
	Lexical lex;
	char ch;
	lex.iType = ELexIdent;
	do
		{
		iCurPtr++;
		ch = *iCurPtr;
		}
	while (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_') || ((ch >= '0') && (ch <= '9')));
	strncpy(lex.iText, iLexPtr, iCurPtr - iLexPtr);
	lex.iText[iCurPtr - iLexPtr] = '\0';
	return lex;
	}

Lexical LexAnal::ReadString()
	{
	Lexical lex;
	char ch;
	lex.iType = ELexString;
	iCurPtr++;
	ch = *iCurPtr;
	while ((ch != '"') && (*iCurPtr != '\0'))
		{
		iCurPtr++;
		ch = *iCurPtr;
		}
	strncpy(lex.iText, iLexPtr + 1, iCurPtr - (iLexPtr + 1));
	lex.iText[iCurPtr - (iLexPtr + 1)] = '\0';
	if (ch == '"')
		iCurPtr++;	// finds position after last double quotes 
	else
		{
		cerr << "Warning: missing quotes\n";
		Report();
		}
	return lex;
	}

Lexical LexAnal::ReadOperator()
	{
	Lexical lex;
	lex.iType = ELexOperator;
	lex.iText[0] = *iCurPtr;
	iCurPtr++;
	return lex;
	}
