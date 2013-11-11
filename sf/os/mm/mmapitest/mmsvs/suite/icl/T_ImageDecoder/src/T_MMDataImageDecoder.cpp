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


#include "T_MMDataImageDecoder.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KBitmapMaskFile,						"bitmapmaskfile");
_LIT(KFrameNumber,							"framenumber");
_LIT(KBitmapFile,							"bitmapfile");
_LIT(KUseRFile,								"userfile");
_LIT(KUseTMMSource,							"usetmmsource");
_LIT(KFilename,								"filename");
_LIT(KMime,									"mime");
_LIT(KOption,								"option%d");
_LIT(KImageType,							"imagetype");
_LIT(KImageSubType,							"imagesubtype");
_LIT(KEncoderUid,							"encoderuid");
_LIT(KUid,									"uid");
_LIT(KUid2,									"uid2");
_LIT(KCommentNumber,						"commentnumber");
_LIT(KOriginalWidth,						"originalwidth");
_LIT(KOriginalHeight,						"originalheight");
_LIT(KReducedWidth,							"reducedwidth");
_LIT(KReducedHeight,						"reducedheight");
_LIT(KReductionFactor,						"reductionfactor");
_LIT(KAgentProperty,						"agentproperty");
_LIT(KAgentValue,							"agentvalue");
_LIT(KUniqueId,								"uniqueid");
_LIT(KEnableUI,								"enableui");
_LIT(KFrameCount,							"framecount");
_LIT(KCount,								"count");
_LIT(KImageName,							"imagename");
_LIT(KComment,								"comment");
_LIT(KExpected,								"expected");

_LIT(KCmdDestructor,						"~");
_LIT(KCmdCancel,							"Cancel");
_LIT(KCmdContinueConvert,					"ContinueConvert");
_LIT(KCmdContinueProcessingHeaderL,			"ContinueProcessingHeaderL");
_LIT(KCmdConvert,							"Convert");
_LIT(KCmdDataNewL,							"DataNewL");
_LIT(KCmdFileNewL,							"FileNewL");
_LIT(KCmdFrameCommentL,						"FrameCommentL");
_LIT(KCmdFrameCount,						"FrameCount");
_LIT(KCmdFrameData,							"FrameData");
_LIT(KCmdFrameInfo,							"FrameInfo");
_LIT(KCmdFrameInfoStringsL,					"FrameInfoStringsL");
_LIT(KCmdFrameInfoStringsLC,				"FrameInfoStringsLC");
_LIT(KCmdGetFileTypesL,						"GetFileTypesL");
_LIT(KCmdGetImageSubTypesL,					"GetImageSubTypesL");
_LIT(KCmdGetImageTypesL,					"GetImageTypesL");
_LIT(KCmdGetImplementationInformationL,		"GetImplementationInformationL");
_LIT(KCmdGetMimeTypeDataL,					"GetMimeTypeDataL");
_LIT(KCmdGetMimeTypeFileL,					"GetMimeTypeFileL");
_LIT(KCmdImageCommentL,						"ImageCommentL");
_LIT(KCmdImageType,							"ImageType");
_LIT(KCmdImplementationUid,					"ImplementationUid");
_LIT(KCmdIsImageHeaderProcessingComplete,	"IsImageHeaderProcessingComplete");
_LIT(KCmdNumberOfFrameComments,				"NumberOfFrameComments");
_LIT(KCmdNumberOfImageComments,				"NumberOfImageComments");
_LIT(KCmdReducedSize,						"ReducedSize");
_LIT(KCmdReductionFactor,					"ReductionFactor");
_LIT(KCmdSetAgentProperty,					"SetAgentProperty");
_LIT(KCmdSetImageTypeL,						"SetImageTypeL");
/*@}*/

const TInt	KMimeBufferSize = 1024;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataImageDecoder* CT_MMDataImageDecoder::NewL()
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
	{
	CT_MMDataImageDecoder*	ret=new (ELeave) CT_MMDataImageDecoder();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataImageDecoder::CT_MMDataImageDecoder()
/**
 * Protected constructor. First phase construction
 */
:	iImageDecoder(NULL)
,	iFbsSessionConnected(EFalse)
,	iState(EState_Unknown)
,	iActive(NULL)
,	iBitmap(NULL)
,	iBitmapMask(NULL)
,	iFrameInfoStrings(NULL)
,	iImplementationInformationType(NULL)
,	iFileOpened(EFalse)
,	iHasBitmapFile(EFalse)
,   iAsyncConvertFailed(EFalse)
,	iDataSource(NULL)
	{
	}

void CT_MMDataImageDecoder::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	User::LeaveIfError(iFs.Connect());

	User::LeaveIfError(iFbsSession.Connect()); // Will leave with KErrNotFound if TextShell in epoc.ini
	iFbsSessionConnected=ETrue;

	iActive = CActiveCallback::NewL(*this);
	}

