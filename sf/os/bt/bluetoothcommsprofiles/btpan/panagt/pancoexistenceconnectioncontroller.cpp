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

#include <bluetooth/logger.h>

#include "pancoexistenceconnectioncontroller.h"
#include "panagtutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

// This is the identifer of the shared mutex used to allow safe read / write of the coexistence P&S key.
// This can only be changed if all other existing definitions of the mutex name are also changed.
_LIT(KIPBearerCoexistenceMutex, "IPBearerCoexistenceMutex");

using namespace PanAgent;

CPanCoexistenceConnectionController* CPanCoexistenceConnectionController::NewL(MPanCoexistenceConnectionObserver& aObserver)
	{
	CPanCoexistenceConnectionController* self = new(ELeave) CPanCoexistenceConnectionController(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPanCoexistenceConnectionController::CPanCoexistenceConnectionController(MPanCoexistenceConnectionObserver& aObserver)
	: CActive(EPriorityStandard),iPanCoexistenceConnectionObserver(aObserver) 
	{
	CActiveScheduler::Add(this);
	}

CPanCoexistenceConnectionController::~CPanCoexistenceConnectionController()
	{
	Cancel();
	iProperty.Close();
	iMutex.Close();
	}

void CPanCoexistenceConnectionController::ConstructL()
	{
	LOG_FUNC
	
	// Creates a global mutex and open a handle to the mutex.
	// (Note if CreateGlobal returns wirh KErrNone then the mutex is also opened).  
	TInt err = iMutex.CreateGlobal(KIPBearerCoexistenceMutex);
	if (err==KErrAlreadyExists)
		{
		err=iMutex.OpenGlobal(KIPBearerCoexistenceMutex);
		}
    
    User::LeaveIfError(err);

	// Define property
    err = DefineProperty();

	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Attach to the property
	TSecureId thisSID = RProcess().SecureId();
	User::LeaveIfError(iProperty.Attach(thisSID, KIPBearerCoexistenceProperty));
	
	SubscribeToProperty();
	}

//Define the P&S key
TInt CPanCoexistenceConnectionController::DefineProperty()
	{
	TInt err;
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	_LIT_SECURITY_POLICY_C1(KNetworkControl, ECapabilityNetworkControl); 

	err=iProperty.Define(KIPBearerCoexistenceProperty,
						 RProperty::EInt,
						 KPassPolicy,	    //	Read policy
						 KNetworkControl);	//	Write policy
	return err;
	}

// If another IP bearer has not set the key then this method will set the P&S key to indicate 
// that a PAN connection is active.
TInt CPanCoexistenceConnectionController::TryToCreateNewPanConnection()
	{	
	LOG_FUNC
	
	TInt err = KErrNone;
	
	// Wait for the Mutex to become free and then acquire it.
	iMutex.Wait();
	
	//Get the property	
	TIPBearerCoexistenceStatus value = ReadStatus();
	
	switch (value)
		{
		case ENoneIsActive:
			iActivePanConnectionExists = ETrue;
			iLocallyInitiatedTransition = ETrue;

			WriteStatus(EBTPanIsActive);
			break;

		case EBTPanIsActive:
			//P&S key is allready set to EBTPanIsActive 
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EBTPanIsAlreadyActive));
			break;

		case ERndisIsActive:
			//BT PAN cannot be active at the point, since Rndis holds the key
			__ASSERT_DEBUG(!iActivePanConnectionExists, PanAgentPanic(ERndisIsActiveWhenBTPanTriesToBeActive));
			err = KErrAccessDenied;
			break;

		default:
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EInvalidIPBearerCoexistanceProperty));
			break;
		}
		
	//Release the mutex
	iMutex.Signal();
	return err; 
	}

// If the co-existence P&S key has been set by the PAN service then
// clear the P&S key
void CPanCoexistenceConnectionController::HandleAllPanConnectionsClosed()
	{
	LOG_FUNC
	
	// Get the property	
	TIPBearerCoexistenceStatus value = ReadStatus();
	
	switch(value)
		{
		case ENoneIsActive:
			// The P&S key is ENoneIsActive, when BT PAN tries to clear it.
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EClearKeyWhenNoneIsActive));
			break;
		
		case ERndisIsActive:
			// This is the race condition where RNDIS sets the key when BT PAN incoming connection request
			// goes through the stack, we didn't own the key, do nothing
			LOG(_L("CPanCoexistenceConnectionController::HandleAllPanConnectionsClosed, enter the race condition"));
			break;
			
		case EBTPanIsActive:
			__ASSERT_DEBUG(iActivePanConnectionExists, PanAgentPanic(ENoPanConnectionExistsWhileInPanIsActiveCoexistencePandSState));
			iActivePanConnectionExists = EFalse;
			iLocallyInitiatedTransition = ETrue;

			WriteStatus(ENoneIsActive);
			break;

		default:
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EInvalidIPBearerCoexistanceProperty));
			break;
		}
	}

