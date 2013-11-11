/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Implementation of CSsmInitCriticalLevels class.
*
*/

#include "ssminitcriticallevels.h"
#include "ssmmapperutility.h"
#include "trace.h"

#include <sysutildomaincrkeys.h>
#include <sysutilinternalpskeys.h>

_LIT_SECURITY_POLICY_C1( KWriteDeviceDataPolicy, ECapabilityWriteDeviceData );
_LIT_SECURITY_POLICY_PASS( KAlwaysPassPolicy );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::NewL
// ---------------------------------------------------------------------------
//
CSsmInitCriticalLevels* CSsmInitCriticalLevels::NewL()
	{
    FUNC_LOG;
    return new ( ELeave ) CSsmInitCriticalLevels();
	}


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::~CSsmInitCriticalLevels
// ---------------------------------------------------------------------------
//
CSsmInitCriticalLevels::~CSsmInitCriticalLevels()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmInitCriticalLevels::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::Execute
// ---------------------------------------------------------------------------
//
void CSsmInitCriticalLevels::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    FUNC_LOG;

    aRequest = KRequestPending;

    TRAPD( errorCode, InitCriticalLevelKeysL() );
    ERROR( errorCode, "Failed to initialize critical disk space P&S keys" );

    TRequestStatus* request = &aRequest;
    User::RequestComplete( request, errorCode );
    }


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmInitCriticalLevels::ExecuteCancel()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::Close
// ---------------------------------------------------------------------------
//
void CSsmInitCriticalLevels::Close()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::Release
// ---------------------------------------------------------------------------
//
void CSsmInitCriticalLevels::Release()
    {
    FUNC_LOG;

	delete this;
    }
// ---------------------------------------------------------------------------
// CSsmInitCriticalLevels::InitializeKeysL
// ---------------------------------------------------------------------------
//
void CSsmInitCriticalLevels::InitCriticalLevelKeysL()
    {
    FUNC_LOG;

    TInt val( 0 );
    CSsmMapperUtility& util = MapperUtilityL(); 
    User::LeaveIfError( util.CrValue( KCRUidDiskLevel, 
                                      KDiskCriticalThreshold,
                                      val ) );

    
    DefineL( KPSUidDiskLevel,
             KOtherDiskCriticalThreshold,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
             
    SetL( KPSUidDiskLevel, KOtherDiskCriticalThreshold, val );

    User::LeaveIfError( util.CrValue( KCRUidDiskLevel,
                                      KRamDiskCriticalLevel,
                                      val ) );
    
    DefineL( KPSUidDiskLevel,
             KRamDiskCriticalThreshold,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
    
    SetL( KPSUidDiskLevel, KRamDiskCriticalThreshold, val );
    }

