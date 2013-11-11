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



#include "T_MMDataVideoPlayerUtility.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KScreenRectax,					"screenrectax");
_LIT(KScreenRectay,					"screenrectay");
_LIT(KScreenRectbx,					"screenrectbx");
_LIT(KScreenRectby,					"screenrectby");
_LIT(KClipRectax,					"cliprectax");
_LIT(KClipRectay,					"cliprectay");
_LIT(KClipRectbx,					"cliprectbx");
_LIT(KClipRectby,					"cliprectby");
_LIT(KInvalidSession,				"invalidsession");
_LIT(KInvalidScreenDevice,			"invalidscreendevice");
_LIT(KInvalidWindow,				"invalidwindow");
_LIT(KFileName,						"filename");
_LIT(KVideoControllerUid,			"videocontrolleruid");
_LIT(KOpenFileType,					"openfiletype");
_LIT(KAccessPointID,				"accesspointid");
_LIT(KMimeType,						"mimetype");
_LIT(KUrl,							"url");
_LIT(KStartPoint,					"startpoint");
_LIT(KEndPoint,						"endpoint");
_LIT(KVideoLoadingProgress,			"videoloadingprogress");
_LIT(KDisplayMode,					"displaymode");
_LIT(KVideoFrameRate,				"videoframerate");
_LIT(KVideoFrameWidth,				"videoframewidth");
_LIT(KVideoFrameHeight,				"videoframeheight");
_LIT(KVideoFormat,					"videoformat");
_LIT(KVideoBitRate,					"videobitrate");
_LIT(KAudioBitRate,					"audiobitrate");
_LIT(KAudioEnabled,					"audioenabled");
_LIT(KPosition,						"position");
_LIT(KDuration,						"duration");
_LIT(KVolume,						"volume");
_LIT(KBalance,						"balance");
_LIT(KRotation,						"rotation");
_LIT(KWidthPercentage,				"widthpercentage");
_LIT(KHeightPercentage,				"heightpercentage");
_LIT(KAntiAliasing,					"antialiasing");
_LIT(KNrOfMetaDataEntries,			"nrofmetadataentries");
_LIT(KMetaDataEntryIndex,			"metadataentryindex");
_LIT(KFunction,						"function");
_LIT(KUseDataFrom,					"usedatafrom");
_LIT(KDataTo1,						"datato1");
_LIT(KDataTo2,						"datato2");
_LIT(KInvalidFileHandle,			"invalidfilehandle");
_LIT(KName,							"name");
_LIT(KValue,						"value");

_LIT(KCmdNewL,						"NewL");
_LIT(KCmdOpenFileL,					"OpenFileL");
_LIT(KCmdOpenDesL,					"OpenDesL");
_LIT(KCmdOpenUrlL,					"OpenUrlL");
_LIT(KCmdPrepare,					"Prepare");
_LIT(KCmdClose,						"Close");
_LIT(KCmdPlay,						"Play");
_LIT(KCmdStop,						"Stop");
_LIT(KCmdPauseL,					"PauseL");
_LIT(KCmdSetPriorityL,				"SetPriorityL");
_LIT(KCmdPriorityL,					"PriorityL");
_LIT(KCmdSetDisplayWindowL,			"SetDisplayWindowL");
_LIT(KCmdRegLoadNotification,		"RegisterForVideoLoadingNotification");
_LIT(KCmdGetVideoLoadingProgressL,	"GetVideoLoadingProgressL");
_LIT(KCmdGetFrameL,					"GetFrameL");
_LIT(KCmdRefreshFrameL,				"RefreshFrameL");
_LIT(KCmdVideoFrameRateL,			"VideoFrameRateL");
_LIT(KCmdSetVideoFrameRateL,		"SetVideoFrameRateL");
_LIT(KCmdVideoFrameSizeL,			"VideoFrameSizeL");
_LIT(KCmdVideoFormatMimeType,		"VideoFormatMimeType");
_LIT(KCmdVideoBitRateL,				"VideoBitRateL");
_LIT(KCmdAudioBitRateL,				"AudioBitRateL");
_LIT(KCmdAudioTypeL,				"AudioTypeL");
_LIT(KCmdAudioEnabledL,				"AudioEnabledL");
_LIT(KCmdSetPositionL,				"SetPositionL");
_LIT(KCmdPositionL,					"PositionL");
_LIT(KCmdDurationL,					"DurationL");
_LIT(KCmdSetVolumeL,				"SetVolumeL");
_LIT(KCmdVolume,					"Volume");
_LIT(KCmdMaxVolume,					"MaxVolume");
_LIT(KCmdSetBalanceL,				"SetBalanceL");
_LIT(KCmdBalance,					"Balance");
_LIT(KCmdSetRotationL,				"SetRotationL");
_LIT(KCmdRotationL,					"RotationL");
_LIT(KCmdSetScaleFactorL,			"SetScaleFactorL");
_LIT(KCmdGetScaleFactorL,			"GetScaleFactorL");
_LIT(KCmdSetCropRegionL,			"SetCropRegionL");
_LIT(KCmdGetCropRegionL,			"GetCropRegionL");
_LIT(KCmdNumberOfMetaDataEntriesL,	"NumberOfMetaDataEntriesL");
_LIT(KCmdMetaDataEntryL,			"MetaDataEntryL");
_LIT(KCmdControllerImplInfoL,		"ControllerImplementationInformationL");
_LIT(KCmdCustomCommandSync,			"CustomCommandSync");
_LIT(KCmdCustomCommandASync,		"CustomCommandAsync");
_LIT(KCmdGetDRMCustomCommand,		"GetDRMCustomCommand");
_LIT(KCmdStopDirectScreenAccessL,	"StopDirectScreenAccessL");
_LIT(KCmdStartDirectScreenAccessL,	"StartDirectScreenAccessL");
_LIT(KCmdDestructor,				"~");

_LIT(KTDesC,						"TDesC");
_LIT(KRFile,						"RFile");
_LIT(KTMMSource,					"TMMSource");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
Two phase constructor
@internalComponent
@return Data Dictionary object
@pre None
@post None
*/
CT_MMDataVideoPlayerUtility* CT_MMDataVideoPlayerUtility::NewL()
	{
	CT_MMDataVideoPlayerUtility*	ret = new (ELeave) CT_MMDataVideoPlayerUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
CT_MMDataVideoPlayerUtility constructor.
@internalComponent
@return N/A
@pre None
@post CT_MMDataVideoPlayerUtility 1st stage constructor
*/
CT_MMDataVideoPlayerUtility::CT_MMDataVideoPlayerUtility()
:	iVideoPlayerUtility(NULL)
,	iApiAsyncErrorIndex(0)
,	iWsSessionConnected(EFalse)
,	iScrDevice(NULL)
,	iInvalidScrDevice(NULL)
,	iWindowGroup(NULL)
,	iWindow(NULL)
,	iInvalidWindow(NULL)
,	iActiveCustomCommandAsync(NULL)
,	iFileOpened(EFalse)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
	{
	}

void CT_MMDataVideoPlayerUtility::ConstructL()
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

	User::LeaveIfError(iWsSession.Connect());
	iWsSessionConnected = ETrue;

	iScrDevice = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScrDevice->Construct());

	iInvalidScrDevice = new (ELeave) CWsScreenDevice(iWsSession);

	iWindowGroup = new (ELeave) RWindowGroup(iWsSession);
	User::LeaveIfError(iWindowGroup->Construct(reinterpret_cast<TUint32>(iWindowGroup), EFalse));

	iWindow = new (ELeave) RWindow(iWsSession);
	User::LeaveIfError(iWindow->Construct(*iWindowGroup, reinterpret_cast<TUint32>(iWindowGroup+1)));
    iWindow->SetExtent(TPoint(0,0), TSize(100,100));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    iWsSession.Flush();

	iInvalidWindow = new (ELeave) RWindow(iWsSession);
	iActiveCustomCommandAsync = CActiveCallback::NewL(*this);
	}

