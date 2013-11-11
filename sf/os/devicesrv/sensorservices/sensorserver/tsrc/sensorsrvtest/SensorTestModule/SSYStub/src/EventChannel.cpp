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
* Description:  Stub imlementation
*
*/


// INCLUDE FILES
#include <SsyCallback.h>
#include <e32property.h>
#include "EventChannel.h"
#include "siftrace.h"
#include "SensrvInternalPSKeys.h"
#include <sensrvchannelfinder.h>
#include <sensrvchannel.h>
#include "sensrvchanneldatatypes.h"
#include "sensrvtestchanneldatatypes.h"




//
// ----------------------------------------------------------------------------------
// CEventChannel::CEventChannel()
// ----------------------------------------------------------------------------------
//  
CEventChannel::CEventChannel(  MSsyCallback* aSsyCallback, 
                       MSsyPropertyProvider* aPropertyProvider, 
                       TSensrvChannelInfo aChannelInfo )
    : CActive( EPriorityStandard ), 
      iSsyCallback( aSsyCallback ),
      iChannelInfo( aChannelInfo ),
      iPropertyProvider( aPropertyProvider )      
    {
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CEventChannel() - Enter" ) ) );
	
	iChannelState = EChannelIdle;
	
	// add to active scheduler
    CActiveScheduler::Add( this );
	
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CEventChannel() - Return" ) ) );
	}

// -----------------------------------------------------------------------------
// CEventChannel::ConstructL(
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEventChannel::ConstructL( )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CSSYChannelXYZ::ConstructL() - Enter" ) ) );
    
            
    COMPONENT_TRACE( ( _L( "EventSsyStub - CSSYChannelXYZ::ConstructL() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CEventChannel::NewL() 
// ----------------------------------------------------------------------------------
//  
// Two-phased constructor.
CEventChannel* CEventChannel::NewL( MSsyCallback* aSsyCallback, 
                                    MSsyPropertyProvider* aPropertyProvider, 
                                    TSensrvChannelInfo aChannelInfo )
	{
	CEventChannel* self = new ( ELeave ) CEventChannel(  aSsyCallback, aPropertyProvider, aChannelInfo );
	
	CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
	}
	
//
// ----------------------------------------------------------------------------------
// CEventChannel::~CEventChannel()
// ----------------------------------------------------------------------------------
//  
// Destructor.
CEventChannel::~CEventChannel()
	{
	Cancel();
	
	if( iChannel )
	    {
	    delete iChannel;
	    iChannel = NULL;
	    }

	}
	
void CEventChannel::IssueRequest( TInt aError )
	{
	if ( !IsActive() )
        {
	    TRequestStatus *s = &iStatus;
	    User::RequestComplete( s, aError );
	    SetActive();
        }
	}
//
// ----------------------------------------------------------------------------------
// CEventChannel::~CEventChannel()
// ----------------------------------------------------------------------------------
//  
void CEventChannel::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::DoCancel() - Enter" ) ) );

            
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::DoCancel() - Return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CEventChannel::RunL()
// ----------------------------------------------------------------------------------
//  
void CEventChannel::RunL()
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - %S" ), &location ) );
    
    switch( iChannelState )
        {
        case EChannelOpening:
            {
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - EChannelOpening" ) ) );
            TInt err( KErrNone );    
            if( iStatus.Int() == KErrNone )
                {
                COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() iChannelState = EChannelOpen" ) ) );
                iChannelState = EChannelOpen;
                
                // Find data channel
                CSensrvChannelFinder* channelFinder = CSensrvChannelFinder::NewLC();
                
                TBufC8<KSensrvLocationLength> location( _L8("First0") );
                TBufC8<KSensrvVendorIdLength> vendorId( _L8( "VendorFirst" ) );
                TSensrvChannelInfo channelInfo( 0, ESensrvContextTypeNotDefined, ESensrvQuantityNotdefined, 0, location, vendorId, 0, 0 );
                RSensrvChannelInfoList channelList(10);
                
                TRAP( err, channelFinder->FindChannelsL( channelList, channelInfo ) );
                
                if( !err && channelList.Count() > 0 )
                    {
                    iChannel = CSensrvChannel::NewL( channelList[ 0 ] );
                    TRAP( err, iChannel->OpenChannelL() );
                    }
                
                CleanupStack::PopAndDestroy( channelFinder );   
                channelList.Reset();                              
                }
            if( err || iStatus.Int() != KErrNone )
                {
                COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() iChannelState = EChannelIdle" ) ) );
                iChannelState = EChannelIdle;
                if( !err )
                    {
                    err = iStatus.Int();
                    }                
                }
                
            iSsyCallback->ChannelOpened( iChannelInfo.iChannelId, err, this, iPropertyProvider );
            break;
            }
        case EChannelClosing:
            {
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - EChannelClosing" ) ) );
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() iChannelState = EChannelIdle" ) ) );
            
            iChannelState = EChannelIdle;
            if( iChannel )
                {
                iChannel->CloseChannel();
                delete iChannel;
                iChannel = NULL;
                }
            iSsyCallback->ChannelClosed( iChannelInfo.iChannelId );
            break;
            }
        case EChannelOpen:
            {
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - EChannelOpen" ) ) );
            break;
            }
        case EChannelListening:
            {
           
            break;
            }
        case EChannelStopListening:
            {
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - EChannelStopListening" ) ) );
            
           
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() iChannelState = EChannelOpen" ) ) );
            iChannelState = EChannelOpen;
            break;
            }
        case EChannelBufferFilled:
        case EChannelForceBufferFilled:
            {
             
            
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() - EChannelForceBufferFilled" ) ) );
            iSsyCallback->BufferFilled( iChannelInfo.iChannelId, iCounter, iDataBuffer, iMaxDataCount );
            iCounter = 0;
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() iChannelState = EChannelListening" ) ) );
            iChannelState = EChannelListening;
            IssueRequest();
            break;
            }
        default:
            {
            COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::RunL() unknown channel state" ) ) );
            }
        }

            
    }

// ---------------------------------------------------------------------------
// CEventChannel::StartChannelDataL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#if defined( COMPONENT_TRACE_FLAG) &&  defined( _DEBUG )
void CEventChannel::StartChannelDataL( const TSensrvChannelId aChannelId, 
                                       TUint8* aBuffer, TInt aCount)
#else
void CEventChannel::StartChannelDataL( const TSensrvChannelId /*aChannelId*/, 
                                       TUint8* aBuffer, TInt aCount)
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StartChannelDataL() - %S: channelId = %d" ), &location, aChannelId ) );
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StartChannelDataL() - aBuffer = %x, aCount = %d" ), aBuffer, aCount ) );
    
    
    if( iChannelState == EChannelOpen )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StartChannelDataL() iChannelState = EChannelListening" ) ) );
        iChannelState = EChannelListening;
        iDataBuffer = aBuffer;
        iMaxDataCount = aCount;
        iCounter = 0;
        
        if( iChannel )
            {
            iChannel->StartDataListeningL( this, 1, 20, 10000 );
            }
        else
            {
            User::Leave( KErrGeneral );
            }
        
        IssueRequest();
        
        }
    else
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StartChannelDataL() Incorrect channel state - Leave" ) ) );
        User::Leave( KErrGeneral );
        }
            
    }

