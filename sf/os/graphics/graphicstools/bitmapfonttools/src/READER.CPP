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
* Header READER.CPP
*
*/


#include "READER.H"

Reader::Reader()
	{
	iLexAnal = NULL;
	}

boolean Reader::Open(const String& aFilename)
	{
	boolean state;
	String string = aFilename;
	if (iLexAnal != NULL)
		delete iLexAnal;
	iLexAnal = new LexAnal(string.Text());

	if (iLexAnal)
		{
		state = etrue;
		iLex =& (iLexAnal->iLex);
		iLexAnal->Read();	// reads first lexical
		}
	else
		state = efalse;

	return state;
	}

Reader::~Reader()
	{
	if (iLexAnal != NULL)
		delete iLexAnal;
	}

boolean Reader::_EOF()
	{
	return ((iLex->iType) == ELexEOF);
	}

boolean Reader::NewLine()
	{
	boolean state;
	if (iLex->iType == ELexNL)
		{
		iLexAnal->Read();
		state = etrue;
		}
	else
		{
		Error(String("Newline expected"));
		state = efalse;
		}
	return state;
	}

boolean Reader::Number(int& aNumber)
	{
	boolean state;
	if (iLex->iType == ELexNumber)
		{
		aNumber = iLex->iNumber;
		iLexAnal->Read();
		state = etrue;
		}
	else
		{
		Error(String("Number expected"));
		state = efalse;
		}
	return state;
	}

boolean Reader::IdentComp(const String& aIdent)
	{
	boolean state;
	if (iLex->iType == ELexIdent)
		{
		if (aIdent == iLex->iText)
			{
			iLexAnal->Read();
			state = etrue;
			}
		else
			state = efalse;
		}
	else
		{
		state = efalse;
		}
	return state;
	}

boolean Reader::IdentCopy(String& aIdent)
	{
	boolean state;
	if (iLex->iType == ELexIdent)
		{
		aIdent = iLex->iText;
		iLexAnal->Read();
		state = etrue;
		}
	else
		{
		Error(String("Identifier expected"));
		state = efalse;
		}
	return state;
	}

boolean Reader::StringCopy(String& aString)
	{
	boolean state;
	if (iLex->iType == ELexString)
		{
		aString = iLex->iText;
		iLexAnal->Read();
		state = etrue;
		}
	else
		{
		Error(String("String expected"));
		state = efalse;
		}
	return state;
	}

boolean Reader::Operator(char& aCh)
	{
	boolean state;
	if (iLex->iType == ELexOperator)
		{
		aCh = iLex->iText[0];
		iLexAnal->Read();
		state = etrue;
		}
	else
		{
		Error(String("Operator expected"));
		state = efalse;
		}
	return state;
	}

EXPORT_C void Reader::Error(const String& aString)
	{
	cerr << "Error: " << aString;
	iLexAnal->Report();
	while ((iLex->iType != ELexNL) && (iLex->iType != ELexEOF))
		iLexAnal->Read();
	}
