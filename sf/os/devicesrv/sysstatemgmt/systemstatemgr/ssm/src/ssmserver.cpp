// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmuiproviderdll.h>

#include "ssmserverpanic.h"
#include "ssmserver.h"
#include "ssmsession.h"
#include "ssmclisrv.h"
#include "clesessionproxy.h"
#include "ssmstatepolicyresolver.h"
#include "ssmstatetransitionengine.h"
#include "ssmstatepolicyresolverproxy.h"
#include "ssmstatetransitionrequest.h"
#include "ssmstatepolicyframe.h"
#include "ssmswppolicyresolver.h"
#include "ssmswprequesthandler.h"
#include "ssmdebug.h"

// ------------------- Policy Server Security Setup ----------------------

const TUint  KRangeCount = 3;
const TInt   KRanges[KRangeCount] =
	{
	0,						// op-code 0 not used
	KSsmFirstUsedOpCode,	//range: KFirstUsedSsmOpCode...(EEndOfSsmOpCodes-1) inclusive
	EEndOfSsmOpCodes
	};

/**
 Specifies the appropriate action for each range in KRanges.
 The nth element of KElementsIndex specifies the appropriate action for the nth range in KRanges.
 */
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,	//All request will be accepted and passed on to policy DLL for security checks
	CPolicyServer::ENotSupported,
	};

/**
 Setup a security policy that always allows connection requests for all requests.
 */
const CPolicyServer::TPolicy KSsmServerPolicy =
	{
	CPolicyServer::EAlwaysPass,	// Always allow connection requests
	KRangeCount,
	KRanges,
	KElementsIndex,
	NULL
	};

// ------------------- SsmServer Implementation ----------------------

/**
 EPriorityHigh to make sure the server is always scheduled before other AO such
 as CSsmStateTransitionEngine.
 */
CSsmServer::CSsmServer() : CPolicyServer(EPriorityHigh, KSsmServerPolicy)
	{
	}

/**
 */
CSsmServer::~CSsmServer()
	{
	const TInt ignore = User::SetCritical(User::ENotCritical);

	delete iStateTransitionEngine;
	delete iSwpRequestHandler;
	delete iSwpResolver;
	if(NULL != iSwpCleSession)
		{
		iSwpCleSession->ReleaseCle();
		}
	
	if(iSsmUiSpecific)
        {
        iSsmUiSpecific->Release();
        }
	} //lint !e529 not subsequently referenced

/**
 */
CSsmServer* CSsmServer::NewLC()
	{
	return CSsmServer::NewLC(ESsmStartup, EDoValidateCommandLists);
	}

/**
 */
CSsmServer* CSsmServer::NewLC(TUint16 aInitialState)
	{
	return CSsmServer::NewLC(aInitialState, EDoValidateCommandLists);
	}

/**
 */
CSsmServer* CSsmServer::NewLC(TUint16 aInitialState, TValidation aSetting)
	{
	CSsmServer* server = new(ELeave) CSsmServer();
	CleanupStack::PushL(server);
	server->ConstructL(aInitialState, aSetting);
	server->StartL(KSsmServerName);
	return server;
	}

#ifdef TEST_SSM_SERVER
/**
 Factory method used by automated tests to avoid clash with production server name.
 */
CSsmServer* CSsmServer::NewLC(const TDesC& aServerName, TUint16 aInitialState)
	{
	CSsmServer* server = new(ELeave) CSsmServer();
	CleanupStack::PushL(server);
	server->ConstructL(aInitialState, ENoCommandListValidation);
	server->StartL(aServerName);
	return server;
	}
#endif //TEST_SSM_SERVER
/**
 */
