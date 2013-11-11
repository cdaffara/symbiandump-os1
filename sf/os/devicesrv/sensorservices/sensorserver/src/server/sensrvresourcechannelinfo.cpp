/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The channel basic information with knowledge of which implementation
*                this channel belongs to.
*
*/


#include "sensrvresourcechannelinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::TSensrvResourceChannelInfo()
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo::TSensrvResourceChannelInfo()
    : TSensrvChannelInfo(),
      iSsyImplementationUid( KNullUid ),
      iPolicy( TSecurityPolicy::EAlwaysPass ),
      iDynamic( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::TSensrvResourceChannelInfo()
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo::TSensrvResourceChannelInfo( const TSensrvResourceChannelInfo& aChannelInfo )
    : TSensrvChannelInfo( aChannelInfo ),
      iSsyImplementationUid( aChannelInfo.iSsyImplementationUid ),
      iPolicy( aChannelInfo.iPolicy ),
      iDynamic( aChannelInfo.iDynamic )
    {
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::TSensrvResourceChannelInfo()
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo::TSensrvResourceChannelInfo( const TSensrvChannelInfo& aChannelInfo )
    : TSensrvChannelInfo( aChannelInfo ),
      iSsyImplementationUid( KNullUid ),
      iPolicy( TSecurityPolicy::EAlwaysPass ),
      iDynamic( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::IsMatch()
// ---------------------------------------------------------------------------
//
TBool TSensrvResourceChannelInfo::IsMatch( const TSensrvResourceChannelInfo &aInfo ) const
    {
    if ( aInfo.iSsyImplementationUid != iSsyImplementationUid )
        {
        return EFalse;
        }

    return TSensrvChannelInfo::IsMatch( aInfo );
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::IsMatch()
// ---------------------------------------------------------------------------
//
TBool TSensrvResourceChannelInfo::IsMatch( const TSensrvResourceChannelInfo &aInfo, const RMessage2& aMessage ) const
    {
#ifdef COMPONENT_TRACE_DEBUG
    TraceCapabilities(aMessage);
#endif //COMPONENT_TRACE_DEBUG

    if( !iPolicy.CheckPolicy( aMessage ) )
        {
    	return EFalse;
        }

    return TSensrvChannelInfo::IsMatch( aInfo );
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::IsComplete()
// ---------------------------------------------------------------------------
//
TBool TSensrvResourceChannelInfo::IsComplete() const
    {
    if ( iSsyImplementationUid == KNullUid ) // must have an implementation UID
        {
        return EFalse;
        }

    return TSensrvChannelInfo::IsComplete();
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::CompareFindMatch()
// ---------------------------------------------------------------------------
//
TInt TSensrvResourceChannelInfo::CompareFindMatch( const TSensrvChannelInfo* aChannelInfoToMatchFrom,
                                                   const TSensrvResourceChannelInfo& aChannelInfoToMatchWith )
    {
    if( aChannelInfoToMatchFrom )
        return aChannelInfoToMatchFrom->IsMatch(aChannelInfoToMatchWith);
    else 
        return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::CompareFindMatchReverse()
// ---------------------------------------------------------------------------
//
TInt TSensrvResourceChannelInfo::CompareFindMatchReverse( const TSensrvChannelInfo* aChannelInfoToMatchWith,
                                                          const TSensrvChannelInfo& aChannelInfoToMatchFrom )
    {
    if( aChannelInfoToMatchWith )
        return aChannelInfoToMatchFrom.IsMatch(*aChannelInfoToMatchWith);
    else 
        return KErrNotFound;
    }

#ifdef COMPONENT_TRACE_DEBUG
// ---------------------------------------------------------------------------
// TSensrvResourceChannelInfo::TraceCapabilities()
// ---------------------------------------------------------------------------
//
void TSensrvResourceChannelInfo::TraceCapabilities( const RMessage2& aMessage ) const
{
    if( aMessage.HasCapability( ECapabilityTCB ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityTCB" ) ));
        }

    if( aMessage.HasCapability( ECapabilityCommDD ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityCommDD" ) ));
        }

    if( aMessage.HasCapability( ECapabilityPowerMgmt ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityPowerMgmt" ) ));
        }

    if( aMessage.HasCapability( ECapabilityMultimediaDD ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityMultimediaDD" ) ));
        }

    if( aMessage.HasCapability( ECapabilityReadDeviceData ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityReadDeviceData" ) ));
        }

    if( aMessage.HasCapability( ECapabilityWriteDeviceData ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityWriteDeviceData" ) ));
        }

    if( aMessage.HasCapability( ECapabilityDRM ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityDRM" ) ));
        }

    if( aMessage.HasCapability( ECapabilityTrustedUI ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityTrustedUI" ) ));
        }

    if( aMessage.HasCapability( ECapabilityProtServ ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityProtServ" ) ));
        }

    if( aMessage.HasCapability( ECapabilityDiskAdmin ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityDiskAdmin" ) ));
        }

    if( aMessage.HasCapability( ECapabilityNetworkControl ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityNetworkControl" ) ));
        }

    if( aMessage.HasCapability( ECapabilityAllFiles ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityAllFiles" ) ));
        }

    if( aMessage.HasCapability( ECapabilitySwEvent ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilitySwEvent" ) ));
        }

    if( aMessage.HasCapability( ECapabilityNetworkServices ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityNetworkServices" ) ));
        }

    if( aMessage.HasCapability( ECapabilityLocalServices ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityLocalServices" ) ));
        }

    if( aMessage.HasCapability( ECapabilityReadUserData ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityReadUserData" ) ));
        }

    if( aMessage.HasCapability( ECapabilityWriteUserData ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityWriteUserData" ) ));
        }

    if( aMessage.HasCapability( ECapabilityLocation ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityLocation" ) ));
        }

    if( aMessage.HasCapability( ECapabilitySurroundingsDD ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilitySurroundingsDD" ) ));
        }

    if( aMessage.HasCapability( ECapabilityUserEnvironment ) )
        {
    	COMPONENT_TRACE( ( _L( "### Channel info. Channel has capability ECapabilityUserEnvironment" ) ));
        }
}

#endif //COMPONENT_TRACE_DEBUG


// End of file
