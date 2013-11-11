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
//

#ifndef __SSMCOMMANDLISTIMPL_H__
#define __SSMCOMMANDLISTIMPL_H__

#include <e32base.h>

#include "ssmcommandlistinterface.h"
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmconditionalcallback.h>

class RReadStream;
class RWriteStream;
class CSsmDeferredDeleter;
class CSsmCommandUtilProvider;

/**
@internalComponent
@released
*/
NONSHARABLE_CLASS(CSsmCommandListImpl) : public CBase, public MSsmCommandList
	{
public:
	static CSsmCommandListImpl* NewL();
	static CSsmCommandListImpl* NewL(const CSsmCommandListImpl* aSsmCommandListImpl, CSsmCommandUtilProvider* aUtilProvider);
	~CSsmCommandListImpl();

	void InternalizeL(RReadStream& aReadStream, CSsmCommandUtilProvider* aUtilProvider);
	void ExternalizeL(RWriteStream& aWriteStream) const;

	void Execute(const TInt aIndex, TRequestStatus& aStatus);
	void ExecuteCancel(const TInt aIndex);
	TInt Count() const;
	const MSsmCommand* operator[](TInt aIndex) const;
	
	//from MSsmCommandList
	void AppendL(MSsmCommand* aCmd);
	void SetDelayBetweenCommands(TInt aDelayBetweenCommands);
	void SetResourceFileNameL(const TDesC& aFileName);
	void SetConditionalCallback(MSsmConditionalCallback& aCallBack);
	MSsmConditionalCallback& ConditionalCallback() const;
	
	TInt DelayBetweenCommands() const;
	TArray<MSsmCommand*> PendingDeferred() const;
	void DeferredDelete();
	TInt PendingCommandIndex();
	TCmdErrorSeverity Severity(TInt aCommandIndex) const;
	void GetDataToValidateCommandlist(TInt& aPublishSysStateCmdCount, TInt& aPublishSwpCmdCount)  const;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static TInt ComparePriority(const MSsmCommand& aFirst, const MSsmCommand& aSecond);
#endif
private:
	CSsmCommandListImpl();
	void ConstructL();
	void ConstructL(const CSsmCommandListImpl* aSsmCommandListImpl, CSsmCommandUtilProvider* aUtilProvider);
	
	void InitialiseResourceFileL();
	
private:
	RPointerArray<MSsmCommand> iCmdList;
	RPointerArray<MSsmCommand> iDeferredList;
	TInt iDelayBetweenCommands;
	CSsmDeferredDeleter* iDeferredDeleter;
	RBuf iResourceFileName;
	CResourceFile* iResourceFile;
	MSsmConditionalCallback* iConditionalCallback;
	TInt iPublishSysStateCmdCount;
	TInt iPublishSwpCmdCount;
	TBool iInValidDeferredWaitCmd;
	};

#endif // __SSMCOMMANDLISTIMPL_H__
