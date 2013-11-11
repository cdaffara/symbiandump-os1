// console.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <e32base.h>
#include <fshell/iocli.h>


class MConsoleObserver
	{
public:
	virtual void CoHandleKey(TUint aKeyCode, TUint aModifiers) = 0;
	virtual void CoHandleError(TInt aError) = 0;
	};


class CConsoleReader : public CActive
	{
public:
	static CConsoleReader* NewL(RIoConsoleReadHandle& aStdin, MConsoleObserver& aObserver);
	~CConsoleReader();
private:
	CConsoleReader(RIoConsoleReadHandle& aStdin, MConsoleObserver& aObserver);
	void Queue();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	RIoConsoleReadHandle& iStdin;
	MConsoleObserver& iObserver;
	TBool iRawRead;
	TBuf<1> iRawReadBuf;
	};


#endif // __CONSOLE_H__
