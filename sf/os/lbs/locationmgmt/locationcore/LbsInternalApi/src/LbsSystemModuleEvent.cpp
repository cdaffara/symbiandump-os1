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
//

#include <centralrepository.h>
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "LbsInternalCommon.h"

#include <lbscommon.h>

/** Initialize the system module event data. Check the security of the process, set the available keys to 
RProperty object. The secure ID of the property is set to the initialization process secure ID.

@released
@internalComponent
 */
 EXPORT_C void RLbsSystemModuleEvent::InitializeL()
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TInt err = RProperty::Define(KLbsSystemModuleEventKeyBase, RProperty::EInt, KReadPolicy, KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), KLbsSystemModuleEventKeyBase, TPositionModuleStatusEventBase::ESystemUnknown));

	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KSystemModuleEventCategoryKey, RProcess().SecureId());
	}
	
/** Delete all keys defined in RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsSystemModuleEvent::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KLbsSystemModuleEventKeyBase));
	}
		
/**	Nothing here.

@released
@internalComponent
 */
 EXPORT_C RLbsSystemModuleEvent::RLbsSystemModuleEvent()
	{
	}

/**	Open the RProperty object by initialization process UID

@param aInitializationProcessUid The initialization process UID.
@released
@internalComponent
 */
 EXPORT_C void RLbsSystemModuleEvent::OpenL()
	{
	__ASSERT_DEBUG(iProperty.Handle()==NULL, User::Invariant());		//Handle already open.
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KSystemModuleEventCategoryKey, categoryUid);
	
	User::LeaveIfError(iProperty.Attach(categoryUid, KLbsSystemModuleEventKeyBase));
	}

/** Close the RProperty object of the module 

@released
@internalComponent
 */
EXPORT_C void RLbsSystemModuleEvent::Close()
	{
	iProperty.Close();
	}

/** Set system module event data.

@param aEvent A reference to TSystemModuleEvent storing the system module event data.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsSystemModuleEvent::SetSystemModuleEvent(const TPositionModuleStatusEventBase::TSystemModuleEvent& aEvent)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return iProperty.Set(reinterpret_cast<const TInt&>(aEvent));
	}
	
/** Get system module event data.

@param aEvent A reference to TSystemModuleEvent storing the returned system module event data.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsSystemModuleEvent::GetSystemModuleEvent(TPositionModuleStatusEventBase::TSystemModuleEvent& aEvent) const
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return const_cast<RLbsSystemModuleEvent*>(this)->iProperty.Get(reinterpret_cast<TInt&>(aEvent));
	}
	
/** Set request for system module event notification by subscribe to the RProperty object

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
 */
 EXPORT_C void RLbsSystemModuleEvent::NotifySystemModuleEvent(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Subscribe(aStatus);
	}

/** Cancel the system module event notification request. Stop the notification publish from the RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsSystemModuleEvent::CancelNotifySystemModuleEvent()
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.

	iProperty.Cancel();
	}
