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
* Implementation of SsmMapperUtility class.
*
*/

#include <ssm/ssmstate.h>

#include "ssmmapperutilitystatic.h"
#include "ssmmapperutility.h"
#include "trace.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// CreateUtility
// ---------------------------------------------------------------------------
//
static CSsmMapperUtility* CreateUtility()
    {
    FUNC_LOG;

    CSsmMapperUtility* utility( NULL );
    TInt err( KErrNone );
    TRAP( err, utility = CSsmMapperUtility::NewL() );
    ERROR( err, "Failed to create CSsmMapperUtility" );
    return utility;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SsmMapperUtility::PsUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid SsmMapperUtility::PsUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid ret( TUid::Null() );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->PsUid( aUid );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::CrUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid SsmMapperUtility::CrUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid ret( TUid::Null() );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->CrUid( aUid );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::FeatureUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid SsmMapperUtility::FeatureUid( const TUid& aUid )
    {
    FUNC_LOG;

    TUid ret( TUid::Null() );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->FeatureUid( aUid );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::GetCurrentState
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SsmMapperUtility::GetCurrentState( TSsmState& aState )
    {
    FUNC_LOG;

    TInt ret( KErrGeneral );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->GetCurrentState( aState );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::FeatureStatus
// ---------------------------------------------------------------------------
//
EXPORT_C TBool SsmMapperUtility::FeatureStatus( const TUid& aUid )
    {
    FUNC_LOG;

    TBool ret( EFalse );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->FeatureStatus( aUid );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::CrValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SsmMapperUtility::CrValue( const TUid& aUid, TUint32 aKey, TInt& aValue )
    {
    FUNC_LOG;

    TInt ret( KErrGeneral );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->CrValue( aUid, aKey, aValue );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::CrValue
// ---------------------------------------------------------------------------
//
EXPORT_C TInt SsmMapperUtility::CrValue( const TUid& aUid, TUint32 aKey, TDes& aValue )
    {
    FUNC_LOG;

    TInt ret( KErrGeneral );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->CrValue( aUid, aKey, aValue );
        delete utility;
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// SsmMapperUtility::GetCommandListPath
// ---------------------------------------------------------------------------
//
EXPORT_C void SsmMapperUtility::GetCommandListPath( TDes& aCmdListPath )
    {
    FUNC_LOG;

    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        utility->GetCommandListPath( aCmdListPath );
        delete utility;
        }
    }

// ---------------------------------------------------------------------------
// SsmMapperUtility::CheckCaps
// ---------------------------------------------------------------------------
//
EXPORT_C TBool SsmMapperUtility::CheckCaps( const RMessagePtr2& aMessage )
    {
    FUNC_LOG;

    TBool ret( EFalse );
    CSsmMapperUtility* utility = CreateUtility();
    if ( utility )
        {
        ret = utility->CheckCaps( aMessage );
        delete utility;
        }
    return ret;
    }
