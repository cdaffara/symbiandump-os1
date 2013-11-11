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
#include "epos_cposexternalgpsmonitor.h"
#include "epos_cpospsylisthandler.h"
#include "epos_cpospsyfixstatemanager.h"
#include "epos_cpossingletonmanager.h"
#include "epos_cposrequestor.h"
#include "epos_cposconstmanager.h"
#include "epos_defaultproxycommon.h"

#include <lbs/epos_cpositioner.h>



// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosExternalGpsMonitor::CPosExternalGpsMonitor(
        MPosModuleStatusManager& aModuleStatusManager )
    : iModuleStatusManager( aModuleStatusManager )
    {
    }

// EPOC default constructor can leave.
void CPosExternalGpsMonitor::ConstructL()
    {
    TRACESTRING( "CPosExternalGpsMonitor::ConstructL start... " )

    iPsyListHandler = CPosPsyListHandler::GetInstanceL();
    iPsyListHandler->AddListenerL( this );

    iFixStateManager = CPosPsyFixStateManager::GetInstanceL();
    iConstManager = CPosConstManager::GetInstanceL();
    
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    TRACESTRING( "CPosExternalGpsMonitor::ConstructL end" )
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::GetInstanceL
// ---------------------------------------------------------
//
CPosExternalGpsMonitor* CPosExternalGpsMonitor::GetInstanceL(
        CPositioner& aDefaultPositioner,
        MPosModuleStatusManager& aModuleStatusManager )
    {
    CPosExternalGpsMonitor* self = reinterpret_cast < CPosExternalGpsMonitor* > 
        ( CPosSingletonManager::GetObject( 
            EPosSigletonObjectExtGpsMonitorId ) );
        
    if ( !self )
        {
        //Construct a new object and store it to CPosSingletonManager
        self = new ( ELeave ) CPosExternalGpsMonitor(
            aModuleStatusManager );
        CleanupStack::PushL(self);
        self->ConstructL();
        CPosSingletonManager::SetObjectL(
            self,
            EPosSigletonObjectExtGpsMonitorId );
        self->iDefaultPositionerArray.AppendL( &aDefaultPositioner );
        self->ConstructExtGpsPsyArrayL();
        CleanupStack::Pop(self);
        }
    else
        {
        self->iDefaultPositionerArray.AppendL( &aDefaultPositioner );
        }
        
    self->iRefCount++;
    
    return self;
    }

// Destructor
CPosExternalGpsMonitor::~CPosExternalGpsMonitor()
    {
    TRACESTRING( "CPosExternalGpsMonitor::destructor start" )

    if ( iPsyListHandler )
        {
        iPsyListHandler->RemoveListener( this );
        iPsyListHandler->ReleaseInstance();
        }
        
    if ( iFixStateManager )
        {
        iFixStateManager->ReleaseInstance();
        }
        
    if ( iConstManager )
        {
        iConstManager->ReleaseInstance();
        }
    
    iExtGpsPsyArray.ResetAndDestroy();
    iExtGpsPsyArray.Close();
    iExtGpsPsyUsedArray.Close();
    iDefaultPositionerArray.Close();
    delete iTimer;
    TRACESTRING( "CPosExternalGpsMonitor::destructor end" )
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::ReleaseInstance
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::ReleaseInstance(
        CPositioner& aDefaultPositioner )
    {
    TInt index = iDefaultPositionerArray.Find( &aDefaultPositioner );
    if ( index != KErrNotFound )
        {
        iDefaultPositionerArray.Remove( index );
        }
        
    iRefCount--;
    if ( iRefCount == 0 )
        {
        //We shall delete the instance
        CPosSingletonManager::ReleaseObject(
            EPosSigletonObjectExtGpsMonitorId );
        }
    else
		{
		// Delete & reconstruct iExtGpsPsyArray
		iExtGpsPsyArray.ResetAndDestroy();
		iExtGpsPsyArray.Close();  
				
		TInt ignore = KErrNone;
		TRAP( ignore, ConstructExtGpsPsyArrayL() ); //error ignored
		}		    
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::PsyUsed
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::PsyUsed( TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosExternalGpsMonitor::PsyUsed" )
    TRACESTRING2( "PSY: %x", aPsyId );

    TInt index = GetIndexInPsyArray( aPsyId );
    //iExtGpsPsyArray and iExtGpsPsyUsedArray are saved in the same
    //order. 
    if ( index != KErrNotFound )
        {
        iExtGpsPsyUsedArray[index]++;
        }
        
    //Location request received
    iIsLoacationRequestMade = ETrue;
    
    //Update state of Ext GPS PSY used
    iIsExtGpsPsyUsed = IsExtGpsPsyInUse();
        
    //Check if we should start timer
    if ( iExtGpsPsyArray.Count() > 0 &&
        !iPsyListHandler->IsFirstGpsPsyExternal() && 
        !IsExtGpsPsyInUse() )
        {
        //We start timer if there are GPS PSY availabe, but
        //the first GPS PSY in the list is not located on external
        //device. 
        //If a external GPS PSY is already used, then we don't
        //start the timer
        
        //Start timer if it's not started yet
        StartTimer();
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::PsyNotUsed
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::PsyNotUsed( TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosExternalGpsMonitor::PsyNotUsed" )
    TRACESTRING2( "PSY: %x", aPsyId );

    TInt index = GetIndexInPsyArray( aPsyId );
    //iExtGpsPsyArray and iExtGpsPsyUsedArray are saved in the same
    //order. 
    if ( index != KErrNotFound )
        {
        if ( iExtGpsPsyUsedArray[index] > 0 )
            {
            iExtGpsPsyUsedArray[index]--;
            }
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::ConstructExtGpsPsyArrayL
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::ConstructExtGpsPsyArrayL()
    {
    TRACESTRING( "CPosExternalGpsMonitor::ConstructExtGpsPsyArrayL" )

    RArray< TPositionModuleId > extGpsPsyIdArray;
    CleanupClosePushL( extGpsPsyIdArray );
    iPsyListHandler->GetExtGpsPsyListL( extGpsPsyIdArray );
    
    TInt i;
    
    //Remove those modules that are not in the settings
    TInt positionerCount = iExtGpsPsyArray.Count();
    
    //Reserve enough space for iExtGpsPsyUsedArray first
    iExtGpsPsyUsedArray.ReserveL( positionerCount );
    
    for ( i = positionerCount-1; i >= 0 ; i-- )
        {
        TInt index = extGpsPsyIdArray.Find(
            iExtGpsPsyArray[i]->ModuleId() );
        if ( index == KErrNotFound )
            {
            delete iExtGpsPsyArray[i];
            iExtGpsPsyArray.Remove( i );
            iExtGpsPsyUsedArray.Remove( i );
            }
        else
            {
            extGpsPsyIdArray.Remove( index );
            }
        }
        
    //Add those PSYs that are not in the PSY array        
    TInt count = extGpsPsyIdArray.Count();
    
    if ( iDefaultPositionerArray.Count() > 0 )
        {
        for ( i = 0; i < count; i++ )
            {
            //Add this module to the list
            CPosRequestor* requestor = CPosRequestor::NewL(
                *iDefaultPositionerArray[0],
                extGpsPsyIdArray[i],
                *this,
                *iFixStateManager,
                iModuleStatusManager );
            CleanupStack::PushL( requestor );
            
            //Notify fix state manager that a PSY is loaded
            iFixStateManager->PsyLoadedL( 
                extGpsPsyIdArray[i] );
                
            //Add this requestor in the array
            User::LeaveIfError( iExtGpsPsyArray.Append( requestor ) );
            
            //This function won't fail, since we have enough space already
            iExtGpsPsyUsedArray.AppendL( 0 ); 
            
            CleanupStack::Pop( requestor );
            }
        }
    CleanupStack::PopAndDestroy( &extGpsPsyIdArray );
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::LocationRequestCompleted
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::LocationRequestCompleted( 
            TPositionModuleId aModuleId,
            TInt aErr, 
            const TPositionInfoBase& /*aPosInfo*/,
            TBool /*aIsPosInfoUpToDate*/ )
    {
    TRACESTRING( "CPosExternalGpsMonitor::LocationRequestCompleted" )
    TRACESTRING2( "PSY: %x", aModuleId )

    iFixStateManager->SetExtGpsPsyFixState( 
        aModuleId,
        aErr );
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::ExtPsyCheckingTimeoutCallback
// ---------------------------------------------------------
//
TInt CPosExternalGpsMonitor::ExtPsyCheckingTimeoutCallback( TAny* aAny )
    {
    TRACESTRING( "CPosExternalGpsMonitor::ExtPsyCheckingTimeoutCallback" )

    reinterpret_cast < CPosExternalGpsMonitor* > ( aAny ) ->
        ExtPsyCheckingTimeout();
    return KErrNone;
    }
        
// ---------------------------------------------------------
// CPosExternalGpsMonitor::ExtPsyCheckingTimeout
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::ExtPsyCheckingTimeout()
    {
    if ( !iIsLoacationRequestMade )
        {
        //Stop timer and cancel all location requests
        iTimer->Cancel();
        CancelAllLocationRequests();
        }
    else
        {
        //If there is location request during this period, but 
        //external GPS has not been tried, then we make location
        //request to external GPS PSYs.
        if ( !iIsExtGpsPsyUsed )
            {
            MakeLocationRequests();
            }
        else
            {
            //If external GPS PSY has been used during this period,
            //We don't need to make location request. All outgoing
            //location request will be canceled.
            CancelAllLocationRequests();
            }
        }
    iIsLoacationRequestMade = EFalse;
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::PsyListChanged
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::PsyListChanged( 
            const TPosPsyListChangeEvent& aEvent )
    {
    TRACESTRING( "CPosExternalGpsMonitor::PsyListChanged" )

    if ( aEvent.iType == EPosPsyListChangeEventPriorityChanged )
        {
        //do nothing
        return;
        }
        
    if ( aEvent.iType == EPosPsyListChangeEventPsyDeleted )
        {
        CancelLocationRequest( aEvent.iPsyId );
        }
        
    //Reconstruct Ext GPS PSY array
    TInt ignore = KErrNone;
    TRAP( ignore, ConstructExtGpsPsyArrayL() ); //error ignored
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::MakeLocationRequests
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::MakeLocationRequests()
    {
    TRACESTRING( "CPosExternalGpsMonitor::MakeLocationRequests" )

    TInt count = iExtGpsPsyArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        //If location request is not started
        if ( !iExtGpsPsyArray[i]->IsActive() )
            {
            TInt ignore = KErrNone;
            TRAP( ignore, 
                iExtGpsPsyArray[i]->MakeLocationRequestL(iPosInfo) );
            TRACESTRING2( "PSY: %x", iExtGpsPsyArray[i]->ModuleId() )
            }
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::CancelAllLocationRequests
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::CancelAllLocationRequests()
    {
    TInt count = iExtGpsPsyArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iExtGpsPsyArray[i]->Cancel();
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::CancelLocationRequest
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::CancelLocationRequest( TPositionModuleId aPsyId )
    {
    TInt count = iExtGpsPsyArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iExtGpsPsyArray[i]->ModuleId() == aPsyId )
            {
            iExtGpsPsyArray[i]->Cancel();
            }
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::GetIndexInPsyArray
// ---------------------------------------------------------
//
TInt CPosExternalGpsMonitor::GetIndexInPsyArray( TPositionModuleId aModuleId ) const
    {
    TInt count = iExtGpsPsyArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iExtGpsPsyArray[i]->ModuleId() == aModuleId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::StartTimer
// ---------------------------------------------------------
//
void CPosExternalGpsMonitor::StartTimer()
    {
    if ( !iTimer->IsActive() )
        {
        iTimer->Start(
            iConstManager->GetExternalGpsCheckingTimeoutValue().Int64(),
            iConstManager->GetExternalGpsCheckingTimeoutValue().Int64(),
            TCallBack( ExtPsyCheckingTimeoutCallback, this ) );
            
        iIsLoacationRequestMade = EFalse;
        iIsExtGpsPsyUsed = IsExtGpsPsyInUse();
        }
    }

// ---------------------------------------------------------
// CPosExternalGpsMonitor::IsExtGpsPsyInUse
// ---------------------------------------------------------
//
TBool CPosExternalGpsMonitor::IsExtGpsPsyInUse() const
    {
    TInt count = iExtGpsPsyUsedArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iExtGpsPsyUsedArray[i] > 0 )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//  End of File
