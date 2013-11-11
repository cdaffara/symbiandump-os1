/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Script Reader and Executor Header
* 
*
*/



/**
 @file Sscrread.h
*/

#ifndef __SSCRREAD_H__
#define __SSCRREAD_H__

#include <e32base.h>

struct TLinePosition
	{
public:
	TLinePosition(TInt aLineCount =0,TInt aLineStart =0,TInt aLineLength =0,TInt aOffset =0);
	TLinePosition& operator=(const TLinePosition& aLinePosition);
	void Reset();
public:
	TInt iLineCount;		///< number of line
	TInt iLineStart;		///< character number of line start
	TInt iLineLength;		///< length of line
	TInt iOffset;			///< offset in line
	};


class TScriptStatus
/**
@internalComponent
*/
	{
public:
	TScriptStatus(TInt& aOffset,TPtrC& aLine,TBool& aSkip,TBool& aSkipModeToggleReq);
	TScriptStatus(const TScriptStatus& aStatus);
public:
	TInt& iOffset;
	TPtrC& iLine;
	TBool& iSkip;
	TBool& iSkipModeToggleReq;
	};


class CScriptReader : public CBase
/**
One of these per CScriptExecutor object.
Gets chunk of the script from the database and reads script line by line.

@internalComponent
*/
	{
public:
	static CScriptReader* NewL(TInt aBufferSize);
	~CScriptReader();
	void SetScript(const TDesC& aScript);
	TBool IsScriptSet() const;
	TScriptStatus ScriptStatus();
	TInt GetNextLine();
	void CurrentPos(TLinePosition& aPosition,TInt aOffset);
	void SetCurrentPos(TLinePosition aPosition);
	TInt Reset();
	inline void SetLoggingOff();
	inline void SetLoggingOn();
private:
	CScriptReader();
	void ConstructL(TInt aBufferSize);
	TInt GetCurrentLine();
private:
	HBufC* iScriptBuffer;
	TPtr iScript;
	TLinePosition iCurrentPosition;
	TBool iLoggingOn;
	TBool iScriptSet;
// These for script status only
	TPtrC iCurrentLine;
	TBool iSkip;
	TBool iSkipModeToggleReq;
	};

#include "SSCRREAD.INL"

#endif
