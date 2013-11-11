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
#include "epos_defaultproxycommon.h"
#include "epos_cposrequestor.h"
#include "epos_cpossingletonmanager.h"
#include "epos_mposrequestorlistener.h"
#include "epos_cpospsyfixstatemanager.h"

#include <lbs/epos_cpositioner.h>
#include "epos_mposmodulestatusmanager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosRequestor::CPosRequestor( 
        MPosRequestorListener& aListener,
        CPosPsyFixStateManager& aFixStateManager,
        MPosModuleStatusManager& aModuleStatusManager )
    : CActive( CActive::EPriorityStandard ),
      iListener( aListener ),
      iFixStateManager( aFixStateManager ),
      iModuleStatusManager( aModuleStatusManager )
    {
    CActiveScheduler::Add( this );
    }

// EPOC default constructor can leave.
void CPosRequestor::ConstructL( 
        CPositioner& aDefaultPositioner,
        TPositionModuleId aModuleId )
    {
    iPositioner = CPositioner::NewL( aModuleId, aDefaultPositioner );
    }

// Two phase constructor
CPosRequestor* CPosRequestor::NewL( 
        CPositioner& aDefaultPositioner,           
        TPositionModuleId aModuleId,
        MPosRequestorListener& aListener,
        CPosPsyFixStateManager& aFixStateManager,
        MPosModuleStatusManager& aModuleStatusManager  )
    {
    CPosRequestor* self = new ( ELeave ) CPosRequestor( 
        aListener,
        aFixStateManager,
        aModuleStatusManager );
    CleanupStack::PushL(self);
    self->ConstructL( aDefaultPositioner, aModuleId );
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosRequestor::~CPosRequestor()
    {
    Cancel();
    delete iPosInfo;
    delete iPositioner;
    }

// ---------------------------------------------------------
// CPosRequestor::ModuleId
// ---------------------------------------------------------
//
TPositionModuleId CPosRequestor::ModuleId() const
    {
    return iPositioner->ImplementationUid();
    }

// ---------------------------------------------------------
// CPosRequestor::MakeLocationRequestL
// ---------------------------------------------------------
//
void CPosRequestor::MakeLocationRequestL( const TPositionInfoBase& aPosInfo )
    {
    if ( IsActive() )
        {
        //The previous location request will not be canceled.
        //We mark the iPosInfo is not uptodate.
        iIsPosInfoUpTodate= EFalse;
        return;
        }
    
    if ( iPosInfo != NULL )
        {
        delete iPosInfo;
        iPosInfo = NULL;
        }
        
    //Copy position info data structure
    TInt posInfoSize = aPosInfo.PositionClassSize();
    
    iPosInfo = reinterpret_cast< TPositionInfoBase* > (
        User::AllocL( posInfoSize ) );
        
    Mem::Copy( iPosInfo , &aPosInfo, posInfoSize );
    
    //Make location request
    iStatus = KRequestPending;
    iPositioner->NotifyPositionUpdate( *iPosInfo, iStatus );
    SetActive();
    iIsPosInfoUpTodate = ETrue;
    
    //Get previous device state
    TPositionModuleStatus moduleStatus;
    iModuleStatusManager.GetModuleStatus( 
        ModuleId(),
        moduleStatus );
        
    //Notify fix state manager that a PSY is used
    iFixStateManager.PsyUsed(
        ModuleId(),
        moduleStatus.DeviceStatus() );
    }

// ---------------------------------------------------------
// CPosRequestor::Cancel(TInt)
// ---------------------------------------------------------
//
void CPosRequestor::CancelWithReason(TInt aCancelReason)
	{
	iCancelReason = aCancelReason;
	Cancel();
	iCancelReason = KErrCancel;	//reset for next time
	}

// ---------------------------------------------------------
// CPosRequestor::StartTrackingL
// ---------------------------------------------------------
//
void CPosRequestor::StartTrackingL( TTimeIntervalMicroSeconds& aInterval )
    {
    if ( iPositioner->TrackingOverridden() && !iTrackingStarted )
        {
        iPositioner->StartTrackingL( aInterval );
        iTrackingStarted = ETrue;
        }
    }
        
// ---------------------------------------------------------
// CPosRequestor::StopTracking
// ---------------------------------------------------------
//
void CPosRequestor::StopTracking()
    {
    if ( iPositioner->TrackingOverridden() && iTrackingStarted )
        {
        iPositioner->StopTracking();
        iTrackingStarted = EFalse;
        }
    }

// ---------------------------------------------------------
// CPosRequestor::RunL
// ---------------------------------------------------------
//
void CPosRequestor::RunL()
    {
    iListener.LocationRequestCompleted( 
        ModuleId(),
        iStatus.Int(),
        *iPosInfo,
        iIsPosInfoUpTodate );
    }

// ---------------------------------------------------------
// CPosRequestor::DoCancel
// ---------------------------------------------------------
//
void CPosRequestor::DoCancel()
    {
    //We don't need to check if requestor is active or not.
    //CActive::Cancel() function will not call DoCancel()
    //if the active object is not active
    TRACESTRING2( "Location Request Canceled: %x", ModuleId() )
    iPositioner->CancelNotifyPositionUpdate(iCancelReason);

    //Notify fix state manager that location request is canceled
    iFixStateManager.SetPsyFixState(
        ModuleId(),
        iStatus.Int() );
    }


//  End of File
