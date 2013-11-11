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


#include <datawrapper.h>
#include <devvideoplay.h>
#include <devvideobase.h> 
#include <hal.h>
#include <devvideoconstants.h>
#include "t_cmmfdevvideoplaydata.h"
#include "t_devvideoconstants.h"

/*@{*/
//LIT's to get params from ini files
_LIT(KPostPro,								"postprocessor");
_LIT(KCodec,								"codec");
_LIT(KCombination,							"combination");
_LIT(KScreenout,							"screen");
_LIT(KFramerate,							"framerate");
_LIT(KInputfile,							"inputfile");
_LIT(KOutputfile,							"outputfile");
_LIT(KSynch,								"synch");
_LIT(KDecoder, 								"dec");
_LIT(KRotation, 							"rotation");
_LIT(KWidth, 								"width");
_LIT(KHeight, 								"height");
_LIT(Kx, 									"x");
_LIT(Ky, 									"y");
_LIT(KPos, 									"pos");
_LIT(KAntiAliasFiltering,					"antiAliasFiltering");
_LIT(KMimeType,								"mimeType");
_LIT(KUseDecoder,							"useDecoder");
_LIT(KInputPath,							"inputPath");
_LIT(KOutputPath,							"outputPath");
_LIT(KDisplayMode,							"displayMode");
_LIT(KBuf,									"buffer");
_LIT(KDataInOrder, 							"dataInOrder");
_LIT(KTVideoDataUnitType, 					"TVideoDataUnitType");
_LIT(KTVideoDataUnitEncapsulation, 			"TVideoDataUnitEncapsulation");
//H264
_LIT(KUid,									"uidInt");
_LIT(KBuffMgmt,								"buffMgmt");
_LIT(KOutput,								"output");
_LIT(KHwdev,								"hwdev");
_LIT(KDataUnitTypeInt,						"dataUnitTypeInt");
_LIT(KEncapsulation,						"encapsulation");
_LIT(KFormat,								"format");
_LIT(KDataFormat,                           "dataFormat");
_LIT(KDataLayout,                           "dataLayout");
_LIT(KPattern,                              "pattern");
_LIT(KAspectRatioNum,                       "aspectRatioNum");
_LIT(KAspectRatioDenom,                     "aspectRatioDenom");
_LIT(KCoefficients,                         "coefficients");
_LIT(KPreDecodeBufferSize,                  "preDecodeBufferSize");
_LIT(KMaxPostDecodeBufferSize,              "maxPostDecodeBufferSize");
_LIT(KPreDecoderBufferPeriodInt,			"preDecoderBufferPeriodInt");
_LIT(KPostDecoderBufferPeriodInt,           "postDecoderBufferPeriodInt");
_LIT(KMaxInputBufferSize,                   "maxInputBufferSize");
_LIT(KMinNumInputBuffers,                   "minNumInputBuffers");
_LIT(KInputEnd,								"inputEnd");
_LIT(KDest,                                 "dest");
_LIT(KX,                                    "x");
_LIT(KY,                                    "y");
_LIT(KSync,                                 "sync");
/*@}*/

/*@{*/
//LIT's to identify commands
_LIT(KCmdNewL,								"NewL");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdInitialize,						"Initialize");
_LIT(KCmdStart,								"Start");
_LIT(KCmdStop,								"Stop");
_LIT(KCmdPause,								"Pause");
_LIT(KCmdResume,							"Resume");
_LIT(KCmdSetRotateOptionsL,					"SetRotateOptionsL");
_LIT(KCmdSetScaleOptionsL,					"SetScaleOptionsL");
_LIT(KCmdSetPosition,						"SetPosition");
_LIT(KCmdSetClockSource,					"SetClockSource");
_LIT(KCmdSelectDecoderL,					"SelectDecoderL");
_LIT(KCmdSetInputFormat,					"SetInputFormat");
_LIT(KCmdSelectPostProcessorL,				"SelectPostProcessorL");
_LIT(KCmdPostProcessorInfoLC,				"PostProcessorInfoLC");
_LIT(KCmdFindCommonFormat,					"FindCommonFormat");
_LIT(KCmdSetOutputFormatL,					"SetOutputFormatL");
_LIT(KCmdGetHeaderInformationL,				"GetHeaderInformationL");
_LIT(KCmdReturnHeader,						"ReturnHeader");
_LIT(KCmdSetVideoDestScreenL,				"SetVideoDestScreenL");
_LIT(KCmdGetBufferOptions,					"GetBufferOptions");
_LIT(KCmdSetBufferOptionsL,					"SetBufferOptionsL");
_LIT(KCmdSetPostProcessTypesL,				"SetPostProcessTypesL");
_LIT(KCmdGetOutputFormatListL,				"GetOutputFormatListL");
_LIT(KCmdAbortDirectScreenAccess,			"AbortDirectScreenAccess");
_LIT(KCmdStartDirectScreenAccess,			"StartDirectScreenAccess");
_LIT(KCmdUtilityGetEnvironment,  			"UtilityGetEnvironment");
/*@}*/
_LIT(KCmdH264CreateDecTest,  				"H264CreateDecTest");
_LIT(KCmdH264SelectPostProcessor,           "H264SelectPostProcessor");
_LIT(KCmdH264SetPostProcessTypes,           "H264SetPostProcessTypes");
_LIT(KCmdH264SetInputFormatCompr,			"H264SetInputFormatCompr");	
_LIT(KCmdH264SetOutputFormat,				"H264SetOutputFormat");
_LIT(KCmdH264SetBufferOptions,				"H264SetBufferOptions");
_LIT(KCmdH264ListFrameSize,                 "H264ListFrameSize");
_LIT(KCmdH264SetVideoDestScreen,            "H264SetVideoDestScreen"); 
_LIT(KCmdH264SetWindowRect,                 "H264SetWindowRect");    
_LIT(KCmdH264Initialize,					"H264Initialize");
_LIT(KCmdH264Start,							"H264Start");
_LIT(KCmdH264Stop,							"H264Stop");
_LIT(KCmdH264Pause,                         "H264Pause");
_LIT(KCmdH264Resume,                        "H264Resume");
_LIT(KCmdH264IOFrameMatch,					"H264IOFrameMatch");
_LIT(KCmdH264FindCommonFormats,             "H264FindCommonFormats");    
_LIT(KCmdH264GetHeaderInformation,          "H264GetHeaderInformation");
_LIT(KCmdH264EnableSynchronization,         "H264EnableSynchronization");
_LIT(KCmdH264SetScaleOptions,               "H264SetScaleOptions");
_LIT(KCmdH264SetRotateOptions,              "H264SetRotateOptions");
_LIT(KCmdH264SynchronizeDecoding,           "H264SynchronizeDecoding");
_LIT(KCmdH264Delete,                        "H264Delete");
_LIT(KCmdH264SetPosition,                   "H264SetPosition");
_LIT(KCmdH264AbortDirectScreenAccess,       "H264AbortDirectScreenAccess");

/*@{*/
_LIT(KNoCodec,								"ENoCodec");
_LIT(KH263,									"EH263");
_LIT(KMPEG4,								"EMPEG4");
_LIT(KH264,									"EH264");
_LIT(KRV9,									"ERV9");
/*@}*/

const CDataWrapperBase::TEnumEntryTable	CT_CMMFDevVideoPlay::iCodecs[] =
	{
	KNoCodec,	ENoCodec,
	KH263,		EH263,
	KMPEG4,		EMPEG4,
	KH264,		EH264,
	KRV9,		ERV9
	};

/*@{*/
//LIT's for display mode
_LIT(KNone,									"ENone");
_LIT(KGray2,								"EGray2");
_LIT(KGray4,								"EGray4");
_LIT(KGray16,								"EGray16");
_LIT(KGray256,								"EGray256");
_LIT(KColor16,								"EColor16");
_LIT(KColor256,								"EColor256");
_LIT(KColor64K,								"EColor64K");
_LIT(KColor16M,								"EColor16M");
_LIT(KRgb,									"ERgb");
_LIT(KColor4K,								"EColor4K");
_LIT(KColor16MU,							"EColor16MU");
_LIT(KColor16MA,							"EColor16MA");
_LIT(KColorLast,							"EColorLast");
/*@}*/

const CDataWrapperBase::TEnumEntryTable	CT_CMMFDevVideoPlay::iDisplayModes[] =
	{
	KNone,		ENone,
	KGray2,		EGray2,
	KGray4,		EGray4,
	KGray16,	EGray16,
	KGray256,	EGray256,
	KColor16,	EColor16,
	KColor256,	EColor256,
	KColor64K,	EColor64K,
	KColor16M,	EColor16M,
	KRgb,		ERgb,
	KColor4K,	EColor4K,
	KColor16MU,	EColor16MU,
	KColor16MA,	EColor16MA,
	KColorLast,	EColorLast,
	KColor64K,	EColor64K,
	};

/*@{*/
//LIT's for rotation
_LIT(KRotateNone,							"ERotateNone");
_LIT(KRotate90Clockwise,					"ERotate90Clockwise");
_LIT(KRotate90Anticlockwise,				"ERotate90Anticlockwise");
_LIT(KRotate180,							"ERotate180");
/*@}*/
const CDataWrapperBase::TEnumEntryTable	CT_CMMFDevVideoPlay::iRotations[] =
	{
	KRotateNone,			ERotateNone,
	KRotate90Clockwise,		ERotate90Clockwise,
	KRotate90Anticlockwise,	ERotate90Anticlockwise,
	KRotate180,				ERotate180
	};

/*@{*/
//LIT's for Video Data Unit Type
_LIT(KEDuCodedPicture,						"EDuCodedPicture");
_LIT(KEDuVideoSegment,						"EDuVideoSegment");
_LIT(KEDuSeveralSegments,					"EDuSeveralSegments");
_LIT(KEDuArbitraryStreamSection,			"EDuArbitraryStreamSection");
/*@}*/

const CDataWrapperBase::TEnumEntryTable	CT_CMMFDevVideoPlay::iTVideoDataUnitType[] =
	{
	KEDuCodedPicture,				EDuCodedPicture,
	KEDuVideoSegment,				EDuVideoSegment,
	KEDuSeveralSegments,			EDuSeveralSegments,
	KEDuArbitraryStreamSection,		EDuArbitraryStreamSection 
	};

/*@{*/
//LIT's for video data unit encapsulation
_LIT(KEDuElementaryStream,					"EDuElementaryStream");
_LIT(KEDuGenericPayload,					"EDuGenericPayload");
_LIT(KEDuRtpPayload,						"EDuRtpPayload");
/*@}*/
const CDataWrapperBase::TEnumEntryTable	CT_CMMFDevVideoPlay::iTVideoDataUnitEncapsulation[] =
	{
	KEDuElementaryStream,	EDuElementaryStream,
	KEDuGenericPayload,		EDuGenericPayload,
	KEDuRtpPayload,			EDuRtpPayload
	};

//H264
// -----------------------------------------------------------------------------
// class CEngineObserver
// -----------------------------------------------------------------------------
//
CEngineObserver::CEngineObserver(CT_CMMFDevVideoPlay& aTestClass) :
  iTestClass(aTestClass)
    {
    
    }
void CEngineObserver::MvdeStreamEndReached()
    {
    iTestClass.DecOutstanding();
    }
void CEngineObserver::MvdeSetError(TInt aError)
    {
    iTestClass.SetError(aError);
    }

/**
 * Public destructor
 */
CT_CMMFDevVideoPlay::~CT_CMMFDevVideoPlay()
	{
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay Destructor In"));
	if(!iUse264)
	    {
	    if(iDsaStarted)
	        {
	        iDevvp->AbortDirectScreenAccess();      
	        iDsaStarted = EFalse;
	        }
	    DestroyData();
	    }

    iUse264 = EFalse;
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay Destructor Out"));
	}

/**
 * Return a pointer to the object that the data wraps
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CMMFDevVideoPlay::GetObject()
	{
	return iDevvp;
	}

/**
 * Command that destroy the wrapped object and helper objects
 */ 
