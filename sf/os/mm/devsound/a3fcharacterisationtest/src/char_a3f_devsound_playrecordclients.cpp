// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// char_a3f_devsound_playrecordclients.h
// 
//

// user includes
#include "char_a3f_devsound_playrecordclients.h"

CA3FDevSoundPlayClient::CA3FDevSoundPlayClient(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
	: 	iDevSoundState(EStateInitial), 
		iDevSound(NULL), 
		iObserver(aObserver),
		iFilename(KNullDesC),
		iFs(aFs),
		iFourccCode(aFourccCode),
		iSuite(aSuite)
	{
	}

CA3FDevSoundPlayClient::~CA3FDevSoundPlayClient()
	{
	if(iDevSound)
		{
		delete iDevSound;
		}
	iFile.Close();
	}

CA3FDevSoundPlayClient* CA3FDevSoundPlayClient::NewL(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
	{
	CA3FDevSoundPlayClient* self = new (ELeave) CA3FDevSoundPlayClient(aObserver,aFourccCode ,aFs, aSuite);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CA3FDevSoundPlayClient::ConstructL()
	{   
	iDevSound = CMMFDevSound::NewL();
	iDevSoundState = EStateCreated;
	}

void CA3FDevSoundPlayClient::SetPriority(TInt priority)
	{   
	TMMFPrioritySettings settings;
	settings.iPriority = priority;
	settings.iPref = EMdaPriorityPreferenceTime;
	iDevSound->SetPrioritySettings(settings);
	}

TInt CA3FDevSoundPlayClient::PlayInit(const TDesC& aFileName)
	{
	iFilename.Copy(aFileName);
	TInt err  = iFile.Open(iFs, iFilename, EFileRead);
	return Fsm(EEventInitialize);
	}

TInt CA3FDevSoundPlayClient::PlayAudio()
	{
	return Fsm(EEventInitComplete);
	}

void CA3FDevSoundPlayClient::StopPlay()
	{
	iDevSound->Stop();
	}

void CA3FDevSoundPlayClient::DeleteClient()
	{
	delete iDevSound;
	iDevSound = NULL;
	}

void CA3FDevSoundPlayClient::InitializeComplete(TInt aError)
	{
	iObserver.ClientInitializeCompleteCallback(aError);
	}

void CA3FDevSoundPlayClient::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	TInt err;
	if (!aBuffer)
		{
		err = KErrGeneral;	// Null Buffer was received
		}
	else
		{
		iBuffer = aBuffer;	
		err = Fsm(EEventBTBF);
		}
	iObserver.ClientBufferToBeFilledCallback(err);
	}

void CA3FDevSoundPlayClient::PlayError(TInt aError)
	{
	if (iBuffer)
		{
		iObserver.ClientPlayErrorCallback(aError, iBuffer->LastBuffer());
		}
	else
		{
		iObserver.ClientPlayErrorCallback(aError, EFalse);
		}
	}

TInt CA3FDevSoundPlayClient::Fsm(TMmfDevSoundEvent aDevSoundEvent)
	{
	TInt err;
	switch (iDevSoundState)
		{
		case EStateCreated:
			{
			INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
			if(aDevSoundEvent == EEventInitialize)
				{
				TRAP(err, iDevSound->InitializeL(*this, iFourccCode, EMMFStatePlaying));
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling InitializeL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				err=EInvalidClientFSMEvent;
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iDevSound->SetVolume(iDevSound->MaxVolume());
				INFO_PRINTF1(_L("Calling CMMFDevSound::PlayInitL"));
				TRAP(err, iDevSound->PlayInitL());
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStatePlaying"));
					iDevSoundState = EStatePlaying;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling PlayInitL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				err=EInvalidClientFSMEvent;
				}
			break;
			}
		case EStatePlaying:
			{
			if(aDevSoundEvent == EEventBTBF)
				{
				CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
				err = iFile.Read(buffer->Data());
				if (err != KErrNone)
					{
					err=EReadFileErrorInClient;
					iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EReadFileErrorInClient);
					break;
					}
				if (buffer->Data().Length() != buffer->RequestSize())
					{
					iBuffer->SetLastBuffer(ETrue);
					}
				iDevSound->PlayData();
				}
			else
				{
				iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMEvent);
				}
			break;
			}
		default:
			{
			iObserver.ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMState);
			}
		}
	return err;
	}


