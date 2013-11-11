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


#include <mm/mmcaf.h>
//	User Includes
#include "T_MMDataMdaAudioPlayerUtility.h"
#include "T_MMUtil.h"



/*@{*/
//Parameters implementation
_LIT(KBalance,										"balance");
_LIT(KPosition,										"position");
_LIT(KVolume,										"volume");
_LIT(KInputFile,									"inputfile");
_LIT(KRFile,										"rfile");
_LIT(KTMMSource,									"tmmsource");
_LIT(KPlayWindowStart,								"start");
_LIT(KPlayWindowEnd,								"end");
_LIT(KRepeatNumberOfTimes,							"repeatnumberoftimes");
_LIT(KTrailingSilence,								"trailingsilence");
_LIT(KRampDuration,									"rampduration");
_LIT(KMetaDataIndex,								"metadataindex");
_LIT(KFunction,										"function");
_LIT(KUseDataFrom,									"usedatafrom");
_LIT(KUniqueId, 									"uniqueid");
_LIT(KEnableUI, 									"enableui");
_LIT(KMaxVolume, 									"maxvolume");
_LIT(KDuration2, 									"duration2");
_LIT(KTime,		 									"time");
_LIT(KBitRate, 										"bitrate");
_LIT(KNbrEntries, 									"nbrentries");
_LIT(KEventUID, 									"eventuid");
_LIT(KDRMCustomCommand, 							"drmcommand");
_LIT(KAccessPointID, 								"accesspoint");
_LIT(KMimeType, 									"mimetype");
_LIT(KDataTo1,										"datato1");
_LIT(KDataTo2,										"datato2");
_LIT(KOpenFile,										"openfile");
_LIT(KDurationState, 								"durationstate");

_LIT(KEMMFDurationInfoValid, 						"Valid");
_LIT(KEMMFDurationInfoInfinite, 					"Infinite");
_LIT(KEMMFDurationInfoUnknown, 						"Unknown");
_LIT(KProgress, 									"Progress");
_LIT(KRepeatForever,    				         	"RepeatForever");

//Commands implementation
_LIT(KCmdDestructor,								"~");
_LIT(KCmdNewL,										"NewL");
_LIT(KCmdNewFilePlayerL,							"NewFilePlayerL");
_LIT(KCmdNewDesPlayerReadOnlyL,						"NewDesPlayerReadOnlyL");
_LIT(KCmdNewDesPlayerL,								"NewDesPlayerL");
_LIT(KCmdOpenDesL,									"OpenDesL");
_LIT(KCmdOpenUrlL,									"OpenUrlL");
_LIT(KCmdOpenFileL,									"OpenFileL");
_LIT(KCmdClose,										"Close");
_LIT(KCmdPlay,										"Play");
_LIT(KCmdPause,										"Pause");
_LIT(KCmdWillResumePlay,							"WillResumePlay");
_LIT(KCmdStop,										"Stop");
_LIT(KCmdSetVolume,									"SetVolume");
_LIT(KCmdSetBalance,								"SetBalance");
_LIT(KCmdSetPosition,								"SetPosition");
_LIT(KCmdMaxVolume,									"MaxVolume");
_LIT(KCmdGetVolume,									"GetVolume");
_LIT(KCmdGetBalance,								"GetBalance");
_LIT(KCmdGetPosition,								"GetPosition");
_LIT(KCmdDuration,									"Duration");
_LIT(KCmdSetPlayWindow,								"SetPlayWindow");
_LIT(KCmdClearPlayWindow,							"ClearPlayWindow");
_LIT(KCmdSetRepeats,								"SetRepeats");
_LIT(KCmdSetVolumeRamp,								"SetVolumeRamp");
_LIT(KCmdGetBitRate,								"GetBitRate");
_LIT(KCmdGetMetaDataEntryL,							"GetMetaDataEntryL");
_LIT(KCmdGetNumberOfMetaDataEntries,				"GetNumberOfMetaDataEntries");
_LIT(KCmdRegisterAudioResourceNotification,			"RegisterAudioResourceNotification");
_LIT(KCmdCancelRegisterAudioResourceNotification,	"CancelRegisterAudioResourceNotification");
_LIT(KCmdRegisterForAudioLoadingNotification,		"RegisterForAudioLoadingNotification");
_LIT(KCmdSetPriority,								"SetPriority");
_LIT(KCmdSetThreadPriority,							"SetThreadPriority");
_LIT(KCmdGetAudioLoadingProgressL,					"GetAudioLoadingProgressL");
_LIT(KCmdGetDRMCustomCommand,						"GetDRMCustomCommand");
_LIT(KCmdControllerImplementationInformationL,		"ControllerImplementationInformationL");
_LIT(KCmdCustomCommandSync,							"CustomCommandSync");
_LIT(KCmdCustomCommandASync,						"CustomCommandASync");
_LIT(KCmdUseSharedHeap,								"UseSharedHeap");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataMdaAudioPlayerUtility* CT_MMDataMdaAudioPlayerUtility::NewL()
	{
	CT_MMDataMdaAudioPlayerUtility*	ret = new (ELeave) CT_MMDataMdaAudioPlayerUtility();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataMdaAudioPlayerUtility::CT_MMDataMdaAudioPlayerUtility()
:	iPlayerUtility(NULL)
,	iActiveCustomCommandAsync(NULL)
,	iServer(NULL)
,	iAsyncErrorIndex(KErrNone)
,	iFileOpened(EFalse)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
,	iPlaying(EFalse)
	{
	}

void CT_MMDataMdaAudioPlayerUtility::ConstructL()
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

	iActiveCustomCommandAsync = CActiveCallback::NewL(*this);
	iServer = CMdaServer::NewL();
	}