void CT_CMMFDevVideoPlay::DoCmdDestructor()
	{
	DestroyData();	
	if ( iFileOpen )
		{
		CloseFile();
		iFileOpen = EFalse;
		}
	if ( iDsaStarted )
		{
		iDevvp->AbortDirectScreenAccess();      
		iDsaStarted = EFalse;
		}      		
	//Postproc input buffer
	if ( !iCIBuffMgmtOn && iRawInBuffer)
		{ 
		delete (TUint8*)iRawInBuffer->iData.iRawData->Ptr();
		delete iRawInBuffer->iData.iRawData;
		delete iRawInBuffer;
		iRawInBuffer = NULL;
		}    	
	if(iScreenDevice)
		{
		delete iScreenDevice;
		iScreenDevice = NULL;
		RFbsSession::Disconnect();
		}		
	if(iClock)
		{
		delete iClock;
		iClock = NULL;
		}		
	iFrameSizeList.Close();
	iFs.Close();
	REComSession::FinalClose(); 	
	if(iInfo)
		{
		delete iInfo;
		iInfo=NULL;
		}	
	while(Outstanding())
		{
		INFO_PRINTF1(_L("DoCmdDestructor DecOutstanding()"));
		DecOutstanding();
		}
	}

/**
 * Helper method tha destroy the wrapped object
 */
void CT_CMMFDevVideoPlay::DestroyData()
{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DestroyData")); 
	// Delete Devvideo play 
	delete iDevvp;
	iDevvp = NULL;
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DestroyData"));
}

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CMMFDevVideoPlay* CT_CMMFDevVideoPlay::NewL()
	{	
	CT_CMMFDevVideoPlay*	ret=new (ELeave) CT_CMMFDevVideoPlay();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;		
	}

/**
 * Protected constructor. First phase construction
 */
CT_CMMFDevVideoPlay::CT_CMMFDevVideoPlay()
	:
	iDevvp(NULL),
	iDecHWDevId(0),
	iPostProcId(0),
	iInBuffSize(0),
	iInputEnd(EFalse),
	iDsaStarted(EFalse),
	iSynchronized(EFalse),
	iFrameListed(EFalse),
	iPictureSize(0,0),
	iAsyncErrorIndex(0),
	iFileOpen(EFalse),
	iCodedInBuffer(NULL),
	iOutBuffer(NULL),
	iRawInBuffer(NULL),
	iTimeStamp(0),
	iFrameTimeInterval(0),
	iSentBuffCount(0),
	iReturnedBuffCount(0),
	iPictureLoss(0),
	iStreamEnd(EFalse),
	iCodecType(EH263),
	iClock(NULL),
	iDirectScreenAccess(EFalse),
	iCIBuffMgmtOn(EFalse),
	iFrameMeasurement(EFalse),
	iCommonFormatFound(EFalse),
	iErrorInputBuffer(EFalse),
	iEngine(NULL),
	iObserver(NULL),
	iUse264(EFalse),
	iFrameRate(15)
	{
	}

/**
 * Second phase construction
 * @internalComponent
 * @return	N/A
 * @pre		None
 * @post	None * @leave	system wide error
 */
void CT_CMMFDevVideoPlay::ConstructL()
	{		
    iClock = CSystemClockSource::NewL();
	}

/**
 * Process a command read from the ini file
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to 
 * @return					ETrue if the command is processed *
 * @leave					System wide error
 */
TBool CT_CMMFDevVideoPlay::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection,
											 const TInt aAsyncErrorIndex)
	{	
	TBool	ret=ETrue;

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdInitialize )
		{
		DoCmdInitialize(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdStart )
		{
		DoCmdStart(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdStop )
		{
		DoCmdStop();
		}
	else if ( aCommand==KCmdSetPosition )
		{
		DoCmdSetPosition(aSection);
		}
	else if ( aCommand==KCmdPause )
		{
		DoCmdPause();
		}
	else if ( aCommand==KCmdResume )
		{
		DoCmdResume();
		}
	else if ( aCommand==KCmdSetRotateOptionsL )
		{
		DoCmdSetRotateOptionsL(aSection);
		}
	else if ( aCommand==KCmdSetScaleOptionsL )
		{
		DoCmdSetScaleOptionsL(aSection);
		}
	else if ( aCommand==KCmdSetClockSource )
		{
		DoCmdSetClockSource(aSection);
		}
	else if ( aCommand==KCmdSelectDecoderL )
		{
		DoCmdSelectDecoderL(aSection);
		}
	else if ( aCommand==KCmdSelectPostProcessorL )
		{
		DoCmdSelectPostProcessorL(aSection);
		}
	else if ( aCommand==KCmdPostProcessorInfoLC )
		{
		DoCmdPostProcessorInfoLC();
		}
	else if ( aCommand==KCmdGetOutputFormatListL )
		{
		DoCmdGetOutputFormatListL(aSection);
		}	
	else if ( aCommand==KCmdFindCommonFormat )
		{
		DoCmdFindCommonFormat();
		}
	else if ( aCommand==KCmdSetOutputFormatL )
		{
		DoCmdSetOutputFormatL(aSection);
		}
	else if ( aCommand==KCmdGetHeaderInformationL )
		{
		DoCmdGetHeaderInformationL(aSection);
		}	
	else if ( aCommand==KCmdReturnHeader )
		{
		DoCmdReturnHeader();
		}	
	else if ( aCommand==KCmdSetVideoDestScreenL )
		{
		DoCmdSetVideoDestScreenL();
		}
	else if ( aCommand==KCmdGetBufferOptions )
		{
		DoCmdGetBufferOptions();
		}
	else if ( aCommand==KCmdSetBufferOptionsL )
		{
		DoCmdSetBufferOptionsL();
		}	
	else if ( aCommand==KCmdSetPostProcessTypesL )
		{
		DoCmdSetPostProcessTypesL(aSection);
		}
	else if ( aCommand==KCmdSetInputFormat )
		{
		DoCmdSetInputFormatL(aSection);
		}
	else if ( aCommand==KCmdStartDirectScreenAccess )
		{
		DoCmdStartDirectScreenAccessL(aSection);
		}
	else if ( aCommand==KCmdAbortDirectScreenAccess )
		{
		DoCmdAbortDirectScreenAccess();
		}
	else if ( aCommand==KCmdUtilityGetEnvironment )
		{
		DoCmdUtilityGetEnvironmentL();
		}
	else if ( aCommand==KCmdH264CreateDecTest)
		{
		DoCmdH264CreateDecTest(aSection);
		}
	else if (aCommand==KCmdH264SelectPostProcessor)
	    {
	    DoCmdH264SelectPostProcessor(aSection);
	    }
	else if (aCommand==KCmdH264SetPostProcessTypes)
	    {
	    DoCmdH264SetPostProcessTypes(aSection);
	    }
	else if (aCommand==KCmdH264SetInputFormatCompr)
		{
		DoCmdH264SetInputFormatCompr(aSection);
		}
	else if (aCommand==KCmdH264SetOutputFormat)
		{
		DoCmdH264SetOutputFormat(aSection);
		}
	else if (aCommand==KCmdH264SetBufferOptions)
		{
		DoCmdH264SetBufferOptions(aSection);
		}
	else if (aCommand==KCmdH264ListFrameSize)
	    {
	    DoCmdH264ListFrameSize();
	    }
	else if (aCommand==KCmdH264SetVideoDestScreen)
	    {
	    DoCmdH264SetVideoDestScreen(aSection);
	    }
	else if (aCommand==KCmdH264SetWindowRect)
	    {
	    DoCmdH264SetWindowRect(aSection);
	    }
	else if (aCommand==KCmdH264Initialize)
		{
		DoCmdH264Initialize();
		}
	else if (aCommand==KCmdH264Start)
		{
		DoCmdH264Start(aSection);
		}
	else if (aCommand==KCmdH264IOFrameMatch)
		{
		DoCmdH264IOFrameNumMatch();
		}
	else if (aCommand==KCmdH264Stop)
		{
		DoCmdH264Stop();
		}
	else if (aCommand==KCmdH264GetHeaderInformation)
	    {
	    DoCmdH264GetHeaderInformation(aSection);
	    }
	else if (aCommand==KCmdH264FindCommonFormats)
	    {
	    DoCmdH264FindCommonFormats();
	    }
	else if (aCommand==KCmdH264EnableSynchronization)
	    {
	    DoCmdH264EnableSynchronization(aSection);
	    }
	else if (aCommand==KCmdH264SetScaleOptions)
	    {
	    DoCmdH264SetScaleOptions(aSection);
	    }
	else if (aCommand==KCmdH264SetRotateOptions)
	    {
	    DoCmdH264SetRotateOptions(aSection);
	    }
	else if (aCommand==KCmdH264SynchronizeDecoding)
	    {
	    DoCmdH264SynchronizeDecoding(aSection);
	    }
	else if (aCommand==KCmdH264Delete)
	    {
	    DoCmdH264Delete();
	    }
    else if (aCommand==KCmdH264Pause)
        {
        DoCmdH264Pause();
        }
    else if (aCommand==KCmdH264Resume)
        {
        DoCmdH264Resume();
        }
    else if (aCommand==KCmdH264SetPosition)
        {
        DoCmdH264SetPosition(aSection);
        }
	else if (aCommand==KCmdH264AbortDirectScreenAccess)
		{
		DoCmdH264AbortDirectScreenAccess();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}
	return ret;
	}

/**
 * Constructs CMMFDevVideoPlay instance and initialize params.
 * @param	aSection	- The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdNewL"));
	TBool dataOk = ETrue;
	DestroyData();	
	TRAPD(err, iDevvp = CMMFDevVideoPlay::NewL(*this));
	if ( err!=KErrNone)
		{
		ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdNewL failed with error %d"), err);
		SetError (err);
		dataOk = EFalse;
		}	
	if(dataOk)
		{
		TRAP(err, InitializeParamsL(aSection));	
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdNewL failed in InitializeParamsL. Error: %d"), err);
			SetError (err);
			}
		CreateFiles(aSection);	
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdNewL"));
	}  

/**
 * Open File session
 */ 
void CT_CMMFDevVideoPlay::CreateFiles(const TTEFSectionName& aSection)
	{     
	TPtrC inputPath;
	TPtrC outputPath;
	TPtrC inputfile;
	TPtrC outputfile;
	TBool screenout = EFalse;
	TInt error = KErrNone;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::CreateFiles"));
	
	if ( !(GetBoolFromConfig(aSection, KScreenout(), screenout)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KScreenout());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if ( !(GetStringFromConfig(aSection, KInputfile(),inputfile)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputfile());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if ( !screenout)
		{
		if ( !(GetStringFromConfig(aSection, KOutputfile(),outputfile)))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputfile());
			SetBlockResult(EFail);
			dataOk = EFalse;
			}
		} 
	if ( !(GetStringFromConfig(aSection, KInputPath(),inputPath)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputPath());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}			
	if(dataOk)
		{		
		error = iFs.Connect ();
		if ( error!=KErrNone)
			{
			ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdNewL failed with error %d"), error);
			SetError (error);
			}
		else
			{
			TFileName inFileName(inputPath);
			inFileName.Append (inputfile);
			//Open File
			if ( screenout)
				{
				TRAPD(err, OpenFileL(inFileName));
				if ( err != KErrNone)
					{
					ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdNewL failed with error %d"), err);
					SetError (err);
					}
				iFileOpen = ETrue;
				INFO_PRINTF1(_L("File Opened!"));
				}
			else
				{
				if ( ! (GetStringFromConfig(aSection, KOutputPath(),outputPath)))
					{
					ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputPath());
					SetBlockResult(EFail);
					}
				TFileName outFileName(outputPath);
				outFileName.Append (outputfile);
				TRAPD(err, OpenFileL(outFileName, inFileName));
				if ( err != KErrNone)
					{
					ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::OpenFileL failed with error %d"), err);
					SetError (err);
					}
				iFileOpen = ETrue;
				}
			}												
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::CreateFiles"));
	}

/**
 * Initilize Engine parameters 
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::InitializeParamsL(const TTEFSectionName& aSection)
	{
	TInt codec = 0;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::InitializeParamsL"));
	if ( !(GetEnumFromConfig(aSection, KCodec(), iCodecs, codec)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCodec());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}	
	if ( !(GetBoolFromConfig(aSection, KScreenout(), iDirectScreenAccess)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KScreenout());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if(dataOk)
		{
		iCodecType=(TVideoCodec)codec;
		if (iDirectScreenAccess)
			{
			User::LeaveIfError(FbsStartup());
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::InitializeParamsL"));
	}

/**
 * Sets the clock source in case the sync parameter is true
 * @param	aSection	- The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSetClockSource(const TTEFSectionName& aSection)
	{
	TInt framerate = 0;
	TInt synch = 0;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetClockSource"));	
	if ( !( GetIntFromConfig(aSection, KFramerate(), framerate)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFramerate());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
	if ( !( GetIntFromConfig(aSection,KSynch(), synch)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSynch());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
	if(dataOk)
		{
		if (synch) //Set Clock and frame rate for syncronization    
	       	{    
	       	TRAPD(err, ListFrameSizeL(iCodecType));
	    	if ( err != KErrNone)
	    		{
	    		ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::ListFrameSizeL failed with error %d"), err);
	    		SetError (err);
	    		}
	    	else
	    		{
	    		INFO_PRINTF1(_L("List Frame done!"));    	
		       	iDevvp->SetClockSource(iClock);  
		        iSynchronized = ETrue;
		        iFrameTimeInterval = (TInt64)(1000000/framerate);
		        iFramerate = framerate;
	    		}	    	
	       	}
		}		
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetClockSource"));
	}

/**
 * Gets the decoders list and Select a Decoder
 * @param	aSection	-	Section to read from the ini file
 * @return none
 */
void CT_CMMFDevVideoPlay::DoCmdSelectDecoderL(const TTEFSectionName& aSection)
	{
	TInt decoder;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSelectDecoder"));
	if ( !(GetHexFromConfig(aSection, KDecoder(), decoder)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDecoder());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iSelectDecoder=TUid::Uid(decoder);
		TRAPD(err, iDecHWDevId = iDevvp->SelectDecoderL(iSelectDecoder));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SelectDecoderL left with error: %d"), err);
			SetError(err);
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSelectDecoder"));
	}

/**
 * Gets the postprocessores list and Select a PostProcessor
 * @param	aSection	-	Section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSelectPostProcessorL(const TTEFSectionName& aSection)
	{
	TInt postPro;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSelectPostProcessor"));
	if ( !(GetHexFromConfig(aSection, KPostPro(), postPro)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDecoder());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iSelectPostProcessor=TUid::Uid(postPro);
		TRAPD(err, iPostProcId = iDevvp->SelectPostProcessorL(iSelectPostProcessor));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SelectPostProcessorL left with error %d"), err);
	        SetError(err);
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSelectPostProcessor"));
	}

/**
 * Sets the input format for the decoder and the postprocessor
 * @param	aSection	-	Section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSetInputFormatL(const TTEFSectionName& aSection)
	{
	TBuf8<50> mimetype;
	TBool useDecoder = ETrue;
	TInt tVideoDataUnitType = EDuCodedPicture;
	TInt tVideoDataUnitEncapsulation = EDuElementaryStream;
	TBool dataInOrder=ETrue;	
	TPtrC mimeType;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetInputFormat"));
	if(!GetStringFromConfig(aSection, KMimeType(), mimeType))
		{
		ERR_PRINTF1(_L("Mime type not retrieved, command will not be executed"));
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if ( !(GetBoolFromConfig(aSection, KUseDecoder(), useDecoder)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUseDecoder());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
		
	if( !(GetBoolFromConfig(aSection, KDataInOrder(), dataInOrder)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file, using default value"),&KDataInOrder());
		}			
	if(! (GetEnumFromConfig(aSection, KTVideoDataUnitType(), iTVideoDataUnitType,tVideoDataUnitType)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file, using default value"),&KTVideoDataUnitType());
		}
	if( !(GetEnumFromConfig(aSection, KTVideoDataUnitEncapsulation(), iTVideoDataUnitEncapsulation,tVideoDataUnitEncapsulation)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file, using default value"),&KTVideoDataUnitEncapsulation());
		}	
	if(dataOk)
		{
		mimetype.Append(mimeType);
		if ( useDecoder) //Decoder
			{
			CCompressedVideoFormat* compressedFormat=NULL;
			TRAPD(error, compressedFormat = CCompressedVideoFormat::NewL (mimetype));
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("Error in CCompressedVideoFormat::NewL: %d"), error);
				SetError (error);
				}
			else
				{
				CleanupStack::PushL (compressedFormat);
				TRAP(error,
						iDevvp->SetInputFormatL(iDecHWDevId, *compressedFormat, 
									(TVideoDataUnitType)tVideoDataUnitType, 
									(TVideoDataUnitEncapsulation)tVideoDataUnitEncapsulation,
									dataInOrder));
				CleanupStack::PopAndDestroy (compressedFormat);
				if ( error != KErrNone)
					{
					ERR_PRINTF2(_L("Error in SetInputFormatL for decoder: %d"), error);
					SetError (error);
					}
				else
					{
					INFO_PRINTF1(_L("SetInputFormatL done!"));			
					}
				}
			
			}
		else // PostProcessor
			{
			TRAPD(err, iDevvp->SetInputFormatL(iPostProcId, iCommonFormat));
			if ( err != KErrNone)
				{
				ERR_PRINTF2(_L("Error in SetInputFormatL for PostProcessor: %d"), err);
				SetError (err);
				}
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetInputFormat"));
	}

/**
 * Gets the output format list from the decoder and the postprocessor
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdGetOutputFormatListL(const TTEFSectionName& aSection)
	{
	TBool useDecoder = ETrue; //ETrue - Decoder , EFalse - PostProcessor
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdGetOutputFormatListL"));
	if ( !(GetBoolFromConfig(aSection, KUseDecoder(), useDecoder)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUseDecoder());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		if ( useDecoder) //Decoder
			{
			TRAPD(err, iDevvp->GetOutputFormatListL(iDecHWDevId, iDecFormats));
			if ( err != KErrNone)
				{
				ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdGetOutputFormatListL failed with error %d"), err);
				SetError (err);
				}
			}
		else //PostProcessor
			{
			TRAPD(err1, iDevvp->GetOutputFormatListL(iPostProcId, iPPFormats));
			if ( err1 != KErrNone)
				{
				ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdGetOutputFormatListL left with error %d"), err1);
				SetError (err1);
				}
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdGetOutputFormatListL"));
	}

/**
 * Gets the postprocessor info.
 */
void CT_CMMFDevVideoPlay::DoCmdPostProcessorInfoLC()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdPostProcessorInfo"));
	delete iInfo;
	iInfo=NULL;
	TRAPD(err, {iInfo = iDevvp->PostProcessorInfoLC(iSelectPostProcessor); CleanupStack::Pop(iInfo);} );
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("PostProcessorInfoLC failed with error %i"),err);
		}
	else
		{
		iPPFormats = iInfo->SupportedFormats();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdPostProcessorInfo"));
	}