/**
CT_MMDataVideoPlayerUtility destructor
@internalComponent
@return N/A
@pre None
@post releases CT_MMDataVideoPlayerUtility heap memory
*/
CT_MMDataVideoPlayerUtility::~CT_MMDataVideoPlayerUtility()
	{
	DestroyData();

	if ( iWindow != NULL )
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}

	if ( iInvalidWindow != NULL )
		{
		delete iInvalidWindow;
		iInvalidWindow = NULL;
		}

	if ( iWindowGroup != NULL )
		{
		iWindowGroup->Close();
		delete iWindowGroup;
		}

	delete iScrDevice;
	iScrDevice = NULL;

	delete iInvalidScrDevice;
	iInvalidScrDevice = NULL;

	if ( iWsSessionConnected )
		{
		iWsSession.Close();
		iWsSessionConnected=EFalse;
		}

	iFs.Close();

	delete iActiveCustomCommandAsync;
	iActiveCustomCommandAsync = NULL;
	}

TAny* CT_MMDataVideoPlayerUtility::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return pointer to the object that the data wraps
 */
	{
	return iVideoPlayerUtility;
	}

void CT_MMDataVideoPlayerUtility::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iVideoPlayerUtility	= static_cast<CVideoPlayerUtility*> (aObject);
	}

void CT_MMDataVideoPlayerUtility::DisownObjectL()
	{
	iVideoPlayerUtility = NULL;
	}

/**
Releases CT_MMDataVideoPlayerUtility dynamic memory.
@internalComponent
@return void
@pre None
@post All dynamic memory deallocated
*/
void CT_MMDataVideoPlayerUtility::DestroyData()
	{
	delete iVideoPlayerUtility;
	iVideoPlayerUtility = NULL;

	if (iFileOpened)
		{
		iFile.Close();
		iFileOpened = EFalse;
		}

	delete iBuffer;
	iBuffer	= NULL;

	delete iBufferPtr;
	iBufferPtr	= NULL;
	}

