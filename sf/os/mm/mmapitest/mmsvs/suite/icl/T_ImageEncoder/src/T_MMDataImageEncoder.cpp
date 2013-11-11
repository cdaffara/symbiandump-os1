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


#include "T_MMDataImageEncoder.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KBitmapFile,							"bitmapfile");
_LIT(KUseRFile,								"userfile");
_LIT(KUseClosedRFile,						"useclosedrfile");
_LIT(KFilename,								"filename");
_LIT(KMime,									"mime");
_LIT(KOption,								"option%d");
_LIT(KImageType,							"imagetype");
_LIT(KImageSubType,							"imagesubtype");
_LIT(KEncoderUid,							"encoderuid");
_LIT(KThumbNail,							"thumbnail");
_LIT(KUseBmpDataType,						"usebmpdatatype");
_LIT(KDataTypeSetting,						"datatypesetting");
_LIT(KCount,								"count");

_LIT(KCmdDestructor,						"~");
_LIT(KCmdCancel,							"Cancel");
_LIT(KCmdConvert,							"Convert");
_LIT(KCmdDataNewL,							"DataNewL");
_LIT(KCmdFileNewL,							"FileNewL");
_LIT(KCmdGetFileTypesL,						"GetFileTypesL");
_LIT(KCmdGetImageSubTypesL,					"GetImageSubTypesL");
_LIT(KCmdGetImageTypesL,					"GetImageTypesL");
_LIT(KCmdImplementationUid,					"ImplementationUid");
_LIT(KCmdSetThumbnail,						"SetThumbnail");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataImageEncoder* CT_MMDataImageEncoder::NewL()
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
	{
	CT_MMDataImageEncoder*	ret=new (ELeave) CT_MMDataImageEncoder();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataImageEncoder::CT_MMDataImageEncoder()
/**
 * Protected constructor. First phase construction
 */
:	iFbsSessionConnected(EFalse)
,	iImageEncoder(NULL)
,	iActive(NULL)
,	iBitmap(NULL)
,	iBuffer(NULL)
,	iFileOpened(EFalse)
	{
	}

void CT_MMDataImageEncoder::ConstructL()
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

	iActive=CActiveCallback::NewL(*this);
	}

CT_MMDataImageEncoder::~CT_MMDataImageEncoder()
/**
 * Public destructor
 */
	{
	DestroyData();

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

TAny* CT_MMDataImageEncoder::GetObject()
	{
	return iImageEncoder;
	}

void CT_MMDataImageEncoder::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iImageEncoder	= static_cast<CImageEncoder*> (aObject);
	}

void CT_MMDataImageEncoder::DisownObjectL()
	{
	iImageEncoder = NULL;
	}

void CT_MMDataImageEncoder::DestroyData()
	{
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	iFs.Close();
	delete iBuffer;
	iBuffer=NULL;
	iImageTypeArray.Close();
	iSubTypeArray.Close();
	iFileExtensionArray.Close();
	delete iImageEncoder;
	iImageEncoder=NULL;
	}

