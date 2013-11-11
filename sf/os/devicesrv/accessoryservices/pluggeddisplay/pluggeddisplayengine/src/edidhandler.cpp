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
 * Description:  EDID data Handler class for HDMI Cable Status FSM.
 *
 */

#include <tvoutconfig.h>
#include <accpolhdmisink.h>
#include <accpolhdmiaudioformat.h>
#include <accpolhdmivideoformat.h>
#include <accpolhdmilatency.h>
#include <accpolhdmispeakerallocation.h>
#include <e32cmn.h>
#include <e32math.h>
#include <accessoriescrkeys.h>
#include <centralrepository.h>
#include <sysutil.h>

#include "pdeconstants.h"
#include "tvoutconfigforhdmi.h"
#include "edidhandler.h"
#include "edidparserbase.h"
#include "cea861edidparser.h"
#include "multifinitestatemachine.h"
#include "trace.h"
#include "traceediddata.h"

const TReal K16d9 = 1.778;
const TReal K4d3 = 1.333;

const TInt KDefaultCEAMode = E640x480p59_94d60Hz4d3;
const TInt KDefaultCEAModeIndex = 0;
const TInt KDefaultDMTModeIndex = 3;

// Retry Delay for EDID access
const TInt KRetryDelay = 50 * 1000; // 50 milliseconds

// Maximum retry count for EDID access
const TInt KMaxRetryCount = 40;  // 40 * 50ms = 2 Seconds

const TUint16 KDefaultCEAModePhysImgAspRatioNr = 4;

const TUint16 KDefaultCEAModePhysImgAspRatioDr = 3;

