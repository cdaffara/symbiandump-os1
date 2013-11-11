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
* Header READER.H
*
*/


#ifndef __READER_H__
#define __READER_H__

#include "LEXICAL.H"
#include "RECORD.H"

class Reader
/**
@internalComponent
*/
	{
public:
	Reader();
	virtual boolean Read(const String& aFilename) = 0;
	~Reader();
protected:
	boolean Open(const String& aFilename);
	boolean _EOF();
	boolean NewLine();
	boolean Number(int& aNumber);
	boolean Ident();
	boolean IdentComp(const String& aIdent);
	boolean IdentCopy(String& aIdent);
	boolean StringCopy(String& aString);
	boolean Operator(char& aCh);
	IMPORT_C void Error(const String& aString);
protected:
	LexAnal *iLexAnal;
	Lexical *iLex;
	};

#endif
