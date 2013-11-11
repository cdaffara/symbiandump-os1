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

#ifndef __SSMCOMMAND_H__
#define __SSMCOMMAND_H__

#include <ssm/ssmcmd.hrh>
class RWriteStream;
class MSsmConditionalCallback;
class CResourceFile;

const TInt KSsmCommandPending = KRequestPending;

/** Interface to a command.
@publishedPartner
@released
*/
class MSsmCommand
	{
public:
	virtual void Execute(TRequestStatus& aStatus) = 0;
	virtual void Release() = 0;		
	virtual void ExecuteCancel() = 0;
	virtual TSsmCommandType Type() const = 0;
	virtual void ExternalizeL(RWriteStream& aWriteStream) const = 0;
	virtual TBool IsDeferred() const = 0;
	virtual TCmdErrorSeverity Severity() const = 0;
	virtual TInt CompletionStatus() const = 0;
	virtual	void RegisterCompletionObserver(TRequestStatus& aStatus) = 0;
	virtual	void RegisterCompletionObserverCancel() = 0;
	virtual TSsmExecutionBehaviour ExecutionBehaviour() const = 0;
	virtual void SetConditionalInformation(TInt aConditionalInformation) = 0;
	virtual TInt ConditionalInformation() const = 0;
	virtual TBool EvaluateCondition(MSsmConditionalCallback* aCallback, CResourceFile* aResourceFile) = 0;
	};

#endif // __SSMCOMMAND_H__
