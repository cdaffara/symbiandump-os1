/*
 * Copyright (c) 2008,2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Implementation of CCea861EdidParser class.
 *
 */

#include "cea861edidparser.h"
#include "trace.h"

const TInt KIEEERegistrationID = 0x00000C03;

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CCea861EdidParser::CCea861EdidParser
// ---------------------------------------------------------------------------
//
CCea861EdidParser::CCea861EdidParser() :
    iParsed( EFalse ),
    iAudioDataBlockSupported( EFalse ),
    iVideoDataBlockSupported( EFalse ),
    iVendorSpecificDataBlockSupported( EFalse ),
    iSpeakerAllocationDataBlockSupported( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::ConstructL
// ---------------------------------------------------------------------------
//
void CCea861EdidParser::ConstructL( const TExtDataBlock& aData )
    {
    FUNC_LOG;

    iParsedInfo = CCea861ExtEdidInformation::NewL();
    ParseExtensionBlockL( aData );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCea861EdidParser* CCea861EdidParser::NewL( const TExtDataBlock& aData )
    {
    FUNC_LOG;

    CCea861EdidParser* self = CCea861EdidParser::NewLC( aData );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCea861EdidParser* CCea861EdidParser::NewLC( const TExtDataBlock& aData )
    {
    FUNC_LOG;

    CCea861EdidParser* self = new ( ELeave ) CCea861EdidParser();
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return self;
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::~CCea861EdidParser
// ---------------------------------------------------------------------------
//
CCea861EdidParser::~CCea861EdidParser()
    {
    FUNC_LOG;

    delete iParsedInfo;
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::ParseExtensionBlockL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCea861EdidParser::ParseExtensionBlockL( const TExtDataBlock& aData )
    {
    FUNC_LOG;

    TInt err( KErrNone );
    TInt index( 0 );

    // Check first byte to see if tag is 0x2.
    // If not return error.
    if( aData[index] != KBit1 )
        {
        return KErrNotSupported;
        }

    iParsedInfo->iExtensionTag = aData[index];
    index++;

    // Check revision
    iParsedInfo->iRevision = aData[index];
    index++;
    switch( iParsedInfo->iRevision )
        {
        case 1:
            err = ReadCeaVersion1L( aData, index );
            break;
        case 2:
            err = ReadCeaVersion2L( aData, index );
            break;
        case 3:
            err = ReadCeaVersion3L( aData, index );
            break;
        default:
            err = KErrNotSupported;
            break;
        }

    if( err == KErrNone )
        {
        iParsed = ETrue;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::isExtensionBlockParsed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::IsExtensionBlockParsed()
    {
    FUNC_LOG;

    return iParsed;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVIC
// ----------------------------------------------------------------------------
//
EXPORT_C TInt8 CCea861EdidParser::GetVIC( TUint8 aNumber )
    {
    FUNC_LOG;

    if( !iParsed )
        {
        return KErrNotFound;
        }

    if( IsVideoDataBlockSupported() )
        {
        return KErrNotFound;
        }

    if( iParsedInfo->iShortVideoDescriptors )
        {
        TCEA861VideoDataBlock* cur = iParsedInfo->iShortVideoDescriptors;
        TUint8 count = 0;
        while( cur )
            {
            if( cur->iNative )
                {
                if( count == aNumber )
                    {
                    return cur->iVIC;
                    }
                count++;
                }
            cur = cur->iNext;
            }
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::Underscan
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::Underscan()
    {
    FUNC_LOG;

    if( iParsedInfo->UnderscanDataSupported() )
        {
        return iParsedInfo->iUnderscan;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::BasicAudio
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::BasicAudio()
    {
    FUNC_LOG;

    if( iParsedInfo->AudioDataSupported() )
        {
        return iParsedInfo->iAudio;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::YCbCr444
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::YCbCr444()
    {
    FUNC_LOG;

    if( iParsedInfo->YCbCr444DataSupported() )
        {
        return iParsedInfo->iYCbCr444;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::YCbCr422
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::YCbCr422()
    {
    FUNC_LOG;

    if( iParsedInfo->YCbCr422DataSupported() )
        {
        return iParsedInfo->iYCbCr422;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::TotalNumberOfNativeFormats
//
// ----------------------------------------------------------------------------
//
EXPORT_C TUint8 CCea861EdidParser::TotalNumberOfNativeFormats()
    {
    FUNC_LOG;
    
    return iParsedInfo->TotalNumberOfNativeFormats();
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::IsAudioDataBlockSupported
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::IsAudioDataBlockSupported()
    {
    FUNC_LOG;

    return iAudioDataBlockSupported;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::IsVideoDataBlockSupported
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::IsVideoDataBlockSupported()
    {
    FUNC_LOG;

    return iVideoDataBlockSupported;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::IsVideoDataBlockSupported
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::IsVendorSpecificDataBlockSupported()
    {
    FUNC_LOG;

    return iVendorSpecificDataBlockSupported;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::HasIEEERegistration
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::HasIEEERegistration()
    {
    FUNC_LOG;

	if ( iVendorSpecificDataBlockSupported && iParsedInfo->iVendorSpecificData &&
		iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier == KIEEERegistrationID )
		{
		return ETrue;
		}

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetParsedInformation
//
// ----------------------------------------------------------------------------
//
EXPORT_C CCea861ExtEdidInformation* CCea861EdidParser::GetParsedInformation()
    {
    FUNC_LOG;

    return iParsedInfo;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::IsSpeakerAllocationDataBlockSupported
//
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCea861EdidParser::IsSpeakerAllocationDataBlockSupported()
    {
    FUNC_LOG;

    return iSpeakerAllocationDataBlockSupported;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVideoLatency
//
// ----------------------------------------------------------------------------
//
EXPORT_C TUint8 CCea861EdidParser::GetVideoLatency()
    {
    FUNC_LOG;

    // Video latency is kept in Vendor Specific Data Block according to
    // HDMISpecification13a.pdf, table 8-6
    TUint8 latency = 0;
    TCEA861VendorSpecificDataBlock* data = iParsedInfo->iVendorSpecificData;
    if( ( !data ) || ( !IsVendorSpecificDataBlockSupported() ) )
        {
        return latency;
        }

    TCEA861VendorSpecificDataBlockPayload* cur = data->iVendorSpecificPayloadStart;

    // According to HDMI spec table 8-6,  index of video latency is 9
    // Start counting from index 4 
    for( TInt ii = 0; ii < 6; ii++ )
        {
        if( !cur )
            {
            break;
            }
        
        // Byte 8
        else if( ii == 4 )
            {
            // Check if latency field bits are present
            if( !LatencyFieldsPresent( cur->iData ) )
                {
                // Latency fields are not present
                break;
                }
            }

        // Byte 9
        else if( ii == 5 )
            {
            // Get Video_latency byte
            latency = cur->iData;
            }

        cur = cur->iNext;
        }
    
    // Convert latency in to ms
    latency = LatencyInMs( latency );

    return latency;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetAudioLatency
//
// ----------------------------------------------------------------------------
//
EXPORT_C TUint8 CCea861EdidParser::GetAudioLatency()
    {
    FUNC_LOG;

    // Audio latency is kept in Vendor Specific Data Block according to
    // HDMISpecification13a.pdf, table 8-6
    TUint8 latency = 0;
    TCEA861VendorSpecificDataBlock* data = iParsedInfo->iVendorSpecificData;
    if( ( !data ) || ( !IsVendorSpecificDataBlockSupported() ) )
        {
        return latency;
        }

    TCEA861VendorSpecificDataBlockPayload* cur = data->iVendorSpecificPayloadStart;

    // According to HDMI spec table 8-6,  index of audio latency is 10
    // Start counting from index 4 
    for( TInt ii = 0; ii < 7; ii++ )
        {
        if( !cur )
            {
            break;
            }

        // Byte 8
        else if( ii == 4 )
            {
            // Check if latency field bits are present
            if( !LatencyFieldsPresent( cur->iData ) )
                {
                // Latency fields are not present
                break;
                }
            }
        
        // Byte 10
        else if( ii == 6 )
            {
            latency = cur->iData;
            }

        cur = cur->iNext;
        }

    // Convert latency in to ms
    latency = LatencyInMs( latency );

    return latency;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetInterlacedVideoLatency
//
// ----------------------------------------------------------------------------
//
EXPORT_C TUint8 CCea861EdidParser::GetInterlacedVideoLatency()
    {
    // Video latency is kept in Vendor Specific Data Block according to
    // HDMISpecification13a.pdf, table 8-6
    TUint8 latency = 0;
    TCEA861VendorSpecificDataBlock* data = iParsedInfo->iVendorSpecificData;
    if( ( !data ) || ( !IsVendorSpecificDataBlockSupported() ) )
        {
        return latency;
        }

    TCEA861VendorSpecificDataBlockPayload* cur = data->iVendorSpecificPayloadStart;

    // According to HDMI spec table 8-6,  index of video latency is 9 or 11
    // Start counting from index 4
    
    // Start index == 4, video latency == 9 (4 + 5)
    TInt videoLatencyByte = 5;
    for( TInt ii = 0; ii < 8; ii++ )
        {
        if( !cur )
            {
            break;
            }
        
        // Byte 8
        else if( ii == 4 )
            {
            // Check if latency field bits are present
            TUint8 byte8 = cur->iData;
            if( InterlacedLatencyFieldsPresent( byte8 ) )
                {
                // Interlaced latency fields are present
                // Start index == 4, interlaced video latency == 11 (4 + 7)
                videoLatencyByte = 7;
                }
            else if( !LatencyFieldsPresent( byte8 ) )
                {
                // Latency fields are not present
                break;
                }
            }

        // Byte 9 or Byte 11
        else if( ii == videoLatencyByte )
            {
            // Get Video_latency byte
            latency = cur->iData;
            }

        cur = cur->iNext;
        }

    // Convert latency in to ms
    latency = LatencyInMs( latency );

    return latency;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetInterlacedAudioLatency
//
// ----------------------------------------------------------------------------
//
EXPORT_C TUint8 CCea861EdidParser::GetInterlacedAudioLatency()
    {
    FUNC_LOG;

    // Audio latency is kept in Vendor Specific Data Block according to
    // HDMISpecification13a.pdf, table 8-6
    TUint8 latency = 0;
    TCEA861VendorSpecificDataBlock* data = iParsedInfo->iVendorSpecificData;
    if( ( !data ) || ( !IsVendorSpecificDataBlockSupported() ) )
        {
        return latency;
        }

    TCEA861VendorSpecificDataBlockPayload* cur = data->iVendorSpecificPayloadStart;

    // According to HDMI spec table 8-6,  index of audio latency is 10 or 12
    // Start counting from index 4
    
    // Start index == 4, audio latency == 10 (4 + 6)
    TInt audioLatencyByte = 6;
    for( TInt ii = 0; ii < 9; ii++ )
        {
        if( !cur )
            {
            break;
            }

        // Byte 8
        else if( ii == 4 )
            {
            // Check if interlaced latency field bits are present
            TUint8 byte8 = cur->iData;
            if( InterlacedLatencyFieldsPresent( byte8 ) )
                {
                // Start index == 4, interlaced audio latency == 12 (4 + 8)
                audioLatencyByte = 8;
                }
            
            // Check if latency field bits are present
            else if( !LatencyFieldsPresent( byte8 ) )
                {
                // Latency fields are not present
                break;
                }
            }
        
        // Byte 10 or Byte 12
        else if( ii == audioLatencyByte )
            {
            latency = cur->iData;
            }

        cur = cur->iNext;
        }

    // Convert latency in to ms
    latency = LatencyInMs( latency );

    return latency;
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::ReadCeaVersion1L
// ---------------------------------------------------------------------------
//
TInt CCea861EdidParser::ReadCeaVersion1L( const TExtDataBlock& aData,
    TInt aIndex )
    {
    FUNC_LOG;

    if( aIndex > KEdidParserSizeOfEdidBlock )
        {
        return KErrOverflow;
        }
        
    TCEA861TEdidDescriptorBlockList* lastnode = iParsedInfo->iDescriptorBlocks;

    TInt index = aIndex;
    while( index < KEdidParserSizeOfEdidBlock )
        {
        // Offset. If this is a newer version of CEA-861,
        // then this is skipped (we are already past the offset)
        if( index == 2 )
            {
            iParsedInfo->iOffset = aData[index];
            // Skip over the offset
            while( index < iParsedInfo->iOffset )
                {
                index++;
                }
            }

        if( aData[index] != KEdidPaddingByte ) // padding = 0x00
            {
            // start of 18-byte descriptors: See section 3.10.2 of E-EDID Standard [10]
            // read the descriptors here
            TEdidDescriptorBlock tmp = GetDescriptorBlock( aData, index );
			
            if( iParsedInfo->iDescriptorBlocks == 0 )
                {
                iParsedInfo->iDescriptorBlocks = new ( ELeave ) TCEA861TEdidDescriptorBlockList();
                lastnode = iParsedInfo->iDescriptorBlocks;
                }
            else
                {
                lastnode->iNext = new ( ELeave ) TCEA861TEdidDescriptorBlockList();
                lastnode = lastnode->iNext;
                }
            lastnode->iData = tmp;
            }
        else
            {
            // this is only padding, no need to do anything
            index++;
            }

        // after the descriptors there is padding to fill Extension block to 128 bytes.
        // beginning of padding               | End of padding  | Checksum
        // d+(18*n) ..........................| 126             | 127
        if( index == 0x7F )
            {
            iParsedInfo->iChecksum = aData[index];
            // Finished reading
            return KErrNone;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::ReadCeaVersion2L
// ---------------------------------------------------------------------------
//
TInt CCea861EdidParser::ReadCeaVersion2L( const TExtDataBlock& aData,
    TInt aIndex )
    {
    FUNC_LOG;

    ReadCeaVersion2and3Common( aData, aIndex );

    return ReadCeaVersion1L( aData, aIndex );
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::ReadCeaVersion3L
// ---------------------------------------------------------------------------
//
TInt CCea861EdidParser::ReadCeaVersion3L( const TExtDataBlock& aData,
    TInt aIndex )
    {
    FUNC_LOG;

    // first read the common information in versions 2 and 3
    ReadCeaVersion2and3Common( aData, aIndex );

    if( aIndex == 4 ) // start of the Data block collection (see table 39)
        {
        while( aIndex < iParsedInfo->iOffset ) // it is possible to have more than one DataBlockCollection
            {
            TInt ret = ReadCeaDataBlockCollectionL( aData, aIndex );
            if( ret != KErrNone )
                {
                return ret;
                }
            if( aIndex > KEdidParserSizeOfEdidBlock )
                {
                return KErrOverflow;
                }
            }
        // after reading the CEADataBlockCollections, we can read all the descriptors
        }
    else
        {
        // error
        if( aIndex < 4 )
            {
            return KErrNotFound;
            }
        else
            {
            return KErrOverflow;
            }
        }
    return ReadCeaVersion1L( aData, aIndex );
    }

// ---------------------------------------------------------------------------
// CCea861EdidParser::ReadCeaVersion2L
// ---------------------------------------------------------------------------
//
TInt CCea861EdidParser::ReadCeaVersion2and3Common( const TExtDataBlock& aData,
    TInt& aIndex )
    {
    FUNC_LOG;

    for( TInt i = aIndex; i < KEdidParserSizeOfEdidBlock; i++ )
        {
        if( aIndex == 2 ) // offset
            {
            iParsedInfo->iOffset = aData[i];
            // don't skip over the offset
            }
        if( aIndex == 3 )
            {
            // bit 7 = underscan: 1 if sink underscans IT video formats by default
            // bit 6 = audio    : 1 if sink supports basic audio
            // bit 5 = YCbCr 4:4:4 = 1 if sink supports YCbCr 4:4:4 in addition to RGB
            // bit 4 = YCbCr 4:2:2 = 1 if sink supports YCbCr 4:2:2 in addition to RGB
            // lower 4 bits = total number of native DTDs (detailed timing descriptors) (see section 2.2 for definition of "native format")
            iParsedInfo->iUnderscan = ( KBit7 & aData[i] ? ETrue : EFalse );
            iParsedInfo->iAudio = ( KBit6 & aData[i] ? ETrue : EFalse );
            iParsedInfo->iYCbCr444 = ( KBit5 & aData[i] ? ETrue : EFalse );
            iParsedInfo->iYCbCr422 = ( KBit4 & aData[i] ? ETrue : EFalse );
            iParsedInfo->iTotalNumberOfNativeFormats = 0x0F & aData[i];
            aIndex++;
            return KErrNone; // end of common data
            }
        aIndex++;
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetDescriptorBlock
//
// ----------------------------------------------------------------------------
//
TEdidDescriptorBlock CCea861EdidParser::GetDescriptorBlock( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    TEdidDescriptorBlock descBlock;

    descBlock.iPixelClock = GetPixelClock( aData, aIndex );

    descBlock.iHorizontalAddressableVideoPixels
        = GetHorizontalAddressableVideoPixels( aData, aIndex );

    descBlock.iHorizontalBlanking = GetHorizontalBlanking( aData, aIndex );

    descBlock.iVerticalAddressableVideoPixels
        = GetVerticalAddressableVideoPixels( aData, aIndex );

    descBlock.iVerticalBlanking = GetVerticalBlanking( aData, aIndex );

    descBlock.iHorizontalFrontPorch = GetHorizontalFrontPorch( aData, aIndex );

    descBlock.iHorizontalSyncPulse
        = GetHorizontalSyncPulseWidth( aData, aIndex );

    descBlock.iVerticalFrontPorch = GetVerticalFrontPorch( aData, aIndex );

    descBlock.iVerticalSyncPulse = GetVerticalSyncPulseWidth( aData, aIndex );

    descBlock.iHorizontalAddressableImageSize
        = GetHorizontalAddressableImageSizeInMm( aData, aIndex );

    descBlock.iVerticalAddressableImageSize
        = GetVerticalAddressableImageSizeInMm( aData, aIndex );

    descBlock.iHorizontalBorder = GetHorizontalBorderSize( aData, aIndex );

    descBlock.iVerticalBorder = GetVerticalBorderSize( aData, aIndex );

    descBlock.iInterlacedVideo = GetVideoIsInterlaced( aData, aIndex );

    descBlock.iStereoSupport = GetStereoViewingSupport( aData, aIndex );

    descBlock.iSyncs = GetSyncSignalDefinitions( aData, aIndex );

    return descBlock;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetPixelClock
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetPixelClock( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // Stored Value = Pixel clock ÷ 10,000
    // LSB stored in byte 0 and MSB stored in byte 1
    // 0, 1 2 Range: 10 kHz to 655.35 MHz in 10 kHz steps
    // (00 00)h Reserved: Do not use for Detailed Timing Descriptor
    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 1];

    TUint16 pixelClock = word2; // xxxx xxxx 1010 1010
    pixelClock = pixelClock << 8; // 1010 1010 xxxx xxxx shifted to left (MSB)
    pixelClock += word1; // 1010 1010 0101 0101 add the LSB

    aIndex += 2;

    return pixelClock;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalAddressableVideoPixels
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetHorizontalAddressableVideoPixels( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #2
    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 2];

    // 12bits long, defined in 8 bits of byte 2 and upper nibble of byte 4 (4bits), range 0-4095
    TUint16 horAddrVideo = 0;

    // nullify the 4 lsb
    // ---- ---- xxxx yyyy >> 4  = ---- ---- ---- xxxx
    horAddrVideo = word1 >> 4;
    // return back to original position (4)+4
    // ---- ---- ---- xxxx << 4  = ---- xxxx 0000 0000
    horAddrVideo = horAddrVideo << ( 4 + 4 );
    // ---- xxxx zzzz zzzz = correct values for the 12-bit-long var
    horAddrVideo += word2;

    aIndex++; // to byte #3

    return horAddrVideo;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalBlanking
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetHorizontalBlanking( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #3
    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 1];

    TUint16 horBlanking = 0;

    // nullify the 4 msb
    // ---- ---- xxxx yyyy << 12 = yyyy ---- ---- ----
    horBlanking = word1 << 12;
    // nullify the 4 msb
    // yyyy ---- ---- ---- >> 4  = ---- yyyy ---- ----
    horBlanking = horBlanking >> 4;
    // ---- yyyy zzzz zzzz = correct values for the 12-bit-long var
    horBlanking += word2;

    aIndex += 2; // to byte #5

    return horBlanking;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalAddressableVideoPixels
// Vertical Addressable Video in lines
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetVerticalAddressableVideoPixels( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #5
    TUint16 word1 = aData[aIndex + 2];
    TUint16 word2 = aData[aIndex];

    //Upper nibble of byte 7 and the 8 bits of byte 5) - Range is 0 lines to 4095 lines
    TUint16 vertAddrVideo = 0;
    vertAddrVideo = word1 >> 4;
    vertAddrVideo = vertAddrVideo << ( 4 + 4 );
    vertAddrVideo += word2; // like before with HorAddrVideo

    aIndex++; // to byte #6
    return vertAddrVideo;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalBlanking
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetVerticalBlanking( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #6
    TUint16 word1 = aData[aIndex + 1];
    TUint16 word2 = aData[aIndex];

    // 12bits long, defined in 8 bits of byte 2 and upper nibble of byte 4 (4bits), range 0-4095
    TUint16 vertBlanking = 0;

    // nullify the 4 msb
    // ---- ---- xxxx yyyy << 12 = yyyy ---- ---- ----
    vertBlanking = word1 << 12;
    // nullify the 4 msb
    // yyyy ---- ---- ---- >> 4  = 0000 yyyy ---- ----
    vertBlanking = vertBlanking >> 4;
    // ---- yyyy zzzz zzzz = correct values for the 12-bit-long var
    vertBlanking += word2;

    aIndex += 2; // to byte #8
    return vertBlanking;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalFrontPorch
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetHorizontalFrontPorch( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #8

    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 3]; // 2 bits from #11

    // Horizontal Front Porch in Pixels (from blanking start to start of sync) is represented by a 10 bit
    // number (Bits 7 & 6 of byte 11 and the 8 bits of byte 8) - Range is 0 pixels to 1023 pixels.

    TUint16 horFrontPorch = 0;

    TUint8 bit67 = word1;
    bit67 = ( bit67 & KBit7 ) | ( bit67 & KBit6 ); // bits 6 and 7 remain

    horFrontPorch = bit67 << 2;
    horFrontPorch += word2;

    aIndex++; // to byte #9
    return horFrontPorch;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalSyncPulseWidth
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetHorizontalSyncPulseWidth( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #9
    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 2]; // 2 bits from #11

    //Horizontal Sync Pulse Width in Pixels (from the end of the front porch to the start of the back
    //porch) is represented by a 10 bit number (Bits 5 & 4 of byte 11 and the 8 bits of byte 9) - Range
    //is 0 pixels to 1023 pixels.

    TUint16 horSyncPulseWidth = word1;
    horSyncPulseWidth = ( horSyncPulseWidth & KBit5 ) | ( horSyncPulseWidth
        & KBit4 ); // only bits 5 and 4 remain
    horSyncPulseWidth = horSyncPulseWidth << 4;
    horSyncPulseWidth += word2;

    aIndex++; // aIndex at byte #10

    return horSyncPulseWidth;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalFrontPorch
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetVerticalFrontPorch( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // Vertical Front Porch in Lines (from blanking start to start of sync) is represented by a 6 bit
    // number (Bits 3 & 2 of byte 11 and the upper nibble of byte 10) - Range is 0 lines to 63 lines.

    // aIndex at byte #10
    TUint16 word1 = aData[aIndex + 1];
    TUint16 word2 = aData[aIndex]; // 2 bits from #11

    TUint16 vertFrontPorch = 0;

    TUint8 bit23 = word1;
    // bits 2 and 3 remain  0000 xx00
    bit23 = ( bit23 & KBit3 ) | ( bit23 & KBit2 );

    // 00xx 0000
    vertFrontPorch = bit23 << 2;
    // 00xx yyyy
    vertFrontPorch += ( word2 >> 4 );

    //Index remains at byte #10

    return vertFrontPorch;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalSyncPulseWidth
//
// ----------------------------------------------------------------------------
//
TUint8 CCea861EdidParser::GetVerticalSyncPulseWidth( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // Vertical Sync Pulse Width in Lines (from the end of the front porch to the start of the back
    // porch) is represented by a 6 bit number (Bits 1 & 0 of byte 11 and the lower nibble of byte 10 -
    // Range is 0 lines to 63 lines.

    // aIndex at byte #10
    TUint16 word1 = aData[aIndex];
    TUint16 word2 = aData[aIndex + 1]; // 2 bits from #11

    // Vertical Sync Pulse Width in Lines (from the end of the front porch to the start of the back
    // porch) is represented by a 6 bit number (Bits 1 & 0 of byte 11 and the lower nibble of byte 10 -
    // Range is 0 lines to 63 lines.
    TUint8 vertSyncPulse = 0;

    // bits 1 and 0 remain   0000 00xx
    word2 = ( word2 & KBit1 ) | ( word2 & KBit0 );

    // 00xx 0000
    vertSyncPulse = word2 << 4;
    // only the lower nibble, %16 removes the upper nibble
    word1 = word1 % 16;
    // 00xx yyyy
    vertSyncPulse += word1;

    aIndex += 2; // to byte #12

    return vertSyncPulse;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalAddressableImageSizeInMm
// Horizontal Addressable Video Image Size in mm
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetHorizontalAddressableImageSizeInMm( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #12
    TUint16 word1 = aData[aIndex + 2];
    TUint16 word2 = aData[aIndex];

    //Horizontal Addressable Video Image Size in mm is represented by a 12 bit number (Upper
    //nibble of byte 14 and the 8 bits of byte 12) - Range is 0 mm to 4095 mm.

    // first nullify 4 lsb
    TUint16 hais = word1 >> 4;
    // and then move back the first 4 and then again 4
    hais = hais << ( 4 + 4 );
    hais += word2;

    aIndex++; // to byte #13

    return hais;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalAddressableImageSizeInMm
//
// ----------------------------------------------------------------------------
//
TUint16 CCea861EdidParser::GetVerticalAddressableImageSizeInMm( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #13
    TUint16 word1 = aData[aIndex + 1];
    TUint16 word2 = aData[aIndex];

    //Vertical Addressable Video Image Size in mm is represented by a 12 bit number (Lower nibble
    //of byte 14 and the 8 bits of byte 13) - Range is 0 mm to 4095 mm.

    TUint16 vais = word1;
    // first nullify 4 msb and then move back 1 nibble
    vais = vais << 12;
    vais = vais >> 4;
    vais += word2;

    aIndex += 2; // to byte #15

    return vais;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetHorizontalBorderSize
//
// ----------------------------------------------------------------------------
//
TUint8 CCea861EdidParser::GetHorizontalBorderSize( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #15
    TUint16 word1 = aData[aIndex];

    //Right Horizontal Border or Left Horizontal Border in Pixels is represented by an 8 bit number
    //(the 8 bits of byte 15) - Range is 0 pixels to 255 pixels.

    // the same value is used for (left and right) / (top and bottom)
    aIndex++; // to byte #16
    return word1;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVerticalBorderSize
// Top Vertical Border or Bottom Vertical Border in Lines
//
// ----------------------------------------------------------------------------
//
TUint8 CCea861EdidParser::GetVerticalBorderSize( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #16
    TUint16 word1 = aData[aIndex];

    //Top Vertical Border or Bottom Vertical Border in Lines is represented by an 8 bit number (the
    //8 bits of byte 16) - Range is 0 lines to 255 lines.

    aIndex++; // to byte #17
    return word1;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetVideoIsInterlaced
//
// ----------------------------------------------------------------------------
//
TBool CCea861EdidParser::GetVideoIsInterlaced( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #17
    TUint16 word1 = aData[aIndex];

    // byte 17
    //Bytes Bit Definitions Detailed Timing Definitions
    //7 6 5 4 3 2 1 0 Signal Interface Type:
    //0 _ _ _ _ _ _ _ Non-Interlaced (1 frame = 1 field)
    //1 _ _ _ _ _ _ _ Interlaced (1 frame = 2 fields)

    // Index remains at byte #17

    return KBit7 & word1;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetStereoViewingSupport
//
// ----------------------------------------------------------------------------
//
TEdidStereoViewingSupport CCea861EdidParser::GetStereoViewingSupport( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #17
    TUint16 word1 = aData[aIndex];

    // See TEdidStereoViewingSupport enum declaration

    // byte 17
    //Bytes Bit Definitions Detailed Timing Definitions
    //_ 6 5 _ _ _ _ 0 Stereo Viewing Support:
    //  0 0 _ _ _ _ x Normal Display – No Stereo. The value of bit 0 is "don't care"
    //  0 1 _ _ _ _ 0 Field sequential stereo, right image when stereo sync signal = 1
    //  1 0 _ _ _ _ 0 Field sequential stereo, left image when stereo sync signal = 1
    //  0 1 _ _ _ _ 1 2-way interleaved stereo, right image on even lines
    //  1 0 _ _ _ _ 1 2-way interleaved stereo, left image on even lines
    //  1 1 _ _ _ _ 0 4-way interleaved stereo
    //  1 1 _ _ _ _ 1 Side-by-Side interleaved stereo
    TUint8 bit17 = word1;

    // Index remains at byte #17

    if( !( ( KBit6 & bit17 ) | ( KBit5 & bit17 ) ) ) //  0 0 _ _ _ _ x Normal Display – No Stereo.
        { //                The value of bit 0 is "don't care"
        return ENormalDisplay;
        }
    else
        {
        if( !( KBit6 & bit17 ) && ( KBit5 & bit17 ) && !( KBit0 & bit17 ) ) //  0 1 _ _ _ _ 0 Field sequential stereo,
            { //                right image when stereo sync signal=1
            return EFieldSequentialStereoRightWhenStereoSyncSignal1;
            }
        else
            {
            if( ( KBit6 & bit17 ) && !( KBit5 & bit17 ) && !( KBit0 & bit17 ) ) //  1 0 _ _ _ _ 0 Field sequential stereo,
                { //                left image when stereo sync signal=1
                return EFieldSequentialStereoLeftWhenStereoSyncSignal1;
                }
            else
                {
                if( !( KBit6 & bit17 ) && ( KBit5 & bit17 ) && ( KBit0 & bit17 ) ) //  0 1 _ _ _ _ 1 2-way interleaved stereo,
                    { //                right image on even lines
                    return ETwoWayInterleavedStereoRightImageOnEvenLines;
                    }
                else
                    {
                    if( ( KBit6 & bit17 ) && !( KBit5 & bit17 ) && ( KBit0
                        & bit17 ) )//  1 0 _ _ _ _ 1 2-way interleaved stereo,
                        { //                left image on even lines
                        return ETwoWayInterleavedStereoLeftImageOnEvenLines;
                        }
                    else
                        {
                        if( ( KBit6 & bit17 ) && ( KBit5 & bit17 ) && !( KBit0
                            & bit17 ) ) //  1 1 _ _ _ _ 0 4-way interleaved stereo
                            {
                            return EFourWayInterleaverStereo;
                            }
                        else
                            {
                            if( ( KBit6 & bit17 ) && ( KBit5 & bit17 )
                                && ( KBit0 & bit17 ) ) //  1 1 _ _ _ _ 1 Side-by-Side interleaved stereo
                                {
                                return ESideBySideInterleavedStere;
                                }
                            else
                                {
                                return EUnknownStereoViewingSupport;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetAnalogSyncSignalDefinitions
//
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CCea861EdidParser::GetSyncSignalDefinitions( const TExtDataBlock& aData,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    // aIndex at byte #18
    TUint8 byte17 = aData[aIndex];

    // byte 17
    // 4 3 2 1 _ Analog Sync Signal Definitions:
    //------------------------------------------
    // 0 0 _ _ _ Analog Composite Sync:
    // 0 1 _ _ _ Bipolar Analog Composite Sync:

    aIndex++; // advance to byte #18 finally -> this descriptor block is now finished

    if( !( KBit4 & byte17 ) ) // if bit4 == 0 then this is analog display
        {
        return GetAnalogSyncSignalDefinitions( byte17 );
        }

    return GetDigitalSyncSignalDefinitions( byte17 );
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetAnalogSyncSignalDefinitions
//
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CCea861EdidParser::GetAnalogSyncSignalDefinitions( const TUint8 aByte17 )
    {
    FUNC_LOG;

    // byte 17
    // 4 3 2 1 _ Analog Sync Signal Definitions:
    //------------------------------------------
    // 0 0 _ _ _ Analog Composite Sync:
    // 0 1 _ _ _ Bipolar Analog Composite Sync:
    // 0 _ 0 _ _ ---------- Without Serrations;
    // 0 _ 1 _ _ ---------- With Serrations (H-sync during V-sync);
    // 0 _ _ 0 _ -------------------- Sync On Green Signal only
    // 0 _ _ 1 _ -------------------- Sync On all three (RGB) video signals

    TBool ACS = !( KBit3 & aByte17 ); // if true -> bipolar analog composite sync
    TBool serrations = KBit2 & aByte17;
    TBool syncOnGreen = !( KBit1 & aByte17 );

    if( ACS )
        {
        if( !serrations )
            {
            if( syncOnGreen )
                {
                return EAnalogCompositeSyncWithoutSerrationsSyncSyncOnGreenSignalOnly;
                }
            else
                {
                return EAnalogCompositeSyncWithoutSerrationsSyncOnAllThreeVideoSignals;
                }
            }
        else
            {
            if( syncOnGreen )
                {
                return EAnalogCompositeSyncWithoutSerrationsSyncSyncOnGreenSignalOnly;
                }
            else
                {
                return EAnalogCompositeSyncWithoutSerrationsSyncOnAllThreeVideoSignals;
                }
            }
        }
    else
        {
        if( !serrations )
            {
            if( syncOnGreen )
                {
                return EAnalogBipolarAnalogCompositeSyncWithoutSerrationsSyncSyncOnGreenSignalOnly;
                }
            else
                {
                return EAnalogBipolarAnalogCompositeSyncWithoutSerrationsSyncOnAllThreeVideoSignals;
                }
            }
        else
            {
            if( syncOnGreen )
                {
                return EAnalogBipolarAnalogCompositeSyncWithSerrationsSyncSyncOnGreenSignalOnly;
                }
            else
                {
                return EAnalogBipolarAnalogCompositeSyncWithSerrationsSyncOnAllThreeVideoSignals;
                }
            }
        }
    // unreachable
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::GetDigitalSyncSignalDefinitions
//
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CCea861EdidParser::GetDigitalSyncSignalDefinitions( const TUint8 aByte17 )
    {
    FUNC_LOG;

    //        4 3 2 1 0 bit
    //        --------------
    //        1 0 _ _ _ Digital Composite Sync:
    //        1 0 0 _ _ ---------- Without Serrations;
    //        1 0 1 _ _ ---------- With Serrations (H-sync during V-sync);
    //        1 1 _ _ _ Digital Separate Sync:
    //        1 1 0 _ _ ---------- Vertical Sync is Negative;
    //        1 1 1 _ _ ---------- Vertical Sync is Positive;
    //        1 _ _ 0 _ -------------------- Horizontal Sync is Negative (outside of V-sync)
    //        1 _ _ 1 _ -------------------- Horizontal Sync is Positive (outside of V-sync)

    TBool digComp = !( KBit3 & aByte17 ); // ETrue == digital separate sync
    TBool bit2 = KBit2 & aByte17;

    if( digComp ) // digital composite sync
        {
        if( !bit2 ) // without serrations
            {
            return EDigitalCompositeSyncWithoutSerrations;
            }
        else // with serrations
            {
            return EDigitalCompositeSyncWithSerrations;
            }
        } // endif  digital composite sync
    else //        digital separate sync
        {
        TBool bit1 = KBit1 & aByte17;

        if( !bit2 ) // vertical = neg
            {
            if( !bit1 )
                { // vertical = neg, horizontal = neg
                return EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsNegative;
                }
            else
                { // vertical = neg, horizontal = pos
                return EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsPositive;
                }
            }
        else // vertical = pos
            {
            if( !bit1 )
                { // vertical = pos, horizontal = neg
                return EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsNegative;
                }
            else
                { // vertical = pos, horizontal = pos
                return EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsPositive;
                }
            }
        }
    // unreachable
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadCeaDataBlockCollectionL
//
// ----------------------------------------------------------------------------
//
TInt CCea861EdidParser::ReadCeaDataBlockCollectionL( const TExtDataBlock& aData,
    TInt& aIndex )
    {
    FUNC_LOG;

    // bits 5-7         | bits 0-4
    // Tag Code         | Length = total number of video bytes following this byte (L1)
    TInt tag = 0; // video tag code
    TInt L1 = 0; // lenght (L1)
    if( KBit7 & aData[aIndex] )
        {
        tag += 4;
        }
    if( KBit6 & aData[aIndex] )
        {
        tag += 2;
        }
    if( KBit5 & aData[aIndex] )
        {
        tag += 1;
        }
    L1 = aData[aIndex] & 0x1F; // clear the 3 MSBits

    aIndex++; // jump over the first byte

    TInt auxTag = 0;
    switch( tag )
        {
        case 0:
            //reserved
            ReadUnknownTagCode( aData, aIndex, L1 );
            break;
        case 1:
            ReadCea861ShortAudioDataBlockL( aData, aIndex, L1 );
            break;
        case 2:
            ReadCea861ShortVideoDataBlockL( aData, aIndex, L1 );
            break;
        case 3:
            ReadCea861VendorSpecificDataBlockL( aData, aIndex, L1 );
            break;
        case 4:
            ReadCea861SpeakerAllocationDataBlock( aData, aIndex, L1 );
            break;
        case 5:
            //DTC Data Block
            ReadUnknownTagCode( aData, aIndex, L1 );
            break;
        case 6:
            //reserved
            ReadUnknownTagCode( aData, aIndex, L1 );
            break;
        case 7:
            //use extended tag (second byte holds the extended tag-code)
            // (see table 43 in CEA-861 spec) (if using repeater, then these information must be re-transmitted)
            auxTag = aData[aIndex + 1];
            if( auxTag == 5 ) // colorimetry, see table 51, table 52 and table 53 in CEA-861-E.pdf
                {
                }
            else if( auxTag == 0x07 ) // video Capability Data Block, see table 54
                {
                ReadVideoCapabilityDataBlockL( aData, aIndex, L1 );
                }
            else if( auxTag == 0x01 ) // Vendor-Specific Video Data Block, see table 56
                {
                }
            else if( auxTag == 0x11 ) // Vendor-Specific Audio Data Block, see table 57
                {
                }
            // TODO: these should be read as well, not just as unknown
            ReadUnknownTagCode( aData, aIndex, L1 );
            break;
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadCea861ShortAudioDataBlockL
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadCea861ShortAudioDataBlockL( const TExtDataBlock& aData,
    TInt& aIndex,
    const TInt8 aLen )
    {
    FUNC_LOG;
    TBool first = EFalse;

    iAudioDataBlockSupported = ETrue;

    if( !iParsedInfo->iShortAudioDescriptors ) // linked list
        {
        iParsedInfo->iShortAudioDescriptors
            = new ( ELeave ) TCEA861AudioDataBlock();
		first = ETrue;
		iParsedInfo->iShortAudioDescriptors->iNext = 0;
        }
    TCEA861AudioDataBlock* cur = iParsedInfo->iShortAudioDescriptors;

    while( cur->iNext != 0 )
        {
        cur = cur->iNext; // jump to the end
        }

	TInt i = 0;
	while (i < aLen)
        {
        // read aLen-amount of short video descriptors

        // first link is a special case
        if( ( iParsedInfo->iShortAudioDescriptors == cur ) && first )
            {
            first = EFalse;
            cur->iByte1 = aData[aIndex];
            cur->iByte2 = aData[aIndex + 1];
            cur->iByte3 = aData[aIndex + 2];
            DetermineAudioBlockInformation( cur );
            aIndex += 3;
            i += 3;
            continue;
            }

        // create new data blocks
        cur->iNext = new ( ELeave ) TCEA861AudioDataBlock();
        cur = cur->iNext;
        cur->iByte1 = aData[aIndex];
        cur->iByte2 = aData[aIndex + 1];
        cur->iByte3 = aData[aIndex + 2];
        cur->iNext = 0;

        DetermineAudioBlockInformation( cur );

        aIndex += 3;
        i += 3;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadCea861ShortVideoDataBlockL
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadCea861ShortVideoDataBlockL( const TExtDataBlock& aData,
    TInt& aIndex,
    const TInt8 aLen )
    {
    FUNC_LOG;

    TBool first = EFalse;
    iVideoDataBlockSupported = ETrue;
    if( !iParsedInfo->iShortVideoDescriptors ) // linked list
        {
        iParsedInfo->iShortVideoDescriptors
            = new ( ELeave ) TCEA861VideoDataBlock();
        iParsedInfo->iShortVideoDescriptors->iNext = 0; // make sure there are no stray pointers
        first = ETrue;
        }

    TCEA861VideoDataBlock* cur = iParsedInfo->iShortVideoDescriptors;
    while( cur->iNext != 0 )
        {
        cur = cur->iNext; // jump to the end
        }

    for( int i = 0; i < aLen; i++ )
        {

        // read aLen-amount of short video descriptors

        // first link is a special case
        if( ( iParsedInfo->iShortVideoDescriptors == cur ) && first )
            {
            first = EFalse;
            cur->iNative = KBit7 & aData[aIndex];
            cur->iVIC = ( ~KBit7 ) & aData[aIndex]; // nullify the most signicant bit
            cur->iNext = 0;
            aIndex++;
            continue;
            }

        // create new data blocks
        cur->iNext = new ( ELeave ) TCEA861VideoDataBlock();
        cur = cur->iNext;
        cur->iNative = KBit7 & aData[aIndex];
        cur->iVIC = ( ~KBit7 ) & aData[aIndex]; // nullify the most signicant bit
        cur->iNext = 0;
        aIndex++;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadCea861VendorSpecificDataBlockL
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadCea861VendorSpecificDataBlockL( const TExtDataBlock& aData,
    TInt& aIndex,
    TInt8 aLen )
    {
    FUNC_LOG;

    iVendorSpecificDataBlockSupported = ETrue;

    if( !iParsedInfo->iVendorSpecificData )
        {
        iParsedInfo->iVendorSpecificData = new ( ELeave ) TCEA861VendorSpecificDataBlock();
        }


    TUint8 data1 = aData[aIndex]; // least significant byte first
    TUint8 data2 = aData[aIndex + 1];
    TUint8 data3 = aData[aIndex + 2];
    aIndex += 3; // three bytes of IEEE registration identifier

    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier = 0; // convert from LSB -> MSB
    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier += data3;
    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier
        = iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier
            << 8;
    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier += data2;
    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier
        = iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier
            << 8;
    iParsedInfo->iVendorSpecificData->iIEEE24bitRegistrationIdentifier += data1;

    aLen -= 3; // this is needed: Vendor specific payload length = L4-3bytes

    TBool first = EFalse;
    if( iParsedInfo->iVendorSpecificData->iVendorSpecificPayloadStart == 0 )
        {
        iParsedInfo->iVendorSpecificData->iVendorSpecificPayloadStart
            = new ( ELeave ) TCEA861VendorSpecificDataBlockPayload();
		iParsedInfo->iVendorSpecificData->iVendorSpecificPayloadStart->iNext = 0;
		first = ETrue;
        }
    TCEA861VendorSpecificDataBlockPayload* cur =
        iParsedInfo->iVendorSpecificData->iVendorSpecificPayloadStart;
    // traverse to the end of the list
    while( cur->iNext != 0 )
        {
        cur = cur->iNext;
        }

    for( int i = 0; i < aLen; i++ )
        {
        if( ( cur == iParsedInfo->iVendorSpecificData->iVendorSpecificPayloadStart )
            && first ) // first byte is special case
            {
            first = EFalse;
            cur->iData = aData[aIndex];
            cur->iNext = NULL;
            aIndex++;
            continue;
            }
        cur->iNext = new ( ELeave ) TCEA861VendorSpecificDataBlockPayload();
        cur = cur->iNext;
        cur->iData = aData[aIndex];
        cur->iNext = 0;
        aIndex++;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadCea861SpeakerAllocationDataBlock
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadCea861SpeakerAllocationDataBlock( const TExtDataBlock& aData,
    TInt& aIndex,
    const TInt8 aLen )
    {
    FUNC_LOG;

    // read the 3 bytes
    if( aLen != 3 )
        {
        return;
        }

    iSpeakerAllocationDataBlockSupported = ETrue;

    for( TInt i = 0; i < aLen; i++ )
        {
        switch( i )
            {
            case 0:
                iParsedInfo->iSpeakerAllocationData.iByte1 = aData[aIndex];
                break;
            case 1:
                iParsedInfo->iSpeakerAllocationData.iByte2 = aData[aIndex];
                break;
            case 2:
                iParsedInfo->iSpeakerAllocationData.iByte3 = aData[aIndex];
                break;
            default:
                // error
                break;
            }
        aIndex++;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::ReadVideoCapabilityDataBlockL
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadVideoCapabilityDataBlockL( const TExtDataBlock& aData,
    TInt& aIndex,
    const TInt8 aLen )
    {
    FUNC_LOG;
	TBool first = EFalse;

    aIndex++; // jump to the extended tag code (aLen is the length from extended tag to the end)

    if( iParsedInfo->iVideoCapabilityDataBlock == 0 )
        {
        iParsedInfo->iVideoCapabilityDataBlock
            = new ( ELeave ) TCEA861VideoCapabilityDataBlock();
		first = ETrue;
        }
    TCEA861VideoCapabilityDataBlock* cur =
        iParsedInfo->iVideoCapabilityDataBlock;
    while( cur->iNext != 0 )
        {
        cur = cur->iNext;
        }

    // Payload currently only contains a single byte in addition to the extended tag code. The Source should ignore such additional bytes when present.
    for( int i = 0; i < aLen; i++ )
        {
        if( i == 1 )
            {
            if( !first ) // if this wasn't the first time we are reading to this
                {
                cur->iNext = new ( ELeave ) TCEA861VideoCapabilityDataBlock(); // sets Next to 0
                cur = cur->iNext;
                }
            cur->iInitialized = ETrue;
            cur->iQuantizationRange = aData[aIndex] & KBit7;
            cur->iQuantizationRangeSelectable = aData[aIndex] & KBit6;
            cur->iS_PT1 = aData[aIndex] & KBit5;
            cur->iS_PT0 = aData[aIndex] & KBit4;
            cur->iS_IT1 = aData[aIndex] & KBit3;
            cur->iS_IT0 = aData[aIndex] & KBit2;
            cur->iS_CE1 = aData[aIndex] & KBit1;
            cur->iS_CE0 = aData[aIndex] & KBit0;
            cur->iNext = 0;
            }
        aIndex++;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::readUnknownTagCode
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::ReadUnknownTagCode( const TExtDataBlock& /*aData*/,
    TInt& aIndex,
    const TInt8 aLen )
    {
    FUNC_LOG;

    // Skip the data. This must be done if the tag is unknown
    aIndex += aLen;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::DetermineAudioBlockInformation
//
// ----------------------------------------------------------------------------
//
void CCea861EdidParser::DetermineAudioBlockInformation( TCEA861AudioDataBlock* aAudioBlock )
    {
    FUNC_LOG;

    // x6543xxx we need the information in the bytes 6543
    aAudioBlock->iAudioFormatCode = ( KBit3 | KBit2 | KBit1 | KBit0 )
        & ( aAudioBlock->iByte1 >> 3 );
    aAudioBlock->iSupport192kHz = ( aAudioBlock->iByte2 & KBit6 ? ETrue : EFalse );
    aAudioBlock->iSupport176kHz = ( aAudioBlock->iByte2 & KBit5 ? ETrue : EFalse );
    aAudioBlock->iSupport96kHz = ( aAudioBlock->iByte2 & KBit4 ? ETrue : EFalse );
    aAudioBlock->iSupport88kHz = ( aAudioBlock->iByte2 & KBit3 ? ETrue: EFalse );
    aAudioBlock->iSupport48kHz = ( aAudioBlock->iByte2 & KBit2 ? ETrue : EFalse );
    aAudioBlock->iSupport44kHz = ( aAudioBlock->iByte2 & KBit1 ? ETrue : EFalse );
    aAudioBlock->iSupport32kHz = ( aAudioBlock->iByte2 & KBit0 ? ETrue : EFalse );
    aAudioBlock->iMaxChannels = aAudioBlock->iByte1 & 0x07; // only the 3 LSB is used
    //TODO: 3rd byte shall be read according to the audioformatcode (see table 45 in CEA-861-E.pdf)
    switch( aAudioBlock->iAudioFormatCode )
        {
        case 1:
            aAudioBlock->iSupport24Bit = ( aAudioBlock->iByte3 & KBit2 ? ETrue : EFalse );
            aAudioBlock->iSupport20Bit = ( aAudioBlock->iByte3 & KBit1 ? ETrue : EFalse );
            aAudioBlock->iSupport16Bit = ( aAudioBlock->iByte3 & KBit0 ? ETrue : EFalse );
            break;
        case 2: //cases 2-8
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            aAudioBlock->iMaxBitrate = aAudioBlock->iByte3 * 8;
            break;
        case 9: // cases 9-13
        case 10:
        case 11:
        case 12:
        case 13:
            // byte3 is Audio format code dependent value
            break;
        case 14:
            // byte3 bits 0,1,2 is profile. bits 3-7 are reserved.
            break;
        case 15: // bits 7-3 = audio format code extension, bits 2-0
            aAudioBlock->iAudioFormatCodeExtension = aAudioBlock->iByte3 >> 3;
            break;
        default:
            // error
            break;
        }
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::LatencyFieldsPresent
//
// ----------------------------------------------------------------------------
//
TBool CCea861EdidParser::LatencyFieldsPresent( TUint8 aByte ) const
    {
    FUNC_LOG;
    
    TBool present = EFalse;
    
    // Latency_Fields_Preset bit is bit6 (01000000) in the aByte
    if( aByte & 0x40 )
        {
        present = ETrue;
        }
    return present;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::InterlacedLatencyFieldsPresent
//
// ----------------------------------------------------------------------------
//
TBool CCea861EdidParser::InterlacedLatencyFieldsPresent( TUint8 aByte ) const
    {
    FUNC_LOG;

    TBool present = EFalse;
    
    // Latency_Fields_Preset bit is bit7 (10000000) in the aByte
    if( aByte & 0x80 )
        {
        present = ETrue;
        }
    return present;
    }

// ----------------------------------------------------------------------------
// CCea861EdidParser::InterlacedLatencyFieldsPresent
//
// ----------------------------------------------------------------------------
//
TUint8 CCea861EdidParser::LatencyInMs( TUint8 aByte ) const
    {
    FUNC_LOG;
    
    TUint8 latency = 0;
    if( aByte > 0 && aByte <= 251 )
        {
        latency = ( aByte - 1 ) * 2;
        }
    
    return latency;
    }

// End of file
