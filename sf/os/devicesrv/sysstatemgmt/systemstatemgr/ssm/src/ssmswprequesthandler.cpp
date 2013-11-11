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

#include <ssm/ssmswppolicy.h>

#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmswprequesthandler.h"
#include "ssmswptransitionscheduler.h"
#include "ssmswptransitionengine.h"
#include "ssmswppolicyresolverproxy.h"
#include "ssmswppolicyframe.h"


//
// Creation and Construction methods
//

/**
@leave 	One of the system-wide error codes.
@return A newly created CSsmSwpRequestHandler
*/
CSsmSwpRequestHandler* CSsmSwpRequestHandler::NewL( )
	{
	CSsmSwpRequestHandler* self = NewLC ( );
	CleanupStack::Pop (self );
	return self;
	}

/**
@leave One of the system-wide error codes.
@return A newly created CSsmSwpRequestHandler on the cleanup stack
*/
CSsmSwpRequestHandler* CSsmSwpRequestHandler::NewLC( )
	{
	CSsmSwpRequestHandler* self = new (ELeave) CSsmSwpRequestHandler();
	CleanupStack::PushL (self );
	self->ConstructL ( );
	return self;
	}

/**
Empty constructor
*/
CSsmSwpRequestHandler::CSsmSwpRequestHandler() : iPerformCommandListValidation(ETrue)
	{
	}

/**
 Construct transition scheduler

 @leave	One of the system-wide error codes.
 */
void CSsmSwpRequestHandler::ConstructL( )
	{
	iSwpTransitionScheduler = CSsmSwpTransitionScheduler::NewL ( );
	}

/**
Empty destructor
*/
CSsmSwpRequestHandler::~CSsmSwpRequestHandler( )
	{
	delete iSwpTransitionScheduler;
	}

//
// Public API methods
//

void CSsmSwpRequestHandler::SetSwpHandlerMonitor(MSwpHandlerMonitor* aMonitor)
	{
	iSwpHandlerMonitor = aMonitor;
	}

void CSsmSwpRequestHandler::SetSwpPolicyResolverProxy(MSsmSwpPolicyResolverProxy* aPolicyResolverProxy)
	{
	iSwpPolicyResolverProxy = aPolicyResolverProxy;
	}

void CSsmSwpRequestHandler::SetCleSessionProxy(MCleSessionProxy* aCleSessionProxy)
	{
	iCleSessionProxy = aCleSessionProxy;
	}

void CSsmSwpRequestHandler::PerformCommandListValidation(TBool aSetting)
	{
	iPerformCommandListValidation = aSetting;
#ifdef _DEBUG
	if(iPerformCommandListValidation)
		{
		DEBUGPRINT1(_L("Turning on State command list validation"));
		}
	else
		{
		DEBUGPRINT1(_L("Turning off State command list validation"));
		}
#endif
	}

/**
Receives a swp request and handles it synchronously

@param aRequest An swp change request
@leave One of the system-wide error codes.
*/
void CSsmSwpRequestHandler::SubmitRequestL(const TSsmSwp& aRequest )
	{
	__ASSERT_DEBUG(NULL != iSwpTransitionScheduler, PanicNow(KPanicSysStateMgr,ESwpRequestHandlerError1));
	DoSubmitRequestL (aRequest, NULL );
	}

/**
Receives a swp request and handles it asynchronously

@param aRequest An swp change request
@param aMessage Client message  to complete
@leave One of the system-wide error codes.
*/
void CSsmSwpRequestHandler::SubmitRequestL(const TSsmSwp& aRequest,
		const RMessage2& aMessage )
	{
	__ASSERT_DEBUG(NULL != iSwpTransitionScheduler, PanicNow(KPanicSysStateMgr,ESwpRequestHandlerError2));
	DoSubmitRequestL (aRequest, &aMessage );
	}

/**
 Cancel all running and queued requests
 */
void CSsmSwpRequestHandler::Cancel(CSession2* aSession)
	{
	__ASSERT_DEBUG(NULL != iSwpTransitionScheduler, PanicNow(KPanicSysStateMgr,ESwpRequestHandlerError3));
	iSwpTransitionScheduler->Cancel(aSession);
	}

//
// Private methods
//

/**
Resolve the policy for the swp value
Set up the engine ready for running
Add the engine to the scheduler

@param aRequest An swp change request
@leave KErrNotFound If swp value is not registered
@leave Or one of the system-wide error codes
*/

void CSsmSwpRequestHandler::DoSubmitRequestL(const TSsmSwp& aRequest, const RMessage2* aMessage)
	{
	__ASSERT_DEBUG(NULL != iSwpPolicyResolverProxy, PanicNow(KPanicSysStateMgr,ESwpRequestHandlerError4));
	__ASSERT_DEBUG(NULL != iCleSessionProxy, PanicNow(KPanicSysStateMgr,ESwpRequestHandlerError5));

	NotifyRequestMade (aRequest );

	// get the policy
	CSsmSwpPolicyFrame* policyFrame = iSwpPolicyResolverProxy->GetSwpPolicyL(aRequest);
	SSMLOGLEAVEIFNULL(policyFrame);
	CleanupStack::PushL(policyFrame);

	// Create an engine
	CSsmSwpTransitionEngine* engine = CSsmSwpTransitionEngine::NewL (aRequest);
	engine->SetCleSessionProxy(iCleSessionProxy);
	engine->SetSwpPolicyFrame(policyFrame); // and take ownership
	CleanupStack::Pop (policyFrame ); // engine takes ownership
	CleanupStack::PushL (engine);

	engine->PerformCommandListValidation(iPerformCommandListValidation);

	iSwpTransitionScheduler->SubmitL (engine ); // scheduler takes ownership
	CleanupStack::Pop (engine );

	engine->SetClientMessage (aMessage ); // client wants to be notified
	}

//
//Event notification interface
//

/**
Flag request made

@param aRequest Value of swp request
*/
void CSsmSwpRequestHandler::NotifyRequestMade(const TSsmSwp& aRequest )
	{
	if (NULL != iSwpHandlerMonitor )
		{
		iSwpHandlerMonitor->SwpRequestMade (aRequest );
		}
	}
/**
 * Used only for testing purposes
 * Cleans up the Transition Scheduler
 */
#ifdef _DEBUG
void CSsmSwpRequestHandler::CleanupSwpTransitionScheduler()
	{
	iSwpTransitionScheduler->CleanupIndividualSchedulerArray();
	}
#endif