TBool CT_MMDataVideoPlayerUtility::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if (aCommand == KCmdNewL)
		{
		DoCmdConstructor(aSection);
		}
	else if(aCommand == KCmdOpenFileL)
		{
		DoCmdOpenFileL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdOpenDesL)
		{
		DoCmdOpenDesL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdOpenUrlL)
		{
		DoCmdOpenUrlL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdPrepare)
		{
		DoCmdPrepare(aAsyncErrorIndex);
		}
	else if(aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else if(aCommand == KCmdPlay)
		{
		DoCmdPlay(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdStop)
		{
		DoCmdStop();
		}
	else if(aCommand == KCmdPauseL)
		{
		DoCmdPauseL();
		}
	else if(aCommand == KCmdSetPriorityL)
		{
		DoCmdSetPriorityL(aSection);
		}
	else if(aCommand == KCmdPriorityL)
		{
		DoCmdPriorityL(aSection);
		}
	else if(aCommand == KCmdSetDisplayWindowL)
		{
		DoCmdSetDisplayWindowL(aSection);
		}
	else if(aCommand == KCmdRegLoadNotification)
		{
		DoCmdRegisterForVideoLoadingNotification();
		}
	else if(aCommand == KCmdGetVideoLoadingProgressL)
		{
		DoCmdGetVideoLoadingProgressL(aSection);
		}
	else if(aCommand == KCmdGetFrameL)
		{
		DoCmdGetFrameL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdRefreshFrameL)
		{
		DoCmdRefreshFrameL();
		}
	else if(aCommand == KCmdVideoFrameRateL)
		{
		DoCmdVideoFrameRateL(aSection);
		}
	else if(aCommand == KCmdSetVideoFrameRateL)
		{
		DoCmdSetVideoFrameRateL(aSection);
		}
	else if(aCommand == KCmdVideoFrameSizeL)
		{
		DoCmdVideoFrameSizeL(aSection);
		}
	else if(aCommand == KCmdVideoFormatMimeType)
		{
		DoCmdVideoFormatMimeTypeL(aSection);
		}
	else if(aCommand == KCmdVideoBitRateL)
		{
		DoCmdVideoBitRateL(aSection);
		}
	else if(aCommand == KCmdAudioBitRateL)
		{
		DoCmdAudioBitRateL(aSection);
		}
	else if(aCommand == KCmdAudioTypeL)
		{
		DoCmdAudioTypeL(aSection);
		}
	else if(aCommand == KCmdAudioEnabledL)
		{
		DoCmdAudioEnabledL(aSection);
		}
	else if(aCommand == KCmdSetPositionL)
		{
		DoCmdSetPositionL(aSection);
		}
	else if(aCommand == KCmdPositionL)
		{
		DoCmdPositionL(aSection);
		}
	else if(aCommand == KCmdDurationL)
		{
		DoCmdDurationL(aSection);
		}
	else if(aCommand == KCmdSetVolumeL)
		{
		DoCmdSetVolumeL(aSection);
		}
	else if(aCommand == KCmdVolume)
		{
		DoCmdVolume(aSection);
		}
	else if(aCommand == KCmdMaxVolume)
		{
		DoCmdMaxVolume(aSection);
		}
	else if(aCommand == KCmdSetBalanceL)
		{
		DoCmdSetBalanceL(aSection);
		}
	else if(aCommand == KCmdBalance)
		{
		DoCmdBalance(aSection);
		}
	else if(aCommand == KCmdSetRotationL)
		{
		DoCmdSetRotationL(aSection);
		}
	else if(aCommand == KCmdRotationL)
		{
		DoCmdRotationL(aSection);
		}
	else if(aCommand == KCmdSetScaleFactorL)
		{
		DoCmdSetScaleFactorL(aSection);
		}
	else if(aCommand == KCmdGetScaleFactorL)
		{
		DoCmdGetScaleFactorL(aSection);
		}
	else if(aCommand == KCmdSetCropRegionL)
		{
		DoCmdSetCropRegionL(aSection);
		}
	else if(aCommand == KCmdGetCropRegionL)
		{
		DoCmdGetCropRegionL(aSection);
		}
	else if(aCommand == KCmdNumberOfMetaDataEntriesL)
		{
		DoCmdNumberOfMetaDataEntriesL(aSection);
		}
	else if(aCommand == KCmdMetaDataEntryL)
		{
		DoCmdMetaDataEntryL(aSection);
		}
	else if(aCommand == KCmdControllerImplInfoL)
		{
		DoCmdControllerImplementationInformationL();
		}
	else if(aCommand == KCmdCustomCommandSync)
		{
		DoCmdCustomCommandSyncL(aSection);
		}
	else if(aCommand == KCmdCustomCommandASync)
		{
		DoCmdCustomCommandASyncL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdGetDRMCustomCommand)
		{
		DoCmdGetDRMCustomCommand();
		}
	else if(aCommand == KCmdStopDirectScreenAccessL)
		{
		DoCmdStopDirectScreenAccessL();
		}
	else if(aCommand == KCmdStartDirectScreenAccessL)
		{
		DoCmdStartDirectScreenAccessL();
		}
	else if(aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_MMDataVideoPlayerUtility::DoCmdConstructor(const TDesC& aSection)
	{
	// Read priority from INI file
	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	// Read priorityPreference from INI file
	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceTimeAndQuality;
		}

	// Read session parameter from INI file
	TBool	invalidSession;
	if ( !GetBoolFromConfig(aSection, KInvalidSession(), invalidSession) )
		{
		invalidSession = EFalse;
		}

	RWsSession*	wSession = NULL;
	if (!invalidSession)
		{
		wSession = &iWsSession;
		}
	else
		{
		wSession = &iInvalidWsSession;
		}

	// Read screenDevice parameter from INI file
	TBool	invalidScrDevice = EFalse;
	GetBoolFromConfig(aSection, KInvalidScreenDevice(), invalidScrDevice);

	CWsScreenDevice*	scrDevice = iScrDevice;
	if (invalidScrDevice)
		{
		scrDevice = iInvalidScrDevice;
		}

	// Read window parameter from INI file
	TBool	invalidWindow = EFalse;
	GetBoolFromConfig(aSection, KInvalidWindow(), invalidWindow);

	RWindow*	window = iWindow;
	if (invalidWindow)
		{
		window = iInvalidWindow;
		}

	// Temporary coordinate variables
	TInt	ax = 0;
	TInt	ay = 0;
	TInt	bx = 0;
	TInt	by = 0;

	// Read screenRectangle coordinates from INI file
	GetIntFromConfig(aSection, KScreenRectax(), ax);
	GetIntFromConfig(aSection, KScreenRectay(), ay);
	GetIntFromConfig(aSection, KScreenRectbx(), bx);
	GetIntFromConfig(aSection, KScreenRectby(), by);

	TRect	screenRect(ax, ay, bx, by);

	ax = 0;
	ay = 0;
	bx = 0;
	by = 0;

	// Read clipRectangle coordinates from INI file
	GetIntFromConfig(aSection, KClipRectax(), ax);
	GetIntFromConfig(aSection, KClipRectay(), ay);
	GetIntFromConfig(aSection, KClipRectbx(), bx);
	GetIntFromConfig(aSection, KClipRectby(), by);

	TRect	clipRect(ax, ay, bx, by);

	TRAPD(err, iVideoPlayerUtility = CVideoPlayerUtility::NewL(*this, priority, priorityPreference, *wSession, *scrDevice, *window, screenRect, clipRect));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::NewL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdOpenFileL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;

	// Read filename from INI file
	TPtrC filename;
	if (!GetStringFromConfig(aSection, KFileName(), filename))
		{
		ERR_PRINTF2(_L("Can not call OpenFileL. Parameter %S is missing in INI file."), &KFileName());
		SetBlockResult(EFail);
		}
	else
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeVideo, EFalse, iVideoControllerUid, iVideoFormatUid);
		}

	// Use default video controller Uid unless one is passed in
	TInt	videoControllerId;
	if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
		{
		iVideoControllerUid=TUid::Uid(videoControllerId);
		}

	// Read Open type (TDesC, RFile, TMMSource) from INI file
	TPtrC 	openFileType = KTDesC();
	GetStringFromConfig(aSection, KOpenFileType(), openFileType);

	if (openFileType == KRFile())
		{
		if (iFileOpened)
			{
			iFile.Close();
			iFileOpened = EFalse;
			}
		TBool	invalidFileHandle = EFalse;
		GetBoolFromConfig(aSection, KInvalidFileHandle(), invalidFileHandle);
		if (!invalidFileHandle)
			{
			User::LeaveIfError(iFile.Open(iFs, filename, EFileShareReadersOnly));
			iFileOpened = ETrue;
			}
		TRAPD(err, iVideoPlayerUtility->OpenFileL(iFile, iVideoControllerUid));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CVideoPlayerUtility::OpenFileL failed with error %d."), err);
			SetError(err);
			}
		else
			{
			IncOutstanding();
			}
		}
	else if (openFileType == KTMMSource())
		{
		TMMFileSource	videoSource(filename);
		TRAPD(err, iVideoPlayerUtility->OpenFileL(videoSource, iVideoControllerUid));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("CVideoPlayerUtility::OpenFileL failed with error %d."), err);
			SetError(err);
			}
		else
			{
			IncOutstanding();
			}
		}
	else
		{
		TRAPD(err, iVideoPlayerUtility->OpenFileL(filename, iVideoControllerUid));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CVideoPlayerUtility::OpenFileL failed with error %d."), err);
			SetError(err);
			}
		else
			{
			IncOutstanding();
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdOpenDesL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;

	// Read filename from INI file
	TPtrC	filename;
	if (!GetStringFromConfig(aSection, KFileName(), filename))
		{
		ERR_PRINTF2(_L("Can not call OpenFileL. Parameter %S is missing in INI file."), &KFileName());
		SetBlockResult(EFail);
		}
	CT_MMUtil::GetControllerAndFormatUidL(*this, filename, KUidMediaTypeVideo, EFalse, iVideoControllerUid, iVideoFormatUid);

	// Use default video controller Uid unless one is passed in
	TInt	videoControllerId;
	if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
		{
		iVideoControllerUid=TUid::Uid(videoControllerId);
		}

	if ( iFileOpened )
		{
   		iFile.Close();
   		iFileOpened=EFalse;
   		}

	User::LeaveIfError(iFile.Open(iFs, filename, EFileShareReadersOnly));
	iFileOpened = ETrue;
	TInt	size = 0;
	User::LeaveIfError(iFile.Size(size));

    delete iBuffer;
    iBuffer = HBufC8::NewL(size);
    delete iBufferPtr;
    iBufferPtr = new (ELeave) TPtr8(iBuffer->Des());
    User::LeaveIfError(iFile.Read(*iBufferPtr));

	TRAPD(err, iVideoPlayerUtility->OpenDesL(*iBufferPtr, iVideoControllerUid));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::OpenDesL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdOpenUrlL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;

	// Read url from INI file
	TPtrC	url;
	if (!GetStringFromConfig(aSection, KUrl(), url))
		{
		ERR_PRINTF2(_L("Can not call OpenFileL. Parameter %S is missing in INI file."), &KFileName());
		SetBlockResult(EFail);
		}
	else
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, url, KUidMediaTypeVideo, EFalse, iVideoControllerUid, iVideoFormatUid);
		}

	// Read accesspoint ID from INI file
	TInt	accessPointID = KUseDefaultIap;
	GetIntFromConfig(aSection, KAccessPointID(), accessPointID);

	// Read MIME type from INI file
	HBufC8*	mimeType = NULL;
	TPtrC	mimeTypeStr;
	if (GetStringFromConfig(aSection, KMimeType(), mimeTypeStr))
		{
		mimeType = HBufC8::NewLC(mimeTypeStr.Length());
		mimeType->Des().Copy(mimeTypeStr);
		}
	else
		{
		mimeType = HBufC8::NewLC(0);
		mimeType->Des().Copy(KNullDesC8);
		}

	// Use default video controller Uid unless one is passed in
	TInt	videoControllerId;
	if ( GetHexFromConfig(aSection, KVideoControllerUid(), videoControllerId) )
		{
		iVideoControllerUid=TUid::Uid(videoControllerId);
		}

	TRAPD(err, iVideoPlayerUtility->OpenUrlL(url, accessPointID, *mimeType, iVideoControllerUid));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::OpenUrlL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		IncOutstanding();
		}

	CleanupStack::PopAndDestroy(1, mimeType);
	}

