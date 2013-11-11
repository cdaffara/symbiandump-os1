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
#include <ecom/ecom.h>

#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cpositioner.h>
#include <lbssatellite.h>
#include "epos_mposmodulestatusmanager.h"

#include "EPos_CPosRequestController.h"
#include "epos_cposrequestor.h"
#include "epos_defaultproxycommon.h"
#include "epos_cpospsylisthandler.h"
#include "epos_cpospsyfixstatemanager.h"
#include "epos_cposconstmanager.h"
#include "epos_cposexternalgpsmonitor.h"
#include "epos_posgenericinfouser.h"


// ================= LOCAL FUNCTIONS =======================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosRequestController::CPosRequestController(
    CPosDefaultPositioner& aDefaultPositioner, 
    MPosModuleStatusManager& aModuleStatusManager)
    :
    iDefaultPositioner( aDefaultPositioner ),
    iModuleStatusManager( aModuleStatusManager ),
    iCurrentPsy( KErrNotFound )
    {
    }

// EPOC default constructor can leave.
void CPosRequestController::ConstructL(
        MPosModuleSettingsManager& aSettingsManager )
    {
    TRACESTRING( "CPosRequestController::ConstructL start... " )
    
    iPsyListHandler = CPosPsyListHandler::GetInstanceL();
    iPsyListHandler->SetModuleSettingsManagerL( 
        aSettingsManager );
    iPsyListHandler->AddListenerL( this );

    
    iPsyFixStateManager = CPosPsyFixStateManager::GetInstanceL();
    iPsyFixStateManager->AddListenerL( this );
    iConstManager = CPosConstManager::GetInstanceL();

    iExtGpsPsyMonitor = CPosExternalGpsMonitor::GetInstanceL( 
        iDefaultPositioner,
        iModuleStatusManager );

    //Construct cleanup timer
    iCleanupTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    TRACESTRING( "CPosRequestController::ConstructL end " )
    
    }

// Two-phased constructor.
CPosRequestController* CPosRequestController::NewL(
    CPosDefaultPositioner& aDefaultPositioner,
    MPosModuleSettingsManager& aSettingsManager,
    MPosModuleStatusManager& aModuleStatusManager )
    {
    CPosRequestController* self = new (ELeave)
        CPosRequestController(
            aDefaultPositioner,
            aModuleStatusManager );

    CleanupStack::PushL(self);
    self->ConstructL( 
        aSettingsManager );
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosRequestController::~CPosRequestController()
    {
    TRACESTRING( "CPosRequestController::destructor start... " )
    
    if ( iPsyFixStateManager )
        {
        iPsyFixStateManager->RemoveListener( this );
        iPsyFixStateManager->ReleaseInstance();
        }
    
    if ( iPsyListHandler )
        {
        iPsyListHandler->RemoveListener( this );
        iPsyListHandler->ReleaseInstance();
        }
        
    if ( iConstManager )
        {
        iConstManager->ReleaseInstance();
        }
        
    if ( iExtGpsPsyMonitor )
        {
        iExtGpsPsyMonitor->ReleaseInstance(
            iDefaultPositioner );
        }

    // Close all requestors 
    iRequestorArray.ResetAndDestroy();
    iRequestorArray.Close();
    
    //Close PSY List
    iPsyList.Close();
    
    // Cleanup timer
    delete iCleanupTimer;

    TRACESTRING( "CPosRequestController::destructor end... " )

    }

// ---------------------------------------------------------
// CPosRequestController::NotifyPositionUpdate
// ---------------------------------------------------------
//
void CPosRequestController::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    TRACESTRING( "CPosRequestController::NotifyPositionUpdate start... " )

    //Default Proxy can't handle simultaneous location request.
    __ASSERT_DEBUG( 
        iPosRequestStatus == NULL, 
        DefaultProxyPanic( EDefaultProxyPanic_SimualtaneousLR ) );
    
    aStatus = KRequestPending;
    iPosRequestStatus = &aStatus;
    iPosInfo = &aPosInfo;
    
    //When location request received, clear cleanup timer
    iCleanupTimer->Cancel();
    
    TRAPD( err, StartPositionUpdateL() );
    if( err != KErrNone )
        {
        CompleteRequest(err);
        }

    TRACESTRING( "CPosRequestController::NotifyPositionUpdate end " )
    }