/**
 * Find common format for dec output and postproc input. 
 * Precondition: The command GetOutputList has to be called before, for the Decoder and the postprocessor,
 * because in this method we use both list to find the common format.
 */
void CT_CMMFDevVideoPlay::DoCmdFindCommonFormat()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdFindCommonFormat"));
	if ( iDevvp->FindCommonFormat (iDecFormats.Array (), iPPFormats.Array (), iCommonFormat) )
		{
		INFO_PRINTF1(_L("DoCmdFindCommonFormat found common Format!"));
		}
	else
		{
		iCommonFormat = iDecFormats[0];
		ERR_PRINTF1(_L("Error in CT_CMMFDevVideoPlay::DoCmdFindCommonFormat: Common Format not found."));
		SetBlockResult(EFail);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdFindCommonFormat"));
	}

/**
 * Sets the output format for both the decoder and the postprocessor.
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSetOutputFormatL(const TTEFSectionName& aSection)
	{
	TBool useDecoder = ETrue;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetOutputFormat"));
	if ( !(GetBoolFromConfig(aSection, KUseDecoder(), useDecoder)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUseDecoder());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		if ( useDecoder)
			{ //Decoder			
			TRAPD(error, iDevvp->SetOutputFormatL (iDecHWDevId, iCommonFormat));
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("SetOutputFormatL left with error %d"), error);
				SetError(error);
				}
			}
		else
			{ //PostProcessor
			if ( iDirectScreenAccess)
				{
				//Set Output Fomat
				TUncompressedVideoFormat outFormat;
				outFormat.iDataFormat = ERgbRawData;
				outFormat.iRgbFormat = ERgb32bit888; //RGB format for display         
				TRAPD(error, iDevvp->SetOutputFormatL (iPostProcId, outFormat));
				if(error != KErrNone)
					{
					ERR_PRINTF2(_L("SetOutputFormatL left with error %d"), error);
					SetError(error);
					}

				}
			else // Memory buffer output
				{
				TRAPD(error, iDevvp->SetOutputFormatL (iPostProcId, iCommonFormat));
				if(error != KErrNone)
					{
					ERR_PRINTF2(_L("SetOutputFormatL left with error %d"), error);
					SetError(error);
					}
				}
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetOutputFormat"));
	}


/**
 * Gets the header information.
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdGetHeaderInformationL(const TTEFSectionName& aSection)
	{
	TInt bufSize = 0;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdGetHeaderInformationL"));
	if ( !(GetIntFromConfig(aSection, KBuf(), bufSize)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBuf());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}		
	if(dataOk)
		{
		INFO_PRINTF1(_L("GetHeaderInformationL init..!"));
		TInt err = KErrNone;
		iHeaderInfo = NULL;
		HBufC8* tempBuff=  NULL;
		//Read Picture header : Size is not known
		TVideoInputBuffer* codedBuffer = new(ELeave) TVideoInputBuffer;
		CleanupStack::PushL ( codedBuffer);
		if ( iFrameListed) //frame size known 
			{
			tempBuff = HBufC8::NewLC(iInBuffSize);
			codedBuffer->iData.Set (tempBuff->Des ());
			if(iFrameSizeList.Count() > 0)
				{
				err = ReadOneCodedPicture (codedBuffer, iFrameSizeList[0]);
				}
			else
				{
				err = KErrGeneral;
				}
			
			TInt pos =0;
			if ( iInFile.Seek (ESeekStart, pos))
				{
				err = KErrGeneral;
				}
			if ( err < 0)
				{
				CleanupStack::PopAndDestroy (tempBuff);
				}
			}
		else //frame size not known 
			{
			iInBuffSize = KH263MaxCodedSizeQCIF;
			err = KErrOverflow;
			while ( (err == KErrOverflow) && (iInBuffSize <= KMP4MaxCodedSizeVGA))
				{
				tempBuff = HBufC8::NewLC(iInBuffSize);
				codedBuffer->iData.Set (tempBuff->Des ());
				err = ReadOneCodedPicture (codedBuffer);
				TInt pos =0;
				if ( iInFile.Seek (ESeekStart, pos))
					{
					err = KErrGeneral;
					}
				if ( iInputEnd)
					{
					err = KErrNotFound;
					}
				if ( err < 0)
					{
					CleanupStack::PopAndDestroy (tempBuff);
					iInBuffSize = 4*iInBuffSize;
					}
				}
			}
		// Reitrieve header information from bitstream 
		if ( err < 0)
			{
			CleanupStack::PopAndDestroy (codedBuffer);
			}
		else
			{
			TRAPD(error, iHeaderInfo = iDevvp->GetHeaderInformationL(EDuCodedPicture, EDuElementaryStream,codedBuffer));
			CleanupStack::PopAndDestroy (tempBuff);
			CleanupStack::PopAndDestroy (codedBuffer);		
			if ( error != KErrNone)
				{
				ERR_PRINTF2(_L("Error in DoCmdGetHeaderInformationL: %d"), error);
				SetError(error);				
				}
			else
				{
				//Check Header info 
				if ( iHeaderInfo)
					{
					//Size in Memory
					iPictureSize = iHeaderInfo->iDisplayedRect.Size ();
					// Display rect  
					TSize dispsize = iHeaderInfo->iDisplayedRect.Size ();
					iDispRect.SetSize (dispsize);
					INFO_PRINTF3(_L("Size from Header, Height: %d Width: %d"),dispsize.iHeight,
							dispsize.iWidth);
					INFO_PRINTF3(_L("Size from iPictureSize, Height: %d Width: %d"),iPictureSize.iHeight,
							iPictureSize.iWidth);
					}
				// Set size of decoder input buffer
				if ( !iFrameListed)
					{
					iInBuffSize = bufSize;
					}
				}				
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdGetHeaderInformationL"));
	}

/**
 * Returns  the HeaderInformation of the video
 */
