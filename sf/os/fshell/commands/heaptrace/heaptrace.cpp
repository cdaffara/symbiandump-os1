// heaptrace.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/memoryaccesscmd.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdHeapTrace : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdHeapTrace();
private:
	CCmdHeapTrace();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	enum TCmd
		{
		EEnable,
		EDisable,
		//EDump
		};
	TCmd iCommand;
	TUint iThreadId;
	};

EXE_BOILER_PLATE(CCmdHeapTrace)

CCommandBase* CCmdHeapTrace::NewLC()
	{
	CCmdHeapTrace* self = new(ELeave) CCmdHeapTrace();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdHeapTrace::~CCmdHeapTrace()
	{
	}

CCmdHeapTrace::CCmdHeapTrace()
	{
	}

const TDesC& CCmdHeapTrace::Name() const
	{
	_LIT(KName, "heaptrace");
	return KName;
	}

void CCmdHeapTrace::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("command"));
	aArguments.AppendUintL(iThreadId, _L("thread_id"));
	}

void CCmdHeapTrace::DoRunL()
	{
	LoadMemoryAccessL();
	TInt err;
	switch (iCommand)
		{
		case EEnable:
			err = iMemAccess.EnableHeapTracing(iThreadId, ETrue);
			LeaveIfErr(err, _L("Couldn't enable tracing."));
			break;
		case EDisable:
			err = iMemAccess.EnableHeapTracing(iThreadId, EFalse);
			LeaveIfErr(err, _L("Couldn't disable tracing."));
			break;
		}
	}
