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
#include <centralrepository.h>

#include "epos_cposconstmanager.h"
#include "epos_cpossingletonmanager.h"
#include "epos_defaultproxyprivatecrkeys.h"
#include "epos_defaultproxycommon.h"


//Define the const for calculating the timeshift value
const TInt KPosTimeshiftValueMultiplier = 2;

//Maximum length of last working GPS PSY UID in CenRep
const TInt KPosMaximumLastWorkingGpsPsyUidStringLength = 24;

//The width of each PSY uid in the PSY list
const TInt KPosPsyUidWidth = 8;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosConstManager::CPosConstManager()
    {
    iLastWorkingGpsPsyId = KPositionNullModuleId;
    }

// EPOC default constructor can leave.
void CPosConstManager::ConstructL()
    {
    //Central repository
    iRepository = CRepository::NewL( KCRUidDefaultProxyConfiguration );
    
    //Get constant value
    User::LeaveIfError( iRepository->Get(
        KDefaultProxyTimeshiftInActiveOrReady, 
        iMaxTimeshiftInActiveOrReady ) );
        
    User::LeaveIfError( iRepository->Get(
        KDefaultProxyTimeshiftNotInActiveOrReady,
        iMaxTimeshiftNotInActiveOrReady ) );
        
    User::LeaveIfError( iRepository->Get(
        KDefaultProxyPsyStateUnknownTimeout, 
        iPsyStateUnknownTimeout ) );
        
    User::LeaveIfError( iRepository->Get(
        KDefaultProxyExternalGPSCheckingTimeout, 
        iExternalGpsCheckingTimeout ) );
        
    User::LeaveIfError( iRepository->Get(
        KDefaultProxyCleanupTimeout, 
        iCleanupTimeout ) );
    
    }

// ---------------------------------------------------------
// CPosConstManager::GetIntanceL
// ---------------------------------------------------------
//
CPosConstManager* CPosConstManager::GetInstanceL()
    {
    CPosConstManager* self = reinterpret_cast < CPosConstManager* > 
        ( CPosSingletonManager::GetObject( 
            EPosSigletonObjectConstManagerId ) );
        
    if ( !self )
        {
        //Construct a new object and store it to CPosSingletonManager
        self = new ( ELeave ) CPosConstManager;
        CleanupStack::PushL(self);
        self->ConstructL();
        CPosSingletonManager::SetObjectL(
            self,
            EPosSigletonObjectConstManagerId );
        CleanupStack::Pop(self);
        }
        
    self->iRefCount++;
    
    return self;
    }

// Destructor
CPosConstManager::~CPosConstManager()
    {
    delete iRepository;
    }

// ---------------------------------------------------------
// CPosConstManager::ReleaseInstance
// ---------------------------------------------------------
//
void CPosConstManager::ReleaseInstance()
    {
    iRefCount--;
    if ( iRefCount == 0 )
        {
        //We shall delete the instance
        CPosSingletonManager::ReleaseObject(
            EPosSigletonObjectConstManagerId );
        }
    }

// ---------------------------------------------------------
// CPosConstManager::GetTimeshiftValue
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds CPosConstManager::GetTimeshiftValue( 
            TTimeIntervalMicroSeconds aTtff, 
            TTimeIntervalMicroSeconds aTtnf, 
            TPositionModuleStatus::TDeviceStatus aDeviceStatus )
    {
    switch ( aDeviceStatus )
        {
        case TPositionModuleStatus::EDeviceActive:
            return Min( 
                KPosTimeshiftValueMultiplier*aTtnf.Int64(), 
                iMaxTimeshiftInActiveOrReady );
        default:
            return Min( aTtff, iMaxTimeshiftNotInActiveOrReady );
        }
    }
            
// ---------------------------------------------------------
// CPosConstManager::GetUnusedTimeoutValue
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds  CPosConstManager::GetUnusedTimeoutValue() const
    {
    return iPsyStateUnknownTimeout;
    }
    
// ---------------------------------------------------------
// CPosConstManager::GetExternalGpsCheckingTimeoutValue
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds  CPosConstManager::GetExternalGpsCheckingTimeoutValue() const
    {
    return iExternalGpsCheckingTimeout;
    }
        
// ---------------------------------------------------------
// CPosConstManager::GetCleanupTimeoutValue
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds CPosConstManager::GetCleanupTimeoutValue() const
    {
    return iCleanupTimeout;
    }

// ---------------------------------------------------------
// CPosConstManager::GetLastWorkingGpsPsyId
// ---------------------------------------------------------
//
TPositionModuleId CPosConstManager::GetLastWorkingGpsPsyId()
    {
    if ( iLastWorkingGpsPsyId != KPositionNullModuleId )
        {
        return iLastWorkingGpsPsyId;
        }
        
    TPositionModuleId id = KPositionNullModuleId;
    TBuf< KPosMaximumLastWorkingGpsPsyUidStringLength > idBuf;
    TInt err = iRepository->Get( 
        KDefaultProxyLastWorkingGpsPsy,
        idBuf );
    TLex lex ( idBuf );
    lex.SkipSpace();
    TUint32 psyUid;
    if ( err == KErrNone )
        {
        err = lex.Val( psyUid , EHex );
        if ( err == KErrNone )
            {
            id = TUid::Uid( psyUid );
            }
        }

    TRACESTRING2( "GetLastWorkingGpsPsyId Id: %x", id)

    iLastWorkingGpsPsyId = KPositionNullModuleId;
    
    return id;
    }
        
// ---------------------------------------------------------
// CPosConstManager::SetLastWorkingGpsPsyId
// ---------------------------------------------------------
//
void CPosConstManager::SetLastWorkingGpsPsyId( TPositionModuleId aId )
    {
    TRACESTRING2( "SetLastWorkingGpsPsyId Id: %x", aId)

    if ( aId == iLastWorkingGpsPsyId )
        {
        return;
        }
        
    iLastWorkingGpsPsyId = aId;
    
    TBuf< KPosMaximumLastWorkingGpsPsyUidStringLength > idBuf;

    idBuf.AppendNumFixedWidth( aId.iUid, EHex, KPosPsyUidWidth );
    iRepository->Set( 
        KDefaultProxyLastWorkingGpsPsy,
        idBuf ); //error ignored
    }

//  End of File