// ---------------------------------------------------------------------------
// CEventChannel::StopChannelDataL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#if defined( COMPONENT_TRACE_FLAG) &&  defined( _DEBUG )
void CEventChannel::StopChannelDataL( const TSensrvChannelId aChannelId )
#else
void CEventChannel::StopChannelDataL( const TSensrvChannelId /*aChannelId*/ )
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StopChannelDataL() - %S: channelId = %d" ), &location, aChannelId ) );
    
    
    if( iChannelState == EChannelListening )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StopChannelDataL() iChannelState = EChannelStopListening" ) ) );
        iChannelState = EChannelStopListening;
        // Stop data buffering
        IssueRequest();
        
        }
    else
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::StopChannelDataL() Incorrect channel state - Leave" ) ) );
        User::Leave( KErrGeneral );
        }
            
    }

// ---------------------------------------------------------------------------
// CEventChannel::ForceBufferFilledL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#if defined( COMPONENT_TRACE_FLAG) &&  defined( _DEBUG )
void CEventChannel::ForceBufferFilledL( const TSensrvChannelId aChannelId )
#else
void CEventChannel::ForceBufferFilledL( const TSensrvChannelId /*aChannelId*/ )
#endif
    {
    TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::ForceBufferFilledL() - %S: channelId = %d" ), &location, aChannelId ) );
    
        
    if( iChannelState == EChannelListening ||
        iChannelState == EChannelBufferFilled ||
        iChannelState == EChannelForceBufferFilled )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::ForceBufferFilledL() iChannelState = EChannelForceBufferFilled" ) ) );
        iChannelState = EChannelForceBufferFilled;
        IssueRequest();
        }
    else
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::ForceBufferFilledL() Incorrect channel state: %d - Leave" ), iChannelState ) );
        User::Leave( KErrGeneral );
        }
            
    }

// -----------------------------------------------------------------------------
// CEventChannel::GetChannelDataProviderInterfaceL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEventChannel::GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
	                                                    TAny*& aInterface )
    {
    aInterface = NULL;

	  if ( aInterfaceUid.iUid == KSsyChannelDataProviderInterface1.iUid )
		    {
		    aInterface = reinterpret_cast<TAny*>(
			    static_cast<MSsyChannelDataProvider*>( this ) );
		    }
    }
    
// ---------------------------------------------------------------------------
// CEventChannel::ChannelId()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TSensrvChannelId CEventChannel::ChannelId()
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::ChannelId() - Return %d" ), iChannelInfo.iChannelId ) );
    return iChannelInfo.iChannelId;
    }
 
