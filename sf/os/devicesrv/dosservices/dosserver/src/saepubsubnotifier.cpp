/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "saepubsubnotifier.h"
#include "saeetelobserver.h"

#include "SaeSignalStrengthObserver.h"
#include "sae_debug.h"

#include <e32std.h>
#include <e32cons.h>
#include <e32svr.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSaePubSubNotifier::CSaePubSubNotifier() : iCfStatusInitialised( EFalse )
    {
    }

// EPOC default constructor can leave.
void CSaePubSubNotifier::ConstructL()
    {

	TInt err(KErrNone);
	// Define Publish and Suvscribe properties
	DefineProperties();

	// Create the ETel observers.
	iEtelObserver = CSaeEtelObserver::NewL( this );
	// Create GPRS session and observers, and start listening for events.
	// If not succesfull ETEL messages are missed, but DosObserver still works
	// and messages are delelgated to SA.
	TRAP(err, iEtelObserver->StartListeningL());
	COM_TRACE_1( "SAE: CSaeSysAgentNotifier::ConstructL(): iEtelObserver->StartListeningL() return value: %d", err );
    
	if (err != KErrNone) 
		{
		COM_TRACE_( "SAE: CSaeSysAgentNotifier::ConstructL(): deleting iEtelObserver..." );
		delete iEtelObserver;
		iEtelObserver = 0;
		COM_TRACE_( "SAE: CSaeSysAgentNotifier::ConstructL(): iEtelObserver deleted - GPRS states not handled by etelObs." );
		}

    // Start polling network signal strength
    iSignalObserver = CSaeSignalStrengthObserver::NewL( this );
    iSignalObserver->Receive();
	
	}

// Two-phased constructor.
CSaePubSubNotifier* CSaePubSubNotifier::NewL()
    {
    CSaePubSubNotifier* self = new ( ELeave ) CSaePubSubNotifier();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CSaePubSubNotifier::~CSaePubSubNotifier()
    {
	// Disallocate reserved resources...
	delete iEtelObserver;
	delete iSignalObserver;
	}

void CSaePubSubNotifier::DefineProperties()
    {
	};

// ---------------------------------------------------------
// CSaeSysAgentNotifier::NotifySystemAgent
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSaePubSubNotifier::NotifyPubSub( const TUid aVariable, const TInt aState )
	{
#ifdef _DEBUG
	TInt variable = aVariable.iUid;
    COM_TRACE_3( "SAE: CSaePubSubNotifier::NotifyPubSub : aVariable Dec:%d, aVariable Hex:%x, aState: %d", variable, variable, aState );
#endif

	TInt returnValue = 	iSAProperty.Set(KUidSystemCategory, aVariable.iUid, aState);
	
    COM_TRACE_1( "SAE: CSaePubSubNotifier::NotifyPubSub : returnValue == %d", returnValue );
    
    return returnValue;                        	
	}
//  End of File