CT_MMDataImageDecoder::~CT_MMDataImageDecoder()
/**
 * Public destructor
 */
	{
	DestroyData();

	delete iDataSource;
	iDataSource=NULL;
	delete iImplementationInformationType;
	iImplementationInformationType=NULL;
	delete iFrameInfoStrings;
	iFrameInfoStrings=NULL;
	delete iBitmapMask;
	iBitmapMask=NULL;
	delete iBitmap;
	iBitmap=NULL;
	delete iActive;
	iActive=NULL;
	if ( iFbsSessionConnected )
		{
		iFbsSession.Disconnect();
		iFbsSessionConnected=EFalse;
		}
	}

TAny* CT_MMDataImageDecoder::GetObject()
	{
	return iImageDecoder;
	}

void CT_MMDataImageDecoder::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iImageDecoder	= static_cast<CImageDecoder*> (aObject);
	}

void CT_MMDataImageDecoder::DisownObjectL()
	{
	iImageDecoder = NULL;
	}

void CT_MMDataImageDecoder::DestroyData()
	{
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	iFs.Close();
	iImageTypeArray.Close();
	iSubTypeArray.Close();
	iFileExtensionArray.Close();
	delete iImageDecoder;
	iImageDecoder=NULL;
	}

CImageDecoder::TOptions CT_MMDataImageDecoder::ReadOptions(const TDesC& aSection)
/**
 ReadOptions - read in bitwise image decoder options
 @internalComponent
 @param aSection - main entry in .ini file
 @return CImageDecoder::TOptions
 @pre None
 @post 2nd stage construction complete
*/
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								index=0;
	TBool								moreData=ETrue;
	TPtrC								optionString;
	TInt								options=CImageDecoder::EOptionNone;
	while ( moreData )
		{
		tempStore.Format(KOption(), ++index);
		if ( GetStringFromConfig(aSection, tempStore, optionString) )
			{
			CImageDecoder::TOptions	option;
			if ( CT_MMUtil::ConvertToOption(optionString, option) )
				{
				options |= option;
				}
			else
				{
				ERR_PRINTF2(_L("Illegal option %S"), &optionString);
				SetBlockResult(EFail);
				}
			}
		else
			{
			moreData=EFalse;
			}
		}

	return CImageDecoder::TOptions(options);
	}

void CT_MMDataImageDecoder::ReadUids(const TDesC& aSection, TUid& aImageType, TUid& aImageSubType, TUid& aEncoderUid)
/**
 ReadUids - read in bitwise image encoder options
 @internalComponent
 @param aSection - main entry in .ini file
 @param aImageType - reference to image type uid if not present uid returned is 0
 @param aImageSubType - reference to image sub type uid if not present uid returned is 0
 @param aEncoderUid - reference to encoder uid if not present uid returned is 0
 @return N/A
 @pre None
 @post 2nd stage construction complete
*/
	{
	TInt								intTemp;

	intTemp=0;
	GetHexFromConfig(aSection, KImageType(), intTemp);
	aImageType=TUid::Uid(intTemp);

	intTemp=0;
	GetHexFromConfig(aSection, KImageSubType(), intTemp);
	aImageSubType=TUid::Uid(intTemp);

	intTemp=0;
	GetHexFromConfig(aSection, KEncoderUid(), intTemp);
	aEncoderUid=TUid::Uid(intTemp);
	}

