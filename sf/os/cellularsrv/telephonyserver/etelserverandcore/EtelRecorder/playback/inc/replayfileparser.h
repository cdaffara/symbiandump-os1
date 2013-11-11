// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions of file parsing related classes.
// 
//

/**
 @file
*/

#ifndef __REPLAYFILEPARSER_H__
#define __REPLAYFILEPARSER_H__

#include <e32base.h>

class CRecordMessage;

class CReplayFileParser : public CBase
	{
public:


public:
	static CReplayFileParser* NewLC(CConsoleBase* aConsole, const TDesC& aName);
	virtual ~CReplayFileParser();
	void CheckHeaderValidity();
	CRecordMessage* GetNextMessageL();
	
protected:
	
	CReplayFileParser(CConsoleBase* aConsole);
	void ConstructL(const TDesC& aName);
	void ForwardCursor(TInt aNoBytesToForward);
	
private:
	CConsoleBase* iConsole;
	RBuf8 iFileInMemory;
	TPtrC8 iCursor;
	TBool iNeedByteReversing;
	};


#endif
