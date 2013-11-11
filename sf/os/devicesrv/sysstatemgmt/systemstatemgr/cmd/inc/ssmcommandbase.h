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

#ifndef __SSMCOMMANDBASE_H__
#define __SSMCOMMANDBASE_H__

#include <e32base.h>
#include <ssm/ssmcommand.h>

#define __COMPARE_VERSION(version, expectedVersion) ((version <= expectedVersion) && (version > 0))

class CSsmCommandUtilProvider;

/** Abstract base class for all commands.
@internalComponent
@released
*/
NONSHARABLE_CLASS(CSsmCommandBase) : public CActive, public MSsmCommand
	{
public:
	~CSsmCommandBase();

	void SetUtilProvider(CSsmCommandUtilProvider& aUtilProvider);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TUint16 Priority() const;
	void SetCommandResourceFileNameL(const TDesC& aFileName);
	const TDesC& GetCommandResourceFileName() const;
	TInt GetCommandResourceFileNameLength() const;
#endif
	
	// from MSsmCommand
	TBool IsDeferred() const;
	TCmdErrorSeverity Severity() const;
	TInt CompletionStatus() const;
	void RegisterCompletionObserver(TRequestStatus& aStatus);
	void RegisterCompletionObserverCancel();
	TSsmExecutionBehaviour ExecutionBehaviour() const;
	TBool EvaluateCondition(MSsmConditionalCallback* aCallback, CResourceFile* aResourceFile);
	void SetConditionalInformation(TInt aConditionalInformation);
	TInt ConditionalInformation() const;
	
protected:
	CSsmCommandBase();
	CSsmCommandBase(TCmdErrorSeverity aSeverity);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CSsmCommandBase(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	
	void CompleteRequest(TRequestStatus& aStatus, TInt aReason);
	virtual	void CompleteExecuteRequest(TInt aReason);

protected:
	TRequestStatus* iExecuteRequest;
	TCmdErrorSeverity iSeverity;
	CSsmCommandUtilProvider* iUtilProvider;	// Not owning
	TInt iConditionalResourceId;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	// the priority of the command, indicating the order of execution in the list
	TUint16 iPriority;
	RBuf iCmdResourceFileName;
#endif
	};

#endif // __SSMCOMMANDBASE_H__