// Read property and handle error conditions
TIPBearerCoexistenceStatus CPanCoexistenceConnectionController::ReadStatus()
	{
	LOG_FUNC

	//Get the property
	TInt value = ENoneIsActive;
	TInt err = iProperty.Get(value);
	
	if(err)
		{
		if (err == KErrNotFound)
			{
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyIsNotDefined));
					
			//Redefine property if property has been deleted	
			err = DefineProperty();
			 			
			if (err == KErrNone)
				{
				value = iActivePanConnectionExists ? EBTPanIsActive : ENoneIsActive;
				err = iProperty.Set(value);
				if (err)
					{
					//reset failed, log the error and assert the debug
					LOG1(_L("CPanCoexistenceConnectionController::ReadProperty reset Property with error: %d"), err);
					__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertySetFailed));
					}
				}
			else
				{
				//Define P&S failed, log the error and assert the debug
				LOG1(_L("CPanCoexistenceConnectionController::ReadProperty Define Property with error: %d"), err);
				__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyCanNotBeDefined));
				}			
			}
		else
			{
			//Getter returns error rather than KErrNotFound, log the error and assert the debug
			LOG1(_L("CPanCoexistenceConnectionController::ReadProperty Get Property with error: %d"), err);
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyGetFailed));
			}
		}
	
	return static_cast<TIPBearerCoexistenceStatus>(value);
	}

//Write the property and handle setter errors
void CPanCoexistenceConnectionController::WriteStatus(TIPBearerCoexistenceStatus aStatus)
	{
	LOG_FUNC
	
	//Set the property
	TInt err = iProperty.Set(aStatus);
	if (err)
		{
		if (err == KErrNotFound)
			{
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyIsNotDefined));
					
			//Redefine property if property has been deleted				
			err = DefineProperty();
			 	
			if (err == KErrNone)
				{
				//Reset the property
				err = iProperty.Set(aStatus);
				if (err)
					{
					LOG1(_L("CPanCoexistenceConnectionController::WriteStatus Set Property with error: %d"), err);
					__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertySetFailed));
					}
				}
			else
				{
				//Define P&S failed, log the error and assert the debug
				LOG1(_L("CPanCoexistenceConnectionController::ReadProperty Define Property with error: %d"), err);
				__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyCanNotBeDefined));
				//Nothing else we can do at this stage
				}

			}
		else
			{
			//Setter returns error rather than KErrNotFound
			LOG1(_L("CPanCoexistenceConnectionController::WriteStatus Set Property with error: %d"), err);
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertySetFailed));
			}
		}
	}

void CPanCoexistenceConnectionController::SubscribeToProperty()
	{
	LOG_FUNC
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CPanCoexistenceConnectionController::DoCancel()
	{
	LOG_FUNC
	iProperty.Cancel();
	}

void CPanCoexistenceConnectionController::RunL()
	{	
	LOG_FUNC
	
	// Do not check the iStatus, since the ReadStatus() will redefine the P&S key if the key 
	// has been deleted by mistakes

	//Read the property
	TIPBearerCoexistenceStatus value = ReadStatus();
	
	switch(value)
		{
		case ENoneIsActive:
			if (!iLocallyInitiatedTransition)
				{
				// BT PAN cannot be active at this point, since the P&S key is ENoneIsActive
				__ASSERT_DEBUG(!iActivePanConnectionExists, PanAgentPanic(EPropertyIsNoneIsActiveWhenBTPanIsActive));
						
				// Notify Agent P&S Key has been updated
				iPanCoexistenceConnectionObserver.MpccoPanCoexistenceConnectionUpdated(ENoneIsActive);
				}
			break;

		case ERndisIsActive:
			// BT PAN cannot be active at this point
			__ASSERT_DEBUG(!iActivePanConnectionExists, PanAgentPanic(ERndisIsActiveWhenBTPanTriesToBeActive));
			__ASSERT_DEBUG(!iLocallyInitiatedTransition, PanAgentPanic(EBTPanInitiateDirectTransitionToRndis));
			
			//from non service to Rndis
			//Notify Agent P&S Key has been updated
			iPanCoexistenceConnectionObserver.MpccoPanCoexistenceConnectionUpdated(ERndisIsActive);
			break;

		case EBTPanIsActive:
			//Setting the P&S key to EBTPanIsActive when BT PAN is not active or BT PAN is already active
			__ASSERT_DEBUG(iActivePanConnectionExists && iLocallyInitiatedTransition, PanAgentPanic(EImproperSituationToSetPropertyToBTPanIsActive));
			break;

		default:
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EInvalidIPBearerCoexistanceProperty));
			break;
		}
	
	iLocallyInitiatedTransition = EFalse;

	//Resubscribe to the property
	SubscribeToProperty();
	}

