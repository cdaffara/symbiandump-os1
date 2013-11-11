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
* Description: Implementation of CSsmPolicyBase class.
*
*/

#include <bautils.h>
#include <centralrepository.h>
#include <ssm/conditionevaluate.h>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstateawaresession.h>

#include "ssmpolicybase.h"
#include "ssmmapperutility.h"
#include "ssmpolicypluginsprivatecrkeys.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmPolicyBase::CSsmPolicyBase
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmPolicyBase::CSsmPolicyBase()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmPolicyBase::~CSsmPolicyBase
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmPolicyBase::~CSsmPolicyBase()
    {
    FUNC_LOG;

	delete iCommandListResourceReader;
    delete iEvaluator;
    delete iUtil;
	iFs.Close();
    }


// ---------------------------------------------------------------------------
// CSsmPolicyBase::ConditionalCommandAllowedL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmPolicyBase::ConditionalCommandAllowedL(
    CResourceFile& aResourceFile,
	TInt aResourceId )
	{
    FUNC_LOG;

    return iEvaluator->EvaluateL( aResourceFile, aResourceId );
	}


// ---------------------------------------------------------------------------
// CSsmPolicyBase::GetCurrentState
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSsmPolicyBase::GetCurrentState( TSsmState& aState ) const
    {
    FUNC_LOG;

    return iUtil->GetCurrentState( aState );
    }


// ---------------------------------------------------------------------------
// CSsmPolicyBase::BaseConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmPolicyBase::BaseConstructL()
    {
    FUNC_LOG;

    User::LeaveIfError( iFs.Connect() );

    iUtil = CSsmMapperUtility::NewL();
    iEvaluator = CConditionEvaluate::NewL();

	// Let's not waste stack and limit max cmd list path to 64 characters
	// Resource file path format is as follows:
    // "\private\<SID of SSM>\<state>\<substate if any>\"
	const TInt KMaxCmdListPath( 64);
	HBufC* cmdListPath = HBufC::NewLC( KMaxCmdListPath );
	TPtr path = cmdListPath->Des();
	GetCommandListPathL( path );
	INFO_1( "Command list path : %S", &path );

	// create resource reader
	iCommandListResourceReader =
	    CSsmCommandListResourceReader::NewL( iFs, path, *this );
    CleanupStack::PopAndDestroy( cmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmPolicyBase::ResetLimitReached
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmPolicyBase::ResetLimitReached()
    {
    FUNC_LOG;

    TBool retVal( ETrue ); // Avoid infinite reset loop
    TRAPD_ERR( errorCode, retVal = ResetLimitReachedL() );
    ERROR( errorCode, "Failed to check reset limit - defaulting to reset limit reached" );
    return retVal;
    }


// ---------------------------------------------------------------------------
// CSsmPolicyBase::ResetLimitReachedL
// ---------------------------------------------------------------------------
//
TBool CSsmPolicyBase::ResetLimitReachedL()
    {
    FUNC_LOG;

    CRepository* repository = CRepository::NewL( iUtil->CrUid( KCRUidSsmStartupPolicy ) );
    CleanupStack::PushL( repository );

    TInt count;
    User::LeaveIfError( repository->Get( KSsmStartupErrorResetCounter, count ) );

    INFO_1( "Reset count is %d", count );

    TInt limit;
    User::LeaveIfError( repository->Get( KSsmStartupErrorResetLimit, limit ) );

    INFO_1( "Reset limit is %d", limit );

    const TBool retVal = ( count >= limit );

    if ( count < limit )
        {
        count++;
        User::LeaveIfError( repository->Set( KSsmStartupErrorResetCounter, count ) );
        }

    CleanupStack::PopAndDestroy( repository );
    return retVal;
    }