// ---------------------------------------------------------
// CPosRequestController::CancelNotifyPositionUpdate
// ---------------------------------------------------------
//
void CPosRequestController::CancelNotifyPositionUpdate(TInt aCancelReason)
    {
    TRACESTRING( "CPosRequestController::CancelNotifyPositionUpdate(TInt) start... " )

    //Cancel location request with error to all loaded requestor
    TInt loadedRequestorCount = iRequestorArray.Count();
    for ( TInt i = 0; i < loadedRequestorCount; i++ )
        {
        CancelRequest( i, aCancelReason );
        }
        
    CompleteRequest(KErrCancel);

    TRACESTRING( "CPosRequestController::CancelNotifyPositionUpdate(TInt) end " )
    }

// ---------------------------------------------------------
// CPosRequestController::CancelNotifyPositionUpdate
// ---------------------------------------------------------
//
void CPosRequestController::CancelNotifyPositionUpdate()
    {
    TRACESTRING( "CPosRequestController::CancelNotifyPositionUpdate start... " )

    // This is a real user cancel
    CancelNotifyPositionUpdate(KErrCancel);
    
    TRACESTRING( "CPosRequestController::CancelNotifyPositionUpdate end " )
    }

// ---------------------------------------------------------
// CPosRequestController::CancelRequest
// ---------------------------------------------------------
//
void CPosRequestController::CancelRequest( TInt aIndex, TInt aCancelReason )
    {
    TInt count = iRequestorArray.Count();
    if ( aIndex < count )
        {
        CPosRequestor* requestor = iRequestorArray[aIndex];
        if ( requestor->IsActive() )
            {
            //Notify external GPS PSY monitor that this PSY is not used.
            iExtGpsPsyMonitor->PsyNotUsed( requestor->ModuleId() );
            requestor->CancelWithReason(aCancelReason);
            }
        }
    }

// ---------------------------------------------------------
// CPosRequestController::CancelRequest
// ---------------------------------------------------------
//
void CPosRequestController::CancelRequest( TPositionModuleId aPsyId )
    {
    TInt count = iRequestorArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CPosRequestor* requestor = iRequestorArray[i];
        if ( requestor->ModuleId() == aPsyId && 
            requestor->IsActive() )
            {
            //Notify external GPS PSY monitor that this PSY is not used.
            iExtGpsPsyMonitor->PsyNotUsed( requestor->ModuleId() );
            requestor->Cancel();
            }
        }
    }

// ---------------------------------------------------------
// CPosRequestController::StartTrackingL
// ---------------------------------------------------------
//
void CPosRequestController::StartTracking(
    const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    //This function does nothing. Tracking will be started
    //to any specific PSY only if this PSY will be used by
    //Default Proxy. 

    TRACESTRING( "CPosRequestController::StartTracking" )

    }

// ---------------------------------------------------------
// CPosRequestController::StopTracking
// ---------------------------------------------------------
//
void CPosRequestController::StopTracking()
    {
    TRACESTRING( "CPosRequestController::StopTracking start..." )
    
    //Stop tracking to all loaded PSYs
    TInt loadedRequestorCount = iRequestorArray.Count();
    for ( TInt i = 0; i < loadedRequestorCount; i++ )
        {
        iRequestorArray[i]->StopTracking();
        }

    TRACESTRING( "CPosRequestController::StopTracking end" )

    }

// ---------------------------------------------------------
// CPosRequestController::StartPositionUpdateL
// ---------------------------------------------------------
//
void CPosRequestController::StartPositionUpdateL()
    {
    TRACESTRING( "CPosRequestController::StartPositionUpdateL start..." )

    //Rebuid PSY list if neccessary
    if ( !iPsyListValid )
        {
        iPsyListHandler->GetPsyListL( iPsyList );
        iPsyListValid = ETrue;
        }
        
    iFirstResult = KErrNotFound;    // as if no enabled psys found
    iCurrentPsy = KErrNotFound;     // start from first module
    TryNextPositioner();
    
    if ( !IsLocationRequestOnGoing() )
        {
        CompleteRequest( iFirstResult );
        }

    TRACESTRING( "CPosRequestController::StartPositionUpdateL end" )

    }


