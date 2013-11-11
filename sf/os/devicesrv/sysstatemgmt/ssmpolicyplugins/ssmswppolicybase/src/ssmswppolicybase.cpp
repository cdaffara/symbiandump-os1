/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmSwpPolicyBase class.
*
*/

#include <bautils.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/starterclient.h>
#include <startupdomaindefs.h>

#include "ssmswppolicybase.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmSwpPolicyBase::CSsmSwpPolicyBase()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::~CSsmSwpPolicyBase
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmSwpPolicyBase::~CSsmSwpPolicyBase()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::Initialize
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::Initialize( TRequestStatus& aStatus )
    {
    FUNC_LOG;
    ASSERT_TRACE( iCommandListResourceReader );

	// Initialise command list resource reader.
	iCommandListResourceReader->Initialise( aStatus );
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::InitializeCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::InitializeCancel()
    {
    FUNC_LOG;

    iCommandListResourceReader->InitialiseCancel();
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::PrepareCommandList
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::PrepareCommandList(
    const TSsmSwp& aSwp,
    TRequestStatus& aStatus )
    {
    FUNC_LOG;
    INFO_2( "Preparing cmd list for swp: 0x%08x::0x%08x", aSwp.Key(), aSwp.Value() );

    // SWP value is used as a command list id.
    iCommandListResourceReader->PrepareCommandList( aSwp.Value(), aSwp, aStatus );
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::PrepareCommandListCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::PrepareCommandListCancel()
    {
    FUNC_LOG;

    iCommandListResourceReader->PrepareCommandListCancel();
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::CommandList
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmCommandList* CSsmSwpPolicyBase::CommandList()
    {
    FUNC_LOG;

	return iCommandListResourceReader->GetCommandList();
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::HandleCleReturnValue
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::HandleCleReturnValue(
    const TSsmSwp& INFO_PARAM( aSwp ),
	TInt aError,
	TInt aSeverity,
	TRequestStatus& aStatus )
	{
    FUNC_LOG;
    INFO_3( "Completed swp transition: 0x%08x::0x%08x with result code %d",
        aSwp.Key(), aSwp.Value(), aError );

    if ( aError == KErrNone || aSeverity < ECmdHighSeverity )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        {
		TSsmState currentState;
		TInt errorCode = GetCurrentState(currentState);

		if ( ((currentState.MainState() != ESsmShutdown) && (currentState.MainState() != ESsmFail)) || KErrNone != errorCode)
			{
			RSsmStateManager session;
			errorCode = session.Connect();
			ERROR( errorCode, "Failed to connect to RSsmStateManager" );
			if ( KErrNone == errorCode )
				{
				if ( ResetLimitReached() ) // Updates the reset count
					{
					INFO_1( "SWP 0x%08x transition failed -> Fail", aSwp.Key() );
					errorCode = session.RequestStateTransition(	TSsmStateTransition( ESsmFail, KSsmAnySubState, 0 ) );
					}
				else
					{
					INFO_1( "SWP 0x%08x transition failed -> Reset", aSwp.Key() );
					errorCode = session.RequestStateTransition(
									TSsmStateTransition( ESsmShutdown, KSsmAnySubState, RStarterSession::EUnknownReset ) );
					}
				session.Close();
				}
			}

        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, errorCode );
        }
	}


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::HandleCleReturnValueCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::HandleCleReturnValueCancel()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmSwpPolicyBase::Release
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmSwpPolicyBase::Release()
    {
    FUNC_LOG;

    delete this;
    }