TBool CT_MMDataImageDecoder::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdCancel )
		{
		DoCmdCancel();
		}
	else if ( aCommand==KCmdContinueConvert )
		{
		DoCmdContinueConvert(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdContinueProcessingHeaderL )
		{
		DoCmdContinueProcessingHeaderL();
		}
	else if ( aCommand==KCmdConvert )
		{
		DoCmdConvertL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdDataNewL )
		{
		DoCmdDataNewL(aSection);
		}
	else if ( aCommand==KCmdFileNewL )
		{
		DoCmdFileNewL(aSection);
		}
	else if ( aCommand==KCmdFrameCommentL )
		{
		DoCmdFrameCommentL(aSection);
		}
	else if ( aCommand==KCmdFrameCount )
		{
		DoCmdFrameCount(aSection);
		}
	else if ( aCommand==KCmdFrameData )
		{
		DoCmdFrameData(aSection);
		}
	else if ( aCommand==KCmdFrameInfo )
		{
		DoCmdFrameInfo(aSection);
		}
	else if ( aCommand==KCmdFrameInfoStringsL )
		{
		DoCmdFrameInfoStringsL(aSection);
		}
	else if ( aCommand==KCmdFrameInfoStringsLC )
		{
		DoCmdFrameInfoStringsLCApi(aSection);
		}
	else if ( aCommand==KCmdGetFileTypesL )
		{
		DoCmdGetFileTypesL(aSection);
		}
	else if ( aCommand==KCmdGetImageSubTypesL )
		{
		DoCmdGetImageSubTypesL(aSection);
		}
	else if ( aCommand==KCmdGetImageTypesL )
		{
		DoCmdGetImageTypesL(aSection);
		}
	else if ( aCommand==KCmdGetImplementationInformationL )
		{
		DoCmdGetImplementationInformationL(aSection);
		}
	else if ( aCommand==KCmdGetMimeTypeDataL )
		{
		DoCmdGetMimeTypeDataL(aSection);
		}
	else if ( aCommand==KCmdGetMimeTypeFileL )
		{
		DoCmdGetMimeTypeFileL(aSection);
		}
	else if ( aCommand==KCmdImageCommentL )
		{
		DoCmdImageCommentL(aSection);
		}
	else if ( aCommand==KCmdImageType )
		{
		DoCmdImageType(aSection);
		}
	else if ( aCommand==KCmdImplementationUid )
		{
		DoCmdImplementationUid();
		}
	else if ( aCommand==KCmdIsImageHeaderProcessingComplete )
		{
		DoCmdIsImageHeaderProcessingComplete(aSection);
		}
	else if ( aCommand==KCmdNumberOfFrameComments )
		{
		DoCmdNumberOfFrameComments(aSection);
		}
	else if ( aCommand==KCmdNumberOfImageComments )
		{
		DoCmdNumberOfImageComments(aSection);
		}
	else if ( aCommand==KCmdReducedSize )
		{
		DoCmdReducedSize(aSection);
		}
	else if ( aCommand==KCmdReductionFactor )
		{
		DoCmdReductionFactor(aSection);
		}
	else if ( aCommand==KCmdSetAgentProperty )
		{
		DoCmdSetAgentProperty(aSection);
		}
	else if ( aCommand==KCmdSetImageTypeL )
		{
		DoCmdSetImageTypeL(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataImageDecoder::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_MMDataImageDecoder::DoCmdCancel()
	{
	iImageDecoder->Cancel();
	}

void CT_MMDataImageDecoder::DoCmdContinueConvert(const TInt aAsyncErrorIndex)
	{
	iImageDecoder->ContinueConvert(&iActive->iStatus);
	iActive->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_MMDataImageDecoder::DoCmdContinueProcessingHeaderL()
	{
	TRAPD(err, iImageDecoder->ContinueProcessingHeaderL());
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::ContinueProcessingHeaderL Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataImageDecoder::DoCmdConvertL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TPtrC	bitmapMaskFile;
	TBool	hasMask=GetStringFromConfig(aSection, KBitmapMaskFile(), bitmapMaskFile);

	if ( hasMask )
		{
		delete iBitmapMask;
		iBitmapMask=NULL;
		iBitmapMask=new (ELeave) CFbsBitmap();
		User::LeaveIfError(iBitmapMask->Load(bitmapMaskFile));
		}

	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	iHasBitmapFile=GetStringFromConfig(aSection, KBitmapFile(), iBitmapFile);

	delete iBitmap;
	iBitmap=NULL;
	iBitmap=new (ELeave) CFbsBitmap();
	TInt	err=iBitmap->Create(iFrameInfo.iOverallSizeInPixels, iFrameInfo.iFrameDisplayMode);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CFbsBitmap::Load failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		if ( hasMask )
			{
			iState=EState_Convert;
			iImageDecoder->Convert(&(iActive->iStatus), *iBitmap, *iBitmapMask, frameNumber);
			}
		else
			{
			iState=EState_Convert;
			iImageDecoder->Convert(&(iActive->iStatus), *iBitmap, frameNumber);
			}
		iActive->Activate(aAsyncErrorIndex);
	    IncOutstanding();
		}
	}

void CT_MMDataImageDecoder::DoCmdDataNewL(const TDesC& aSection)
	{
	delete iDataSource;
	iDataSource=NULL;
	TPtrC	filename;
	TBool fileNameFound = GetStringFromConfig(aSection, KFilename(), filename);
	if ( !fileNameFound )
		{
		ERR_PRINTF1(_L("No Filename"));
		SetBlockResult(EFail);
		}
	else
		{
		RFile	file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead|EFileShareAny));
		CleanupClosePushL(file);
		TInt	size;
		User::LeaveIfError(file.Size(size));
		iDataSource=HBufC8::NewL(size);
		TPtr8	sourcePtr(iDataSource->Des());
		User::LeaveIfError(file.Read(sourcePtr));
		CleanupStack::PopAndDestroy(&file);
		}

	TPtrC	mime;
	TBool	hasMime=GetStringFromConfig(aSection, KMime(), mime);

	//	Read options
	CImageDecoder::TOptions	options=ReadOptions(aSection);

	//	Read Uids
	TUid	imageType;
	TUid	imageSubType;
	TUid	encoderUid;
	ReadUids(aSection, imageType, imageSubType, encoderUid);

	if ( iAsyncConvertFailed==EFalse )
		{
		HBufC8*	mime8=HBufC8::NewLC(mime.Length());
		mime8->Des().Copy(mime);

		if ( hasMime )
			{
			TRAPD(err, iImageDecoder=CImageDecoder::DataNewL(iFs, *iDataSource, *mime8, options));
			if ( err == KErrNone )
				{
				INFO_PRINTF1(_L("CImageDecoder::DataNewL(iFs, *iDataSource, mime8->Des(), options)"));
				}
			else
				{
				ERR_PRINTF2(_L("CImageDecoder::DataNewL Error %d"), err);
				SetError(err);
				}
			}
		else
			{
			TRAPD(err, iImageDecoder=CImageDecoder::DataNewL(iFs, *iDataSource, options, imageType, imageSubType, encoderUid));
			if ( err == KErrNone )
				{
				INFO_PRINTF1(_L("CImageDecoder::DataNewL(iFs, *iDataSource, options, imageType, imageSubType, encoderUid)"));
				}
			else
				{
				ERR_PRINTF2(_L("CImageDecoder::DataNewL Error %d"), err);
				SetError(err);
				}
			}
		CleanupStack::PopAndDestroy(mime8);
		}
	}

void CT_MMDataImageDecoder::DoCmdFileNewL(const TDesC& aSection)
	{
	TBool	useRFile=EFalse;
	GetBoolFromConfig(aSection, KUseRFile(), useRFile);

	TBool	useTMMSource=EFalse;
	GetBoolFromConfig(aSection, KUseTMMSource(), useTMMSource);

	TPtrC	filename;
	TBool fileNameFound = GetStringFromConfig(aSection, KFilename(), filename);
	if ( !fileNameFound )
		{
		ERR_PRINTF1(_L("No Filename"));
		SetBlockResult(EFail);
		}

	TPtrC	mime;
	TBool	hasMime=GetStringFromConfig(aSection, KMime(), mime);

	//	Read options
	CImageDecoder::TOptions	options=ReadOptions(aSection);

	//	Read intent
	ContentAccess::TIntent	intent=ContentAccess::EPeek;
	TBool					hasIntent=CT_MMUtil::ReadIntent(*this, aSection, intent);

	//	Read Uids
	TUid	imageType;
	TUid	imageSubType;
	TUid	encoderUid;
	ReadUids(aSection, imageType, imageSubType, encoderUid);

	if ( iAsyncConvertFailed==EFalse )
		{
		HBufC8*	mime8=HBufC8::NewLC(mime.Length());
		mime8->Des().Copy(mime);
		if ( useRFile )
			{
			if ( iFileOpened )
				{
				iFile.Close();
				iFileOpened=EFalse;
				}
			User::LeaveIfError(iFile.Open(iFs, filename, EFileWrite));
			iFileOpened=ETrue;
			if ( hasMime )
				{
				TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFile, mime8->Des(), intent, options));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFile, mime8->Des(), intent, options)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			else
				{
				TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFile, intent, options, imageType, imageSubType, encoderUid));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFile, intent, options, imageType, imageSubType, encoderUid)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			}
		else
			{
			if ( useTMMSource )
				{
				TPtrC	uniqueId = _L("");
				GetStringFromConfig(aSection, KUniqueId(), uniqueId);

				TBool	enableUI = EFalse;
				GetBoolFromConfig(aSection, KEnableUI(), enableUI);

				TMMFileSource	fileSource(filename, uniqueId, intent, enableUI);
				if ( hasMime )
					{
					TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, fileSource, mime8->Des(), options));
					if ( err == KErrNone )
						{
						INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, fileSource, mime8->Des(), options)"));
						}
					else
						{
						ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
						SetError(err);
						}
					}
				else
					{
					TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, fileSource, options, imageType, imageSubType, encoderUid));
					if ( err == KErrNone )
						{
						INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, fileSource, options, imageType, imageSubType, encoderUid)"));
						}
					else
						{
						ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
						SetError(err);
						}
					}
				}
			else
				{
				if ( hasMime )
					{
					if ( hasIntent )
						{
						TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, filename, mime8->Des(), intent, options));
						if ( err == KErrNone )
							{
							INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, filename, mime8->Des(), intent, options)"));
							}
						else
							{
							ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
							SetError(err);
							}
						}
					else
						{
						TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, filename, mime8->Des(), options));
						if ( err == KErrNone )
							{
							INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, filename, mime8->Des(), options)"));
							}
						else
							{
							ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
							SetError(err);
							}
						}
					}
				else
					{
					if ( hasIntent )
						{
						TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, filename, intent, options, imageType, imageSubType, encoderUid));
						if ( err == KErrNone )
							{
							INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, filename, intent, options, imageType, imageSubType, encoderUid)"));
							}
						else
							{
							ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
							SetError(err);
							}
						}
					else
						{
						TRAPD(err, iImageDecoder=CImageDecoder::FileNewL(iFs, filename, options, imageType, imageSubType, encoderUid));
						if ( err == KErrNone )
							{
							INFO_PRINTF1(_L("CImageDecoder::FileNewL(iFs, filename, options, imageType, imageSubType, encoderUid)"));
							}
						else
							{
							ERR_PRINTF2(_L("CImageDecoder::FileNewL Error %d"), err);
							SetError(err);
							}
						}
					}
				}
			}
		CleanupStack::PopAndDestroy(mime8);
		}
	}