void CA3FDevSoundPlayClient::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::RecordError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::ToneFinished(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundPlayClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundPlayClient::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	if(!iSuite)
		{
		return;
		}
	VA_LIST list;
	VA_START(list, aFmt);

	TIntegrationTestLog16OverflowClient overflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, list, &overflow16);

	// send the data to the log system via the suite
	if(LogSeverity::IsActive(aSeverity, iSuite->Severity()))
		iSuite->LogExtra(aFile, aLine, aSeverity, lineBuf);

	VA_END(list); 
	}

/*
 *========================================================================================================
 */

CA3FDevSoundRecordClient::CA3FDevSoundRecordClient(MA3FDevsoundRecordAudioClientObserver &aObserver,TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
: 	iDevSoundState(EStateInitial),
	iObserver(aObserver),
	iFs(aFs),
	iFourccCode(aFourccCode),
	iSuite(aSuite)
	{
	}

CA3FDevSoundRecordClient::~CA3FDevSoundRecordClient()
	{
	if(iDevSound)
		{
		iDevSound->Stop();
		}
	delete iDevSound;
	
	if (iAsyncWriteBTFAO)
		{
		delete iAsyncWriteBTFAO;
		iAsyncWriteBTFAO = NULL;
		}
	
	iFile.Close();
	}

CA3FDevSoundRecordClient* CA3FDevSoundRecordClient::NewL(MA3FDevsoundRecordAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite)
	{
	CA3FDevSoundRecordClient* self = new (ELeave) CA3FDevSoundRecordClient(aObserver, aFourccCode, aFs, aSuite);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CA3FDevSoundRecordClient::ConstructL()
	{   
	iDevSound = CMMFDevSound::NewL();
	iDevSoundState = EStateCreated;
	iAsyncWriteBTFAO = CAsyncWriteBufferToFile2::NewL(iFile, iDevSound);
	}

void CA3FDevSoundRecordClient::SetPriority(TInt priority)
	{   
	TMMFPrioritySettings settings;
	settings.iPriority = priority;
	settings.iPref = EMdaPriorityPreferenceTime;
	iDevSound->SetPrioritySettings(settings);
	}

TInt CA3FDevSoundRecordClient::InitializeRecordClient(const TDesC& aFileName)
	{
	iFilename.Copy(aFileName);
	return Fsm(EEventInitialize);
	}

TInt CA3FDevSoundRecordClient::RecordAudio()
	{
	TInt err = iFile.Replace(iFs, iFilename, EFileWrite);
	if(err)
		{
		return err;
		}
	return Fsm(EEventInitComplete);
	}

void CA3FDevSoundRecordClient::StopRecord()
	{
	INFO_PRINTF1(_L("Calling CMMFDevSound::Pause"));
	iDevSound->Pause();
	iDevSoundState=EStatePause;
	}

void CA3FDevSoundRecordClient::InitializeComplete(TInt aError)
	{
	iObserver.ClientInitializeCompleteCallback(aError);
	}

void CA3FDevSoundRecordClient::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	TInt err;
	if (!aBuffer)
		{
		err = KErrGeneral;	// Null Buffer was received
		}
	if(aBuffer->LastBuffer())
		{
		INFO_PRINTF1(_L("CMMFBuffer::LastBuffer is set"));
		if(iDevSoundState == EStatePause)
			{
			iDevSound->Stop();
			err=KErrNone;
			}
		else
			{
			err=KErrUnknown;
			}
		}
	else
		{
		iBuffer = aBuffer;
		err=Fsm(EEventBTBE);
		}
	iObserver.ClientBufferToBeEmptiedCallback(err);
	}

void CA3FDevSoundRecordClient::RecordError(TInt aError)
	{
	iObserver.ClientRecordErrorCallback(aError);
	}

TInt CA3FDevSoundRecordClient::Fsm(TMmfDevSoundEvent aDevSoundEvent)
	{
	TInt err = KErrNone;
	switch(iDevSoundState)
		{
		case EStateCreated:
			{
			if(aDevSoundEvent == EEventInitialize)
				{
				INFO_PRINTF1(_L("Calling CMMFDevSound::InitializeL"));
				TRAP(err, iDevSound->InitializeL(*this, iFourccCode, EMMFStateRecording));
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStateInitializing"));
					iDevSoundState = EStateInitializing;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling InitializeL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				iObserver.ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMEvent);
				}
			break;
			}
		case EStateInitializing:
			{
			if(aDevSoundEvent == EEventInitComplete)
				{
				iDevSound->SetGain(iDevSound->MaxGain()/2);
				INFO_PRINTF1(_L("Calling CMMFDevSound::RecordInitL"));
				TRAP(err, iDevSound->RecordInitL());
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("DevSound State: EStateRecording"));
					iDevSoundState = EStateRecording;
					break;
					}
				else
					{
					ERR_PRINTF2(_L("Leave Error when calling RecordInitL() on second DevSound instance: %d"),err);
					}
				}
			else
				{
				iObserver.ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMEvent);
				}
			break;
			}
		case EStateRecording:
			{
			if (aDevSoundEvent == EEventBTBE)
				{
				WriteDataToFile();
				}
			break;
			}
		case EStatePause:	
			{
			 if (aDevSoundEvent == EEventBTBE)
				{
				WriteDataToFile();
				}
			else
				{
				iObserver.ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMEvent);
				}
			break;
			}
		default:
			{
			iObserver.ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(EInvalidClientFSMState);
			}
		}
	return err;
	}

