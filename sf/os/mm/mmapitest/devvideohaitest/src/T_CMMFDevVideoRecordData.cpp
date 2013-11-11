/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_cmmfdevvideorecorddata.h"

/*@{*/
//LIT's to identify commands
_LIT(KCmdNewL,						"NewL");
_LIT(KCmdDestructor,				"~");
_LIT(KCmdSelectEncoderL,			"SelectEncoderL");
_LIT(KCmdSetInputFormatL,			"SetInputFormatL");
_LIT(KCmdSetOutputFormatL,			"SetOutputFormatL");
_LIT(KCmdSetBufferOptionsL,			"SetBufferOptionsL");
_LIT(KCmdSetSourceMemoryL,			"SetSourceMemoryL");
_LIT(KCmdSetMinRandomAccessRate,	"SetMinRandomAccessRate");
_LIT(KCmdInitialize,				"Initialize");
_LIT(KCmdSetComplexityLevel,		"SetComplexityLevel");
_LIT(KCmdStart,						"Start");
_LIT(KCmdPause,						"Pause");
_LIT(KCmdResume,					"Resume");
_LIT(KCmdStop,						"Stop");
_LIT(KCmdUtilityGetEnvironment,  	"UtilityGetEnvironment");
_LIT(KCmdSetRateControlOptions,  	"SetRateControlOptions");


/*@}*/

/*@{*/
//LIT's to get params from ini files
_LIT(KInputFile,			"inputfile");
_LIT(KOutputFile,			"outputfile");
_LIT(KInputPath,			"inputPath");
_LIT(KOutputPath,			"outputPath");
_LIT(KFrameRate,			"framerate");
_LIT(KUid,					"uid");
_LIT(KMimeType,				"mimeType");
_LIT(KDataUnitType,			"DataUnitType");
_LIT(KDataEncapsulation,	"DataEncapsulation");
_LIT(KHrdVbvSpecification,	"HrdVbvSpecification");
_LIT(KMaxPictureSize,		"maxPictureSize");
_LIT(KRandomAccessRate, 	"randomAccessRate");
_LIT(KComplexityLevel,		"complexityLevel");
_LIT(KRawPicLength , 		"rawPicLength");
_LIT(KPicWidth,				"picWidth");
_LIT(KPicHeight,			"picHeight");
_LIT(KHrdVbvNone, 			"EHrdVbvNone");
_LIT(KHrdVbvCodingStandard, "EHrdVbvCodingStandard");
_LIT(KHrdVbv3GPP, 			"EHrdVbv3GPP");
_LIT(KLayer, 			    "layer");
_LIT(KBitrateControlType, 	"bitrateControlType");
_LIT(KBitrate, 			    "bitrate");
_LIT(KPictureQuality, 		"pictureQuality");
_LIT(KPicRate, 			    "picRate");
_LIT(KQualityTempTradeOff, 	"qualityTempTradeOff");
_LIT(KLatencyQuality, 		"latencyQuality");
/*@{*/

const CDataWrapperBase::TEnumEntryTable	CT_MMFDevVideoRecordData::iHrdVbvSpecification[] =
	{
	KHrdVbvNone, 			EHrdVbvNone,
	KHrdVbvCodingStandard, 	EHrdVbvCodingStandard,
	KHrdVbv3GPP, 			EHrdVbv3GPP
	};

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_MMFDevVideoRecordData* CT_MMFDevVideoRecordData::NewL()
	{
	CT_MMFDevVideoRecordData* self = new(ELeave)CT_MMFDevVideoRecordData();
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (self);
	return self;
	}

/**
 * Public destructor
 */
CT_MMFDevVideoRecordData::~CT_MMFDevVideoRecordData()
	{
	INFO_PRINTF1(_L("Destroying data..."));
	DestroyData();
	INFO_PRINTF1(_L("Data destroyed"));
	}

/**
 * Protected constructor. First phase construction
 */
CT_MMFDevVideoRecordData::CT_MMFDevVideoRecordData()
	:	
	iBufferCount(0),	
	iTimeStamp(0), 	
	iWrittenPictures(0), 	
	iReturnedPictures(0), 	
	iCompressedBuffers(0), 	
	iBufferedPictures(0),	
	iStopAfterAllFramesReturned(EFalse), 	
	iInputEnd(EFalse),	
	iDatabuf(),	
	iRequests(0),	
	iOutputRequests(0)
	{
	}