CImageEncoder::TOptions CT_MMDataImageEncoder::ReadOptions(const TDesC& aSection)
/**
 ReadOptions - read in bitwise image encoder options
 @internalComponent
 @param aSection - main entry in .ini file
 @return CImageEncoder::TOptions
 @pre None
 @post 2nd stage construction complete
*/
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt								index=0;
	TBool								moreData=ETrue;
	TPtrC								optionString;
	TInt								options=CImageEncoder::EOptionNone;
	while ( moreData )
		{
		tempStore.Format(KOption(), ++index);
		if ( GetStringFromConfig(aSection, tempStore, optionString) )
			{
			CImageEncoder::TOptions	option;
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
	return CImageEncoder::TOptions(options);
	}

void CT_MMDataImageEncoder::ReadUids(const TDesC& aSection, TUid& aImageType, TUid& aImageSubType, TUid& aEncoderUid)
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

TBool CT_MMDataImageEncoder::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	else if ( aCommand==KCmdImplementationUid )
		{
		DoCmdImplementationUid();
		}
	else if ( aCommand==KCmdSetThumbnail )
		{
		DoCmdSetThumbnail(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataImageEncoder::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_MMDataImageEncoder::DoCmdCancel()
	{
	iImageEncoder->Cancel();
	}

void CT_MMDataImageEncoder::DoCmdConvertL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	delete iBitmap;
	iBitmap=NULL;
	iBitmap=new (ELeave) CFbsBitmap();

	TPtrC	bitmapFile;
	if ( GetStringFromConfig(aSection, KBitmapFile(), bitmapFile) )
		{
		TInt	err=iBitmap->Load(bitmapFile);
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CFbsBitmap::Load failed with error %d"), err);
			SetBlockResult(EFail);
			}
		}

	CFrameImageData*	frameImageData = NULL;

	TBool	useBmpDataType=EFalse;
	if ( GetBoolFromConfig(aSection, KUseBmpDataType(), useBmpDataType) )
		{
		frameImageData=CFrameImageData::NewL();
		TInt	imageDataSetting = 0;
		GetIntFromConfig(aSection, KDataTypeSetting(), imageDataSetting);

		if ( useBmpDataType )
			{
			TBmpImageData*	imageData = new(ELeave) TBmpImageData;
			imageData->iBitsPerPixel = imageDataSetting;
			frameImageData->AppendImageData(imageData);
			INFO_PRINTF2(_L("Using TBmpImageData (iBitsPerPixel=%d)"), imageDataSetting);
			}
		else
			{
			TJpegImageData*	imageData = new(ELeave) TJpegImageData;
			imageData->iSampleScheme = TJpegImageData::EColor444;
			imageData->iQualityFactor = imageDataSetting;
			frameImageData->AppendImageData(imageData);
			INFO_PRINTF2(_L("Using TJpegImageData (iQualityFactor=%d)"), imageDataSetting);
			}
		INFO_PRINTF1(_L("CImageEncoder::Convert(&(iActive->iStatus), iBitmap, frameImageData)"));
		}
	else
		{
		INFO_PRINTF1(_L("CImageEncoder::Convert(&(iActive->iStatus), iBitmap, NULL)"));
		}

	iImageEncoder->Convert(&(iActive->iStatus), *iBitmap, frameImageData);
	iActive->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}

void CT_MMDataImageEncoder::DoCmdDataNewL(const TDesC& aSection)
	{
	TPtrC	mime;
	TBool	hasMime=GetStringFromConfig(aSection, KMime(), mime);

	//	Read options
	CImageEncoder::TOptions	options=ReadOptions(aSection);

	//	Read Uids
	TUid	imageType;
	TUid	imageSubType;
	TUid	encoderUid;
	ReadUids(aSection, imageType, imageSubType, encoderUid);

	if ( EPass==BlockResult() )
		{
		HBufC8*	mime8=HBufC8::NewLC(mime.Length());
		mime8->Des().Copy(mime);

		delete iBuffer;
		iBuffer=NULL;
		if ( hasMime )
			{
			TRAPD(err, iImageEncoder=CImageEncoder::DataNewL(iBuffer, mime8->Des(), options));
			if ( err == KErrNone )
				{
				INFO_PRINTF1(_L("CImageEncoder::DataNewL(iBuffer, mime8->Des(), options)"));
				}
			else
				{
				ERR_PRINTF2(_L("CImageEncoder::DataNewL Error %d"), err);
				SetError(err);
				}
			}
		else
			{
			TRAPD(err, iImageEncoder=CImageEncoder::DataNewL(iBuffer, options, imageType, imageSubType, encoderUid));
			if ( err == KErrNone )
				{
				INFO_PRINTF1(_L("CImageEncoder::DataNewL(iBuffer, options, imageType, imageSubType, encoderUid)"));
				}
			else
				{
				ERR_PRINTF2(_L("CImageEncoder::DataNewL Error %d"), err);
				SetError(err);
				}
			}
		CleanupStack::PopAndDestroy(mime8);
		}
	}

void CT_MMDataImageEncoder::DoCmdFileNewL(const TDesC& aSection)
	{
	TBool	useRFile=EFalse;
	GetBoolFromConfig(aSection, KUseRFile(), useRFile);

	TBool	useClosedRFile=EFalse;
	GetBoolFromConfig(aSection, KUseClosedRFile(), useClosedRFile);

	TPtrC	filename;
	if ( !GetStringFromConfig(aSection, KFilename(), filename) )
		{
		ERR_PRINTF1(_L("No Filename"));
		SetBlockResult(EFail);
		}

	TPtrC	mime;
	TBool	hasMime=GetStringFromConfig(aSection, KMime(), mime);

	//	Read options
	CImageEncoder::TOptions	options=ReadOptions(aSection);

	//	Read Uids
	TUid	imageType;
	TUid	imageSubType;
	TUid	encoderUid;
	ReadUids(aSection, imageType, imageSubType, encoderUid);

	if ( EPass==BlockResult() )
		{
		HBufC8*	mime8=HBufC8::NewLC(mime.Length());
		mime8->Des().Copy(mime);
		if ( useRFile || useClosedRFile )
			{
			if ( iFileOpened )
				{
				iFile.Close();
				iFileOpened=EFalse;
				}
			if ( !useClosedRFile )
				{
				if ( iFile.Open(iFs, filename, EFileWrite)!=KErrNone )
					{
					User::LeaveIfError(iFile.Create(iFs, filename, EFileWrite));
					}
				iFileOpened=ETrue;
				}
			if ( hasMime )
				{
				TRAPD(err, iImageEncoder=CImageEncoder::FileNewL(iFile, mime8->Des(), options));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageEncoder::FileNewL(iFile, mime8->Des(), options)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageEncoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			else
				{
				TRAPD(err, iImageEncoder=CImageEncoder::FileNewL(iFile, options, imageType, imageSubType, encoderUid));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageEncoder::FileNewL(iFile, options, imageType, imageSubType, encoderUid)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageEncoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			}
		else
			{
			if ( hasMime )
				{
				TRAPD(err, iImageEncoder=CImageEncoder::FileNewL(iFs, filename, mime8->Des(), options));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageEncoder::FileNewL(iFs, filename, mime8->Des(), options)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageEncoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			else
				{
				TRAPD(err, iImageEncoder=CImageEncoder::FileNewL(iFs, filename, options, imageType, imageSubType, encoderUid));
				if ( err == KErrNone )
					{
					INFO_PRINTF1(_L("CImageEncoder::FileNewL(iFs, filename, options, imageType, imageSubType, encoderUid)"));
					}
				else
					{
					ERR_PRINTF2(_L("CImageEncoder::FileNewL Error %d"), err);
					SetError(err);
					}
				}
			}
		CleanupStack::PopAndDestroy(mime8);
		}
	}

void CT_MMDataImageEncoder::DoCmdGetFileTypesL(const TDesC& aSection)
	{
	iFileExtensionArray.ResetAndDestroy();
	TRAPD(err, CImageEncoder::GetFileTypesL(iFileExtensionArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageEncoder::GetFileTypesL Error %d"), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::VerifyMIMETypeArrayL(*this, aSection, iFileExtensionArray);
		}
	}

void CT_MMDataImageEncoder::DoCmdGetImageSubTypesL(const TDesC& aSection)
	{
	TInt	uid=0;
	GetHexFromConfig(aSection, KImageType(), uid);

	iSubTypeArray.ResetAndDestroy();
	TRAPD(err, CImageEncoder::GetImageSubTypesL(TUid::Uid(uid), iSubTypeArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageEncoder::GetImageSubTypesL Error %d"), err);
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

void CT_MMDataImageEncoder::DoCmdGetImageTypesL(const TDesC& aSection)
	{

	iImageTypeArray.ResetAndDestroy();
	TRAPD(err, CImageEncoder::GetImageTypesL(iImageTypeArray));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CImageEncoder::GetImageTypesL Error %d"), err);
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

void CT_MMDataImageEncoder::DoCmdImplementationUid()
	{
	TUid	uid=iImageEncoder->ImplementationUid();
	INFO_PRINTF2(_L("ImplementationUid Uid: 0x%X"), uid.iUid);
	}

void CT_MMDataImageEncoder::DoCmdSetThumbnail(const TDesC& aSection)
	{
	TBool	generateThumbnail=EFalse;
	GetBoolFromConfig(aSection, KThumbNail(), generateThumbnail);

	iImageEncoder->SetThumbnail(generateThumbnail);
	}

void CT_MMDataImageEncoder::RunL(CActive* aActive, const TInt aAsyncErrorIndex)
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
		RunActiveL(aAsyncErrorIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageEncoder::DoCancel(CActive* aActive, const TInt aAsyncErrorIndex)
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
		DoCancelActive(aAsyncErrorIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

void CT_MMDataImageEncoder::RunActiveL(const TInt aAsyncErrorIndex)
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
		SetAsyncError(aAsyncErrorIndex, err);
		}
	DecOutstanding();
	}


void CT_MMDataImageEncoder::DoCancelActive(const TInt /*aAsyncErrorIndex*/)
/**
 DoCancelActive - Process DoCancel for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
	{
	ERR_PRINTF1(_L("Canceled"));
	iImageEncoder->Cancel();
	}