void CT_CMMFDevVideoPlay::DoCmdReturnHeader()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdReturnHeader"));
	if (iHeaderInfo)
		{
		iDevvp->ReturnHeader(iHeaderInfo);
		}
	else
		{
		ERR_PRINTF1(_L("Error in DoCmdReturnHeader!"));
  		SetBlockResult(EFail);
		}
		INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdReturnHeader"));
	}

/**
 * Sets output on screen or memory buffer
 */
void CT_CMMFDevVideoPlay::DoCmdSetVideoDestScreenL()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetVideoDestScreenL"));
	TRAPD(err, iDevvp->SetVideoDestScreenL(iDirectScreenAccess));
	if (err != KErrNone)
  		{
  		ERR_PRINTF2(_L("Error in DoCmdSetVideoDestScreenL: %d"), err);
  		SetError(err);
  		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetVideoDestScreenL"));
	}
    
/**
 * Gets the buffer options.
 */
void CT_CMMFDevVideoPlay::DoCmdGetBufferOptions()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdGetBufferOptions"));
	iDevvp->GetBufferOptions(iBufferOptions);	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdGetBufferOptions"));
	}

/**
 * Sets the buffer options for the DevVideoPlay
 */
void CT_CMMFDevVideoPlay::DoCmdSetBufferOptionsL()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetBufferOptionsL"));
	iBufferOptions.iMaxInputBufferSize = iInBuffSize;
	//iBufferOptions.iMaxPostDecodeBufferSize = 0;	
	iBufferOptions.iPreDecodeBufferSize = 0;
	iBufferOptions.iMinNumInputBuffers = KNumOfInputBuffers;
	TRAPD(err, iDevvp->SetBufferOptionsL(iBufferOptions));
	if ( err != KErrNone)
		{
		ERR_PRINTF2(_L("Error in DoCmdSetBufferOptionsL: %d"), err);
		SetError (err);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetBufferOptionsL"));
	}

/**
 * Sets the options for the postprocessor
 */
void CT_CMMFDevVideoPlay::DoCmdSetPostProcessTypesL(const TTEFSectionName& aSection)
	{
	TInt combination = 0;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetPostProcessTypesL"));
	if ( ! (GetHexFromConfig(aSection, KCombination(), combination)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCombination());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
	if(dataOk)
		{
		TRAPD(err,iDevvp->SetPostProcessTypesL(iPostProcId, combination));
		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdSetPostProcessTypesL left with error %d"), err);
			SetError(err);		
			} 
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetPostProcessTypesL"));
	}

/**
 * Initialize the decoder with the specified parameters.
 * @return - KErrNone if no error
 */ 
void CT_CMMFDevVideoPlay::DoCmdInitialize(const TInt aAsyncErrorIndex)
    {  
    INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdInitialize"));
    iAsyncErrorIndex = aAsyncErrorIndex;    
    iDevvp->Initialize();
    IncOutstanding();	
    INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdInitialize"));
    }

/**
 * Starts video decoding directly to the display frame buffer using Direct Screen Access. Playback to the display can start immediately.
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::DoCmdStart(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdStart"));
    iAsyncErrorIndex = aAsyncErrorIndex;
    // Start DevVideoPlay
    INFO_PRINTF1(_L("Start Decoding Engine.."));
   	iDevvp->Start();    	
   	//Reset Clock at Stream Start
   	if ( iSynchronized ) 
   	    { 	
   	    TTimeIntervalMicroSeconds offset(KStartOffset);
   	    iClock->Reset(offset);  
   	    }
   	INFO_PRINTF1(_L("Start Data transfer."));
   	TRAPD(err, HandleNewBufferL()); 
   	if (err != KErrNone)
   	    {
   	    ERR_PRINTF2(_L("HandleNewBufferL()left with error %d"),err);
   	    SetError(err);
   	    }
   	else
   	   {
   	    if(!iErrorInputBuffer)
   	    	{
   	    	IncOutstanding();
   		    INFO_PRINTF1(_L("Start..done!"));     
   		    INFO_PRINTF2(_L("iFrameSizeList.Count %d"),iFrameSizeList.Count());  
   	    	} 	       
   	   }         	   
    INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdStart"));
    }

/**
 * Stops the playback
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdStartDirectScreenAccessL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdStartDirectScreenAccessL"));	
	if ( !iScreenDevice)
		{
		delete iScreenDevice;
		iScreenDevice = NULL;
		RFbsSession::Disconnect();
		TDisplayMode displayMode; //0 - Decoder , 1 - PostProcessor
		TInt value = 0;
		if ( !(GetEnumFromConfig(aSection, KDisplayMode(), iDisplayModes, value)))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDisplayMode());
			SetBlockResult(EFail);
			}
		displayMode = (TDisplayMode)value;
		User::LeaveIfError (RFbsSession::Connect());
		
		TRAPD(error, iScreenDevice = CFbsScreenDevice::NewL (KScreenNumber, displayMode));
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Error creating screen device"),error);
			SetError(error);
			}
		}
	
	if ( iDispRect.iBr.iY > KQVGAHeight)
		{
		iDispRect.iTl.iY = 0;
		iDispRect.SetHeight (KQVGAHeight);
		}
	if ( iDispRect.iBr.iX > KQVGAWidth)
		{
		iDispRect.iTl.iX = 0;
		iDispRect.SetWidth (KQVGAWidth);
		}
	
	TRegionFix<1> region(iDispRect);
	//Start DSA

	INFO_PRINTF2(_L("Size window Y: %d"),iDispRect.Height());
	INFO_PRINTF2(_L("Size window X: %d"),iDispRect.Width());

	TRAPD(error,iDevvp->StartDirectScreenAccessL(iDispRect, *iScreenDevice, region));
	if ( error != KErrNone)
		{
		ERR_PRINTF2(_L("StartDirectScreenAccessL error: %d"),error);
		SetError(error);
		}
	else
		{
		iDsaStarted = ETrue;
		}
	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdStartDirectScreenAccessL"));
	}

/** 
 * Stop Devvideoplay 
 */
void CT_CMMFDevVideoPlay::DoCmdStop()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdStop"));
	iDevvp->Stop ();
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdStop"));
	}

/**
 * Aborts Direct Screen Access completely
 * @return None
 */
void CT_CMMFDevVideoPlay::DoCmdAbortDirectScreenAccess()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdAbortDirectScreenAccess"));
	if ( iDirectScreenAccess && iDsaStarted)
		{
		iDevvp->AbortDirectScreenAccess ();
		INFO_PRINTF1(_L("Abort Direct Screen Access Done"));
		iDsaStarted = EFalse;
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdAbortDirectScreenAccess"));
	}

/**
 * Pause the decoding process
 */
void CT_CMMFDevVideoPlay::DoCmdPause()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdPause"));
	if ( iSynchronized)
		{
		iClock->Suspend (); //Stop Clock source
		}
	iDevvp->Pause();
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdPause"));
	}

/**
 * Resume the decodind process
 */
void CT_CMMFDevVideoPlay::DoCmdResume()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdResume"));
	if ( iSynchronized)
		{
		iClock->Resume (); //Restart Clock source
		}
	iDevvp->Resume ();	
	if ( iDecHWDevId)
		{		
		if ( iDevvp->NumFreeBuffers () > 0 )
			{
			INFO_PRINTF1(_L("Restart data transfer."));
	    	TRAPD(err1, HandleNewBufferL()); 
	        if (err1 != KErrNone)
	        	{
	        	ERR_PRINTF2(_L("HandleNewBufferL()left with error %d"),err1);
	        	SetError(err1);
	        	return;
	        	}
			}
		}

	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdResume"));
	}

/**
 * Rotating the display
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSetRotateOptionsL(const TTEFSectionName& aSection)
	{
	TRotationType rotation;
	TInt rotItem;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetRotateOptionsL"));
	if ( !( GetEnumFromConfig(aSection, KRotation(), iRotations, rotItem)))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRotation());
        SetBlockResult(EFail);
        dataOk = EFalse;
		}
	if(dataOk)
		{
		rotation = (TRotationType)rotItem;		
		TRAPD(error,iDevvp->SetRotateOptionsL(iPostProcId, rotation));
		if ( error != KErrNone)
			{
			ERR_PRINTF2(_L("DoCmdSetRotateOptionsL! Error = %d"),error);
			SetError (error);
			}
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetRotateOptionsL"));
	}

/**
 * Setting the screen options
 * @param	aSection	-	The section to read from the ini file
 * @return	none
 */
void CT_CMMFDevVideoPlay::DoCmdSetScaleOptionsL(const TTEFSectionName& aSection)
    {
	TInt width;
	TInt height;
  	TInt x,y;
  	TBool antiAliasFiltering = ETrue;
  	TBool dataOk = ETrue;
  	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetScaleOptionsL"));
	if ( !(GetIntFromConfig(aSection, Kx(), x)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &Kx());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
	if ( !(GetIntFromConfig(aSection, Ky(), y)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &Ky());
        SetBlockResult(EFail);
        dataOk = EFalse;
        } 
	if ( !(GetIntFromConfig(aSection, KWidth(), width)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KWidth());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
	if ( !(GetIntFromConfig(aSection, KHeight(), height)) )
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHeight());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }	
	GetBoolFromConfig(aSection, KAntiAliasFiltering(),antiAliasFiltering);
 	if(dataOk)
 		{
 		TSize size(width, height);	  
 		TRAPD(error, SetWindowRect(x, y,size));
 		if ( error != KErrNone)
 			{
 			ERR_PRINTF2(_L("SetWindowRect! left with Error = %d"),error);
 			SetBlockResult (EFail);
 			}

 		TRAP(error, iDevvp->SetScaleOptionsL(iPostProcId, size, antiAliasFiltering));
 		if ( error != KErrNone)
 			{
 			ERR_PRINTF2(_L("SetScaleOptionsL! left with Error = %d"),error);
 			SetError (error);
 			}
 		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetScaleOptionsL"));
    }

/**
 * Sets the new position for playback and decoding. It is use for forward and backward
 */
void CT_CMMFDevVideoPlay::DoCmdSetPosition(const TTEFSectionName& aSection)
	{
	TInt pos;
	Int64 time = 0;
	TBool dataOk = ETrue;
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdSetPosition"));
	if ( !(GetIntFromConfig(aSection, KPos(), pos)) ) //This parameter indicates seconds to move forward/backward from the current position
		{
	    ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPos());
	    SetBlockResult(EFail);
	    dataOk = EFalse;
		}
	if(dataOk)
		{
		pos = pos*iFramerate;	
		INFO_PRINTF2(_L("pos %i"),pos);
		if((pos > iFrameSizeList.Count()) || ((pos+iSentBuffCount) < 0))
			{
			ERR_PRINTF2(_L("Invalid value, check %S from INI file"), &KPos());
			//SetBlockResult(EFail);
			}
		else
			{
			iSentBuffCount +=pos;
			TTimeIntervalMicroSeconds timeStamp;	
			if ( iSynchronized )
				{
				timeStamp = (TTimeIntervalMicroSeconds)(iSentBuffCount*iFrameTimeInterval);
				time = timeStamp.Int64();
				iClock->Reset(timeStamp);  
				iTimeStamp = time; 		 
				}
			else
				{
				timeStamp = (TTimeIntervalMicroSeconds)(iSentBuffCount);    
				}
			 
			iDevvp->SetPosition(timeStamp);
			 
			TInt temp=0;
			for(TInt i=0; i < iSentBuffCount; i++)
				{
				temp += iFrameSizeList[i];
				}			 
			TInt error = KErrNone;
			error = iInFile.Seek(ESeekStart,temp);
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("Error Seeking in file, check ini for parameter POS: %d"),error);
				SetBlockResult(EFail);
				}
			}		
		}	
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdSetPosition"));
	}

/**
 * Utility command to get the initial parameters
 */
