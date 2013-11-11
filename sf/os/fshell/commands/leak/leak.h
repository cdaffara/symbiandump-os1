// leak.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>

class RLoggingAllocator;

using namespace IoUtils;

class CCmdLeak : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdLeak();
private:
	CCmdLeak();
	TInt LeakStep(TInt aAmount);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TInt iAmount;
	TBool iVerbose;
	TInt iIncrementAmount;
	TUint iRate;

	TUint iHeapAddr;
	TBool iUseHeap;
	TBool iRetry;
	TBool iUseLoggingAllocator;

	RChunk iChunk;
	RAllocator* iChunkHeap;
	RLoggingAllocator* iLoggingAllocator;
	RAllocator* iAllocatorToUse;

	TInt iCurrentLeak;
	};