// ---------------------------------------------------------------------------
// CEventChannel::SetChannelId()
// ?implementation_description
// ---------------------------------------------------------------------------
//   
void CEventChannel::SetChannelId( const TSensrvChannelId aChannelId )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::SetChannelId() - aChannelId=%d" ), aChannelId ) );
    iChannelInfo.iChannelId = aChannelId;
    }
// ---------------------------------------------------------------------------
// CEventChannel::ChannelInfo()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TSensrvChannelInfo& CEventChannel::ChannelInfo()
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::ChannelInfo()" ) ) );
    return iChannelInfo;
    }
  
// ---------------------------------------------------------------------------
// CEventChannel::OpenChannel()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CEventChannel::OpenChannel()
	{
	TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::OpenChannel() - %S" ), &location ) );
	
	TInt err( KErrNone );
	
	if( iChannelState == EChannelIdle )
	    {
	    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::OpenChannel() iChannelState = EChannelOpening" ) ) );
	    iChannelState = EChannelOpening;
	    IssueRequest();
	    
	    RProperty::Set( KPSUidSensrvTest, KSensrvLatestOpenedChannel, iChannelInfo.iChannelId );
	    }
	else
	    {
	    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::OpenChannel() Incorrect channel state" ) ) );
	    err = KErrGeneral;
	    }
	    
				
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::OpenChannel() - Return %d" ), err ) );
	return err;
	}



// ---------------------------------------------------------------------------
// CEventChannel::CloseChannel()
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt CEventChannel::CloseChannel()
	{
	TBuf<16> location;
    location.Copy( iChannelInfo.iLocation );
	COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CloseChannel() - %S" ), &location ) );
	
	TInt err( KErrNone );
	
	if( iChannelState != EChannelIdle )
	    {
	    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CloseChannel() iChannelState = EChannelClosing" ) ) );
	    iChannelState = EChannelClosing;
	    IssueRequest();
	    }
	else
	    {
	    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CloseChannel() Incorrect channel state" ) ) );
	    err = KErrGeneral;
	    }
		
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CloseChannel() - Return %d " ),err ) );
	return err;
	}

// ---------------------------------------------------------------------------
// CEventChannel::DataReceived()
// ?implementation_description
// ---------------------------------------------------------------------------
//
#if defined( COMPONENT_TRACE_FLAG) &&  defined( _DEBUG )
void CEventChannel::DataReceived( CSensrvChannel& /*aChannel*/, TInt aCount, TInt aDataLost )
#else
void CEventChannel::DataReceived( CSensrvChannel& /*aChannel*/, TInt aCount, TInt /*aDataLost*/ )
#endif
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::DataReceived(): aCount = %d, aDataLost = %d" ), aCount, aDataLost ) );
    
    iReceivedDataCount += aCount;
    
    TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
    // read datas
    for( TInt i = 0; i < aCount; i++ )
        {
        iChannel->GetData( dataBuf );
        }
            
    for( TInt i = 0; iReceivedDataCount >= 10; i++ )
        {        
        if( iCounter == 0 )
            {
            iWritePointer = iDataBuffer;
            }
            
        TTime time;
        time.HomeTime();
        //TInt64
        TSensrvTestEventData data;
        data.iTimeStampFromEventPlugin = time;
        TSensrvAccelerometerAxisData accelerometerData = dataBuf();
        data.iTimeStampFromAccelerometerPlugin = accelerometerData.iTimeStamp;
        
       
        TUint8* dataPtr = reinterpret_cast<TUint8*>( &data );
        
        TInt size( sizeof( TSensrvTestEventData ) );
        
        for( TInt i = 0; i < size; i++ )
            {
            *iWritePointer++ = *dataPtr++;
            }
        
        iCounter++;
        iReceivedDataCount -= 10;
        
        if( iCounter >= iMaxDataCount )
            {
            iSsyCallback->BufferFilled( iChannelInfo.iChannelId, iCounter, iDataBuffer, iMaxDataCount );
            iCounter = 0;
            }
        
        }
    
    }
    
// ---------------------------------------------------------------------------
// CEventChannel::DataError()
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CEventChannel::DataError( CSensrvChannel& /*aChannel*/, TSensrvErrorSeverity aError )
    {
    COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::DataReceived(): aError = %d" ), aError ) );
    if( aError == ESensrvErrorSeverityFatal )
        {
        COMPONENT_TRACE( ( _L( "EventSsyStub - CEventChannel::CloseChannel() iChannelState = EChannelClosing" ) ) );
	    iChannelState = EChannelClosing;
	    IssueRequest();
        }
    }

// ---------------------------------------------------------------------------
// CEventChannel::GetDataListenerInterfaceL()
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CEventChannel::GetDataListenerInterfaceL( TUid /*aInterfaceUid*/, TAny*& aInterface )
    {    
		aInterface = reinterpret_cast<TAny*>(
			static_cast<MSensrvDataListener*>( this ) );
    }
