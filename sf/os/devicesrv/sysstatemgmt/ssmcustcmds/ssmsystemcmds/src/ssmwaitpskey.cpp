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
* Implementation of CSsmWaitPsKey class.
*
*/

#include "ssmwaitpskey.h"
#include "ssmpskeyobserver.h"
#include "ssmpskeyobserverexactvalue.h"
#include "ssmpskeyobserverrange.h"
#include "ssmmapperutilitystatic.h"
#include "trace.h"

#include <barsread2.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmWaitPsKey::NewL
// ---------------------------------------------------------------------------
//
CSsmWaitPsKey* CSsmWaitPsKey::NewL( TSsmWaitPsKeyType aPsKeyWaitType )
	{
    FUNC_LOG;
	return new ( ELeave ) CSsmWaitPsKey( aPsKeyWaitType );
	}


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::~CSsmWaitPsKey
// ---------------------------------------------------------------------------
//
CSsmWaitPsKey::~CSsmWaitPsKey()
    {
    FUNC_LOG;
	delete iPsObserver;
    }


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmWaitPsKey::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::Execute
// ---------------------------------------------------------------------------
//
void CSsmWaitPsKey::Execute( const TDesC8& aParams, TRequestStatus& aRequest )
    {
    FUNC_LOG;

    aRequest = KRequestPending;

    TRAPD( errorCode, iPsObserver = ExtractParamsL( aParams ) );
    ERROR( errorCode, "Failed to extract parameters" );
    if ( errorCode == KErrNone )
        {
        iPsObserver->StartObserving( aRequest );
        }
    else
        {
        TRequestStatus* request = &aRequest;
        User::RequestComplete( request, errorCode );
        }
    }


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmWaitPsKey::ExecuteCancel()
    {
    FUNC_LOG;

    if ( iPsObserver )
        {
        iPsObserver->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::Close
// ---------------------------------------------------------------------------
//
void CSsmWaitPsKey::Close()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmWaitPsKey::Release
// ---------------------------------------------------------------------------
//
void CSsmWaitPsKey::Release()
    {
    FUNC_LOG;

	delete this;
    }
// ---------------------------------------------------------------------------
// CSsmWaitPsKey::CSsmWaitPsKey
// ---------------------------------------------------------------------------
//
CSsmWaitPsKey::CSsmWaitPsKey( TSsmWaitPsKeyType aPsKeyWaitType )
             : iWaitType( aPsKeyWaitType )
    {
    }
// ---------------------------------------------------------------------------
// CSsmWaitPsKey::ExtractParamsL
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserver* CSsmWaitPsKey::ExtractParamsL( const TDesC8& aParams )
    {
    FUNC_LOG;

    RResourceReader reader;
    reader.OpenLC( aParams );
    TUid category = TUid::Uid( reader.ReadInt32L() );
    category = SsmMapperUtility::PsUid( category );
    
    TUint key = static_cast<TUint>( reader.ReadInt32L() );

    CSsmPsKeyObserver* observer = NULL;
    if ( iWaitType == ESsmWaitPsExactValue )
        {
        TInt targetValue = reader.ReadInt32L();
        observer = new ( ELeave ) CSsmPsKeyObserverExactValue( category,
                                                               key,
                                                               targetValue );
        }
    else if ( iWaitType == ESsmWaitPsRange )
        {
        TInt targetLow = reader.ReadInt32L();
        TInt targetHigh = reader.ReadInt32L();
        observer = new ( ELeave ) CSsmPsKeyObserverRange( category,
                                                          key,
                                                          targetLow,
                                                          targetHigh );
        }

    CleanupStack::PopAndDestroy( &reader );
    return observer;
    }

