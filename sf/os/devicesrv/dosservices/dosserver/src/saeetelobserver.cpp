/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "saepubsubnotifier.h"
#include "saeetelobserver.h"
#include "saegprsavailabilityobs.h"
#include "sae_debug.h"

#include <mmtsy_names.h>


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSaeEtelObserver::CSaeEtelObserver( CSaePubSubNotifier* aPubSubNotifier ) :
	iPubSubNotifier( aPubSubNotifier ),
	iGprsAvailabilityObs( 0 )
    {
    }

// EPOC default constructor can leave.
void CSaeEtelObserver::ConstructL()
    {
	COM_TRACE_( "SAE: CSaeEtelObserver::ConstructL()" );
    }

// Two-phased constructor.
CSaeEtelObserver* CSaeEtelObserver::NewL( CSaePubSubNotifier* aPubSubNotifier )
    {
    CSaeEtelObserver* self = new (ELeave) CSaeEtelObserver( aPubSubNotifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CSaeEtelObserver::~CSaeEtelObserver()
    {
	delete iGprsAvailabilityObs;

	COM_TRACE_( "SAE: CSaeEtelObserver dtor: RPacketService::Close()" );
	iService.Close();
	COM_TRACE_( "SAE: CSaeEtelObserver dtor: RPhone::Close()" );
	iPhone.Close();
	COM_TRACE_( "SAE: CSaeEtelObserver dtor: RTelServer::Close()" );
	iEtelServer.Close();
    }

// Create GPRS session and observers, and start listening for events.
TInt CSaeEtelObserver::StartListeningL()
	{
	COM_TRACE_( "SAE: CSaeEtelObserver: StartListeningL() START ->" );
	TInt err = iEtelServer.Connect();
	if( err != KErrNone )
		{
		// Retry
		User::LeaveIfError( iEtelServer.Connect() );
		}

	COM_TRACE_( "SAE: CSaeEtelObserver: Loading phone module..." );
	err = iEtelServer.LoadPhoneModule( KMmTsyModuleName );

	COM_TRACE_( "SAE: CSaeEtelObserver: phone module loaded, now opening phone..." );
	User::LeaveIfError(iPhone.Open( iEtelServer, KMmTsyPhoneName ));
	// Assumes RPhone created previously. Create RPacketService sub-session.
	COM_TRACE_( "SAE: CSaeEtelObserver: phone opened, now opening service..." );
	User::LeaveIfError(iService.Open( iPhone ));
	COM_TRACE_( "SAE: CSaeEtelObserver: service opened" );

	// iService.Attach() gets called automatically on context creation, so no
	// need to call it manually.

  if( err == KErrNone )
		{
		COM_TRACE_( "SAE: CSaeEtelObserver: trying to create availability observer..." );
		iGprsAvailabilityObs = new (ELeave) CSaeGprsAvailabilityObs( this, &iService );
		COM_TRACE_( "SAE: CSaeEtelObserver: availability observer created" );
		}
	else 
	{
		User::Leave(err);
	}
	
	COM_TRACE_( "SAE: CSaeEtelObserver: StartListeningL() <- END" );
	return err;
	}

// ---------------------------------------------------------
// CSaeEtelObserver::NotifyPubSub
// 
// ---------------------------------------------------------
//
void CSaeEtelObserver::NotifyPubSub( const TUid aVariable, const TInt aState )
	{
	iPubSubNotifier->NotifyPubSub( aVariable, aState );
	}
//  End of File  
