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
* Description:  CTCSsyControl class implementation.
*
*/


#include <ssycallback.h>

#include "tcssycontrol.h"
#include "tcchanneltilt.h"
#include "trace.h"

CTCSsyControl* CTCSsyControl::NewL( MSsyCallback* aSsyCallback )
    {
    FUNC_LOG;

    CTCSsyControl* self = new( ELeave ) CTCSsyControl;

    CleanupStack::PushL( self );
    self->ConstructL( aSsyCallback );
    CleanupStack::Pop( self );

    return self;
    }

CTCSsyControl::~CTCSsyControl()
    {
    FUNC_LOG;
    
    iChannels.ResetAndDestroy();
    }

CTCSsyControl::CTCSsyControl()
    {
    FUNC_LOG;
    }

void CTCSsyControl::ConstructL( MSsyCallback* aSsyCallback )
    {
    FUNC_LOG;
    
    // Create tilt channel. Tilt channel provides compensated tilt data
    // from magnetometer and accelerometer vectors
    CTCChannel* channel = CTCChannelTilt::NewL( *aSsyCallback );
    CleanupStack::PushL( channel );
    iChannels.AppendL( channel );
    CleanupStack::Pop( channel );
    
    // Register channels
    RSensrvChannelInfoList channelList;
    CleanupClosePushL( channelList );
    TInt channelCount = iChannels.Count();
    for( TInt i = 0; i < channelCount; i++ )
        {
        iChannels[ i ]->RegisterChannelL( channelList );
        }
    aSsyCallback->RegisterChannelsL( channelList );
    
    _LIT( KThreadName, "TiltThread" );
    RThread thread;
    thread.RenameMe( KThreadName );
    
    // Update channel IDs
    TInt channelInfoCount = channelList.Count();
    channelCount = iChannels.Count();
    for( TInt i = 0; i < channelInfoCount; i++ )
        {
        channel = NULL;
        for( TInt j = 0; j < channelCount; j++ )
            {
            channel = iChannels[ j ];
            const TSensrvChannelInfo& info = channelList[ i ];
            if( channel->ChannelSupported( info ) )
                {
                channel->SetChannelId( info.iChannelId );
                }
            }
        }
    CleanupStack::PopAndDestroy( &channelList );
    }

// ----------------------------------------------------------------------------------
// CTCSsyControl::OpenChannelL
// ----------------------------------------------------------------------------------
//
void CTCSsyControl::OpenChannelL( TSensrvChannelId aChannelId )
    {
    FUNC_LOG;
    
    CTCChannel* channel = ChannelSupported( aChannelId );
    if( channel )
        {
        INFO_1( "Found channel [%d]. Opening channel", aChannelId );
        channel->OpenChannelL();
        INFO_1( "Channel [%d] opened", aChannelId );
        }
    else
        {
        ERROR_GEN_1( "Channel [%d] not supported", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCSsyControl::CloseChannelL
// ----------------------------------------------------------------------------------
//
void CTCSsyControl::CloseChannelL( TSensrvChannelId aChannelId )
    {
    FUNC_LOG;

    CTCChannel* channel = ChannelSupported( aChannelId );
    if( channel )
        {
        INFO_1( "Found channel [%d]. Opening channel", aChannelId );
        channel->CloseChannelL();
        INFO_1( "Channel [%d] opened", aChannelId );
        }
    else
        {
        ERROR_GEN_1( "Channel [%d] not supported", aChannelId );
        User::Leave( KErrNotSupported );
        }
    }

// ----------------------------------------------------------------------------------
// CTCSsyControl::ChannelSupported
// ----------------------------------------------------------------------------------
//
CTCChannel* CTCSsyControl::ChannelSupported( TSensrvChannelId aChannelId ) const
    {
    FUNC_LOG;
    
    CTCChannel* channel = NULL;
    TInt count( iChannels.Count() );
    for( TInt i = 0; i < count; i++ )
        {
        channel = iChannels[ i ];
        TSensrvChannelId id = channel->ChannelId();
        if( id == aChannelId )
            {
            // Channel found, break
            break;
            }
        }
        
    return channel;
    }

// End of File