CT_MMDataMdaAudioPlayerUtility::~CT_MMDataMdaAudioPlayerUtility()
/**
 * Public destructor
 */
	{
	DestroyData();

	delete iServer;
	iServer = NULL;

	delete iActiveCustomCommandAsync;
	iActiveCustomCommandAsync = NULL;

	iFs.Close();
	}

TAny* CT_MMDataMdaAudioPlayerUtility::GetObject()
	{
	return iPlayerUtility;
	}

void CT_MMDataMdaAudioPlayerUtility::SetObjectL(TAny* aObject)
	{
	DestroyData();
	iPlayerUtility	= static_cast<CMdaAudioPlayerUtility*> (aObject);
	}

void CT_MMDataMdaAudioPlayerUtility::DisownObjectL()
	{
	iPlayerUtility = NULL;
	}

void CT_MMDataMdaAudioPlayerUtility::DestroyData()
	{
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened = EFalse;
		}

	delete iPlayerUtility;
	iPlayerUtility = NULL;
	delete iBufferPtr;
	iBufferPtr=NULL;
	delete iBuffer;
	iBuffer	= NULL;
	}

TBool CT_MMDataMdaAudioPlayerUtility::DoCommandL( const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	TBool ret = ETrue;

	if ( aCommand == KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand == KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand == KCmdNewFilePlayerL )
		{
		DoCmdNewFilePlayerL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdNewDesPlayerReadOnlyL )
		{
		DoCmdNewDesPlayerReadOnlyL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdNewDesPlayerL )
		{
		DoCmdNewDesPlayerL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdOpenDesL )
		{
		DoCmdOpenDesL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdOpenUrlL )
		{
		DoCmdOpenUrlL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdOpenFileL )
		{
		DoCmdOpenFileL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand == KCmdPlay )
		{
		DoCmdPlay(aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdPause )
		{
		DoCmdPause();
		}
	else if ( aCommand == KCmdWillResumePlay )
		{
		DoCmdWillResumePlay();
		}
	else if ( aCommand == KCmdStop )
		{
		DoCmdStop();
		}
	else if ( aCommand == KCmdSetVolume )
		{
		DoCmdSetVolume(aSection);
		}
	else if ( aCommand == KCmdSetBalance )
		{
		DoCmdSetBalance(aSection);
		}
	else if ( aCommand == KCmdSetPosition )
		{
		DoCmdSetPosition(aSection);
		}
	else if ( aCommand == KCmdMaxVolume )
		{
		DoCmdMaxVolume(aSection);
		}
	else if ( aCommand == KCmdGetVolume )
		{
		DoCmdGetVolume(aSection);
		}
	else if ( aCommand == KCmdGetBalance )
		{
		DoCmdGetBalance(aSection);
		}
	else if ( aCommand == KCmdGetPosition )
		{
		DoCmdGetPosition(aSection);
		}
	else if ( aCommand == KCmdDuration )
		{
		DoCmdDuration(aSection);
		}
	else if ( aCommand == KCmdSetPlayWindow )
		{
		DoCmdSetPlayWindow(aSection);
		}
	else if ( aCommand == KCmdClearPlayWindow )
		{
		DoCmdClearPlayWindow();
		}
	else if ( aCommand == KCmdSetRepeats )
		{
		DoCmdSetRepeats(aSection);
		}
	else if ( aCommand == KCmdSetVolumeRamp )
		{
		DoCmdSetVolumeRamp(aSection);
		}
	else if ( aCommand == KCmdGetBitRate )
		{
		DoCmdGetBitRate(aSection);
		}
	else if ( aCommand == KCmdGetMetaDataEntryL )
		{
		DoCmdGetMetaDataEntryL(aSection);
		}
	else if ( aCommand == KCmdGetNumberOfMetaDataEntries )
		{
		DoCmdGetNumberOfMetaDataEntries(aSection);
		}
	else if ( aCommand == KCmdRegisterAudioResourceNotification )
		{
		DoCmdRegisterAudioResourceNotificationL(aSection);
		}
	else if ( aCommand == KCmdCancelRegisterAudioResourceNotification )
		{
		DoCmdCancelRegisterAudioResourceNotification(aSection);
		}
	else if ( aCommand == KCmdRegisterForAudioLoadingNotification )
		{
		DoCmdRegisterForAudioLoadingNotification();
		}
	else if ( aCommand == KCmdSetPriority )
		{
		DoCmdSetPriority(aSection);
		}
	else if ( aCommand == KCmdSetThreadPriority )
		{
		DoCmdSetThreadPriority(aSection);
		}
	else if ( aCommand == KCmdGetAudioLoadingProgressL )
		{
		DoCmdGetAudioLoadingProgressL(aSection);
		}
	else if ( aCommand == KCmdGetDRMCustomCommand )
		{
		DoCmdGetDRMCustomCommand(aSection);
		}
	else if ( aCommand == KCmdControllerImplementationInformationL )
		{
		DoCmdControllerImplementationInformationL();
		}
	else if ( aCommand == KCmdCustomCommandSync )
		{
		DoCmdCustomCommandSyncL(aSection);
		}
	else if ( aCommand == KCmdCustomCommandASync )
		{
		DoCmdCustomCommandAsyncL(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdUseSharedHeap )
		{
		DoCmdUseSharedHeap();
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdNewL( const TDesC& aSection)
	{
	TInt	priority;
	TBool	hasPriority=CT_MMUtil::ReadMdaPriority(*this, aSection, priority);

	TMdaPriorityPreference	priorityPreference;
	TBool					hasPreference=CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference);

	if ( hasPriority && hasPreference )
		{
		TRAPD( err, iPlayerUtility = CMdaAudioPlayerUtility::NewL(*this, priority, priorityPreference) );
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this, priority, priorityPreference) Error %d"), err);
			SetError(err);
			}
		}
	else	// if not setting Priority param
		{
		TRAPD(err, iPlayerUtility = CMdaAudioPlayerUtility::NewL(*this));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this) Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF1(_L("Function NewL(this) OK"));
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdNewFilePlayerL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TPtrC	inputFile;
	if( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, inputFile, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		iAsyncErrorIndex = aAsyncErrorIndex;

		TRAPD(err, iPlayerUtility = CMdaAudioPlayerUtility::NewFilePlayerL (inputFile,
																		    *this,
																		    priority,
																		    priorityPreference,
																		    iServer));
		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("Function NewFilePlayerL OK, file = %S"), &inputFile);
			IncOutstanding();
			//call back in MapcInitComplete
			}
		else
			{
			ERR_PRINTF3(_L("NewFilePlayerL(%S) Error %d"), &inputFile, err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdNewDesPlayerL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TPtrC	inputFile;
	if( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, inputFile, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		// Open file and read contents into buffer
		RFile	file;
		User::LeaveIfError(file.Open(iFs, inputFile, EFileRead | EFileShareAny));
		CleanupClosePushL(file);

		TInt fileSize = 0;
		User::LeaveIfError(file.Size(fileSize));

		delete iBuffer;
		iBuffer=NULL;
		iBuffer=HBufC8::NewL(fileSize);
		*iBuffer=KNullDesC8;
		delete iBufferPtr;
		iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());

		User::LeaveIfError(file.Read(*iBufferPtr));

		iAsyncErrorIndex = aAsyncErrorIndex;

		TRAPD(err, iPlayerUtility = CMdaAudioPlayerUtility::NewDesPlayerL (*iBufferPtr,
																		   *this,
																		   priority,
																		   priorityPreference,
																		   iServer));
		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("Function NewDesPlayerL OK, file = %S"), &inputFile);
			IncOutstanding();
			//call back in MapcInitComplete
			}
		else
			{
			ERR_PRINTF3(_L("NewDesPlayerL Error %d, file = %S"), err, &inputFile);
			SetError(err);
			}

		CleanupStack::PopAndDestroy(&file);
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdNewDesPlayerReadOnlyL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TPtrC	inputFile;
	if( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, inputFile, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		// Open file and read contents into buffer
		RFile	file;
		User::LeaveIfError(file.Open(iFs, inputFile, EFileRead | EFileShareAny));
		CleanupClosePushL(file);

		TInt fileSize = 0;
		User::LeaveIfError(file.Size(fileSize));

		delete iBuffer;
		iBuffer=NULL;
		iBuffer=HBufC8::NewL(fileSize);
		*iBuffer=KNullDesC8;
		delete iBufferPtr;
		iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());

		User::LeaveIfError(file.Read(*iBufferPtr));

		iAsyncErrorIndex = aAsyncErrorIndex;

		TRAPD(err, iPlayerUtility = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL (*iBufferPtr,
																				   *this,
																				   priority,
																				   priorityPreference,
																				   iServer));
		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("Function NewDesPlayerReadOnlyL OK, file = %S"), &inputFile);
			IncOutstanding();
			//call back in MapcInitComplete
			}
		else
			{
			ERR_PRINTF3(_L("NewDesPlayerReadOnlyL Error %d, file = %S"), err, &inputFile);
			SetError(err);
			}

		CleanupStack::PopAndDestroy(&file);
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile());
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdOpenDesL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	TPtrC	inputFile;
	if( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, inputFile, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		// Open file and read contents into buffer
		RFile	file;
		User::LeaveIfError(file.Open(iFs, inputFile, EFileRead | EFileShareAny));
		CleanupClosePushL(file);

		TInt fileSize = 0;
		User::LeaveIfError(file.Size(fileSize));

		delete iBuffer;
		iBuffer=NULL;
		iBuffer=HBufC8::NewL(fileSize);
		*iBuffer=KNullDesC8;
		delete iBufferPtr;
		iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());

		User::LeaveIfError(file.Read(*iBufferPtr));

		iAsyncErrorIndex = aAsyncErrorIndex;

		TRAPD(err, iPlayerUtility->OpenDesL(*iBufferPtr));

		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("Function OpenDesL OK, file = %S"), &inputFile);
			IncOutstanding();
			//call back in MapcInitComplete
			}
		else
			{
			ERR_PRINTF3(_L("OpenDesL Error %d, file = %S"), err, &inputFile);
			SetError(err);
			}

		CleanupStack::PopAndDestroy(&file);
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdOpenUrlL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	TPtrC	url;
	if( GetStringFromConfig(aSection, KInputFile(), url) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, url, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		TInt	accessPointID = KUseDefaultIap;
		GetIntFromConfig(aSection, KAccessPointID(), accessPointID);

		HBufC8*	mimeType8=NULL;
		TPtrC	mimeType;
		if ( GetStringFromConfig(aSection, KMimeType(), mimeType) )
			{
			mimeType8=HBufC8::NewL(mimeType.Length());
			mimeType8->Des().Copy(mimeType);
			}
		else
			{
			mimeType8=HBufC8::NewL(0);
			mimeType8->Des().Copy(KNullDesC8);
			}
		CleanupStack::PushL(mimeType8);

		iAsyncErrorIndex = aAsyncErrorIndex;

		TRAPD(err, iPlayerUtility->OpenUrlL(url, accessPointID, *mimeType8));
		if ( err == KErrNone )
			{
			INFO_PRINTF2(_L("Function OpenUrlL OK, url = %S"), &url);
			IncOutstanding();
			//call back in MapcInitComplete
			}
		else
			{
			ERR_PRINTF3(_L("OpenUrlL(%S) Error %d"), &url, err);
			SetError(err);
			}

		CleanupStack::PopAndDestroy(mimeType8);
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdOpenFileL( const TDesC& aSection, const TInt aAsyncErrorIndex )
	{

	iAsyncErrorIndex = aAsyncErrorIndex;

	TPtrC	inputFile;
	if( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		CT_MMUtil::GetControllerAndFormatUidL(*this, inputFile, KUidMediaTypeAudio, EFalse, iAudioControllerUid, iAudioFormatUid);

		TBool useTMMSource = EFalse;
		GetBoolFromConfig(aSection, KTMMSource(), useTMMSource);

		TBool useRFile = EFalse;
		GetBoolFromConfig(aSection, KRFile(), useRFile);

		if (useRFile)
		    {
			// Open file and read contents into buffer
		    if ( iFileOpened )
		        {
		        iFile.Close();
		        iFileOpened = EFalse;
		        }

	        TBool openFile = ETrue;
			GetBoolFromConfig(aSection, KOpenFile(), openFile);

	        if( openFile )
		        {
			    User::LeaveIfError(iFile.Open(iFs, inputFile, EFileRead | EFileShareAny));
			    iFileOpened = ETrue;
		        }

		    TRAPD(err, iPlayerUtility->OpenFileL(iFile));
			if ( err == KErrNone )
				{
				INFO_PRINTF2(_L("Function OpenFileL(RFile) OK, file = %S"), &inputFile);
				IncOutstanding();
				//call back in MapcInitComplete
				}
			else
				{
				ERR_PRINTF2(_L("OpenFileL(RFile) Error =%d"), err);
				SetError(err);
				}
	    	}
		else if (useTMMSource)
		    {
		    ContentAccess::TIntent	intent=ContentAccess::EPeek;
			if ( !CT_MMUtil::ReadIntent(*this, aSection, intent) )
				{
				intent=ContentAccess::EPeek;
				}

		    TPtrC uniqueId = _L("");
		    GetStringFromConfig(aSection, KUniqueId(), uniqueId);

		    TBool enableUI = EFalse;
		    GetBoolFromConfig(aSection, KEnableUI(), enableUI);

		    TMMFileSource fileSource(inputFile, uniqueId, intent, enableUI);

		    TRAPD(err, iPlayerUtility->OpenFileL(fileSource));
			if ( err == KErrNone )
				{
				INFO_PRINTF2(_L("Function OpenFileL(TMMSource) OK, file = %S"), &inputFile);
				IncOutstanding();
				//call back in MapcInitComplete
				}
			else
				{
				ERR_PRINTF2(_L("OpenFileL(TMMSource) Error =%d"), err);
				SetError(err);
				}
		    }
		else
		    {
		    TRAPD(err, iPlayerUtility->OpenFileL(inputFile));
			if ( err == KErrNone )
				{
				INFO_PRINTF2(_L("Function OpenFileL(TDesC8) OK, file = %S"), &inputFile);
				IncOutstanding();
				//call back in MapcInitComplete
				}
			else
				{
				ERR_PRINTF2(_L("OpenFileL(TDesC8) Error =%d"), err);
				SetError(err);
				}
		    }
		}
	else
		{
		ERR_PRINTF2(_L("Error read parameter =%S"), &KInputFile);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdClose()
	{
	iPlayerUtility->Close();
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdPlay( const TInt aAsyncErrorIndex )
	{
	iAsyncErrorIndex = aAsyncErrorIndex;
	iPlaying=ETrue;
	iPlayerUtility->Play();
	IncOutstanding();
	//call back in MapcPlayComplete
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdPause()
	{
	TInt error = iPlayerUtility->Pause();
	if ( error != KErrNone )
		{
		ERR_PRINTF2(_L("Cannot Pause, Error %d"), error);
		SetError(error);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdWillResumePlay()
	{
	TInt err = iPlayerUtility->WillResumePlay();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("WillResumePlay Error %d"), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdStop()
	{
	iPlayerUtility->Stop();
	if ( iPlaying )
		{
		iPlaying=EFalse;
		DecOutstanding();
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetVolume( const TDesC& aSection)
	{
	TInt								setVolume=0;

	GetIntFromConfig(aSection, KVolume(), setVolume);

	iPlayerUtility->SetVolume(setVolume);
	INFO_PRINTF2(_L("SetVolume = %d"), setVolume);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetBalance( const TDesC& aSection)
	{
	TInt								balance = 0;

	GetIntFromConfig(aSection, KBalance(), balance);

	iPlayerUtility->SetBalance(balance);
	INFO_PRINTF2(_L("SetBalance = %d"), balance);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetPosition( const TDesC& aSection)
	{
	TInt								inputPosition = 0;

	GetIntFromConfig(aSection, KPosition(), inputPosition);

	TTimeIntervalMicroSeconds time(inputPosition);
	iPlayerUtility->SetPosition(time);
	INFO_PRINTF2(_L("SetPosition OK. Current position: GetPosition() = %d"), inputPosition);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdMaxVolume( const TDesC& aSection)
	{

	TInt	maxVolume = iPlayerUtility->MaxVolume();
	INFO_PRINTF2(_L("MaxVolume = %d"), maxVolume);

	TInt	inputVolume = 0;
	if ( GetIntFromConfig(aSection, KMaxVolume(), inputVolume) )
		{
		if( inputVolume != maxVolume )
			{
			ERR_PRINTF3(_L("MaxVolume (%d) != inputVolume (%d)"), maxVolume, inputVolume);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF3(_L("MaxVolume (%d) == inputVolume (%d)"), maxVolume, inputVolume);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetVolume( const TDesC& aSection)
	{
	TInt								currentVolume=0;

	TInt	error = iPlayerUtility->GetVolume(currentVolume);
	if ( error  != KErrNone )
		{
		ERR_PRINTF2(_L("Error GetVolume, Error %d"), error);
		SetError(error);
		}
	else
		{
		TInt inputVolume	= 0;

		if( GetIntFromConfig(aSection, KVolume(), inputVolume) )
			{
			if(inputVolume != currentVolume)
				{
				ERR_PRINTF3(_L("Current volume %d != %d"), currentVolume, inputVolume);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF3(_L("Current volume %d == %d"), currentVolume, inputVolume);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Current volume = %d"), currentVolume);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetBalance( const TDesC& aSection)
	{
	TInt 								currentBalance=0;

	TInt	error = iPlayerUtility->GetBalance(currentBalance);
	if ( error  != KErrNone )
		{
		ERR_PRINTF2(_L("Error GetBalance, Error %d"), error);
		SetError(error);
		}
	else
		{
		TInt inputBalance 	=0;
		if( GetIntFromConfig(aSection, KBalance(), inputBalance) )
			{
			if(inputBalance != currentBalance)
				{
				ERR_PRINTF3(_L("Current balance %d != %d"), currentBalance, inputBalance);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF3(_L("Current balance %d == %d"), currentBalance, inputBalance);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Current balance = %d"), currentBalance);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetPosition( const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds 			currentPosition=0;

	TInt	error = iPlayerUtility->GetPosition(currentPosition);
	if ( error  != KErrNone )
		{
		ERR_PRINTF2(_L("Error GetPosition, Error %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF2(_L("Current position = %d"), currentPosition.Int64());

		TInt expectedPosition=0;
		if ( GetIntFromConfig(aSection, KPosition(), expectedPosition) )
			{
			if(expectedPosition != currentPosition.Int64())
				{
				ERR_PRINTF3(_L("Current position %d != %d"), currentPosition.Int64(), expectedPosition);
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdDuration( const TDesC& aSection)
	{
	TTimeIntervalMicroSeconds  			actualDuration;
	TInt 								expectedDuration=0;

	//	Read from INI file which function of Duration we can use.
	//	If parameter "command_duration2" is = TRUE then use Duration2
	TBool useDuration2 = EFalse;
	GetBoolFromConfig(aSection, KDuration2(), useDuration2);

	if( !useDuration2 )
		{
		actualDuration = iPlayerUtility->Duration();
		INFO_PRINTF2(_L("Duration = %d (MicroSec)"), actualDuration.Int64());

		if( GetIntFromConfig(aSection, KTime(), expectedDuration) )
			{
			if( expectedDuration != actualDuration.Int64() )
				{
	        	ERR_PRINTF3(_L("expectedDuration (%d) != actualDuration (%d)"), expectedDuration, actualDuration.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	else
		{
		TMMFDurationInfo	actualDurationInfo = iPlayerUtility->Duration(actualDuration);
		switch (actualDurationInfo)
			{
		case EMMFDurationInfoValid:
			INFO_PRINTF2(_L("Duration = %d (MicroSec)"), actualDuration.Int64());
			break;
		case EMMFDurationInfoUnknown:
			INFO_PRINTF1(_L("Duration length is unknown"));
			break;
		case EMMFDurationInfoInfinite:
			INFO_PRINTF1(_L("Clip is of infinite length"));
			break;
		default:
			break;
			}

		TPtrC	expectedDurationState;
		if( GetStringFromConfig(aSection, KDurationState(), expectedDurationState) )
			{
			TMMFDurationInfo	expectedDurationInfo = EMMFDurationInfoValid;

			if( expectedDurationState == KEMMFDurationInfoValid )
				{
				expectedDurationInfo = EMMFDurationInfoValid;
				}
			else if( expectedDurationState == KEMMFDurationInfoUnknown )
				{
				expectedDurationInfo = EMMFDurationInfoUnknown;
				}
			else if( expectedDurationState == KEMMFDurationInfoInfinite )
				{
				expectedDurationInfo = EMMFDurationInfoInfinite;
				}
			else
				{
	        	ERR_PRINTF2(_L("Invalid expected duration state (%S)"), &expectedDurationState);
				SetBlockResult(EFail);
				}

			if( expectedDurationInfo != actualDurationInfo )
				{
	        	ERR_PRINTF2(_L("expectedDurationInfo (%S) != actualDurationInfo"), &expectedDurationState);
				SetBlockResult(EFail);
				}
			}

		if( GetIntFromConfig(aSection, KTime(), expectedDuration) )
			{
			if( expectedDuration != actualDuration.Int64() )
				{
	        	ERR_PRINTF3(_L("expectedDuration (%d) != actualDuration (%d)"), expectedDuration, actualDuration.Int64());
				SetBlockResult(EFail);
				}
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetPlayWindow( const TDesC& aSection)
	{

	TInt inputStart=0;
	GetIntFromConfig(aSection, KPlayWindowStart(), inputStart);

	TInt inputEnd=0;
	GetIntFromConfig(aSection, KPlayWindowEnd(), inputEnd);

	const TTimeIntervalMicroSeconds timeStart(inputStart);
	const TTimeIntervalMicroSeconds timeEnd(inputEnd);

	TInt	error = iPlayerUtility->SetPlayWindow(timeStart, timeEnd);
	if ( error != KErrNone )
		{
		ERR_PRINTF2(_L("SetPlayWindow Error %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF3(_L("SetPlayWindow Start =%d, End =%d"), inputStart, inputEnd);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdClearPlayWindow()
	{
	TInt	error = iPlayerUtility->ClearPlayWindow();
	if ( error != KErrNone )
		{
		ERR_PRINTF2(_L("ClearPlayWindow Error %d"), error);
		SetError(error);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetRepeats( const TDesC& aSection)
	{

	TInt inputNumber=0;
	if ( !GetIntFromConfig(aSection, KRepeatNumberOfTimes(), inputNumber) )
		{
		TPtrC   repeatStr;
		if ( GetStringFromConfig(aSection, KRepeatNumberOfTimes(), repeatStr) )
			{
			if ( repeatStr==KRepeatForever )
				{
				inputNumber=KMdaRepeatForever;
				}
		    else
				{
				TInt repeatInt=0;
				if ( GetIntFromConfig(aSection, KRepeatNumberOfTimes(), repeatInt) )
					{
					inputNumber=repeatInt;
					}
				else
					{
					ERR_PRINTF2(_L("Illegal repeat parameter given %S"), &repeatStr);
					SetBlockResult(EFail);
					}
				}
			}
		}

	TInt inputSilence=0;
	GetIntFromConfig(aSection, KTrailingSilence(), inputSilence);

	TTimeIntervalMicroSeconds	timeTrailingSilence = inputSilence;
	iPlayerUtility->SetRepeats(inputNumber, timeTrailingSilence);
	INFO_PRINTF3(_L("SetRepeats RepeatNumberOfTimes =%d, TrailingSilence =%d"), inputNumber, inputSilence);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetVolumeRamp( const TDesC& aSection)
	{

	TInt inputDuration=0;
	GetIntFromConfig(aSection, KRampDuration(), inputDuration);

	TTimeIntervalMicroSeconds	timeDuration = inputDuration;
	iPlayerUtility->SetVolumeRamp(timeDuration);
	INFO_PRINTF2(_L("SetVolumeRamp RampDuration =%d"), inputDuration);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetBitRate( const TDesC& aSection)
	{
	TUint	currentBitRate;
	TInt	error = iPlayerUtility->GetBitRate(currentBitRate);
	if ( error != KErrNone )
		{
		ERR_PRINTF2(_L("GetBitRate. Error %d"), error);
		SetError(error);
		}
	else
		{
		TInt inputBitRate=0;
		if( GetIntFromConfig(aSection, KBitRate(), inputBitRate) )
			{
			if( (TInt)currentBitRate != inputBitRate)
				{
				ERR_PRINTF3(_L("Current BitRate (%d) != inputBiteRate (%d)"), currentBitRate, inputBitRate);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF3(_L("Current BitRate (%d) == inputBiteRate (%d)"), currentBitRate, inputBitRate);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Current BitRate = %d"), currentBitRate);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetMetaDataEntryL( const TDesC& aSection)
	{

	//	Get index from ini file
	TInt	indexMetaData=0;
	GetIntFromConfig(aSection, KMetaDataIndex(), indexMetaData);

	CMMFMetaDataEntry*	entry=NULL;
	TRAPD(err, entry = iPlayerUtility->GetMetaDataEntryL(indexMetaData));
	CleanupStack::PushL(entry);
	if( (err == KErrNotFound) || (err == KErrNotSupported) )
		{
		WARN_PRINTF1(_L("The meta data entry does not exist."));
		}
	if ( err != KErrNone )
		{
		ERR_PRINTF3(_L("GetMetaDataEntryL(%d) error = %d"), indexMetaData, err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF4(_L("GetMetaDataEntryL(%d), Name = %S, value = %S"), indexMetaData, &entry->Name(), &entry->Value());
		}
	CleanupStack::PopAndDestroy(entry);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetNumberOfMetaDataEntries( const TDesC& aSection)
	{

	TInt	currentNbrEntries;
	TInt	error = iPlayerUtility->GetNumberOfMetaDataEntries(currentNbrEntries);
	if ( error != KErrNone )
		{
		ERR_PRINTF2(_L("GetNumberOfMetaDataEntries , Error %d"), error);
		SetError(error);
		}
	else
		{
		TInt	inputNbrEntries=0;
		if( GetIntFromConfig(aSection, KNbrEntries(), inputNbrEntries) )
			{
			if( currentNbrEntries != inputNbrEntries)
				{
				ERR_PRINTF3(_L("Current NbrEntries (%d) != inputNbrEntries (%d)"), currentNbrEntries, inputNbrEntries);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF3(_L("Current NbrEntries (%d) != inputNbrEntries (%d)"), currentNbrEntries, inputNbrEntries);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Current BitRate = %d"), currentNbrEntries);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdRegisterAudioResourceNotificationL( const TDesC& aSection)
	{

	TInt	inputUid=0;
	if ( GetHexFromConfig(aSection, KEventUID(), inputUid))
		{
		HBufC8*	data8=NULL;
		TPtrC	data;
		if ( GetStringFromConfig(aSection, KMimeType(), data) )
			{
			data8=HBufC8::NewL(data.Length());
			data8->Des().Copy(data);
			}
		else
			{
			data8=HBufC8::NewL(0);
			data8->Des().Copy(KNullDesC8);
			}

		TUid	id = TUid::Uid(inputUid);
		TInt	error = iPlayerUtility->RegisterAudioResourceNotification(*this, id, *data8);
		if ( error != KErrNone )
			{
			ERR_PRINTF2(_L("RegisterAudioResourceNotification Error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF2(_L("RegisterAudioResourceNotification OK, dataNotification = %S"), &data);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error parameter %S"), &KEventUID);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdCancelRegisterAudioResourceNotification( const TDesC& aSection)
	{

	TInt	inputUid=0;
	if( GetHexFromConfig(aSection, KEventUID(), inputUid))
		{
		TUid	id = TUid::Uid(inputUid);
		TInt	error = iPlayerUtility->CancelRegisterAudioResourceNotification(id);
		if ( error != KErrNone )
			{
			ERR_PRINTF2(_L("CancelRegisterAudioResourceNotification Error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF1(_L("CancelRegisterAudioResourceNotification OK."));
			}
		}
	else
		{
		ERR_PRINTF2(_L("Error parameter %S"), &KEventUID);
		SetBlockResult(EFail);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdRegisterForAudioLoadingNotification()
	{
	INFO_PRINTF1(_L("DoCmdRegisterForAudioLoadingNotification"));
	iPlayerUtility->RegisterForAudioLoadingNotification(*this);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetPriority( const TDesC& aSection)
	{
	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityNormal;
		}

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	iPlayerUtility->SetPriority(priority, priorityPreference);
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdSetThreadPriority( const TDesC& aSection)
	{
	TThreadPriority	threadPriority;
	if ( !CT_MMUtil::ReadThreadPriority(*this, aSection, threadPriority) )
		{
		threadPriority=EPriorityNormal;
        }

    TInt	err=iPlayerUtility->SetThreadPriority(threadPriority);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioPlayerUtility::SetThreadPriority failed with error %d."), err);
		SetError(err);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetAudioLoadingProgressL( const TDesC& aSection)
	{
	TInt 								percentageComplete 		=0;

	TRAPD(err, iPlayerUtility->GetAudioLoadingProgressL(percentageComplete));
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("GetAudioLoadingProgressL Error %d"), err);
		SetError(err);
		}
	else
		{
		TInt	inputPercentageComplete =0;

		if ( GetIntFromConfig(aSection, KProgress(), inputPercentageComplete) )
			{
			if( percentageComplete != inputPercentageComplete)
				{
				ERR_PRINTF3(_L("Current percentage (%d) != input percentage (%d)"), percentageComplete, inputPercentageComplete);
				SetBlockResult(EFail);
				}
			else
				{
				INFO_PRINTF3(_L("Current percentage (%d) == input percentage (%d)"), percentageComplete, inputPercentageComplete);
				}
			}
		else
			{
			INFO_PRINTF2(_L("GetAudioLoadingProgressL=%d"), percentageComplete);
			}
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdGetDRMCustomCommand( const TDesC& aSection)
	{
	TBool  							    inputIsCommand 	= ETrue;

	GetBoolFromConfig(aSection, KDRMCustomCommand(), inputIsCommand);

	MMMFDRMCustomCommand*	command = iPlayerUtility->GetDRMCustomCommand();

	if( command && !inputIsCommand )
	    {
	    ERR_PRINTF1(_L("GetDRMCustomCommand() is not null"));
	    SetBlockResult(EFail);
	    }
	else if( !command && inputIsCommand )
	    {
	    ERR_PRINTF1(_L("GetDRMCustomCommand() is null"));
	    SetBlockResult(EFail);
	    }
	else
	    {
	    INFO_PRINTF1(_L("GetDRMCustomCommand() called OK"));
	    }
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdControllerImplementationInformationL()
	{
	const CMMFControllerImplementationInformation* controllerInfo = NULL;

	TRAPD(err, controllerInfo = &(iPlayerUtility->ControllerImplementationInformationL()));

	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CMdaAudioConvertUtility::ControllerImplementationInformationL failed with error %d."), err);
		SetError(err);
		}
	else
		{
		CT_MMUtil::LogL(*this, *controllerInfo);
		}
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdCustomCommandSyncL( const TDesC& aSection)
	{

	TMMFMessageDestination		handleInfo(iAudioControllerUid);
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
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if (GetStringFromConfig(aSection, KDataTo2(), dataTo2Str))
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	TInt 					err = KErrNone;
	TBuf8<KMaxdataFromText>	dataFromText;
	if ( useDataFrom )
		{
		err = iPlayerUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2, dataFromText);
		}
	else
		{
		err = iPlayerUtility->CustomCommandSync(messageDest, function, *dataTo1, *dataTo2);
		}
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("CustomCommandSync failed with error %d."), err);
		SetError(err);
		}
	else
		{
		if (useDataFrom)
			{
			HBufC*	buffer=HBufC::NewLC(dataFromText.Length());
			TPtr	bufferPtr=buffer->Des();
			bufferPtr.Copy(dataFromText); // Create a 16 bit copy of the 8 bit original
			INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);
			CleanupStack::PopAndDestroy(buffer);
			}
		}

	CleanupStack::PopAndDestroy(2, dataTo1);	// dataTo1, dataTo2
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdCustomCommandAsyncL( const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TMMFMessageDestination		handleInfo(iAudioControllerUid);
	TMMFMessageDestinationPckg	messageDest(handleInfo);

	TInt	function = 0;
	GetIntFromConfig(aSection, KFunction(), function);

	TBool	useDataFrom = EFalse;
	GetBoolFromConfig(aSection, KUseDataFrom(), useDataFrom);

	// reference to the first chunk of data
	HBufC8*	dataTo1 = NULL;
	TPtrC	dataTo1Str;
	if ( GetStringFromConfig(aSection, KDataTo1(), dataTo1Str) )
		{
		dataTo1 = HBufC8::NewL(dataTo1Str.Length());
		dataTo1->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo1 = HBufC8::NewL(0);
		dataTo1->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo1);

	// reference to the second chunk of data
	HBufC8*	dataTo2 = NULL;
	TPtrC	dataTo2Str;
	if ( GetStringFromConfig(aSection, KDataTo2(), dataTo2Str) )
		{
		dataTo2 = HBufC8::NewL(dataTo2Str.Length());
		dataTo2->Des().Copy(dataTo1Str);
		}
	else
		{
		dataTo2 = HBufC8::NewL(0);
		dataTo2->Des().Copy(KNullDesC8);
		}
	CleanupStack::PushL(dataTo2);

	if ( useDataFrom )
		{
		iPlayerUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iDataFromText, iActiveCustomCommandAsync->iStatus);
		}
	else
		{
		iPlayerUtility->CustomCommandAsync(messageDest, function, *dataTo1, *dataTo2, iActiveCustomCommandAsync->iStatus);
		}

	iActiveCustomCommandAsync->Activate(aAsyncErrorIndex);
	IncOutstanding();
	CleanupStack::PopAndDestroy(2, dataTo1);	// dataTo1, dataTo2
	}

void CT_MMDataMdaAudioPlayerUtility::DoCmdUseSharedHeap()
	{
	iPlayerUtility->UseSharedHeap();
	}

void CT_MMDataMdaAudioPlayerUtility::RunL(CActive* aActive, TInt aIndex)
	{
 	if ( aActive==iActiveCustomCommandAsync )
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
 Virtual DoCancel - Request to cancel the aAsyncErrorIndex command
 @internalComponent
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
void CT_MMDataMdaAudioPlayerUtility::DoCancel(CActive* aActive)
	{
 	if ( aActive==iActiveCustomCommandAsync )
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
 void CT_MMDataMdaAudioPlayerUtility::RunCustomCommandAsyncL(TInt aIndex)
	{
	HBufC*	buffer=HBufC::NewLC(iDataFromText.Length());
	TPtr	bufferPtr=buffer->Des();
	bufferPtr.Copy(iDataFromText); // Create a 16 bit copy of the 8 bit original
	INFO_PRINTF2(_L("DataFrom: %S"), &bufferPtr);

	TInt	err = iActiveCustomCommandAsync->iStatus.Int();
	if ( err != KErrNone )
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
void CT_MMDataMdaAudioPlayerUtility::DoCancelCustomCommandAsync()
	{
	INFO_PRINTF1(_L("DoCancelCustomCommandAsync"));
	}

void CT_MMDataMdaAudioPlayerUtility::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	if ( aError == KErrNone )
		{
		INFO_PRINTF2(_L("Initialise of the CMdaAudioPlayerUtility object is finished OK. Duration = %d"), aDuration.Int64());
		}
	else
		{
		ERR_PRINTF2(_L("Error the initialise of the CMdaAudioPlayerUtility object. Error =  %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	DecOutstanding();
	}

void CT_MMDataMdaAudioPlayerUtility::MapcPlayComplete(TInt aError)
	{
	if( aError != KErrNone)
		{
		ERR_PRINTF2(_L("MapcPlayComplete aError = %d"), aError);

		SetAsyncError(iAsyncErrorIndex, aError);
		}
	else
		{
		INFO_PRINTF1(_L("Playback of the audio sample is finished"));
		}
	iPlaying=EFalse;
	DecOutstanding();
	}

void CT_MMDataMdaAudioPlayerUtility::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	INFO_PRINTF1(_L("CT_MdaAudioPlayerUtilityTestStep::MarncResourceAvailable"));

	if ( aNotificationEventId == KMMFEventCategoryAudioResourceAvailable )
		{

		INFO_PRINTF1(_L("NotificationEventId == KMMFEventCategoryAudioResourceAvailable"));

		//	Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg	timeIntvbuf;
		timeIntvbuf.Copy( aNotificationData );

		TTimeIntervalMicroSeconds	intPos = timeIntvbuf();
		INFO_PRINTF2(_L("Interval %d"), intPos.Int64());

		DecOutstanding();
		}
	else
		{
		INFO_PRINTF1(_L("MarncResourceAvailable, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

void CT_MMDataMdaAudioPlayerUtility::MaloLoadingStarted()
	{
	INFO_PRINTF1(_L("MaloLoadingStarted"));
	DecOutstanding();
	}

void CT_MMDataMdaAudioPlayerUtility::MaloLoadingComplete()
	{
	INFO_PRINTF1(_L("MaloLoadingComplete"));
	DecOutstanding();
	}
