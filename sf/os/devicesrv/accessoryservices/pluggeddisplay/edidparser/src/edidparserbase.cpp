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
 * Implementation of CEdidParserBase class.
 *
 */

#include "trace.h"
#include "edidparserbase.h"
#include "cea861edidparser.h"
#include "traceediddata.h"

// ---------------------------------------------------------------------------
// CEdidParserBase::CEdidParserBase
// ---------------------------------------------------------------------------
//
CEdidParserBase::CEdidParserBase() :
    iNumberOfExtensionBlocks( 0 )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::ConstructL
// ---------------------------------------------------------------------------
//
void CEdidParserBase::ConstructL( const TDesC8& aBinaryData )
    {
    FUNC_LOG;

    //Check if it is EDID data
    if( !IsEdidData( aBinaryData ) )
        {
        // Not EDID data
        User::Leave( KErrNotSupported );
        }

    iRawData = HBufC8::NewL( aBinaryData.Length() );
    TPtr8 rawPtr = iRawData->Des();
    rawPtr.Copy( aBinaryData );

    // Make a copy of base EDID data
    Mem::Copy( &iBaseEdidData, rawPtr.Ptr(), KEdidParserSizeOfEdidBlock );

	iRawdataLength = aBinaryData.Length();
	
    //TRACE_EDID_DATA( *this );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEdidParserBase* CEdidParserBase::NewL( const TDesC8& aBinaryData )
    {
    FUNC_LOG;

    CEdidParserBase* self = CEdidParserBase::NewLC( aBinaryData );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEdidParserBase* CEdidParserBase::NewLC( const TDesC8& aBinaryData )
    {
    FUNC_LOG;

    CEdidParserBase* self = new ( ELeave ) CEdidParserBase();
    CleanupStack::PushL( self );
    self->ConstructL( aBinaryData );

    return self;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::CEdidParserBase
// ---------------------------------------------------------------------------
//
CEdidParserBase::~CEdidParserBase()
    {
    FUNC_LOG;

    delete iRawData;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::IsEdidData
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CEdidParserBase::IsEdidData( const TDesC8& aBinaryData )
    {
    FUNC_LOG;
    INFO_1( "TBool CEdidParserBase::IsEdidData: aBinaryData length: %d", aBinaryData.Length() );

    TBool edidData = EFalse;

    // Check the length
    if( aBinaryData.Length() >= KEdidParserSizeOfEdidBlock )
        {
        for( int ii = 0; ii < KEdidParserSizeOfEdidHeader; ii++ )
            {
            if( aBinaryData[ii] != KBaseEdidHeader[ii] )
                {
                    // Not a valid EDID header
                ERROR_1( KErrArgument, "TBool CEdidParserBase::IsEdidData: Not a valid EDID header: %d", ii );
                return EFalse;
                }
            }
        edidData = ETrue;
        }

    return edidData;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetManufacturerCode
// ---------------------------------------------------------------------------
//
EXPORT_C void CEdidParserBase::GetManufacturerCode( TDes& aCode )
    {
    FUNC_LOG;

    if( aCode.Length() < KEdidParserManufacturerCodeLength )
        {
        //return;
        }

    TUint16 data = iBaseEdidData.iVendorProductInfo.iManufacturerCode;

    // Manufacturer codes are also called the ISA (Industry Standard Architecture)
    // Plug and Play Device Identifier (PNPID). They are based on 5 bit compressed
    // ASCII codes
    // bits 14-10: first letter (01h='A', 02h='B',..)
    // bits 9-5: second letter
    // bits 4-0: third letter

    // Convert to big endian
    data = ( ( data >> 8 ) & 0xff ) + ( ( data << 8 ) & 0xff00 );

    // Get ASCII codes
    TChar asciicode;
    asciicode = ( ( data >> 10 ) & 0x1f ) + 'A' - 1;
    aCode.Append( asciicode );
    asciicode = ( ( data >> 5 ) & 0x1f ) + 'A' - 1;
    aCode.Append( asciicode );
    asciicode = ( data & 0x1f ) + 'A' - 1;
    aCode.Append( asciicode );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetProductId
// ---------------------------------------------------------------------------
//
EXPORT_C void CEdidParserBase::GetProductId( TDes& aCode )
    {
    FUNC_LOG;

    aCode.AppendFormat( _L( "%04x" ), iBaseEdidData.iVendorProductInfo.iProductId );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetManufactureWeek
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetManufactureWeek()
    {
    FUNC_LOG;

    return iBaseEdidData.iVendorProductInfo.iWeek;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetManufactureYear
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetManufactureYear()
    {
    FUNC_LOG;

    // Year of manufacture stored in EDID data is "Year of Manufacture - 1990"
    return ( iBaseEdidData.iVendorProductInfo.iYear + KEdidBaseYear );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetVersion
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetVersion()
    {
    FUNC_LOG;

    return iBaseEdidData.iVersion;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetRevision
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetRevision()
    {
    FUNC_LOG;

    return iBaseEdidData.iRevision;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetVideoInputDefinition
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetVideoInputDefinition()
    {
    FUNC_LOG;

    // Analog if bit 7 is 0. Digital if bit 7 is 1
    return ( ( iBaseEdidData.iVideoInputDefinition & KBit7 ) == KBit7 ? 1 : 0 );
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetHorizontalScreenSize
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetHorizontalScreenSize()
    {
    FUNC_LOG;

    return iBaseEdidData.iScreenSize.iHorizontal;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetVerticalScreenSize
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetVerticalScreenSize()
    {
    FUNC_LOG;

    return iBaseEdidData.iScreenSize.iVertical;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetAspectRatioLandscape
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetAspectRatioLandscape()
    {
	FUNC_LOG;
	
    // TEdidAspectRatio contains same values as TEdidScreenSize
    // Horizontal = Landscape 
    return iBaseEdidData.iScreenSize.iHorizontal;
    }


// ---------------------------------------------------------------------------
// CEdidParserBase::GetAspectRatioPortrait
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetAspectRatioPortrait()
    {
	FUNC_LOG;
	
    // TEdidAspectRatio contains same values as TEdidScreenSize
    // Vertical = Portrait 
    return iBaseEdidData.iScreenSize.iVertical;
    }


// ---------------------------------------------------------------------------
// CEdidParserBase::IsAspectRationUsed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CEdidParserBase::IsAspectRatioUsed()
    {
    FUNC_LOG;

    // If byte 15h != 00h then byte 16h = Vertical Screen Size in cm.     (Range is 1 cm -> 255 cm)
    // If byte 15h == 00h then byte 16h = Aspect Ratio (Portrait)         (Range is 0.28 : 1 AR -> 0.99 : 1 AR)
    //
    // If byte 16h != 00h then byte 15h = Horizontal Screen Size in cm.   (Range is 1 cm -> 255 cm)
    // If byte 16h == 00h then byte 15h = Aspect Ratio (Landscape)        (Range is 1 : 1 AR -> 3.54 : 1 AR)

    if( ( iBaseEdidData.iScreenSize.iHorizontal == 0 )
        || ( iBaseEdidData.iScreenSize.iVertical == 0 ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::DisplayTransferCharacteristics
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::DisplayTransferCharacteristics()
    {
    FUNC_LOG;

    // Stored Value = (GAMMA x 100) – 100
    return iBaseEdidData.iDisplayTransferCharasteristics;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesRedX
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesRedX()
    {
    FUNC_LOG;

    // Address 19h bits 7&6 are RedX bits 1&0
    // Address 1Bh = RedX bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[2]; //1Bh
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit7 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit6 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesRedY
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesRedY()
    {
    FUNC_LOG;

    // Address 19h bits 5&4 are RedY bits 1&0
    // Address 1Ch = RedY bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[3]; //1Ch
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit5 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit4 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesGreenX
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesGreenX()
    {
    FUNC_LOG;

    // Address 19h bits 3&2 are GreenX bits 1&0
    // Address 1Dh = GreenX bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[4]; //1Dh
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit3 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit2 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesGreenY
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesGreenY()
    {
    FUNC_LOG;

    // Address 19h bits 1&0 are GreenY bits 1&0
    // Address 1Eh = GreenY bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[5]; //1Dh
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit1 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[0] & KBit0 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesBlueX
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesBlueX()
    {
    FUNC_LOG;

    // Address 1Ah bits 7&6 are BlueX bits 1&0
    // Address 1Fh = BlueX bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[6]; //1Fh
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit7 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit6 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesBlueY
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesBlueY()
    {
    FUNC_LOG;

    // Address 1Ah bits 5&4 are BlueY bits 1&0
    // Address 20h = BlueY bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[7]; //20h
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit5 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit4 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesWhiteX
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesWhiteX()
    {
    FUNC_LOG;

    // Address 1Ah bits 3&2 are WhiteX bits 1&0
    // Address 21h = WhiteX bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[8]; //21h
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit3 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit2 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetColorCoordinatesWhiteY
// ---------------------------------------------------------------------------
//
EXPORT_C TUint16 CEdidParserBase::GetColorCoordinatesWhiteY()
    {
    FUNC_LOG;

    // Address 1Ah bits 3&2 are WhiteX bits 1&0
    // Address 22h = WhiteX bits 9 to 2
    TUint16 ret = iBaseEdidData.iColorCharasteristics[9]; //22h
    ret = ret << 2;
    TUint8 add = 0;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit1 )
        {
        add++;
        }
    add = add << 1;
    if( iBaseEdidData.iColorCharasteristics[1] & KBit0 )
        {
        add++;
        }
    ret += add;
    return ret;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetEstablishedTimings1
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetEstablishedTimings1()
    {
    FUNC_LOG;

    //Bit - Established Timing I
    // 7     720 x 400  @ 70Hz
    // 6     720 x 400  @ 88Hz
    // 5     640 x 480  @ 60Hz
    // 4     640 x 480  @ 67Hz
    // 3     640 x 480  @ 72Hz
    // 2     640 x 480  @ 75Hz
    // 1     800 x 600  @ 56Hz
    // 0     800 x 600  @ 60Hz
    return iBaseEdidData.iEstablishedTimings[0];
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetEstablishedTimings2
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetEstablishedTimings2()
    {
    FUNC_LOG;

    //Bit - Established Timing I
    // 7     800 x 600    @ 72Hz
    // 6     800 x 600    @ 75Hz
    // 5     832 x 624    @ 75Hz
    // 4     1024 x 768   @ 87Hz
    // 3     1024 x 768   @ 60Hz
    // 2     1024 x 768   @ 70Hz
    // 1     1024 x 768   @ 75Hz
    // 0     1280 x 1024  @ 75Hz
    return iBaseEdidData.iEstablishedTimings[1];
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetManufacturerTimings
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetManufacturerTimings()
    {
    FUNC_LOG;

    //Bit - Manufacturer Timing
    // 7     1152 x 870   @ 75Hz
    // 6-0   Reserved for Manufacturer Specified Timings
    return iBaseEdidData.iEstablishedTimings[2];
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetManufacturerTimings
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetStandardTimings( TInt aIndex )
    {
    FUNC_LOG;
    
    TUint8 byte = 0;
    if( aIndex >= 0 && aIndex < 15 )
        {
        byte = iBaseEdidData.iStdTimingIdentifications[aIndex];
        }
    
    return byte;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetDescriptorBlock
// ----------------------------------------------------------------------------
//
EXPORT_C TEdidDescriptorBlock CEdidParserBase::GetDescriptorBlock( TUint8 aDescriptorBlockNumber ) const
    {
    FUNC_LOG;

    TEdidDescriptorBlock descBlock;

    descBlock.iPixelClock = GetPixelClock( aDescriptorBlockNumber );

    descBlock.iHorizontalAddressableVideoPixels
        = GetHorizontalAddressableVideo( aDescriptorBlockNumber );

    descBlock.iHorizontalBlanking
        = GetHorizontalBlanking( aDescriptorBlockNumber );

    descBlock.iVerticalAddressableVideoPixels
        = GetVerticalAddressableVideo( aDescriptorBlockNumber );
    
    descBlock.iVerticalBlanking
            = GetVerticalBlanking( aDescriptorBlockNumber );
    
    descBlock.iHorizontalFrontPorch
        = GetHorizontalFrontPorch( aDescriptorBlockNumber );

    descBlock.iHorizontalSyncPulse
        = GetHorizontalSyncPulseWidth( aDescriptorBlockNumber );

    descBlock.iVerticalFrontPorch
        = GetVerticalFrontPorch( aDescriptorBlockNumber );

    descBlock.iVerticalSyncPulse
        = GetVerticalSyncPulseWidth( aDescriptorBlockNumber );

    descBlock.iHorizontalAddressableImageSize
        = GetHorizontalAddressableImageSize( aDescriptorBlockNumber );

    descBlock.iVerticalAddressableImageSize
        = GetVerticalAddressableImageSize( aDescriptorBlockNumber );

    descBlock.iHorizontalBorder
        = GetHorizontalBorderSize( aDescriptorBlockNumber );

    descBlock.iVerticalBorder = GetVerticalBorderSize( aDescriptorBlockNumber );

    descBlock.iInterlacedVideo = GetVideoIsInterlaced( aDescriptorBlockNumber );

    descBlock.iStereoSupport = GetStereoViewingSupport( aDescriptorBlockNumber );

    descBlock.iSyncs = GetSyncSignalDefinitions( aDescriptorBlockNumber );

    return descBlock;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetNumberOfExtensions
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetNumberOfExtensions()
    {
    FUNC_LOG;

    // If Block Maps are used then 254 is the maximum value of number of
    // extension blocks. If Block Maps are not used then 255 is the maximum
    // value of number of extension blocks
    return iBaseEdidData.iExtensionNumber;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetExtension
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CEdidParserBase::ReadExtension( TInt aNumber )
    {
    FUNC_LOG;

    // |-----Base block-----|----Ext1----|---Ext2---|---.....|
    //                            ^           ^
    //                           Extension numbers (1,2,..)
    TInt err = KErrNone;
    if( aNumber <= GetNumberOfExtensions() )
        {
        // Make a copy of extension data. Call ReadExtensionL() with different
        // extension number to read other extension
        TPtr8 rawPtr = iRawData->Des();
        Mem::Copy( &iExtensionData, rawPtr.Ptr() + ( aNumber
            * KEdidParserSizeOfEdidBlock ), KEdidParserSizeOfEdidBlock );
        }
    else
        {
        // Invalid extension number. Return with KErrNotSupported
        err = KErrNotSupported;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::GetExtensionType
// ---------------------------------------------------------------------------
//
EXPORT_C TEdidExtensionTagCode CEdidParserBase::GetExtensionType( TInt aNumber )
    {
    FUNC_LOG;

    TEdidExtensionTagCode tag( EUnknownExt );
    TInt err = ReadExtension( aNumber );
    if( err == KErrNone )
        {
        switch( iExtensionData[0] )
            {
            case 0x02:
                tag = ECea861Ext;
                break;
            case 0x10:
                tag = EVTBExt;
                break;
            case 0x20:
                tag = EEDID20;
                break;
            case 0x40:
                tag = EDIExt;
                break;
            case 0x50:
                tag = ELSExt;
                break;
            case 0x60:
                tag = EDPVLExt;
                break;
            default:
                // tag is EUnknownExt
                break;
            }
        }

    return tag;
    }

// ---------------------------------------------------------------------------
// CEdidParserBase::CreateCea861ExtensionParserL
// ---------------------------------------------------------------------------
//
EXPORT_C CCea861EdidParser* CEdidParserBase::CreateCea861ExtensionParserL( TInt aNumber )
    {
    FUNC_LOG;

    CCea861EdidParser* parser = NULL;
    TInt ext = GetExtensionType( aNumber );
    switch( ext )
        {
        case ECea861Ext:
            {
            // CEA 861 extension
            parser = CCea861EdidParser::NewL( iExtensionData );
            break;
            }
        default:
            {
            // Not supported
            break;
            }
        }

    // Extension is not supported. Return NULL
    return parser;
    }

EXPORT_C void CEdidParserBase::UpdateRawDataL( const TDesC8& aBinaryData )
	{
	FUNC_LOG;

	if ( iRawData )
		{
		iRawData = iRawData->ReAllocL( iRawdataLength + aBinaryData.Length() );
		
		TPtr8 rawPtr = iRawData->Des();
		rawPtr.Append( aBinaryData );		
		}
	else
		{
	    iRawData = HBufC8::NewL( aBinaryData.Length() );
	    TPtr8 rawPtr = iRawData->Des();
	    rawPtr.Copy( aBinaryData );
		}	

	iRawdataLength = iRawdataLength + aBinaryData.Length();
	}

EXPORT_C void CEdidParserBase::UpdateCea861ExtensionL( TInt aNumber, CCea861EdidParser* aParser )
	{
	FUNC_LOG;

    TInt ext = GetExtensionType( aNumber );
    switch( ext )
        {
        case ECea861Ext:
            {
            // CEA 861 extension
			if( aParser )
				{
				aParser->ParseExtensionBlockL( iExtensionData );
				}
            break;
            }
        default:
            {
            // Not supported
            break;
            }
        }
	}

// ---------------------------------------------------------------------------
// CEdidParserBase::GetChecksum
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CEdidParserBase::GetChecksum()
    {
    FUNC_LOG;

    // The Checksum Byte contains a value such that a checksum of the entire
    // 128 byte base EDID equals 00h
    return iBaseEdidData.iChecksum;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetPixelClock
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetPixelClock( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    // Stored Value = Pixel clock ÷ 10,000
    // LSB stored in byte 0 and MSB stored in byte 1
    // 0, 1 2 Range: 10 kHz to 655.35 MHz in 10 kHz steps
    // (00 00)h Reserved: Do not use for Detailed Timing Descriptor

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[0];
        word2 = iBaseEdidData.iTimingDescriptor[1];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[0];
        word2 = iBaseEdidData.iSecondTimingDescriptor[1];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[0];
        word2 = iBaseEdidData.iMonitorDescriptor[1];
        }
    else
        {
        //ERROR
        return 0;
        }

    // xxxx xxxx 1010 1010
    TUint16 pixelClock = word2;
    // 1010 1010 xxxx xxxx shifted to left (MSB)
    pixelClock = pixelClock << 8;
    // 1010 1010 0101 0101 add the LSB
    pixelClock += word1;

    return pixelClock;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetHorizontalAddressableVideo
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetHorizontalAddressableVideo( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[4];
        word2 = iBaseEdidData.iTimingDescriptor[2];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[4];
        word2 = iBaseEdidData.iSecondTimingDescriptor[2];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[4];
        word2 = iBaseEdidData.iMonitorDescriptor[2];
        }
    else
        {
        //ERROR
        return 0;
        }

    // 12bits long, defined in 8 bits of byte 2 and
    // upper nibble of byte 4 (4bits), range 0-4095
    TUint16 horAddrVideo = 0;

    // Nullify the 4 lsb
    // ---- ---- xxxx yyyy >> 4  = ---- ---- ---- xxxx
    horAddrVideo = word1 >> 4;
    // return back to original position (4)+4
    // ---- ---- ---- xxxx << 4  = ---- xxxx 0000 0000
    horAddrVideo = horAddrVideo << ( 4 + 4 );
    // ---- xxxx zzzz zzzz = correct values for the 12-bit-long var
    horAddrVideo += word2;

    return horAddrVideo;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetHorizontalBlanking
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetHorizontalBlanking( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[4];
        word2 = iBaseEdidData.iTimingDescriptor[3];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[4];
        word2 = iBaseEdidData.iSecondTimingDescriptor[3];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[4];
        word2 = iBaseEdidData.iMonitorDescriptor[3];
        }
    else
        {
        //ERROR
        return 0;
        }

    TUint16 horBlanking = 0;

    // nullify the 4 msb
    //      ---- ---- xxxx yyyy << 12 = yyyy ---- ---- ----
    horBlanking = word1 << 12;
    // nullify the 4 msb
    // yyyy ---- ---- ---- >> 4  = ---- yyyy ---- ----
    horBlanking = horBlanking >> 4;
    // ---- yyyy zzzz zzzz = correct values for the 12-bit-long var
    horBlanking += word2;

    return horBlanking;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetVerticalAddressableVideo
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetVerticalAddressableVideo( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[7];
        word2 = iBaseEdidData.iTimingDescriptor[5];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[7];
        word2 = iBaseEdidData.iSecondTimingDescriptor[5];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[7];
        word2 = iBaseEdidData.iMonitorDescriptor[5];
        }
    else
        {
        //ERROR
        return 0;
        }

    //Upper nibble of byte 7 and the 8 bits of byte 5)
    //Range is 0 lines to 4095 lines
    TUint16 vertAddrVideo = 0;
    vertAddrVideo = word1 >> 4;
    vertAddrVideo = vertAddrVideo << ( 4 + 4 );
    vertAddrVideo += word2;

    return vertAddrVideo;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetVerticalBlanking
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetVerticalBlanking( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[7];
        word2 = iBaseEdidData.iTimingDescriptor[6];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[7];
        word2 = iBaseEdidData.iSecondTimingDescriptor[6];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[7];
        word2 = iBaseEdidData.iMonitorDescriptor[6];
        }
    else
        {
        //ERROR
        return 0;
        }

    // 12bits long, defined in 8 bits of byte 2 and
    // upper nibble of byte 4 (4bits), range 0-4095
    TUint16 vertBlanking = 0;

    // nullify the 4 msb
    // ---- ---- xxxx yyyy << 12 = yyyy ---- ---- ----
    vertBlanking = word1 << 12;
    // nullify the 4 msb
    // yyyy ---- ---- ---- >> 4  = 0000 yyyy ---- ----
    vertBlanking = vertBlanking >> 4;
    // ---- yyyy zzzz zzzz = correct values for the 12-bit-long var
    vertBlanking += word2;

    return vertBlanking;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetHorizontalFrontPorch
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetHorizontalFrontPorch( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[6];
        word2 = iBaseEdidData.iTimingDescriptor[8];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[6];
        word2 = iBaseEdidData.iSecondTimingDescriptor[8];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[6];
        word2 = iBaseEdidData.iMonitorDescriptor[8];
        }
    else
        {
        //ERROR
        return 0;
        }

    // Horizontal Front Porch in Pixels (from blanking start to start of sync)
    // is represented by a 10 bit number (Bits 7 & 6 of byte 11 and the 8 bits of byte 8)
    // Range is 0 pixels to 1023 pixels.

    TUint16 horFrontPorch = 0;

    TUint8 bit67 = word1;
    // bits 6 and 7 remain
    bit67 = ( bit67 & KBit7 ) | ( bit67 & KBit6 );

    horFrontPorch = bit67 << 2;
    horFrontPorch += word2;

    return horFrontPorch;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetHorizontalSyncPulseWidth
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetHorizontalSyncPulseWidth( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    //Horizontal Sync Pulse Width in Pixels (from the end of the front porch to the start of the back
    //porch) is represented by a 10 bit number (Bits 5 & 4 of byte 11 and the 8 bits of byte 9) - Range
    //is 0 pixels to 1023 pixels.

    TUint8 word1;
    TUint8 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[11];
        word2 = iBaseEdidData.iTimingDescriptor[9];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[11];
        word2 = iBaseEdidData.iSecondTimingDescriptor[9];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[11];
        word2 = iBaseEdidData.iMonitorDescriptor[9];
        }
    else
        {
        //ERROR
        return 0;
        }

    TUint16 horSyncPulseWidth = word1;

    // only bits 5 and 4 remain: xx00 xxxx
    horSyncPulseWidth = ( horSyncPulseWidth & KBit5 ) | ( horSyncPulseWidth
        & KBit4 );

    // xx00 xxxx -> xx00 xxxx xxxx
    horSyncPulseWidth = horSyncPulseWidth << 4;

    // xx00 xxxx xxxx -> 00 0000 0000
    horSyncPulseWidth += word2;

    return horSyncPulseWidth;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetVerticalFrontPorch
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetVerticalFrontPorch( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    // Vertical Front Porch in Lines (from blanking start to start of sync) is represented by a 6 bit
    // number (Bits 3 & 2 of byte 11 and the upper nibble of byte 10) - Range is 0 lines to 63 lines.

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[11];
        word2 = iBaseEdidData.iTimingDescriptor[10];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[11];
        word2 = iBaseEdidData.iSecondTimingDescriptor[10];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[11];
        word2 = iBaseEdidData.iMonitorDescriptor[10];
        }
    else
        {
        //ERROR
        return 0;
        }

    TUint16 vertFrontPorch = 0;
    TUint8 bit23 = word1;

    // bits 2 and 3 remain  0000 xx00
    bit23 = ( bit23 & KBit3 ) | ( bit23 & KBit2 );

    // 00xx 0000
    vertFrontPorch = bit23 << 2;

    // only the upper nibble
    // 00xx yyyy
    vertFrontPorch += ( word2 >> 4 );

    return vertFrontPorch;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetVerticalSyncPulseWidth
// ----------------------------------------------------------------------------
//
TUint8 CEdidParserBase::GetVerticalSyncPulseWidth( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    // Vertical Sync Pulse Width in Lines (from the end of the front porch to the start of the back
    // porch) is represented by a 6 bit number (Bits 1 & 0 of byte 11 and the lower nibble of byte 10 -
    // Range is 0 lines to 63 lines.

    TUint8 word1;
    TUint8 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[10];
        word2 = iBaseEdidData.iTimingDescriptor[11];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[10];
        word2 = iBaseEdidData.iSecondTimingDescriptor[11];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[10];
        word2 = iBaseEdidData.iMonitorDescriptor[11];
        }
    else
        {
        //ERROR
        return 0;
        }

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

    return vertSyncPulse;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetHorizontalAddressableImageSize
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetHorizontalAddressableImageSize( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[14];
        word2 = iBaseEdidData.iTimingDescriptor[12];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[14];
        word2 = iBaseEdidData.iSecondTimingDescriptor[12];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[14];
        word2 = iBaseEdidData.iMonitorDescriptor[12];
        }
    else
        {
        //ERROR
        return 0;
        }

    //Horizontal Addressable Video Image Size in mm is represented by a 12 bit number (Upper
    //nibble of byte 14 and the 8 bits of byte 12) - Range is 0 mm to 4095 mm.

    TUint16 hais = word1 >> 4; // first nullify 4 lsb
    hais = hais << ( 4 + 4 ); // and then move back the first 4 and then again 4
    hais += word2;

    return hais;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetVerticalAddressableImageSize
// ----------------------------------------------------------------------------
//
TUint16 CEdidParserBase::GetVerticalAddressableImageSize( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    TUint16 word2;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[14];
        word2 = iBaseEdidData.iTimingDescriptor[13];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[14];
        word2 = iBaseEdidData.iSecondTimingDescriptor[13];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[14];
        word2 = iBaseEdidData.iMonitorDescriptor[13];
        }
    else
        {
        //ERROR
        return 0;
        }

    //Vertical Addressable Video Image Size in mm is represented by a 12 bit number (Lower nibble
    //of byte 14 and the 8 bits of byte 13) - Range is 0 mm to 4095 mm.

    TUint16 vais = word1;
    vais = vais << 12; // first nullify 4 msb and then move back 1 nibble
    vais = vais >> 4;
    vais += word2;

    return vais;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetHorizontalBorderSize
// ----------------------------------------------------------------------------
//
TUint8 CEdidParserBase::GetHorizontalBorderSize( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[15];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[15];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[15];
        }
    else
        {
        //ERROR
        return 0;
        }

    //Right Horizontal Border or Left Horizontal Border in Pixels is represented by
    // an 8 bit number (the 8 bits of byte 15) - Range is 0 pixels to 255 pixels.

    // the same value is used for (left and right) / (top and bottom)
    return word1;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetVerticalBorderSize
// ----------------------------------------------------------------------------
//
TUint8 CEdidParserBase::GetVerticalBorderSize( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[16];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[16];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[16];
        }
    else
        {
        //ERROR
        return 0;
        }

    //Top Vertical Border or Bottom Vertical Border in Lines is represented by an 8 bit number (the
    //8 bits of byte 16) - Range is 0 lines to 255 lines.
    return word1;
    }

// ----------------------------------------------------------------------------
// CEdidParserBase::GetVideoIsInterlaced
// ----------------------------------------------------------------------------
//
TBool CEdidParserBase::GetVideoIsInterlaced( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;
    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[17];
        }
    else
        {
        //ERROR
        return EFalse;
        }

    // byte 17
    // Bytes Bit Definitions Detailed Timing Definitions
    // 7 6 5 4 3 2 1 0 Signal Interface Type:
    // 0 _ _ _ _ _ _ _ Non-Interlaced (1 frame = 1 field)
    // 1 _ _ _ _ _ _ _ Interlaced (1 frame = 2 fields)

    return KBit7 & word1;
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetStereoViewingSupport
// ----------------------------------------------------------------------------
//
TEdidStereoViewingSupport CEdidParserBase::GetStereoViewingSupport( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    TUint16 word1;

    if( aDataBlockNumber == 1 )
        {
        word1 = iBaseEdidData.iTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 2 )
        {
        word1 = iBaseEdidData.iSecondTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 3 )
        {
        word1 = iBaseEdidData.iMonitorDescriptor[17];
        }
    else
        {
        //ERROR
        return EUnknownStereoViewingSupport;
        }

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
    if( !( ( KBit6 & bit17 ) | ( KBit5 & bit17 ) ) ) //  0 0 _ _ _ _ x Normal Display – No Stereo.
        { //                The value of bit 0 is "don't care"
        return ENormalDisplay;
        }
    else if( !( KBit6 & bit17 ) && ( KBit5 & bit17 ) && !( KBit0 & bit17 ) ) //  0 1 _ _ _ _ 0 Field sequential stereo,
        { //                right image when stereo sync signal=1
        return EFieldSequentialStereoRightWhenStereoSyncSignal1;
        }
    else if( ( KBit6 & bit17 ) && !( KBit5 & bit17 ) && !( KBit0 & bit17 ) ) //  1 0 _ _ _ _ 0 Field sequential stereo,
        { //                left image when stereo sync signal=1
        return EFieldSequentialStereoLeftWhenStereoSyncSignal1;
        }
    else if( !( KBit6 & bit17 ) && ( KBit5 & bit17 ) && ( KBit0 & bit17 ) ) //  0 1 _ _ _ _ 1 2-way interleaved stereo,
        { //                right image on even lines
        return ETwoWayInterleavedStereoRightImageOnEvenLines;
        }
    else if( ( KBit6 & bit17 ) && !( KBit5 & bit17 ) && ( KBit0 & bit17 ) ) //  1 0 _ _ _ _ 1 2-way interleaved stereo,
        { //                left image on even lines
        return ETwoWayInterleavedStereoLeftImageOnEvenLines;
        }
    else if( ( KBit6 & bit17 ) && ( KBit5 & bit17 ) && !( KBit0 & bit17 ) ) //  1 1 _ _ _ _ 0 4-way interleaved stereo
        {
        return EFourWayInterleaverStereo;
        }
    else if( ( KBit6 & bit17 ) && ( KBit5 & bit17 ) && ( KBit0 & bit17 ) ) //  1 1 _ _ _ _ 1 Side-by-Side interleaved stereo
        {
        return ESideBySideInterleavedStere;
        }
    else
        {
        // ERROR
        return EUnknownStereoViewingSupport;
        }

    // Unreachable
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetSyncSignalDefinitions
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CEdidParserBase::GetSyncSignalDefinitions( TUint8 aDataBlockNumber ) const
    {
    FUNC_LOG;

    // byte 17
    // 4 3 2 1 _ Analog Sync Signal Definitions:
    //------------------------------------------
    // 0 0 _ _ _ Analog Composite Sync:
    // 0 1 _ _ _ Bipolar Analog Composite Sync:

    TUint8 byte17 = 0;
    if( aDataBlockNumber == 1 )
        {
        byte17 = iBaseEdidData.iTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 2 )
        {
        byte17 = iBaseEdidData.iSecondTimingDescriptor[17];
        }
    else if( aDataBlockNumber == 3 )
        {
        byte17 = iBaseEdidData.iMonitorDescriptor[17];
        }
    else
        {
        // ERROR
        return EUnknownSignalDefinition;
        }

    if( !( KBit4 & byte17 ) ) // if bit4 == 0 then this is analog display
        {
        return GetAnalogSyncSignalDefinitions( byte17 );
        }
    else
        {
        return GetDigitalSyncSignalDefinitions( byte17 );
        }
    // unreachable
    }

// ----------------------------------------------------------------------------
// CBaseEdid::GetAnalogSyncSignalDefinitions
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CEdidParserBase::GetAnalogSyncSignalDefinitions( const TUint aByte17 )
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
// CBaseEdid::GetDigitalSyncSignalDefinitions
// ----------------------------------------------------------------------------
//
TEdidSyncSignalDefinitions CEdidParserBase::GetDigitalSyncSignalDefinitions( const TUint aByte17 )
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
    TBool digComp = ( KBit3 & aByte17 ); // ETrue == digital separate sync
    TBool bit2 = KBit2 & aByte17;

    if( !digComp ) // digital composite sync
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

// End of file
