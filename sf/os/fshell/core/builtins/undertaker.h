// undertaker.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef UNDERTAKER_H
#define UNDERTAKER_H

#include <fshell/ioutils.h>
#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

class CCmdUndertaker : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdUndertaker();
private:
	CCmdUndertaker();
	void ConstructL();
	TInt Queue();
	void ProcessHandle(TInt aDeadThreadHandle);
	static TInt WorkerThreadFn(TAny* aSelf);
	TInt DoWork();
	void CloseProcessOwnedHandles();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From CActive
	void RunL();
	void DoCancel();

private:
	TInt iDeadThreadHandle;
	RUndertaker iUndertaker;
	TBool iAll;
	TBool iLeakThreads;
	TBool iParanoid;
	// The following are only used if in paranoid mode
	RFastLock iLock;
	RArray<TInt> iHandles;
	RThread iWorkerThread;
	RThread iMainThread;
	TRequestStatus iMainThreadStat;
	};

#endif
