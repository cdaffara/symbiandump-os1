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


#include "T_MMDataMdaAudioOutputStream.h"
#include "T_MMUtil.h"

/*@{*/
_LIT(KAudioCapsP1,									"audioCapsP1");
_LIT(KAudioCapsP2,									"audioCapsP2");
_LIT(KBufferSize,									"buffersize");
_LIT(KInputFile,									"inputfile");
_LIT(KBalance,										"balance");
_LIT(KBytes,										"bytes");
_LIT(KVolume,										"volume");

_LIT(KCmdDestructor,								"~");
_LIT(KCmdCancelRegisterAudioResourceNotification,	"CancelRegisterAudioResourceNotification");
_LIT(KCmdCustomInterface,							"CustomInterface");
_LIT(KCmdDataType,									"DataType");
_LIT(KCmdGetBalanceL,								"GetBalanceL");
_LIT(KCmdGetBytes,									"GetBytes");
_LIT(KCmdMaxVolume,									"MaxVolume");
_LIT(KCmdNewL,										"NewL");
_LIT(KCmdOpen,										"Open");
_LIT(KCmdPosition,									"Position");
_LIT(KCmdRegisterAudioResourceNotification,			"RegisterAudioResourceNotification");
_LIT(KCmdSetAudioPropertiesL,						"SetAudioPropertiesL");
_LIT(KCmdSetBalanceL,								"SetBalanceL");
_LIT(KCmdSetDataTypeL,								"SetDataTypeL");
_LIT(KCmdSetPriority,								"SetPriority");
_LIT(KCmdSetVolume,									"SetVolume");
_LIT(KCmdStop,										"Stop");
_LIT(KCmdVolume,									"Volume");
_LIT(KCmdWillResumePlay,							"WillResumePlay");
_LIT(KCmdWriteL,									"WriteL");
/*@}*/

const TInt	KMdaAudioOutputStreamBufferSize=1024;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMDataMdaAudioOutputStream* CT_MMDataMdaAudioOutputStream::NewL()
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
	{
	CT_MMDataMdaAudioOutputStream*	ret=new (ELeave) CT_MMDataMdaAudioOutputStream();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_MMDataMdaAudioOutputStream::CT_MMDataMdaAudioOutputStream()
/**
 * Protected constructor. First phase construction
 */
:	iOutputStream(NULL)
,	iBuffer(NULL)
,	iBufferPtr(NULL)
,	iBufferSize(KMdaAudioOutputStreamBufferSize)
,	iStreamOpened(EFalse)
,	iFileOpened(EFalse)
,	iIsAllDataWritten(EFalse)
,	iAsyncErrorIndex(0)
	{
	}

void CT_MMDataMdaAudioOutputStream::ConstructL()
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
	}

CT_MMDataMdaAudioOutputStream::~CT_MMDataMdaAudioOutputStream()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

TAny* CT_MMDataMdaAudioOutputStream::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iOutputStream;
	}

void CT_MMDataMdaAudioOutputStream::DestroyData()
	{
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	iFs.Close();
	delete iBufferPtr;
	iBufferPtr=NULL;
	delete iBuffer;
	iBuffer=NULL;
	delete iOutputStream;
	iOutputStream=NULL;
	iAsyncErrorIndex = 0;
	}

