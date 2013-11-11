// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of TStrtSimStatusMgr class.
//




/**
 @file
 @internalComponent
 @released
*/

#include "ssmuiproviderdll.h"
#include "swppolicy_simstatus.h"
#include "ssmdebug.h"

#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/ssmcommandfactory.h>

EXPORT_C MSsmSwpPolicy* CSimStatuspolicy::NewL()
	{
	CSimStatuspolicy* self = new (ELeave) CSimStatuspolicy;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSimStatuspolicy::CSimStatuspolicy()
	{
	}

CSimStatuspolicy::~CSimStatuspolicy()
	{
	delete iCommandList;
	iProperty.Close();
	}

void CSimStatuspolicy::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey()));
	}

void CSimStatuspolicy::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSimStatuspolicy::InitializeCancel()
	{
	}

void CSimStatuspolicy::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSimStatuspolicy::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	MSsmSwpPolicy::TResponse isTransitionAllowed = ENotAllowed;
	TBool hasCapability = aMessage.HasCapability(ECapabilityWriteDeviceData, __PLATSEC_DIAGNOSTIC_STRING( "Platsec violation for sim status transition" ));

	//Return as transition not allowed if it doesnt have sufficient capability
	if(!hasCapability)
		{
		return isTransitionAllowed;
		}

	switch (aSwp.Value())
		{
		case ESimNotSupported:
			isTransitionAllowed = IsSimNotSupportedTransitionAllowed();
			break;
		case ESimNotPresent:
			isTransitionAllowed = IsSimRemovedTransitionAllowed();
			break;
		case ESimReadable:
			isTransitionAllowed = IsSimReadableTransitionAllowed();
			break;
		case ESimUsable:
			isTransitionAllowed = IsSimUsableTransitionAllowed();
			break;
		case ESimNotReady:
			isTransitionAllowed = EAllowed;
			break;
		}

	return isTransitionAllowed;
	}

MSsmSwpPolicy::TResponse CSimStatuspolicy::IsSimReadableTransitionAllowed()
    {
    TInt simStatus;
    iProperty.Get(simStatus);

    if ((ESimStatusUninitialized == simStatus ||
    		ESimNotPresent == simStatus ||
    		ESimReadable == simStatus ||
    		(iSimHasBeenReadable && ESimUsable ==simStatus) ||
    		(iSimHasBeenReadable && ESimNotReady == simStatus) ) &&
    		CSsmUiSpecific::IsSimStateChangeAllowed())
    	{
    	iSimHasBeenReadable = ETrue;
    	return EAllowed;
    	}
    return ENotAllowed;
    }

MSsmSwpPolicy::TResponse CSimStatuspolicy::IsSimUsableTransitionAllowed()
	{
	TInt simStatus;
	iProperty.Get(simStatus);
	if ((ESimReadable == simStatus ||
			ESimNotPresent == simStatus ||
			ESimUsable == simStatus ||
			(iSimHasBeenUsable && ESimNotReady ==simStatus)) &&
			CSsmUiSpecific::IsSimStateChangeAllowed())
		{
		iSimHasBeenUsable = ETrue;
		return EAllowed;
		}
	return ENotAllowed;
	}

MSsmSwpPolicy::TResponse CSimStatuspolicy::IsSimNotSupportedTransitionAllowed()
	{
	TInt simStatus;
	iProperty.Get(simStatus);
	if (ESimStatusUninitialized == simStatus ||
			ESimNotSupported == simStatus)
		{
		return EAllowed;
		}
	return ENotAllowed;
	}

MSsmSwpPolicy::TResponse CSimStatuspolicy::IsSimRemovedTransitionAllowed()
	{
	TInt simStatus;
	iProperty.Get(simStatus);

	if ((ESimStatusUninitialized == simStatus ||
			ESimUsable == simStatus ||
			ESimReadable == simStatus ||
			ESimNotReady == simStatus ||
			ESimNotPresent == simStatus) &&
			CSsmUiSpecific::IsSimStateChangeAllowed())
		{
		return EAllowed;
		}
	return ENotAllowed;
	}

void CSimStatuspolicy::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	TRAPD(err, DoPrepareCommandListL(aSwp));
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSimStatuspolicy::DoPrepareCommandListL(const TSsmSwp& aSwp)
	{
	// Reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

  // Create a PS command to change the sim status PS for compatibility reasons
	MSsmCommand* cmdPS = SsmCommandFactory::ConstructSetPAndSKeyCommandLC(ECmdCriticalSeverity, CSsmUiSpecific::StartupPSUid(), CSsmUiSpecific::SimStatusPropertyKey(), aSwp.Value());
	iCommandList->AppendL(cmdPS);
	CleanupStack::Pop(cmdPS);

	// Create a command to publish the SwP value
	MSsmCommand* cmdSwp = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	iCommandList->AppendL(cmdSwp);
	CleanupStack::Pop(cmdSwp);
	}

void CSimStatuspolicy::PrepareCommandListCancel()
	{
	delete iCommandList;
	iCommandList = NULL;
	}

CSsmCommandList* CSimStatuspolicy::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSimStatuspolicy::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	(void)aError;
	
	TRequestStatus* rs = &aStatus;
#if defined(_DEBUG)
	if(KErrNone != aError)
		{
		DEBUGPRINT3A("ERROR: Request SwP change for SIM status (SwP key %d) completed with error %d", aSwp.Key(), aError);
		}
	//Complete the status with the error which inturn panics SSM server	
	User::RequestComplete(rs, aError);
#else // _DEBUG
	//Completing the status with KErrNone to avoid panic in SSM server
	User::RequestComplete(rs, KErrNone);
#endif // _DEBUG
	}

void CSimStatuspolicy::HandleCleReturnValueCancel()
	{
	}
