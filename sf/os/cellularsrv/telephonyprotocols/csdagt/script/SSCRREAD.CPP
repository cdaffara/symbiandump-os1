// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NetDial Script reader
// 
//

/**
 @file Scrread.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SSCRREADTraces.h"
#endif

#include "SSCRREAD.H"
#include "ND_STD.H"

// TLinePosition definitions 

TLinePosition::TLinePosition(TInt aLineCount,TInt aLineStart,TInt aLineLength,TInt aOffset)
	: iLineCount(aLineCount), iLineStart(aLineStart), iLineLength(aLineLength), iOffset(aOffset)
/**
Constructor for TLinePosition.

@param aLineCount is line count.
@param aLineStart is line start.
@param aLineLength is line length.
@param aOffset is line offset.
*/
	{}

TLinePosition& TLinePosition::operator=(const TLinePosition& aLinePosition)
/**
TLinePosition =-operator.

@param aLinePosition a reference to line position for comparison.
*/
	{
	iLineCount=aLinePosition.iLineCount;
	iLineStart=aLinePosition.iLineStart;
	iLineLength=aLinePosition.iLineLength;
	iOffset=aLinePosition.iOffset;
	return (*this);
	}

void TLinePosition::Reset()
/**
Reset.
*/
	{
	iLineCount=0;
	iLineStart=0;
	iLineLength=0;
	iOffset=0;
	}

// TScriptStatus definitions 

TScriptStatus::TScriptStatus(TInt& aOffset,TPtrC& aLine,TBool& aSkip,TBool& aSkipModeToggleReq)
	: iOffset(aOffset), iLine(aLine), iSkip(aSkip), iSkipModeToggleReq(aSkipModeToggleReq)
/**
Constructor for TScriptStatus.

@param aOffset a reference to script offset.
@param aLine a reference to script line.
@param aSkip a reference to skip
@param aSkipModeToggleReq a reference to skip mode toggle.
*/
	{}

TScriptStatus::TScriptStatus(const TScriptStatus& aStatus)
	: iOffset(aStatus.iOffset), iLine(aStatus.iLine), iSkip(aStatus.iSkip), iSkipModeToggleReq(aStatus.iSkipModeToggleReq)
/**
Constructor for TScriptStatus.

@param aStatus a reference to script status.
*/
	{}

// CScriptReader (NetDial Script Reader) definitions

CScriptReader* CScriptReader::NewL(TInt aBufferSize)
/**
2 phased constructor for CScriptReader, first phase.

@param aBufferSize is buffer size for script reader.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptReader object.
*/
	{
	CScriptReader* p=new(ELeave) CScriptReader();
	CleanupStack::PushL(p);
	p->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return p;
	}

CScriptReader::CScriptReader() 
	: iScript(NULL,0), iCurrentPosition()
/**
Constructor for CScriptReader, used in the first phase of construction.
*/
	{}

void CScriptReader::ConstructL(TInt aBufferSize)
/**
Instantiate member variables.

@param aBufferSize is buffer size for script reader.
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTREADER_CONSTRUCTL_1,"Script:\tCreating Buffer Of Size %d", aBufferSize);
	iScriptBuffer=HBufC::NewL(aBufferSize);
	TPtr temp=iScriptBuffer->Des();
	iScript.Set(temp);
	iLoggingOn=ETrue;
	}

CScriptReader::~CScriptReader()
/**
Destructor.
Delete members.
*/
	{
	delete iScriptBuffer;
	}

void CScriptReader::SetScript(const TDesC& aScript)
/**
Set script and reset buffers.
@param aScript a reference to used script.
*/
	{
	iScript.Copy(aScript);
	iCurrentPosition.Reset();
	iScriptSet=ETrue;
	iSkip=EFalse;
	}

TBool CScriptReader::IsScriptSet() const
/**
Set script and reset buffers.

@return iScriptSet flag.
*/
	{
	return iScriptSet;
	}	

TScriptStatus CScriptReader::ScriptStatus()
/**
Set script and reset buffers.

@return TScriptStatus.
*/
	{
	return TScriptStatus(iCurrentPosition.iOffset,iCurrentLine,iSkip,iSkipModeToggleReq);
	}

TInt CScriptReader::GetNextLine()
/**
Get next line.

@return current line from GetCurrentLine().
*/
	{
	iCurrentPosition.iLineStart+=iCurrentPosition.iLineLength;		// adjust to the start of the next line
	return GetCurrentLine();
	}
	
TInt CScriptReader::GetCurrentLine()
/**
Get current line.

@return current line.
*/
	{
	TInt activeLen=iScript.Length()-iCurrentPosition.iLineStart;	// length of the script which is still unread
	if (activeLen==0)
		return KErrScriptCompleted;
	
	TPtrC activeDes;
	activeDes.Set(iScript.Right(activeLen));

	TBool isLastLineWithoutReturn=EFalse;
	TInt posCR=activeDes.Locate(KCarriageReturn);
	TInt posLF=activeDes.Locate(KLineFeed);
	if(posCR==KErrNotFound && posLF==KErrNotFound)
		isLastLineWithoutReturn=ETrue;

	if (isLastLineWithoutReturn)
		iCurrentPosition.iLineLength=activeDes.Length();
	else
		{
		if (posCR==KErrNotFound)
			posCR = KMaxTInt;
		if (posLF==KErrNotFound)
			posLF = KMaxTInt;
		TInt pos = Min(posCR,posLF);
		iCurrentPosition.iLineLength=pos+1;
		}

	iCurrentLine.Set(activeDes.Left(iCurrentPosition.iLineLength));		// for script status
	iCurrentPosition.iOffset=0;
	iCurrentPosition.iLineCount++;
	if (iLoggingOn)
		{
		if (iSkip)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTREADER_GETCURRENTLINE_1, "Script Line #%d:\t[Skip] : %S", iCurrentPosition.iLineCount,activeDes);
			}
		else
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTREADER_GETCURRENTLINE_2, "Script Line #%d:\t%S", iCurrentPosition.iLineCount,activeDes);
			}
		}
	return KErrNone;
	}

void CScriptReader::CurrentPos(TLinePosition& aPosition,TInt aOffset)
/**
Get the current position and change the offset to aOffset.

@param aPosition a reference to line position.
@param aOffset is offset.
*/
	{
	aPosition=iCurrentPosition;
	aPosition.iOffset=aOffset;
	}

void CScriptReader::SetCurrentPos(TLinePosition aPosition)
/**
Set current position to aPos and aLine

@param aPosition is line position.
*/
	{
	iCurrentPosition=aPosition;
	}

TInt CScriptReader::Reset()
/**
Reset counters and flags
*/
	{
	iLoggingOn=ETrue;
	iCurrentPosition.Reset();
	TInt ret=GetCurrentLine();
	__ASSERT_DEBUG(!iScriptSet || ret==KErrNone, User::Invariant());
	GetCurrentLine();
	iSkip=EFalse;
	iSkipModeToggleReq=EFalse;
	return ret;
	}
