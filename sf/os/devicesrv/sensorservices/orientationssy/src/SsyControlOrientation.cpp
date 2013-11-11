/*
* Copyright (c) 2007,2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control class of Orientation SSY
*
*/


#include <ssycallback.h>
#include "SsyControlOrientation.h"
#include "SsyProperty.h"
#include "SsyConfiguration.h"
#include "SsyTrace.h"



// ----------------------------------------------------------------------------------
// CSSYControlOrientationOrientation::CSSYControlOrientation()
// ----------------------------------------------------------------------------------
//
CSSYControlOrientation* CSSYControlOrientation::NewL( MSsyCallback* aSsyCallback )
    {
    SSY_TRACE_IN();

    ASSERT_DEBUG_SSY( aSsyCallback );

    CSSYControlOrientation* self = new( ELeave ) CSSYControlOrientation( aSsyCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    SSY_TRACE_OUT();

    return self;
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::ConstructL()
// ----------------------------------------------------------------------------------
//
void CSSYControlOrientation::ConstructL()
    {
    SSY_TRACE_IN();

    // Create sensor properties
    iSensorProperties = new (ELeave) CSSYProperty(
        0, KSSySensorProperties, ARRAY_LENGTH( KSSySensorProperties ) );

    // Create and register all the needed channels
    CreateAndRegisterChannelsL( KSSsyChannelInfo, ARRAY_LENGTH( KSSsyChannelInfo ) );

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::CSSYControlOrientation()
// ----------------------------------------------------------------------------------
//
CSSYControlOrientation::CSSYControlOrientation( MSsyCallback* aSsyCallback ) :
    iSensorProperties( NULL ),
    iCallback( aSsyCallback )
    {
    SSY_TRACE_IN();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::~CSSYControlOrientation()
// ----------------------------------------------------------------------------------
//
CSSYControlOrientation::~CSSYControlOrientation()
    {
    SSY_TRACE_IN();

    delete iSensorProperties;
    iSensorProperties = NULL;

    iChannelArray.ResetAndDestroy();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::IsChannelSupported()
// ----------------------------------------------------------------------------------
//
TBool CSSYControlOrientation::IsChannelSupported(
    const TSensrvChannelId aChannelID,
    TInt& aIndex )
    {
    SSY_TRACE_IN();

    TBool found( EFalse );

    for ( TInt index = 0; index < iChannelArray.Count(); index++ )
        {
        if ( ( *iChannelArray[index] ).ChannelId() == aChannelID )
            {
            found = ETrue;
            aIndex = index;
            break;
            }
        }

    SSY_TRACE_OUT();

    return found;
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::OpenChannelL()
// ----------------------------------------------------------------------------------
//
void CSSYControlOrientation::OpenChannelL( TSensrvChannelId aChannelID )
    {
    SSY_TRACE_IN();

    TInt err( KErrNone );
    TInt channelIndex( 0 );

    if ( IsChannelSupported( aChannelID, channelIndex ) )
        {
        err = ( *iChannelArray[ channelIndex ] ).OpenChannel();
        }
    else
        {
        err = KErrGeneral;
        }

    if ( err != KErrNone )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Opening channel failed: %d", err );
        User::Leave( err );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::CloseChannelL()
// ----------------------------------------------------------------------------------
//
void CSSYControlOrientation::CloseChannelL( TSensrvChannelId aChannelID )
    {
    SSY_TRACE_IN();

    TInt err( KErrNone );
    TInt channelIndex( 0 );

    if ( IsChannelSupported( aChannelID, channelIndex ) )
        {
        if( channelIndex <= iChannelArray.Count() )
            {
            err = ( *iChannelArray[channelIndex] ).CloseChannel();
            }
        }
    else
        {
        err = KErrGeneral;
        }

    if ( err != KErrNone )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Closing channel failed: %d", err );
        User::Leave( err );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYControlOrientation::CreateAndRegisterChannelsL()
// ----------------------------------------------------------------------------------
//
void CSSYControlOrientation::CreateAndRegisterChannelsL(
    const TSSsyChannelInfo* aChannelConfigurations,
    const TUint aNumberofConfigurations )
    {
    SSY_TRACE_IN();

    ASSERT_ALWAYS_SSY( aChannelConfigurations );
    ASSERT_ALWAYS_SSY( aNumberofConfigurations );

    RSensrvChannelInfoList channelInfoList;

    for ( TInt index = 0; index < aNumberofConfigurations; index++ )
        {
        CSSYChannel* channel( NULL );

        TSensrvChannelInfo channelInfo
            (
            aChannelConfigurations[index].iChannelId,
            aChannelConfigurations[index].iContextType,
            aChannelConfigurations[index].iQuantity,
            aChannelConfigurations[index].iChannelType,
            aChannelConfigurations[index].iLocation,
            aChannelConfigurations[index].iVendorId,
            aChannelConfigurations[index].iDataItemSize,
            aChannelConfigurations[index].iDataTypeId
            );

        switch ( channelInfo.iChannelType )
            {
            case KSensrvChannelTypeIdOrientationData:
                // Intentional flowthrough
            case KSensrvChannelTypeIdRotationData:
                channel = CSSYChannel::NewL( iSensorProperties, iCallback, channelInfo );
                break;
            default:
                ASSERT_ALWAYS_SSY( 0 );
                break;
            }

        if ( channel )
            {
            User::LeaveIfError(iChannelArray.Append( channel ));
            User::LeaveIfError(channelInfoList.Append( channelInfo));
            }
        }

    // Register channels to the client
    SSY_TRACE_TRAP( err, iCallback->RegisterChannelsL( channelInfoList ) );

    // Update ChannelId
    for ( TInt i = 0; i < iChannelArray.Count(); i++ )
        {
        (*iChannelArray[i]).SetChannelId( channelInfoList[i].iChannelId );
        }

    channelInfoList.Reset();
    channelInfoList.Close();

    SSY_TRACE_OUT();
    }

// End of File
