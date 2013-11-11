// getstack.cpp
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

#include "getstack.h"
#include <fshell/memoryaccess.h>


CCommandBase* CCmdGetStack::NewLC()
	{
	CCmdGetStack* self = new(ELeave) CCmdGetStack();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGetStack::~CCmdGetStack()
	{
	}

CCmdGetStack::CCmdGetStack()
	{
	}

const TDesC& CCmdGetStack::Name() const
	{
	_LIT(KName, "getstack");
	return KName;
	}

void CCmdGetStack::DoRunL()
	{
	RMemoryAccess memoryAccess;
	TInt err = RMemoryAccess::LoadDriver();
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{
		User::LeaveIfError(memoryAccess.Open());
		CleanupClosePushL(memoryAccess);
		}
	else
		{
		User::LeaveIfError(err);
		}
	RThread thread;
	User::LeaveIfError(thread.Open(iThreadId));
	CleanupClosePushL(thread);

	RFile file;
	User::LeaveIfError(file.Create(FsL(), iFileName, EFileWrite | EFileStream));
	CleanupClosePushL(file);

	TThreadStackInfo stackInfo;
	User::LeaveIfError(thread.StackInfo(stackInfo));
	const TInt stackSize = stackInfo.iBase - stackInfo.iLimit;

	const TInt KBufSize = 4096; // The largest amount RMemoryAccess allows us to copy in one go.
	HBufC8* stackBuf = HBufC8::NewLC(KBufSize);
	TPtr8 stackBufPtr(stackBuf->Des());

	TThreadMemoryAccessParamsBuf accessParamsBuf;
	TThreadMemoryAccessParams& accessParams = accessParamsBuf();
	accessParams.iId = (TInt)thread.Id().Id();

	TInt numBytesRead = 0;
	while (numBytesRead < stackSize)
		{
		accessParams.iAddr = (TUint8*)stackInfo.iLimit + numBytesRead;
		accessParams.iSize = Min(KBufSize, stackSize - numBytesRead);
		stackBufPtr.Zero();
		User::LeaveIfError(memoryAccess.GetThreadMem(accessParamsBuf, stackBufPtr));
		User::LeaveIfError(file.Write(*stackBuf));
		numBytesRead += stackBuf->Length();
		}

	CleanupStack::PopAndDestroy(4, &memoryAccess);
	}

void CCmdGetStack::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);

	_LIT(KArgThreadId, "thread_id");
	aArguments.AppendUintL(iThreadId, KArgThreadId);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdGetStack)
#endif