//------------------------------------------------------------------------------
// Symbian two-phased constructor
//------------------------------------------------------------------------------
//
CEDIDHandler* CEDIDHandler::NewL( MFSMForBody& aFSM,
    CTVOutConfigForHDMI& aTVOutConfigForHDMI )
    {
    FUNC_LOG;
    CEDIDHandler* self = new ( ELeave ) CEDIDHandler(
            aFSM, aTVOutConfigForHDMI );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
//
CEDIDHandler::~CEDIDHandler()
    {
    FUNC_LOG;
    
    Cancel();
    delete iDdcPortAccess;
    delete iDataBlockPtr;
    delete iExtensionParserPtr;
    delete iEdidParserPtr;
    iRetryTimer.Close();
    }

//------------------------------------------------------------------------------
// FetchEDIDData
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::FetchEDIDData()
    {
    FUNC_LOG;
    
    TInt retVal( KErrNone );
    TUint apiVersion = iDdcPortAccess->ApiVersion();
    if( KDdcAccessVersion != apiVersion )
        {
        INFO_1( "iDdcPortAccess->ApiVersion() returned unsupported version != KDdcAccessVersion: %d", apiVersion );
        retVal = KErrNotSupported;
        }
    else
        {
        iRetryCounter = KErrNone;
        TInt err = KErrNone;
        TRAP( retVal , err = ReadEDIDDataL() );
        if ( err != KErrNone )
            {
            retVal = err;
            }
        }
    INFO_1( "CEDIDHandler::FetchEDIDData() retVal: %d", retVal );
    return retVal;
    }

//------------------------------------------------------------------------------
// SetVideoParameters
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetVideoParameters()
    {
    FUNC_LOG;

    TInt retVal = KErrNone;
    RArray<TTvSettings> analogConfigs;
    RArray<THdmiDviTimings> hdmiConfigs;
    
    // Set video parameters
    INFO( "--------------------------------------------------------------------" );
    INFO( "SETTING CEA AND DMT TIMINGS:" );
    retVal = SetCeaModes( hdmiConfigs );
    ERROR( retVal, "Failed to set CEA modes" );
    
    if( KErrNone == retVal )
        {
        retVal = SetDmtModes( hdmiConfigs );
        ERROR( retVal, "Failed to set DMT modes" );
        INFO( "--------------------------------------------------------------------" );
        
        if( KErrNone == retVal )
            {
            // Filtering out the unsupported modes
            // Logical AND(Sink supported modes AND HW supported modes)
            INFO( "Filtering out the unsupported modes" );
            retVal = FilterAvailableTvConfigList( hdmiConfigs );
            ERROR( retVal, "Failed to filter the TV config list." );
    
            if( KErrNone == retVal )
                {
                retVal = iTVOutConfigForHDMI.SetAvailableTvConfigList( analogConfigs, hdmiConfigs );
                ERROR( retVal, "Failed to set available TV config list." );
                }            
            }
        }
    
    // Clean up
    hdmiConfigs.Close();
    analogConfigs.Close();

    return retVal;
    }

//------------------------------------------------------------------------------
// GetEDIDDataL
//------------------------------------------------------------------------------
//
void CEDIDHandler::ResetData()
    {
    FUNC_LOG;
    
    delete iDataBlockPtr;
    iDataBlockPtr = NULL;
    delete iEdidParserPtr;
    iEdidParserPtr = NULL;
    delete iExtensionParserPtr;
    iExtensionParserPtr = NULL;

	iCurrentBlock = 0;
	inbrOfExtensions = 0;
    }

//------------------------------------------------------------------------------
// CreateHdmiSinkL
//------------------------------------------------------------------------------
//
CAccPolHdmiSink* CEDIDHandler::CreateHdmiSinkL()
    {
    FUNC_LOG;
    
    CAccPolHdmiSink* hdmiSink( NULL );
    if( iExtensionParserPtr && iTVOutConfigForHDMI.GetTvOutConfig() )
        {
        hdmiSink = CAccPolHdmiSink::NewL(
            iExtensionParserPtr->BasicAudio(),
            ETrue );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    return hdmiSink;
    }

//------------------------------------------------------------------------------
// CreateHdmiVideoFormatL
//------------------------------------------------------------------------------
//
void CEDIDHandler::CreateHdmiVideoFormatL( RAccPolHdmiVideoFormatArray& aHdmiVideoFormatArray )
    {
    FUNC_LOG;
    
    // Active Video Format object values can be fetched from TvOutConfig interface.
    if( iEdidParserPtr && iTVOutConfigForHDMI.GetTvOutConfig() )
        {
        THdmiDviTimings hdmiDviTimings;
        TInt retVal = iTVOutConfigForHDMI.GetTvOutConfig()->GetConfig(
            hdmiDviTimings );
        if( KErrNone == retVal )
            {
            CAccPolHdmiVideoFormat* hdmiVideoFormat = CAccPolHdmiVideoFormat::NewLC();            
            hdmiVideoFormat->SetCeaFixedMode( hdmiDviTimings.iCeaMode );
            hdmiVideoFormat->SetDmtFixedMode( EDmtFixedModeNone );
            hdmiVideoFormat->SetPixelRepeat( hdmiDviTimings.iPixelRepeat );
            hdmiVideoFormat->SetInterlaced( hdmiDviTimings.iInterlaced );
            aHdmiVideoFormatArray.AppendL( hdmiVideoFormat );
            CleanupStack::Pop( hdmiVideoFormat );
            }
        else
            {
            User::Leave( retVal );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

//------------------------------------------------------------------------------
// CreateHdmiLatencyL
//------------------------------------------------------------------------------
//
void CEDIDHandler::CreateHdmiLatencyL( RAccPolHdmiLatencyArray& aHdmiLatencyArray )
    {
    FUNC_LOG;
    
    if( iEdidParserPtr && iExtensionParserPtr )
        {
        // Interlaced audio and video latency
        CAccPolHdmiLatency* hdmiLatency = CAccPolHdmiLatency::NewLC(
            HdmiLatency::KUidInterlacedLatency,
            iExtensionParserPtr->GetInterlacedAudioLatency(),
            iExtensionParserPtr->GetInterlacedVideoLatency() ); 
        aHdmiLatencyArray.AppendL( hdmiLatency );
        CleanupStack::Pop( hdmiLatency );

        // Progressive audio and video latency
        hdmiLatency = CAccPolHdmiLatency::NewLC(
            HdmiLatency::KUidLatency,
            iExtensionParserPtr->GetAudioLatency(),
            iExtensionParserPtr->GetVideoLatency() );
        aHdmiLatencyArray.AppendL( hdmiLatency );
        CleanupStack::Pop( hdmiLatency );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

//------------------------------------------------------------------------------
// CreateHdmiAudioFormatL
//------------------------------------------------------------------------------
//
void CEDIDHandler::CreateHdmiAudioFormatL( RAccPolHdmiAudioFormatArray& aHdmiAudioFormatArray )
    {
    FUNC_LOG;
    
    if( iExtensionParserPtr )
        {
        if( iExtensionParserPtr->IsAudioDataBlockSupported() )
            {
            TCEA861AudioDataBlock
                * audioDataBlock =
                    iExtensionParserPtr->GetParsedInformation()->iShortAudioDescriptors;
            while( audioDataBlock )
                {
                    CAccPolHdmiAudioFormat* hdmiAudioFormat =
                        CAccPolHdmiAudioFormat::NewL();
                    CleanupStack::PushL( hdmiAudioFormat );

                    // Set audio format
                    TUid audioFormat;
                    // Map the audio format code defined in 
                    // cea861ediddatatypes.h to accpolhdmiaudioformat.h
                    switch ( audioDataBlock->iAudioFormatCode )
                        {
                        case KAudioFormatCodePCM:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatPCM16;
                            break;
                            }
                        case KAudioFormatCodeAC3:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatAC3;
                            break;
                            }
                        case KAudioFormatCodeMPEG1:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatMPEG1;
                            break;
                            }
                        case KAudioFormatCodeMP3:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatMP3;
                            break;
                            }
                        case KAudioFormatCodeMPEG2:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatMPEG2;
                            break;
                            }
                        case KAudioFormatCodeAACLC:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatAACLC;
                            break;
                            }
                        case KAudioFormatCodeDTS:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatDTS;
                            break;
                            }
                        case KAudioFormatCodeATRAC:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatATRAC;
                            break;
                            }
                        case KAudioFormatCodeDSD:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatDSD;
                            break;
                            }
                        case KAudioFormatCodeEAC3:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatEAC3;
                            break;
                            }
                        case KAudioFormatCodeDTSHD:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatDTSHD;
                            break;
                            }
                        case KAudioFormatCodeMLP:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatMLP;
                            break;
                            }
                        case KAudioFormatCodeDST:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatDST;
                            break;
                            }
                        case KAudioFormatCodeWMAPRO:
                            {
                            audioFormat = HdmiAudioFormat::KUidFormatWMAPRO;
                            break;
                            }
                        default:
                            {
                            audioFormat.iUid = KAudioFormatCodeNA;
                            break;
                            }
                        }
                    hdmiAudioFormat->SetAudioFormat( audioFormat ); // const TUid aAudioFormat,

                    // Set bit resolution
                    TUint32 bitResolution( CAccPolHdmiAudioFormat::EUnknownBitsPerSample );
                    if( audioDataBlock->iSupport24Bit )
                        {
                        bitResolution |= CAccPolHdmiAudioFormat::EBitsPerSample24;
                        }
                    if( audioDataBlock->iSupport20Bit )
                        {
                        bitResolution |= CAccPolHdmiAudioFormat::EBitsPerSample20;
                        }
                    if( audioDataBlock->iSupport16Bit )
                        {
                        bitResolution |= CAccPolHdmiAudioFormat::EBitsPerSample16;
                        }
                    hdmiAudioFormat->SetBitResolution( bitResolution ); // const TUint32 aBitResolution,
                    hdmiAudioFormat->SetMaxBitResolution( audioDataBlock->iMaxBitrate ); // const TUint32 aMaxBitResolution,
                    hdmiAudioFormat->SetChannelCount( audioDataBlock->iMaxChannels ); // const TUint32 aMaxChannelCount,

                    // Set sample frequency 
                    TUint32 samFreq( CAccPolHdmiAudioFormat::EUnknownFrequency );
                    if( audioDataBlock->iSupport192kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq192KHz;
                        }
                    if( audioDataBlock->iSupport176kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq176KHz;
                        }
                    if( audioDataBlock->iSupport96kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq96KHz;
                        }
                    if( audioDataBlock->iSupport88kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq88KHz;
                        }
                    if( audioDataBlock->iSupport48kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq48KHz;
                        }
                    if( audioDataBlock->iSupport44kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq44KHz;
                        }
                    if( audioDataBlock->iSupport32kHz )
                        {
                        samFreq
                            |= CAccPolHdmiAudioFormat::ESamplingFreq32KHz;
                        }
                    hdmiAudioFormat->SetSamFreq( samFreq ); // const TUint32 aSamFreq,
                    hdmiAudioFormat->SetFormatDependentValue( audioDataBlock->iAudioFormatCodeExtension ); // const TUint32 aFormatDependentValue
                    aHdmiAudioFormatArray.AppendL( hdmiAudioFormat );
                    CleanupStack::Pop( hdmiAudioFormat );
                    
                    audioDataBlock = audioDataBlock->iNext;
                }
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

