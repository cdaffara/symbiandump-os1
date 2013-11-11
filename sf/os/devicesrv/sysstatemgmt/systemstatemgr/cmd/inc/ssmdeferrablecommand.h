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

#ifndef __SSMDEFERRABLECOMMAND_H__
#define __SSMDEFERRABLECOMMAND_H__

#include "ssmcommandbase.h"

/** Abstract interface to a built-in command.
	All commands that support MultipleWait should derive from this interface.
@internalComponent
@released
*/
NONSHARABLE_CLASS (CSsmDeferrableCommand) : public CSsmCommandBase
	{
public:
	~CSsmDeferrableCommand();

	// from MSsmCommand
	TBool IsDeferred() const;
	TInt CompletionStatus() const;
	void RegisterCompletionObserver(TRequestStatus& aStatus);
	void RegisterCompletionObserverCancel();
	TBool EvaluateCondition(MSsmConditionalCallback* aCallback, CResourceFile* aResourceFile);

	TSsmExecutionBehaviour ExecutionBehaviour() const;
protected:
	CSsmDeferrableCommand();
	CSsmDeferrableCommand(TCmdErrorSeverity aSeverity);
	CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUint16 aPriority);
	CSsmDeferrableCommand(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
#endif
	void CompleteDeferredExecuteRequest(TInt aReason);
	virtual	void ValidateL();

protected:
	TInt iCompletionStatus;					// contains the ongoing status of the deferred command
	TRequestStatus* iCompletionObserver; 	// Notifies command completion if registered through RegisterCompletionObserver().
	TSsmExecutionBehaviour iExecutionBehaviour;
	};

#endif // __SSMDEFERRABLECOMMAND_H__
