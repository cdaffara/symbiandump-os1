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
* Description:  Implementation of restartsys class.
*
*/

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/starterclient.h>
#include "restartsys.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// RestartSys::RestartSystem
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RestartSys::RestartSystem()
    {
    FUNC_LOG;

    return RestartSystem( RStarterSession::EUnknownReset );
    }


// ---------------------------------------------------------------------------
// RestartSys::RestartSystem
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RestartSys::RestartSystem( TInt aStartupMode )
    {
    FUNC_LOG;
    INFO_1( "System re-start requested, mode %d", aStartupMode );

    RSsmStateManager session;
    TInt errorCode = session.Connect();
    ERROR( errorCode, "Failed to connect to RSsmStateManager" );

    if ( errorCode == KErrNone )
        {
        TSsmStateTransition stateinfo( ESsmShutdown, 
            KSsmAnySubState, aStartupMode );

        // Make request and ignore return value
        errorCode = session.RequestStateTransition( stateinfo );
        ERROR( errorCode, "Failed to RequestStateTransition" );
        session.Close();
        }

    return errorCode;
    }