void CSsmServer::ConstructL(TUint16 aInitialState, TValidation aSetting)
	{
	__ASSERT_ALWAYS( KErrNone == User::SetCritical(User::ESystemCritical), PanicNow(KPanicSysStateMgr,ESsmServerError1));
	__ASSERT_ALWAYS( KErrNone == User::RenameThread(KSsmServerName), PanicNow(KPanicSysStateMgr,ESsmServerError2));

	iSsmUiSpecific = CSsmUiSpecific::InstanceL();
	//Reserve memory for critical operation especially when phone memory will be full
    TInt err = iSsmUiSpecific->ReservePhoneMemorySpace();
	DEBUGPRINT2(_L("CSsmUiSpecific::ReservePhoneMemorySpace returned with = %d"), err);
	// --- Instantiate the "System State" handling classes ---
	// Create a state policy resolver and load the initial policy
	CSsmStatePolicyResolver* stateResolver = CSsmStatePolicyResolver::NewLC();
	const TSsmState initalState = TSsmState(aInitialState, KSsmAnySubState);
	stateResolver->GetStatePolicyL(initalState);
	CCleSessionProxy* stateCleSession = CCleSessionProxy::NewL();
	CleanupStack::PushL(stateCleSession);
	// Create the state transition engine and let it take ownership of stateResolver and stateCleSession
	iStateTransitionEngine = CSsmStateTransitionEngine::NewL(stateResolver, stateCleSession);
	CleanupStack::Pop(2, stateResolver);
	// Configure engine
	iStateTransitionEngine->PerformCommandListValidation(EDoValidateCommandLists==aSetting);

	// --- Instantiate the SWP handling classes ---
	iSwpResolver = CSsmSwpPolicyResolver::NewL();
	iSwpCleSession = CCleSessionProxy::NewL(); // Thought: Perhaps the CleSessionProxy should be a CLE session factory, thus allowing a separate session to be created for each engine
	// Create the swp request handler and wire it up to the proxies
	iSwpRequestHandler = CSsmSwpRequestHandler::NewL();
	iSwpRequestHandler->SetSwpPolicyResolverProxy(iSwpResolver);
	iSwpRequestHandler->SetCleSessionProxy(iSwpCleSession);
	iSwpRequestHandler->PerformCommandListValidation(EDoValidateCommandLists==aSetting);

	//Request initial actions
	const TSsmStateTransition doStartup(initalState, 0);
	iStateTransitionEngine->SubmitL(doStartup);
	}

/**
 */
CSession2* CSsmServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	// Check we're the right version
	TVersion version(KSsmServMajorVersionNumber,KSsmServMinorVersionNumber,KSsmServBuildVersionNumber);
	if (!User::QueryVersionSupported(version,aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	// version supported, go ahead
	return CSsmSession::NewL();
	}

/**
 */
void CSsmServer::RequestStateTransitionL(const TSsmState& aState, TInt aReason, const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(iStateTransitionEngine, PanicNow(KPanicSysStateMgr,ESsmServerError3));

	const TSsmStateTransition request(aState, aReason);
	const MSsmStatePolicy::TResponse response = iStateTransitionEngine->TransitionAllowed(request, aMessage);
	switch(response)
		{
		case MSsmStatePolicy::EDefinitelyAllowed:
		case MSsmStatePolicy::ECurrentRemainReplaceQueued:
			{
			// A leave here will complete the client-request with KErrNoMemory
			iStateTransitionEngine->SubmitL(request, aMessage);
			break;
			}

		case MSsmStatePolicy::EReplaceCurrentClearQueue:
			{
			iStateTransitionEngine->Cancel(NULL);
			iStateTransitionEngine->SubmitL(request, aMessage);
			break;
			}

		case MSsmStatePolicy::ENotAllowed:
		default:
			{
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	}

/**
 Cancel any transition job that is accepted but not yet started.
 */
void CSsmServer::RequestStateTransitionCancel(CSession2* aSession)
	{
	iStateTransitionEngine->Cancel(aSession);
	}

/**
 Initiate processing of the request for a system wide property change
 */
void CSsmServer::RequestSwpChangeL(const TSsmSwp& aSwp, const RMessage2& aMessage)
	{
	iSwpRequestHandler->SubmitRequestL(aSwp, aMessage);
	}

/**
 Cancel processing of pending and running system wide property change requests
 */
void CSsmServer::RequestSwpChangeCancel(CSession2* aSession)
	{
	iSwpRequestHandler->Cancel(aSession);
	}

/**
 Register the policy file name for this swp
 This also marks the actual existence of the swp
 */
void CSsmServer::RequestRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename)
	{
	iSwpResolver->RegisterSwpMappingL(aSwpKey, aFilename);
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
 * Used only for testing purposes
 * DeRegister the policy file name for this swp
 */
#ifdef _DEBUG
void CSsmServer::RequestDeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename)
	{
	iSwpResolver->DeRegisterSwpMappingL(aSwpKey, aFilename);
	}
#endif

/**
 * Used only for testing purposes
 * Cleanup Transition Engine after the test is over
 */
#ifdef _DEBUG
void CSsmServer::DoCleanupStateTransitionEngine()
	{
	iStateTransitionEngine->CleanupTransitionEngine();
	}
#endif

/**
 * Used only for testing purposes
 * Cleanup Request Handler after the test is over
 */
#ifdef _DEBUG
void CSsmServer::DoCleanupSwpRequestHandler()
	{
	iSwpRequestHandler->CleanupSwpTransitionScheduler();
	}
#endif