void CT_MMFDevVideoRecordData::ConstructL()
	{
	iFs.Connect();
	}

/**
 * Returns a pointer to the object that the data wraps
 * @return wrapped object
 */
TAny* CT_MMFDevVideoRecordData::GetObject()
	{
	return iDevvr;
	}


/*
 * COMMANDS
 */

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to 
 * @return					ETrue if the command is processed *
 * @leave					System wide error
 */
TBool CT_MMFDevVideoRecordData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool cmdProcessed = ETrue;
	if(aCommand == KCmdNewL())
		{
		DoCmdNewL(aSection);
		}
	else if(aCommand == KCmdDestructor())
		{
		DoCmdDestructor();
		}
	else if(aCommand == KCmdSelectEncoderL())
		{
		DoCmdSelectEncoderL(aSection);
		}
	else if(aCommand == KCmdSetInputFormatL())
		{
		DoCmdSetInputFormatL();
		}
	else if(aCommand == KCmdSetOutputFormatL())
		{
		DoCmdSetOutputFormatL(aSection);
		}
	else if(aCommand == KCmdSetBufferOptionsL())
		{
		DoCmdSetBufferOptionsL(aSection);
		}
	else if(aCommand == KCmdSetSourceMemoryL())
		{
		DoCmdSetSourceMemoryL();
		}
	else if(aCommand == KCmdSetMinRandomAccessRate())
		{
		DoCmdSetMinRandomAccessRate(aSection);
		}
	else if(aCommand == KCmdInitialize())
		{
		DoCmdInitialize(aAsyncErrorIndex);
		}
	else if(aCommand == KCmdSetComplexityLevel())
		{
		DoCmdSetComplexityLevel(aSection);
		}
	else if(aCommand == KCmdStart())
		{
		DoCmdStart(aAsyncErrorIndex);
		}
	else if(aCommand == KCmdPause())
		{
		DoCmdPause();
		}
	else if(aCommand == KCmdResume())
		{
		DoCmdResume();
		}
	else if(aCommand == KCmdStop())
		{
		DoCmdStop();
		}
	else if ( aCommand==KCmdUtilityGetEnvironment )
		{
		DoCmdUtilityGetEnvironmentL();
		}
	else if ( aCommand==KCmdSetRateControlOptions )
		{
		DoCmdSetRateControlOptions(aSection);
		}
	else
		{
		cmdProcessed = EFalse;
		}

	return cmdProcessed;
	}

/**
 * DoCmdNewL
 * @param aSection			The section in the ini containing data for the command
 */
void CT_MMFDevVideoRecordData::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdNewL"));
	DestroyData();
	TRAPD(error,iDevvr = CMMFDevVideoRecord::NewL( *this ));
	if(error!=KErrNone)
		{
		ERR_PRINTF2(_L("Error creating CMMFDevVideoRecord %i"), error);
		SetError(error);
		}
	else
		{
		CreateBuffers();
	
		if(CreateFiles(aSection)!= KErrNone)
			{
			ERR_PRINTF1(_L("Error creating input/output files"));
			SetBlockResult(EFail);
			}
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdNewL"));
	}

/**
 * DoCmdDestructor
 */
void CT_MMFDevVideoRecordData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdDestructor"));
	iInFile.Close();   
	iOutFile.Close();
	iFs.Close();
	REComSession::FinalClose();	
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdDestructor"));
	}

/**
 *	DoCmdSelectEncoderL
 *  Selects the video encoder to be used. This method must be called before any other video encoder related 
 *  methods are used. The encoder can be changed by calling this method again before the system has been 
 *  initialized with Initialize().
 *  All video encoder settings are reset to their default values. By default no pre-processing is performed.
 *  @param aSection			The section in the ini containing data for the command
 */
void CT_MMFDevVideoRecordData::DoCmdSelectEncoderL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSelectEncoderL"));
	TInt hUid = 0;

	if(!GetHexFromConfig(aSection, KUid(), hUid))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUid());
		SetBlockResult(EFail);
		}
	else
		{
		TUid Uid = TUid::Uid(hUid);
		SetPictureTypeL(aSection);
		TRAPD(error,iEncHWDevId = iDevvr->SelectEncoderL(Uid));
		if(error!=KErrNone)
			{
			iEncHWDevId = NULL;		
			ERR_PRINTF2(_L("Error Selecting encoder: %i"), error);
			SetError(error);
			}
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSelectEncoderL"));
	}

