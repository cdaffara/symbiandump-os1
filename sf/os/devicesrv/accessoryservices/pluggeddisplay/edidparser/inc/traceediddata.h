/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 *
 */

#ifndef TRACEEDIDDATA_H
#define TRACEEDIDDATA_H

// SYSTEM INCLUDE FILES
#include <e32std.h>

// USER INCLUDE FILES
#include "ediddatatypes.h"
#include "cea861ediddatatypes.h"
#include "videotimings.h"

// FORWARD DECLARATIONS
static void LogEdidBaseDataL( CEdidParserBase& aSelf );
static void LogEdidBaseData( CEdidParserBase& aSelf );
static void LogCea861ExtensionL( CEdidParserBase& aSelf );

// DATA TYPES

// Audio format code literals
_LIT( KAudioFormatNA,       "N/A" );
_LIT( KAudioFormatPCM,      "PCM" );
_LIT( KAudioFormatAC3,      "AC-3" );
_LIT( KAudioFormatMPEG1,    "MPEG-1" );
_LIT( KAudioFormatMP3,      "MP3" );
_LIT( KAudioFormatMPEG2,    "MPEG-2" );
_LIT( KAudioFormatAACLC,    "AAC LC" );
_LIT( KAudioFormatDTS,      "DTS" );
_LIT( KAudioFormatATRAC,    "ATRAC" );
_LIT( KAudioFormatDSD,      "DSD" );
_LIT( KAudioFormatEAC3,     "E-AC-3" );
_LIT( KAudioFormatDTSHD,    "DTS-HD" );
_LIT( KAudioFormatMLP,      "MLP" );
_LIT( KAudioFormatDST,      "DST" );
_LIT( KAudioFormatWMAPRO,   "WMA Pro" );

// Macro which disables tracing in release builds
#ifdef _DEBUG

    #define TRACE_EDID_DATA( aSelf )\
        {\
        LogEdidBaseData( aSelf );\
        }

    #define TRACE_TIMINGS( aTiming )\
        {\
        LogTiming( aTiming );\
        }

#else

    #define TRACE_EDID_DATA( aSelf )
    #define TRACE_TIMINGS( aTiming )

#endif



// LOCAL FUNCTIONS

static TPtrC AudioCodeAsDesC( TInt aCode )
    {
    TPtrC code( KNullDesC );
    switch( aCode )
        {
        case KAudioFormatCodeNA:
            {
            code.Set( KAudioFormatNA );
            break;
            }
        case KAudioFormatCodePCM:
            {
            code.Set( KAudioFormatPCM );
            break;
            }
        case KAudioFormatCodeAC3:
            {
            code.Set( KAudioFormatAC3 );
            break;
            }
        case KAudioFormatCodeMPEG1:
            {
            code.Set( KAudioFormatMPEG1 );
            break;
            }
        case KAudioFormatCodeMP3:
            {
            code.Set( KAudioFormatMP3 );
            break;
            }
        case KAudioFormatCodeMPEG2:
            {
            code.Set( KAudioFormatMPEG2 );
            break;
            }
        case KAudioFormatCodeAACLC:
            {
            code.Set( KAudioFormatAACLC );
            break;
            }
        case KAudioFormatCodeDTS:
            {
            code.Set( KAudioFormatDTS );
            break;
            }
        case KAudioFormatCodeATRAC:
            {
            code.Set( KAudioFormatATRAC );
            break;
            }
        case KAudioFormatCodeDSD:
            {
            code.Set( KAudioFormatDSD );
            break;
            }
        case KAudioFormatCodeEAC3:
            {
            code.Set( KAudioFormatEAC3 );
            break;
            }
        case KAudioFormatCodeDTSHD:
            {
            code.Set( KAudioFormatDTSHD );
            break;
            }
        case KAudioFormatCodeMLP:
            {
            code.Set( KAudioFormatMLP );
            break;
            }
        case KAudioFormatCodeDST:
            {
            code.Set( KAudioFormatDST );
            break;
            }
        case KAudioFormatCodeWMAPRO:
            {
            code.Set( KAudioFormatWMAPRO );
            break;
            }
        default:
            {
            code.Set( KAudioFormatNA );
            break;
            }
        }
    return code;
    }

