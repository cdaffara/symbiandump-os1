// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains method definitions for EtelMM subsession RMobileLine
// 
//

// From core API
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include "mm_hold.h"


/************************************************************************/
//
//  RMobileLine
//
/************************************************************************/

EXPORT_C RMobileLine::RMobileLine()
	: iMmPtrHolder(NULL)
/** Constructor. */
	{
	}

EXPORT_C void RMobileLine::ConstructL()
/** Constructs and initializes the object.

@leave KErrNoMemory Out of memory. 
@capability None
*/
	{
	RLine::ConstructL();
	__ASSERT_ALWAYS(iMmPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobileLinePtrHolder::NewL(CMobileLinePtrHolder::EMaxNumberLinePtrSlots,CMobileLinePtrHolder::EMaxNumberLinePtrCSlots);
	}

EXPORT_C void RMobileLine::Destruct()
/** Destructor 
@capability None
*/
	{
	RLine::Destruct();
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	}

EXPORT_C TInt RMobileLine::GetMobileLineStatus(RMobileCall::TMobileCallStatus& aStatus) const
/** Returns the current state of the line.

If there is one call object RMobileCall associated with this line the returned 
line status will be equal to the call status.

If there is more than one call object associated with this line, the state 
of the call that is most active, i.e. the call that is connected and active 
or is about to be connected, will be returned my this function.

@param aStatus On completion, contains the mobile line status.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPckg<RMobileCall::TMobileCallStatus> ptr1(aStatus);
	return Get(EMobileLineGetMobileLineStatus, ptr1);
	}

EXPORT_C void RMobileLine::NotifyMobileLineStatusChange(TRequestStatus& aReqStatus, RMobileCall::TMobileCallStatus& aStatus) const
/** Allows the client to be notified when a line changes state. This function completes 
when the mobile line changes it state.

Use RTelSubSessionBase::CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange) 
to cancel a previously placed asynchronous NotifyMobileLineStatusChange().

@param aReqStatus On return, KErrNone if successful.
@param aStatus On completion, contains the new mobile call status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobileLinePtrHolder::ESlot1LineNotifyMobileLineStatusChange, aStatus);

	Get(EMobileLineNotifyMobileLineStatusChange, aReqStatus, ptr1);
	}