void CT_CMMFDevVideoPlay::DoCmdUtilityGetEnvironmentL()
	{	
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::DoCmdUtilityGetEnvironmentL"));
			
	CPostProcessorInfo *infoPost = NULL;
	RArray< TUid > postProcessors;
	iDevvp->GetPostProcessorListL(postProcessors);
	for(TInt i = 0; i< postProcessors.Count(); i++)
		{
		INFO_PRINTF3(_L("PostProcessors %d = 0x%x"),i,postProcessors[i]);
		TRAPD(err,infoPost = iDevvp->PostProcessorInfoLC(postProcessors[i]);CleanupStack::Pop(infoPost));
		if(err != KErrNone)
			{
			ERR_PRINTF1(_L("Error in getting Devvp->PostProcessorInfoLC(postProcessors[i])"));
			}
		else
			{
			TBool supported = infoPost->SupportsDirectDisplay();
			if (supported)
				{
				INFO_PRINTF1(_L("Direct Display Supported"));
				}
			else
				{
				INFO_PRINTF1(_L("Direct Display Not Supported"));
				}	
			const RArray<TUint32> combinations = infoPost->SupportedCombinations();	  
			for (i = 0; i < combinations.Count(); i++)
				{
				INFO_PRINTF2(_L("Supported Combination : %d"),combinations[i]);
				}
			TUint32 test = 0x3040;	
			if (infoPost->SupportsCombination(test))
				{
				INFO_PRINTF2(_L("Combination %x is Supported"),test);
				}
			else
				{
				INFO_PRINTF2(_L("Combination %x is NOT Supported"),test);
				}
			test = 0x40;//play	
			if (infoPost->SupportsCombination(test))
				{
				INFO_PRINTF2(_L("Combination %x is Supported"),test);
				}
			else
				{
				INFO_PRINTF2(_L("Combination %x is NOT Supported"),test);
				}
			test = 0x44;//rotation
			if (infoPost->SupportsCombination(test))
				{
				INFO_PRINTF2(_L("Combination %x is Supported"),test);
				}
			else
				{
				INFO_PRINTF2(_L("Combination %x is NOT Supported"),test);
				}
			test = 0x48;//scale
			if (infoPost->SupportsCombination(test))
				{
				INFO_PRINTF2(_L("Combination %x is Supported"),test);
				}
			else
				{
				INFO_PRINTF2(_L("Combination %x is NOT Supported"),test);
				}
			TUint32 rotations = infoPost->SupportedRotations();
			if (rotations == 0)
				{
				INFO_PRINTF1(_L("Rotation is Not Supported"));
				}
			else
				{
				INFO_PRINTF2(_L("Supported rotation : %d"),rotations);
				}
			supported = infoPost->SupportsArbitraryScaling();
			if (supported)
				{
				INFO_PRINTF1(_L("Arbitrary Scaling is Supported"));
				}
			else
				{
				RArray<TScaleFactor> scaleFactors = infoPost->SupportedScaleFactors();
				TScaleFactor factor;
				for (i = 0; i < scaleFactors.Count(); i++)
					{
					factor = scaleFactors[i];
					INFO_PRINTF3(_L("Scale factor: scale num - %d scale denom - %d"),factor.iScaleNum,factor.iScaleDenom);
					}		
				}
			}
		
		}
	
  			
	RArray<TUid> decoders;
	iDevvp->GetDecoderListL(decoders);
	CVideoDecoderInfo *infDec =NULL;
	INFO_PRINTF1(_L("INFORMATION ABOUT DECODERS:"));
	for(TInt i = 0; i< decoders.Count(); i++)
		{		
		TRAPD(err, infDec = iDevvp->VideoDecoderInfoLC(decoders[i]);CleanupStack::Pop(infDec));		
		if(err != KErrNone )
			{
			 ERR_PRINTF1(_L("Error in iDevvp->VideoDecoderInfoLC(decoders[i])"));
			}
		else
			{
			INFO_PRINTF6(_L("\nDecoder =0x%x\n Identifier = %S\n Accelerated = %i\n Supports Direct Display: %i\n Manufacturer%S"), 
					decoders[i].iUid,
					&infDec->Identifier(),
					infDec->Accelerated(),
					infDec->SupportsDirectDisplay(),
					&infDec->Manufacturer());				
			INFO_PRINTF1(_L("SUPPORTED FORMATS:"));
			TBuf<255> mime;
			const RPointerArray<CCompressedVideoFormat>& outputFormats =infDec->SupportedFormats();
			for (TInt j = 0; j < outputFormats.Count(); j++)
				{
				mime.Copy(outputFormats[j]->MimeType());		
				INFO_PRINTF3(_L("Supported type %d MIMETYPE: %S"),j,&mime);					
				}	
			INFO_PRINTF1(_L("\n\n"));	
			}						
		}
		    
	TInt colors = 0;
	HAL::Get(HALData::EDisplayColors, colors);
	INFO_PRINTF2(_L("Display colors : %d"),colors);
	INFO_PRINTF1(_L("Display mode!"));
	switch(colors)
		{
		case 0:
	 		INFO_PRINTF1(_L("None"));
	 	break;		
		case 16777216:
			INFO_PRINTF1(_L("EColor16MU"));
		break;
		case 65536:
			INFO_PRINTF1(_L("EColor64K"));
		break;
		case 4096:
			INFO_PRINTF1(_L("EColor4K"));
		break;
		default:
			INFO_PRINTF1(_L("Default mode EColor4K"));
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::DoCmdUtilityGetEnvironmentL"));
	}

/**
 * Sets the original window for display
 */
void CT_CMMFDevVideoPlay::SetWindowRect(TInt aX, TInt aY, TSize aSize)
    {
    INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::SetWindowRect"));
    iDispRect.SetRect(TPoint(aX,aY),aSize);
    INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::SetWindowRect"));
    }

/**
 * Read one coded picture into decoder input buffer
 * 
 * @param aCodedInBuffer
 * 
 * @return err
 */
TInt CT_CMMFDevVideoPlay::ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer)
	{
	TInt buffLength = iInBuffSize;
	TBool found = EFalse;
	TInt pos =0;
	TUint8* testData= NULL;
	TInt err = KErrNone;	
	if ( aCodedInBuffer->iData.MaxLength ()< buffLength)
		{
		err = KErrOverflow;
		return err;
		}
	aCodedInBuffer->iData.SetLength (0);
	//Read data from Input File  
	err = iInFile.Read (aCodedInBuffer->iData, buffLength);
	if ( err != KErrNone)
		{
		return err;
		}
	//Buffer is not full,  reset size of buffer   
	if ( aCodedInBuffer->iData.Length ()< buffLength)
		{
		buffLength = aCodedInBuffer->iData.Length ();
		}
	testData = const_cast<TUint8*>(aCodedInBuffer->iData.Ptr());
	//Check the length of one coded picture
	if ( (iCodecType == EH263) || ((testData[0] == 0x00) && (testData[1] == 0x00) & ((testData[2] & 0xFC) == 0x80)))
		{
		if ( (aCodedInBuffer->iData.Length() > 0))
			{
			pos = KScLength; //skip first 3 byte (PSC)   
			// Find next PSC
			while ( pos < (buffLength - KScLength) && !found)
				{
				if ( (testData[pos+1] == 0x00) && (testData[pos+2] == 0x00) & ((testData[pos+3] & 0xFC) == 0x80))
					{
					found = ETrue;
					}
				pos++;
				}
			}
		else
			{
			iInputEnd = ETrue;
			return err;
			}
		}
	else
		if ( iCodecType == EMPEG4)
			{
			if ( (aCodedInBuffer->iData.Length() > 0))
				{
				// Check if start code of Visual Object Sequence              
				pos = 0;
				while ( pos < (buffLength - KScLength) && !found)
					{ // Find 1st VOP start code in the buffer              
					if ( (testData[pos] == 0x00) && (testData[pos+1] == 0x00)&& (testData[pos+2] == 0x01) && (testData[pos+3] == 0xB6))
						{
						while (pos < (buffLength - KScLength-1) && !found)
							{
							pos++;
							// Find 2nd start code in the buffer >> End of a Picture
							if ( (testData[pos+1] == 0x00) && (testData[pos+2] == 0x00)&& (testData[pos+3] == 0x01) && (testData[pos+4] != 0xB1))
								{
								found = ETrue;
								}
							}
						}
					pos++;
					}
				}
			else
				{
				iInputEnd = ETrue;
				return err;
				}
			}
		else
			{
			err = KErrNotSupported;
			}
	// Start and End of a picure are found 
	if ( found)
		{
		//Set Size to length of the header + one picure, and reset position to be read next time
		aCodedInBuffer->iData.SetLength (pos);
		TInt tempLength = pos - buffLength;
		err = iInFile.Seek (ESeekCurrent, tempLength);
		}
	// All data in Buffer are checked and end of data not found 
	else
		{
		if ( pos >= (iInBuffSize-3))
			{
			err = KErrOverflow;
			}
		}
	if ( err == KErrNone)
		{
		err = pos;
		}	
	return err;	
	}          
    
/**
 * Read one coded picture into decoder input buffer, frame size is known 
 * 
 * @param aCodedInBuffer
 * @param aSize				- frame size
 * @return err				- The error
 */
TInt CT_CMMFDevVideoPlay::ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer, TInt aSize)
    {
    TInt err = KErrNone;        
    if(aCodedInBuffer !=NULL)
    	{
    	err = iInFile.Read(aCodedInBuffer->iData, aSize);                
    	}
    else
    	{
    	INFO_PRINTF1(_L("aCodedInBuffer is NULL"));
    	err = KErrNotFound;
    	}
    if (aSize == 0)
    	{
    	iInputEnd = ETrue;
    	}
    return err;
    }


/**
 * Notifies the client that one or more new empty input buffers are available
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoNewBuffers()
    {
	TRAPD(err, HandleNewBufferL()); 
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("HandleNewBufferL()left with error %d"),err);
		SetBlockResult(EFail);    	
		}
    }

/**
 * Returns a used input video picture back to the caller. The picture memory can be re-used or freed. 
 * 
 * @param aPicture	the picture to return
 */
void CT_CMMFDevVideoPlay::MdvpoReturnPicture(TVideoPicture* aPicture)
    {   
    if ( iRawInBuffer == aPicture )
        {        
    	TRAPD(err1, HandleNewBufferL()); 
        if (err1 != KErrNone)
        	{
        	ERR_PRINTF2(_L("HandleNewBufferL()left with error %d"),err1);
        	SetBlockResult(EFail);
        	}
        }    
    }

/**
 * Delivers supplemental information from a coded data unit.
 * 
 * @param aData			The supplemental data. The reference is only valid until the method returns, and thus the data must be processed or copied immediately
 * @param aTimestamp	The presentation timestamp for the picture that the supplemental data is part of
 * @param aPictureId	Picture identifier for the picture. If a picture ID is not available, aPictureId.iIdType is set to ENone
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoSupplementalInformation(const TDesC8& /*aData*/, const TTimeIntervalMicroSeconds& /*aTimestamp*/, const TPictureId& /*aPictureId*/)
    {	  
    }

/**
 * Back channel information from the decoder, indicating a picture loss without specifying the lost picture.
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoPictureLoss()
    {
    iPictureLoss++;      
    }

/**
 * Back channel information from the decoder, indicating a picture loss without specifying the lost picture.
 * 
 * @param aPictures	Picture identifiers for the lost pictures.
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoPictureLoss(const TArray<TPictureId>& aPictures)
    {         
    iPictureLoss += aPictures.Count();     
    }

/**
 * Back channel information from the decoder, indicating the loss of consecutive macroblocks in raster scan order.
 * 
 * @param aFirstMacroblock	The first lost macroblock
 * @param aNumMacroblocks	The number of lost macroblocks that are consecutive in raster-scan order
 * @param aPicture			The picture identifier for the picture where the macroblocks were lost
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
    {  
    }

/**
 * Back channel information from the decoder, indicating a reference picture selection request. 
 * 
 * @param aSelectionData	The	reference picture selection request message. 
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
    {
    }

/**
 * Called when a timed snapshot request has been completed. When this method is called, the snapshot has been taken, and the memory reserved for the picture can be re-used or freed. 
 * 
 * @param aError					An error code, KErrNone if no errors occurred.
 * @param aPictureData				The snapshot picture data
 * @param aPresentationTimestamp	The presentation timestamp for the snapshot picture
 * @param aPictureId				Picture identifier for the picture. If a picture ID is not available, aPictureId.iIdType is set to ENone
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoTimedSnapshotComplete(TInt aError, TPictureData* /*aPictureData*/, 
														const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/,
														const TPictureId& /*aPictureId*/)
    {
    if(aError!= KErrNone)
    	{
    	INFO_PRINTF2(_L("An error has ocurred: %i in CT_CMMFDevVideoPlay::MdvpoTimedSnapshotComplete"),aError);
    	}    
    }    

