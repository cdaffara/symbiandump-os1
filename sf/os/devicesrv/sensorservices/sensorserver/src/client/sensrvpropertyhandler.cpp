/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSensrvPropertyHandler implementation
*
*/


#include "sensrvpropertyhandler.h"
#include "sensrvpropertyhandlerobserver.h"
#include "sensrvtrace.h"


// ======== LOCAL FUNCTIONS ========

static TSetPropertySuccessIndicator ErrorToSuccessIndicator( TInt aError )
    {
    TSetPropertySuccessIndicator ret = ESetPropertyIndicationUnknown;
    switch ( aError )
        {
        case KSensrvErrPropertySetAvailable:
            {
            ret = ESetPropertyIndicationAvailable;
            break;
            }
        case KSensrvErrPropertySetPossible:
            {
            ret = ESetPropertyIndicationPossible;
            break;
            }
        case KSensrvErrPropertySetUnavailable:
            {
            ret = ESetPropertyIndicationUnavailable;
            break;
            }
        default:
            {
            break;
            }
        }
    return ret;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//

CSensrvPropertyHandler::CSensrvPropertyHandler( RSensrvClient& aSensrvClient, 
                                                const TSensrvChannelInfo& aChannelInfo )
    : CActive( EPriorityStandard), 
      iSensrvClient( aSensrvClient ), 
      iChannelInfo( aChannelInfo )
    {    
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyHandler* CSensrvPropertyHandler::NewL( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvPropertyHandler* self = CSensrvPropertyHandler::NewLC( aSensrvClient, aChannelInfo );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyHandler* CSensrvPropertyHandler::NewLC( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvPropertyHandler* self = new( ELeave ) CSensrvPropertyHandler( aSensrvClient, aChannelInfo );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyHandler::~CSensrvPropertyHandler()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvPropertyHandler::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::DoCancel()" ) ) );
    
    // Stop Listening
    TInt ignore = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopPropertyListening, 
                                   TIpcArgs( iChannelInfo.iChannelId ) );
                                   
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::DoCancel - return void" ) ) );
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvPropertyHandler::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::RunL(iStatus: %d" ), iStatus.Int() ) );

	Deque();
	CActiveScheduler::Add(this);
    
    TInt err = iStatus.Int();
    if ( err == KSensrvErrPropertySetAvailable ||
         err == KSensrvErrPropertySetPossible ||
         err == KSensrvErrPropertySetUnavailable )
        {
        // Continue listening and send property set availability change
        CreateAndSendRequest();
        iPropertyObserver->SetPropertySuccessIndicationChanged( ErrorToSuccessIndicator( err ) );
        }
    else if( err == KErrNone )
        {        
        if( iErrorCount > 0 )
            {
            iErrorCount = 0;
            }

        // Note: Callback is done intentionally before resending async request,
        // so that we don't need duplicate buffer like in data reading.
        // It shouldn't have noticeable effect on performance in real use cases,
        iPropertyObserver->PropertyChanged( iPropertyPckgBuf() );
        CreateAndSendRequest();  
        }
    else
        {
        if( iErrorCount == 0 )
            {
            iErrorCount++;
            CreateAndSendRequest();
            
            // Send minor error
            iPropertyObserver->PropertyError( ESensrvErrorSeverityMinor );
            }
        else if( iErrorCount == 1 )
            {
            iErrorCount++;
            
            // Reopen channel
            iPropertyObserver->ReopenChannelForPropertyListeningL();
            
            // Start listening
            CreateAndSendRequest();
            }
        
        else
            {
            // send fatal error
            iPropertyObserver->PropertyError( ESensrvErrorSeverityFatal );  
            }
        }
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::RunL - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// CSensrvPropertyHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CSensrvPropertyHandler::RunError( TInt /*aError*/ )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::RunError" ) ) );
    // send fatal error
    iPropertyObserver->PropertyError( ESensrvErrorSeverityFatal );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSensrvPropertyHandler::StartListening
// ---------------------------------------------------------------------------
//
void CSensrvPropertyHandler::StartListening( MSensrvPropertyHandlerObserver* aPropertyObserver )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::StartListening - Start" ) ) );
    if( !IsActive() && aPropertyObserver )
        {
        iPropertyObserver = aPropertyObserver; 

        CreateAndSendRequest();
        }    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::StartListening - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvPropertyHandler::CreateAndSendRequest
// ---------------------------------------------------------------------------
//
void CSensrvPropertyHandler::CreateAndSendRequest()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::CreateAndSendRequestL(Channel:%d)" ),iChannelInfo.iChannelId ) );
      
    iSensrvClient.SendReceiveAsync( ESensrvSrvReqAsyncPropertyData, 
                                    TIpcArgs( iChannelInfo.iChannelId, &iPropertyPckgBuf ),
                                    iStatus );
    
    SetActive();
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvPropertyHandler::CreateAndSendRequestL - Return" ) ) );
    }

