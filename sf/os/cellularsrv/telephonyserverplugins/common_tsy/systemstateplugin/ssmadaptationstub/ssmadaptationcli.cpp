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

#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	#include "ssmadaptationcli.h"
#else
	#include <ssm/ssmadaptationcli.h>
#endif

// for keys definition. These keys are  used to manage SSM_Stub behaviour
#include "../../test/component/inc/ssmstub_properties.h"

#include "e32property.h"

const TUid KComponentTestUID = {0x101F777C};


EXPORT_C void RSsmEmergencyCallRfAdaptation::InitializeLibL()
	{			
	}

EXPORT_C void RSsmEmergencyCallRfAdaptation::UnInitializeLib()
	{
	}

/**
 Connect this session without pre-allocating any asynchronous message slots.
 
 @return KErrNone, if successful, otherwise one of the other system-wide
 			error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C TInt RSsmEmergencyCallRfAdaptation::Connect()
	{
	TInt error(KErrNone);	
	if (KErrNone != RProperty::Get (KComponentTestUID, EErrorForConnect, error))
		{
		error = KErrNone;
		}
	return error;
	}

/**
 Connect this session and pre-allocate the number of asynchronous message slots you will need.
 I.e. the max number of asynchronoud calls to this call you will ever need.
 
 Use this version of @c Connect if you want to be guarantee allocation of slots rather
 than relying on slots being allocated from the system wide pool.

 @return KErrNone, if successful, otherwise one of the other system-wide
 			error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C TInt RSsmEmergencyCallRfAdaptation::Connect(TInt /*aAsyncMessageSlotCount*/)
	{
	TInt error(KErrNone);	
	if (KErrNone != RProperty::Get (KComponentTestUID, EErrorForConnect, error))
		{
		error = KErrNone;
		}
	return error;	
	}


/**
 Requests the adaptation to enable RF communication for emergency calls.
   
 @param aStatus 	The request status.
					This is set to KErrNone on successful completion of self tests, KErrNotSupported
					if this operations is not supported by the adaptation, KErrPermissionDenied
					if the current process does not have sufficient capabilities, otherwise one
					of the other system-wide error codes.

 @capability WriteDeviceData
 @capability PowerMgmt
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmEmergencyCallRfAdaptation::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
	{	
	// check if we should complete Activate
	TBool complete(ETrue);	
	
	// store request status 
	iRequestStatus = aStatus;
	
	if (KErrNone == RProperty::Get (KComponentTestUID, ECompleteActivate, complete))
		{
		// if we shouldn't complete this request
		if (!complete)
			{
			// do nothing
			return;
			}
		}
	
	TRequestStatus* ptrStatus = &aStatus;
	TInt error(KErrNone);
	
	if (KErrNone != RProperty::Get (KComponentTestUID, EErrorForActivate, error))
		{
		error = KErrNone;
		}
		
	User::RequestComplete(ptrStatus, error);
	}

/**
  Requests the adaptation to disable RF communication for emergency calls.
   
 @param aStatus 	The request status.
					This is set to KErrNone on successful completion of self tests, KErrNotSupported
					if this operations is not supported by the adaptation, KErrPermissionDenied
					if the current process does not have sufficient capabilities, otherwise one
					of the other system-wide error codes.
					
 @capability WriteDeviceData
 @capability PowerMgmt
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
	{	
	// check if we should complete Activate
	TBool complete(ETrue);	
	
	// store request status 
	iRequestStatus = aStatus;
	
	if (KErrNone == RProperty::Get (KComponentTestUID, ECompleteDeactivate, complete))
		{
		// if we shouldn't complete this request
		if (!complete)
			{
			// do nothing
			return;
			}
		}
	
	TRequestStatus* ptrStatus = &aStatus;
	TInt error(KErrNone);
	
	if (KErrNone != RProperty::Get (KComponentTestUID, EErrorForDeactivate, error))
		{
		error = KErrNone;
		}
		
	User::RequestComplete(ptrStatus, error);
	}



EXPORT_C void RSsmEmergencyCallRfAdaptation::Close()
	{
	
	}
	
EXPORT_C void RSsmEmergencyCallRfAdaptation::Cancel()
	{
	TRequestStatus* ptrStatus = &iRequestStatus;
	User::RequestComplete(ptrStatus, KErrCancel);
	}

