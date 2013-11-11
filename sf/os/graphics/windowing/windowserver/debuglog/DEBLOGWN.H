// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class declaration for the windows-dependent derived class
// 
//

#ifndef __DEBLOGWN_H__
#define __DEBLOGWN_H__

#include <e32std.h>

#include "DEBUGLOG.H"

class CDebugLogWin: public CDebugLogDevice
	{
public:
	CDebugLogWin();
	virtual ~CDebugLogWin();
	//Pure virtual functions from CDebugLogDevice
	void ConstructL(TBool aIsFirst, TDesC &aParams);
	void WriteToLogL(const TDesC &aDes, const TDesC &aDes2);
	void WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2);
private:
	RThread iThread;
	TBool iThreadCreated;
	TBuf<LogTBufSize> iTextBuf;
	};

TInt logWinMain(TAny *);
const TUint KHeapSize=0x8000;

#endif

