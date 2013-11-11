// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SSMSWPPOLICYSRV_H
#define SSMSWPPOLICYSRV_H

#include <e32base.h>
#include <s32mem.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmswppolicy.h>

_LIT(KSsmSwpPolicySrvName, "!SsmSwpPolicySrv");

class CSsmSwpPolicyStepCompletion;

/**
 The SsmSwpPolicy server
 
@internalComponent
@released
*/
NONSHARABLE_CLASS(CSsmSwpPolicyServer) : public CPolicyServer
	{
public:
	static CSsmSwpPolicyServer* NewLC(const TDesC& aServerName);
	IMPORT_C static TInt StartSsmSwpPolicySrv(const TDesC& aServerName);
	~CSsmSwpPolicyServer();
	
	void RegisterSessionL( TInt& aSessionIndex);
	void DeregisterSession( const TInt& aSessionIndex );
	
	void CallSetDllHandleAndInitializeL(const RMessage2& aMessage, TInt aSessionIndex);
	void CallInitializeCancel(TInt aSessionIndex);
	void CallPrepareCommandListL(const RMessage2& aMessage, TInt aSessionIndex);
	void CallPrepareCommandListCancel(TInt aSessionIndex);
	void CallCommandListL(const RMessage2& aMessage, TInt aSessionIndex);
	void CallHandleCleReturnValueL(const RMessage2& aMessage, TInt aSessionIndex );
	void CallHandleCleReturnValueCancel(TInt aSessionIndex );
	
	// From CServer2
	IMPORT_C CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

	void DeleteSSwpPolicySessionInfo(TInt aSessionIndex);

// This active object is used to call different steps involved in swp transition (viz Initialize/Prepare.. command list)
// Since this server is used by CSsmSwpPolicyFrame, all the relevent checking like whether initalize is called before
// prepare command list/ preparecommandlist is called before CallCommandList is checked, so no additional
// checking is done while calling the swppolicy methods in this class
NONSHARABLE_CLASS(CSsmSwpPolicyStepCompletion) : public CActive
	{
public:
	enum TStep
		{
		EIdle = 0,
		EInitialize,
		EPrepareCommandList,
		ECallCommandList,
		EHandleCleReturnValue,
		};
public:
	static CSsmSwpPolicyStepCompletion* NewL(MSsmSwpPolicy* aSwpPolicy, CSsmSwpPolicyServer& aServer, const TInt aSessionIndex);
	~CSsmSwpPolicyStepCompletion();
	
	void StartInitialize(const RMessagePtr2& aMessagePtr2);
	void StartPrepareCommandList(const TSsmSwp& aSwp, const RMessagePtr2& aMessagePtr2);
	void StartHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, const RMessagePtr2& aMessagePtr2);
	
	TStep CurrentStep() const;
	void SetCurrentStep(TStep aStep);
	
protected:
	void RunL();
	void DoCancel();
	TInt RunError();

private:
	CSsmSwpPolicyStepCompletion(MSsmSwpPolicy* aSwpPolicy, CSsmSwpPolicyServer& aServer, const TInt aSessionIndex);
private:
	TStep iStep;
	RMessagePtr2 iMessagePtr;	// doesn't own
	MSsmSwpPolicy* iSwpPolicy;	// doesn't own
	TInt iSessionIndex;		// doesn't own, storing it to delete the session info once the swp request is completed
	CSsmSwpPolicyServer& iServer;	// doesn't own
	};	//CSsmSwpPolicyStepCompletion

struct SSwpPolicySessionInfo
	{
	TBool iInUse;
	MSsmSwpPolicy* iSwpPolicy;
	CSsmSwpPolicyStepCompletion* iSsmSwpPolicyStepCompletion;
	};	// SSwpPolicySessionInfo

private:	
	void ConstructL(const TDesC& aServerName);
	CSsmSwpPolicyServer();
	static TInt SsmSwpPolicySrvThreadFn(TAny* aAny);

private:
	RArray<SSwpPolicySessionInfo> iSessionInfoArray;
	};

inline CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::TStep CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::CurrentStep() const
	{
	return iStep;
	}

inline void CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::SetCurrentStep(CSsmSwpPolicyServer::CSsmSwpPolicyStepCompletion::TStep aStep)
	{
	iStep = aStep;
	}

#endif //SSMSWPPOLICYSRV_H