void CA3FDevSoundRecordClient::WriteDataToFile()
	{
	CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
	iAsyncWriteBTFAO->Start(buffer);
	}
	
void CA3FDevSoundRecordClient::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundRecordClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundRecordClient::PlayError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundRecordClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundRecordClient::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundRecordClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundRecordClient::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundRecordClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundRecordClient::ToneFinished(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundRecordClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundRecordClient::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	if(!iSuite)
		{
		return;
		}
	VA_LIST list;
	VA_START(list, aFmt);

	TIntegrationTestLog16OverflowClient overflow16;

	// decode formatted data for display on console
	TBuf<KMaxLogLineLength> lineBuf;
	lineBuf.AppendFormatList(aFmt, list, &overflow16);

	// send the data to the log system via the suite
	if(LogSeverity::IsActive(aSeverity, iSuite->Severity()))
		iSuite->LogExtra(aFile, aLine, aSeverity, lineBuf);

	VA_END(list); 
	}

//
// CAsyncWriteBufferToFile2
//

CAsyncWriteBufferToFile2::CAsyncWriteBufferToFile2(RFile& aFile, CMMFDevSound* aDevSound)
:CActive(EPriorityStandard),
 iFile(aFile),
 iDevSound(aDevSound)
	{
	CActiveScheduler::Add(this);
	}

CAsyncWriteBufferToFile2::~CAsyncWriteBufferToFile2()
	{
	Cancel();
	}

CAsyncWriteBufferToFile2* CAsyncWriteBufferToFile2::NewL(RFile& aFile, CMMFDevSound* aDevSound)
	{
	CAsyncWriteBufferToFile2* self = new(ELeave) CAsyncWriteBufferToFile2(aFile, aDevSound);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAsyncWriteBufferToFile2::ConstructL()
	{
	// Nothing to do here
	}

void CAsyncWriteBufferToFile2::RunL()
	{
	//If error occurs then deal with problem in RunError()
	User::LeaveIfError(iStatus.Int());
	// Continue recording data to a buffer
	iDevSound->RecordData();

	}

void CAsyncWriteBufferToFile2::Start(CMMFDataBuffer* aBuffer)
	{
	iFile.Write(aBuffer->Data(), iStatus);
	SetActive();
	}

void CAsyncWriteBufferToFile2::DoCancel()
	{
	// Can't cancel an async write request
	}

TInt CAsyncWriteBufferToFile2::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//
// TIntegrationTestLog16OverflowClient
//

void TIntegrationTestLog16OverflowClient::Overflow(TDes16& aDes)
	{
	aDes = _L("*** ERROR : line too long, cannot log ***");
	}
