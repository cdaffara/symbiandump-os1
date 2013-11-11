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
// mmrcserverpolicymanager.cpp
// 
//

#include "mmrcserverrulemanager.h"
#include "mmrcserversession.h"
#include "mmrcservercontroller.h"
#include "mmrcerrorcodes.h"
#include "mlogicalchain.h"
#include <a3f/a3f_trace_utils.h>


/**
 * By default Symbian 2nd phase constructor is private.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 */
CMMRCServerRuleManager::CMMRCServerRuleManager(CMMRCServerController& aServerController)
:iMMRCServerController(aServerController)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServerRuleManager::CMMRCServerRuleManager *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * Destructor
 */
CMMRCServerRuleManager::~CMMRCServerRuleManager( )
	{
	DP_CONTEXT(----> CMMRCServerRuleManager::~CMMRCServerRuleManager *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * Constructs, and returns a pointer to, a new CMMRCServerRuleManager object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @return CMMRCServerRuleManager* A pointer to newly created utlitly object.
 */
CMMRCServerRuleManager* CMMRCServerRuleManager::NewL(CMMRCServerController& aServerController)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerRuleManager::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerRuleManager* self = NewLC(aServerController);
	CleanupStack::Pop ( self );

	DP0_RET(self, "self=0x%x");
	}

/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new CMMRCServerRuleManager object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @return CMMRCServerRuleManager* A pointer to newly created utlitly object.
 */
CMMRCServerRuleManager* CMMRCServerRuleManager::NewLC(CMMRCServerController& aServerController)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerRuleManager::NewLC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerRuleManager* self = new(ELeave) CMMRCServerRuleManager(aServerController);
	CleanupStack::PushL (self );
	self->ConstructL (  );

	DP0_RET(self, "self=0x%x");
	}

/**
 	CMMRCServerRuleManager::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMRCServerRuleManager::ConstructL()
	{
	DP_CONTEXT(----> CMMRCServerRuleManager::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * Deduce the rule on the message according to the previous and current state
 * Return EProcessOnIdle if no rule has been found.
 * Leaves on failure.
 * @param TAudioState aAudioStateAllocatedResource
 * @param TAudioState aAudioStateRequiringProcess
 * @return TMMRCRule rule
 */
TMMRCRule CMMRCServerRuleManager::DecisionRule(TAudioState aAudioStateAllocatedResource, TAudioState aAudioStateRequiringProcess )
	{
	DP_CONTEXT(RMMRCClient::Open *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TMMRCRule rule = EProcessOnIdle;
	switch ( aAudioStateAllocatedResource )
		{
		//
		case EUninitialized:
		case EInitialized:
		case EDead:
			rule = EAlwaysProcess;
		break;
		
		//
		case EIdle:
		case EPrimed:
		case EActive:
			{
			if ( aAudioStateRequiringProcess == EUninitialized || aAudioStateRequiringProcess == EInitialized)
				{
				rule = EAlwaysProcess;
				}
			}
		break;
		
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KMMRCErrorNameUnkwnonAudioState, EMMRCErrorUnkwnonAudioState));
		break;
		}
	
	DP0_RET(rule, "rule=%d");
	}

/**
 * Deduce the rule on the message according to the previous and current state
 * Return EStop if no rule has been found.
 * @param TAudioState aAudioStateAllocatedResource
 * @param TAudioState aAudioStateRequiringProcess
 * @return TMMRCRule rule
 */
TReason CMMRCServerRuleManager::ActionReason(TAudioState aAudioStateLastCommited, TAudioState aAudioStateRequested )
	{
	DP_CONTEXT(----> CMMRCServerRuleManager::ActionReason *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TReason reason;
	switch (aAudioStateRequested )
		{
		case EUninitialized:
		if ( aAudioStateLastCommited == EInitialized )
			{
			reason = EUninitialize;
			}
		else
			{
			reason = EUninitialize ;
			}
		break;
		
		case EInitialized:
			if ( aAudioStateLastCommited == EUninitialized )
				{
				reason = EInitialize;
				}
			else if ( aAudioStateLastCommited == EIdle )
				{
				reason = EUnload;
				}
			else if ( aAudioStateLastCommited == EPrimed )
				{
				reason = EPrimeReason;
				}
			else if ( aAudioStateLastCommited == EActive )
				{
				reason = EStop;
				}
			else 
				{
				reason = EInitialize ;
				}
		break;
		
		case EIdle:
			if ( aAudioStateLastCommited == EUninitialized )
				{
				reason = ELoad;
				}
			else if ( aAudioStateLastCommited == EInitialized )
				{
				reason = ELoad;
				}
			else if ( aAudioStateLastCommited == EPrimed )
				{
				reason = EStop;
				}
			else if ( aAudioStateLastCommited == EActive )
				{
				reason = EStop;
				}
			else 
				{
				reason = ELoad;
				}
		break;
		
		case EPrimed:
			if ( aAudioStateLastCommited == EIdle )
				{
				reason = EPrimeReason;
				}
			else if ( aAudioStateLastCommited == EActive )
				{
				reason = EPrimeReason;
				}
			else 
				{
				reason = EPrimeReason;
				}
		break;
		
		case EActive:
			if ( aAudioStateLastCommited == EPrimed )
				{
				reason = EActivate;
				}
			else if ( aAudioStateLastCommited == EIdle )
				{
				reason = EActivate;
				}
			else 
				{
				reason = EActivate;
				}
		break;
		
		case EDead:
		default:
			reason = EStop;
		break;
		}
	
	DP0_RET(reason, "reason=%d");
	}

//EOF