// ---------------------------------------------------------
// CPosRequestController::TryNextPositioner
// ---------------------------------------------------------
//
void CPosRequestController::TryNextPositioner()
{
    TRACESTRING( "CPosRequestController::TryNextPositioner start..." )

    // Find next enabled plugin we should try with
    TInt count = iPsyList.Count();
    while ( ++iCurrentPsy < count )
        {
        TPositionModuleId currentPsyId = iPsyList[iCurrentPsy];
        
        TRACESTRING2( "PSY: %x", currentPsyId )

        TUint32 classType = iPosInfo->PositionClassType();
        if ( !iPsyListHandler->IsClassSupported( 
            classType,
            currentPsyId ) )
            {
            //if the requested class is not supported, then we try next PSY
            //in the PSY list
            
            if ( iCurrentPsy == 0 )
                {
                //Error code fromt the first PSY
                iFirstResult = KErrArgument;
                }

            continue;
            }
            
        TRAPD( err, TryPositionerL( currentPsyId ) );
        
        TRACESTRING2( "Try PSY completion code = %d", err )

        if( err == KErrNone )
            {
            //Location request is made to currentPsy. Check the fix state of
            //current PSY, if current PSY can't give a fix, we load next PSY
            if ( iPsyFixStateManager->GetPsyFixState( currentPsyId ) 
                != CPosPsyFixStateManager::EPsyFixStateNo )
                {
                //break from here. Othsewise, try next PSY
                break;
                }
            }
        else
            {
            //In error case, we store the error code if needed and try next PSY
            if ( iCurrentPsy == 0 )
                {
                //Error code fromt the first PSY
                iFirstResult = err;
                }
            }
        }

    TRACESTRING( "CPosRequestController::TryNextPositioner end" )

}

// ---------------------------------------------------------
// CPosRequestController::TryPositionerL
// ---------------------------------------------------------
//
void CPosRequestController::TryPositionerL( TPositionModuleId aPsyId )
    {
    CPosRequestor* currentRequestor = NULL;
    
    
    //Check device status of the PSY. If it's in error status,
    //it will not be used at all.
    TPositionModuleStatus moduleStatus;
    iModuleStatusManager.GetModuleStatus( 
        aPsyId,
        moduleStatus );
    if ( moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceError )
        {
        TRACESTRING2( "PSY %x device error", aPsyId )

        User::Leave( KErrGeneral );
        }
    
    //Find if the PSY is already loaded
    TInt requestorCount = iRequestorArray.Count();
    for ( TInt i = 0; i < requestorCount; i++ )
        {
        if ( iRequestorArray[i]->ModuleId() == aPsyId )
            {
            currentRequestor = iRequestorArray[i];
            break;
            }
        }
    
    if ( currentRequestor == NULL )
        {
        //PSY has not been loaded before, load it now
        TRACESTRING2( "Loading Positioner: %x", aPsyId )
        currentRequestor = 
            CPosRequestor::NewL(
                iDefaultPositioner,
                aPsyId,
                *this,
                *iPsyFixStateManager,
                iModuleStatusManager );
                
        CleanupStack::PushL( currentRequestor );
        
        //Notify fix state manager that a PSY is loaded
        iPsyFixStateManager->PsyLoadedL( aPsyId );
        
        //Add this requestor to requestor array
        User::LeaveIfError( iRequestorArray.Append( 
            currentRequestor ) );
        CleanupStack::Pop( currentRequestor );
        }
        
    //Start tracking if needed
    TTimeIntervalMicroSeconds interval = iDefaultPositioner.TrackingInterval();
    if (interval.Int64() != 0 )
        {
        currentRequestor->StartTrackingL( interval );
        }
    
    //Make location request to current positioner
    currentRequestor->MakeLocationRequestL( *iPosInfo );
    
    //Notify external GPS PSY monitor that a PSY is used
    iExtGpsPsyMonitor->PsyUsed( aPsyId );
    }