/**
 * Notifies the client that one or more new output pictures are available. 
 * 
 * @return None
 */    
void CT_CMMFDevVideoPlay::MdvpoNewPictures()
    {    
    GetReturnedPicture();
    }

/**
 * Reports a fatal decoding or playback error to the client.
 * 
 * @param aError	The error code
 * 
 * @return None
 */       
void CT_CMMFDevVideoPlay::MdvpoFatalError(TInt aError)
    {
    INFO_PRINTF1(_L("MdvpoFatalError DecOutstanding"));
    DecOutstanding();
    ERR_PRINTF2(_L("A Fatal error has ocurred: %i"),aError);
    HandleRuntimeError(aError);  
    }

/**
 * Reports that DevVideoPlay initialization has completed. The interface can now be used for video playback. 
 * 
 * @param aError	Initialization error code, KErrNone if no error occurred
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoInitComplete(TInt aError)
	{

	if ( aError == KErrNone)
		{
		INFO_PRINTF1(_L("MdvpoInitComplete OK"));
		//Initilize parameters for Decoding/Playback
		iCodedInBuffer = NULL;
		iOutBuffer =NULL;
		iSentBuffCount = 0;
		iReturnedBuffCount = 0;
		iPictureLoss =0;
		iInputEnd = EFalse;
		iStreamEnd = EFalse;
		iTimeStamp = 0;
		}
	else
		{		
		ERR_PRINTF2(_L("MdvpoInitComplete Error %d"), aError);
		SetAsyncError (iAsyncErrorIndex, aError);
		}
	INFO_PRINTF1(_L("MdvpoInitComplete DecOutstanding"));	
	DecOutstanding ();
	}

/**
 * Reports that the input video stream end has been reached and all pictures have been processed.
 * This method is only called after the client has called InputEnd(). No more output pictures will be available.
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::MdvpoStreamEnd()
    {
	iStreamEnd = ETrue;     
    INFO_PRINTF1(_L("Stream End!"));
    INFO_PRINTF2(_L("iSentBuffCount: %d"),iSentBuffCount);
    INFO_PRINTF2(_L("iReturnedBuffCount: %d"),iReturnedBuffCount);
    INFO_PRINTF2(_L("iPictureLoss: %d"),iPictureLoss);
    INFO_PRINTF1(_L("MdvpoStreamEnd DecOutStanding"));
   	DecOutstanding();
    }


/**
 * Get Next input data 
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::HandleNewBufferL()
    {
	if ( !iInputEnd )
		{
		if ( iDecHWDevId || iCIBuffMgmtOn ) //Retrieve buffer from Hwdevice
			{
			GetInputBuffer();     
			}
		else
			{
			FillAndSendBufferL();   // Postproc input
			}
		}	
    }

/**
 * Retrieve decoder input buffer
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::GetInputBuffer()
    {
    //TBool buffer = EFalse; 	 	 
    if ( iDecHWDevId )
    	{	
    	TRAPD(error, iCodedInBuffer = iDevvp->GetBufferL(iInBuffSize));
    	if (error != KErrNone)
    		{	
    		ERR_PRINTF2(_L("Error GetInputBufferL!. Error %d"),error);
    		iErrorInputBuffer = ETrue;
    		SetError(error);    		
    		}
    	else
    		{
    		if ( iCodedInBuffer )
    	    	{    	    	    	       	    	
    	        TRAPD(error, FillAndSendBufferL());
    	        if (error != KErrNone)
    	     	   {
    	     	   ERR_PRINTF2(_L("Error GetInputBufferL->FillAndSendBufferL!. Error %d"),error);
    	     	   SetBlockResult(EFail);        	   
    	     	   }
    	    	}
    		}    
    	} 
    }
    
    
    
/**
 * Fill data into input buffer and send to devvideoplay 
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::FillAndSendBufferL()
	{
	TInt err = KErrNone;	
	if ( iDecHWDevId)
		{
		if ( iSynchronized)
			{
			TInt size = 0;
			if((iSentBuffCount>=0) && (iSentBuffCount < iFrameSizeList.Count()))
				{
				size = iFrameSizeList[iSentBuffCount];
				}
			else
				{
				ERR_PRINTF2(_L("error %i"),iSentBuffCount);
				}
			
			err = ReadOneCodedPicture (iCodedInBuffer, size); // Read compressed data of one picture from file 
			}
		else
			{
			err = ReadOneCodedPicture (iCodedInBuffer); // Read compressed data of one picture from file 
			}
		}
	else // Postproc input case
		{
		err = ReadRawPicture (); // Read raw data for one picture      
		}
	if ( iInputEnd)
		{
		iDevvp->InputEnd (); // All input Data sent        
		return;
		}
	if ( err >= 0)
		{
		if ( iDecHWDevId)
			{
			iCodedInBuffer->iOptions = TVideoInputBuffer::EDecodingTimestamp;
			if ( iPostProcId)
				{
				iCodedInBuffer->iOptions |= TVideoInputBuffer::EPresentationTimestamp;
				}
			if ( iSynchronized)
				{
				iCodedInBuffer->iDecodingTimestamp = iTimeStamp;
				if ( iPostProcId)
					{
					iCodedInBuffer->iPresentationTimestamp = iTimeStamp;
					}
				iTimeStamp += iFrameTimeInterval;
				}
			else // Clock source is not set
				{
				iCodedInBuffer->iDecodingTimestamp = iSentBuffCount;
				if ( iPostProcId)
					{
					iCodedInBuffer->iPresentationTimestamp = iSentBuffCount;
					}
				}
			if ( iFrameMeasurement)
				{
				iClock->Reset (); // Reset Timer 
				}
			TRAPD(error,iDevvp->WriteCodedDataL(iCodedInBuffer)); // Input Data Sent to Decoder 
			if ( error != KErrNone)
				{
				ERR_PRINTF2(_L("Error Writing data!. Error %d"),error);
				SetError(error);
				return;
				}
			}
		else
			{
			iRawInBuffer->iData.iDataSize.SetSize (iPictureSize.iWidth,
					iPictureSize.iHeight);
			iRawInBuffer->iOptions |= (TVideoPicture::ETimestamp);
			if ( iSynchronized)
				{
				iRawInBuffer->iTimestamp = iTimeStamp;
				iTimeStamp += iFrameTimeInterval;
				}
			else
				{
				iRawInBuffer->iTimestamp = iSentBuffCount;
				}
			TRAPD(error, iDevvp->WritePictureL (iRawInBuffer)); // Input Data Sent to PostProcessor 
			if (error != KErrNone)
				{
				ERR_PRINTF2(_L("Error Writing Picture!. Error %d"),error);
				SetError(error);
				return;
				}
			}
		iSentBuffCount++;
		}
	else
		{
		iInputEnd = ETrue;
		iDevvp->InputEnd ();
		return;
		}	
	} 
   

/**
 * Retrieve output picture
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::GetReturnedPicture()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::GetReturnedPicture"));
	TRAPD(err, iOutBuffer = iDevvp->NextPictureL());
	if ( err != KErrNone)
		{
        HandleRuntimeError(err);
        ERR_PRINTF1(_L("Error en GetReturnedPicture!!"));
        }        
    if ( iOutBuffer )
        {       
        iReturnedBuffCount++;
        SaveAndReturnPicture();
        }
    INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::GetReturnedPicture"));
    }

/**
 * This function finishes the async call depending on the error occured while handling the picture
 * 
 * @param aError Error Value passed
 * @return none
 */
void CT_CMMFDevVideoPlay::HandleRuntimeError(TInt aError)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::HandleRuntimeError"));
	ERR_PRINTF2(_L("HandleRuntimeError! Error = %d"),aError);
	SetError (aError);
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::HandleRuntimeError"));
	}    

/**
 * Store output data into a file
 * 
 * @return None
 */
void CT_CMMFDevVideoPlay::SaveAndReturnPicture()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::SaveAndReturnPicture"));
	TInt err = iOutFile.Write (*(iOutBuffer->iData.iRawData), iOutBuffer->iData.iRawData->Size ());
	iDevvp->ReturnPicture (iOutBuffer);
	iOutBuffer = NULL;
	if ( err != KErrNone)
		{
		HandleRuntimeError (err);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::SaveAndReturnPicture"));
	}

/**
 * List size of each frame of coded bitstream 
 * 
 * @param  aCodec picture 
 * @return none
 */
void CT_CMMFDevVideoPlay::ListFrameSizeL(TVideoCodec aCodec)
	{
	TInt framesize = 0;
	TInt readpos = 0;
	TInt buffsize = KH263MaxCodedSizeQCIF; //buffer size for listing frame size
	iCodecType = aCodec; // this is for ReadOneCodedPicture
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::ListFrameSizeL"));
	//Initial buffer size
	// Allocate buffer 
	TVideoInputBuffer* videoBuffer = new(ELeave) TVideoInputBuffer;
	CleanupStack::PushL (videoBuffer);
	HBufC8* tempBuff = HBufC8::NewLC( buffsize);
	videoBuffer->iData.Set (tempBuff->Des ());
	while (!iInputEnd)
		{
		iInBuffSize = buffsize;
		videoBuffer->iData.SetLength (0);
		framesize = ReadOneCodedPicture (videoBuffer);//        
		if ( framesize >= 0) // Frame size is added to list
			{
			iFrameSizeList.AppendL (framesize);
			readpos += framesize;
			}
		else // resize buffer to bigger one
			{
			if ( buffsize < KMP4MaxCodedSizeVGA)
				{
				CleanupStack::PopAndDestroy(tempBuff);
				buffsize = buffsize + KH263MaxCodedSizeQCIF;
				tempBuff = HBufC8::NewLC( buffsize);
				videoBuffer->iData.Set (tempBuff->Des ());
				iInFile.Seek (ESeekStart, readpos);
				}
			else
				{
				framesize = KErrNotFound;
				break;
				}
			}
		}
	//Reset file postion to start
	TInt pos =0;
	iInFile.Seek (ESeekStart, pos);
	iInputEnd = EFalse;
	// cleanup    
	CleanupStack::PopAndDestroy(2, videoBuffer);
	//Error case
	if ( framesize < 0)
		{
		User::Leave (framesize);
		}
	//Set frame size listed   
	iFrameListed = ETrue;
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::ListFrameSizeL"));
	}

/**
 * Closes the opened files
 */
void CT_CMMFDevVideoPlay::CloseFile()
	{	
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::CloseFile"));
	iInFile.Close ();
	if ( !iDirectScreenAccess)
		{
		iOutFile.Close ();
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::CloseFile"));
	}

/**
 * Opens the input and output file
 * @param	aInFileName		-	The name of input file
 * @param	aOutFileName	-	The name of output file
 * @return	none
 */
void CT_CMMFDevVideoPlay::OpenFileL(TFileName& aOutFileName, TFileName& aInFileName)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::OpenFileL"));
	TInt err = iInFile.Open (iFs, aInFileName, EFileRead | EFileShareReadersOnly);
	User::LeaveIfError (err);
	err = iOutFile.Replace (iFs, aOutFileName,
			EFileShareExclusive|EFileWrite);
	if ( err!= KErrNone)
		{
		iInFile.Close ();
		User::Leave(err);
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::OpenFileL"));
	}

/**
 * Opens the input File
 * @param	aInFileName	-The name of input file
 * @return	none 
 */
void CT_CMMFDevVideoPlay::OpenFileL(TFileName& aInFileName)
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::OpenFileL"));
	User::LeaveIfError(iInFile.Open (iFs, aInFileName, EFileRead | EFileShareReadersOnly));
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::OpenFileL"));
	}

/**
 * Read one raw picture into postprocessor input buffer
 */
