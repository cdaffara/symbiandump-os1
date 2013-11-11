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
// CStringParser.
// 

//user include
#include "stringparser.h"
#include "mslogger.h"

CStringParser* CStringParser::NewL(TInt aBufferSize)
	{
	LOGTEXT(_L8("[Ltsy] Starting CStringParser::NewL()"));
	
	CStringParser* self = CStringParser::NewLC(aBufferSize);
	CleanupStack::Pop(self);
	return self;
	}

CStringParser* CStringParser::NewLC(TInt aBufferSize)
	{
	LOGTEXT(_L8("[Ltsy] Starting CStringParser::NewLC()"));
	
	CStringParser* self = new (ELeave) CStringParser;
	CleanupStack::PushL(self);
	self->ConstructL(aBufferSize);
	return self;
	}

CStringParser::~CStringParser()
	{
	User::Free(iBuffer);
	}

CStringParser::CStringParser()
	{
	}

void CStringParser::ConstructL(TInt aBufferSize)
	{
	LOGTEXT(_L8("[Ltsy] Starting CStringParser::ConstructL()"));
	CreateL(aBufferSize);
	}

TPtrC8 CStringParser::Buffer() const
	{
	return TPtrC8(iBuffer, iLastChar-iBuffer);
	}

TPtrC8 CStringParser::CurrentLine() const
	{
	TInt len=iLastChar-iLineStart;
	if (len>0 && iInDelimiter)
		len-=1;
	return TPtrC8(iLineStart, len);	
	}

void CStringParser::ClearBuffer()
	{
	iLastChar = iBuffer;
	iLineStart = iBuffer;
	iInDelimiter = ETrue;	
	}

//
// Remove current line, which is always at the end of the buffer
//
void CStringParser::ClearCurrentLine()
	{
	iLastChar = iLineStart;
	iInDelimiter = ETrue;	
	}

void CStringParser::CreateL(TInt aBufSize)
	{
	iBuffer = (TText8*)User::AllocL(aBufSize);
	iBufferEnd = (iBuffer+aBufSize)-1;
	ClearBuffer();
	}


void CStringParser::SetReadLineNotify(MCommReadLineNotifier* aNofity)
	{
	iReadLineNotify = aNofity;
	}


void CStringParser::ProcessReadString(const TDesC8& aReadString)
	{
	LOGTEXT(_L8("[Ltsy] Starting CStringParse::ProcessReadString()"));
	LOGTEXT2(_L8("[Ltsy] Read String = %S"), &aReadString);
	for (TInt nBufferOffset = 0; nBufferOffset < aReadString.Length(); nBufferOffset++)
		{
		TText8 tChar = aReadString[nBufferOffset];
		
		// we have already found a char which is a delimiter of the Line
        // so would dismiss any of delimiter following this one already found		
		if (iInDelimiter && (tChar == '\r' || tChar == '\n'))
			{
			continue;
			}
		
		// move on lastchar
		*iLastChar++ = tChar;
		// Diacarding characters if our buffer was overflowed
		if (iLastChar >= iBufferEnd)
			{
			iLastChar = iBufferEnd;
			}
        // we anyway to find out the delimiter and notify the obser the line was found 		
		if (tChar == '\r' || tChar == '\n')
			{
			iInDelimiter = ETrue;
			TPtrC8 tLine(CurrentLine());
			LOGTEXT2(_L8("[Ltsy] Line Content = %S"), &tLine);
			TRAPD(err, iReadLineNotify->NotifyOneLineFoundL(KErrNone, tLine));
			if (err != KErrNone)
				{
				ClearCurrentLine();
				}
			}
		else
			{
			if(iInDelimiter) // the last char is a delimiter
			    {
				iInDelimiter=EFalse;
			    iLineStart=iLastChar-1;	
			    if(tChar == '>')
			    	{
			    	TPtrC8 tLine(CurrentLine());
			    	TRAPD(err, iReadLineNotify->NotifyOneLineFoundL(KErrNone, tLine));
					if (err != KErrNone)
						{
						ClearCurrentLine();
						}			    	
			    	}
				}   
			}
		}
	LOGTEXT(_L8("[Ltsy] End CStringParse::ProcessReadString()"));
	}
//End of file
