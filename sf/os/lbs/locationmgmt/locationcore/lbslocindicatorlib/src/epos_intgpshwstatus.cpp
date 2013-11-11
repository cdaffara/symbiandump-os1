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



// INCLUDE FILES
#include <e32property.h>
#include <e32cmn.h>
#include <lbs/locationfwdomainpskeys.h>
#include <lbs/epos_intgpshwstatus.h>
#include "epos_intgpshwstatusimpl.h"

//=============================================================================
// CPosIntGpsHwStatus
//=============================================================================


// ================= MEMBER FUNCTIONS =======================


CPosIntGpsHwStatus::CPosIntGpsHwStatus()
    {
    }

/** 
 Static constructor. 
 This function is usually used by the Hardware Integration Layer.
 @capability WriteDeviceData
 **/
EXPORT_C CPosIntGpsHwStatus* CPosIntGpsHwStatus::NewL()
    {
    CPosIntGpsHwStatus* self = CPosIntGpsHwStatus::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

/** 
 Static constructor. 
 This function is usually used by the UI and applications.
 It Subscribes to callbacks which fire when the status of Integrated GPS HW changes
 This is the overload for NewL() 
 
 @param aObserver [In] The observer to be used for callbacks
 **/
EXPORT_C CPosIntGpsHwStatus* CPosIntGpsHwStatus::NewL(MPosIntGpsHwStatusObserver& aObserver)
    {
    CPosIntGpsHwStatus* self = CPosIntGpsHwStatus::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// EPOC default constructor can leave.
void CPosIntGpsHwStatus::ConstructL()
    {
    iImpl = CPosIntGpsHwStatusImpl::NewL();
    }

// EPOC default constructor can leave.
void CPosIntGpsHwStatus::ConstructL(MPosIntGpsHwStatusObserver& aObserver)
    {
    iImpl = CPosIntGpsHwStatusImpl::NewL(aObserver);
    }

/** 
 Static constructor, leaves class on the cleanup stack.
 This function is usually used by the Hardware Integration Layer.
 This is the overload for NewL() 
 @capability WriteDeviceData
 **/
EXPORT_C CPosIntGpsHwStatus* CPosIntGpsHwStatus::NewLC()
    {
    CPosIntGpsHwStatus* self = new (ELeave) CPosIntGpsHwStatus;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/** 
 Static constructor, leaves class on the cleanup stack.
 This function is usually used by the UI and applications.
 It subscribes to callbacks which fire when the status of Integrated GPS HW changes
 This is the overload for NewLC() 
 
 @param aObserver [In] The observer to be used for callbacks
 **/
EXPORT_C CPosIntGpsHwStatus* CPosIntGpsHwStatus::NewLC(MPosIntGpsHwStatusObserver& aObserver)
    {
    CPosIntGpsHwStatus* self = new (ELeave) CPosIntGpsHwStatus;
    CleanupStack::PushL( self );
    self->ConstructL(aObserver);
    return self;
    }

/**
 * Destructor
 */
CPosIntGpsHwStatus::~CPosIntGpsHwStatus()
    {
    delete iImpl;
    }

/**
 Updates information about current status of Integrated GPS HW.
 @capability WriteDeviceData
 @leave KErrArgument, aStatus is not one of the values defined in TTechnologyStatus

 @param aStatus [In] Sets new status for Integrated GPS HW
 */
EXPORT_C void CPosIntGpsHwStatus::SetStatusL( TIntGpsHwStatus aStatus )
    {
    iImpl->SetStatusL(aStatus);
    }

/**
 Get the current status of Integrated GPS HW.
 @leave KErrPermissionDenied, if the caller process doesn't pass the Read Policy. KErrNotFound, if the property has not been defined. Or any other error code.
 
 @param aStatus	[Out] return value
 */
EXPORT_C void CPosIntGpsHwStatus::GetStatusL(TIntGpsHwStatus& aStatus)
	{
	User::LeaveIfError(iImpl->GetStatus(aStatus));
	}

//=============================================================================
// MPosIntGpsHwStatusObserver
//=============================================================================


/**
Provides the version of the observer interface. This can be used by the library 
to determine the functionality supported by the client application.

@return Version of the observer interface.
*/  
EXPORT_C TVersion MPosIntGpsHwStatusObserver::Version() const
	{
	return TVersion(1, 0, 0);
	}


/**
Should not be used. Provides for future expansion of the observer interface. 
*/  
/*virtual*/ EXPORT_C TAny* MPosIntGpsHwStatusObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


//  End of File