/*
 *	Sets the input format for a hardware device. If both a pre-processor and an encoder are used, the 
 *	pre-processor output format and the encoder input format must be the same. The input format for the 
 *	first device in the system is the input format for video input data.
*/
void CT_MMFDevVideoRecordData::DoCmdSetInputFormatL()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetInputFormatL"));
	TUncompressedVideoFormat unCompressedFormat;	

	unCompressedFormat.iDataFormat = EYuvRawData; 
	unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataPlanar;
	unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma1;
	unCompressedFormat.iYuvFormat.iYuv2RgbMatrix = NULL;
	unCompressedFormat.iYuvFormat.iRgb2YuvMatrix = NULL;
	unCompressedFormat.iYuvFormat.iAspectRatioNum = 1;
	unCompressedFormat.iYuvFormat.iAspectRatioDenom = 1;
	unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt601Range0;

	TRAPD(error,iDevvr->SetInputFormatL( iEncHWDevId, unCompressedFormat, iPictSize ));
	if(error!=KErrNone)
		{
		ERR_PRINTF2(_L("Error Setting input format: %i"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetInputFormatL"));
	}

/*
 *	Sets the output format for a hardware device to a compressed video format. Only applicable for encoder 
 *	devices. The picture size depends on the input data format and possible scaling performed.
 *  	@param aSection			The section in the ini containing data for the command
 */
void CT_MMFDevVideoRecordData::DoCmdSetOutputFormatL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetOutputFormatL"));
	TBuf8<50> mimetype;		
	TVideoDataUnitType dataUnitType;
	TVideoDataUnitEncapsulation dataEncapsulation;
	TBool dataOk = ETrue;
	TInt iDataUnitType = 0;
	TInt iDataEncapsulation = 0;
		
	TPtrC mimeType;
	if(!GetStringFromConfig(aSection, KMimeType(), mimeType))
		{
		ERR_PRINTF1(_L("Mime type not retrieved, command will not be executed"));
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(!GetHexFromConfig(aSection, KDataUnitType(), iDataUnitType))
		{
		ERR_PRINTF2(_L("check parameter: %S"), &KDataUnitType());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(!GetHexFromConfig(aSection, KDataEncapsulation(), iDataEncapsulation))
		{
		ERR_PRINTF2(_L("check parameter: %S"), &KDataEncapsulation());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		mimetype.Append(mimeType);
		CCompressedVideoFormat* compressedFormat=NULL;
		TRAPD(cfError, compressedFormat = CCompressedVideoFormat::NewL(mimetype));
		CleanupStack::PushL(compressedFormat);
		if(cfError != KErrNone)
			{
			ERR_PRINTF2(_L("Error selecting compressed video format, check selected mimeType: %i"), cfError);
			SetError(cfError);
			}
		else
			{
			dataUnitType = static_cast<TVideoDataUnitType>(iDataUnitType);
			dataEncapsulation = static_cast<TVideoDataUnitEncapsulation>(iDataEncapsulation);
			
			TRAPD(error,iDevvr->SetOutputFormatL(iEncHWDevId, *compressedFormat, dataUnitType, dataEncapsulation, EFalse));  	
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("Couldn't set OutputFormat %i"), error);	
				delete iDevvr;
				iDevvr = NULL;
				SetError(error);
				}
			}
		CleanupStack::PopAndDestroy(compressedFormat);
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetOutputFormatL"));
	}

/*
 *	Sets encoder buffering options.
 *	@param aSection			The section in the ini containing data for the command
*/
void CT_MMFDevVideoRecordData::DoCmdSetBufferOptionsL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetBufferOptionsL"));
	TEncoderBufferOptions bufferOptions;
	TInt HrdVbvSpecification = 0;
	TInt maxPictureSize = 0;
	TBool dataOk = ETrue;
	if(!GetEnumFromConfig(aSection, KHrdVbvSpecification(), iHrdVbvSpecification, HrdVbvSpecification))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHrdVbvSpecification());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(!GetIntFromConfig(aSection, KMaxPictureSize(), maxPictureSize))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaxPictureSize());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}	
	if(dataOk)
		{
		bufferOptions.iHrdVbvParams.Set(NULL, 0);
		bufferOptions.iHrdVbvSpec = (THrdVbvSpecification)HrdVbvSpecification;
		bufferOptions.iMaxPreEncoderBufferPictures = KNumPictures;
		bufferOptions.iMaxCodedPictureSize = maxPictureSize;
		bufferOptions.iMaxOutputBufferSize = maxPictureSize;      
		bufferOptions.iMaxCodedSegmentSize = maxPictureSize;
	
		//Not needed more that 4 input buffers for normal fileRead test cases
		bufferOptions.iMinNumOutputBuffers = KNumPictures - 2;
		TRAPD(error, iDevvr->SetBufferOptionsL( bufferOptions ));
		if(error!=KErrNone)
			{
			ERR_PRINTF2(_L("Error Setting buffer options: %i"), error);
			SetError(error);
			}	
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetBufferOptionsL"));
	}