TBool CT_MMDataMdaAudioOutputStream::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	else if ( aCommand==KCmdCancelRegisterAudioResourceNotification )
		{
		DoCmdCancelRegisterAudioResourceNotification();
		}
	else if ( aCommand==KCmdCustomInterface )
		{
		DoCmdCustomInterface();
		}
	else if ( aCommand==KCmdSetBalanceL )
		{
		DoCmdSetBalanceL(aSection);
		}
	else if ( aCommand==KCmdGetBalanceL )
		{
		DoCmdGetBalanceL(aSection);
		}
	else if ( aCommand==KCmdGetBytes )
		{
		DoCmdGetBytes(aSection);
		}
	else if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdPosition )
		{
		DoCmdPosition();
		}
	else if ( aCommand==KCmdRegisterAudioResourceNotification )
		{
		DoCmdRegisterAudioResourceNotification();
		}
	else if ( aCommand==KCmdSetAudioPropertiesL )
		{
		DoCmdSetAudioPropertiesL(aSection);
		}
	else if ( aCommand==KCmdSetDataTypeL )
		{
		DoCmdSetDataTypeL(aSection);
		}
	else if ( aCommand==KCmdDataType )
		{
		DoCmdDataTypeL(aSection);
		}
	else if ( aCommand==KCmdSetPriority )
		{
		DoCmdSetPriority(aSection);
		}
	else if ( aCommand==KCmdSetVolume )
		{
		DoCmdSetVolume(aSection);
		}
	else if ( aCommand==KCmdMaxVolume )
		{
		DoCmdMaxVolume();
		}
	else if ( aCommand==KCmdVolume )
		{
		DoCmdVolume(aSection);
		}
	else if ( aCommand==KCmdStop )
		{
		DoCmdStop();
		}
	else if ( aCommand==KCmdWillResumePlay )
		{
		DoCmdWillResumePlay();
		}
	else if ( aCommand==KCmdWriteL )
		{
		DoCmdWriteL(aSection, aAsyncErrorIndex);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


void CT_MMDataMdaAudioOutputStream::DoCmdDestructor()
	{
	DestroyData();
	}


void CT_MMDataMdaAudioOutputStream::DoCmdCancelRegisterAudioResourceNotification()
	{
	TInt	err=iOutputStream->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CancelRegisterAudioResourceNotification Error %d"), err);
		SetError(err);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdCustomInterface()
	{
	TAny*	any=iOutputStream->CustomInterface(KNullUid);
	if ( any!=NULL )
		{
		ERR_PRINTF2(_L("Non-NULL return value %0x"), any);
		SetBlockResult(EFail);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdSetBalanceL(const TDesC& aSection)
	{

	TInt	balance=KMMFBalanceCenter;
	GetIntFromConfig(aSection, KBalance(), balance);
	TRAPD(err, iOutputStream->SetBalanceL(balance));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetBalanceL Error %d"), err);
		SetError(err);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdGetBalanceL(const TDesC& aSection)
	{

	TInt	actualBalance = 0;

	TRAPD(err, actualBalance = iOutputStream->GetBalanceL() );
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("GetBalanceL Error %d"), err);
		SetError(err);
		}
	else
		{
		TInt expectedBalance = 0;
		if(GetIntFromConfig(aSection, KBalance(), expectedBalance))
			{
			INFO_PRINTF3(_L("actualBalance=%d expectedBalance=%d"), actualBalance, expectedBalance);
			if(actualBalance != expectedBalance)
				{
				ERR_PRINTF1(_L("Balance does not meet expected value!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF2(_L("GetBalanceL=%d"), actualBalance);
			}
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdGetBytes(const TDesC& aSection)
	{

	TInt	actualBytes = iOutputStream->GetBytes();
	TInt	expectedBytes = 0;
	if(GetIntFromConfig(aSection, KBytes(), expectedBytes))
		{
		INFO_PRINTF3(_L("actualBytes=%d expectedBytes=%d"), actualBytes, expectedBytes);
		if(actualBytes != expectedBytes)
			{
			ERR_PRINTF1(_L("Bytes does not meet expected value!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF2(_L("GetBytes=%d"), actualBytes);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdNewL(const TDesC& aSection)
	{
	delete iOutputStream;
	iOutputStream=NULL;

	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		TRAPD(err, iOutputStream=CMdaAudioOutputStream::NewL(*this, priority, priorityPreference));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this, priority, priorityPreference) Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		TRAPD(err, iOutputStream=CMdaAudioOutputStream::NewL(*this));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewL(this) Error %d"), err);
			SetError(err);
			}
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdOpen(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	//	Open the stream
	TMdaAudioDataSettings	settings;
	iAsyncErrorIndex = aAsyncErrorIndex;
	if ( CT_MMUtil::ReadAudioDataSettings(*this, settings, aSection) )
		{
		iOutputStream->Open(&settings);
		}
	else
		{
		iOutputStream->Open(NULL);
		}
	IncOutstanding();
	}


void CT_MMDataMdaAudioOutputStream::DoCmdPosition()
	{
	const TTimeIntervalMicroSeconds&	position=iOutputStream->Position();
	INFO_PRINTF2(_L("Position=%d"), position.Int64());
	}


void CT_MMDataMdaAudioOutputStream::DoCmdRegisterAudioResourceNotification()
	{
	TInt	err=iOutputStream->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RegisterAudioResourceNotification Error %d"), err);
		SetError(err);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdSetAudioPropertiesL(const TDesC& aSection)
	{
	TPtrC	paramStr;

	TMdaAudioDataSettings::TAudioCaps sampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
	TBool unknownSampleRate = EFalse;
	if( GetStringFromConfig(aSection, KAudioCapsP1(), paramStr))
		{
		if( !CT_MMUtil::ConvertToAudioCaps(paramStr, sampleRate) )
			{
			TInt otherSampleRate = 0;
			if ( GetIntFromConfig(aSection, KAudioCapsP1(), otherSampleRate) )
				{
				sampleRate = (TMdaAudioDataSettings::TAudioCaps) otherSampleRate;
				unknownSampleRate = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("SetAudioPropertiesL cannot parse %S"), &KAudioCapsP1());
				SetBlockResult(EFail);
				}
			}
		}

	TMdaAudioDataSettings::TAudioCaps channels = TMdaAudioDataSettings::EChannelsMono;
	TBool unknownChannels = EFalse;
	if( GetStringFromConfig(aSection, KAudioCapsP2(), paramStr))
		{
		if( !CT_MMUtil::ConvertToAudioCaps(paramStr, channels) )
			{
			TInt otherChannels = 0;
			if ( GetIntFromConfig(aSection, KAudioCapsP2(), otherChannels) )
				{
				channels = (TMdaAudioDataSettings::TAudioCaps) otherChannels;
				unknownChannels = ETrue;
				}
			else
				{
				ERR_PRINTF2(_L("SetAudioPropertiesL cannot parse %S"), &KAudioCapsP2());
				SetBlockResult(EFail);
				}
			}
		}

	TRAPD(err, iOutputStream->SetAudioPropertiesL((TInt)sampleRate, (TInt)channels));
	if (( err==KErrNotSupported ) && ( !unknownSampleRate ) && ( !unknownChannels ))
		{
		WARN_PRINTF1(_L("SetAudioPropertiesL not supported"));
		}
	else if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("SetAudioPropertiesL Error %d"), err);
		SetError(err);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdSetDataTypeL(const TDesC& aSection)
	{
	TFourCC	val;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, val) )
		{
		TRAPD(err, iOutputStream->SetDataTypeL(val));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("SetDataTypeL Error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1(_L("No data type"));
		SetBlockResult(EFail);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdDataTypeL(const TDesC& aSection)
	{
	TFourCC	actualFourCC=iOutputStream->DataType();
	INFO_PRINTF2(_L("DataType=%d"), actualFourCC.FourCC());

	TFourCC	expectedVal;
	if ( CT_MMUtil::ReadFourCC(*this, aSection, expectedVal) )
		{
		if ( expectedVal!=actualFourCC )
			{
			ERR_PRINTF3(_L("dataType(%d) != expectedDataType(%d)"), actualFourCC.FourCC(), expectedVal.FourCC());
			SetBlockResult(EFail);
			}
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdSetPriority(const TDesC& aSection)
	{
	TMdaPriorityPreference	priorityPreference;
	if ( !CT_MMUtil::ReadMdaPriorityPreference(*this, aSection, priorityPreference) )
		{
		priorityPreference=EMdaPriorityPreferenceNone;
		}

	TInt	priority;
	if ( !CT_MMUtil::ReadMdaPriority(*this, aSection, priority) )
		{
		priority=EMdaPriorityMin;
		}
	iOutputStream->SetPriority(priority, priorityPreference);
	}


void CT_MMDataMdaAudioOutputStream::DoCmdSetVolume(const TDesC& aSection)
	{

	TInt	volume=0;
	GetIntFromConfig(aSection, KVolume(), volume);
	INFO_PRINTF2(_L("SetVolume(%d)"), volume);
	iOutputStream->SetVolume(volume);
	}


void CT_MMDataMdaAudioOutputStream::DoCmdMaxVolume()
	{
	TInt	maxVolume=iOutputStream->MaxVolume();
	INFO_PRINTF2(_L("MaxVolume=%d"), maxVolume);
	}


void CT_MMDataMdaAudioOutputStream::DoCmdVolume(const TDesC& aSection)
	{
	TInt	actualVolume=iOutputStream->Volume();
	INFO_PRINTF2(_L("Volume=%d"), actualVolume);

	TInt	expectedVolume=0;
	if(GetIntFromConfig(aSection, KVolume(), expectedVolume))
		{
		TInt	maxVol=iOutputStream->MaxVolume();
		if ( expectedVolume>maxVol )
			{
			expectedVolume=maxVol;
			}
		if(actualVolume != expectedVolume)
			{
			SetBlockResult(EFail);
			ERR_PRINTF3(_L("actualVolume(%d) != expectedVolume(%d)"), actualVolume, expectedVolume);
			}
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdStop()
	{
	iOutputStream->Stop();
	}


void CT_MMDataMdaAudioOutputStream::DoCmdWillResumePlay()
	{
	TInt	err=iOutputStream->WillResumePlay();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("WillResumePlay Error %d"), err);
		SetError(err);
		}
	}


void CT_MMDataMdaAudioOutputStream::DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	GetIntFromConfig(aSection, KBufferSize(), iBufferSize);
	delete iBuffer;
	iBuffer=NULL;
	iBuffer=HBufC8::NewL(iBufferSize);
	iAsyncErrorIndex = aAsyncErrorIndex;

	TPtrC	inputFile;
	if ( GetStringFromConfig(aSection, KInputFile(), inputFile) )
		{
		TInt	err=iFile.Open(iFs, inputFile, EFileRead);
		if ( err==KErrNone )
			{
			INFO_PRINTF1(_L("File opened"));
			iFileOpened=ETrue;
			if ( WriteStreamAudioDataL() )
				{
				IncOutstanding();
				}
			}
		else
			{
			ERR_PRINTF3(_L("Cannot open inputfile %S. Error %d"), &inputFile, err);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("No %S"), &KInputFile());
		SetBlockResult(EFail);
		}
	}


/**
aAsyncErrorIndex call back in response to a call to CMdaAudioOutputStream::Open
@internalComponent
@return void
@param aError KErrNone if successfull - otherwise a standard Symbian platform error code
@pre N/A
@post On success audio output stream has been opened ok
*/
void CT_MMDataMdaAudioOutputStream::MaoscOpenComplete(TInt aError)
	{
	if ( aError==KErrNone )
		{
		iStreamOpened=ETrue;
		INFO_PRINTF1(_L("Open OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Open Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		iStreamOpened=EFalse;
		}
	DecOutstanding();
	}


/**
aAsyncErrorIndex call back in response to a call to CMdaAudioOutputStream::Stop
@internalComponent
@return void
@param aError KErrNone if successfull - otherwise a standard Symbian platform error code
@pre N/A
@post On success audio output stream has been played ok
*/
void CT_MMDataMdaAudioOutputStream::MaoscPlayComplete(TInt aError)
	{
	if ( (iIsAllDataWritten) && (aError==KErrUnderflow) )
		{
		aError = KErrNone;
		}
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Play OK"));
		}
	else
		{
		ERR_PRINTF2(_L("Play Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}
	if ( iFileOpened )
		{
		iFile.Close();
		iFileOpened=EFalse;
		}
	DecOutstanding();
	}


/**
aAsyncErrorIndex call back in response to a call to CMdaAudioOutputStream::WriteL
@internalComponent
@return void
@param aError KErrNone if successfull - otherwise a standard Symbian platform error code
@pre N/A
@post On success audio output stream has been opened ok
*/
void CT_MMDataMdaAudioOutputStream::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	TBool	moreToDo=EFalse;
	if ( aError==KErrNone )
		{
		INFO_PRINTF1(_L("Copied OK"));
		TRAPD(err, moreToDo=WriteStreamAudioDataL());
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("WriteStreamAudioDataL Error %d"), err);
			SetAsyncError(iAsyncErrorIndex, err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Buffer Copied Error %d"), aError);
		SetAsyncError(iAsyncErrorIndex, aError);
		}

	if ( !moreToDo )
		{
		if ( iFileOpened )
			{
			iFile.Close();
			iFileOpened=EFalse;
			}
		DecOutstanding();
		}
	}


/**
MMMFAudioResourceNotificationCallback call back Called when the audio resource
becomes available but only if the resource was previously unavailable (e.g. if
the audio resource is available when the client registers, then this callback
is not received).
@internalComponent
@return void
TUid aNotificationEventId, const TDesC8& aNotificationData
@param aNotificationEventId The UID of the event that is being handled.
@param aNotificationData The point from where the audio can be resumed.
@pre N/A
@post N/A
*/
void CT_MMDataMdaAudioOutputStream::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	if ( aNotificationEventId==KMMFEventCategoryAudioResourceAvailable )
		{
		//	Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg	timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds			intPos = timeIntvbuf();
		INFO_PRINTF2(_L("Interval %d"), intPos.Int64());
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}


/**
Utility helper function WriteStreamAudioDataL - writes audio data to a stream
@return void
@param None
@pre N/A
@post contents of iBuffer written to output stream
*/
TBool CT_MMDataMdaAudioOutputStream::WriteStreamAudioDataL()
	{
	TBool	ret=EFalse;
	delete iBufferPtr;
	iBufferPtr=new (ELeave) TPtr8(iBuffer->Des());
	TInt	err=iFile.Read(*iBufferPtr, iBufferSize);
	if ( err==KErrNone )
		{
		if ( iBufferPtr->Length() > 0 )
			{
			ret=ETrue;
			iOutputStream->WriteL(*iBufferPtr);
			}
		else
			{
			iIsAllDataWritten=ETrue;
			}
		}
	else
		{
		INFO_PRINTF2(_L("File read error %d"), err);
		iIsAllDataWritten=ETrue;
		}
	return ret;
	}
