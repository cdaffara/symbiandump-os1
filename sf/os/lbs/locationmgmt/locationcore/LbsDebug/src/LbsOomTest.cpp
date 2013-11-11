// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Includes
// System
// 
//

#include <e32base.h>
#include <e32debug.h>

// LBS
#include <centralrepository.h>
#include "lbsrootcenrepdefs.h"

// Component
#include "LbsOomTest.h"


//****************************************************************
// Class ROomRequest
//****************************************************************
/** Initialize the property.
Initialize the position OOM Test property.
@released
@internalComponent
*/
EXPORT_C void ROomRequest::InitializeL()
	{
	// Do we need to consider the security issue here?
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TInt err = RProperty::Define(KOomTestKey, 
								 RProperty::EInt, 
								 KReadPolicy,
								 KWritePolicy);
    if (err != KErrNone && err != KErrAlreadyExists)
         {
         User::Leave(err);
         }
    
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), KOomTestKey, 0)); // by default no OOM Test

	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	SetCategoryUidL(KOomTestCategoryKey, RProcess().SecureId());
	}
	

/** Delete key defined in RProperty object.
@released
@internalComponent
*/
EXPORT_C void ROomRequest::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KOomTestKey));
	}



/** Constructor
@released
@internalComponent
*/
EXPORT_C ROomRequest::ROomRequest()
	{
	}
	
	
/** Set the Oom property
Update the OOM Test Signal, call by test code
@param aOomTestOn a boolean switching heap allocation failure on/off
@released
@internalComponent
*/
EXPORT_C TInt ROomRequest::OomTestUpdates(TBool aOomTestOn)
	{
	__ASSERT_DEBUG(iOomTestSignal.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return iOomTestSignal.Set((TInt)aOomTestOn);
	}


/** Subscribe to OOM request bus
@param aStatus TRequestStatus request status object to be signalled on update.
@released
@internalComponent
*/
EXPORT_C void ROomRequest::OomTestNotifiyUpdates(TRequestStatus& aStatus)
	{
 	__ASSERT_DEBUG(iOomTestSignal.Handle()!=NULL, User::Invariant()); //Handle not open.
	iOomTestSignal.Subscribe(aStatus);
	}

 
/** Read OOM property
@released
@internalComponent
*/
EXPORT_C TBool ROomRequest::GetOomTestNotifiyUpdates()
 	{
	__ASSERT_DEBUG(iOomTestSignal.Handle() != NULL, User::Invariant());
	TInt signal = 0;
	iOomTestSignal.Get(signal);
	return ((TBool)signal);
 	}
 	
 	
/**	Open the RProperty object 
@released
@internalComponent
*/
EXPORT_C void ROomRequest::OpenL()
	{
	__ASSERT_DEBUG(iOomTestSignal.Handle()==NULL, User::Invariant()); //Handle already open.
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	GetCategoryUidL(KOomTestCategoryKey, categoryUid);
	
	User::LeaveIfError(iOomTestSignal.Attach(categoryUid, KOomTestKey));

	}
	

/** Cancel the subscription
@released
@internalComponent
*/
EXPORT_C void ROomRequest::CancelToReceiveOomSignal()
	{
	iOomTestSignal.Cancel();	
	}
	
	
/** Close the RProperty object of the module 
@released
@internalComponent
*/
EXPORT_C void ROomRequest::Close()
	{
	iOomTestSignal.Cancel();	
	iOomTestSignal.Close();
	}
	

/** Utility function to simplify retrieving the CategoryUID of the given
property from the LbsRoot central repository file.
@param aKey A property key
@param aCategoryUid A TUid category key
@released
@internalComponent
*/
void ROomRequest::GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid)
	{
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(aKey, category));
	aCategoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);
	}
	
	
/** Utility function to simplify setting the CategoryUID of the given
property in the LbsRoot central repository file.
@param aKey A property key
@param aCategoryUid A TUid category key
@released
@internalComponent
*/
void ROomRequest::SetCategoryUidL(TUint32 aKey, TUid aCategoryUid)
	{
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(aKey, TInt(aCategoryUid.iUid)));
	CleanupStack::PopAndDestroy(rep);
	}