/*
 *	Sets the data source to be memory buffers.
 */
void CT_MMFDevVideoRecordData::DoCmdSetSourceMemoryL()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetSourceMemoryL"));
	
	TRAPD(error, iDevvr->SetSourceMemoryL(iFrameRate, EFalse, EFalse));	
	if(error != KErrNone)
		{
		ERR_PRINTF2(_L("Error Setting Source Memory: %i"), error);
		SetError(error);
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetSourceMemoryL"));
	}

/*
 *	Sets the minimum frequency (in time) for instantaneous random access points in the bitstream. 
 *	An instantaneous random access point is such where the encoder can achieve a full output picture 
 *	immediately by encoding data starting from the random access point. The random access point frequency
 *	may be higher than signalled, if the sequence contains scene cuts which typically cause a coding of 
 *	a random access point.
 *	@param aSection			The section in the ini containing data for the command
*/
void CT_MMFDevVideoRecordData::DoCmdSetMinRandomAccessRate(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetMinRandomAccessRate"));
	TReal randomAccessRate = 0; 
	
	if(!GetRealFromConfig(aSection, KRandomAccessRate(), randomAccessRate))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRandomAccessRate());
		SetBlockResult(EFail);
		}
	else
		{
		iDevvr->SetMinRandomAccessRate( randomAccessRate );
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetMinRandomAccessRate"));
	}

/*
 * Initializes the video devices, and reserves hardware resources. This method is asynchronous, 
 *	DevVideoRecord will call MMMFDevVideoRecordObserver::MdvroInitializeComplete() after initialization has 
 *	completed. If direct capture is used, this method also prepares the camera API for capture by calling 
 *	PrepareVideoCaptureL(). No DevVideoRecord method may be called while initialization is in progress, the 
 *	initialization process can only be cancelled by destroying the DevVideoRecord object. 
 *	@param a AsyncErrorIndex 	Command index for async calls to return errors to
*/
void CT_MMFDevVideoRecordData::DoCmdInitialize(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdInitialize"));
	iAsyncErrorIndex = aAsyncErrorIndex;
	IncOutstanding();
	iDevvr->Initialize();	
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdInitialize"));
	}

/*
 * Sets the complexity level to use for video processing in a hardware device. The level can be changed at 
 * any time. 
 * @param a AsyncErrorIndex 	Command index for async calls to return errors to
*/
void CT_MMFDevVideoRecordData::DoCmdSetComplexityLevel(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdSetComplexityLevel"));
	TInt complexityLevel = 0; 
	
	INFO_PRINTF1(_L("SET COMPLEXITY LEVEL begin"));
	
	if(!GetIntFromConfig(aSection, KComplexityLevel(), complexityLevel))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KComplexityLevel());
		SetBlockResult(EFail);
		}
	else
		{
		iDevvr->SetComplexityLevel( iEncHWDevId, complexityLevel );
		INFO_PRINTF1(_L("SET COMPLEXITY LEVEL stop"));	
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdSetComplexityLevel"));
	}

/*
 *	Starts recording video. This includes capturing pictures from the camera (if direct capture is used), 
 *	pre-processing and encoding. Recording will proceed until it is stopped or paused. Initally recording 
 *	is stopped.
 * 	@param a AsyncErrorIndex 	Command index for async calls to return errors to
 */
void CT_MMFDevVideoRecordData::DoCmdStart(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdStart"));
	iAsyncErrorIndex = aAsyncErrorIndex;
	
	iDevvr->Start();
	
	iRequests++;
	if ( FillAndSendBuffer() == 0 ) 
		{
		iStopAfterAllFramesReturned = ETrue;
		}
	IncOutstanding();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdStart"));
	}