void CT_MMDataImageDecoder::DoCmdFrameCommentL(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	TInt	commentNumber=0;
	GetIntFromConfig(aSection, KCommentNumber(), commentNumber);

	HBufC*	comment=NULL;
	TRAPD(err, comment=iImageDecoder->FrameCommentL(frameNumber, commentNumber));
	CleanupStack::PushL(comment);
	if ( err==KErrNone )
		{
		TPtrC	output=comment->Des();
		INFO_PRINTF2(_L("CImageDecoder::FrameCommentL() %S"), &output);

		TPtrC	expectedComment;
		if ( GetStringFromConfig(aSection, KComment(), expectedComment) )
			{
			if ( output != expectedComment )
				{
				ERR_PRINTF3(_L("comment (%S) != expectedComment (%S)"), &output, &expectedComment);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("CImageDecoder::FrameCommentL Error %d"), err);
		SetError(err);
		}
	CleanupStack::PopAndDestroy(comment);
	}

void CT_MMDataImageDecoder::DoCmdFrameCount(const TDesC& aSection)
	{
	TInt	frameCount=iImageDecoder->FrameCount();
	INFO_PRINTF2(_L("Frame Count=%d"), frameCount);

	TInt	expectedFrameCount=0;
	if ( GetIntFromConfig(aSection, KFrameCount(), expectedFrameCount) )
		{
		if ( frameCount != expectedFrameCount )
			{
			ERR_PRINTF3(_L("frameCount(%d) != expectedFrameCount(%d)"), frameCount, expectedFrameCount);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdFrameData(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	const CFrameImageData&	frameData=iImageDecoder->FrameData(frameNumber);
	TInt					imageDataCount=frameData.ImageDataCount();
	for ( TInt index=0; index<imageDataCount; ++index )
		{
		const TImageDataBlock*	imageData=frameData.GetImageData(index);
		TUid					dataType=imageData->DataType();
		INFO_PRINTF3(_L("DataType[%d]=0x%X"), index, dataType.iUid);
		}
	}

void CT_MMDataImageDecoder::DoCmdFrameInfo(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	iFrameInfo=iImageDecoder->FrameInfo(frameNumber);
	// Only log if frameNumber is valid
	if ( frameNumber<iImageDecoder->FrameCount() )
		{
		INFO_PRINTF2(_L("Current Frame State         %d"), iFrameInfo.CurrentFrameState());
		INFO_PRINTF2(_L("Current Data Offset         %d"), iFrameInfo.CurrentDataOffset());
		INFO_PRINTF2(_L("Frame Data Offset           %d"), iFrameInfo.FrameDataOffset());
		INFO_PRINTF2(_L("Colour depth                %d"), iFrameInfo.iBitsPerPixel);
		INFO_PRINTF2(_L("iOverallSizeInPixels Width  %d"), iFrameInfo.iOverallSizeInPixels.iWidth);
		INFO_PRINTF2(_L("iOverallSizeInPixels Height %d"), iFrameInfo.iOverallSizeInPixels.iHeight);
		INFO_PRINTF2(_L("iFrameDisplayMode           %d"), iFrameInfo.iFrameDisplayMode);
		}
	}

void CT_MMDataImageDecoder::DoCmdFrameInfoStringsL(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	delete iFrameInfoStrings;
	iFrameInfoStrings=NULL;
	TRAPD(err, iFrameInfoStrings=iImageDecoder->FrameInfoStringsL(frameNumber));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::FrameInfoStringsL Error %d"), err);
		SetError(err);
		}
	else
		{
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TPtrC								expectedString;
		for ( TInt index=0; index<iFrameInfoStrings->Count(); ++index )
			{
			TPtrC	currentString = iFrameInfoStrings->String(index);
			INFO_PRINTF3(_L("iFrameInfoStrings::String(%d) %S"), index, &currentString);

			tempStore.Format(KOption(), index);
			if ( GetStringFromConfig(aSection, tempStore, expectedString) )
				{
				if ( currentString != expectedString )
					{
					ERR_PRINTF3(_L("currentString (%S) != expectedString (%S)"), &currentString, &expectedString);
					SetBlockResult(EFail);
					}
				}
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdFrameInfoStringsLCApi(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	delete iFrameInfoStrings;
	iFrameInfoStrings=NULL;
	TRAPD(err, iFrameInfoStrings=iImageDecoder->FrameInfoStringsLC(frameNumber); CleanupStack::Pop(iFrameInfoStrings));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::FrameInfoStringsLC Error %d"), err);
		SetError(err);
		}
	else
		{
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		TPtrC								expectedString;
		for ( TInt index=0; index<iFrameInfoStrings->Count(); ++index )
			{
			TPtrC	currentString = iFrameInfoStrings->String(index);
			INFO_PRINTF3(_L("iFrameInfoStrings::String(%d) %S"), index, &currentString);

			tempStore.Format(KOption(), index);
			if ( GetStringFromConfig(aSection, tempStore, expectedString) )
				{
				if ( currentString != expectedString )
					{
					ERR_PRINTF3(_L("currentString (%S) != expectedString (%S)"), &currentString, &expectedString);
					SetBlockResult(EFail);
					}
				}
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdGetFileTypesL(const TDesC& aSection)
	{
	iFileExtensionArray.ResetAndDestroy();
	TRAPD(err, CImageDecoder::GetFileTypesL(iFileExtensionArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::GetFileTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::VerifyMIMETypeArrayL(*this, aSection, iFileExtensionArray);
		}
	}

void CT_MMDataImageDecoder::DoCmdGetImageSubTypesL(const TDesC& aSection)
	{
	TInt	uid=0;
	GetHexFromConfig(aSection, KImageType(), uid);

	iSubTypeArray.ResetAndDestroy();
	TRAPD(err, CImageDecoder::GetImageSubTypesL(TUid::Uid(uid), iSubTypeArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::GetImageSubTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		TInt	count=iSubTypeArray.Count();
		for ( TInt index=0; index<count; ++index )
			{
			CImageTypeDescription*	data=iSubTypeArray[index];
			const TDesC&			description=data->Description();
			TUid					imageType=data->ImageType();
			TUid					subType=data->SubType();
			INFO_PRINTF3(_L("Description[%d]      =%S"), index, &description);
			INFO_PRINTF3(_L("ImageType[%d]        =0x%X"), index, imageType.iUid);
			INFO_PRINTF3(_L("SubType[%d]          =0x%X"), index, subType.iUid);
			}
		TInt	expectedCount=0;
		if ( GetIntFromConfig(aSection, KCount(), expectedCount) )
			{
			if ( count != expectedCount )
				{
				ERR_PRINTF3(_L("count (%d) != expectedCount (%d)"), count, expectedCount);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdGetImageTypesL(const TDesC& aSection)
	{
	iImageTypeArray.ResetAndDestroy();
	TRAPD(err, CImageDecoder::GetImageTypesL(iImageTypeArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::GetImageTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		TInt	count=iImageTypeArray.Count();
		for ( TInt index=0; index<count; ++index )
			{
			CImageTypeDescription*	data=iImageTypeArray[index];
			const TDesC&			description=data->Description();
			TUid					imageType=data->ImageType();
			TUid					subType=data->SubType();
			INFO_PRINTF3(_L("Description[%d]      =%S"), index, &description);
			INFO_PRINTF3(_L("ImageType[%d]        =0x%X"), index, imageType.iUid);
			INFO_PRINTF3(_L("SubType[%d]          =0x%X"), index, subType.iUid);
			}
		TInt	expectedCount=0;
		if ( GetIntFromConfig(aSection, KCount(), expectedCount) )
			{
			if ( count != expectedCount )
				{
				ERR_PRINTF3(_L("count (%d) != expectedCount (%d)"), count, expectedCount);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdGetImplementationInformationL(const TDesC& aSection)
	{
	TInt	uid=0;
	GetHexFromConfig(aSection, KUid(), uid);

	delete iImplementationInformationType;
	iImplementationInformationType=NULL;
	TRAPD(err, iImplementationInformationType=CImageDecoder::GetImplementationInformationL(TUid::Uid(uid)));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::GetImplementationInformationL Error %d"), err);
		SetError(err);
		}
	else
		{
		TUid			implementationUid=iImplementationInformationType->ImplementationUid();
		const TDesC&	displayName=iImplementationInformationType->DisplayName();
		INFO_PRINTF2(_L("iImplementationInformationType->ImplementationUid()  =0x%X"), implementationUid.iUid);
		INFO_PRINTF2(_L("iImplementationInformationType->Version()			=%d"), iImplementationInformationType->Version());
		INFO_PRINTF2(_L("iImplementationInformationType->DisplayName()		=%S"), &displayName);

		TInt	expectedUid=uid;
		if ( expectedUid != implementationUid.iUid )
			{
			ERR_PRINTF3(_L("implementationUid (0x%X) != expectedImplementationUid (0x%X)"), implementationUid.iUid, expectedUid);
			SetBlockResult(EFail);
			}

		TPtrC	expectedImageName;
		if ( GetStringFromConfig(aSection, KImageName(), expectedImageName) )
			{
			if ( displayName != expectedImageName )
				{
				ERR_PRINTF3(_L("displayName (%S) != expectedDisplayName (%S)"), &displayName, &expectedImageName);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdGetMimeTypeDataL(const TDesC& aSection)
	{
	TPtrC	filename;
	if ( GetStringFromConfig(aSection, KFilename(), filename) )
		{
		RFile	file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead|EFileShareAny));
		CleanupClosePushL(file);
		TInt	size;
		User::LeaveIfError(file.Size(size));
		HBufC8*	imageData=HBufC8::NewLC(size);
		TPtr8	imageDataPtr(imageData->Des());
		User::LeaveIfError(file.Read(imageDataPtr));

		TBuf8<KMimeBufferSize>	mimeBuffer;
		TRAPD(err, CImageDecoder::GetMimeTypeDataL(*imageData, mimeBuffer));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CImageDecoder::GetMimeTypeDataL Error %d"), err);
			SetError(err);
			}
		else
			{
			HBufC16*	mimeBuffer16=NULL;
			mimeBuffer16=HBufC16::NewL(mimeBuffer.Length());
			CleanupStack::PushL(mimeBuffer16);
			mimeBuffer16->Des().Copy(mimeBuffer);
			INFO_PRINTF2(_L("CImageDecoder::GetMimeTypeDataL() %S"), mimeBuffer16);

			TPtrC	expectedMimeType;
			if ( GetStringFromConfig(aSection, KMime(), expectedMimeType) )
				{
				if ( *mimeBuffer16 != expectedMimeType )
					{
					ERR_PRINTF3(_L("mimeType (%S) != expectedMimeType (%S)"), mimeBuffer16, &expectedMimeType);
					SetBlockResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(mimeBuffer16);
			}
		CleanupStack::PopAndDestroy(2, &file); // imageData, file
		}
	else
		{
		ERR_PRINTF1(_L("No Filename"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageDecoder::DoCmdGetMimeTypeFileL(const TDesC& aSection)
	{
	TPtrC	filename;
	if ( GetStringFromConfig(aSection, KFilename(), filename) )
		{
		TBuf8<KMimeBufferSize>	mimeBuffer;
		TRAPD(err, CImageDecoder::GetMimeTypeFileL(iFs, filename, mimeBuffer));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CImageDecoder::GetMimeTypeFileL Error %d"), err);
			SetError(err);
			}
		else
			{
			HBufC16*	mimeBuffer16=NULL;
			mimeBuffer16=HBufC16::NewL(mimeBuffer.Length());
			CleanupStack::PushL(mimeBuffer16);
			mimeBuffer16->Des().Copy(mimeBuffer);
			INFO_PRINTF2(_L("CImageDecoder::GetMimeTypeFileL() %S"), mimeBuffer16);

			TPtrC	expectedMimeType;
			if ( GetStringFromConfig(aSection, KMime(), expectedMimeType) )
				{
				if ( *mimeBuffer16 != expectedMimeType )
					{
					ERR_PRINTF3(_L("mimeType (%S) != expectedMimeType (%S)"), mimeBuffer16, &expectedMimeType);
					SetBlockResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(mimeBuffer16);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No Filename"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageDecoder::DoCmdImageCommentL(const TDesC& aSection)
	{
	TInt	commentNumber=1;
	GetIntFromConfig(aSection, KCommentNumber(), commentNumber);

	HBufC*	comment=NULL;
	TRAPD(err, comment=iImageDecoder->ImageCommentL(commentNumber));
	CleanupStack::PushL(comment);
	if ( err==KErrNone )
		{
		TPtrC	output=comment->Des();
		INFO_PRINTF2(_L("CImageDecoder::ImageCommentL() %S"), &output);

		TPtrC	expectedComment;
		if ( GetStringFromConfig(aSection, KComment(), expectedComment) )
			{
			if ( output != expectedComment )
				{
				ERR_PRINTF3(_L("comment (%S) != expectedComment (%S)"), &output, &expectedComment);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("CImageDecoder::ImageCommentL Error %d"), err);
		SetError(err);
		}
	CleanupStack::PopAndDestroy(comment);
	}

void CT_MMDataImageDecoder::DoCmdImageType(const TDesC& aSection)
	{
	TInt	frameNumber=0;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	TUid	imageType;
	TUid	imageSubType;
	iImageDecoder->ImageType(frameNumber, imageType, imageSubType);
	INFO_PRINTF3(_L("Image TypeUid 0x%X SubTypeUid 0x%X"), imageType.iUid, imageSubType.iUid);

	TInt	expectedTypeUid=0;
	if ( GetHexFromConfig(aSection, KUid(), expectedTypeUid) )
		{
		if ( expectedTypeUid != imageType.iUid )
			{
			ERR_PRINTF3(_L("TypeUid (0x%X) != expectedTypeUid (0x%X)"), imageType.iUid, expectedTypeUid);
			SetBlockResult(EFail);
			}
		}

	TInt	expectedSubTypeUid=0;
	if ( GetHexFromConfig(aSection, KUid2(), expectedSubTypeUid) )
		{
		if ( expectedSubTypeUid != imageSubType.iUid )
			{
			ERR_PRINTF3(_L("SubTypeUid (0x%X) != expectedSubTypeUid (0x%X)"), imageSubType.iUid, expectedSubTypeUid);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdImplementationUid()
	{
	TUid	uid=iImageDecoder->ImplementationUid();
	INFO_PRINTF2(_L("ImplementationUid Uid:0x%X"), uid.iUid);
	}

void CT_MMDataImageDecoder::DoCmdIsImageHeaderProcessingComplete(const TDesC& aSection)
	{
	TBool	ret=iImageDecoder->IsImageHeaderProcessingComplete();
	INFO_PRINTF2(_L("IsImageHeaderProcessingComplete %d"), ret);

	TBool	expectedResult=EFalse;
	if ( GetBoolFromConfig(aSection, KExpected(), expectedResult) )
		{
		if ( expectedResult != ret )
			{
			ERR_PRINTF3(_L("HeaderProcessingComplete (%d) != expectedHeaderProcessingComplete (%d)"), ret, expectedResult);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdNumberOfFrameComments(const TDesC& aSection)
	{
	TInt	frameNumber=1;
	GetIntFromConfig(aSection, KFrameNumber(), frameNumber);

	TInt	ret=iImageDecoder->NumberOfFrameComments(frameNumber);
	INFO_PRINTF2(_L("NumberOfFrameComments %d"), ret);

	TInt	expectedResult=0;
	if ( GetIntFromConfig(aSection, KExpected(), expectedResult) )
		{
		if ( expectedResult != ret )
			{
			ERR_PRINTF3(_L("NumberOfFrameComments (%d) != expectedNumberOfFrameComments (%d)"), ret, expectedResult);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdNumberOfImageComments(const TDesC& aSection)
	{
	TInt	ret=iImageDecoder->NumberOfImageComments();
	INFO_PRINTF2(_L("NumberOfImageComments %d"), ret);

	TInt	expectedResult=0;
	if ( GetIntFromConfig(aSection, KExpected(), expectedResult) )
		{
		if ( expectedResult != ret )
			{
			ERR_PRINTF3(_L("NumberOfImageComments (%d) != expectedNumberOfImageComments (%d)"), ret, expectedResult);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdReducedSize(const TDesC& aSection)
	{
	TSize	original(1, 1);
	GetIntFromConfig(aSection, KOriginalWidth(), original.iWidth);
	GetIntFromConfig(aSection, KOriginalHeight(), original.iHeight);

	TInt	reductionFactor=1;
	GetIntFromConfig(aSection, KReductionFactor(), reductionFactor);

	TSize	reducedSize;
	TInt	err=iImageDecoder->ReducedSize(original, reductionFactor, reducedSize);
	if ( err==KErrNone )
		{
		INFO_PRINTF3(_L("ReducedSize (width=%d, height=%d)"), reducedSize.iWidth, reducedSize.iHeight);

		TInt	expectedReducedWidth=0;
		if ( GetIntFromConfig(aSection, KReducedWidth(), expectedReducedWidth) )
			{
			if ( expectedReducedWidth != reducedSize.iWidth )
				{
				ERR_PRINTF3(_L("ReducedWidth (%d) != expectedReducedWidth (%d)"), reducedSize.iWidth, expectedReducedWidth);
				SetBlockResult(EFail);
				}
			}

		TInt	expectedReducedHeight=0;
		if ( GetIntFromConfig(aSection, KReducedHeight(), expectedReducedHeight) )
			{
			if ( expectedReducedHeight != reducedSize.iHeight )
				{
				ERR_PRINTF3(_L("ReducedHeight (%d) != expectedReducedHeight (%d)"), reducedSize.iHeight, expectedReducedHeight);
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("CImageDecoder::ReducedSize Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataImageDecoder::DoCmdReductionFactor(const TDesC& aSection)
	{
	TSize	original(1, 1);
	GetIntFromConfig(aSection, KOriginalWidth(), original.iWidth);
	GetIntFromConfig(aSection, KOriginalHeight(), original.iHeight);

	TSize	reduced(1, 1);
	GetIntFromConfig(aSection, KReducedWidth(), reduced.iWidth);
	GetIntFromConfig(aSection, KReducedHeight(), reduced.iHeight);

	TInt	factor=iImageDecoder->ReductionFactor(original, reduced);
	INFO_PRINTF2(_L("Reduction factor %d"), factor);

	TInt	expectedFactor=0;
	if ( GetIntFromConfig(aSection, KReductionFactor(), expectedFactor) )
		{
		if ( expectedFactor != factor )
			{
			ERR_PRINTF3(_L("factor (%d) != expectedFactor (%d)"), factor, expectedFactor);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataImageDecoder::DoCmdSetAgentProperty(const TDesC& aSection)
	{
	ContentAccess::TAgentProperty	agentProperty;
	TPtrC							agentPropertyString;
	if ( GetStringFromConfig(aSection, KAgentProperty(), agentPropertyString) )
		{
		if ( !CT_MMUtil::ConvertToAgentProperty(agentPropertyString, agentProperty) )
			{
			ERR_PRINTF2(_L("Illegal agent property %S"), &agentPropertyString);
			SetBlockResult(EFail);
			}
		else
			{
			TInt	agentValue=0;
			GetIntFromConfig(aSection, KAgentValue(), agentValue);
			TInt	err=iImageDecoder->SetAgentProperty(agentProperty, agentValue);
			if ( err!=KErrNone )
				{
				ERR_PRINTF2(_L("CImageDecoder::SetAgentProperty Error %d"), err);
				SetError(err);
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("No agent property"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageDecoder::DoCmdSetImageTypeL(const TDesC& aSection)
	{
	CImageDecoder::TImageType	imageType;
	if ( !CT_MMUtil::ReadImageType(*this, aSection, imageType) )
		{
		imageType=CImageDecoder::EImageTypeMain;
		}

	TRAPD(err, iImageDecoder->SetImageTypeL(imageType));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageDecoder::SetImageTypeLError %d"), err);
		SetError(err);
		}
	}

void CT_MMDataImageDecoder::RunL(CActive* aActive, TInt aIndex)
/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
	{
	if ( aActive==iActive )
		{
		RunActiveL(aIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageDecoder::DoCancel(CActive* aActive, TInt /*aIndex*/)
/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @internalComponent
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
	{
	if ( aActive==iActive )
		{
		DoCancelActive();
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageDecoder::RunActiveL(TInt aIndex)
/**
 RunActiveL - Process RunL for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
	{
	TInt	err=iActive->iStatus.Int();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
		iAsyncConvertFailed = ETrue;
		}
	else
		{
		switch ( iState )
			{
		case EState_Unknown:
			WARN_PRINTF1(_L("State not set"));
			break;
		case EState_Convert:
			if ( iHasBitmapFile )
				{
				iBitmap->Save(iBitmapFile);
				iHasBitmapFile=EFalse;
				}
			iState=EState_Unknown;
			break;
		default:
			ERR_PRINTF2(_L("Illegal state %d"), iState);
			SetBlockResult(EFail);
			};
		}
    DecOutstanding();
	}

void CT_MMDataImageDecoder::DoCancelActive()
/**
 DoCancelActive - Process DoCancel for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
	{
	iImageDecoder->Cancel();
	}