TInt CT_CMMFDevVideoPlay::ReadRawPicture()
	{
	INFO_PRINTF1(_L("*START*CT_CMMFDevVideoPlay::ReadRawPicture"));
	iRawInBuffer->iData.iRawData->Set ((TUint8*)iRawInBuffer->iData.iRawData->Ptr(), 0, iInBuffSize);
	TInt err = iInFile.Read (*(iRawInBuffer->iData.iRawData), iInBuffSize);
	if ( !err)
		{
		if ( (iRawInBuffer->iData.iRawData->Length()) == 0)
			{
			iInputEnd = ETrue;
			}
		}
	INFO_PRINTF1(_L("*END*CT_CMMFDevVideoPlay::ReadRawPicture"));
	return err;
	}

void CT_CMMFDevVideoPlay::DoCmdH264CreateDecTest(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::CreateDecTest, In"));
    iUse264 = ETrue;
    TInt uidInt = 0;
    TInt output = 0;
    TInt width = 0;
    TInt height = 0;
    TInt buffMgmt = 0;
    TInt codec = 0;
    TPtrC inputfile;
    TPtrC inputDirectory;
    
    iObserver = new (ELeave) CEngineObserver(*this);
    //Create Decoder test engine     
    iEngine = CVDecTestEngine::NewL(*iObserver);    

    //Read from test case file 
	if(!GetHexFromConfig(aSection, KUid(), uidInt))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUid());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KBuffMgmt(), buffMgmt))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBuffMgmt());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KOutput(), output))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutput());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KWidth(), width))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KWidth());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KHeight(), height))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHeight());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KCodec(), codec))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCodec());
		SetBlockResult(EFail);
		}
	if(!GetStringFromConfig(aSection, KInputPath(), inputDirectory))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputPath());
		SetBlockResult(EFail);
		}
	if(!GetStringFromConfig(aSection, KInputfile(), inputfile))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputfile());
		SetBlockResult(EFail);
		}
  
	//Open File
    TFileName inFileName(inputDirectory);
    inFileName.Append(inputfile);

	TUid decoderUid;
    decoderUid.iUid = uidInt;
    INFO_PRINTF2(_L("Decoder uid is [%x]"), decoderUid.iUid);

	TSize size;
	size.SetSize(width, height);
        
    if (output == EDecodedFile) 
    	{
    	TPtrC outputfile;
    	TPtrC outputDirectory;
		if ( ! (GetStringFromConfig(aSection, KOutputPath(),outputDirectory)))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputPath());
			SetBlockResult(EFail);
			}
		if ( !(GetStringFromConfig(aSection, KOutputfile(),outputfile)))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutputfile());
			SetBlockResult(EFail);
			}
    		
    	//Start sequence    
    	iEngine->SetUpL(size, EFalse, buffMgmt);
    	TFileName outFileName(outputDirectory);
    	outFileName.Append(outputfile);   
    	iEngine->OpenFileL(outFileName, inFileName);
    	}
    else if (output == EScreenOutput) 
    	{
   		iEngine->SetUpL(size, ETrue, buffMgmt);    	   
    	iEngine->OpenFileL(inFileName);    	
    	}               
    
    TRAPD(result, iEngine->SelectDecoderL(decoderUid));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SelectDecoderL left with error %d"), result);
        SetError(result);
        }
    
    TVideoCodec videoCodec;
    if (codec == 1) 
    	{
    	videoCodec = EH263;	
    	}
    else if (codec == 2) 
    	{
    	videoCodec = EMPEG4;	
    	}
    else if (codec == 3) 
    	{
    	videoCodec = EAVC;	
    	}
    else if (codec == 4) 
    	{
    	videoCodec = EVC1;	
    	}	
    else if (codec == 5) 
    	{
    	videoCodec = EFLV;	
    	}	
		else if (codec == 6) 
    	{
    	videoCodec = EVP6;	
    	}
    else 
    	{
    	videoCodec = EMPEG4;	
    	}
    iEngine->SetCodecType(videoCodec);
    
    iFileOpen = ETrue;
   
	INFO_PRINTF1(_L("CT_MMFDevVideoPlayData::CreateDecTest, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetInputFormatCompr(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_MMFDevVideoPlayData::SetInputFormatCompr, In"));
  
    TInt hwdev = 0;
    TInt dataUnitTypeInt = 0;
    TInt encapsulation = 0;
    TInt dataInOrder = 0;
    TPtrC format;
	if(!GetIntFromConfig(aSection, KHwdev(), hwdev))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHwdev());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KDataUnitTypeInt(), dataUnitTypeInt))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataUnitTypeInt());
		SetBlockResult(EFail);
		}
	if(!GetIntFromConfig(aSection, KEncapsulation(), encapsulation))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KEncapsulation());
		SetBlockResult(EFail);
		}
	if(!GetBoolFromConfig(aSection, KDataInOrder(), dataInOrder))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataInOrder());
		SetBlockResult(EFail);
		}
	if(!GetStringFromConfig(aSection, KFormat(), format))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFormat());
		SetBlockResult(EFail);
		}
	
	TVideoDataUnitType dataUnitType;
	switch (dataUnitTypeInt)
		{
		case 1: dataUnitType = EDuCodedPicture;
				break;
		case 2: dataUnitType = EDuVideoSegment;
				break;
		case 3: dataUnitType = EDuSeveralSegments;
				break;			
		case 4: dataUnitType = EDuArbitraryStreamSection;
				break;				
		default:dataUnitType = EDuCodedPicture;			
		}
			
	TVideoDataUnitEncapsulation dataEncapsulation;
	
	switch (encapsulation) 
		{
		case 1: dataEncapsulation = EDuElementaryStream;
				break;
		case 2: dataEncapsulation = EDuGenericPayload;
				break;
		case 3: dataEncapsulation = EDuRtpPayload;
				break;
		default:dataEncapsulation = EDuElementaryStream;				
		}
	
	TBuf8<128> buf;
	buf.Copy(format);
	CCompressedVideoFormat* compressedFormat = CCompressedVideoFormat::NewL(buf);    
			     	
	TRAPD(result, iEngine->SetInputFormatL(hwdev,*compressedFormat, dataUnitType, dataEncapsulation, dataInOrder));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetInputFormatL left with error %d"), result);
        SetError(result);
        }    
    delete compressedFormat;
    
    INFO_PRINTF1(_L("CT_MMFDevVideoPlayData::SetInputFormatCompr, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetOutputFormat(const TTEFSectionName& aSection)
	{
	TInt hwdev = 0;
	TInt dataFormat = 0;
    TInt dataLayout = 0;
    TInt pattern = 0;
    TInt aspectRatioNum = 1;
    TInt aspectRatioDenom = 1;
    TInt coefficients = 0;
        
    //Read from test case file 
    if ( !(GetIntFromConfig(aSection, KHwdev(), hwdev)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHwdev());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KDataFormat(), dataFormat)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataFormat());
        SetBlockResult(EFail);
        }
    
    if ( !(GetIntFromConfig(aSection, KDataLayout(), dataLayout)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataLayout());
        SetBlockResult(EFail);
        }   
    
    TUncompressedVideoFormat unCompressedFormat;
    
    switch (dataFormat)
    	{
    	case 1: 
    	    unCompressedFormat.iDataFormat = ERgbRawData;    				
    		unCompressedFormat.iRgbFormat = ERgb32bit888;
    		break;    				
    	case 2: unCompressedFormat.iDataFormat = ERgbFbsBitmap;
    			switch (dataLayout) 
    				{
    				case 1: unCompressedFormat.iRgbFormat = ERgb16bit444;
    						break;
    				case 2: unCompressedFormat.iRgbFormat = ERgb16bit565;
    						break;
					case 3: unCompressedFormat.iRgbFormat = ERgb32bit888;
    						break;
    				case 4: unCompressedFormat.iRgbFormat = EFbsBitmapColor4K;
    						break;
    				case 5: unCompressedFormat.iRgbFormat = EFbsBitmapColor16M;
    						break;				    							
    				case 6: unCompressedFormat.iRgbFormat = EFbsBitmapColor16MU;
    						break;		
    				default: unCompressedFormat.iRgbFormat = ERgb16bit444;
    				}
    			break;
    	case 3: unCompressedFormat.iDataFormat = EYuvRawData;
                if ( !(GetIntFromConfig(aSection, KPattern(), pattern)))
                    {
                    ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPattern());
                    SetBlockResult(EFail);
                    }
                if ( !(GetIntFromConfig(aSection, KAspectRatioNum(), aspectRatioNum)))
                    {
                    ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAspectRatioNum());
                    SetBlockResult(EFail);
                    }
                if ( !(GetIntFromConfig(aSection, KAspectRatioDenom(), aspectRatioDenom)))
                    {
                    ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAspectRatioDenom());
                    SetBlockResult(EFail);
                    }
                if(!GetHexFromConfig(aSection, KCoefficients(), coefficients))
                    {
                    ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCoefficients());
                    SetBlockResult(EFail);
                    }
    			switch (dataLayout) 
    				{
    				case 1: unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataPlanar;
    						break;
    				case 2: unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataInterleavedLE;
    						break;
					case 3: unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataInterleavedBE;
    						break;
    				case 4: unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataSemiPlanar;
    						break;
    				default:unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataPlanar;    							    					 
    				}
    			switch (pattern) 
    				{
    				case 1: unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma1;
    						break;
    				case 2: unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma2;
    						break;
					case 3: unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma3;
    						break;
    				case 4: unCompressedFormat.iYuvFormat.iPattern = EYuv422Chroma1;
    						break;
    				case 5: unCompressedFormat.iYuvFormat.iPattern = EYuv422Chroma2;
    						break;    							
    				default:unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma2; 
    				}
    			switch (coefficients) 
    				{
    				case 1: unCompressedFormat.iYuvFormat.iCoefficients = EYuvRange0;
    						break;
    				case 2: unCompressedFormat.iYuvFormat.iCoefficients = EYuvRange1;
    						break;
					case 3: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt709;
    						break;
    				case 4: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt709Range0;
    						break;
    				case 5: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt709Range1;
    						break;    							
    				case 6: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt601;
    						break;    							
    				case 7: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt601Range0;
    						break;    							
    				case 8: unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt601Range1;
    						break;    									
    				case 9: unCompressedFormat.iYuvFormat.iCoefficients = ECustomYuvMatrix;
    						break;    							
    				default:unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt709Range0; 
    				}	
    			unCompressedFormat.iYuvFormat.iAspectRatioNum = aspectRatioNum;
    			unCompressedFormat.iYuvFormat.iAspectRatioDenom = aspectRatioDenom;
    			// We don't use conversion
				unCompressedFormat.iYuvFormat.iYuv2RgbMatrix = NULL;
				unCompressedFormat.iYuvFormat.iRgb2YuvMatrix = NULL;

    			break;
    		default:	    unCompressedFormat.iDataFormat = EYuvRawData;
						    unCompressedFormat.iYuvFormat.iDataLayout = EYuvDataPlanar; 
						    unCompressedFormat.iYuvFormat.iPattern = EYuv420Chroma1;
						    unCompressedFormat.iYuvFormat.iAspectRatioNum = 1;
						    unCompressedFormat.iYuvFormat.iAspectRatioDenom = 1;
						    unCompressedFormat.iYuvFormat.iCoefficients = EYuvBt601Range1;
						    
						    // We don't use conversion
						    unCompressedFormat.iYuvFormat.iYuv2RgbMatrix = NULL;
						    unCompressedFormat.iYuvFormat.iRgb2YuvMatrix = NULL;
    	}            
    
    TRAPD(result, iEngine->SetOutputFormatL(hwdev, unCompressedFormat));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetOutputFormatL left with error %d"), result);
        SetError(result);
        }
	}

