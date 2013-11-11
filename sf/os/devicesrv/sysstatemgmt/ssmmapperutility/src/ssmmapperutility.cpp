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
* Implementation of CSsmMapperUtility class.
*
*/

#include <e32property.h>
#include <featdiscovery.h>
#include <centralrepository.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstateawaresession.h>
#include "ssmmapperutility.h"
#include "trace.h"

typedef CSsmMapperUtility* (*CreateMapperUtilExtL)();

_LIT_SECURE_ID( KStifTestServerSid, 0x102073E4 );
_LIT_SECURE_ID( KStifConsoleUiSid, 0x102073DA );
_LIT_SECURE_ID( KStifGuiSid, 0x102073DC );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmMapperUtility::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmMapperUtility* CSsmMapperUtility::NewL()
    {
    FUNC_LOG;

    CSsmMapperUtility* self = new ( ELeave ) CSsmMapperUtility;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::~CSsmMapperUtility
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmMapperUtility::~CSsmMapperUtility()
    {
    FUNC_LOG;

    delete iExtension;
    iExtensionLib.Close();
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::PsUid
// ---------------------------------------------------------------------------
//
TUid CSsmMapperUtility::PsUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid uid( aUid );
    if ( iExtension )
        {
        uid = iExtension->PsUid( aUid );
        INFO_2( "PS Uid 0x%08x mapped to 0x%08x", aUid.iUid, uid.iUid );
        }

    return uid;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::CrUid
// ---------------------------------------------------------------------------
//
TUid CSsmMapperUtility::CrUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid uid( aUid );
    if ( iExtension )
        {
        uid = iExtension->CrUid( aUid );
        INFO_2( "CR Uid 0x%08x mapped to 0x%08x", aUid.iUid, uid.iUid );
        }

    return uid;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::FeatureUid
// ---------------------------------------------------------------------------
//
TUid CSsmMapperUtility::FeatureUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid uid( aUid );
    if ( iExtension )
        {
        uid = iExtension->FeatureUid( aUid );
        INFO_2( "Feature Uid 0x%08x mapped to 0x%08x", aUid.iUid, uid.iUid );
        }

    return uid;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::GetCurrentState
// ---------------------------------------------------------------------------
//
TInt CSsmMapperUtility::GetCurrentState( TSsmState& aState ) const
    {
    FUNC_LOG;

    TInt err( KErrNone );

    if ( iExtension )
        {
        err = iExtension->GetCurrentState( aState );
        ERROR( err, "Error getting state" );
        }
    else
        {
        RSsmStateAwareSession sas;

        err = sas.Connect( KUIFrameworkDomain3 );
        ERROR( err, "Error connecting RSsmStateAwareSession" );
        if ( err == KErrNone )
            {
            aState = sas.State();
            sas.Close();
            }
        }

    INFO_2( "Current state is %04x.%04x", aState.MainState(), aState.SubState() );

    return err;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::FeatureStatus
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmMapperUtility::FeatureStatus( const TUid& aUid )
    {
    FUNC_LOG;

    TBool status( EFalse );
    CFeatureDiscovery* feat = NULL;
    TRAPD( err, feat = CFeatureDiscovery::NewL() );
    ERROR( err, "Error creating CFeatureDiscovery" );

    if ( err == KErrNone )
        {
        TUid uid( FeatureUid( aUid ) );
        status = feat->IsSupported( uid );
        INFO_2( "Feature 0x%08x status is %d", uid.iUid, status );
        delete feat;
        }

    return status;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::CrValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSsmMapperUtility::CrValue( const TUid& aUid, TUint32 aKey, TInt& aValue )
    {
    FUNC_LOG;

    TUid uid( CrUid( aUid ) );
    TInt err( KErrNone );
    CRepository* repository = NULL;

    TRAP( err, repository = CRepository::NewL( uid ) );
    ERROR_1( err, "Error opening repository 0x%08x", uid.iUid );

    if ( err == KErrNone )
        {
        err = repository->Get( aKey, aValue );
        ERROR_1( err, "Failed to read value of key 0x%08x", aKey );
        delete repository;
        }

    INFO_3( "CR 0x%08x::0x%08x value is %d", uid.iUid, aKey, aValue );

    return err;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::CrValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSsmMapperUtility::CrValue( const TUid& aUid, TUint32 aKey, TDes& aValue )
    {
    FUNC_LOG;

    TUid uid( CrUid( aUid ) );
    TInt err( KErrNone );
    CRepository* repository = NULL;

    TRAP( err, repository = CRepository::NewL( uid ) );
    ERROR_1( err, "Error opening repository 0x%08x", uid.iUid );

    if ( err == KErrNone )
        {
        err = repository->Get( aKey, aValue );
        ERROR_1( err, "Failed to read value of key 0x%08x", aKey );
        delete repository;
        }

    INFO_3( "CR 0x%08x::0x%08x value is '%S'", uid.iUid, aKey, &aValue );

    return err;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::GetCommandListPath
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmMapperUtility::GetCommandListPath( TDes& aCmdListPath )
    {
    FUNC_LOG;

    if ( iExtension )
        {
        iExtension->GetCommandListPath( aCmdListPath );
        }
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::CrValue
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmMapperUtility::CheckCaps( const RMessagePtr2& aMessage )
    {
    FUNC_LOG;

    TBool ret( EFalse );

    if ( iExtension )
        {
        ret = ETrue;
        }
    else if ( aMessage.HasCapability( ECapabilityPowerMgmt ) )
        {
        ret = ETrue;
        }
    else
        {
        INFO( "Client missing the PowerMgmt capability" );
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::CSsmMapperUtility
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmMapperUtility::CSsmMapperUtility()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmMapperUtility::ConstructL
// ---------------------------------------------------------------------------
//
void CSsmMapperUtility::ConstructL()
    {
    FUNC_LOG;

    TSecureId sid = User::CreatorSecureId();

    if ( sid.iId == KStifTestServerSid.iId ||
         sid.iId == KStifConsoleUiSid.iId ||
         sid.iId == KStifGuiSid.iId )
        {
        INFO( "Loading mapper utility stub" );
        // If the loading process is STIF, load the stub library dynamically.
        // Errors loading library are not treated worth leaving. With this
        // approach better coverage for mapper utility can be achieved.
        _LIT( KSsmMapperUtilityStubLib, "ssmmapperutilstub.dll" );

        TInt err( iExtensionLib.Load( KSsmMapperUtilityStubLib ) );
        ERROR( err, "Failed to load stub library!" );
        User::LeaveIfError( err );

        // Function at ordinal 1 creates derived CSsmMapperUtility instance
        TLibraryFunction entry = iExtensionLib.Lookup( 1 );

        if ( entry )
            {
            CreateMapperUtilExtL newL =
                reinterpret_cast<CreateMapperUtilExtL >( entry );
            iExtension = ( *newL )();
            }
        else
            {
            ERROR( KErrNotFound, "Entry not found!" );
            User::Leave( KErrNotFound );
            }
        }
    }
