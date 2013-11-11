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
#include "epos_intgpshwstatusimpl.h"

/**
 @file epos_intgpshwstatusimpl.cpp
 @released
 @internalComponent
 */

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosIntGpsHwStatusImpl::CPosIntGpsHwStatusImpl()
	: CActive(EPriorityStandard)
    {
    
    }

// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::NewL
// ---------------------------------------------------------
//
CPosIntGpsHwStatusImpl* CPosIntGpsHwStatusImpl::NewL()
    {
    CPosIntGpsHwStatusImpl* self = CPosIntGpsHwStatusImpl::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::NewL
// ---------------------------------------------------------
//
CPosIntGpsHwStatusImpl* CPosIntGpsHwStatusImpl::NewL(MPosIntGpsHwStatusObserver& aObserver)
    {
    CPosIntGpsHwStatusImpl* self = CPosIntGpsHwStatusImpl::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// EPOC default constructor can leave.
void CPosIntGpsHwStatusImpl::ConstructL()
    {
    CreatePSKeyL();
    }

void CPosIntGpsHwStatusImpl::CreatePSKeyL()
	{
   //Define read security policy
    _LIT_SECURITY_POLICY_PASS( readPolicy );

    //Define write security policy
    TSecurityPolicy writePolicy( ECapabilityWriteDeviceData );

    TInt error = iProperty.Define(
		KPosIndicatorCategoryUid,
		KPosIntGpsHwStatus,
		RProperty::EInt,
		readPolicy,
		writePolicy );
    
    if(error != KErrAlreadyExists)
        {
        User::LeaveIfError(error);
        }
	}

// EPOC default constructor can leave.
void CPosIntGpsHwStatusImpl::ConstructL(MPosIntGpsHwStatusObserver& aObserver)
    {
    iObserver = &aObserver;
    CActiveScheduler::Add(this);
    User::LeaveIfError(iProperty.Attach(KPosIndicatorCategoryUid, KPosIntGpsHwStatus));
    iProperty.Subscribe(iStatus);
    SetActive();
    }

// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::GetStatus
// ---------------------------------------------------------
//
TInt CPosIntGpsHwStatusImpl::GetStatus(CPosIntGpsHwStatus::TIntGpsHwStatus& aStatus)
	{
	TInt hwStatusInt;
	TInt error = iProperty.Get(KPosIndicatorCategoryUid, KPosIntGpsHwStatus, hwStatusInt);
	TPosIntGpsHwState state = static_cast<TPosIntGpsHwState> (hwStatusInt);
	switch(state)
		{
		case EPosIntGpsHwOff:
			aStatus = CPosIntGpsHwStatus::EStatusOff;
			break;
		case EPosIntGpsHwOn:
			aStatus = CPosIntGpsHwStatus::EStatusOn;
			break;
		case EPosIntGpsHwIntermediate:
			aStatus = CPosIntGpsHwStatus::EStatusActive;
			break;
		default:
			aStatus = CPosIntGpsHwStatus::EStatusOff;
			break;
		}
	return error;
	}

// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::NewLC
// ---------------------------------------------------------
//
CPosIntGpsHwStatusImpl* CPosIntGpsHwStatusImpl::NewLC()
    {
    CPosIntGpsHwStatusImpl* self = new (ELeave) CPosIntGpsHwStatusImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::NewLC
// ---------------------------------------------------------
//
CPosIntGpsHwStatusImpl* CPosIntGpsHwStatusImpl::NewLC(MPosIntGpsHwStatusObserver& aObserver)
    {
    CPosIntGpsHwStatusImpl* self = new (ELeave) CPosIntGpsHwStatusImpl;
    CleanupStack::PushL( self );
    self->ConstructL(aObserver);
    return self;
    }

// Destructor
CPosIntGpsHwStatusImpl::~CPosIntGpsHwStatusImpl()
    {
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------
// CPosIntGpsHwStatusImpl::SetStatusL
// ---------------------------------------------------------
//
void CPosIntGpsHwStatusImpl::SetStatusL( CPosIntGpsHwStatus::TIntGpsHwStatus aStatus )
    {
    TInt indicatorState = EPosIntGpsHwOff;

    switch ( aStatus )
        {
        case CPosIntGpsHwStatus::EStatusOff:
            indicatorState = EPosIntGpsHwOff;
            break;
        case CPosIntGpsHwStatus::EStatusOn:
            indicatorState = EPosIntGpsHwOn;
            break;
        case CPosIntGpsHwStatus::EStatusActive:
            indicatorState = EPosIntGpsHwIntermediate;
            break;
        case CPosIntGpsHwStatus::EStatusUnknown:
        default:
            indicatorState = EPosIntGpsHwOff;
            break;
        }
    CPosIntGpsHwStatus::TIntGpsHwStatus oldStatus = iLastSet;
    
    if(aStatus == CPosIntGpsHwStatus::EStatusUnknown)
    	{
    	// EStatusUnknown gets converted to off when stored in the P&S key
    	iLastSet = CPosIntGpsHwStatus::EStatusOff;
    	}
    else
    	{
    	// Otherwise just store the last value
    	iLastSet = aStatus;
    	}
    
    TInt error = iProperty.Set(
        KPosIndicatorCategoryUid,
        KPosIntGpsHwStatus,
        indicatorState );
    if(error == KErrNotFound)
    	{
    	// Key does not exist yet, define it now
    	CreatePSKeyL();
    	// Now try setting it again
    	TInt retryError = iProperty.Set(
    	        KPosIndicatorCategoryUid,
    	        KPosIntGpsHwStatus,
    	        indicatorState);
    	
    	if (retryError != KErrNone)
    		{
    		// In error condition - restore the last value
    		iLastSet = oldStatus;
    		User::Leave(retryError);
    		}
    	}
    else if(error != KErrNone)
    	{
    	// In error condition - restore the last value
    	iLastSet = oldStatus;
    	User::Leave(error);
    	}
    }


void CPosIntGpsHwStatusImpl::RunL()
	{
	// Resubscribe to avoid missing anything
	iProperty.Subscribe(iStatus);
	
	// Get property
	CPosIntGpsHwStatus::TIntGpsHwStatus hwStatus;
	TInt error = GetStatus(hwStatus);
	
	// Only send this callback if it is different from the last callback sent
	if(iLastSet != hwStatus)
		{
		iLastSet = hwStatus;
		iObserver->OnStatusUpdateEvent(hwStatus, error);
		}
	SetActive();
	}


void CPosIntGpsHwStatusImpl::DoCancel()
	{
	iProperty.Cancel();
	}

//  End of File
