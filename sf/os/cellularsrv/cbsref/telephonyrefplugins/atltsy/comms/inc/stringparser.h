// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CStringParser

#ifndef STRINGPARSER_H_
#define STRINGPARSER_H_

//system include
#include <e32base.h>

//user include
#include "mcommobserver.h"


class CStringParser : public CBase
	{
public:
	static CStringParser* NewL(TInt aBufferSize);
	
	static CStringParser* NewLC(TInt aBufferSize);
	
	virtual ~CStringParser();
	
public:
	void ProcessReadString(const TDesC8& aReadString);
	
	TPtrC8 Buffer() const;
	
	TPtrC8 CurrentLine() const;
	
	void ClearBuffer();
	
	void ClearCurrentLine();
	
	//Must be call this function
	void SetReadLineNotify(MCommReadLineNotifier* aNofity);
	
protected:
	
	CStringParser();
	
	void ConstructL(TInt aBufferSize);
	
	void CreateL(TInt aBufferSize);
	
private:
	TText8 *iBuffer;
	TText8 *iBufferEnd;
	/**
	* The last char added in the current line
	*/
	TText8 *iLastChar;
	
	/**
	* The start of \r\n delimited line
	*/
	TText8 *iLineStart;
	
	/**
	* Means if current char is a delimiter
	*/
	TBool iInDelimiter;
	
	/**
	*
	*/
	MCommReadLineNotifier *iReadLineNotify; //Not owned
	};

#endif /*STRINGPARSER_H_*/
