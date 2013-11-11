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
* Description:  CSensrvChannelFinderImpl implementation
*
*/


#include "sensrvchannelfinderimpl.h"
#include "sensrvchannelchangehandler.h"
#include "sensrvtrace.h"

const TInt KDefaultChannelCount = 10;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelFinderImpl::CSensrvChannelFinderImpl()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::CSensrvChannelFinderImpl()" ) ) );
    }


// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelFinderImpl::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ConstructL() - Start" ) ) );

    User::LeaveIfError( iSensrvClient.Connect() );

    iChannelChangeHandler = CSensrvChannelChangeHandler::NewL( iSensrvClient );
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ConstructL() - Return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelFinderImpl::~CSensrvChannelFinderImpl()
    {
    delete iChannelChangeHandler;
    iSensrvClient.Close();    
    }


// ---------------------------------------------------------------------------
// CSensrvChannelFinderImpl::FindChannelsL
// ---------------------------------------------------------------------------
//
void CSensrvChannelFinderImpl::FindChannelsL(
                RSensrvChannelInfoList& aChannelList,
                const TSensrvChannelInfo& aSearchConditions )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::FindChannelsL() - Start" ) ) );

    // Create condition buffer
    TSensrvChannelInfoPckgBuf searchConditionsBuf( aSearchConditions );
    
    // Create channel info array
    RSensrvChannelInfoList channelInfoList(KDefaultChannelCount);
    CleanupClosePushL( channelInfoList );
    
    for( TInt i = 0; i < KDefaultChannelCount; i++ )
        {
        channelInfoList.AppendL( TSensrvChannelInfo() );
        }
        
    TInt arraySize = ( sizeof(TSensrvChannelInfo) ) * KDefaultChannelCount;
    TPtr8 arrayPtr( (TUint8*)&channelInfoList[0], arraySize, arraySize );
    
    TInt channelInfoCount(0);
    TSensrvTIntPckgBuf intBuf( channelInfoCount );
    
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::FindChannels() - SendReceive..." ) ) );
    TInt err = iSensrvClient.SendReceiveSync( ESensrvSrvReqQueryChannels, TIpcArgs( &searchConditionsBuf, 
                                                                  &arrayPtr, &intBuf ) );
    
    channelInfoCount = intBuf();
    
    if( err == KErrNone )  // Copy channels info to aChannelList
        {
        for( TInt i = 0; i < channelInfoCount; ++i )
            {
            aChannelList.AppendL( channelInfoList[i] );
            }
        }
                                                                  
    else if( err == KErrOverflow ) // buffer overflow. Allocate new buffer
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::FindChannels() - KErrOverflow" ) ) );
        
        for( TInt i = KDefaultChannelCount; i < channelInfoCount; i++ )
            {
            channelInfoList.AppendL( TSensrvChannelInfo() );
            }
            
        TInt newArraySize = ( sizeof(TSensrvChannelInfo) ) * channelInfoCount;
        TPtr8 newArrayPtr( (TUint8*)&channelInfoList[0], newArraySize, newArraySize );
        
        // Send new request
        User::LeaveIfError( iSensrvClient.SendReceiveSync( ESensrvSrvReqQueryChannels, 
                                             TIpcArgs( &searchConditionsBuf, &newArrayPtr, &intBuf ) ) );
                                                                  
        channelInfoCount = intBuf();
        
        for( TInt i = 0; i < channelInfoCount; ++i )
            {
            aChannelList.AppendL( channelInfoList[i] );
            }

        }
    else 
        {
        User::Leave( err );
        }
        
    CleanupStack::PopAndDestroy( &channelInfoList );
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::FindChannels() - return" ) ) );  
    }

// ---------------------------------------------------------------------------
// CSensrvChannelFinderImpl::SetChannelListenerL
// ---------------------------------------------------------------------------
//
void CSensrvChannelFinderImpl::SetChannelListenerL( MSensrvChannelListener* aChannelListener,
                                                    const TSensrvChannelInfo& aSearchParameters )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::SetChannelListenerL() - Start" ) ) );

    if( iChannelChangeListener && aChannelListener )
        {
        ERROR_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::SetChannelListenerL() -  ERROR: Already listening: return KErrAlreadyExists" ) ) );
        User::Leave( KErrAlreadyExists );
        }
        
    iChannelChangeListener = aChannelListener;
    
    if( iChannelChangeListener )
        {
        iChannelChangeHandler->StartListening( this, aSearchParameters );
        }
    else
        {
        if( iChannelChangeHandler->IsActive() )
            {
            iChannelChangeHandler->Cancel();
            }
        else
            {
            TInt ignore = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopChannelChangeListening, 
                                                         TIpcArgs() );
            }
        }
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::SetChannelListenerL() - Return" ) ) );
    }


// ---------------------------------------------------------------------------
// CSensrvChannelFinderImpl::ChannelChangeDetected
// ---------------------------------------------------------------------------
//
void CSensrvChannelFinderImpl::ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                                      TSensrvChannelChangeType aChangeType  )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ChannelChangeDetected()" ) ) );
    
    if( iChannelChangeListener )
        {
        iChannelChangeListener->ChannelChangeDetected( aDetectedChannel, aChangeType );
        }
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ChannelChangeDetected - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// CSensrvChannelFinderImpl::ChannelDetectionError
// ---------------------------------------------------------------------------
//
void CSensrvChannelFinderImpl::ChannelDetectionError( TSensrvErrorSeverity aError )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ChannelDetectionError()" ) ) );
    
    if( aError == ESensrvErrorSeverityFatal )
        {
        iSensrvClient.Close();
        }
        
    if( iChannelChangeListener )
        {
        iChannelChangeListener->ChannelDetectionError( *this, aError );
        }
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelFinderImpl::ChannelDetectionError - return" ) ) );
    }
    