/*
 * 	Pauses video recording. Recording can be resumed using Resume().
 */
void CT_MMFDevVideoRecordData::DoCmdPause()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdPause"));
	iDevvr->Pause();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdPause"));
	}

/*
 * 	Resumes video recording after a pause.
 */
void CT_MMFDevVideoRecordData::DoCmdResume()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdResume"));
	iDevvr->Resume();
	HandleReturnedPicture();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdResume"));
	}

/**
 * Stops recording video. No new pictures will be captured, pre-processed, or encoded.
 */
void CT_MMFDevVideoRecordData::DoCmdStop()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdStop"));
	iDevvr->Stop();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdStop"));	
	}

/*
 * CALLBACKS
 */

/**
 * Returns a used input video picture back to the caller. The picture memory can be re-used or freed. 
 * 
 * @param aPicture	the picture to return
 */
void CT_MMFDevVideoRecordData::MdvroReturnPicture(TVideoPicture* aPicture)
	{
	TInt index = 0;

	iReturnedPictures++;
	iBufferedPictures--;

	for ( index = 0; index < KBufferInputPictures; index++)
		{
		if ( &(iPicture[index]) == aPicture )
			{//set it free
			iPicture[index].iOptions = 0;
			}
		}	
	HandleReturnedPicture();
	}

/**
 * Callback not used.
 * 
 * @return None
 */
void CT_MMFDevVideoRecordData::MdvroSupplementalInfoSent()
	{
	}

/**
 * Observer callback in Custom interface Buffer management. 
 * Client is notified a new buffer(s) is available 
 * 
 * @return None
 */
void CT_MMFDevVideoRecordData::MdvroNewBuffers()
	{
	iCompressedBuffers ++;
	HandleNewBuffer();
	}

/**
 * Reports a fatal decoding or playback error to the client.
 * 
 * @param aError	The error code
 * 
 * @return None
 */      
void CT_MMFDevVideoRecordData::MdvroFatalError(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::MdvroFatalError"));

	DecOutstanding();

	ERR_PRINTF2(_L("A fatal error ocurred %i"), aError);
	SetAsyncError (iAsyncErrorIndex, aError);
	
	if(iDevvr != NULL)
		{
		delete iDevvr;
		iDevvr = NULL;
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::MdvroFatalError"));
	}

/**
 * Reports that DevVideoRecord initialization has completed. The interface can now be used for video recording. 
 * 
 * @param aError	Initialization error code, KErrNone if no error occurred
 * 
 * @return None
 */
void CT_MMFDevVideoRecordData::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::MdvroInitializeComplete"));
	DecOutstanding();
	if ( aError != KErrNone )
        {
        SetAsyncError (iAsyncErrorIndex, aError);
        ERR_PRINTF2(_L("Error at MdvroInitializeComplete %i"), aError);
        }
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::MdvroInitializeComplete"));
	}

/**
 * Reports that the input video stream end has been reached and all pictures have been processed.
 * This method is only called after the client has called InputEnd(). No more output pictures will be available.
 * 
 * @return None
 */
void CT_MMFDevVideoRecordData::MdvroStreamEnd()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::MdvroStreamEnd"));
	DecOutstanding();
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::MdvroStreamEnd"));
	}

/*
 * HELPERS
 */

/**
 * Sets the Picture Type for the test
 * @param aSection			The section in the ini containing data for the command
 */
