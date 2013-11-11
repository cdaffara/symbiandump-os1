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
* Description:  TSensrvChannelDataReader implementation
*
*/


#include <ecom/implementationinformation.h>
#include "sensrvchanneldatareader.h"
#include "sensrvtrace.h"

/**
*   version         1 byte
*   length          1 byte
*   flags           1 byte
*   channeltype     4 bytes
*   contexttype     4 bytes
*   quantity        4 bytes
*   location        1 bytes (byte array, at least length (zero) must be present if empty)
*   vendorid        1 bytes (byte array, at least length (zero) must be present if empty)
*   data type id    4 bytes
*   security policy 8 bytes
*/
const TInt KChannelInfoVersion 		= 1;
const TInt KChannelGroupBitShift	= 4;
const TInt KByteShift 				= 8;

/**
 *  An internal helper class for SSY info read stream.
 *
 *  @lib None.
 *  @since S60 5.0
 */
class TSsyInfoReadStream
    {
    public:
        TSsyInfoReadStream( const CImplementationInformation& aSsyInfo );
        TUint8 ReadTUint8L();
        TUint32 ReadTUint32L();
        void ReadTDes8L( TDes8& aValue, TInt aLength );
        inline TInt DataLeft() const { return iMaxLen - iPos; }
        inline TInt DataPos() const { return iPos; }

    private:
        TSsyInfoReadStream();
        void DoReadL( TAny* aPtr, TInt aLength );

    private:
        TPtrC8 iDefaultData;
        TPtrC8 iOpaqueData;
        TInt iPos;
        TInt iMaxLen;
        };

// ---------------------------------------------------------------------------
// TSsyInfoReadStream::TSsyInfoReadStream
// ---------------------------------------------------------------------------
//
TSsyInfoReadStream::TSsyInfoReadStream( const CImplementationInformation& aSsyInfo ) :
    iDefaultData( aSsyInfo.DataType() ),
    iOpaqueData( aSsyInfo.OpaqueData() ),
    iPos( 0 ),
    iMaxLen( iDefaultData.Length() + iOpaqueData.Length() )
    {
    }