void CT_MMDataVideoPlayerUtility::DoCmdPrepare(TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	iVideoPlayerUtility->Prepare();
	IncOutstanding();
	}

void CT_MMDataVideoPlayerUtility::DoCmdClose()
	{
	iVideoPlayerUtility->Close();
	}

void CT_MMDataVideoPlayerUtility::DoCmdPlay(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;

	// Read the point at which to start playback from INI file
	TTimeIntervalMicroSeconds 	startPoint = 0;
	TInt	tmpStartPoint;
	if (GetIntFromConfig(aSection, KStartPoint(), tmpStartPoint))
		{
		startPoint = (TInt64)tmpStartPoint;
		}

	// Read the point at which to terminate playback from INI file
	TTimeIntervalMicroSeconds 	endPoint = 0;
	TInt	tmpEndPoint;
	if (GetIntFromConfig(aSection, KEndPoint(), tmpEndPoint))
		{
		endPoint = (TInt64)tmpEndPoint;
		iVideoPlayerUtility->Play(startPoint, endPoint);
		}
	else
		{
		iVideoPlayerUtility->Play();
		}

	IncOutstanding();
	}

void CT_MMDataVideoPlayerUtility::DoCmdStop()
	{
	TInt 	err = iVideoPlayerUtility->Stop();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::Stop failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdPauseL()
	{
	TRAPD(err, iVideoPlayerUtility->PauseL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::PauseL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetPriorityL(const TDesC& aSection)
	{
	// Read priority from INI file
	TInt 	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority = EMdaPriorityNormal;
		ERR_PRINTF1(_L("Can not call SetPriorityL. Parameter is missing in INI file."));
		SetBlockResult(EFail);
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceTimeAndQuality;
		ERR_PRINTF1(_L("Can not call SetPriorityL. Parameter is missing in INI file."));
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetPriorityL(priority, priorityPreference));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetPriorityL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdPriorityL(const TDesC& aSection)
	{
	TInt					priority;
	TMdaPriorityPreference	priorityPref;
	TRAPD(err, iVideoPlayerUtility->PriorityL(priority, priorityPref));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::PriorityL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("Playback priority (%d) time and quality preference (%d)"), priority, priorityPref);

		// Read expected priority from INI file
		TInt	expectedPriority;
		if ( CT_MMUtil::ReadMdaPriority(*this, aSection, expectedPriority) )
			{
			if ( priority != expectedPriority )
				{
				ERR_PRINTF3(_L("Playback priority actual(%d)!=expected(%d)"), priority, expectedPriority);
				SetBlockResult(EFail);
				}
			}

		// Read expected priorityPreference from INI file
		TMdaPriorityPreference 	expectedPriorityPref;
		if ( CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, expectedPriorityPref) )
			{
			if (priorityPref != expectedPriorityPref)
				{
				ERR_PRINTF3(_L("Playback time and quality preference actual(%d)!=expected(%d)"), priorityPref, expectedPriorityPref);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetDisplayWindowL(const TDesC& aSection)
	{
	// Read session parameter from INI file
	TBool	invalidSession = EFalse;
	GetBoolFromConfig(aSection, KInvalidSession(), invalidSession);
	RWsSession*	wSession = NULL;
	if (!invalidSession)
		{
		wSession = &iWsSession;
		}
	else
		{
		wSession = &iInvalidWsSession;
		}

	// Read screenDevice parameter from INI file
	TBool	invalidScrDevice = EFalse;
	GetBoolFromConfig(aSection, KInvalidScreenDevice(), invalidScrDevice);
	CWsScreenDevice*	scrDevice = iScrDevice;
	if (invalidScrDevice)
		{
		scrDevice = iInvalidScrDevice;
		}

	// Read window parameter from INI file
	TBool	invalidWindow = EFalse;
	GetBoolFromConfig(aSection, KInvalidWindow(), invalidWindow);
	RWindow*	window = iWindow;
	if (invalidWindow)
		{
		window = iInvalidWindow;
		}

	// Temporary coordinate variables
	TInt	ax = 0;
	TInt	ay = 0;
	TInt	bx = 0;
	TInt	by = 0;

	// Read screenRectangle coordinates from INI file
	GetIntFromConfig(aSection, KScreenRectax(), ax);
	GetIntFromConfig(aSection, KScreenRectay(), ay);
	GetIntFromConfig(aSection, KScreenRectbx(), bx);
	GetIntFromConfig(aSection, KScreenRectby(), by);
	TRect	screenRect(ax, ay, bx, by);

	ax = 0;
	ay = 0;
	bx = 0;
	by = 0;

	// Read clipRectangle coordinates from INI file
	GetIntFromConfig(aSection, KClipRectax(), ax);
	GetIntFromConfig(aSection, KClipRectay(), ay);
	GetIntFromConfig(aSection, KClipRectbx(), bx);
	GetIntFromConfig(aSection, KClipRectby(), by);
	TRect	clipRect(ax, ay, bx, by);

	TRAPD(err, iVideoPlayerUtility->SetDisplayWindowL(*wSession, *scrDevice, *window, screenRect, clipRect));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetDisplayWindowL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdRegisterForVideoLoadingNotification()
	{
	iVideoPlayerUtility->RegisterForVideoLoadingNotification(*this);
	}

void CT_MMDataVideoPlayerUtility::DoCmdGetVideoLoadingProgressL(const TDesC& aSection)
	{
	TInt	videoLoadingProg = 0;
	TRAPD(err, iVideoPlayerUtility->GetVideoLoadingProgressL(videoLoadingProg));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::GetVideoLoadingProgressL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		// Read expected VideoLoadingProgress from INI file
		TInt 	expectedVidoLoadingProg = 0;
		if (GetIntFromConfig(aSection, KVideoLoadingProgress(), expectedVidoLoadingProg))
			{
			if (videoLoadingProg != expectedVidoLoadingProg)
				{
				ERR_PRINTF3(_L("Video loading progress actual(%d)!=expected(%d)"),
					videoLoadingProg, expectedVidoLoadingProg);
				SetBlockResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Video loading progress: %d"), videoLoadingProg);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdGetFrameL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	iApiAsyncErrorIndex = aAsyncErrorIndex;
	// Read displaymode from INI file
	TPtrC 	displayModeStr;
	TDisplayMode 	displayMode = ENone;
	if (GetStringFromConfig(aSection, KDisplayMode(), displayModeStr))
	    {
	    if (!CT_MMUtil::ConvertToDisplayMode(displayModeStr, displayMode))
	        {
	        //Read INT for negative tests
	        TInt	invalidDisplayMode = 0;
	        if (!GetIntFromConfig(aSection, KDisplayMode(), invalidDisplayMode))
	            {
	            ERR_PRINTF2(_L("Can not call GetFrameL. Parameter %S has wrong value."), &KDisplayMode());
	            SetBlockResult(EFail);
	            }
	        displayMode = (TDisplayMode)invalidDisplayMode;
	        }
	    }
	else
		{
		ERR_PRINTF2(_L("Can not call GetFrameL. Parameter %S is missing in INI file or has wrong value."), &KDisplayMode());
		SetBlockResult(EFail);
		}

	// Read intent from INI file
    ContentAccess::TIntent	intent=ContentAccess::EUnknown;
	if ( CT_MMUtil::ReadIntent(*this, aSection, intent) )
		{
		TRAPD(err, iVideoPlayerUtility->GetFrameL(displayMode, intent));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CVideoPlayerUtility::GetFrameL failed with error %d."), err);
			SetError(err);
			}
		else
			{
			IncOutstanding();
			}
		}
	else
		{
		TRAPD(err, iVideoPlayerUtility->GetFrameL(displayMode));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("CVideoPlayerUtility::GetFrameL failed with error %d."), err);
			SetError(err);
			}
		else
			{
			IncOutstanding();
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdRefreshFrameL()
	{
	TRAPD(err, iVideoPlayerUtility->RefreshFrameL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::RefreshFrameL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdVideoFrameRateL(const TDesC& aSection)
	{
	TReal32	videoFrameRate = 0;
	TRAPD(err, videoFrameRate = iVideoPlayerUtility->VideoFrameRateL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::VideoFrameRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Video frame rate (%8.2f)"), videoFrameRate);

		// Read expected VideoFrameRate from INI file
		TInt 	expectedVideoFrameRate = 0;
		if (GetIntFromConfig(aSection, KVideoFrameRate(), expectedVideoFrameRate) &&
			(videoFrameRate != (TReal32)expectedVideoFrameRate))
			{
			ERR_PRINTF3(_L("Video frame rate actual(%8.2f) != expected(%d)"),
				videoFrameRate, expectedVideoFrameRate);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetVideoFrameRateL(const TDesC& aSection)
	{
	// Read VideoFrameRate from INI file
	TInt 	videoFrameRate = 0;
	if (!GetIntFromConfig(aSection, KVideoFrameRate(), videoFrameRate))
		{
		ERR_PRINTF2(_L("Can not call SetVideoFrameRateL. Parameter %S is missing in INI file."), &KVideoFrameRate());
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetVideoFrameRateL(videoFrameRate));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetVideoFrameRateL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdVideoFrameSizeL(const TDesC& aSection)
	{
	TSize 	videoFrameSize;
	TRAPD(err, iVideoPlayerUtility->VideoFrameSizeL(videoFrameSize));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::VideoFrameSizeL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("Video frame size width (%d) and height (%d)"),
			videoFrameSize.iWidth, videoFrameSize.iHeight);

		// Read expected VideoFrameWidth from INI file
		TInt 	expectedVideoFrameWidth = 0;
		if (GetIntFromConfig(aSection, KVideoFrameWidth(), expectedVideoFrameWidth) &&
			(videoFrameSize.iWidth != expectedVideoFrameWidth))
			{
			ERR_PRINTF3(_L("Video frame size width actual (%d) != expected(%d)"),
				videoFrameSize.iWidth, expectedVideoFrameWidth);
			SetBlockResult(EFail);
			}

		// Read expected VideoFrameHeight from INI file
		TInt 	expectedVideoFrameHeight = 0;
		if (GetIntFromConfig(aSection, KVideoFrameHeight(), expectedVideoFrameHeight) &&
			(videoFrameSize.iHeight != expectedVideoFrameHeight))
			{
			ERR_PRINTF3(_L("Video frame size height actual (%d) != expected(%d)"),
				videoFrameSize.iHeight, expectedVideoFrameHeight);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdVideoFormatMimeTypeL(const TDesC& aSection)
	{
	TPtrC8	videoFormat = iVideoPlayerUtility->VideoFormatMimeType();
	HBufC*	mime = HBufC::NewLC(videoFormat.Length());
	TPtr	mimeDes = mime->Des();
	mimeDes.Copy(videoFormat);
	INFO_PRINTF2(_L("VideoFormatMimeType = %S"), &mimeDes);

	TPtrC	expectedMimeType;
	if(GetStringFromConfig(aSection, KVideoFormat(), expectedMimeType) &&
		(mimeDes != expectedMimeType))
		{
		ERR_PRINTF3(_L("Video format actual (%S) != expected (%S)"), &mimeDes, &expectedMimeType);
		SetBlockResult(EFail);
		}

	CleanupStack::PopAndDestroy(mime);
	}

void CT_MMDataVideoPlayerUtility::DoCmdVideoBitRateL(const TDesC& aSection)
	{
	TInt	videoBitRate = 0;
	TRAPD(err, videoBitRate = iVideoPlayerUtility->VideoBitRateL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::VideoBitRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Video bit rate (%d)"), videoBitRate);

		// Read expected VideoBitRate from INI file
		TInt 	expectedVideoBitRate = 0;
		if (GetIntFromConfig(aSection, KVideoBitRate(), expectedVideoBitRate) &&
			(videoBitRate != expectedVideoBitRate))
			{
			ERR_PRINTF3(_L("Video bit rate actual(%d) != expected(%d)"),
				videoBitRate, expectedVideoBitRate);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdAudioBitRateL(const TDesC& aSection)
	{
	TInt	audioBitRate = 0;
	TRAPD(err, audioBitRate = iVideoPlayerUtility->AudioBitRateL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::AudioBitRateL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Audio bit rate (%d)"), audioBitRate);

		// Read expected AudioBitRate from INI file
		TInt 	expectedAudioBitRate = 0;
		if (GetIntFromConfig(aSection, KAudioBitRate(), expectedAudioBitRate) &&
			(audioBitRate != expectedAudioBitRate))
			{
			ERR_PRINTF3(_L("Audio bit rate actual(%d) != expected(%d)"),
				audioBitRate, expectedAudioBitRate);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdAudioTypeL(const TDesC& aSection)
	{
	TFourCC	audioType;
	TRAPD(err, audioType = iVideoPlayerUtility->AudioTypeL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::AudioTypeL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Audio type (%d)"), audioType.FourCC());

		// Read expected AudioType from INI file
		TFourCC	expectedAudioType;
		if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedAudioType) )
			{
			if (expectedAudioType != audioType.FourCC())
				{
				ERR_PRINTF3(_L("Audio type actual(%d) != expected(%d)"), audioType.FourCC(), expectedAudioType.FourCC());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdAudioEnabledL(const TDesC& aSection)
	{
	TBool	audioEnabled = EFalse;
	TRAPD(err, audioEnabled = iVideoPlayerUtility->AudioEnabledL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::AudioEnabledL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Audio enabled = (%d)"), audioEnabled);

		// Read expected AudioEnabled from INI file
		TBool 	expectedAudioEnabled;
		if ((GetBoolFromConfig(aSection, KAudioEnabled(), expectedAudioEnabled)) &&
			(expectedAudioEnabled != audioEnabled))
			{
			ERR_PRINTF3(_L("Audio enabled actual(%d) != expected(%d)"),
				audioEnabled, expectedAudioEnabled);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetPositionL(const TDesC& aSection)
	{
	// Read Position from INI file
	TTimeIntervalMicroSeconds	position = 0;
	TInt	pos = 0;
	if (!GetIntFromConfig(aSection, KPosition(), pos))
		{
		ERR_PRINTF2(_L("Can not call SetPositionL. Parameter %S is missing in INI file."), &KPosition());
		SetBlockResult(EFail);
		}
	else
		{
		position = (TTimeIntervalMicroSeconds)pos;
		}

	TRAPD(err, iVideoPlayerUtility->SetPositionL(position));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetPositionL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdPositionL(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds	position = 0;
	TRAPD(err, position = iVideoPlayerUtility->PositionL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::PositionL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Current playback position = (%LD)"), position.Int64());

		// Read expected Position from INI file
		TInt	expectedPosition;
		if ((GetIntFromConfig(aSection, KPosition(), expectedPosition)) &&
			((TInt64)expectedPosition != position.Int64()))
			{
			ERR_PRINTF3(_L("Playback position actual(%LD) != expected(%d)"),
				position.Int64(), expectedPosition);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdDurationL(const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds	duration = 0;
	TRAPD(err, duration = iVideoPlayerUtility->DurationL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::DurationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("The duration of video clip = (%LD)"), duration.Int64());

		// Read expected Duration from INI file
		TInt	expectedDuration;
		if ((GetIntFromConfig(aSection, KDuration(), expectedDuration)) &&
			((TInt64)expectedDuration != duration.Int64()))
			{
			ERR_PRINTF3(_L("The duration of video clip actual(%LD) != expected(%d)"),
				duration.Int64(), expectedDuration);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetVolumeL(const TDesC& aSection)
	{
	// Read Volume from INI file
	TInt	volume = 0;
	if (!GetIntFromConfig(aSection, KVolume(), volume))
		{
		ERR_PRINTF2(_L("Can not call SetVolumeL. Parameter %S is missing in INI file."), &KVolume());
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetVolumeL(volume));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetVolumeL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdVolume(const TDesC& aSection)
	{
	TInt	volume = iVideoPlayerUtility->Volume();
	INFO_PRINTF2(_L("Playback volume  = (%d)"), volume);

	// Read expected Volume from INI file
	TInt	expectedVolume;
	if ((GetIntFromConfig(aSection, KVolume(), expectedVolume)) &&
		(expectedVolume != volume))
		{
		ERR_PRINTF3(_L("Playback volume actual(%d) != expected(%d)"),
			volume, expectedVolume);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdMaxVolume(const TDesC& aSection)
	{
	TInt	volume = iVideoPlayerUtility->MaxVolume();
	INFO_PRINTF2(_L("Playback Max volume  = (%d)"), volume);

	// Read expected Max Volume from INI file
	TInt	expectedMaxVolume;
	if ((GetIntFromConfig(aSection, KVolume(), expectedMaxVolume)) &&
		(expectedMaxVolume != volume))
		{
		ERR_PRINTF3(_L("Playback Max volume actual(%d) != expected(%d)"),
			volume, expectedMaxVolume);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetBalanceL(const TDesC& aSection)
	{
	// Read Balance from INI file
	TInt	balance = KMMFBalanceCenter;
	if (!GetIntFromConfig(aSection, KBalance(), balance))
		{
		ERR_PRINTF2(_L("Can not call SetBalanceL. Parameter %S is missing in INI file."), &KBalance());
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetBalanceL(balance));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetBalanceL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdBalance(const TDesC& aSection)
	{
	TInt	balance = iVideoPlayerUtility->Balance();
	INFO_PRINTF2(_L("Playback balance  = (%d)"), balance);

	// Read expected Balance from INI file
	TInt	expectedBalance;
	if ((GetIntFromConfig(aSection, KBalance(), expectedBalance)) &&
		(expectedBalance != balance))
		{
		ERR_PRINTF3(_L("Playback balance actual(%d) != expected(%d)"),
			balance, expectedBalance);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetRotationL(const TDesC& aSection)
	{
	// Read Rotation from INI file
	TPtrC	rotationStr;
	TVideoRotation	rotation = EVideoRotationNone;
	if (GetStringFromConfig(aSection, KRotation(), rotationStr))
	    {
	    if ( !CT_MMUtil::ConvertToVideoRotation(rotationStr, rotation) )
	        {
	        //Read INT invalid rotation for negative tests
	        TInt invalidRotation = 0;
	        if ( !GetIntFromConfig(aSection, KRotation(), invalidRotation) )
	            {
	            ERR_PRINTF2(_L("Illegal rotation %S."), &KRotation());
	            SetBlockResult(EFail);
	            }
	        rotation = (TVideoRotation)invalidRotation;
	        }
	    }
	else
		{
		ERR_PRINTF2(_L("Can not call SetRotationL. Parameter %S is missing in INI file or has wrong value."), &KRotation());
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetRotationL(rotation));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetRotationL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdRotationL(const TDesC& aSection)
	{
	TVideoRotation	rotation = EVideoRotationNone;
	TRAPD(err, rotation = iVideoPlayerUtility->RotationL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::RotationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Rotation of the video clip = (%S)"), &CT_MMUtil::GetVideoRotationString(rotation));

		// Read expected Rotation from INI file
		TPtrC	expectedRotationStr;
		if (GetStringFromConfig(aSection, KRotation(), expectedRotationStr))
			{
			TVideoRotation	expectedRotation;
			if (!CT_MMUtil::ConvertToVideoRotation(expectedRotationStr, expectedRotation))
				{
				//Read INT for negative tests
				TInt invalidRotation = 0;
		        if ( !GetIntFromConfig(aSection, KRotation(), invalidRotation) )
		            {
		            ERR_PRINTF2(_L("Illegal rotation %S."), &KRotation());
		            SetBlockResult(EFail);
		            }
		        expectedRotation = (TVideoRotation)invalidRotation;
				}
			if (expectedRotation != rotation)
				{
				ERR_PRINTF3(_L("Rotation of the video clip actual(%S) != expected(%S)"),
					&CT_MMUtil::GetVideoRotationString(rotation),
					&CT_MMUtil::GetVideoRotationString(expectedRotation));
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetScaleFactorL(const TDesC& aSection)
	{
	// Read WidthPercentage from INI file
	TInt	widthPercentage = 0;
	if (!GetIntFromConfig(aSection, KWidthPercentage(), widthPercentage))
		{
		ERR_PRINTF2(_L("Can not call SetScaleFactorL. Parameter %S is missing in INI file."), &KWidthPercentage());
		SetBlockResult(EFail);
		}
	// Read HeightPercentage from INI file
	TInt	heightPercentage = 0;
	if (!GetIntFromConfig(aSection, KHeightPercentage(), heightPercentage))
		{
		ERR_PRINTF2(_L("Can not call SetScaleFactorL. Parameter %S is missing in INI file."), &KHeightPercentage());
		SetBlockResult(EFail);
		}
	// Read AntiAliasing from INI file
	TBool	antiAliasing = EFalse;
	if (!GetBoolFromConfig(aSection, KAntiAliasing(), antiAliasing))
		{
		ERR_PRINTF2(_L("Can not call SetScaleFactorL. Parameter %S is missing in INI file."), &KAntiAliasing());
		SetBlockResult(EFail);
		}

	TRAPD(err, iVideoPlayerUtility->SetScaleFactorL(widthPercentage, heightPercentage, antiAliasing));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetScaleFactorL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdGetScaleFactorL(const TDesC& aSection)
	{
	TReal32 	widthPercentage = 0;
	TReal32 	heightPercentage = 0;
	TBool 	antiAliasing = EFalse;
	TRAPD(err, iVideoPlayerUtility->GetScaleFactorL(widthPercentage, heightPercentage, antiAliasing));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::GetScaleFactorL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF4(_L("Scale factor of the video clip: width = (%3.0f) heigh = (%3.0f) antialiasing (%d)"),
			widthPercentage, heightPercentage, antiAliasing);

		// Read expected WidthPercentage from INI file
		TInt	expectedWidthPercentage = 0;
		if (GetIntFromConfig(aSection, KWidthPercentage(), expectedWidthPercentage) &&
			(expectedWidthPercentage != widthPercentage))
			{
			ERR_PRINTF3(_L("Scale factor width actual(%3.0f) != expected(%d)"), widthPercentage,
					expectedWidthPercentage);
			SetBlockResult(EFail);
			}
		// Read expected HeightPercentage from INI file
		TInt	expectedHeightPercentage = 0;
		if (GetIntFromConfig(aSection, KHeightPercentage(), expectedHeightPercentage) &&
			(expectedHeightPercentage != heightPercentage))
			{
			ERR_PRINTF3(_L("Scale factor height actual(%3.0f) != expected(%d)"), heightPercentage,
					expectedHeightPercentage);
			SetBlockResult(EFail);
			}
		// Read expected AntiAliasing from INI file
		TBool 	expectedAntiAliasing = EFalse;
		if (GetBoolFromConfig(aSection, KAntiAliasing(), expectedAntiAliasing) &&
			(expectedAntiAliasing != antiAliasing))
			{
			ERR_PRINTF3(_L("Scale factor antialiasing actual(%d) != expected(%d)"), antiAliasing,
					expectedAntiAliasing);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdSetCropRegionL(const TDesC& aSection)
	{
	// Temporary coordinate variables
	TInt	ax = 0;
	TInt	ay = 0;
	TInt	bx = 0;
	TInt	by = 0;
	// Read CropRegion coordinates from INI file
	if (!GetIntFromConfig(aSection, KScreenRectax(), ax))
		{
		ERR_PRINTF2(_L("Can not call DoCmdSetCropRegionL. Parameter %S is missing in INI file."), &KScreenRectax());
		SetBlockResult(EFail);
		}
	if (!GetIntFromConfig(aSection, KScreenRectay(), ay))
		{
		ERR_PRINTF2(_L("Can not call DoCmdSetCropRegionL. Parameter %S is missing in INI file."), &KScreenRectay());
		SetBlockResult(EFail);
		}
	if (!GetIntFromConfig(aSection, KScreenRectbx(), bx))
		{
		ERR_PRINTF2(_L("Can not call DoCmdSetCropRegionL. Parameter %S is missing in INI file."), &KScreenRectbx());
		SetBlockResult(EFail);
		}
	if (!GetIntFromConfig(aSection, KScreenRectby(), by))
		{
		ERR_PRINTF2(_L("Can not call DoCmdSetCropRegionL. Parameter %S is missing in INI file."), &KScreenRectby());
		SetBlockResult(EFail);
		}
	TRect	cropRegion(ax, ay, bx, by);

	TRAPD(err, iVideoPlayerUtility->SetCropRegionL(cropRegion));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::SetCropRegionL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdGetCropRegionL(const TDesC& aSection)
	{
	TRect	cropRegion;
	TRAPD(err, iVideoPlayerUtility->GetCropRegionL(cropRegion));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::GetCropRegionL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF5(_L("Crop region of the video clip: ax = (%d) ay = (%d) bx = (%d) by = (%d)"),
			cropRegion.iTl.iX, cropRegion.iTl.iY, cropRegion.iBr.iX, cropRegion.iBr.iY);

		// Read expected coordinate Ax from INI file
		TInt 	expectedAx = 0;
		if (GetIntFromConfig(aSection, KScreenRectax(), expectedAx) &&
			(cropRegion.iTl.iX != expectedAx))
			{
			ERR_PRINTF3(_L("Crop region coordinate Ax (%d) != expected(%d)"),
				cropRegion.iTl.iX, expectedAx);
			SetBlockResult(EFail);
			}

		// Read expected coordinate Ay from INI file
		TInt 	expectedAy = 0;
		if (GetIntFromConfig(aSection, KScreenRectay(), expectedAy) &&
			(cropRegion.iTl.iY != expectedAy))
			{
			ERR_PRINTF3(_L("Crop region coordinate Ay (%d) != expected(%d)"),
				cropRegion.iTl.iY, expectedAy);
			SetBlockResult(EFail);
			}

		// Read expected coordinate Bx from INI file
		TInt 	expectedBx = 0;
		if (GetIntFromConfig(aSection, KScreenRectbx(), expectedBx) &&
			(cropRegion.iBr.iX != expectedBx))
			{
			ERR_PRINTF3(_L("Crop region coordinate Bx (%d) != expected(%d)"),
				cropRegion.iBr.iX, expectedBx);
			SetBlockResult(EFail);
			}

		// Read expected coordinate By from INI file
		TInt 	expectedBy = 0;
		if (GetIntFromConfig(aSection, KScreenRectby(), expectedBy) &&
			(cropRegion.iBr.iY != expectedBy))
			{
			ERR_PRINTF3(_L("Crop region coordinate By (%d) != expected(%d)"),
				cropRegion.iBr.iY, expectedBy);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdNumberOfMetaDataEntriesL(const TDesC& aSection)
	{
	TInt	nrMetaDataEntries = 0;
	TRAPD(err, nrMetaDataEntries = iVideoPlayerUtility->NumberOfMetaDataEntriesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::NumberOfMetaDataEntriesL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("Number of meta data entries (%d)"), nrMetaDataEntries);

		// Read expected nrMetaDataEntries from INI file
		TInt	expectednrMetaDataEntries = 0;
		if (GetIntFromConfig(aSection, KNrOfMetaDataEntries(), expectednrMetaDataEntries) &&
			(expectednrMetaDataEntries != nrMetaDataEntries))
			{
			ERR_PRINTF3(_L("Number of meta data entries actual (%d) != expected (%d)"),
				nrMetaDataEntries, expectednrMetaDataEntries);
			SetBlockResult(EFail);
			}
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdMetaDataEntryL(const TDesC& aSection)
	{
	TInt	metaDataEntryIndex = 0;
	GetIntFromConfig(aSection, KMetaDataEntryIndex(), metaDataEntryIndex);
	CMMFMetaDataEntry*	metaDataEntry = NULL;
	TRAPD(err, metaDataEntry = iVideoPlayerUtility->MetaDataEntryL(metaDataEntryIndex));
	CleanupStack::PushL(metaDataEntry);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MetaDataEntryL Error %d"), err);
		SetError(err);
		}
	else
		{
		const TDesC& name = metaDataEntry->Name();
		const TDesC& value = metaDataEntry->Value();
		INFO_PRINTF3(_L("MetaDataEntryL name = %S, value = %S"), &name, &value);

		// Read expected MetaDataEntry name from INI file
		TPtrC 	expectedName;
		if (GetStringFromConfig(aSection, KName(), expectedName) &&
			(name != expectedName))
			{
			ERR_PRINTF3(_L("MetaDataEntry name (%S) != expected(%S)"),
				&name, &expectedName);
			SetBlockResult(EFail);
			}

		// Read expected MetaDataEntry value from INI file
		TPtrC 	expectedValue;
		if (GetStringFromConfig(aSection, KValue(), expectedValue) &&
			(value != expectedValue))
			{
			ERR_PRINTF3(_L("MetaDataEntry value (%S) != expected(%S)"),
				&value, &expectedValue);
			SetBlockResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(metaDataEntry);
	}

void CT_MMDataVideoPlayerUtility::DoCmdControllerImplementationInformationL()
	{
	const CMMFControllerImplementationInformation*	controllerInfo = NULL;
	TRAPD(err, controllerInfo = &(iVideoPlayerUtility->ControllerImplementationInformationL()));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CT_MMDataVideoPlayerUtility::ControllerImplementationInformationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::LogL(*this, *controllerInfo);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdCustomCommandSyncL(const TDesC& aSection)
	{
	TMMFMessageDestination		handleInfo(iVideoControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if (GetStringFromConfig(aSection, KDataTo1(), dataTo1Str))
		{
		dataTo1 = HBufC8::NewLC(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewLC(0);
		dataTo1->Des().Copy(KNullDesC8);
		}

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if (GetStringFromConfig(aSection, KDataTo2(), dataTo2Str))
		{
		dataTo2 = HBufC8::NewLC(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo2Str);
		}
	else
		{
		dataTo2 = HBufC8::NewLC(0);
		dataTo2->Des().Copy(KNullDesC8);
		}

	TInt					err = KErrNone;
	TBuf8<KMaxdataFromText>	dataFromText;
	if (useDataFrom)
		{
		err = iVideoPlayerUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2, dataFromText);
		}
	else
		{
		err = iVideoPlayerUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2);
		}
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::CustomCommandSync failed with error %d."), err);
		SetError(err);
		}
	else
		{
		if (useDataFrom)
			{
			HBufC*	buffer = HBufC::NewLC(dataFromText.Length());
			TPtr	bufferPtr = buffer->Des();
			bufferPtr.Copy(dataFromText); // Create a 16 bit copy of the 8 bit original
			INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);
			CleanupStack::PopAndDestroy(buffer);
			}
		}

	CleanupStack::PopAndDestroy(2, dataTo1);
}

void CT_MMDataVideoPlayerUtility::DoCmdCustomCommandASyncL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	TMMFMessageDestination		handleInfo(iVideoControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if (GetStringFromConfig(aSection, KDataTo1(), dataTo1Str))
		{
		dataTo1 = HBufC8::NewLC(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewLC(0);
		dataTo1->Des().Copy(KNullDesC8);
		}

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if (GetStringFromConfig(aSection, KDataTo2(), dataTo2Str))
		{
		dataTo2 = HBufC8::NewLC(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo2Str);
		}
	else
		{
		dataTo2 = HBufC8::NewLC(0);
		dataTo2->Des().Copy(KNullDesC8);
		}

	if (useDataFrom)
		{
		iVideoPlayerUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iDataFromText, iActiveCustomCommandAsync->iStatus);
		}
	else
		{
		iVideoPlayerUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iActiveCustomCommandAsync->iStatus);
		}

	iActiveCustomCommandAsync->Activate(aAsyncErrorIndex);
	IncOutstanding();
	CleanupStack::PopAndDestroy(2, dataTo1);
	}

void CT_MMDataVideoPlayerUtility::DoCmdGetDRMCustomCommand()
	{
	MMMFDRMCustomCommand* controllerHandle = NULL;
	controllerHandle = iVideoPlayerUtility->GetDRMCustomCommand();
	if (controllerHandle != NULL)
		{
		INFO_PRINTF1(_L("Controller DRM handle returned"));
		}
	else
		{
		INFO_PRINTF1(_L("Controller DRM handle not returned"));
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdStopDirectScreenAccessL()
	{
	TRAPD(err, iVideoPlayerUtility->StopDirectScreenAccessL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::StopDirectScreenAccessL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdStartDirectScreenAccessL()
	{
	TRAPD(err, iVideoPlayerUtility->StartDirectScreenAccessL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CVideoPlayerUtility::StartDirectScreenAccessL failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataVideoPlayerUtility::DoCmdDestructor()
	{
	DestroyData();
	}

///////////////////////////////////////////////////////////////////
//from the MVideoPlayerUtilityObserver interface  / / / / / / / /
///////////////////////////////////////////////////////////////////

/**
Asynchronous callback called when CVideoPlayerUtility::Open completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if opened ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvpuoOpenComplete(TInt aError)
	{
	if (aError == KErrNone )
		{
		INFO_PRINTF1(_L("Open OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Open Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

/**
Asynchronous callback called when CVideoPlayerUtility::Prepare completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if prepared ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvpuoPrepareComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("Prepare OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Prepare Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

/**
Asynchronous callback called when CVideoPlayerUtility::GetFrameL completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if GetFrame ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("Get frame OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Get frame Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

/**
Asynchronous callback called when CVideoPlayerUtility::Play completes
@internalAll
@return void
@param aError Symbian error code on failure, KErrNone if plaied ok
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvpuoPlayComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		INFO_PRINTF1(_L("Play OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Play Error %d"), aError);
		SetAsyncError(iApiAsyncErrorIndex, aError);
		}

	DecOutstanding();
	}

/**
Asynchronous callback called when general event notification from controller received
@internalAll
@return void
@param aEvent The event sent by the controller
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvpuoEvent(const TMMFEvent &aEvent)
	{
	if(aEvent.iErrorCode == KErrNone)
		{
		INFO_PRINTF1(_L("MvpuoEvent OK"));
		INFO_PRINTF2(_L("MvpuoEvent event type %d"), aEvent.iEventType);
		}
	else
		{
		ERR_PRINTF2(_L("MvpuoEvent Error %d"), aEvent.iErrorCode);
		SetAsyncError(iApiAsyncErrorIndex, aEvent.iErrorCode);
		}

	DecOutstanding();
	}

///////////////////////////////////////////////////////////////////
//from the 	MVideoLoadingObserver interface  / / / / / / / /
///////////////////////////////////////////////////////////////////

/**
Asynchronous callback called when video clip loading/rebuffering has started
@internalAll
@return void
@param N/A
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvloLoadingStarted()
	{
	INFO_PRINTF1(_L("Video clip loading/rebuffering has started"));
	}

/**
Asynchronous callback called when video clip loading/rebuffering has completed
@internalAll
@return void
@param N/A
@pre N/A
@post N/A
*/
void CT_MMDataVideoPlayerUtility::MvloLoadingComplete()
	{
	INFO_PRINTF1(_L("Video clip loading/rebuffering has completed"));
	}

/**
 Virtual RunL - Called on completion of an aaSynchronous command
 @internalComponent
 @see MT_MMActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataVideoPlayerUtility::RunL(CActive* aActive, TInt aIndex)
 	{
 	if (aActive == iActiveCustomCommandAsync)
 		{
 		RunCustomCommandAsyncL(aIndex);
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  Virtual DoCancel - Request to cancel the asynchronous command
  @internalComponent
  @see - MTPActiveCallback
  @param aActive Active Object that DoCancel has been called on
  @pre - N/A
  @post - N/A
  @leave system wide error code
 */
 void CT_MMDataVideoPlayerUtility::DoCancel(CActive* aActive)
 	{
 	if (aActive == iActiveCustomCommandAsync)
 		{
 		DoCancelCustomCommandAsync();
 		}
 	else
 		{
 		ERR_PRINTF1(_L("Stray signal"));
 		SetBlockResult(EFail);
 		}
 	}

/**
  RunActiveL - Process RunL for iActiveCustomCommandAsync
  @internalComponent
  @pre N/A
  @post N/A
  @leave system wide error code
 */
 void CT_MMDataVideoPlayerUtility::RunCustomCommandAsyncL(TInt aIndex)
	{
	HBufC*	buffer = HBufC::NewLC(iDataFromText.Length());
	TPtr	bufferPtr = buffer->Des();
	bufferPtr.Copy(iDataFromText); // Create a 16 bit copy of the 8 bit original
	INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);

	TInt	err = iActiveCustomCommandAsync->iStatus.Int();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
		}
	DecOutstanding();
	CleanupStack::PopAndDestroy(buffer);
	}

/**
 DoCancelActive - Process DoCancel for iActive
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_MMDataVideoPlayerUtility::DoCancelCustomCommandAsync()
	{
	}