void CT_MMFDevVideoRecordData::SetPictureTypeL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::SetPictureTypeL"));
	TInt rawPicLength=0;
	TInt picWidth=0;
	TInt picHeight=0;	
	
	if(!GetIntFromConfig(aSection, KFrameRate(), iFrameRate))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFrameRate());
		SetBlockResult(EFail);
		}
	
	if(!GetIntFromConfig(aSection, KRawPicLength(), rawPicLength))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRawPicLength());
		SetBlockResult(EFail);
		}
	
	if(!GetIntFromConfig(aSection, KPicWidth(), picWidth))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRawPicLength());
		SetBlockResult(EFail);
		}	
	
	if(!GetIntFromConfig(aSection, KPicHeight(), picHeight))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRawPicLength());
		SetBlockResult(EFail);
		}	
		
	iPictSize.SetSize(picHeight,picWidth);
	
	INFO_PRINTF1(_L("Creating Data buffers"));
	TUint8* pointer = NULL; 
	if (!iDatabuf)
        {
        //iDatabuf = new (ELeave) TUint8*[KBufferInputPictures];
        iDatabuf = new RPointerArray<TUint8>[KBufferInputPictures];
        
        for ( TInt i = 0; i < KBufferInputPictures; i ++ ) 
            {              
            pointer = new (ELeave) TUint8[rawPicLength];
            iDatabuf[i].Append(pointer);
            iPicture[i].iData.iRawData = new (ELeave) TPtr8(0, 0, 0);
            iPicture[i].iData.iRawData->Set(pointer, rawPicLength, rawPicLength );
            iPicture[i].iData.iDataSize.SetSize(iPictSize.iWidth, iPictSize.iHeight);
            }
        }
	INFO_PRINTF1(_L("Data buffers created"));
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::SetPictureTypeL"));
	}

/**
 * create buffers to read data
 */
void CT_MMFDevVideoRecordData::CreateBuffers()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::CreateBuffers"));
	for (TInt i = 0; i < KBufferInputPictures; i ++)
		{
		INFO_PRINTF1(_L("creating buffers"));
		iPicture[i].iData.iRawData = NULL;
		INFO_PRINTF1(_L("buffers created"));
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::CreateBuffers"));
	}
	

/**
 *checks for input file and creates output files
 * @param aSection			The section in the ini containing data for the command
 */
int CT_MMFDevVideoRecordData::CreateFiles(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::CreateFiles"));
	TInt err = KErrNone;
	TPtrC inputFile;
	TPtrC outputFile;
	TPtrC inputPath;
	TPtrC outputPath;
	if(!GetStringFromConfig(aSection, KInputFile(), inputFile))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputFile());
		SetBlockResult(EFail);
		err = KErrNotFound;
		}
	if(!GetStringFromConfig(aSection, KOutputFile(), outputFile))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputFile());
		SetBlockResult(EFail);
		err = KErrNotFound;
		}
	if(!GetStringFromConfig(aSection, KInputPath(), inputPath))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputPath());
		SetBlockResult(EFail);
		err = KErrNotFound;
		}
	if(!GetStringFromConfig(aSection, KOutputPath(), outputPath))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputFile());
		SetBlockResult(EFail);
		err = KErrNotFound;
		}	
	
    if(err == KErrNone)
    	{
		TFileName inFileName(inputPath);
	    inFileName.Append(inputFile);
	    TFileName outFileName(outputPath);
	    outFileName.Append(outputFile);
	    
	    err = OpenFile(outFileName, inFileName);
    	}
    INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::CreateFiles"));
    return err;
	}

/**
 * Opens input file.
 * Creates output file.
 * @param aOutFileName the output file
 * @param aInFileName  the input file
 */
int CT_MMFDevVideoRecordData::OpenFile(TFileName& aOutFileName, TFileName& aInFileName)
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::OpenFile"));
	TInt err = KErrNone;
	if ( (err=iInFile.Open(iFs, aInFileName, EFileRead | EFileShareReadersOnly)) != KErrNone)
		{  
		ERR_PRINTF2(_L("Couldn't open input file: %i"),err);
		}        
	else
		{
		if((err=iOutFile.Replace(iFs, aOutFileName, EFileShareExclusive|EFileWrite))!= KErrNone)
			{
			ERR_PRINTF2(_L("Couldn't open output file: %i"),err);
			}
		}
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::OpenFile"));
	return err;
	}

/**
 * gets new buffers from buffer array
 */
void CT_MMFDevVideoRecordData::HandleNewBuffer()
	{
	TVideoOutputBuffer* tmp=NULL;
	TRAPD(error, tmp = iDevvr->NextBufferL());           
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Error in getting NextBufferL, Error: %d"), error);
		SetBlockResult(EFail);
		}
	else
		{
		SaveAndReturnBuffer( tmp );
		}
	}

/**
 * handles the returned pictures 
 */
void CT_MMFDevVideoRecordData::HandleReturnedPicture()
	{
	if ( iStopAfterAllFramesReturned )
		{
		}
	else
		{
		//iOutputRequests++;
		if ( FillAndSendBuffer() == 0 ) 
			{
			iStopAfterAllFramesReturned = ETrue;
			}
		}
	}