// ---------------------------------------------------------
// CPosRequestController::LocationRequestCompleted
// ---------------------------------------------------------
//
void CPosRequestController::LocationRequestCompleted( 
            TPositionModuleId aModuleId,
            TInt aErr,
            const TPositionInfoBase& aPosInfo,
            TBool aIsPosInfoUpToDate )
    {
    TRACESTRING( "CPosRequestController::LocationRequestCompleted start..." )
    TRACESTRING2( "PSY: %x", aModuleId )
    TRACESTRING2( "Err: %d", aErr )
    TRACESTRING2( "Is PosInfo Updated: %d", aIsPosInfoUpToDate )

    //Notify the external GPS PSY monitor that a PSY in not used
    iExtGpsPsyMonitor->PsyNotUsed( aModuleId );

    TInt err = aErr;
    if ( aErr == KPositionPartialUpdate && IsLocationRequestOnGoingOnNetworkPsy() )
        {
        //Partial update is returned when Network PSY is used.
        //partial update is not forwarded to system. Instead, location
        //request is made to the loaded PSY again.
        TInt ignore;
        TRAP( ignore, TryPositionerL( aModuleId ) );
        }
    else if ( aErr == KErrNone || 
        aErr == KErrPositionBufferOverflow ||
        aErr == KPositionPartialUpdate  )
        {
        //Location request succeed or
        //buffer over flow is returned from a PSY or
        //partial update is returned when only GPS PSY is used
        
        //Copy request info, and complete LR
        if ( aIsPosInfoUpToDate )
            {
            __ASSERT_DEBUG( 
                aPosInfo.PositionClassSize() == iPosInfo->PositionClassSize(),
                DefaultProxyPanic( EDefaultProxyPanic_PosInfoSizeMismatch ) );
            
            Mem::Copy( iPosInfo, &aPosInfo, iPosInfo->PositionClassSize() );
            }
        else
            {
            TInt cpErr = CopyPosInfoClass( aPosInfo , *iPosInfo );
            if ( cpErr != KErrNone && err == KErrNone )
                {
                err = cpErr;
                }
            }
        
        if ( iPosInfo->ModuleId() != aModuleId )
            {
            err = KErrGeneral;
            }
            
        CompleteRequest( err );
        }
    else
        {
        if ( aModuleId == iPsyList[0] )
            {
            //Store the result from first PSY
            iFirstResult = aErr;
            
            //Store module ID of first positioner
            iPosInfo->SetModuleId( aModuleId );
            }
        }
        
    //This will cause the state change notification and generate fallback 
    //if it's a error case.
    iPsyFixStateManager->SetPsyFixState( aModuleId, aErr );
    
    if ( !IsLocationRequestOnGoing() )
        {
        CompleteRequest( iFirstResult );
        }

    TRACESTRING( "CPosRequestController::LocationRequestCompleted end" )
    }

