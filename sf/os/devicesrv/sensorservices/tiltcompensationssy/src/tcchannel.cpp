/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCChannel class implementation.
*
*/


#include <sensrvchannel.h>
#include <sensrvchannelfinder.h>
#include <ssycallback.h>

#include "tcchannel.h"
#include "tcutil.h"
#include "common.h"
#include "trace.h"

CTCChannel::~CTCChannel()
    {
    FUNC_LOG;
    
    Cancel();
    iStatePool.ResetAndDestroy();
    }

CTCChannel::CTCChannel( MSsyCallback& aCallback ):
    CActive( EPriorityNormal ),
    iCallback( aCallback )
    {
    FUNC_LOG;
    
    // Check that we have an active scheduler in the current process
    ASSERT_ALWAYS_SSY( CActiveScheduler::Current() );
    
    CActiveScheduler::Add( this );
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCChannel::ChannelId
// ----------------------------------------------------------------------------------
//
void CTCChannel::SetChannelId( TSensrvChannelId aId )
    {
    FUNC_LOG;
    
    INFO_1( "Set channel ID: [%d]", aId );
    
    iChannelInfo.iChannelId = aId;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::ChannelId
// ----------------------------------------------------------------------------------
//
TSensrvChannelId CTCChannel::ChannelId() const
    {
    FUNC_LOG;
    
    return iChannelInfo.iChannelId;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::ChannelSupported
// ----------------------------------------------------------------------------------
//
TBool CTCChannel::ChannelSupported( const TSensrvChannelInfo& aInfo )
    {
    FUNC_LOG;
    
    return iChannelInfo.IsMatch( aInfo );
    }

// ----------------------------------------------------------------------------------
// CTCChannel::DoCancel
// ----------------------------------------------------------------------------------
//
void CTCChannel::DoCancel()
    {
    FUNC_LOG;
    
    CompleteTransaction( KErrCancel );
    }

// ----------------------------------------------------------------------------------
// CTCChannel::RunError
// ----------------------------------------------------------------------------------
//
TInt CTCChannel::RunError( TInt aError )
    {
    FUNC_LOG;
    
    if( aError != KErrNone )
        {
        INFO_1( "RunError called with code [%d]", aError );
        }
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::CurrentState
// ----------------------------------------------------------------------------------
//
CTCState* CTCChannel::CurrentState() const
    {
    FUNC_LOG;
    
    return iCurrentState;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::ChangeState
// ----------------------------------------------------------------------------------
//
void CTCChannel::ChangeStateL( TInt aStateId )
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( aStateId >= CTCState::ETCStateIdle &&
        aStateId < CTCState::ETCStateLast, EInvalidState );
    
    iPreviousState = iCurrentState;
    iCurrentState = iStatePool[aStateId];
    
    // Inform new current state about state entry
    iCurrentState->HandleStateEntryL();
    
    // Inform previous state about state exit
    if( iPreviousState )
        {
        iPreviousState->HandleStateExitL();
        }
    
    INFO_2( "Changing channel state [%S] -> [%S]",
        &TCUtil::StateIdAsDesC( iPreviousState ),
        &TCUtil::StateIdAsDesC( iCurrentState ) );
    }

// ----------------------------------------------------------------------------------
// CTCChannel::DataBuffer
// ----------------------------------------------------------------------------------
//
TUint8* CTCChannel::DataBuffer() const
    {
    FUNC_LOG;
    
    return iData;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::DataCount
// ----------------------------------------------------------------------------------
//
TInt CTCChannel::DataCount() const
    {
    FUNC_LOG;
    
    return iCount;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::BaseConstructL
// ----------------------------------------------------------------------------------
//
void CTCChannel::BaseConstructL()
    {
    FUNC_LOG;
    
    // Register states
    RegisterStateL( CTCState::ETCStateIdle, iStatePool );
    RegisterStateL( CTCState::ETCStateOpen, iStatePool );
    RegisterStateL( CTCState::ETCStateListenData, iStatePool );
    RegisterStateL( CTCState::ETCStateBufferFull, iStatePool );
    RegisterStateL( CTCState::ETCStateClosing, iStatePool );
    
    // Switch to idle
    ChangeStateL( CTCState::ETCStateIdle );
    }

// ----------------------------------------------------------------------------------
// CTCChannel::FindChannelL
// ----------------------------------------------------------------------------------
//
TBool CTCChannel::FindChannelL( const TSensrvChannelTypeId& aChannelTypeId,
    const TDesC8& aLocation,
    const TDesC8& aVendorId,
    TSensrvChannelInfo& aChannelInfo )
    {
    FUNC_LOG;

    TBool found = EFalse;

    // Configure channel
    CSensrvChannelFinder* channelFinder = CSensrvChannelFinder::NewLC();
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL( channelInfoList );
    
    // Find double tap channel
    aChannelInfo.iChannelId = 0;
    aChannelInfo.iContextType = ESensrvContextTypeNotDefined;
    aChannelInfo.iQuantity = ESensrvQuantityNotdefined;
    aChannelInfo.iChannelType = aChannelTypeId;
    aChannelInfo.iLocation = aLocation;
    aChannelInfo.iVendorId = aVendorId;
    aChannelInfo.iDataItemSize = 0;
    channelFinder->FindChannelsL( channelInfoList, aChannelInfo );
    if( channelInfoList.Count() > 0 )
        {
        INFO_2( "aChannelInfo size [%d] vs. channelInfoList[0] size [%d]",
            sizeof( aChannelInfo ),
            sizeof( channelInfoList[ 0 ] ) );
        
        // Take first match and assume it to be best one
        aChannelInfo = channelInfoList[ 0 ];
        found = ETrue;
        INFO_3( "Found sensor channel with parameters: ChannelTypeId: %x, Location: %S, Vendor: %S",
             aChannelTypeId, &aLocation, &aVendorId );
        }
    else
        {
        ERROR_GEN_3( "Did not found sensor channel: ChannelTypeId: %x, Location: %S, Vendor: %S",
             aChannelTypeId,
             &aLocation,
             &aVendorId );
        }
    
    // Cleanup
    CleanupStack::PopAndDestroy( &channelInfoList );
    CleanupStack::PopAndDestroy( channelFinder );

    return found;
    }

// ----------------------------------------------------------------------------------
// CTCChannel::CompleteTransaction
// ----------------------------------------------------------------------------------
//
void CTCChannel::CompleteTransaction( TInt aCode )
    {
    FUNC_LOG;
    
    ASSERT_DEBUG_TRACE( IsActive(), ENotActive );
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aCode );
    }

// End of File
