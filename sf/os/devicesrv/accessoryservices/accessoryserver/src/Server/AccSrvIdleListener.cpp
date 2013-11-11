/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Connection Status Handler
*
*/



// INCLUDE FILES
#include "AccSrvIdleListener.h"
#include "acc_debug.h"
#include "AccSrvServerModel.h"

#include <startupdomainpskeys.h>

#ifdef RD_TSP_CLIENT_MAPPER
#include <tspclientmapper.h>
#endif // RD_TSP_CLIENT_MAPPER
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvIdleListener::CAccSrvIdleListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvIdleListener::CAccSrvIdleListener()
    : CActive ( EPriorityNormal )
    {
    TRACE_ASSERT_ALWAYS;
    }

// -----------------------------------------------------------------------------
// CAccSrvIdleListener::~CAccSrvIdleListener
// -----------------------------------------------------------------------------
//
CAccSrvIdleListener::~CAccSrvIdleListener()
    {
    	iProperty.Close();
    }



// -----------------------------------------------------------------------------
// CAccSrvIdleListener::CAccSrvIdleListener
// -----------------------------------------------------------------------------
//
CAccSrvIdleListener::CAccSrvIdleListener( CAccSrvServerModel* aServerModel )
    : CActive ( EPriorityNormal), iServerModel( aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::CAccSrvIdleListener()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvIdleListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvIdleListener::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::ConstructL()" );
    
    CActiveScheduler::Add( this );
    
    //attach to property
	User::LeaveIfError( iProperty.Attach( KPSUidStartup,
	                    KPSGlobalSystemState ) );

	//start listening...
	RunL();
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::ConstructL() - return void" );

    }

// -----------------------------------------------------------------------------
// CAccSrvIdleListener::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CAccSrvIdleListener* CAccSrvIdleListener::NewL( CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::NewL()" );

	CAccSrvIdleListener * self = new (ELeave) CAccSrvIdleListener( aServerModel );
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::NewL() - return void" );
	return self;
    }
        
// -----------------------------------------------------------------------------
// CAccSrvIdleListener::DoCancel()
// 
// -----------------------------------------------------------------------------
//
void CAccSrvIdleListener::DoCancel()
    {	
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::DoCancel()" );

	iProperty.Close();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::DoCancel() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvIdleListener::RunL()
// 
// -----------------------------------------------------------------------------
//
void CAccSrvIdleListener::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::RunL()" );

	TInt value( 0 );
	
	//check value
	TInt err = iProperty.Get( value );
	
	if ( value == ESwStateNormalRfOn ||
	     value == ESwStateNormalRfOff ||
         value == ESwStateNormalBTSap )
	    {
	    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::RunL() - Idle detected!" );
#ifdef RD_TSP_CLIENT_MAPPER        
        CTspClientMapper* customIf = CTspClientMapper::NewL();
        delete customIf;
#endif // RD_TSP_CLIENT_MAPPER
		//idle detected, self destruction...
		iServerModel->SetIdleDetected();
		delete this;
    	}
	else
	    {
	    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::RunL() - No idle detected, subscribe and wait idle key" );

		// continue listening if not idle
		iStatus = KRequestPending;  
		SetActive();  
		iProperty.Subscribe( iStatus );
	    }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvIdleListener::RunL() - return void" );	
    }

// End of File