// ---------------------------------------------------------
// CPosRequestController::IsLocationRequestOnGoing
// ---------------------------------------------------------
//
TBool CPosRequestController::IsLocationRequestOnGoing() const
    {
    TInt count = iRequestorArray.Count();
    for ( TInt i=0; i < count; i++ )
        {
        const CPosRequestor& requestor = *(iRequestorArray[i]);
        if ( requestor.IsActive() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CPosRequestController::IsLocationRequestOnGoingOnNetworkPsy
// ---------------------------------------------------------
//
TBool CPosRequestController::IsLocationRequestOnGoingOnNetworkPsy() const
    {
    TInt count = iRequestorArray.Count();
    for ( TInt i=0; i < count; i++ )
        {
        const CPosRequestor& requestor = *(iRequestorArray[i]);
        if ( requestor.IsActive() && 
            iPsyListHandler->IsModuleNetworkBased(
                requestor.ModuleId() ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CPosRequestController::PsyFixStateChanged
// ---------------------------------------------------------
//
void CPosRequestController::PsyFixStateChanged( 
            TPositionModuleId aModuleId,
            CPosPsyFixStateManager::TPsyFixState aFixState )
    {
    TRACESTRING( "CPosRequestController::PsyFixStateChanged start..." )
    TRACESTRING2( "PSY: %x", aModuleId )
    TRACESTRING2( "Fix state: %d", aFixState )

    //If there is location request on going and
    //current can't give a fix, we try next PSY
    if( iPosRequestStatus && iCurrentPsy<iPsyList.Count() )
        {
        if ( aFixState == CPosPsyFixStateManager::EPsyFixStateNo &&
            iPsyList[iCurrentPsy] == aModuleId )
            {
            TryNextPositioner();
            }
        }

    TRACESTRING( "CPosRequestController::PsyFixStateChanged end" )
    }

// ---------------------------------------------------------
// CPosRequestController::CompleteRequest
// ---------------------------------------------------------
//
void CPosRequestController::CompleteRequest(TInt aCompleteCode)
    {
    TRACESTRING( "CPosRequestController::CompleteRequest start..." )
    TRACESTRING2( "Completion code: %d", aCompleteCode )
    TRACESTRING2( "Module Id: %x", iPosInfo->ModuleId() )

    if (iPosRequestStatus)
        {
        User::RequestComplete(iPosRequestStatus, aCompleteCode);
        iPosRequestStatus = NULL;
        
        //Clear all location request if this is not 
        //a partial update
        if ( aCompleteCode != KPositionPartialUpdate )
            {
            ClearLocationRequests();
            }
            
        //If there is still location request ongoing to other PSYs,
        //start cleanup timer
        TInt count = iRequestorArray.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iRequestorArray[i]->IsActive() )
                {
                
                if ( !iCleanupTimer->IsActive() )
                    {
                    iCleanupTimer->Start(
                        iConstManager->GetCleanupTimeoutValue().Int64(),
                        iConstManager->GetCleanupTimeoutValue().Int64(),
                        TCallBack( CleanupTimeoutCallback, this )
                        );
                    }
                }
            }
        }
    TRACESTRING( "CPosRequestController::CompleteRequest end" )
    }

// ---------------------------------------------------------
// CPosRequestController::ClearLocationRequests
// ---------------------------------------------------------
//
void CPosRequestController::ClearLocationRequests()
    {
    TInt count = iRequestorArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CancelRequest( i, KErrCancel);
        }
    }

// ---------------------------------------------------------
// CPosRequestController::PsyListChanged
// ---------------------------------------------------------
//
void CPosRequestController::PsyListChanged( 
            const TPosPsyListChangeEvent& aEvent )
    {
    TRACESTRING( "CPosRequestController::PsyListChanged start..." )
    TRACESTRING2( "Event type: %d", aEvent.iType )
    TRACESTRING2( "PSY: %x", aEvent.iPsyId )
    
    switch ( aEvent.iType )
        {
        case EPosPsyListChangeEventPsyDeleted:
            {
            //Cancel location request to this PSY
            CancelRequest( aEvent.iPsyId );
                    
            if ( iCurrentPsy >= 0 && iCurrentPsy < iPsyList.Count() )
                {
                //If there is location request on going
                if ( aEvent.iPsyId == iPsyList[iCurrentPsy] )
                    {
                    //fallback to next PSY
                    TryNextPositioner();
                    }
                }
            //Delete this PSY from the list
            TInt index = iPsyList.Find( aEvent.iPsyId );
            if ( index != KErrNotFound )
                {
                iPsyList.Remove( index );
                }
                
            //Unload this PSY
            UnloadRequestor( aEvent.iPsyId );
            }
            break;
        case EPosPsyListChangeEventListRebuild:
        default:
            //Rebuild the list
            iPsyListValid = EFalse;
            break;
        }

    TRACESTRING( "CPosRequestController::PsyListChanged end" )
    }


// ---------------------------------------------------------
// CPosRequestController::GetRequestor
// ---------------------------------------------------------
//
CPosRequestor* CPosRequestController::GetRequestor(
            TPositionModuleId aPsyId )
    {
    TInt count = iRequestorArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iRequestorArray[i]->ModuleId() == aPsyId )
            {
            return iRequestorArray[i];
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CPosRequestController::UnloadRequestor
// ---------------------------------------------------------
//
void CPosRequestController::UnloadRequestor( TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosRequestController::UnloadRequestor" )
    TRACESTRING2( "PSY: %x", aPsyId )

    TInt count = iRequestorArray.Count();
    for ( TInt i = count-1; i >= 0; i-- )
        {
        if ( iRequestorArray[i]->ModuleId() == aPsyId )
            {
            delete iRequestorArray[i];
            iRequestorArray.Remove( i );
            }
        }
    }

// ---------------------------------------------------------
// CPosRequestController::CopyPosInfoClass
// ---------------------------------------------------------
//
TInt CPosRequestController::CopyPosInfoClass(
            const TPositionInfoBase& aSrc,
            TPositionInfoBase& aDst )
    {
    TInt err = KErrNone;
    
    TUint32 srcClasses = aSrc.PositionClassType();
    TUint32 dstClasses = aDst.PositionClassType();
    
    //Handle TPositionInfoBase
    aDst.SetModuleId( aSrc.ModuleId() );
    aDst.SetUpdateType( aSrc.UpdateType() );
    
    //Handle TPositionInfo
    if ( ( srcClasses & EPositionInfoClass ) &&
        ( dstClasses & EPositionInfoClass ) )
        {
        TPosition pos;
        static_cast < const TPositionInfo& > ( aSrc ).GetPosition( pos );
        static_cast < TPositionInfo& > ( aDst ).SetPosition( pos );
        }
        
    //Handle TPositionCourseInfo
    if ( ( srcClasses & EPositionCourseInfoClass ) &&
        ( dstClasses & EPositionCourseInfoClass ) )
        {
        TCourse course;
        static_cast < const TPositionCourseInfo& > ( aSrc ).GetCourse( course );
        static_cast < TPositionCourseInfo& > ( aDst ).SetCourse( course );
        }
        
    //Handle TPositionSatelliteInfo
    if ( ( srcClasses & EPositionSatelliteInfoClass ) &&
        ( dstClasses & EPositionSatelliteInfoClass ) )
        {
        const TPositionSatelliteInfo& srcSat = 
            static_cast < const TPositionSatelliteInfo& > ( aSrc );
        TPositionSatelliteInfo& dstSat = 
            static_cast < TPositionSatelliteInfo& > ( aDst );

        dstSat.SetSatelliteTime( srcSat.SatelliteTime() );
        dstSat.SetHorizontalDoP( srcSat.HorizontalDoP() );
        dstSat.SetTimeDoP( srcSat.TimeDoP() );
        dstSat.SetVerticalDoP( srcSat.VerticalDoP() );
        
        dstSat.ClearSatellitesInView();
        TInt numSatData = srcSat.NumSatellitesInView();
        
        for ( TInt i = 0; i < numSatData; i++ )
            {
            TSatelliteData satData;
            srcSat.GetSatelliteData( i, satData );
            err = dstSat.AppendSatelliteData( satData );
            if ( err != KErrNone )
                {
                return KErrNone;
                }
            }
        }
        
    //Handle HPositionGenericInfo
    if ( ( srcClasses & EPositionGenericInfoClass ) &&
        ( dstClasses & EPositionGenericInfoClass ) )
        {
        const HPositionGenericInfo& srcGen = 
            static_cast < const HPositionGenericInfo& > ( aSrc );
        HPositionGenericInfo& dstGen = 
            static_cast < HPositionGenericInfo& > ( aDst );
        
        err = PosGenericInfoUser::CopyHGenericInfo( srcGen, dstGen );
        }
    
    return err;
    }

// ---------------------------------------------------------
// CPosRequestController::CleanupTimeoutCallback
// ---------------------------------------------------------
//
TInt CPosRequestController::CleanupTimeoutCallback( TAny* aAny )
    {
    reinterpret_cast< CPosRequestController* > ( aAny ) ->
        CleanupTimeout();
    return KErrNone;
    }
        
// ---------------------------------------------------------
// CPosRequestController::CleanupTimeout
// ---------------------------------------------------------
//
void CPosRequestController::CleanupTimeout()
    {
    //Cleanup all outstanding location requests
    ClearLocationRequests();
    iCleanupTimer->Cancel();
    }

// End of file