// ---------------------------------------------------------------------------
// TSsyInfoReadStream::ReadTUint8L
// ---------------------------------------------------------------------------
//
TUint8 TSsyInfoReadStream::ReadTUint8L()
    {
    TUint8 ret( 0 );
    DoReadL( &ret, sizeof( TUint8 ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// TSsyInfoReadStream::ReadTUint32L
// ---------------------------------------------------------------------------
//
TUint32 TSsyInfoReadStream::ReadTUint32L()
    {
    TUint8 intBytes[ sizeof( TUint32) ];
    DoReadL( intBytes, sizeof( TUint32 ) );

    // Convert array of bytes read from resource file to TUint32. DoReadL() cannot
    // directly copy to TUint32 because of potential panics on ARM targets when byte
    // boundaries are crossed by the raw data read from the resource file.
    TUint32 ret( 0 );
    TInt i( sizeof( TUint32 ) );
    while (i--)
        {
        ret <<= KByteShift;
        ret |= intBytes[ i ];
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TSsyInfoReadStream::ReadTDes8L
// ---------------------------------------------------------------------------
//
void TSsyInfoReadStream::ReadTDes8L( TDes8& aValue, TInt aLength )
    {
    if ( aLength < 0 || aLength > aValue.MaxLength() )
        {
        ERROR_TRACE( ( _L("Sensor server - TSsyInfoReadStream::ReadTDes8L: invalid length %d" ), aLength ) );
        User::Leave( KErrCorrupt );
        }
    else if ( aLength > 0 )
        {
        DoReadL( const_cast< TUint8* >( aValue.Ptr() ), aLength );
        aValue.SetLength( aLength );
        }
    else // !aLength
        {
        aValue.Zero();
        }
    }

// ---------------------------------------------------------------------------
// TSsyInfoReadStream::DoReadL
// ---------------------------------------------------------------------------
//
void TSsyInfoReadStream::DoReadL( TAny* aPtr, TInt aLength )
    {
    if ( iPos + aLength > iMaxLen )
        {
        ERROR_TRACE( ( _L("Sensor server - TSsyInfoReadStream::DoReadL: read overflow (max %d read %d)" ), iMaxLen, iPos + aLength ) );
        User::Leave( KErrCorrupt );
        }
    
    TUint8* ptr = static_cast< TUint8* >( aPtr );
    if( (aPtr == NULL) || (ptr == NULL) )
        return;
    
    TInt defaultLen( iDefaultData.Length() );
    if ( iPos < defaultLen )
        {
        // Copy from default data and handle possible concatenation
        TInt len( Min( defaultLen - iPos, aLength ) );
        Mem::Copy( ptr, iDefaultData.Ptr() + iPos, len );
        ptr += len;
        aLength -= len;
        iPos += len;
        }
    if ( aLength > 0 && iPos < iMaxLen )
        {
        // Copy from opaque data
        Mem::Copy( ptr, iOpaqueData.Ptr() + iPos - defaultLen, aLength );
        iPos += aLength;
        }
    }

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ReadChannelDataL
// ---------------------------------------------------------------------------
//
static void ReadChannelDataL( TSsyInfoReadStream& aReadStream, TSensrvResourceChannelInfo& aChannel )
    {
    COMPONENT_TRACE( _L("Sensor server - ReadChannelDataL") );

    TUint8 flags( aReadStream.ReadTUint8L() ); // Flags field
    aChannel.iChannelType = aReadStream.ReadTUint32L(); // Channel type field
    aChannel.iContextType = aReadStream.ReadTUint32L(); // Context type field
    aChannel.iQuantity = aReadStream.ReadTUint32L(); // Quantity field

	TSensrvResourceChannelInfo::TSensrvChannelGroup group =
		static_cast<TSensrvResourceChannelInfo::TSensrvChannelGroup>(
			( ( flags & TSensrvChannelDataReader::EChannelFlagChannelGroupReserved ) >> KChannelGroupBitShift ) );

    if (   group != TSensrvResourceChannelInfo::ESensrvChannelGroupNotDefined
    	&& group != TSensrvResourceChannelInfo::ESensrvChannelGroupData
    	&& group != TSensrvResourceChannelInfo::ESensrvChannelGroupEvent
    	&& group != TSensrvResourceChannelInfo::ESensrvChannelGroupState )
		{
		ERROR_TRACE( _L("Sensor server - ReadChannelDataL: invalid channel group set") );
		User::Leave( KErrCorrupt );
		}

	aChannel.iChannelGroup = group;

	// check location present for static channels
	if ( !( flags & TSensrvChannelDataReader::EChannelFlagDynamic ) &&
	         !( flags & TSensrvChannelDataReader::EChannelFlagLocationPresent ) )
		{
		ERROR_TRACE( _L("Sensor server - ReadChannelDataL: location missing for a static channel") );
		User::Leave( KErrCorrupt );
		}

	// check required for dynamic channels, will always succeed for static channels
    if ( flags & TSensrvChannelDataReader::EChannelFlagLocationPresent )
        {
        TUint8 locationLen( aReadStream.ReadTUint8L() ); // Location byte array field

        if ( !locationLen )
            {
            ERROR_TRACE( _L("Sensor server - ReadChannelDataL: location length is 0 ") );
            User::Leave( KErrCorrupt );
            }

        if ( locationLen > KSensrvLocationLength )
            {
            ERROR_TRACE( _L("Sensor server - ReadChannelDataL: location length above maximum") );
            User::Leave( KErrCorrupt );
            }

        aReadStream.ReadTDes8L( aChannel.iLocation, locationLen );
        }

	// check vendorId present for static channels
	if ( !( flags & TSensrvChannelDataReader::EChannelFlagDynamic ) &&
	         !( flags & TSensrvChannelDataReader::EChannelFlagVendorIdPresent ) )
		{
		ERROR_TRACE( _L("Sensor server - ReadChannelDataL: vendor id missing for a static channel") );
		User::Leave( KErrCorrupt );
		}

	// check required for dynamic channels, will always succeed for static channels
    if ( flags & TSensrvChannelDataReader::EChannelFlagVendorIdPresent )
        {
        TUint8 vendorIdLen( aReadStream.ReadTUint8L() ); // Vendor byte array field

        if ( !vendorIdLen )
            {
            ERROR_TRACE( _L("Sensor server - ReadChannelDataL: vendor id length is 0") );
            User::Leave( KErrCorrupt );
            }

        if ( vendorIdLen > KSensrvVendorIdLength )
            {
            ERROR_TRACE( _L("Sensor server - ReadChannelDataL: vendor id length above maximum") );
            User::Leave( KErrCorrupt );
            }

        aReadStream.ReadTDes8L( aChannel.iVendorId, vendorIdLen );
        }

    aChannel.iChannelDataTypeId = aReadStream.ReadTUint32L(); // Data type Id field

    // Security policy byte array field, if not present encoded as ETypePass i.e. use default
    if ( flags & TSensrvChannelDataReader::EChannelFlagSecPolicyPresent )
        {
        TBuf8< sizeof( TSecurityPolicy ) > policyData;
        aReadStream.ReadTDes8L( policyData, sizeof( TSecurityPolicy ) );
        aChannel.iPolicy.Set( policyData );
        }

    if ( flags & TSensrvChannelDataReader::EChannelFlagDynamic )
        {
        aChannel.iDynamic = ETrue;
        }

    COMPONENT_TRACE( _L("Sensor server - ReadChannelDataL return") );
    }

// ---------------------------------------------------------------------------
// ReadChannelsL
// ---------------------------------------------------------------------------
//
void ReadChannelsL( const CImplementationInformation& aSsyInfo,
                    RSensrvResourceChannelInfoList& aChannelList,
                    RSensrvResourceChannelInfoList& aDynamicChannelList )
    {
    COMPONENT_TRACE( _L("Sensor server - ReadChannelsL") );

    aChannelList.Reset(); // Clean channel lists
    aDynamicChannelList.Reset();

    // See the channel data field format from beginning of this file. Parsing logic in nutshell:
    // 1. Read 'version' and 'length' fields first
    // 2. Read the actual channel data fields ('flags', ..., 'security policy') using ReadChannelDataL()

    TSsyInfoReadStream readStream( aSsyInfo );
    while ( readStream.DataLeft() > 0 )
        {
        TInt dataStartPos( readStream.DataPos() );
        TUint8 version( readStream.ReadTUint8L() ); // Version field
        if ( version != KChannelInfoVersion )
            {
            ERROR_TRACE( ( _L("Sensor server - ReadChannelsL: invalid version %d" ), version ) );
            User::Leave( KErrCorrupt );
            }

        TInt dataLen( readStream.ReadTUint8L() ); // Length field
        TSensrvResourceChannelInfo channel;
        channel.iSsyImplementationUid = aSsyInfo.ImplementationUid();
        ReadChannelDataL( readStream, channel );

        if ( readStream.DataPos() - dataStartPos != dataLen ) // Verify read data size
            {
            ERROR_TRACE( ( _L("Sensor server - ReadChannelsL: invalid data length %d (expected %d)" ),
                readStream.DataPos() - dataStartPos, dataLen ) );
            User::Leave( KErrCorrupt );
            }

        if ( channel.iDynamic )
            {
            aDynamicChannelList.AppendL( channel );
            }
        else
            {
            aChannelList.AppendL( channel );
            }
        }

    COMPONENT_TRACE( _L("Sensor server - ReadChannelsL return") );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TSensrvChannelDataReader::ReadChannels
// ---------------------------------------------------------------------------
//
TInt TSensrvChannelDataReader::ReadChannels( const CImplementationInformation& aSsyInfo,
                                             RSensrvResourceChannelInfoList& aChannelList,
                                             RSensrvResourceChannelInfoList& aDynamicChannelList )
    {
    COMPONENT_TRACE( _L("Sensor server - TSensrvChannelDataReader::ReadChannels") );

    TRAPD( ret, ReadChannelsL( aSsyInfo, aChannelList, aDynamicChannelList ) );
    if ( ret != KErrNone )
        {
        ERROR_TRACE( (_L("Sensor server - ReadChannels: error %d SsyUid 0x%x" ), ret, aSsyInfo.ImplementationUid().iUid ) );
        }
    else if ( !aChannelList.Count() && !aDynamicChannelList.Count() )
        {
        ERROR_TRACE( (_L("Sensor server - ReadChannels: No channels found SsyUid 0x%x" ), aSsyInfo.ImplementationUid().iUid ) );
        }

    COMPONENT_TRACE( _L("Sensor server - TSensrvChannelDataReader::ReadChannels return" ) );

    return ret;
    }
