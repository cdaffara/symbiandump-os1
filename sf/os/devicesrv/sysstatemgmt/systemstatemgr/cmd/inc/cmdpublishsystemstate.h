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

#ifndef __CMDPUBLISHSYSTEMSTATE_H__
#define __CMDPUBLISHSYSTEMSTATE_H__
#include <domainmanager.h>
#include <ssm/ssmpublishsystemstateinfo.h>
#include "ssmdeferrablecommand.h"

class TSsmCommandParameters;
class RReadStream;

/**
Built-in command for setting the Domain Manager state
@internalComponent
@released
@see RDmDomainManager
@see TSsmPublishSystemStateInfo
*/
NONSHARABLE_CLASS (CCmdPublishSystemState) : public CSsmDeferrableCommand
	{
public:
	static CCmdPublishSystemState* NewL(RReadStream& aReadStream);
	static CCmdPublishSystemState* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdPublishSystemState* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority);
#endif
	static CCmdPublishSystemState* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdPublishSystemState* NewLC(const CCmdPublishSystemState& aCmdPublishSystemState);
	~CCmdPublishSystemState();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	inline TSsmPublishSystemStateInfo PublishSystemStateInfo() const {return iStateInfo;}

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CCmdPublishSystemState();
	CCmdPublishSystemState(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdPublishSystemState(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority);
#endif
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL();
	void ConstructL(const CCmdPublishSystemState& aCmdPublishSystemState);
	void ValidateL();
	static TInt MaxSupportedVersion();
	TInt ConnectAndRequestTransition();

private:
	RDmDomainManager iDomainManager;		// interface to Domain manager
	TSsmPublishSystemStateInfo iStateInfo;	// state info
	TInt iAttempts;							// count of unsuccessful attempts
	TBool iConnected;						// whether connected to service provider or not
	};

#endif // __CMDPUBLISHSYSTEMSTATE_H__