/**
 *handles out to file requests 
 *@param aNumberOfPictures number of pictures to write
 */
TInt CT_MMFDevVideoRecordData::HandleOutputRequest()
	{
	TInt request = 1;
	if ( FillAndSendBuffer() == 0 ) 
		{
		//all data read => stop
		iStopAfterAllFramesReturned = ETrue;
		request = 0;
		}
	return request;
	}

/**
 *writes buffer to file
 *@param tmp current buffer
 */
void CT_MMFDevVideoRecordData::SaveAndReturnBuffer(TVideoOutputBuffer* tmp)
    {
    iOutFile.Write( tmp->iData );
    iDevvr->ReturnBuffer( tmp );
    }

/**
 *fills buffer data buffer and process it
 */
TInt CT_MMFDevVideoRecordData::FillAndSendBuffer()
	{
	TInt64 tsValue = 0;
	TInt index = 0;
	TInt status = KErrNone;
	TInt error = 1;
	if (iStopAfterAllFramesReturned)
		{
		error = 0;
		}
	else
		{
		for ( index = 0; index < KBufferInputPictures; index++ )
			{// find a free buffer
			if ( iPicture[index].iOptions == 0 )
				{            
				break;
				}
			} 
	
		if ( index == KBufferInputPictures )
			{// all buffers in use, hold on for now        
			error = -1;
			}
		else
			{
			// Reset buffer length
			iPicture[index].iData.iRawData->SetMax();
		
			status = iInFile.Read( *(iPicture[index].iData.iRawData));
			if ( status != KErrNone )
				{    
				iStopAfterAllFramesReturned = ETrue;
				iDevvr->InputEnd();
				error = 0;
				}
			else
				{
				// If the buffer is 0 or incomplete, stop test case
				if ( iPicture[index].iData.iRawData->Length() == 0 || iPicture[index].iData.iRawData->Length() < iPicture[index].iData.iRawData->MaxLength() )
					{        
					iStopAfterAllFramesReturned = ETrue;
					iDevvr->InputEnd();
					error = 0;
					}
				else
					{
					// write data
					tsValue = TReal(iTimeStamp) * TReal(1000000) / TReal(KVEDSPH263TimerResolution);
					iPicture[index].iTimestamp = tsValue;
					iPicture[index].iData.iDataFormat = EYuvRawData;
					iWrittenPictures++;  
					iBufferedPictures++;    
					iPicture[index].iOptions = 1;//mark to be used
					// send it as usually
					TRAPD(error, iDevvr->WritePictureL( &(iPicture[index]) ));
					if(error!=KErrNone)
						{
						ERR_PRINTF2(_L("Error int WritePictureL Error: %d"), error);
						SetError(error);
						}
					iTimeStamp ++;
					}
				}
			}
		}
	return error;
	}

