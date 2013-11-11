/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "SaeSignalStrengthObserver.h"
#include "saepubsubnotifier.h"
#include "sae_debug.h"
 
#include <sacls.h>
#include <mmtsy_names.h>
#include <e32svr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt8 KNetworkBarsUninitialized(-111);

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::CSaeSignalStrengthObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSaeSignalStrengthObserver::CSaeSignalStrengthObserver( 
    CSaePubSubNotifier* aNotifier ) : 
    CActive( EPriorityStandard ),
    iNotifier( aNotifier ), 
    iSignalStrength( 0 ), 
    iBar( KNetworkBarsUninitialized ),
    iPreviousBar( KNetworkBarsUninitialized )
    {
    }

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSaeSignalStrengthObserver::ConstructL()
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::ConstructL()" );
    CActiveScheduler::Add( this );
    
	COM_TRACE_( "SAE: CSaeSignalStrengthObserver: Connecting to Etel server..." );

    TInt err = iEtelServer.Connect();
	if( err != KErrNone )
		{
		// Retry
		User::LeaveIfError( iEtelServer.Connect() );
		}

	COM_TRACE_( "SAE: CSaeSignalStrengthObserver: Loading phone module..." );

	err = iEtelServer.LoadPhoneModule( KMmTsyModuleName );
	if( err != KErrNone )
		{
		// Retry
		User::LeaveIfError( iEtelServer.LoadPhoneModule( KMmTsyModuleName ) );
		}

	COM_TRACE_( "SAE: CSaeSignalStrengthObserver: Opening sesion to RMobilePhone..." );

    err = iMobilePhone.Open( iEtelServer, KMmTsyPhoneName );
	if( err != KErrNone )
		{
		// Retry
		User::LeaveIfError( iMobilePhone.Open( iEtelServer, KMmTsyPhoneName ) );
		}
    
    // Issue first request
    Receive();
    
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::ConstructL() return void" );
    }

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSaeSignalStrengthObserver* CSaeSignalStrengthObserver::NewL( 
    CSaePubSubNotifier* aNotifier )
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::CSaeSignalStrengthObserver()" );
    CSaeSignalStrengthObserver* self = new( ELeave ) CSaeSignalStrengthObserver( aNotifier );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::CSaeSignalStrengthObserver() return self" );
    return self;
    }

    
// Destructor
CSaeSignalStrengthObserver::~CSaeSignalStrengthObserver()
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::~CSaeSignalStrengthObserver()" );
    Cancel();
    iMobilePhone.Close();
    iEtelServer.Close();
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::~CSaeSignalStrengthObserver() return void" );
    }

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::Receive
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaeSignalStrengthObserver::Receive()
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::Receive()" );
    if ( IsActive() )
        { 
        COM_TRACE_( "SAE: CSaeSignalStrengthObserver::Receive() allready request pending..." );
        return;
        }
    else
        {
        iMobilePhone.GetSignalStrength( iStatus, iSignalStrength, iBar );
        SetActive();      
        } 
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::Receive() return void." );
    }

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::IssueRequest
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaeSignalStrengthObserver::IssueRequest()
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::IssueRequest()" );
    if ( IsActive() )
        { 
        COM_TRACE_( "SAE: CSaeSignalStrengthObserver::IssueRequest() allready request pending... return void." );
        return;
        }
    else
        {
        iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBar );
        } 
    SetActive(); 
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::IssueRequest() return void." );
    }



// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaeSignalStrengthObserver::DoCancel()
    {
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::DoCancel()" );
    if ( IsActive() )
        {  
        if ( iPreviousBar == KNetworkBarsUninitialized )
            {
            iMobilePhone.CancelAsyncRequest( EMobilePhoneGetSignalStrength );
            }
        else
            {
            iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifySignalStrengthChange );
            }
        }
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::DoCancel() return void." );
    }

// -----------------------------------------------------------------------------
// CSaeSignalStrengthObserver::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaeSignalStrengthObserver::RunL()
    {
    COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL(), bars: %d", iBar );

    TInt availability( ESANetworkAvailable );
    TInt strength( ESANetworkStrengthNone );
    
    if ( iStatus != KErrNone )
        {
        COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL() request failed %d", iStatus.Int() );
        }
    
    // Reissue request
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::RunL(), reissue request" );
    IssueRequest();
    
    if ( iPreviousBar != iBar )
        {
        iPreviousBar = iBar;
        
        switch ( iBar )
            {
            case 0:
                {
                availability = ESANetworkUnAvailable;    
                strength = ESANetworkStrengthNone;  
                }
            break;
            
            case 1:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthLow;  
                }
    		break;

        	case 2:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthLow;  
                }
       		break;

        	case 3:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthMedium;  
                }
			break;

    		case 4:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthMedium;  
                }
            break;
            
            case 5:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthMedium;  
                }
            break;
            
            case 6:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthHigh;  
                }
            break;
            
            case 7:
                {
                availability = ESANetworkAvailable;    
                strength = ESANetworkStrengthHigh;  
                }
            break;
            }
        
        TInt ret(0);
        
        ret = iNotifier->NotifyPubSub( KUidNetworkStatus, availability );
        COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL():KUidNetworkStatus: availability == %d", availability );        
        COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL():ret == %d", ret );        
        
        ret = iNotifier->NotifyPubSub( KUidNetworkStrength, strength );
        COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL():KUidNetworkStrength: strength == %d", strength );
        COM_TRACE_1( "SAE: CSaeSignalStrengthObserver::RunL():ret == %d", ret ); 
        ret = ret; // to suppress warning 'variable not used'                      
        }
    else
        {
        COM_TRACE_( "SAE: CSaeSignalStrengthObserver::RunL(), same state as on last notif., do not update states" );
        }
        
    COM_TRACE_( "SAE: CSaeSignalStrengthObserver::RunL(), return void." );
    }

//  End of File  