//------------------------------------------------------------------------------
// CreateHdmiAudioFormatL
//------------------------------------------------------------------------------
//
CAccPolHdmiSpeakerAllocation* CEDIDHandler::CreateHdmiSpeakerAllocationL()
    {
    FUNC_LOG;
    
    CAccPolHdmiSpeakerAllocation* hdmiSpeakerAllocation( NULL );
    if( iExtensionParserPtr )
        {
        if( iExtensionParserPtr->IsSpeakerAllocationDataBlockSupported() )
            {
            TCEA861SpeakerAllocationData
                * speakerAllocationData =
                    &( iExtensionParserPtr->GetParsedInformation()->iSpeakerAllocationData );
            if( speakerAllocationData )
                {
                hdmiSpeakerAllocation
                    = CAccPolHdmiSpeakerAllocation::NewL( speakerAllocationData->FL_FR(), //const TBool aFrontSpeakers,
                        speakerAllocationData->RL_RR(), //const TBool aRearSpeakers,
                        speakerAllocationData->LFE(), //const TBool aLowFrequencyEffect,
                        speakerAllocationData->FC(), //const TBool aFrontCenter,
                        speakerAllocationData->FCH(), //const TBool aFrontCenterHigh,
                        speakerAllocationData->TC(), //const TBool aTopCenter,
                        speakerAllocationData->RC(), //const TBool aRearCenter,
                        speakerAllocationData->FLC_FRC(), //const TBool aFrontLeftRightCenter,
                        speakerAllocationData->RLC_RRC(), //const TBool aRearLeftRightCenter,
                        speakerAllocationData->FLW_FRW(), //const TBool aFrontWideSpeakers,
                        speakerAllocationData->FLH_FRH() //const TBool aFrontHighSpeakers         
                    );
                }
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    return hdmiSpeakerAllocation;
    }

//------------------------------------------------------------------------------
// GetHdcpSupportStatus
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::GetHdcpSupportStatus( TBool& aHdcpSupportStatus ) const
    {
    FUNC_LOG;
    
    TInt retVal( KErrNone );
    if( iTVOutConfigForHDMI.GetTvOutConfig() )
        {
        aHdcpSupportStatus
            = ( iTVOutConfigForHDMI.GetTvOutConfig() )->CopyProtectionStatus();
        }
    else
        {
        retVal = KErrNotFound;
        }
    return retVal;
    }

//------------------------------------------------------------------------------
// RunL
//------------------------------------------------------------------------------
//
void CEDIDHandler::RunL()
    {

    FUNC_LOG;
    
    switch ( iRequestID )
        {
        case EDdcReadRequest:
            {
			if( KErrNone == iStatus.Int() )
				{				
				if( iCurrentBlock == 0 )
					{
					TPtrC8 dataBlockDes( iDataBlockPtr->iDataBlock, sizeof( *iDataBlockPtr ) );
					
					iEdidParserPtr = CEdidParserBase::NewL( dataBlockDes );
					inbrOfExtensions = iEdidParserPtr->GetNumberOfExtensions();

					INFO_1( "No. of extensions from Block 0: %d", inbrOfExtensions );

					if( inbrOfExtensions )
						{
						inbrOfExtensions--;
						}
 					}
				else
					{
					TPtrC8 dataBlockDes( iDataBlockPtr->iDataBlock, sizeof( *iDataBlockPtr ) );

					INFO_1( "Updating the Rawdata for the Block %d...", iCurrentBlock );
					iEdidParserPtr->UpdateRawDataL(dataBlockDes);
					
					if( inbrOfExtensions >= 2 )
						{
 						inbrOfExtensions = inbrOfExtensions - 2;
						}
					else
						{
						inbrOfExtensions--;
						}
  					}

				if( inbrOfExtensions )
					{
					iCurrentBlock++;
					iRetryCounter = KErrNone;
					
					if( ReadEDIDDataL() != KErrNone )
						{
						ResetData();
						iFSM.Input( EPDEIfEDIDHandler, EPDEIfEDIDHandlerEventEdidDataFetchFailed );
						}
					}
				else
					{
					TInt extensions = iEdidParserPtr->GetNumberOfExtensions();

					INFO_1( "No. of extensions from Block 0: %d", extensions );
					
					for( TInt i = 0; i < extensions; ++i )
						{
						if( ECea861Ext == iEdidParserPtr->GetExtensionType( i + 1 ) )
							{
							INFO_1( "ECea861Ext extension data block number: %d", ( i+1 ) );
							if( !iExtensionParserPtr )
								{
								INFO( "First CEA 861 extension is being read..." );
								iExtensionParserPtr
									= iEdidParserPtr->CreateCea861ExtensionParserL( i + 1 );
								}
							else
								{
								INFO_1( "CEA 861 extension is being read... at the index %d", i+1 );
								iEdidParserPtr->UpdateCea861ExtensionL( i + 1, iExtensionParserPtr );
								}
 							}
						}
					TRACE_EDID_DATA( *iEdidParserPtr );
					
					iFSM.Input( EPDEIfEDIDHandler, EPDEIfEDIDHandlerEventEdidDataFetched );
					iRetryCounter = KErrNone;
					}
				}
            else
                {
                INFO_1( "CDdcPortAccess::Read failed, error code: %d", iStatus.Int() );
                
                if( (iStatus.Int() == KErrNotReady) && (iRetryCounter < KMaxRetryCount) )
                    {
                    iRetryCounter++;
                    iRequestID = ERetryTimerRequest;
                    iRetryTimer.After( iStatus, KRetryDelay );
                    SetActive();
                    }
                else
                    {
                    // No EDID data available from the sink
                    iRetryCounter = KErrNone;
					ResetData();
					iFSM.Input( EPDEIfEDIDHandler, EPDEIfEDIDHandlerEventEdidDataFetched );
                    }
                }
            break;
            }
        case ERetryTimerRequest:
            {
            INFO_1( "Retrying... count: %d", iRetryCounter );
            if( ReadEDIDDataL() != KErrNone )
                {
                iFSM.Input( EPDEIfEDIDHandler, EPDEIfEDIDHandlerEventEdidDataFetchFailed );
                }
            break;
            }
        default:
            {
            INFO_1( "Undefined Request ID %d", iRequestID );
            break;
            }
        }
    }

//------------------------------------------------------------------------------
// From class CActive.
// RunL
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::RunError( TInt /*aError*/ )
    {
    FUNC_LOG;
    
    /*TInt err( aError );
    // Avoid Panic in CActiveScheduler
    if ( err )
        {
        INFO_1( "aError %d", err );
        }*/
    iFSM.Input( EPDEIfEDIDHandler, EPDEIfEDIDHandlerEventEdidDataFetchFailed );
        
    return KErrNone;
    }

//-------------------------------------------------------------------------------
// DoCancel
//
//-------------------------------------------------------------------------------
//
void CEDIDHandler::DoCancel()
    {
    FUNC_LOG;
    
    iDdcPortAccess->CancelAll();
    }

//------------------------------------------------------------------------------
// ReadEDIDDataL
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::ReadEDIDDataL()
    {
    FUNC_LOG;
    
    TInt retVal( KErrNone );
    
    iRequestID = EDdcReadRequest;
    
    if( iDataBlockPtr == NULL )
        {
        iDataBlockPtr = new(ELeave) TDataBlock;
        }
	else if( inbrOfExtensions )
		{
		if( iDataBlockPtr )
			{
			delete iDataBlockPtr;
			iDataBlockPtr = NULL;
			}
		iDataBlockPtr = new(ELeave) TDataBlock;
		}

	INFO_1( "Reading EDID block %d...", iCurrentBlock );
    
    retVal = iDdcPortAccess->Read( EMonitorPort, iCurrentBlock, // First block contains EDID data if that exists
        iDataBlockPtr->iDataBlock,
        iStatus );
        
    SetActive();
        
    ERROR(retVal, "iDdcPortAccess->Read failed" );
    
    return retVal;
    }

//------------------------------------------------------------------------------
// CEDIDHandler::FillCommonHdmiDviTimings
//------------------------------------------------------------------------------
//
void CEDIDHandler::FillCommonHdmiDviTimings( THdmiDviTimings& aTimings ) const
    {
    FUNC_LOG;
    
    aTimings.iTvPhysicalImageAspectRatioNumerator = 0;
    aTimings.iTvPhysicalImageAspectRatioDenominator = 0;
    aTimings.iHorizontalBorderPixels = 0;
    aTimings.iVerticalBorderLinesField1 = 0;
    aTimings.iVerticalBorderLinesField2 = 0;
    aTimings.iLeftBorderPixels = 0;
    aTimings.iRightBorderPixels = 0;
    
    if( iExtensionParserPtr )
        {
        INFO("==CEA Extension Exists");
        aTimings.iUnderscanEnabled = iExtensionParserPtr->Underscan();
        }
	else
		{
        INFO("==No CEA Extension");
		// No CEA Extension so it should be DVI
		// Underscan supported always		
		aTimings.iTvPhysicalImageAspectRatioNumerator = 4;
		aTimings.iTvPhysicalImageAspectRatioDenominator = 3;
		aTimings.iUnderscanEnabled = ETrue;
		}
    
    if( aTimings.iUnderscanEnabled )
        {
        INFO("==Underscan Enabled");
        // Underscan
        aTimings.iLeftTopCorner.iX = 0;
        aTimings.iLeftTopCorner.iY = 0;
        aTimings.iRightBottomCorner.iX = aTimings.iHorizontalActivePixels;
        aTimings.iRightBottomCorner.iY = aTimings.iVerticalActiveLines;
        }
    else
        {
        INFO("==Underscan Disabled");
        // Calculate overscan
        CalculateOverscan( aTimings.iLeftTopCorner,
            aTimings.iRightBottomCorner );                
        }
    aTimings.iConnector = TTvSettings::EHDMI;

	if( iEdidParserPtr )
		{
		aTimings.iTvPhysicalImageWidthMm = iEdidParserPtr->GetHorizontalScreenSize() * 10;
		aTimings.iTvPhysicalImageHeightMm = iEdidParserPtr->GetVerticalScreenSize() * 10;
	    aTimings.iTvPhysicalImageAspectRatioNumerator = iEdidParserPtr->GetAspectRatioLandscape();
	    aTimings.iTvPhysicalImageAspectRatioDenominator = iEdidParserPtr->GetAspectRatioPortrait();
	    aTimings.iTvColorCoordinates.iRed.iX = iEdidParserPtr->GetColorCoordinatesRedX();
	    aTimings.iTvColorCoordinates.iRed.iY = iEdidParserPtr->GetColorCoordinatesRedY();
	    aTimings.iTvColorCoordinates.iGreen.iX = iEdidParserPtr->GetColorCoordinatesGreenX();
	    aTimings.iTvColorCoordinates.iGreen.iY = iEdidParserPtr->GetColorCoordinatesGreenY();
	    aTimings.iTvColorCoordinates.iBlue.iX = iEdidParserPtr->GetColorCoordinatesBlueX();
	    aTimings.iTvColorCoordinates.iBlue.iY = iEdidParserPtr->GetColorCoordinatesBlueY();
	    aTimings.iTvColorCoordinates.iWhite.iX = iEdidParserPtr->GetColorCoordinatesWhiteX();
	    aTimings.iTvColorCoordinates.iWhite.iY = iEdidParserPtr->GetColorCoordinatesWhiteY();
	    aTimings.iTvHdmiVersion = iEdidParserPtr->GetVersion();
	    aTimings.iTvHdmiRevision = iEdidParserPtr->GetRevision();
		}
    Mem::FillZ( ( TAny* )&aTimings.iProductName, ( sizeof( TChar ) * KProductNameChars ) );
    Mem::FillZ( ( TAny* )&aTimings.iProductDescription, ( sizeof( TChar ) * KProductDescriptorsChars ) );
    	
    TInt err = KErrNone;
    CDeviceTypeInformation* deviceTypeInfo = NULL;
    TRAP(err, deviceTypeInfo = SysUtil::GetDeviceTypeInfoL());
    
    if(KErrNone == err)
        {
        #define MAX(a, b) (((a)>(b)) ? (a) : (b))
        #define MIN(a, b) (((a)<(b)) ? (a) : (b))
        
        TPtrC16 bufPtrModelCode;
        TPtrC16 bufPtrManufacturerName;
        TUint loopCount;
        TUint maxLen;
        TUint maxCharsLen;
        TUint minNameCharsLen;
        TUint minDescritopCharsLen;
        
        deviceTypeInfo->GetManufacturerName(bufPtrManufacturerName);
        deviceTypeInfo->GetModelCode(bufPtrModelCode);
        maxLen = MAX(bufPtrModelCode.Length(),bufPtrManufacturerName.Length());
        maxCharsLen = MAX(KProductNameChars,KProductDescriptorsChars);
        minNameCharsLen = MIN(KProductNameChars, bufPtrModelCode.Length());
        minDescritopCharsLen = MIN(KProductDescriptorsChars, bufPtrManufacturerName.Length());
        
        INFO_1( "maxLen : %d", maxLen);
        INFO_1( "maxCharsLen : %d", maxCharsLen);
        INFO_1( "minNameCharsLen : %d", minNameCharsLen);
        INFO_1( "minDescritopCharsLen : %d", minDescritopCharsLen);
        
        for(loopCount = 0; loopCount < maxLen; loopCount++)
            {
            if(loopCount >= maxCharsLen)
                {
                        break;
                }
            else
                {
                    if(loopCount <  minNameCharsLen)
                        {
                        aTimings.iProductName[loopCount] = bufPtrModelCode[loopCount];
                        INFO_1( "aTimings.iProductName : %d", aTimings.iProductName[loopCount].GetLowerCase());
                        }
                    if(loopCount <  minDescritopCharsLen)
                        {
                        aTimings.iProductDescription[loopCount] = bufPtrManufacturerName[loopCount];
                        INFO_1( "aTimings.iProductDescription : %d", aTimings.iProductDescription[loopCount].GetLowerCase());
                        }
                }
            }
        INFO("Before Deleting Pointer deviceTypeInfo");
        delete deviceTypeInfo;
        INFO("After Deleting Pointer deviceTypeInfo");
        aTimings.iSourceType = THdmiDviTimings::ESourceTypePCGeneral;
        }
    else
        {
        aTimings.iSourceType = THdmiDviTimings::ESourceTypeUnknown;
        }
    }

//------------------------------------------------------------------------------
// CEDIDHandler::FillHdmiDviTimings
//------------------------------------------------------------------------------
//
void CEDIDHandler::FillHdmiDviTimings( const TTimingItem& aItem,
    THdmiDviTimings& aTimings ) const
    {
    FUNC_LOG;
    
    // Fill attributes from the static table
    if( aItem.iTimingType == ETimingModeCEA )
        {
        // CEA
        aTimings.iCeaMode = static_cast<TFixedModeCea>( aItem.iTimingId );
        }
    else
        {
        // DMT
        aTimings.iDmtMode = static_cast<TFixedModeDmt>( aItem.iTimingId );
        }
    aTimings.iPixelClockKHz = aItem.iDotClock;
    aTimings.iHorizontalActivePixels = aItem.iHorizontalActive;
    aTimings.iHorizontalBlankingPixels = aItem.iHorizontalBlanking;
    aTimings.iHorizontalSyncOffsetPixels = aItem.iHorizontalFrontPorch;
    aTimings.iHorizontalSyncPulseWidthPixels = aItem.iHorizontalSync;
    aTimings.iVerticalActiveLines = aItem.iVertical1stActive;
    aTimings.iVerticalBlankingLines = aItem.iVertical1stBlanking;
    aTimings.iVerticalSyncOffsetLinesField1 = aItem.iVertical1stFrontPorch;
    aTimings.iVerticalSyncPulseWidthLinesField1 = aItem.iVertical1stSync;
    aTimings.iVerticalSyncOffsetLinesField2 = aItem.iVertical2ndFrontPorch;
    aTimings.iVerticalSyncPulseWidthLinesField2 = aItem.iVertical2ndSync;
    aTimings.iInterlaced = aItem.iInterlaced;
    aTimings.iHorizontalSyncPolarity = aItem.iHorizontalSyncPolarity;
    aTimings.iVerticalSyncPolarity = aItem.iVerticalSyncPolarity;
    aTimings.iPixelRepeat = aItem.iPixelRepeat;
    aTimings.iRightBottomCorner.iX = aItem.iWidth;
    aTimings.iRightBottomCorner.iY = aItem.iHeight;
    aTimings.iImageAspectRatio = aItem.iAspectRatio;
    if( aTimings.iImageAspectRatio == TTvSettings::EUndefRatio )
        {
        // Resolve ratio from width and height
        aTimings.iImageAspectRatio = ResolveAspectRatio( aItem.iWidth, aItem.iHeight );
        }
    aTimings.iPixelAspectRatioNumerator = aItem.iPixelAspectRatioNumerator;
    aTimings.iPixelAspectRatioDenominator = aItem.iPixelAspectRatioDenominator;

    // Fill the common attributes 
    FillCommonHdmiDviTimings( aTimings );
    
    TRACE_TIMINGS( aTimings );
    }

//------------------------------------------------------------------------------
// CEDIDHandler::FillHdmiDviTimings
//------------------------------------------------------------------------------
//
void CEDIDHandler::FillHdmiDviTimings( const TEdidDescriptorBlock& aDescBlock,
    THdmiDviTimings& aTimings ) const
    {
    FUNC_LOG;

    // Fill attributes from timing descriptor
    aTimings.iCeaMode = ECeaFixedModeNone;
    aTimings.iDmtMode = EDmtFixedModeNone;
    aTimings.iPixelClockKHz = aDescBlock.iPixelClock;
    aTimings.iHorizontalActivePixels = aDescBlock.iHorizontalAddressableVideoPixels;
    aTimings.iHorizontalBlankingPixels = aDescBlock.iHorizontalBlanking;
    aTimings.iHorizontalSyncOffsetPixels = aDescBlock.iHorizontalFrontPorch;
    aTimings.iHorizontalSyncPulseWidthPixels = aDescBlock.iHorizontalSyncPulse;
    aTimings.iHorizontalBorderPixels = aDescBlock.iHorizontalBorder;
    aTimings.iVerticalActiveLines = aDescBlock.iVerticalAddressableVideoPixels;
    aTimings.iVerticalBlankingLines = aDescBlock.iVerticalBlanking;
    aTimings.iVerticalSyncOffsetLinesField1 = aDescBlock.iVerticalFrontPorch;
    aTimings.iVerticalSyncPulseWidthLinesField1 = aDescBlock.iVerticalSyncPulse;
    aTimings.iVerticalBorderLinesField1 = aDescBlock.iVerticalBorder;
    aTimings.iInterlaced = aDescBlock.iInterlacedVideo;
    if( aDescBlock.iSyncs ==
        EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsNegative )
        {
        aTimings.iVerticalSyncPolarity = EFalse;
        aTimings.iHorizontalSyncPolarity = EFalse;
        }
    else if( aDescBlock.iSyncs ==
        EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsPositive )
        {
        aTimings.iVerticalSyncPolarity = EFalse;
        aTimings.iHorizontalSyncPolarity = ETrue;
        }
    else if( aDescBlock.iSyncs ==
        EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsNegative )
        {
        aTimings.iVerticalSyncPolarity = ETrue;
        aTimings.iHorizontalSyncPolarity = EFalse;
        }
    else if( aDescBlock.iSyncs ==
        EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsPositive )
        {
        aTimings.iVerticalSyncPolarity = ETrue;
        aTimings.iHorizontalSyncPolarity = ETrue;
        }
    aTimings.iPixelRepeat = 1;
    aTimings.iRightBottomCorner.iX = aDescBlock.iHorizontalAddressableVideoPixels;
    aTimings.iRightBottomCorner.iY = aDescBlock.iVerticalAddressableVideoPixels;
    aTimings.iImageAspectRatio = ResolveAspectRatio(
        aTimings.iHorizontalActivePixels,
        aTimings.iVerticalActiveLines );

    // Fill the common attributes 
    FillCommonHdmiDviTimings( aTimings );

    TRACE_TIMINGS( aTimings );
    }

//------------------------------------------------------------------------------
// SetCeaModes
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetCeaModes( RArray<THdmiDviTimings>& aTimings ) const
    {
    FUNC_LOG;
    
    TInt retVal(KErrNone);

    // Use only CEA-861 extension parser
    if( iExtensionParserPtr )
        {
        // Get parsed information
        CCea861ExtEdidInformation* info =
            iExtensionParserPtr->GetParsedInformation();
        if( info )
            {
            // Go through all supported CEA modes
            TCEA861VideoDataBlock* vdb = info->iShortVideoDescriptors;
            THdmiDviTimings timings;
            while( vdb )
                {
                // Get a timing item matched with the VIC mode
                TInt index = vdb->iVIC - 1;
                if( ( index >= 0 ) && ( index < KCEATimingCount ) )
                    {
                    const TTimingItem* item = TimingByIndex( index, ETimingModeCEA );
                    if( item )
                        {
                        Mem::FillZ( ( TAny* )&timings, sizeof( timings ) );
                        FillHdmiDviTimings( *item, timings );
                        retVal = aTimings.Append( timings );
                        ERROR_1( retVal, "Failed to append CEA timing: %S in array", item->iTimingName );
                        }
                    else
                        {
                        ERROR_1( KErrArgument, "CEA timing item not found for VIC mode: %d", index );
                        }
                    }
                vdb = vdb->iNext;
                }
            }
        }
    
    return retVal;
    }

//------------------------------------------------------------------------------
// SetDmtModes
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetDmtModes( RArray<THdmiDviTimings>& aTimings ) const
    {
    FUNC_LOG;
    
    TInt retVal(KErrNone);

	if( iDataBlockPtr )
		{
	    // Check established timings 1 and 2
	    retVal = SetDmtModesFromEstablishedTimings( aTimings );
	    
	    if( KErrNone == retVal )
	        {
	        // Check standard timings
	        retVal = SetDmtModesFromStandardTimings( aTimings );
	        
	        if( KErrNone == retVal )
	            {
	            // Check timing descriptors
	            retVal = SetDmtModesFromTimingDescriptors( aTimings );        
	            }
	        }
		}
	else
		{
		INFO( "==No EDID available from the Sink. Setting DMT 4" );
		// No EDID data available from the sink
		// Default VGA resolution should be selected
		THdmiDviTimings timings;
		const TTimingItem* item = TimingByIndex( KDefaultDMTModeIndex, ETimingModeDMT );
		if( item )
			{
			Mem::FillZ( ( TAny* )&timings, sizeof( timings ) );
			FillHdmiDviTimings( *item, timings );
			retVal = aTimings.Append( timings );
			ERROR_1( retVal, "Failed to append DMT timing: %S in array", item->iTimingName );
			}
		else
			{
			ERROR_1( KErrArgument, "DMT timing item not found for VIC mode: %d", KDefaultDMTModeIndex );
			retVal = KErrNotFound;
			}
		}
    
    return retVal;
    }

//------------------------------------------------------------------------------
// SetDmtModesFromEstablishedTimings
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetDmtModesFromEstablishedTimings(
    RArray<THdmiDviTimings>& aTimings ) const
    {
    FUNC_LOG;
    
    TUint8 timings = 0;
    TUint16 width = 0;
    TUint16 height = 0;
    TUint16 refreshRate = 0;
    TInt retVal(KErrNone);
    
    // Established timings 1
    // Bits 4, 6 and 7 left out since these does not match to DMT
    timings = iEdidParserPtr->GetEstablishedTimings1();
    if( timings & E800x600_60Hz ) // Bit 0
        {
        width = 800;
        height = 600;
        refreshRate = 60;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E800x600_56Hz ) // Bit 1
        {
        width = 800;
        height = 600;
        refreshRate = 56;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E640x480_75Hz ) // Bit 2
        {
        width = 640;
        height = 480;
        refreshRate = 75;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E640x480_72Hz ) // Bit 3
        {
        width = 640;
        height = 480;
        refreshRate = 72;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E640x480_60Hz ) // Bit 5
        {
        width = 640;
        height = 480;
        refreshRate = 60;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }

    // Established timings 2
    // Bit 5 left out since it does not match to DMT
    timings = iEdidParserPtr->GetEstablishedTimings2();
    if( timings & E1280x1024_75Hz ) // Bit 0
        {
        width = 1280;
        height = 1024;
        refreshRate = 75;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E1024x768_75Hz ) // Bit 1
        {
        width = 1024;
        height = 768;
        refreshRate = 75;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E1024x768_70Hz ) // Bit 2
        {
        width = 1280;
        height = 768;
        refreshRate = 70;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E1024x768_60Hz ) // Bit 3
        {
        width = 1280;
        height = 768;
        refreshRate = 60;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E1024x768_87Hz ) // Bit 4
        {
        width = 1280;
        height = 768;
        refreshRate = 87;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E800x600_75Hz ) // Bit 6
        {
        width = 800;
        height = 600;
        refreshRate = 75;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    if( timings & E800x600_72Hz ) // Bit 7
        {
        width = 800;
        height = 600;
        refreshRate = 72;
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    
    return retVal;
    }

//------------------------------------------------------------------------------
// SetDmtModesFromStandardTimings
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetDmtModesFromStandardTimings(
    RArray<THdmiDviTimings>& aTimings ) const
    {
    FUNC_LOG;
    
    // One timing descriptor is specified in two bytes:
    // 26h:
    // -------------------------------------------------------------------------
    //      01h-FFh:
    //      Horizontal addressable pixels: Value Stored (in hex) =
    //          (Horizontal addressable pixels / 8) – 31
    //      Range: 256 pixels -> 2288 pixels, in increments of 8 pixels
    //      --------------------------------------------------------------------
    //      00h:
    //      Reserved. Do not use!
    // -------------------------------------------------------------------------
    // 27h: 
    // -------------------------------------------------------------------------
    //      Bits 7 - 6:
    //      Image Aspect Ratio:
    //      0 0 _ _ _ _ _ _ (16 : 10 AR)
    //      0 1 _ _ _ _ _ _ ( 4 : 3  AR)
    //      1 0 _ _ _ _ _ _ ( 5 : 4  AR)
    //      1 1 _ _ _ _ _ _ (16 : 9  AR)
    //      --------------------------------------------------------------------
    //      Bits 0 - 5:
    //      Field Refresh Rate: Value Stored (in binary) =
    //          Field Refresh Rate (in Hz) – 60
    //      Range: 60 Hz -> 123Hz
    // -------------------------------------------------------------------------
    // 27h: 
    // 28h: 
    // -------------------------------------------------------------------------
    // etc.

    // There can be up to 8 standard timings (2 bytes * 8)
    const TInt KStandardTimingBytes = 16;
    const TUint KRefreshRateMask = 0x3F; // --XXXXXX
    TInt retVal(KErrNone);
    
    TUint16 width = 0;
    TUint16 height = 0;
    TUint16 refreshRate = 0;
    TUint8 byte1 = 0;
    TUint8 byte2 = 0;
    for( TInt i = 0; i < KStandardTimingBytes; i += 2 )
        {
        byte1 = iEdidParserPtr->GetStandardTimings( i );
        byte2 = iEdidParserPtr->GetStandardTimings( i + 1 );
        
        // Horizontal pixels
        width = ( byte1 + 31 ) * 8;
        
        // Aspect ratio & vertical lines
        if( !( byte2 & KBit7 ) && !( byte2 & KBit6 ) )
            {
            // 16 : 10 AR
            height = ( width * 10 ) / 16;
            }
        else if( !( byte2& KBit7 ) && ( byte2 & KBit6 ) )
            {
            // 4 : 3 AR
            height = ( width * 3 ) / 4;
            }
        else if( ( byte2 & KBit7 ) && !( byte2& KBit6 ) )
            {
            // 5 : 4 AR
            height = ( width * 4 ) / 5;
            }
        else // ( byte2 & KBit7 ) && ( byte2 & KBit6 )
            {
            // 16 : 9 AR
            height = ( width * 9 ) / 16;
            }
        
        // Refresh rate
        // Nullify bits 6 and 7
        byte2 = byte2 & KRefreshRateMask;
        refreshRate = byte2 + 60;
        
        // Set timing item by resolution
        retVal = SetDmtModeByResolution( aTimings, width, height, refreshRate );
        }
    
    return retVal;
    }

//------------------------------------------------------------------------------
// SetDmtModesFromTimingDescriptors
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetDmtModesFromTimingDescriptors(
    RArray<THdmiDviTimings>& aTimings ) const
    {
    FUNC_LOG;
    
    // 1st 18 byte timing descriptor
    THdmiDviTimings timings;
    TInt retVal(KErrNone);
    
    TEdidDescriptorBlock first =
        iEdidParserPtr->GetDescriptorBlock( EEdidDescriptorBlockFirstTiming );
    FillHdmiDviTimings( first, timings );
    retVal = aTimings.Append( timings );
    ERROR( retVal, "Failed to append 1st timing descriptor in array" );

    if( KErrNone == retVal )
        {
        // 2nd 18 byte timing descriptor
        Mem::FillZ( ( TAny* )&timings, sizeof( timings ) );
        TEdidDescriptorBlock second =
            iEdidParserPtr->GetDescriptorBlock( EEdidDescriptorBlockSecondTiming );
        FillHdmiDviTimings( second, timings );
        retVal = aTimings.Append( timings );
        ERROR( retVal, "Failed to append 2nd timing descriptor in array" );
        }
    
    return retVal;
    }

//------------------------------------------------------------------------------
// TimingByIndex
//------------------------------------------------------------------------------
//
const TTimingItem* CEDIDHandler::TimingByIndex( TInt aIndex,
    TTimingModeType aType ) const
    {
    FUNC_LOG;
    
    const TTimingItem* item = NULL;
    if( aIndex >= 0 )
        {
        if( aType == ETimingModeCEA )
            {
            if( aIndex < KCEATimingCount )
                {
                item = &KCEATimings[aIndex];
                }
            }
        else
            {
            if( aIndex < KDMTTimingCount )
                {
                item = &KDMTTimings[aIndex];
                }
            }
        }

    return item;
    }

//------------------------------------------------------------------------------
// TimingByResolution
//------------------------------------------------------------------------------
//
const TTimingItem* CEDIDHandler::TimingByResolution( TUint16 aWidth,
    TUint16 aHeight,
    TUint16 aRefreshRate,
    TTimingModeType aType ) const
    {
    FUNC_LOG;

    const TTimingItem* item = NULL;
    if( aType == ETimingModeCEA )
        {
        // CEA mode
        for( TInt i = 0; i < KCEATimingCount; i++ )
            {
            item = &KCEATimings[i];
            if( item->iWidth == aWidth &&
                item->iHeight == aHeight &&
                item->iFieldRate == aRefreshRate )
                {
                // Item found, break
                break;
                }
            item = NULL;
            }
        }
    else
        {
        // DMT mode
        for( TInt i = 0; i < KDMTTimingCount; i++ )
            {
            item = &KDMTTimings[i];
            if( item->iWidth == aWidth &&
                item->iHeight == aHeight &&
                item->iFieldRate == aRefreshRate )
                {
                // Item found, break
                break;
                }
            item = NULL;
            }
        }

    return item;
    }

//------------------------------------------------------------------------------
// SetDmtModeByResolution
//------------------------------------------------------------------------------
//
TInt CEDIDHandler::SetDmtModeByResolution( RArray<THdmiDviTimings>& aTimings,
    TUint16 aWidth,
    TUint16 aHeight,
    TUint16 aRefreshRate ) const
    {
    FUNC_LOG;
    
    TInt retVal(KErrNone);

    const TTimingItem* item = TimingByResolution( aWidth,
        aHeight, aRefreshRate, ETimingModeDMT );
    if( item )
        {
        THdmiDviTimings timings;
        FillHdmiDviTimings( *item, timings );
        retVal = aTimings.Append( timings );
        ERROR_1( retVal, "Failed to append DMT timing: %S in array",
            item->iTimingName );
        }
    else
        {
        ERROR_3( KErrArgument, "DMT timing item not found for width: %d, height: %d, refresh rate: %d",
            aWidth, aHeight, aRefreshRate );
        }
    
    return retVal;
    }

//------------------------------------------------------------------------------
// SetDmtModeByResolution
//------------------------------------------------------------------------------
//
TTvSettings::TAspectRatio CEDIDHandler::ResolveAspectRatio( TUint16 aWidth,
    TUint16 aHeight ) const
    {
    FUNC_LOG;
    
    TTvSettings::TAspectRatio aspectRatio = TTvSettings::EUndefRatio;
    TReal source = ( TReal )aWidth / ( TReal )aHeight;
    TReal target = 0.0;
    if( Math::Round( target, source, 3 ) == KErrNone )
        {
        if( target == K16d9 )
            {
            // 16:9
            aspectRatio = TTvSettings::E16d9;
            }
        else if( target == K4d3 )
            {
            // 4:3
            aspectRatio = TTvSettings::E4d3;
            }
        }
    
    return aspectRatio;
    }

//------------------------------------------------------------------------------
// CalculateOverscan
//------------------------------------------------------------------------------
//
void CEDIDHandler::CalculateOverscan( TPoint& aTLCorner,
    TPoint& aBRCorner ) const
    {
    FUNC_LOG;

    // No need to calculate if the screen size is zero
    if( aBRCorner.iX > 0 && aBRCorner.iY > 0 )
        {
        // hOverscanPixels = ( ( Width * hOverscan ) + 50 ) / 10000
        //
        //  hOverscanPixels:
        //      pixels which are needed to be added to top left X
        //      pixels which are needed to be reduced from bottom right X
        //  Width:
        //      Horizontal resolution
        //  hOverscan:
        //      Horizontal overscan in percents (1% == 100)
        //  50:
        //      Used to round up possible decimals
        //  10000:
        //      Used to get rid of percentage multiplier and to get the overscan value
        //      for one side
        TInt hOverscanPixels = ( aBRCorner.iX * iHOverscan + 50 ) / 10000;
        aTLCorner.iX = hOverscanPixels;
        aBRCorner.iX = ( aBRCorner.iX - hOverscanPixels );
        
        // vOverscanPixels = ( ( Height * vOverscan ) + 50 ) / 10000
        //
        //  vOverscanPixels:
        //      pixels which are needed to be added to top left Y
        //      pixels which are needed to be reduced from bottom right Y
        //  Height:
        //      Horizontal resolution
        //  vOverscan:
        //      Vertical overscan in percents (1% == 100)
        //  50:
        //      Used to round up possible decimals
        //  10000:
        //      Used to get rid of percentage multiplier and to get the overscan value
        //      for one side
        TInt vOverscanPixels = ( aBRCorner.iY * iVOverscan + 50 ) / 10000;
        aTLCorner.iY = vOverscanPixels;
        aBRCorner.iY = ( aBRCorner.iY - vOverscanPixels );
        }
    }

//------------------------------------------------------------------------------
// CalculateOverscan
//------------------------------------------------------------------------------
//
TBool CEDIDHandler::UpdateOverscanValues()
    {
    FUNC_LOG;

    // Overscan from cenrep
    TInt hOverscan = 0;
    TInt vOverscan = 0;
    CRepository* cenRep = NULL;
    TInt err = KErrNone;
	TBool valChanged = EFalse;
    
    TRAP( err, cenRep = CRepository::NewL( KCRUidTvoutSettings ) );
    if( err == KErrNone )
        {
        
        // Horizontal
        err = cenRep->Get( KSettingsTvoutHorizontalOverscan, hOverscan );
        if( err != KErrNone )
            {
            hOverscan = 0;
            }
        
        // Vertical
        err = cenRep->Get( KSettingsTvoutVerticalOverscan, vOverscan );
        if( err != KErrNone )
            {
            vOverscan = 0;
            }
        
        // Cleanup
        delete cenRep;
        }

	if( (iHOverscan != hOverscan) || (iVOverscan != vOverscan) )
		{
		valChanged = ETrue;
		}
    
    // Update overscan values
    iHOverscan = hOverscan;
    iVOverscan = vOverscan;

	INFO_3( "Overscan Values: %d,%d Changed:%d", iHOverscan, iVOverscan, valChanged );

	return valChanged;
    }

// ----------------------------------------------------------------------------
// CEDIDHandler::FilterAvailableTvConfigList
//
// ----------------------------------------------------------------------------
//
TInt CEDIDHandler::FilterAvailableTvConfigList( RArray<THdmiDviTimings>& aHdmiConfigs )
    {
    FUNC_LOG;
    
	TUint supportedCount;
	TInt  retVal( KErrNotFound );
	TInt  availableCount( aHdmiConfigs.Count() );
	RArray<TSupportedHdmiDviMode> supportedModes;

	INFO_1( "HDMI CONFIGS --- From SINK -- Total : %d", availableCount );

	retVal = iTVOutConfigForHDMI.GetSupportedHdmiModes( supportedModes );

	if( KErrNone == retVal )
		{
		TInt availableIndex = 0;
		TBool found( EFalse );
		TBool defaultCEAmode( EFalse ), ceaMode( EFalse );
		supportedCount = supportedModes.Count();
		INFO_1( "HDMI CONFIGS --- From HW -- Total : %d", supportedCount );

		INFO( "Filtered list -- START" );
		while( availableIndex < availableCount )
			{
			found = EFalse;
			
			for( TInt supportedIndex = 0; (supportedIndex < supportedCount); supportedIndex++ )
				{
				// Check CEA mode
				if( aHdmiConfigs[ availableIndex ].iCeaMode && 
					(TSupportedHdmiDviMode::ECea == supportedModes[ supportedIndex ].iStandardModeType) &&
					(aHdmiConfigs[ availableIndex ].iCeaMode == supportedModes[ supportedIndex ].iStandardMode) )
					{
					ceaMode = ETrue;
					found = ETrue;
					if( aHdmiConfigs[ availableIndex].iCeaMode == KDefaultCEAMode )
					    {
					    defaultCEAmode = ETrue;
					    }
					TRACE_TIMINGS( (aHdmiConfigs[ availableIndex ]) );
					break;
					}

				// Check DMT mode
				if( aHdmiConfigs[ availableIndex ].iDmtMode &&
					(TSupportedHdmiDviMode::EDmt == supportedModes[ supportedIndex ].iStandardModeType) &&
					(aHdmiConfigs[ availableIndex ].iDmtMode == supportedModes[ supportedIndex ].iStandardMode) )
					{
					found = ETrue;
					TRACE_TIMINGS( (aHdmiConfigs[ availableIndex ]) );
					break;
					}
				}

			if( EFalse == found )
				{
				// Remove from the list
				aHdmiConfigs.Remove( availableIndex );
				availableCount--;
				continue;
				}

			availableIndex++;
			}

			// If the Vendor Specific Data Block supported and it has IEEE registration number then it is HDMI
			if ( iExtensionParserPtr && iExtensionParserPtr->IsVendorSpecificDataBlockSupported() && iExtensionParserPtr->HasIEEERegistration() )
			{
			    INFO( "<<<<<<<<<<<<<<It is HDMI connector>>>>>>>>>>>>>>" );
				// Add default CEA mode 1 to the list if it is not there already
			    if( ( (KDefaultCEAModePhysImgAspRatioNr == iEdidParserPtr->GetAspectRatioLandscape()) 
			            && (KDefaultCEAModePhysImgAspRatioDr == iEdidParserPtr->GetAspectRatioPortrait()) ) 
			            && !defaultCEAmode )		    
	
				{
	            THdmiDviTimings timings;
	            
	            // Get a timing item for default CEA Mode (1)
	            const TTimingItem* item = TimingByIndex( KDefaultCEAModeIndex, ETimingModeCEA );
	            if( item )
	                {
	                Mem::FillZ( ( TAny* )&timings, sizeof( timings ) );
	                FillHdmiDviTimings( *item, timings );
	                retVal = aHdmiConfigs.Append( timings );
	                ERROR( retVal, "Failed to append CEA timing in available config array" );
	                }
				}
		    }
		    else // It is DVI connector
		    {
				TInt modecount = aHdmiConfigs.Count();

				if( !modecount )
					{
					THdmiDviTimings timings;
					
					INFO( "==No EDID available from the Sink. Setting DMT 4" );
					// No EDID data available from the sink
					// Default VGA resolution should be selected
					const TTimingItem* item = TimingByIndex( KDefaultDMTModeIndex, ETimingModeDMT );
					if( item )
						{
						Mem::FillZ( ( TAny* )&timings, sizeof( timings ) );
						FillHdmiDviTimings( *item, timings );
						timings.iTvPhysicalImageAspectRatioNumerator = 4;
						timings.iTvPhysicalImageAspectRatioDenominator = 3;
						retVal = aHdmiConfigs.Append( timings );
						ERROR_1( retVal, "Failed to append DMT timing: %S in array", item->iTimingName );
						if( retVal == KErrNone )
							{
							modecount = 1;
							ceaMode = EFalse;
							}
						}
					}
				
				while( modecount-- )
				  {

				    if( aHdmiConfigs[ modecount ].iDmtMode && (EFalse == ceaMode) )
				    	{
						INFO( "<<<<<<<<<<<<<<It is DVI connector>>>>>>>>>>>>>>" );
						
					    // Change it to DVI mode as it is existing in both Supported and available configurations
					    aHdmiConfigs[ modecount ].iConnector = TTvSettings::EDVI;
					
					    // Version should be zeroed for non-HDMI
					    aHdmiConfigs[ modecount ].iTvHdmiVersion = 0;
					    aHdmiConfigs[ modecount ].iTvHdmiRevision = 0;
				    	}
				  }
		    }

		INFO( "Filtered list -- END" );
		supportedModes.Close();
		}

	return retVal;
    }

void CEDIDHandler::GetCurrentOverscanValue( TInt& aHOverscan, TInt& aVOverscan )
	{
	FUNC_LOG;
	
	aHOverscan = iHOverscan;
	aVOverscan = iVOverscan;

	INFO_2("Overscan used: %d, %d", iHOverscan, iVOverscan);
	}

//------------------------------------------------------------------------------
// C++ constructor
//------------------------------------------------------------------------------
//
CEDIDHandler::CEDIDHandler( MFSMForBody& aFSM,
    CTVOutConfigForHDMI& aTVOutConfigForHDMI ) :
    CActive( CActive::EPriorityLow ),
    iFSM( aFSM ),
    iTVOutConfigForHDMI( aTVOutConfigForHDMI ),
    iRetryCounter( 0 ),
    iRequestID( EUndefRequest ),
    inbrOfExtensions( 0 ),
    iCurrentBlock( 0 )
    {
    FUNC_LOG;
    }

//------------------------------------------------------------------------------
// ConstructL
//------------------------------------------------------------------------------
//
void CEDIDHandler::ConstructL()
    {
    FUNC_LOG;
    
    INFO( "Creating Retry Timer object" );
    User::LeaveIfError(iRetryTimer.CreateLocal());
    
    INFO( "Creating CDdcPortAccess object" );
    iDdcPortAccess = CDdcPortAccess::NewL();

    CActiveScheduler::Add( this );
    }

// ======== GLOBAL FUNCTIONS ========