void CT_MMFDevVideoRecordData::DoCmdUtilityGetEnvironmentL()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DoCmdUtilityGetEnvironmentL"));
	RArray<TUid> decoders;	
	RArray<TPictureRateAndSize> maxPictureRates;
	TBuf<255> mime;
	CleanupClosePushL (decoders);	
	iDevvr->GetEncoderListL(decoders);
	CVideoEncoderInfo* info = NULL;
	for (TInt i = 0; i < decoders.Count ();i++)
		{
		info =iDevvr->VideoEncoderInfoLC(decoders[i]);		
		TSize maxSize = info->MaxPictureSize();
		INFO_PRINTF6(_L("\n%S\nManufacturer%S\nUid: 0x%x\nAccelerated: %i\nSupports Direct Capture: %i"), 
				&info->Identifier(),
				&info->Manufacturer(),
				decoders[i].iUid,
				info->Accelerated(),
				info->SupportsDirectCapture());
		
		INFO_PRINTF3(_L("\nMax width %i, Max height %i"),maxSize.iWidth, maxSize.iHeight);
		
		maxPictureRates = info->MaxPictureRates(); 

		INFO_PRINTF4(_L("\nData unit types supported:%d\nData unit encapsulations:%d\nMaximum number of bitrate layers supported:%d"),
				info->SupportedDataUnitTypes(),
				info->SupportedDataUnitEncapsulations(),
				info->MaxBitrateLayers()
				);
		

		INFO_PRINTF4(_L("\nPicture options supported:%d\nSupportsPictureLoss:%i\nSupportsSliceLoss:%i"),
				info->SupportedPictureOptions(),
				info->SupportsPictureLoss(),
				info->SupportsSliceLoss()
				);
		
		INFO_PRINTF4(_L("\nSupplemental enhancement info:%i\nMaximum unequal error protection level:%d\nMaxBitrate:%d"),
				info->SupportsSupplementalEnhancementInfo(),
				info->MaxUnequalErrorProtectionLevels(),
				info->MaxBitrate()
				);
		
		INFO_PRINTF2(_L("Coding-standard specific information: %S"),&info->CodingStandardSpecificInfo());
		TInt j = 0;
		for (j = 0; j < maxPictureRates.Count(); j++)
			{
			TSize pictureSize=maxPictureRates[j].iPictureSize;			
			INFO_PRINTF4(_L("max width: %i, max height %i, max rate %f"),
					pictureSize.iWidth, pictureSize.iHeight, maxPictureRates[j].iPictureRate);
			}
		
		const RPointerArray<CCompressedVideoFormat>& outputFormats =info->SupportedOutputFormats();
		for (j = 0; j < outputFormats.Count(); j++)
			{
			mime.Copy(outputFormats[j]->MimeType());		
			INFO_PRINTF3(_L("Supported type %d MIMETYPE: %S"),j,&mime);					
			}
		CleanupStack::Pop (info);
		}
	CleanupStack::PopAndDestroy(&decoders);
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DoCmdUtilityGetEnvironmentL"));
	}

void CT_MMFDevVideoRecordData::DoCmdSetRateControlOptions(const TTEFSectionName& aSection)
    {
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::SetRateControlOptions"));


	TInt layer = 0;
	TInt bitrateControlInt = 0;
	TInt bitrate = 0;
	TInt pictureQuality = 0;
	TReal pictureRate;  
	TReal qualityTemporalTradeoff;  
	TReal latencyQualityTradeoff; 	
	
    if(!GetIntFromConfig(aSection, KLayer(), layer))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLayer());
        SetBlockResult(EFail);
        }
    if(!GetIntFromConfig(aSection, KBitrateControlType(), bitrateControlInt))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBitrateControlType());
        SetBlockResult(EFail);
        }	
    if(!GetIntFromConfig(aSection, KBitrate(), bitrate))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLayer());
        SetBlockResult(EFail);
        }		
    if(!GetIntFromConfig(aSection, KPictureQuality(), pictureQuality))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPictureQuality());
        SetBlockResult(EFail);
        }	
    if(!GetRealFromConfig(aSection, KPicRate(), pictureRate))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPicRate());
        SetBlockResult(EFail);
        }	
    if(!GetRealFromConfig(aSection, KQualityTempTradeOff(), qualityTemporalTradeoff))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KQualityTempTradeOff());
        SetBlockResult(EFail);
        }	
    if(!GetRealFromConfig(aSection, KLatencyQuality(), latencyQualityTradeoff))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLatencyQuality());
        SetBlockResult(EFail);
        }	

	TBitrateControlType bitrateControlType;
	switch (bitrateControlInt) 
		{
		case 1: bitrateControlType = EBrControlNone;
				break;
		case 2: bitrateControlType = EBrControlStream;
				break;		
		case 3: bitrateControlType = EBrControlPicture;
				break;		
		default:bitrateControlType = EBrControlStream; 
		}


    TRateControlOptions rateControlOptions;
	rateControlOptions.iControl = bitrateControlType;
	rateControlOptions.iBitrate = bitrate;
	rateControlOptions.iPictureQuality = pictureQuality;
	rateControlOptions.iPictureRate = pictureRate;
	rateControlOptions.iQualityTemporalTradeoff = qualityTemporalTradeoff;
	rateControlOptions.iLatencyQualityTradeoff = latencyQualityTradeoff;

	iDevvr->SetRateControlOptions(layer, rateControlOptions);  

	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::SetRateControlOptions"));

	}





void CT_MMFDevVideoRecordData::DestroyData()
	{
	INFO_PRINTF1(_L("*START*CT_MMFDevVideoRecordData::DestroyData"));
	delete iDevvr;
	iDevvr = NULL;
	INFO_PRINTF1(_L("*END*CT_MMFDevVideoRecordData::DestroyData"));
	}