void CT_CMMFDevVideoPlay::DoCmdH264SetBufferOptions(const TTEFSectionName& aSection)
	{
    TInt preDecodeBufferSize = 0;
	TInt maxPostDecodeBufferSize = 0;
	TInt preDecoderBufferPeriodInt = 0;
	TInt postDecoderBufferPeriodInt = 0;
	TInt maxInputBufferSize = 0;
	TInt minNumInputBuffers = 0;

    if ( !(GetIntFromConfig(aSection, KPreDecodeBufferSize(), preDecodeBufferSize)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPreDecodeBufferSize());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KMaxPostDecodeBufferSize(), maxPostDecodeBufferSize)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaxPostDecodeBufferSize());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KPreDecoderBufferPeriodInt(), preDecoderBufferPeriodInt)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPreDecoderBufferPeriodInt());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KPostDecoderBufferPeriodInt(), postDecoderBufferPeriodInt)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPostDecoderBufferPeriodInt());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KMaxInputBufferSize(), maxInputBufferSize)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMaxInputBufferSize());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KMinNumInputBuffers(), minNumInputBuffers)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMinNumInputBuffers());
        SetBlockResult(EFail);
        }    

    CMMFDevVideoPlay::TBufferOptions bufferOptions;
	bufferOptions.iPreDecodeBufferSize = preDecodeBufferSize;
	bufferOptions.iMaxPostDecodeBufferSize = maxPostDecodeBufferSize;
	bufferOptions.iPreDecoderBufferPeriod = preDecoderBufferPeriodInt;
	bufferOptions.iPostDecoderBufferPeriod = postDecoderBufferPeriodInt;
	bufferOptions.iMaxInputBufferSize = maxInputBufferSize;
	bufferOptions.iMinNumInputBuffers = minNumInputBuffers;

	TRAPD(result, iEngine->SetBufferOptionsL(bufferOptions));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetBufferOptionsL left with error %d"), result);
        SetError(result);
        }
	}

void CT_CMMFDevVideoPlay::DoCmdH264Initialize()
	{
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Initialize, In"));
	iEngine->Initialize();
	IncOutstanding();
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Initialize, Out"));
	}

void  CT_CMMFDevVideoPlay::DoCmdH264Start(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Start, In"));
    TInt inputEnd = 0;
    if ( !(GetIntFromConfig(aSection, KInputEnd(), inputEnd)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KInputEnd());
        SetBlockResult(EFail);
        }
    if (inputEnd != 0)  
    	{
    	iEngine->Start(ETrue);
    	}
    else 
    	{
    	iEngine->Start(EFalse);
    	}
    IncOutstanding();
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Start, Out"));
	}

void CT_CMMFDevVideoPlay::DoCmdH264Stop()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Stop, In"));
    TRAPD(result, iEngine->Stop());
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("Stop left with error %d"), result);
        SetError(result);
        }
    INFO_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdH264Stop, Out result=%d"),result);
    }

void CT_CMMFDevVideoPlay::DoCmdH264Pause()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Pause, In"));
    TInt result = KErrNone;
    result = iEngine->Pause();
    INFO_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdH264Pause, Out result=%d"),result);    
    }

void CT_CMMFDevVideoPlay::DoCmdH264Resume()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Resume, In"));
    TInt result = KErrNone;
    result = iEngine->Resume();
    INFO_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdH264Resume, Out result=%d"),result);    
    }

void CT_CMMFDevVideoPlay::DoCmdH264IOFrameNumMatch()
	{
    iEngine->SetFrameMatch( ETrue );
	}

void CT_CMMFDevVideoPlay::DoCmdH264ListFrameSize()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264ListFrameSize, In"));
    RArray<TInt> framesizes;
    TRAPD(result, iEngine->ListFrameSizeL(framesizes));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("ListFrameSizeL left with error %d"), result);
        SetError(result);
        }
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264ListFrameSize, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetVideoDestScreen(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::SetVideoDestScreen, In"));
   
    TBool dest = 0;
    if(!GetBoolFromConfig(aSection, KDest(), dest))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDest());
        SetBlockResult(EFail);
        }

    TRAPD(result, iEngine->SetVideoDestScreenL(dest));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetVideoDestScreenL left with error %d"), result);
        SetError(result);
        }    
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetVideoDestScreen, Out"));
    } 

void CT_CMMFDevVideoPlay::DoCmdH264SetWindowRect(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetWindowRect, In"));
    
    TInt x;
    TInt y;
    TInt width;
    TInt height;

    if ( !(GetIntFromConfig(aSection, KX(), x)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KX());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KY(), y)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KY());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KWidth(), width)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KWidth());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KHeight(), height)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHeight());
        SetBlockResult(EFail);
        }

    TSize size(width, height);
    
    iEngine->SetWindowRect(x, y, size);
    
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetWindowRect, Out"));  
    }

void CT_CMMFDevVideoPlay::DoCmdH264SelectPostProcessor(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SelectPostProcessor, In"));
    
    TInt uidInt = 0;
    if(!GetHexFromConfig(aSection, KUid(), uidInt))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUid());
        SetBlockResult(EFail);
        }
    
    TUid postproUid;
    postproUid.iUid = uidInt;
    INFO_PRINTF2(_L("PostProcessor uid is [%x]"), postproUid.iUid);
                
    TRAPD(result, iEngine->SelectPostProcessorL(postproUid));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SelectPostProcessorL left with error %d"), result);
        SetError(result);
        }     
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SelectPostProcessor, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetPostProcessTypes(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetPostProcessTypes, In"));
    
    TInt combination = 0;
    TInt hwdev = 0;

    if(!GetIntFromConfig(aSection, KHwdev(), hwdev))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHwdev());
        SetBlockResult(EFail);
        }
    if(!GetHexFromConfig(aSection, KCombination(), combination))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCombination());
        SetBlockResult(EFail);
        }
    
    TRAPD(result, iEngine->SetPostProcessTypesL(hwdev, combination));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetPostProcessTypesL left with error %d"), result);
        SetError(result);
        }    
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetPostProcessTypes, In"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264FindCommonFormats()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264FindCommonFormats, In") );
      
    TRAPD(result, iEngine->FindCommonFormatsL());
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("FindCommonFormatsL left with error %d"), result);
        SetError(result);
        }     
    INFO_PRINTF2(_L("CT_CMMFDevVideoPlay::DoCmdH264FindCommonFormats, Out [%d]"), result);
    }


void CT_CMMFDevVideoPlay::DoCmdH264GetHeaderInformation(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::GetHeaderInformation, In") );
    TInt result = KErrNone; 
    TInt dataUnitTypeInt = 0;
    TBool getDataUnitTypeInt = ETrue;
    if(!GetIntFromConfig(aSection, KDataUnitTypeInt(), dataUnitTypeInt))
        {
        getDataUnitTypeInt = EFalse;
        }
    TInt encapsulation = 0;
    TBool getEncapsulation = ETrue;
    if(!GetIntFromConfig(aSection, KEncapsulation(), encapsulation))
        {
        getEncapsulation = EFalse;
        }
    if(getDataUnitTypeInt && getEncapsulation)
        {
        TVideoDataUnitType dataUnitType;
        switch (dataUnitTypeInt)
            {
            case 1: dataUnitType = EDuCodedPicture;
                    break;
            case 2: dataUnitType = EDuVideoSegment;
                    break;
            case 3: dataUnitType = EDuSeveralSegments;
                    break;          
            case 4: dataUnitType = EDuArbitraryStreamSection;
                    break;              
            default:dataUnitType = EDuCodedPicture;         
            }
                
        TVideoDataUnitEncapsulation dataEncapsulation;      
        switch (encapsulation) 
            {
            case 1: dataEncapsulation = EDuElementaryStream;
                    break;
            case 2: dataEncapsulation = EDuGenericPayload;
                    break;
            case 3: dataEncapsulation = EDuRtpPayload;
                    break;
            default:dataEncapsulation = EDuElementaryStream;                
            }
        
        TRAP(result, iEngine->GetHeaderInformationL(dataUnitType, dataEncapsulation));
        }
    else 
        {
        TRAP(result, iEngine->GetHeaderInformationL());
        }           
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("GetHeaderInformationL left with error %d"), result);
        SetError(result);
        }     
    INFO_PRINTF2(_L("CT_CMMFDevVideoPlay::GetHeaderInformation, Out [%d]"), result);
    }

void CT_CMMFDevVideoPlay::DoCmdH264EnableSynchronization(const TTEFSectionName& aSection)
    {
    if(!GetIntFromConfig(aSection, KFramerate(), iFrameRate))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFramerate());
        SetBlockResult(EFail);
        }
    
    iEngine->EnableSynchronization(iFrameRate);
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetScaleOptions(const TTEFSectionName& aSection)
    {
    TInt width = 0;
    TInt height = 0;
    TInt antiAliasing = 0;
    TInt hwdev = 0;

    if ( !(GetIntFromConfig(aSection, KHwdev(), hwdev)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHwdev());
        SetBlockResult(EFail);
        }   
    if ( !(GetIntFromConfig(aSection, KWidth(), width)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KWidth());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KHeight(), height)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHeight());
        SetBlockResult(EFail);
        }
    if ( !(GetIntFromConfig(aSection, KY(), antiAliasing)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KY());
        SetBlockResult(EFail);
        }
 
    TSize size(width, height);
      
    TRAPD(result, iEngine->SetScaleOptionsL(hwdev, size, antiAliasing));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("FindCommonFormatsL left with error %d"), result);
        SetError(result);
        } 
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetRotateOptions(const TTEFSectionName& aSection)
{
    TInt rotItem = 0;       
    TInt hwdev = 0;
    if ( !(GetIntFromConfig(aSection, KHwdev(), hwdev)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHwdev());
        SetBlockResult(EFail);
        }   
    if ( !(GetIntFromConfig(aSection, KRotation(), rotItem)))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRotation());
        SetBlockResult(EFail);
        }
    TRotationType rotation;   
    switch(rotItem)
    {
        case 0:
            rotation = ERotateNone;
            break;
        case 90:
            rotation = ERotate90Clockwise;
            break;              
        case 180:        
            rotation = ERotate180;
            break;        
        case 270:
            rotation = ERotate90Anticlockwise;
            break;              
        default:               
            rotation = ERotateNone;                             
    }
    TRAPD(result, iEngine->SetRotateOptionsL(hwdev, rotation));
    if(result != KErrNone)
        {
        ERR_PRINTF2(_L("SetRotateOptionsL left with error %d"), result);
        SetError(result);
        } 
}

void CT_CMMFDevVideoPlay::DoCmdH264SynchronizeDecoding(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SynchronizeDecoding, In"));
    TBool sync = 0;
    if(!GetBoolFromConfig(aSection, KSync(), sync))
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSync());
        SetBlockResult(EFail);
        }
    iEngine->SynchronizeDecoding(sync);
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SynchronizeDecoding, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264Delete()
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Delete, In"));
    if (iFileOpen && iUse264)
        {
        iEngine->CloseFile();
        }
    iEngine->TearDown();
    if(iEngine)
        {
        delete iEngine;
        iEngine = NULL;
        }
    if(iObserver)
        {
        delete iObserver;
        iObserver = NULL;
        }
    if(iClock)
        {
        delete iClock;
        iClock = NULL;
        }
    while(Outstanding())
        {
        INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Delete DecOutstanding()"));
        DecOutstanding();
        }
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264Delete, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264SetPosition(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetPosition, In"));
    TInt pos;
    TBool dataOk = ETrue;
    if ( !(GetIntFromConfig(aSection, KPos(), pos)) ) //This parameter indicates seconds to move forward/backward from the current position
        {
        ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPos());
        SetBlockResult(EFail);
        dataOk = EFalse;
        }
    if(dataOk)
        {
        TInt frameTimeInterval = (TInt64)(1000000/iFrameRate);
        TInt newPosition = (iEngine->PlaybackPosition()).Int64()/frameTimeInterval + pos;
        INFO_PRINTF2(_L("set to position %d"),newPosition);
        TInt maxPosition = iEngine->GetFrameSizeCount();
        INFO_PRINTF2(_L("total framesize count %d"),maxPosition);
        if(newPosition >= maxPosition)
            {
            INFO_PRINTF3(_L("Invalid forward position %d, auto set to maximum framesize count %d"),newPosition,maxPosition-1);
            newPosition = maxPosition - 1;
            }
        if(newPosition <= 0)
            {
            INFO_PRINTF2(_L("Invalid backward position %d, auto set to starting position."),newPosition);
            newPosition = 1;
            }
        TTimeIntervalMicroSeconds timestamp = newPosition*frameTimeInterval;  
        iEngine->SetPosition(timestamp);
        }
    INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264SetPosition, Out"));
    }

void CT_CMMFDevVideoPlay::DoCmdH264AbortDirectScreenAccess()
	{
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264AbortDirectScreenAccess In"));
	iEngine->AbortDirectScreenAccess();
	INFO_PRINTF1(_L("CT_CMMFDevVideoPlay::DoCmdH264AbortDirectScreenAccess, Out"));
	}
