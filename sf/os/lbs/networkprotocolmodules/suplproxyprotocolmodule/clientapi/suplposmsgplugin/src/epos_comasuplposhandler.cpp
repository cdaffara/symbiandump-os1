/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for the SUPL POS message plugin.
*
*/



#include <ecom/ecom.h>
#include "epos_comasuplposhandler.h"
#include "epos_comasuplpossession.h"
#include "lbssuplproxyposclient.h"
#include "lbsdevloggermacros.h"

// -----------------------------------------------------------------------------
// COMASuplPosHandler::NewL
// Factory function to instantiate an object of COMASuplPosHandler
// -----------------------------------------------------------------------------
//
COMASuplPosHandlerBase* COMASuplPosHandler::NewL()
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::NewL() Begin and End\n");
	LBSLOG(ELogP9, "<-S COMASuplPosHandler::NewL() SUPL-FW\n");    
	return new ( ELeave ) COMASuplPosHandler;
    }
    

// -----------------------------------------------------------------------------
// COMASuplPosHandler::~COMASuplPosHandler
// Destructor
// -----------------------------------------------------------------------------
//
COMASuplPosHandler::~COMASuplPosHandler()
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::~COMASuplPosHandler() Begin\n");
	LBSLOG(ELogP9, "<-A COMASuplPosHandler::~COMASuplPosHandler() SUPL-FW\n");
	
	// Close the session with the Supl proxy protocol module.
	if ( iSuplProxyPosSession.Handle() )
		{
		iSuplProxyPosSession.Close();
		}
	
	LBSLOG(ELogP1, "COMASuplPosHandler::~COMASuplPosHandler() End\n");
    }
    


// -----------------------------------------------------------------------------
// COMASuplPosHandler::InitializeL
// Initializes the instance of COMASuplPosHandler
// -----------------------------------------------------------------------------
//
void COMASuplPosHandler::InitializeL( TRequestStatus& aStatus )
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::InitializeL() Begin\n");
	LBSLOG(ELogP9, "<-A COMASuplPosHandler::InitializeL() SUPL-FW\n");

    aStatus = KRequestPending;
    
	// Establish a session with the SuplProxyProtoclModule server
    // The subsessions are established in the COMASuplPosSession class.
    User::LeaveIfError(iSuplProxyPosSession.Connect()); 
    
    TRequestStatus* status = &aStatus;

	LBSLOG(ELogP9, " < TRequestStatus aStatus = 0\n"); 
    User::RequestComplete( status, KErrNone );
    
	LBSLOG(ELogP1, "COMASuplPosHandler::InitializeL() End\n");
    }
    

// -----------------------------------------------------------------------------
// COMASuplPosHandler::CancelInitialize
// Cancels an outstanding request to initialize
// -----------------------------------------------------------------------------
//
void COMASuplPosHandler::CancelInitialize()
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::CancelInitialize() Begin and End\n");
	LBSLOG(ELogP9, "<-S COMASuplPosHandler::CancelInitialize() SUPL-FW\n");    
    }
    

// -----------------------------------------------------------------------------
// COMASuplPosHandler::CreateNewSessionL
// Factory function to create a new instance of COMASuplPosSession
// -----------------------------------------------------------------------------
//
COMASuplPosSessionBase* COMASuplPosHandler::CreateNewSessionL( 
                                MOMASuplObserver* aSuplObserver )
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::CreateNewSessionL() Begin and End\n");
	LBSLOG(ELogP9, "<-S COMASuplPosHandler::CreateNewSessionL() SUPL-FW\n");    
	LBSLOG2(ELogP9, "  > MOMASuplObserver* aSuplObserver = 0x%08X\n", aSuplObserver);

    // Passing the session handle to the COMASuplPosSession for opening subsessions.
    return COMASuplPosSession::NewL( aSuplObserver, iSuplProxyPosSession );
    }
    

// -----------------------------------------------------------------------------
// COMASuplPosHandler::COMASuplPosHandler
// Constructor
// -----------------------------------------------------------------------------
//
COMASuplPosHandler::COMASuplPosHandler():COMASuplPosHandlerBase()
    {
	LBSLOG(ELogP1, "COMASuplPosHandler::COMASuplPosHandler():COMASuplPosHandlerBase() Begin and End\n");
    }