// Trace edid base content - cannot leave
static void LogEdidBaseData( CEdidParserBase& aSelf )
    {
    TRAP_IGNORE( LogEdidBaseDataL( aSelf ) );
    }

// Trace edid base content
static void LogEdidBaseDataL( CEdidParserBase& aSelf )
    {
    // Edid base
    INFO( "--------------------------------------------------------------------" );
    INFO( "EDID BASE DATA:" );
    INFO( "--------------------------------------------------------------------" );
    
    // Manufacturer code
    TBuf<KEdidParserManufacturerCodeLength> manufacturerCode( KNullDesC );
    aSelf.GetManufacturerCode( manufacturerCode );
    INFO_1( "## Manufacturer code: %S", &manufacturerCode );
    
    // Product ID
    TBuf<KEdidParserManufacturerCodeLength + 1> productId( KNullDesC );
    aSelf.GetProductId( productId );
    INFO_1( "## Product ID: %S", &productId );
    
    // Manufacturer year and week
    INFO_2( "## Manufacturer Year: %d, Week: %d",
        aSelf.GetManufactureWeek(), aSelf.GetManufactureWeek() );

    // Version and revision
    INFO_2( "## Version: %d.%d", aSelf.GetVersion(), aSelf.GetRevision() );
    
    // Video input definition
    INFO_1( "## Video input definition: %d", aSelf.GetVideoInputDefinition() );
    
    // Screen size
    INFO_2( "## Screen size: %d x %d",
        aSelf.GetHorizontalScreenSize(), aSelf.GetVerticalScreenSize() );
    
    // Is aspect ratio used
    INFO_1( "## Aspect ratio used: %d", aSelf.IsAspectRatioUsed() );
    
    // Display transfer charasteristic
    INFO_1( "## Display transfer characteristics: %d",
        aSelf.DisplayTransferCharacteristics() );
    
    // Established timings 1
    INFO( "## Established timings 1:" );
    TUint8 timings = aSelf.GetEstablishedTimings1();
    if( timings & E800x600_60Hz )
        {
        INFO( "###### 800x600 60Hz" );
        }
    if( timings & E800x600_56Hz )
        {
        INFO( "###### 800x600 56Hz" );
        }    
    if( timings & E640x480_75Hz )
        {
        INFO( "###### 640x480 75Hz" );
        }
    if( timings & E640x480_72Hz )
        {
        INFO( "###### 640x480 72Hz" );
        }
    if( timings & E640x480_67Hz )
        {
        INFO( "###### 640x480 67Hz" );
        }
    if( timings & E640x480_60Hz )
        {
        INFO( "###### 640x480 60Hz" );
        }
    if( timings & E720x400_88Hz )
        {
        INFO( "###### 720x400 88Hz" );
        }
    if( timings & E720x400_70Hz )
        {
        INFO( "###### 720x400 70Hz" );
        }
    
    // Established timings 2
    INFO( "## Established timings 2:" );
    timings = aSelf.GetEstablishedTimings2();
    if( timings & E1280x1024_75Hz )
        {
        INFO( "###### 1280x1024 75Hz" );
        }
    if( timings & E1024x768_75Hz )
        {
        INFO( "###### 1024x768 75Hz" );
        }
    if( timings & E1024x768_70Hz )
        {
        INFO( "###### 1024x768 70Hz" );
        }
    if( timings & E1024x768_60Hz )
        {
        INFO( "###### 1024x768 60Hz" );
        }
    if( timings & E1024x768_87Hz )
        {
        INFO( "###### 1024x768 87Hz" );
        }
    if( timings & E832x624_75Hz )
        {
        INFO( "###### 832x624 75Hz" );
        }
    if( timings & E800x600_75Hz )
        {
        INFO( "###### 800x600 75Hz" );
        }
    if( timings & E800x600_72Hz )
        {
        INFO( "###### 800x600 72Hz" );
        }
    
    // Color coordinates
    INFO( "## Color coordinates: " );
    INFO_1( "###### Red X:   %d", aSelf.GetColorCoordinatesRedX() );
    INFO_1( "###### Red Y:   %d", aSelf.GetColorCoordinatesRedY() );
    INFO_1( "###### Green X: %d", aSelf.GetColorCoordinatesGreenX() );
    INFO_1( "###### Green Y: %d", aSelf.GetColorCoordinatesGreenY() );
    INFO_1( "###### Blue X:  %d", aSelf.GetColorCoordinatesBlueX() );
    INFO_1( "###### Blue Y:  %d", aSelf.GetColorCoordinatesBlueY() );
    INFO_1( "###### White X: %d", aSelf.GetColorCoordinatesWhiteX() );
    INFO_1( "###### White Y: %d", aSelf.GetColorCoordinatesWhiteY() );
    
    // Extensions
    TInt numOfExtensions = aSelf.GetNumberOfExtensions();
    INFO_1( "## Number of extensions found: %d", numOfExtensions );
    INFO( "--------------------------------------------------------------------" );
    TEdidExtensionTagCode extensionCode = EUnknownExt;
    for( TInt i = 0; i < numOfExtensions; i++ )
        {
        extensionCode = aSelf.GetExtensionType( i + 1 );
        switch( extensionCode )
            {
            case ECea861Ext:
                {
                LogCea861ExtensionL( aSelf );
                break;
                }
            case EVTBExt:
                {
                INFO( "VTB EXTENSION DATA: Not supported" );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            case EEDID20:
                {
                INFO( "EDID 2.0 EXTENSION DATA: Not supported" );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            case EDIExt:
                {
                INFO( "DI EXTENSION DATA: Not supported" );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            case ELSExt:
                {
                INFO( "LS EXTENSION DATA: Not supported" );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            case EDPVLExt:
                {
                INFO( "DPVL EXTENSION DATA: Not supported" );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            default:
                {
                INFO_1( "UNKNOWN EXTENSION DATA [0x%x]: Not supported", extensionCode );
                INFO( "--------------------------------------------------------------------" );
                break;
                }
            }
        }
    }

// Trace CEA 861 extension
static void LogCea861ExtensionL( CEdidParserBase& aSelf )
    {
    CCea861EdidParser* extParser = aSelf.CreateCea861ExtensionParserL( 1 );
    CleanupStack::PushL( extParser );
    if( extParser->IsExtensionBlockParsed() )
        {    
        // CEA 861 extension
        INFO( "CEA-861 EXTENSION DATA: Supported" );
        INFO( "--------------------------------------------------------------------" );
        
        // Underscan
        INFO_1( "## Underscan supported: %d", extParser->Underscan() );
        
        // Basic audio
        INFO_1( "## Basic audio supported: %d", extParser->BasicAudio() );
        
        // YCbCr444
        INFO_1( "## YCbCr444 supported: %d", extParser->YCbCr444() );
        
        // YCbCr422
        INFO_1( "## YCbCr422 supported: %d", extParser->YCbCr422() );
        
        // Number of native formats supported
        INFO_1( "## Number of native formats supported: %d", extParser->TotalNumberOfNativeFormats() );
        
        // Video data block
        TBool videoDataBlockSupported = extParser->IsVideoDataBlockSupported();
        INFO_1( "## Video data block supported: %d", videoDataBlockSupported );
        if( videoDataBlockSupported )
            {
            TInt count = 0;
            CCea861ExtEdidInformation* parsedInfo = extParser->GetParsedInformation();
            TCEA861VideoDataBlock* cur = parsedInfo->iShortVideoDescriptors;
            while( cur )
                {
                count++; 
                INFO_2( "###### VIC: %d, Native: %d", cur->iVIC, cur->iNative );
                cur = cur->iNext;
                }         
            INFO_1( "###### Number of DTV formats supported: %d", count );
            }
        
        // Audio data block
        TBool audioDataBlockSupported = extParser->IsAudioDataBlockSupported();
        INFO_1( "## Audio data block supported: %d", audioDataBlockSupported );
        if( audioDataBlockSupported )
            {
            CCea861ExtEdidInformation* parsedInfo = extParser->GetParsedInformation();
            TCEA861AudioDataBlock* cur = parsedInfo->iShortAudioDescriptors;
            while( cur )
                {
                TPtrC audioFormatCode( AudioCodeAsDesC( cur->iAudioFormatCode ) );
                INFO_2( "###### Audio format code:     0x%x [%S]", cur->iAudioFormatCode, &audioFormatCode );                
                INFO_1( "###### Supports 192kHz:       %d", cur->iSupport192kHz );
                INFO_1( "###### Supports 176kHz:       %d", cur->iSupport176kHz );
                INFO_1( "###### Supports 96kHz:        %d", cur->iSupport96kHz );
                INFO_1( "###### Supports 88kHz:        %d", cur->iSupport88kHz );
                INFO_1( "###### Supports 48kHz:        %d", cur->iSupport48kHz );
                INFO_1( "###### Supports 44kHz:        %d", cur->iSupport44kHz );
                INFO_1( "###### Supports 32kHz:        %d", cur->iSupport32kHz );
                INFO_1( "###### Maximum channels:      %d", cur->iMaxChannels );
                
                TBool bitDataSupported = cur->BitDataSupported();
                INFO_1( "###### Bit data supported: %d", bitDataSupported );
                if( bitDataSupported )
                    {
                    INFO_1( "###### Supports 24-bit rate:  %d", cur->iSupport24Bit );
                    INFO_1( "###### Supports 20-bit rate:  %d", cur->iSupport20Bit );
                    INFO_1( "###### Supports 16-bit rate:  %d", cur->iSupport16Bit );
                    }
                TBool maxBitRateValueSupported = cur->MaximumBitRateValueSupported();
                INFO_1( "###### Maximum bit data supported: %d", maxBitRateValueSupported );
                if( maxBitRateValueSupported )
                    {
                    INFO_1( "###### Maximum bit rate:      %d", cur->iMaxBitrate );
                    }
                TBool audioFormatCodeExtSupported = cur->AudioFormatCodeExtensionSupported();
                INFO_1( "###### Audio format code extension supported: %d", audioFormatCodeExtSupported );
                if( audioFormatCodeExtSupported )
                    {
                    INFO_1( "###### Audio format code ext: 0x%x", cur->iAudioFormatCodeExtension );
                    }
                cur = cur->iNext;
                }                
            }
        
        // Vendor specific data block
        TBool vendorDataBlockSupported = extParser->IsVendorSpecificDataBlockSupported();
        INFO_1( "## Vendor specific data block supported: %d", vendorDataBlockSupported );
        if( vendorDataBlockSupported )
            {
            TUint8 latency = extParser->GetVideoLatency();
            INFO_1( "###### Video latency (p): %d ms", latency );
            latency = extParser->GetAudioLatency();
            INFO_1( "###### Audio latency (p): %d ms", latency );
            latency = extParser->GetInterlacedVideoLatency();
            INFO_1( "###### Video latency (i): %d ms", latency );
            latency = extParser->GetInterlacedAudioLatency();
            INFO_1( "###### Audio latency (i): %d ms", latency );
            }
        
        // Speaker allocation data block
        TBool speakerAllocationDataBlockSupported = extParser->IsSpeakerAllocationDataBlockSupported();
        INFO_1( "## Speaker allocation data block supported: %d", speakerAllocationDataBlockSupported );
        if( speakerAllocationDataBlockSupported )
            {
            CCea861ExtEdidInformation* parsedInfo = extParser->GetParsedInformation();
            INFO_1( "###### FLW_FRW: %d", parsedInfo->iSpeakerAllocationData.FLW_FRW() );
            INFO_1( "###### RLC_RRC: %d", parsedInfo->iSpeakerAllocationData.RLC_RRC() );
            INFO_1( "###### FLC_FRC: %d", parsedInfo->iSpeakerAllocationData.FLC_FRC() );
            INFO_1( "###### RC:      %d", parsedInfo->iSpeakerAllocationData.RC() );
            INFO_1( "###### RL_RR:   %d", parsedInfo->iSpeakerAllocationData.RL_RR() );
            INFO_1( "###### FC:      %d", parsedInfo->iSpeakerAllocationData.FC() );
            INFO_1( "###### LFE:     %d", parsedInfo->iSpeakerAllocationData.LFE() );
            INFO_1( "###### FL_FR:   %d", parsedInfo->iSpeakerAllocationData.FL_FR() );
            INFO_1( "###### TC:      %d", parsedInfo->iSpeakerAllocationData.TC() );
            INFO_1( "###### FLH_FRH: %d", parsedInfo->iSpeakerAllocationData.FLH_FRH() );
            }
        INFO( "--------------------------------------------------------------------" );
        }        
    CleanupStack::PopAndDestroy( extParser );
    }

// Log timing item
static void LogTiming( const THdmiDviTimings& aTiming )
    {
    INFO( "--------------------------------------------------------------------" );
    if( aTiming.iCeaMode )
        {
        INFO_1( "Mode: CEA, Id: %d", aTiming.iCeaMode );
        }
    else if( aTiming.iDmtMode )
        {
        INFO_1( "Mode: DMT, Id: %d", aTiming.iDmtMode );
        }
    else
        {
        INFO( "Mode: Unknown, Id: Not set" );
        }
    INFO_1( "Interlaced: %d", aTiming.iInterlaced );
    INFO_1( "Width: %d", aTiming.iHorizontalActivePixels );
    INFO_1( "Height: %d", aTiming.iVerticalActiveLines );
    INFO_1( "DOT clock: %d [kHz]", aTiming.iPixelClockKHz );
    INFO_1( "Pixel repeat: %d", aTiming.iPixelRepeat );
    if( aTiming.iImageAspectRatio == TTvSettings::E4d3 )
        {
        INFO( "Aspect ratio: 4:3" );
        }
    else if( aTiming.iImageAspectRatio == TTvSettings::E16d9 )
        {
        INFO( "Aspect ratio: 16:9" );
        }
    else
        {
        INFO( "Aspect ratio: Undefined" );
        }
    INFO_1( "Pixel Aspect Ratio Numerator: %d", aTiming.iPixelAspectRatioNumerator );
    INFO_1( "Pixel Aspect Ratio Denominator: %d", aTiming.iPixelAspectRatioDenominator );
    INFO_1( "Horizontal front porch: %d", aTiming.iHorizontalSyncOffsetPixels );
    INFO_1( "Horizontal sync: %d", aTiming.iHorizontalSyncOffsetPixels );
    INFO_1( "Horizontal blanking: %d", aTiming.iHorizontalBlankingPixels );
    INFO_1( "Horizontal active lines: %d", aTiming.iHorizontalActivePixels );
    INFO_1( "Vertical 1st front porch: %d", aTiming.iVerticalSyncOffsetLinesField1 );
    INFO_1( "Vertical 1st sync: %d", aTiming.iVerticalSyncPulseWidthLinesField1 );
    INFO_1( "Vertical 1st blanking: %d", aTiming.iVerticalBlankingLines );
    INFO_1( "Vertical 1st active lines: %d", aTiming.iVerticalActiveLines );
    INFO_1( "Vertical 2nd front porch: %d", aTiming.iVerticalSyncOffsetLinesField2 );
    INFO_1( "Vertical 2nd sync: %d", aTiming.iVerticalSyncPulseWidthLinesField2 );
    INFO_1( "Vertical 2nd blanking: %d", aTiming.iVerticalBlankingLines );
    INFO_1( "Vertical 2nd active lines: %d", aTiming.iVerticalActiveLines );
    if( !aTiming.iHorizontalSyncPolarity )
        {
        INFO( "Horizontal sync polarity: Negative" );
        }
    else
        {
        INFO( "Horizontal sync polarity: Positive" );
        }
    if( !aTiming.iVerticalSyncPolarity  )
        {
        INFO( "Vertical sync polarity: Negative" );
        }
    else
        {
        INFO( "Vertical sync polarity: Positive" );
        }
    }

#endif // TRACEEDIDDATA_H
