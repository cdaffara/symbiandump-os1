// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The test specification for these tests can be found in:
// V:\Dev\GT\GT 0137 Multimedia Codec - Msrvr\Testing\Unit Test Specifications\
// Typhoon\SGL.GT0137.134 Rev1.00 MMF SRSSNK Unit Test Specification.xls 
// Changes made to the test harness as a result of the DevSound WP AudioInput/Output
// modifications have been marked with DevSound WP Change along with an explanation
// of why the code changed
// Main changes caused by:
// 7.0s test seemed to assume the Prime could come after the thread logon
// this assumption is invalid, but happened to work on the 7.0s audio I/O as
// the prime did very little
// assumed Prime does very little including no memory allocation
// assmued that once the active scheduler started no further memory
// was allocated and hence no callbacks if an OOM condition occured during play
// assumed test could drive Empty/FillBufferL calls and could own and set
// buffers - in 8.0s buffers are ultimately owned by HwDevice not test app
// so cannot make multiple calls to Empty/FillBuffer
// assumed audio output could only accept pcm16 - not true on 8.0
// 
//

// EPOC includes
#include <e32base.h>
#include <caf/caf.h>

using namespace ContentAccess;

// Test system includes
#include "TSU_MMF_SRSSNK.h"
#include "TSU_MMF_SRSSNKSuite.h"

 //INC085391
#include "mmfclientutility.h"

// __REQUIREDFORDEBUG macro gets rid of warnings in release builds
#if defined(_DEBUG)
#define __REQUIREDFORDEBUG(X) X
#else
#define __REQUIREDFORDEBUG(X)
#endif

const TInt KStartFileFailAt = 8;	// start at zero when RFile::Duplicate() problem is fixed

_LIT8(KEmptyFourCCCode, "    ");

_LIT(KTest101FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile1.dat");

MDataSource* CreateFileSourceL(TSourceType aSourceType, RFs& aFs, const TDesC& aFileName)
	{
	ASSERT(aSourceType == ESourceTypeFile || aSourceType == ESourceTypeFileHandle);

	MDataSource* source;

	if (aSourceType == ESourceTypeFile)
		{
		TMMFFileConfig configFile;
		configFile().iPath = aFileName;
		source = MDataSource::NewSourceL(KUidMmfFileSource, configFile);
		}
	else
		{
		User::LeaveIfError(aFs.ShareProtected());
		TMMFFileHandleConfig configFile;
		RFile file;
		User::LeaveIfError(file.Open(aFs, aFileName, EFileRead|EFileShareReadersOnly));
		CleanupClosePushL(file);
		configFile().iFile = &file;
		source = MDataSource::NewSourceL(KUidMmfFileSource, configFile);
		CleanupStack::PopAndDestroy(&file);

		}
	return source;
	}

MDataSource* CreateFileHandleSourceL(RFs& aFs, const TDesC& aFileName)
	{
	User::LeaveIfError(aFs.ShareProtected());
	RFile file;
	User::LeaveIfError(file.Open(aFs, aFileName, EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file);
	CMMFileSourceSink* fileSourceSink = CMMFileSourceSink::NewLC(KMMFileHandleSourceUid, file);
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, fileSourceSink->SourceSinkData());
	CleanupStack::PopAndDestroy(2, &file); //fileSourceSink

	return source;
	}

MDataSink* CreateFileSinkL(TSinkType aSinkType, RFs& aFs, const TDesC& aFileName)
	{
	MDataSink* sink;

	if (aSinkType == ESinkTypeFile)
		{
		TMMFFileConfig configFile;
		configFile().iPath = aFileName;
		sink = MDataSink::NewSinkL(KUidMmfFileSink, configFile);
		}
	else
		{
		User::LeaveIfError(aFs.ShareProtected());
		RFile file;
		TMMFFileHandleConfig fileConfig(&file);
		TInt err = file.Open(aFs, aFileName, EFileWrite|EFileRead|EFileShareAny);
		if (err == KErrNotFound)
			{
			User::LeaveIfError(file.Create(aFs, aFileName, EFileWrite|EFileRead|EFileShareAny));
			}
		else
			{
			User::LeaveIfError(err);
			}
		CleanupClosePushL(file);
		sink = MDataSink::NewSinkL(KUidMmfFileSink, fileConfig);
		CleanupStack::PopAndDestroy(&file);

		}
	return sink;
	}



void TTestStep::DoTestPreambleL()
	{
	}

TVerdict TTestStep::DoTestPostambleL(TBool /*aCheck*/)
	{
	return EPass;
	}

TVerdict TTestStep::DoNegativeTestPostambleL()
	{
	return DoTestPostambleL(EFalse);
	}

MDataSource* TTestStep::CreateSourceL(TSourceType aSourceType, HBufC8* aDes)
	{
	MDataSource* source = NULL;
	switch(aSourceType)
		{
		case ESourceTypeDes:
			{
			TPtr8 dataPtr(aDes->Des());
			// Initialise config data.
			TMMFDescriptorConfig configDes;
			configDes().iDes = &dataPtr;
			configDes().iDesThreadId = RThread().Id();
			source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
			}
			break;
		case ESourceTypeFile:
			{
			// Initialise config data.
			TMMFFileConfig configDes;
			configDes().iPath = KTest101FileName;
			source = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
			}
			break;

		case ESourceTypeFileHandle:
			{
			TMMFFileHandleConfig fileConfig;
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			RFile file;
			User::LeaveIfError(file.Open(fs, KTest101FileName, EFileRead|EFileShareReadersOnly));
			CleanupClosePushL(file);
			fileConfig().iFile = &file;
			source = MDataSource::NewSourceL(KUidMmfFileSource, fileConfig);
			CleanupStack::PopAndDestroy(2, &fs);
			}
			break;

		case ESourceTypeAudio:
			{
			TBuf8<1> configDes;
			source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);

			//DevSound WP - needs resetting as of OOM occurs buffer may not be filled ok
			static_cast<CMMFDummySink*>(iSink)->ResetBufferFilledOk();
			}
			break;
		default:
			User::Leave(KErrGeneral);
		}
	return source;
	}


void TReadBufferTestStep::RandomiseBuffer()
	{
	if ((iBuffer->Type() == KUidMmfTransferBuffer) ||
	(iBuffer->Type() == KUidMmfDescriptorBuffer))
		{
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);

		// Re-randomise data buffer.
		TDes8& dataDes = buffer->Data();
		TUint8* dataPtr = CONST_CAST(TUint8*, dataDes.Ptr());
		TInt bufferMaxLength = dataDes.MaxLength();
		for (TInt i = 0 ; i<bufferMaxLength ; i++)
			dataPtr[i] = TUint8(Math::Rand(iSeed));

		dataDes.SetLength(bufferMaxLength);
		}
	}

TInt TReadBufferTestStep::CheckBuffer()
	{
	TInt readLength = iBuffer->BufferSize();
	CMMFDescriptorBuffer* buffer = STATIC_CAST(CMMFDescriptorBuffer*, iBuffer);

	const TUint8* sourcePtr = iSourceDes.Ptr();
	return Mem::Compare(sourcePtr+iPosition, readLength, buffer->Data().Ptr(), readLength);
	}


void TWriteBufferTestStep::RandomiseBuffer()
	{
	if ((iBuffer->Type() == KUidMmfTransferBuffer) ||
	(iBuffer->Type() == KUidMmfDescriptorBuffer))
		{
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);

		// Re-randomise data buffer.
		TDes8& dataDes = buffer->Data();
		TUint8* dataPtr = CONST_CAST(TUint8*, dataDes.Ptr());
		TInt bufferMaxLength = dataDes.MaxLength();
		for (TInt i = 0 ; i<bufferMaxLength ; i++)
			dataPtr[i] = TUint8(Math::Rand(iSeed));
		}
	}

TInt TWriteBufferToDescriptorTestStep::CheckDestinationL()
	{
	TInt length = iDestinationDes.Length();
	if (length != iCopyOfDestinationDes.Length())
		return -1;

	return Mem::Compare(iDestinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	}

TInt TWriteBufferToFileTestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead|EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(length);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);

	return err;
	}

void TWriteBufferToFileTestStep::DoTestPreambleL()
	{
	if (iSink != NULL)
		iSink->SinkPrimeL();
	}

//
// Tests that use ECom to create sources/sinks should call
// REcomSession::FinalClose in the PostambleL.
//
TVerdict TFileTestStep::DoTestPostambleL(TBool /*aCheck*/)
	{
	REComSession::FinalClose();
	return EPass;
	}

// TNewSourceTestStep
// Attempt to create a new data source, using the supplied Uid and config data.
// Delete the source after creation (test for memory leak in construction)
TVerdict TNewSourceTestStep::DoTestStepL()
	{
	MDataSource* source = MDataSource::NewSourceL(iUid, *iConfigDes);

	delete source;
	return EPass;
	}

// TSourceTypeTestStep
// Check that the data source type and data type codes are initialised correctly.
TVerdict TSourceTypeTestStep::DoTestStepL()
	{
	TUid dataSourceType = iSource->DataSourceType();
	if (dataSourceType != iUid)
		return EFail;

	TMediaId mediaId;
	TFourCC fourCC = iSource->SourceDataTypeCode(mediaId);
	if (fourCC != iFourCC)
		return EFail;

	fourCC = 0x4E554C4C; // 'LLUN'
	TInt err = iSource->SetSourceDataTypeCode(fourCC, mediaId);
	if (iCanSetSourceDataType)
		{
		if (err != KErrNone)
			User::Leave(err);

		if (iSource->SourceDataTypeCode(mediaId) != fourCC)
			return EFail;

		User::LeaveIfError(iSource->SetSourceDataTypeCode(iFourCC, mediaId));
		}
	else
		{
		if (err != KErrNotSupported)
			{
			if (err == KErrNone)
				return EFail;

			User::Leave(err);
			}

		if (iSource->SourceDataTypeCode(mediaId) != iFourCC)
			return EFail;
		}

	return EPass;
	}

// TCanCreateSourceBufferTestStep
// Test that MDataSource::CanCreateSourceBuffer returns the expected value. (either ETrue or EFalse)
TVerdict TCanCreateSourceBufferTestStep::DoTestStepL()
	{
	return (iSource->CanCreateSourceBuffer() == iCanCreateSourceBuffer) ? EPass : EFail;
	}

// TSourceSampleConvertTestStep
// Test that MDataSource::SourceSampleConvert returns the expected value. (either ETrue or EFalse)
TVerdict TSourceSampleConvertTestStep::DoTestStepL()
	{
	return (iSource->SourceSampleConvert() == iSourceSampleConvert) ? EPass : EFail;
	}

TInt TCreateSourceBuffer1TestStep::SendEventToClient(const TMMFEvent& aEvent)
	{
	// This callback won't get called for this test
	//	DevSoundWP - yes it will
	if (aEvent.iErrorCode == KErrNoMemory)
		CActiveScheduler::Stop();
//	ASSERT(aEvent.iErrorCode == KErrNone);
	return 0;
	}


TVerdict TCreateSourceBuffer1TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	TBool ref;
	MDataSource* source = NULL;
	HBufC8* dataDes = NULL;


	if (iSourceType == ESourceTypeDes)
		{
		dataDes = HBufC8::NewLC(KTestDataSize);
		}

	source = CreateSourceL(iSourceType, dataDes);
	CleanupDeletePushL(source);

	CMMFBuffer* nullBuffer = source->CreateSourceBufferL(mediaId, ref);

	TInt err = source->SourceThreadLogon(*this);
	User::LeaveIfError(err);
	TBool loggedOn = ETrue;

	source->NegotiateSourceL(*iSink);

	//DevSound WP - Must Prime before calling FillBufferL
	source->SourcePrimeL();

	//DevSound WP - Audio Input effectively drives flow in v8.0
	//so just make first call to FillBufferL and then call active scheduler
	source->FillBufferL(nullBuffer, iSink, mediaId);	// first call inits DevSound
	//source->FillBufferL(nullBuffer, iSink, mediaId);	// second call starts recording

	CActiveScheduler::Start();	// wait for BufferFilledL callback

	CMMFBuffer* buffer = static_cast<CMMFDummySink*>(iSink)->MmfBuffer();

	TVerdict result = EPass;
	if ((buffer) && (static_cast<CMMFDummySink*>(iSink)->BufferFilledOk()))
		{
		if (ref != iExpectedRef)
			result = EFail;

		if (buffer->Type() != iExpectedType)
			result = EFail;

	
		//	if (buffer->Status() != EAvailable)
		//	result = EFail;




		buffer = NULL;

		//DevSound WP done this	source->SourcePrimeL();
		source->SourcePlayL();
		source->SourcePauseL();
		source->SourceStopL();
		}


	TMMFPrioritySettings prioritySettings;
	source->SetSourcePrioritySettings(prioritySettings);
	if (loggedOn)
		source->SourceThreadLogoff();

//	if (buffer != NULL) DevSound WP  it's not your buffer to delete
//		result = EFail;		// buffer not deleted

	CleanupStack::PopAndDestroy(source);
	if (dataDes)
		CleanupStack::PopAndDestroy(dataDes);

	return result;
	}

TInt TCreateSourceBuffer2TestStep::SendEventToClient(const TMMFEvent& aEvent)
	{
	// This callback won't get called for this test
	//DevSound WP - this callback can get called as an OOM condition can occur
	//on the alloc tests after the active scheduler has started
	//ASSERT(aEvent.iErrorCode == KErrNone);
	//	DevSoundWP - if it is OOM need to stop active scheduler
	if (aEvent.iErrorCode == KErrNoMemory)
		CActiveScheduler::Stop();
	return 0;
	}

TVerdict TCreateSourceBuffer2TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	TBool ref;
	MDataSource* source = NULL;
	HBufC8* dataDes = NULL;

	if (iSourceType == ESourceTypeDes)
		{
		dataDes = HBufC8::NewLC(KTestDataSize);
		}

	source = CreateSourceL(iSourceType, dataDes);
	CleanupDeletePushL(source);

	CMMFBuffer* nullBuffer = source->CreateSourceBufferL(mediaId, *iSinkBuffer, ref);

	TInt err = source->SourceThreadLogon(*this);
	User::LeaveIfError(err);
	TBool loggedOn = ETrue;

	source->NegotiateSourceL(*iSink);

	//DevSound WP - Must Prime before calling FillBufferL
	source->SourcePrimeL();

	//DevSound WP - Audio Input effectively drives flow in v8.0
	//so just make first call to FillBufferL and then call active scheduler
	source->FillBufferL(nullBuffer, iSink, mediaId);	// first call inits DevSound
	//source->FillBufferL(nullBuffer, iSink, mediaId);	// second call starts recording

	CActiveScheduler::Start();	// wait for BufferFilledL callback

	CMMFBuffer* buffer = static_cast<CMMFDummySink*>(iSink)->MmfBuffer();

	TVerdict result = EPass;
	//DevSound WP	if (buffer)
	//an OOM can occur during AS in which case buffer is not valid
	if ((buffer) && (static_cast<CMMFDummySink*>(iSink)->BufferFilledOk()))
		{
		if (ref != iExpectedRef)
			result = EFail;

		if (buffer->Type() != iExpectedType)
			result = EFail;

	
		buffer = NULL;

		source->SourcePlayL();
		source->SourcePauseL();
		source->SourceStopL();
		}

	TMMFPrioritySettings prioritySettings;
	source->SetSourcePrioritySettings(prioritySettings);
	if (loggedOn)
		source->SourceThreadLogoff();

//DevSound WP buffer not null if OOM
	//if (buffer != NULL)
	//result = EFail;		// buffer not deleted

	CleanupStack::PopAndDestroy(source);
	if (dataDes)
		CleanupStack::PopAndDestroy(dataDes);

	return result;
	}

TVerdict TSourceThreadLogonTestStep::DoTestStepL()
	{
	User::LeaveIfError(iSource->SourceThreadLogon(*iEventHandler));

	iSource->SourceThreadLogoff();

	return EPass;
	}

TVerdict TNegotiateSourceTestStep::DoTestStepL()
	{
	iSource->NegotiateSourceL(*iSink);
	return EPass;
	}

TVerdict TFillBufferTestStep::DoTestStepL()
	{
	RandomiseBuffer();

	TMediaId mediaId;
	TRAPD(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	err = CheckBuffer();
	iPosition += readLength;
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFillLastBufferTestStep::DoTestStepL()
	{
	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Create a descriptor source.
	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	MDataSource* source = MDataSource::NewSourceL(iSourceUid, iConfigDes);
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	iPosition = 0;
	TMediaId mediaId;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(source);
		User::Leave(err);
		}

	if (!sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);

		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	if (iBuffer->LastBuffer())
		{
		CleanupStack::PopAndDestroy(source);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(source);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	// Randomise the big buffer.
	iBuffer = bigBuffer;
	RandomiseBuffer();

	TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(source);

		User::Leave(err);
		}

	if (!sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);

		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	if (!iBuffer->LastBuffer())
		{
		CleanupStack::PopAndDestroy(source);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	iPosition += KTestBufferSize;
	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(source);

		CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
		return EFail;
		}

	CleanupStack::PopAndDestroy(source);

	// Test last buffer with request size.
	source = STATIC_CAST(CMMFClip*, MDataSource::NewSourceL(iSourceUid, iConfigDes));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TBool lastBuffer;
	validBuffer->SetRequestSizeL(KTestReadSize);

	iPosition = 0;
	iBuffer = validBuffer;
	do
		{
		RandomiseBuffer();
		TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
		if (iAsynchronousRead && (err == KErrNone))
			CActiveScheduler::Start();

		sinkBufferFilledOk = dummySink->BufferFilledOk();
		if ((err != KErrNone) && sinkBufferFilledOk)
			{
			CleanupStack::PopAndDestroy(source);

			CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
			return EFail;
			}

		if (err != KErrNone)
			{
			CleanupStack::PopAndDestroy(source);

			User::Leave(err);
			}

		if (!sinkBufferFilledOk)
			{
			CleanupStack::PopAndDestroy(source);

			if ((err = dummySink->Error()) != KErrNone)
				User::Leave(err);

			CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
			return EFail;
			}

		lastBuffer = (KTestDataSize-iPosition)<KTestReadSize;
		if (iBuffer->LastBuffer() != lastBuffer)
			{
			CleanupStack::PopAndDestroy(source);

			CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
			return EFail;
			}

		err = CheckBuffer();
		if (err)
			{
			CleanupStack::PopAndDestroy(source);

			CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
			return EFail;
			}

		iPosition+= validBuffer->BufferSize();
		}
	while (validBuffer->BufferSize() != 0);

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(2, bigBuffer); // validBuffer, bigBuffer
	return EPass;
	}

TVerdict TReadLastBufferTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer without request size.
	iPosition = 0;
	TRAPD(err, clip->ReadBufferL(KTestBufferSize, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() < TUint(KTestBufferSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize-10;
	TRAP(err, clip->ReadBufferL(KTestReadSize, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (!validBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}
	CleanupStack::PopAndDestroy(validBuffer);

	validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer with request size.
	validBuffer->SetRequestSizeL(KTestReadSize);

	iPosition = 0;
	TRAP(err, clip->ReadBufferL(KTestReadSize, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() < TUint(KTestReadSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize - 10;
	TRAP(err, clip->ReadBufferL(KTestReadSize, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (!iBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	CleanupStack::PopAndDestroy(validBuffer);
	return EPass;
	}

TVerdict TReadLastBuffer2TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer without request size.
	iPosition = 0;
	TRAPD(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() != TUint(KTestBufferSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize-10;
	TRAP(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (!validBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}
	CleanupStack::PopAndDestroy(validBuffer);

	validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer with request size.
	validBuffer->SetRequestSizeL(KTestReadSize);

	iPosition = 0;
	TRAP(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() != TUint(KTestReadSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize - 10;
	TRAP(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	if (!iBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	CleanupStack::PopAndDestroy(validBuffer);
	return EPass;
	}

TVerdict TReadLastBuffer3TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer without request size.
	iPosition = 0;
	clip->ReadBufferL(iBuffer, iPosition);

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() != TUint(KTestBufferSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	TInt err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize-10;
	clip->ReadBufferL(iBuffer, iPosition);

	if (!validBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}
	CleanupStack::PopAndDestroy(validBuffer);

	validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Randomise the valid buffer.
	iBuffer = validBuffer;
	RandomiseBuffer();

	// Test last buffer with request size.
	validBuffer->SetRequestSizeL(KTestReadSize);

	iPosition = 0;
	clip->ReadBufferL(iBuffer, iPosition);

	if (iBuffer->LastBuffer() || (iBuffer->BufferSize() != TUint(KTestReadSize)))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	iPosition = KTestDataSize - 10;
	clip->ReadBufferL(iBuffer, iPosition);

	if (!iBuffer->LastBuffer() || (iBuffer->BufferSize() != 10))
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	err = CheckBuffer();
	if (err)
		{
		CleanupStack::PopAndDestroy(validBuffer);
		return EFail;
		}

	CleanupStack::PopAndDestroy(validBuffer);
	return EPass;
	}


TVerdict TNewSinkTestStep::DoTestStepL()
	{
	MDataSink* sink = MDataSink::NewSinkL(iUid, *iConfigDes);
	delete sink;

	return EPass;
	}

TVerdict TSinkTypeTestStep::DoTestStepL()
	{
	TUid dataSinkType = iSink->DataSinkType();
	if (dataSinkType != iUid)
		return EFail;

	TMediaId mediaId;
	TFourCC fourCC = iSink->SinkDataTypeCode(mediaId);
	if (fourCC != iFourCC)
		return EFail;

	fourCC = 0x4E554C4C; // 'LLUN'
	TInt err = iSink->SetSinkDataTypeCode(fourCC, mediaId);
	if (iCanSetSinkDataType)
		{
		if (err != KErrNone)
			User::Leave(err);

		if (iSink->SinkDataTypeCode(mediaId) != fourCC)
			return EFail;

		User::LeaveIfError(iSink->SetSinkDataTypeCode(iFourCC, mediaId));
		}
	else
		{
		if (err != KErrNotSupported)
			{
			if (err == KErrNone)
				return EFail;

			User::Leave(err);
			}

		if (iSink->SinkDataTypeCode(mediaId) != iFourCC)
			return EFail;
		}

	return EPass;
	}

TVerdict TCanCreateSinkBufferTestStep::DoTestStepL()
	{
	return (iSink->CanCreateSinkBuffer() == iCanCreateSinkBuffer) ? EPass : EFail;
	}

TVerdict TCreateSinkBufferTestStep0::DoTestStepL()
	{
	TVerdict result = EPass;
	TMediaId mediaId;
	TBool ref;

	// Create sink buffer
	CMMFBuffer* nullBuffer = iSink->CreateSinkBufferL(mediaId, ref);
	User::LeaveIfError( iSink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	TBool loggedOn = ETrue;

	iSink->EmptyBufferL(nullBuffer, iSource, mediaId);

	CMMFBuffer* buffer = static_cast<CMMFDummySource*>(iSource)->MmfBuffer();

	if (ref != iExpectedRef)
		result = EFail;

	if (!buffer)
		{
		RDebug::Print(_L("*** TCreateSinkBufferTestStep:  iSink->CreateSinkBufferL returned NULL"));
		result = EFail;
		}

	if (result != EFail)
		{
		if (buffer->Type() != iExpectedType)
			result = EFail;

		if (buffer->Status() != EAvailable)
			result = EFail;

		if (buffer->Type() == KUidMmfDescriptorBuffer)
			{
			CMMFDataBuffer* dataBuffer = STATIC_CAST(CMMFDataBuffer*, buffer);
			TDes8& data = dataBuffer->Data();

			if (data.MaxLength() != iExpectedMaxLength)
				result = EFail;
			}

		buffer = NULL;
		}

	if (loggedOn)
		iSink->SinkThreadLogoff();

	if (buffer != NULL)
		result = EFail;		// buffer not deleted

	return result;
	}


TVerdict TCreateSinkBufferTestStep::DoTestPostambleL(TBool /*aCheck*/)
	{
	REComSession::FinalClose();
	return EPass;
	}


TVerdict TCreateSinkBufferTestStep1::DoTestStepL()
	{
	TVerdict result = EPass;
	TMediaId mediaId;
	TBool ref;

	TBuf8<1> dummyConfigDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, dummyConfigDes);
	CleanupDeletePushL(sink);

	//DevSound WP can't call prime before login	sink->SinkPrimeL();

	// Create sink buffer
	CMMFBuffer* nullBuffer = sink->CreateSinkBufferL(mediaId, ref);
	User::LeaveIfError( sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	TCleanupItem threadLogOff(DoDataSinkThreadLogoff, sink);
	CleanupStack::PushL(threadLogOff);

	//DevSound WP	sink->SinkPrimeL(); this should go after negotiate
	TCleanupItem sinkStop(DoDataSinkStop, sink);
	CleanupStack::PushL(sinkStop);
	if (!iSource)
		User::Leave(KErrNoMemory);
	if (iFormat)
		sink->NegotiateL(*iFormat);
	else
		sink->NegotiateL(*iSource);
	sink->SinkPrimeL();//DevSound WP moved from above
	sink->SinkPlayL();
	sink->EmptyBufferL(nullBuffer, iSource, mediaId);
	CActiveScheduler::Start();

	CMMFBuffer* buffer = static_cast<CMMFDummySource*>(iSource)->MmfBuffer();

	if (ref != iExpectedRef)
		result = EFail;

	if (!buffer)
		{
		RDebug::Print(_L("*** TCreateSinkBufferTestStep:  sink->CreateSinkBufferL returned NULL"));
		result = EFail;
		}

	if (result != EFail)
		{
		if (buffer->Type() != iExpectedType)
			result = EFail;

		//	DevSound WP buffers are owned and controller by
		//	HwPlugins test should not make assumptions
		//  if (buffer->Status() != EAvailable)
		//	result = EFail;

		if (buffer->Type() == KUidMmfDescriptorBuffer)
			{
			/*
			DevSound WP buffers are owned and controller by
			HwPlugins test should not make assumptions
			CMMFDataBuffer* dataBuffer = STATIC_CAST(CMMFDataBuffer*, buffer);
			TDes8& data = dataBuffer->Data();

			if (data.MaxLength() != iExpectedMaxLength)
				result = EFail;
			*/
			}

		buffer = NULL;
		}

	// Re-test after logging on. (iNeedsSWConversion = ETrue)
	User::LeaveIfError(sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))));

	CleanupStack::PopAndDestroy(3, sink);	// sinkStop, threadLogOff, sink

	//	if (buffer != NULL) DevSound WP might not be null if was never created
	//		result = EFail; also not test apps buffer to delete

	return result;
	}

TVerdict TCreateSinkBufferTestStep2::DoTestStepL()
	{
	TVerdict result = EPass;
	TMediaId mediaId;
	TBool ref;

	TBuf8<1> dummyConfigDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, dummyConfigDes);
	CleanupDeletePushL(sink);
	sink->SinkPrimeL();

	// Create sink buffer
	CMMFBuffer* nullBuffer = sink->CreateSinkBufferL(mediaId, ref);
	User::LeaveIfError( sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	TCleanupItem threadLogOff(DoDataSinkThreadLogoff, sink);
	CleanupStack::PushL(threadLogOff);

	sink->SinkPrimeL();
	TCleanupItem sinkStop(DoDataSinkStop, sink);
	CleanupStack::PushL(sinkStop);
	if (!iSource)
		User::Leave(KErrArgument);
	if (iFormat)
		sink->NegotiateL(*iFormat);
	else
		sink->NegotiateL(*iSource);
	sink->SinkPlayL();
	sink->EmptyBufferL(nullBuffer, iSource, mediaId);
	CActiveScheduler::Start();

	CMMFBuffer* buffer = static_cast<CMMFDummySource*>(iSource)->MmfBuffer();

	if (ref != iExpectedRef)
		result = EFail;

	if (!buffer)
		{
		RDebug::Print(_L("*** TCreateSinkBufferTestStep:  sink->CreateSinkBufferL returned NULL"));
		result = EFail;
		}

	if (result != EFail)
		{
		if (buffer->Type() != iExpectedType)
			result = EFail;

		if (buffer->Status() != EAvailable)
			result = EFail;

		if (buffer->Type() == KUidMmfDescriptorBuffer)
			{
			CMMFDataBuffer* dataBuffer = STATIC_CAST(CMMFDataBuffer*, buffer);
			TDes8& data = dataBuffer->Data();

			if (data.MaxLength() != iExpectedMaxLength)
				result = EFail;
			}

		buffer = NULL;
		}

	// Test the rest...
	TFourCC fourCC = KMMFFourCCCodePCM16;
	TFourCC emptyFourCC;

	TBufC8<5> fourCCString(KEmptyFourCCCode);
	TPtr8 fourCCPtr = fourCCString.Des();
	TPtr8 fourCCPtr1(&fourCCPtr[0], 4);
	emptyFourCC.FourCC(&fourCCPtr1);
	TInt err = sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource)));
	if (err != KErrNone)
		{
	  //RDebug::Print(_L("Audio Output: Expecting error %d, got %d"), KErrNone, err);(Commented under DEF105143)
		result = EFail;
		}

	TRAP(err, sink->NegotiateL(*iSource));
	if (err != KErrNotSupported)
		{
	//	RDebug::Print(_L("Audio Output: Expecting error %d, got %d"), KErrNotSupported, err);(Commented under DEF105143)
		result = EFail;
		}

	if ((sink->SinkDataTypeCode(mediaId) != fourCC) && (sink->SinkDataTypeCode(mediaId) != emptyFourCC))
		{
		RDebug::Print(_L("Audio Output: NegotiateL sink code does not match\n"));
		result = EFail;
		}

	sink->SinkPrimeL();
	TRAP(err, sink->SinkPlayL());
	if (err != KErrNotSupported)
		{
	//	RDebug::Print(_L("Audio Output: Expecting error %d, got %d"), KErrNotSupported, err);(Commented under DEF105143)
		result = EFail;
		}
	sink->SinkPauseL();
	sink->SinkStopL();

	TMMFPrioritySettings prioritySettings;
	sink->SetSinkPrioritySettings(prioritySettings);
	sink->SinkThreadLogoff();

	CleanupStack::PopAndDestroy(3, sink);	// sinkStop, threadLogOff, sink

	if (buffer != NULL)
		result = EFail;

	return result;
	}

TVerdict TCreateSinkBufferTestStep3::DoTestStepL()
	{
	TVerdict result = EPass;
	TMediaId mediaId;
	TBool ref;
	// DevSoundWP - The source file is pcmu8 not pcm16
	// on 7.0s audio output this didn't matter as it could only
	// accept pcm16 - 8.0 can accept pcmu8 as well
	// TFourCC fourCC = KMMFFourCCCodePCM16;
	TFourCC fourCC = KMMFFourCCCodePCMU8;
	TFourCC emptyFourCC;
	TMMFPrioritySettings prioritySettings;

	TBuf8<1> dummyConfigDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, dummyConfigDes);
	CleanupDeletePushL(sink);

	//DevSound WP - need to rest this as buffer may not be emptied ok if OOM in AS occurs
	static_cast<CMMFDummySource*>(iSource)->ResetBufferEmptiedOk();

	//DevSound WP must call logon before prime
	User::LeaveIfError( sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	sink->SinkPrimeL();

	// Create sink buffer
	CMMFBuffer* nullBuffer = sink->CreateSinkBufferL(mediaId, ref);
	//DevSound WP must call logon before prime
//	User::LeaveIfError( sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	TCleanupItem threadLogOff(DoDataSinkThreadLogoff, sink);
	CleanupStack::PushL(threadLogOff);

	sink->SinkPrimeL();
	TCleanupItem sinkStop(DoDataSinkStop, sink);
	CleanupStack::PushL(sinkStop);
	if (!iSource)
		User::Leave(KErrNoMemory);
	if (iFormat)
		sink->NegotiateL(*iFormat);
	else
		sink->NegotiateL(*iSource);
	sink->SinkPlayL();
	sink->EmptyBufferL(nullBuffer, iSource, mediaId);
	CActiveScheduler::Start();

	CMMFBuffer* buffer = static_cast<CMMFDummySource*>(iSource)->MmfBuffer();

	if ((buffer) && (static_cast<CMMFDummySource*>(iSource)->BufferEmptiedOk()))
		{//DevSound WP buffer not emptied ok if OOM in AS

		if (ref != iExpectedRef)
			result = EFail;

		if (!buffer)
			{
			RDebug::Print(_L("*** TCreateSinkBufferTestStep:  sink->CreateSinkBufferL returned NULL"));
			result = EFail;
			}

		if (result != EFail)
			{
			if (buffer->Type() != iExpectedType)
				result = EFail;

			if (buffer->Status() != EAvailable)
				result = EFail;
	
			buffer = NULL;
			}

		// Test the rest...
		if ((sink->SinkDataTypeCode(mediaId) != fourCC) && (sink->SinkDataTypeCode(mediaId) != emptyFourCC))
			{
			RDebug::Print(_L("Audio Output: NegotiateL sink code does not match\n"));
			result = EFail;
			}

		User::LeaveIfError(sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))));
		sink->SinkPrimeL();
		sink->SinkPlayL();
		sink->SinkPauseL();
		sink->SinkStopL();

		sink->SetSinkPrioritySettings(prioritySettings);
		}

	CleanupStack::PopAndDestroy(3, sink);	// sinkStop, threadLogOff, sink

//	if (buffer != NULL) DevSound WP  OOM in AS may not be null
//		result = EFail;

	return result;
	}


TVerdict TCreateSinkBufferTestStep4::DoTestStepL()
	{
	TVerdict result = EPass;
	TMediaId mediaId;
	TBool ref;

	// DevSoundWP - The source file is pcmu8 not pcm16
	// however the sample rate is non standard and so
	// the conversion is done in audio output which means the
	// datatype is still pcm16
	TFourCC fourCC = KMMFFourCCCodePCM16;
	//TFourCC fourCC = KMMFFourCCCodePCMU8;

	TFourCC emptyFourCC;
	TMMFPrioritySettings prioritySettings;

	TBuf8<1> dummyConfigDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, dummyConfigDes);
	CleanupDeletePushL(sink);
	//	DevSoundWP Prime should be after logon sink->SinkPrimeL();

	// Create sink buffer
	CMMFBuffer* nullBuffer = sink->CreateSinkBufferL(mediaId, ref);

	//DevSound WP needs resetting as buffer is not emptied ok during OOM in AS
	static_cast<CMMFDummySource*>(iSource)->ResetBufferEmptiedOk();

	User::LeaveIfError( sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))) );
	TCleanupItem threadLogOff(DoDataSinkThreadLogoff, sink);
	CleanupStack::PushL(threadLogOff);

	sink->SinkPrimeL();
	TCleanupItem sinkStop(DoDataSinkStop, sink);
	CleanupStack::PushL(sinkStop);
	if (!iSource)
		User::Leave(KErrNoMemory);
	if (iFormat)
		sink->NegotiateL(*iFormat);
	else
		sink->NegotiateL(*iSource);
	sink->SinkPlayL();
	sink->EmptyBufferL(nullBuffer, iSource, mediaId);
	CActiveScheduler::Start();

	CMMFBuffer* buffer = static_cast<CMMFDummySource*>(iSource)->MmfBuffer();

	if ((buffer) && (static_cast<CMMFDummySource*>(iSource)->BufferEmptiedOk()))
		{//DevSound WP buffer not emptied ok if OOM in AS

		if (ref != iExpectedRef)
			result = EFail;

		if (!buffer)
			{
			RDebug::Print(_L("*** TCreateSinkBufferTestStep:  sink->CreateSinkBufferL returned NULL"));
			result = EFail;
			}

		if (result != EFail)
			{
			if (buffer->Type() != iExpectedType)
				result = EFail;

			if (buffer->Status() != EAvailable)
				result = EFail;

			/*
		DevSound WP buffers are owned and controller by
		HwPlugins test should not make assumptions
#ifdef __USE_MMF_TRANSFERBUFFERS__
		if (buffer->Type() == KUidMmfTransferBuffer)
#else
		if (buffer->Type() == KUidMmfDescriptorBuffer)
#endif
			{
			CMMFDataBuffer* dataBuffer = STATIC_CAST(CMMFDataBuffer*, buffer);
			TDes8& data = dataBuffer->Data();

			if (data.MaxLength() != iExpectedMaxLength)
				result = EFail;
			}
		else
			result = EFail;
		*/


			buffer = NULL;
			}

		// Test the rest...
		if ((sink->SinkDataTypeCode(mediaId) != fourCC) && (sink->SinkDataTypeCode(mediaId) != emptyFourCC))
			{
			RDebug::Print(_L("Audio Output: NegotiateL sink code does not match\n"));
			result = EFail;
			}

		User::LeaveIfError(sink->SinkThreadLogon(*(static_cast<CMMFDummySource*>(iSource))));
		sink->SinkPrimeL();
		sink->SinkPlayL();
		sink->SinkPauseL();
		sink->SinkStopL();

		sink->SetSinkPrioritySettings(prioritySettings);

		}

	CleanupStack::PopAndDestroy(3, sink);	// sinkStop, threadLogOff, sink

//	if (buffer != NULL) DevSound WP if OOM in AS may not be null
//		result = EFail;

	return result;
	}


TVerdict TSinkThreadLogonTestStep::DoTestStepL()
	{
	User::LeaveIfError(iSink->SinkThreadLogon(*iEventHandler));

	iSink->SinkThreadLogoff();

	return EPass;
	}

TVerdict TNegotiateTestStep::DoTestStepL()
	{
	iSink->NegotiateL(*iSource);
	return EPass;
	}

TVerdict TEmptyBufferTestStep::DoTestStepL()
	{
	RandomiseBuffer();

	TMediaId mediaId;
	TRAPD(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		{
		TInt destinationLength = iDestinationDes.MaxLength();
		if (Mem::Compare(iDestinationDes.Ptr(), destinationLength, iCopyOfDestinationDes.Ptr(), destinationLength))
			return EFail;

		User::Leave(err);
		}

	if (!sourceBufferEmptiedOk)
		return EFail;

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = buffer->BufferSize();
	if (writeLength != iWriteLength)
		return EFail;

	if (iCopyOfDestinationDes.Length()<(iPosition+iWriteLength))
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
	iPosition += writeLength;
	return EPass;
	}


// TClipBytesFreeTestStep
// Test that the number of bytes free is what we expect.
// Zero bytes free is also an acceptable result, when file failures are expected.
TVerdict TClipBytesFreeTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);
	TInt64 bytesFree = clip->BytesFree();

	// tolerance incase another process accesses file system during test
	TInt64 tolerance = 8192;		// 2 blocks of 4096 bytes

	// FileServer file failure simulation is currently only supported on WINS
	// so ignore the iFileFailureExpected on target builds

	TVerdict result = EFail;
	if ((bytesFree == iExpectedBytesFree) || (Abs(bytesFree - iExpectedBytesFree) <= tolerance))
		{
		result = EPass;
		}
	else if (iFileFailureExpected && (bytesFree == 0))
		{
		result = EPass;
		}

	return result;
	}

// TClipSizeTestStep
// Test that the size of the clip is what we expect.
// Zero bytes free is also an acceptable result, when file failures are expected.
TVerdict TClipSizeTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);
	TInt size = clip->Size();

	return (size == iExpectedSize)  || ((size == 0) && iFileFailureExpected) ? EPass : EFail;
	}

// TClipDeleteTestStep
// Attempt to delete the clip.
// The test leaves if there is an error.
TVerdict TClipDeleteTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);
	User::LeaveIfError(clip->Delete());

	return EPass;
	}

// TClipSetSizeTestStep
// Attempt to set the size of the clip
// Test that the results are correct, in the case when setting the size is supported.
// In the case when it is not supported check that the size does not change.
TVerdict TClipSetSizeTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	TInt size;
	TInt oldSize = clip->Size();
	if (oldSize <= 0)
		{
		if (iFileFailureExpected && (oldSize == 0))
			User::Leave(KErrFileFail);

		return EFail;
		}

	TInt newSize = oldSize+1;
	TInt err = clip->SetSize(newSize);
	if (iCanSetSize)
		{
		if (err != KErrNone)
			User::Leave(err);

		if ((size = clip->Size()) != newSize)
			{
			if (iFileFailureExpected && (size == 0))
				User::Leave(KErrFileFail);

			return EFail;
			}
		}
	else
		{
		if (err != KErrNotSupported)
			{
			if (err == KErrNone)
				return EFail;

			User::Leave(err);
			}

		if ((size = clip->Size()) != oldSize)
			{
			if (iFileFailureExpected && (size == 0))
				User::Leave(KErrFileFail);

			return EFail;
			}
		}

	return EPass;
	}


TVerdict TClipReadBufferTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RandomiseBuffer();
	TRAPD(err, clip->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength < iReadLength) // Reading more data than we asked for is ok.
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TClipReadBuffer2TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RandomiseBuffer();
	TRAPD(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TClipReadBuffer3TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RandomiseBuffer();
	clip->ReadBufferL(iBuffer, iPosition);

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	TInt err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TClipReadBuffer4TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RandomiseBuffer();
	TRAPD(err, clip->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		return EFail;

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iExpectedReadLength)
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}


TVerdict TDescriptorClipWriteBufferTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iWriteLength, iBuffer, iPosition, iSource));

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		return EFail;

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);

	TInt position = iPosition;
	if (iCopyOfDestinationDes.Length()<iPosition)
		position = iCopyOfDestinationDes.Length();

	if (iCopyOfDestinationDes.Length()<(position+iWriteLength))
		iCopyOfDestinationDes.SetLength(position+iWriteLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(position, iWriteLength, bufferData);
	return EPass;
	}

TVerdict TDescriptorClipWriteBuffer2TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iBuffer, iPosition, iSource));

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		return EFail;

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = buffer->BufferSize();
	if (writeLength != iWriteLength)
		return EFail;

	TInt position = iPosition;
	if (iCopyOfDestinationDes.Length()<iPosition)
		position = iCopyOfDestinationDes.Length();

	if (iCopyOfDestinationDes.Length()<(position+iWriteLength))
		iCopyOfDestinationDes.SetLength(position+iWriteLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(position, iWriteLength, bufferData);
	return EPass;
	}

TVerdict TDescriptorClipWriteBuffer3TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	clip->WriteBufferL(iBuffer, iPosition);

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = buffer->BufferSize();
	if (writeLength != iWriteLength)
		return EFail;

	TInt position = iPosition;
	if (iCopyOfDestinationDes.Length()<iPosition)
		position = iCopyOfDestinationDes.Length();

	if (iCopyOfDestinationDes.Length()<(position+iWriteLength))
		iCopyOfDestinationDes.SetLength(position+iWriteLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(position, iWriteLength, bufferData);
	return EPass;
	}

TVerdict TNewFileSourceTestStep::DoTestStepL()
	{
	CMMFFile* source = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL(iUid, *iConfigDes));
	CleanupStack::PushL(source);
	source->SourcePrimeL();

	CleanupStack::PopAndDestroy(source);
	return EPass;
	}

TInt TFileSourceThreadLogonThread(TAny* initPtr)
	{
	TThreadData &threadData = *REINTERPRET_CAST(TThreadData*, initPtr);
	CMMFFile* source = threadData.iFilePtr;
	MAsyncEventHandler* eventHandler = threadData.iEventHandler;

	threadData.iErr = KErrNone;

	threadData.iErr = source->SourceThreadLogon(*eventHandler);
	if (threadData.iErr == KErrNone)
		{
		threadData.iFileSize = source->Size();
		source->SourceThreadLogoff();
		}


	return 0;
	}

TVerdict TFileSourceThreadLogonTestStep::DoTestStepL()
	{

	TThreadData threadData; // Is this going to work on target? (or do we need to package the data and use RThread::ReadL/WriteL?)
	threadData.iFilePtr = STATIC_CAST(CMMFFile*, iSource);
	threadData.iEventHandler = iEventHandler;
	
	TRequestStatus logonStatus;
	
	RThread sourceThread;
	User::LeaveIfError(sourceThread.Create(_L("TFileSourceThreadLogonThread"), TFileSourceThreadLogonThread, KDefaultStackSize, NULL, &threadData));
	CleanupClosePushL(sourceThread);

	sourceThread.Logon(logonStatus);
	if(logonStatus != KRequestPending)
		{
		User::Leave(logonStatus.Int());
		}
	sourceThread.Resume();
	
	User::WaitForRequest(logonStatus);
	if(logonStatus != KErrNone)
		{
		User::Leave(logonStatus.Int());
		}
	
	if (threadData.iErr != KErrNone)
		User::Leave(threadData.iErr);

	if ((threadData.iFileSize != iFileSize))
		{
		if (!iFileFailureExpected)
			return EFail;

		if (threadData.iFileSize != 0)
			return EFail;
		}

	CleanupStack::PopAndDestroy(1); // sourceThread
	return EPass;
	}

TVerdict TFileFillBufferTestStep::DoTestStepL()
	{
	RandomiseBuffer();

	TMediaId mediaId;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	MDataSource* source = CreateFileSourceL(iSourceType, fs, iFileName);
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs); // source, fs

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileFillBuffer2TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	MDataSource* source = CreateFileSourceL(iSourceType, fs, iFileName);
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TInt readLength;
	TVerdict result = EPass;
	iPosition = 0;
	do
		{
		RandomiseBuffer();

		TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
		if (iAsynchronousRead && (err == KErrNone))
			CActiveScheduler::Start();

		TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
		if ((err != KErrNone) && sinkBufferFilledOk)
			{
			result = EFail;
			break;
			}

		if (err != KErrNone)
			User::Leave(err);

		if (!sinkBufferFilledOk)
			{
			if ((err = dummySink->Error()) != KErrNone)
				User::Leave(err);

			result = EFail;
			break;
			}

		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		readLength = buffer->BufferSize();

		err = CheckBuffer();
		iPosition += readLength;

		if (err != 0)
			{
			result = EFail;
			break;
			}
		}
	while (readLength != 0);

	CleanupStack::PopAndDestroy(2, &fs); // source, fs
	return result;
	}

TVerdict TFileFillBuffer3TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	MDataSource* source = CreateFileSourceL(iSourceType, fs, iFileName);
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;
	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
		if (err != KErrNone)
			break;
		}

	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(2, &fs);

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += readLength;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	return EPass;
	}

TVerdict TFileFillBuffer4TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
		if (err != KErrNone)
			break;
		}

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += readLength;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	iSource->FillBufferL(bufferArray[completionCount], iSink, mediaId);
	return EPass;
	}

TVerdict TFileReadBufferTestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);

	CMMFDummySink* sink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*sink));
	source->SourcePrimeL();

	RandomiseBuffer();
	TRAPD(err, source->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs);

	TBool sinkBufferFilledOk = sink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = sink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength < iReadLength) // Reading more data than we asked for is ok.
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileReadBuffer2TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);

	CMMFDummySink* sink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*sink));
	source->SourcePrimeL();

	RandomiseBuffer();
	TRAPD(err, source->ReadBufferL(iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs); // source, fs

	TBool sinkBufferFilledOk = sink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = sink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileReadBuffer3TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);
	source->SourcePrimeL();

	RandomiseBuffer();
	source->ReadBufferL(iBuffer, iPosition);
	CleanupStack::PopAndDestroy(2, &fs); //source, fs

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iReadLength)
		return EFail;

	TInt err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileReadBuffer4TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	RandomiseBuffer();
	TRAPD(err, source->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));
	if (iAsynchronousRead && (err == KErrNone))
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs); //source, fs

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt readLength = buffer->BufferSize();
	if (readLength != iExpectedReadLength)
		return EFail;

	err = CheckBuffer();
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileReadBuffer5TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;
	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, source->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));
		if (err != KErrNone)
			break;

		iPosition += 2*KTestBufferSize;
		}

	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(2, &fs); // source, fs
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	iPosition = 0;

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += 2*KTestBufferSize;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	return EPass;
	}

TVerdict TFileReadBuffer6TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, fs, iFileName));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;
	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, source->ReadBufferL(iBuffer, iPosition, iSink));
		if (err != KErrNone)
			break;

		iPosition += 2*KTestBufferSize;
		}

	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(2, &fs); // source, fs
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	iPosition = 0;

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += 2*KTestBufferSize;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	return EPass;
	}

TVerdict TFileReadBuffer7TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, clip->ReadBufferL(iReadLength, iBuffer, iPosition, iSink));
		if (err != KErrNone)
			break;

		iPosition += 2*KTestBufferSize;
		}

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	iPosition = 0;

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += 2*KTestBufferSize;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	clip->ReadBufferL(iReadLength, bufferArray[completionCount], iPosition, iSink);
	return EPass;
	}

TVerdict TFileReadBuffer8TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSource);

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, clip->ReadBufferL(iBuffer, iPosition, iSink));
		if (err != KErrNone)
			break;

		iPosition += 2*KTestBufferSize;
		}

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if (!requestCount && sinkBufferFilledOk)
		return EFail;

	if (!sinkBufferFilledOk && requestCount)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	iPosition = 0;

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt readLength = buffer->BufferSize();
		if (readLength != iReadLength)
			return EFail;

		err = CheckBuffer();
		iPosition += 2*KTestBufferSize;
		if (err != 0)
			return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	clip->ReadBufferL(bufferArray[completionCount], iPosition, iSink);
	return EPass;
	}


TVerdict TNewFileSinkTestStep::DoTestStepL()
	{
	CMMFFile* sink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(iUid, *iConfigDes));
	CleanupStack::PushL(sink);
	sink->SinkPrimeL();

	CleanupStack::PopAndDestroy(sink);
	return EPass;
	}

TVerdict TFileSinkSetSizeTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	TInt size;
	TInt oldSize = clip->Size();

	TInt newSize = oldSize+1;
	TInt err = clip->SetSize(newSize);
	if (iCanSetSize)
		{
		if (err != KErrNone)
			User::Leave(err);

		if ((size = clip->Size()) != newSize)
			{
			if (iFileFailureExpected && (size == 0))
				User::Leave(KErrFileFail);

			return EFail;
			}
		}
	else
		{
		if (err != KErrNotSupported)
			{
			if (err == KErrNone)
				return EFail;

			User::Leave(err);
			}

		if ((size = clip->Size()) != oldSize)
			return EFail;
		}
	return EPass;
	}

TInt TFileSinkThreadLogonThread(TAny* initPtr)
	{
	TThreadData &threadData = *REINTERPRET_CAST(TThreadData*, initPtr);
	CMMFFile* sink = threadData.iFilePtr;
	MAsyncEventHandler* eventHandler = threadData.iEventHandler;

	threadData.iErr = KErrNone;

	threadData.iErr = sink->SinkThreadLogon(*eventHandler);
	if (threadData.iErr == KErrNone)
		{
		threadData.iFileSize = sink->Size();
		sink->SinkThreadLogoff();
		}


	return 0;
	}

TVerdict TFileSinkThreadLogonTestStep::DoTestStepL()
	{
	TBool failure = EFalse;


	TThreadData threadData; // Is this going to work on target? (or do we need to package the data and use RThread::ReadL/WriteL?)
	threadData.iFilePtr = STATIC_CAST(CMMFFile*, iSink);
	threadData.iEventHandler = iEventHandler;

	TRequestStatus logonStatus;
	
	RThread sinkThread;
	User::LeaveIfError(sinkThread.Create(_L("TFileSinkThreadLogonThread"), TFileSinkThreadLogonThread, KDefaultStackSize, NULL, &threadData));
	CleanupClosePushL(sinkThread);

	sinkThread.Logon(logonStatus);
	if(logonStatus != KRequestPending)
		{
		User::Leave(logonStatus.Int());
		}
	sinkThread.Resume();
	
	User::WaitForRequest(logonStatus);
	if(logonStatus != KErrNone)
		{
		User::Leave(logonStatus.Int());
		}

	if (threadData.iErr != KErrNone)
		User::Leave(threadData.iErr);

	if ((threadData.iFileSize != iFileSize))
		{
		if (!iFileFailureExpected)
			{
			failure = ETrue;
			}

		if (threadData.iFileSize != 0)
			{
			failure = ETrue;
			}
		}

	CleanupStack::PopAndDestroy(1); // sinkThread
	return (failure ? EFail : EPass);
	}

TVerdict TFileEmptyBufferTestStep::DoTestStepL()
	{
	RandomiseBuffer();

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(iSink->SinkThreadLogon(*dummySource));
	iSink->SinkPrimeL();

	TMediaId mediaId;
	TRAPD(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));
	if (iAsynchronousWrite && (err == KErrNone))
		{
		CActiveScheduler::Start();
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	User::LeaveIfError(err);
	if (!sourceBufferEmptiedOk)
		{
		User::LeaveIfError(dummySource->Error());
		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = buffer->BufferSize();
	if (writeLength != iWriteLength)
		return EFail;

	if (iCopyOfDestinationDes.Length()<(iPosition+iWriteLength))
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
	return EPass;
	}

TVerdict TWriteBufferTestStep::DoTestPostambleL(TBool aCheck)
	{
	if (iSink != NULL)
		iSink->SinkStopL();

	if (aCheck)
		{
		TInt err = CheckDestinationL();
		return (err == 0) ? EPass : EFail;
		}

	return EPass;
	}

TVerdict TFileEmptyBuffer2TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	MDataSink* sink = CreateFileSinkL(iSinkType, iFsSession, iDestinationFileName);
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();

	RandomiseBuffer();
	TRAPD(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
	if (iAsynchronousWrite && (err == KErrNone))
		{
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(sink);

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		{
		return EFail;
		}

	User::LeaveIfError(err);
	if (!sourceBufferEmptiedOk)
		{
		User::LeaveIfError(dummySource->Error());
		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = buffer->BufferSize();
	if (writeLength != iWriteLength)
		{
		return EFail;
		}

	if (iCopyOfDestinationDes.Length()<(iPosition+iWriteLength))
		{
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);
		}

	TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
	iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
	return EPass;
	}

void TFileEmptyBuffer3TestStep::DoTestPreambleL()
	{
	CMMFClip* sink = STATIC_CAST(CMMFClip*, CreateFileSinkL(iSinkType, iFsSession, iDestinationFileName ));
	CleanupDeletePushL(sink);

	TInt err = sink->Delete();
	if ((err != KErrNone) && (err != KErrNotFound))
		{
		User::Leave(err);
		}

	iCopyOfDestinationDes.SetLength(0);
	CleanupStack::PopAndDestroy(sink);
	}

TVerdict TFileEmptyBuffer3TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	CMMFClip* sink = STATIC_CAST(CMMFClip*, CreateFileSinkL(iSinkType, iFsSession, iDestinationFileName));
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();

	TInt totalWriteLength = 0;
	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt maxWriteLength = iCopyOfDestinationDes.MaxLength();
	TBool failure = EFalse;
	
	do
		{
		RandomiseBuffer();

		buffer->Data().SetLength(Min(iWriteLength, maxWriteLength - totalWriteLength));
		TRAPD(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
		if (iAsynchronousWrite && (err == KErrNone))
			{
			CActiveScheduler::Start();
			}

		TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
		if ((err != KErrNone) && sourceBufferEmptiedOk)
			{
			failure = ETrue;
			break;
			}

		User::LeaveIfError(err);

		if (!sourceBufferEmptiedOk)
			{
			User::LeaveIfError(dummySource->Error());
			failure = ETrue;
			break;
			}

		totalWriteLength += buffer->BufferSize();
		iCopyOfDestinationDes.Append(buffer->Data());
		}
	while (totalWriteLength < maxWriteLength);

	CleanupStack::PopAndDestroy(sink);
	return (failure ? EFail : EPass);
	}

TVerdict TFileEmptyBuffer3TestStep::DoTestPostambleL(TBool aCheck)
	{
	// Close ECom
	TVerdict verdict = TFileTestStep::DoTestPostambleL(aCheck);
	if (verdict != EPass)
		{
		return verdict;
		}

	// Reset the buffer.
	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	buffer->Data().SetLength(iWriteLength);

	if (aCheck)
		{
		TInt err = CheckDestinationL();
		return (err == 0) ? EPass : EFail;
		}

	return EPass;
	}

void TFileEmptyBuffer4TestStep::DoTestPreambleL()
	{
	TWriteBufferToFileTestStep::DoTestPreambleL();
	}

TVerdict TFileEmptyBuffer4TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	CMMFClip* sink = STATIC_CAST(CMMFClip*, CreateFileSinkL(iSinkType, iFsSession, iDestinationFileName));
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = Min(iMaxRequestCount, bufferArray.Count());
	TInt err = KErrNone;
	iPosition = 0;

	// For each buffer in the array try to empty it. Stop at the first failure.
	// The test step should not Leave if there is an error as this will cause
	// the CheckDestinationL function to fail... all successful empties must
	// be completed.
	TInt requestCount;
	for (requestCount = 0; requestCount < maxRequestCount; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
		if (err != KErrNone)
			{
			break;
			}
		}

	if (requestCount)
		{
		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(sink);

	if (dummySource->ErrorCount())
		{
		User::LeaveIfError(dummySource->Error());
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		{
		return EFail;
		}

	TInt completionCount;
	for (completionCount = 0; completionCount < requestCount; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			{
			return EFail;
			}

		if ((iPosition + iWriteLength) > iCopyOfDestinationDes.Length())
			iCopyOfDestinationDes.SetLength(iPosition + iWriteLength);

		TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		iPosition += iWriteLength;
		}

	return EPass;
	}

TVerdict TFileEmptyBuffer4TestStep::DoTestPostambleL(TBool aCheck)
	{
	// Close ECom
	TVerdict verdict = TFileTestStep::DoTestPostambleL(aCheck);
	if (verdict != EPass)
		{
		return verdict;
		}

	if (aCheck)
		{
		TInt err = CheckDestinationL();
		return (err == 0) ? EPass : EFail;
		}

	return EPass;
	}

TInt TFileEmptyBuffer5TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead | EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(destinationFileSize);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining two writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return err;
	}

TVerdict TFileEmptyBuffer6TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAP(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));
		if (err != KErrNone)
			break;
		}

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		return EFail;

	if (!sourceBufferEmptiedOk && requestCount)
		{
		requestCount-= dummySource->ErrorCount();
		err = dummySource->Error();

		if (err == KErrNone)
			return EFail;
		}

	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];

		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
			iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

		TPtrC8 bufferData(buffer->Data().Ptr(), iWriteLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		iPosition += iWriteLength;
		}

	if (err != KErrNone)
		User::Leave(err);

	iSink->EmptyBufferL(bufferArray[0], iSource, mediaId);
	return EPass;
	}

TInt TFileEmptyBuffer6TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead | EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		if (destinationFileSize != (length+iWriteLength))
			{
			if (destinationFileSize != (length+2*iWriteLength))
				{
				if (destinationFileSize != (length+3*iWriteLength))
					{
					CleanupStack::PopAndDestroy(&destinationFile);
					return -1;
					}
				}
			}
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(destinationFileSize);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), destinationFileSize, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining three writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		if ((iPosition+iWriteLength)<=iCopyOfDestinationDes.Length())
			return EFail;
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);
		length = iPosition+iWriteLength;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), destinationFileSize, iCopyOfDestinationDes.Ptr(), length);
		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			if ((iPosition+iWriteLength)<=iCopyOfDestinationDes.Length())
				return EFail;
			iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);
			length = iPosition+iWriteLength;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), destinationFileSize, iCopyOfDestinationDes.Ptr(), length);
			if (err != 0)
				{
				iPosition += iWriteLength;
				if ((iPosition+iWriteLength)<=iCopyOfDestinationDes.Length())
					return EFail;
				iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);
				length = iPosition+iWriteLength;

				buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[0]);
				bufferData.Set(buffer->Data().Ptr(), iWriteLength);

				iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
				err = Mem::Compare(destinationDes.Ptr(), destinationFileSize, iCopyOfDestinationDes.Ptr(), length);
				}
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return err;
	}

TVerdict TFileWriteBufferTestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iWriteLength, iBuffer, iPosition, iSource));
	if (err == KErrNone)
		CActiveScheduler::Start();

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

	iCopyOfDestinationDes.Replace(iPosition, iWriteLength, buffer->Data());
	return EPass;
	}

TVerdict TFileWriteBuffer2TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iBuffer, iPosition, iSource));
	if (err == KErrNone)
		CActiveScheduler::Start();

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

	iCopyOfDestinationDes.Replace(iPosition, iWriteLength, buffer->Data());
	return EPass;
	}

TVerdict TFileWriteBuffer3TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RandomiseBuffer();
	clip->WriteBufferL(iBuffer, iPosition);

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

	iCopyOfDestinationDes.Replace(iPosition, buffer->BufferSize(), buffer->Data());
	return EPass;
	}

TVerdict TFileWriteBuffer4TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* clip = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, fs, iFileName));
	CleanupDeletePushL(clip);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	clip->SinkPrimeL();

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iWriteLength, iBuffer, iPosition, iSource));
	if (err == KErrNone)
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs); // clip, fs

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt writeLength = iWriteLength;
	if (writeLength>buffer->Data().Length())
		writeLength = buffer->Data().Length();

	TInt position = iPosition;
	if (position>iCopyOfDestinationDes.Length())
		position = iCopyOfDestinationDes.Length();

	if ((position+writeLength)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(position+writeLength);

	TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
	iCopyOfDestinationDes.Replace(position, writeLength, bufferData);
	return EPass;
	}

TInt TFileWriteBuffer5TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead | EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	if (destinationFileSize != (length+(iWriteLength-buffer->Data().Length())))
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(destinationFileSize);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);

	// Copy the file.
	iCopyOfDestinationDes.Copy(destinationDes);

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return err;
	}

TVerdict TFileWriteBuffer5TestStep::DoNegativeTestPostambleL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length == 0) ? EPass : EFail;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return EFail;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(length);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return (err == 0) ? EPass : EFail;
	}

TVerdict TFileWriteBuffer6TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* clip = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, fs, iFileName));
	CleanupDeletePushL(clip);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	clip->SinkPrimeL();

	RandomiseBuffer();
	TRAPD(err, clip->WriteBufferL(iBuffer, iPosition, iSource));
	if (err == KErrNone)
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, &fs); // clip, fs

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt position = iPosition;
	TInt bufferSize = buffer->BufferSize();
	if (position>iCopyOfDestinationDes.Length())
		position = iCopyOfDestinationDes.Length();

	if ((position+bufferSize)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(position+bufferSize);

	iCopyOfDestinationDes.Replace(position, bufferSize, buffer->Data());
	return EPass;
	}

TVerdict TFileWriteBuffer7TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* clip = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, fs, iFileName));
	CleanupDeletePushL(clip);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	clip->SinkPrimeL();

	RandomiseBuffer();
	clip->WriteBufferL(iBuffer, iPosition);
	CleanupStack::PopAndDestroy(2, &fs);

	CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
	TInt position = iPosition;
	TInt bufferSize = buffer->BufferSize();
	if (position>iCopyOfDestinationDes.Length())
		position = iCopyOfDestinationDes.Length();

	if ((position+bufferSize)>iCopyOfDestinationDes.Length())
		iCopyOfDestinationDes.SetLength(position+bufferSize);

	iCopyOfDestinationDes.Replace(position, bufferSize, buffer->Data());
	return EPass;
	}

TVerdict TFileWriteBuffer8TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* clip = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, fs, iFileName));
	CleanupDeletePushL(clip);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	clip->SinkPrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = Min(iMaxRequestCount, bufferArray.Count());
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0; requestCount < maxRequestCount; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAPD(err, clip->WriteBufferL(iWriteLength, iBuffer, iPosition, iSource));
		if (err != KErrNone)
			{
			break;
			}

		iPosition += iWriteLength;
		}

	if (requestCount)
		{
		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(2, &fs);

	if (dummySource->ErrorCount())
		{
		User::Leave(dummySource->Error());
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		{
		return EFail;
		}

	iPosition = 0;
	TInt completionCount;
	for (completionCount = 0; completionCount < requestCount; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			{
			return EFail;
			}

		if ((iPosition + iWriteLength) > iCopyOfDestinationDes.Length())
			{
			iCopyOfDestinationDes.SetLength(iPosition + iWriteLength);
			}

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		iPosition += writeLength;
		}

	return EPass;
	}

TVerdict TFileWriteBuffer9TestStep::DoTestStepL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CMMFClip* clip = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, fs, iFileName));
	CleanupDeletePushL(clip);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	clip->SinkPrimeL();

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = Min(iMaxRequestCount, bufferArray.Count());
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0; requestCount < maxRequestCount; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAPD(err, clip->WriteBufferL(iBuffer, iPosition, iSource));
		if (err != KErrNone)
			{
			break;
			}

		iPosition += iWriteLength;
		}

	if (requestCount)
		{
		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(2, &fs);

	if (dummySource->ErrorCount())
		{
		User::LeaveIfError(dummySource->Error());
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		{
		return EFail;
		}

	iPosition = 0;
	TInt completionCount;
	for (completionCount = 0; completionCount < requestCount; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			{
			return EFail;
			}

		if ((iPosition + iWriteLength) > iCopyOfDestinationDes.Length())
			{
			iCopyOfDestinationDes.SetLength(iPosition + iWriteLength);
			}

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		iPosition += iWriteLength;
		}

	return EPass;
	}

TInt TFileWriteBuffer10TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead|EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(length);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining two writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);

		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);

	return err;
	}

TInt TFileWriteBuffer11TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead | EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(length);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining two writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);

		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);

	return err;
	}

TVerdict TFileWriteBuffer12TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAPD(err, clip->WriteBufferL(iWriteLength, iBuffer, iPosition, iSource));
		if (err != KErrNone)
			break;

		iPosition += iWriteLength;
		}

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		return EFail;

	if (!sourceBufferEmptiedOk && requestCount)
		{
		requestCount-= dummySource->ErrorCount();
		err = dummySource->Error();

		if (err == KErrNone)
			return EFail;
		}

	iPosition = 0;
	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
			iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		iPosition += iWriteLength;
		}

	if (err != KErrNone)
		User::Leave(err);

	clip->WriteBufferL(iWriteLength, bufferArray[0], iPosition + 2*iWriteLength, iSource);
	return EPass;
	}

TInt TFileWriteBuffer12TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead |EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		if (destinationFileSize != (length + iWriteLength))
			{
			CleanupStack::PopAndDestroy(&destinationFile);
			return -1;
			}
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(destinationFileSize);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining three writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			if (err != 0)
				return EFail;

			iPosition += iWriteLength;
			if ((iPosition+iWriteLength)<=iCopyOfDestinationDes.Length())
				return EFail;
			iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);
			length = iPosition+iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[0]);
			bufferData.Set(buffer->Data().Ptr(), iWriteLength);

			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return err;
	}

TVerdict TFileWriteBuffer13TestStep::DoTestStepL()
	{
	CMMFClip* clip = STATIC_CAST(CMMFClip*, iSink);

	RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
	TInt maxRequestCount = bufferArray.Count();
	TInt err = KErrNone;
	iPosition = 0;

	TInt requestCount;
	for (requestCount = 0 ; requestCount<maxRequestCount ; requestCount++)
		{
		iBuffer = bufferArray[requestCount];
		RandomiseBuffer();

		TRAPD(err, clip->WriteBufferL(iBuffer, iPosition, iSource));
		if (err != KErrNone)
			break;

		iPosition += iWriteLength;
		}

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	if (requestCount)
		{
		if (requestCount>iMaxRequestCount)
			requestCount = iMaxRequestCount;

		dummySource->SetExpectedEmptyCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if (!requestCount && sourceBufferEmptiedOk)
		return EFail;

	if (!sourceBufferEmptiedOk && requestCount)
		{
		requestCount-= dummySource->ErrorCount();
		err = dummySource->Error();

		if (err == KErrNone)
			return EFail;
		}

	iPosition = 0;
	TInt completionCount;
	for (completionCount = 0 ; completionCount<requestCount ; completionCount++)
		{
		iBuffer = bufferArray[completionCount];
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, iBuffer);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		if ((iPosition+iWriteLength)>iCopyOfDestinationDes.Length())
			iCopyOfDestinationDes.SetLength(iPosition+iWriteLength);

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

		iPosition += iWriteLength;
		}

	if (err != KErrNone)
		User::Leave(err);

	clip->WriteBufferL(bufferArray[0], iPosition + 2*iWriteLength, iSource);
	return EPass;
	}

TInt TFileWriteBuffer13TestStep::CheckDestinationL()
	{
	TInt length = iCopyOfDestinationDes.Length();

	// Read the file into a descriptor.
	RFile destinationFile;
	TInt err = destinationFile.Open(iFsSession, iDestinationFileName, EFileRead | EFileShareAny);
	CleanupClosePushL(destinationFile);
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(&destinationFile);

		if (err == KErrNotFound)
			return (length != 0) ? -1 : 0;

		User::Leave(err);
		}

	TInt destinationFileSize;
	User::LeaveIfError(destinationFile.Size(destinationFileSize));

	// Check that the file length is correct.
	if (destinationFileSize != length)
		{
		CleanupStack::PopAndDestroy(&destinationFile);
		return -1;
		}

	HBufC8* destinationHBuf = HBufC8::NewLC(destinationFileSize);
	TPtr8 destinationDes = destinationHBuf->Des();
	User::LeaveIfError(destinationFile.Read(destinationDes));

	// Check that the file data matches.
	err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
	if (err != 0)
		{
		// At least one of the remaining three writes was completed.
		RPointerArray<CMMFBuffer>& bufferArray = *iBufferArray;
		CMMFDataBuffer* buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[3]);
		TInt writeLength = buffer->BufferSize();
		if (writeLength != iWriteLength)
			return EFail;

		TPtrC8 bufferData(buffer->Data().Ptr(), writeLength);
		iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);
		err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
		if (err != 0)
			{
			iPosition += iWriteLength;

			buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[4]);
			writeLength = buffer->BufferSize();
			if (writeLength != iWriteLength)
				return EFail;

			bufferData.Set(buffer->Data().Ptr(), writeLength);
			iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

			err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
			if (err != 0)
				{
				iPosition += iWriteLength;

				buffer = STATIC_CAST(CMMFDataBuffer*, bufferArray[0]);
				writeLength = buffer->BufferSize();
				if (writeLength != iWriteLength)
					return EFail;

				bufferData.Set(buffer->Data().Ptr(), writeLength);
				iCopyOfDestinationDes.Replace(iPosition, iWriteLength, bufferData);

				err = Mem::Compare(destinationDes.Ptr(), length, iCopyOfDestinationDes.Ptr(), length);
				}
			}
		}

	CleanupStack::PopAndDestroy(destinationHBuf);
	CleanupStack::PopAndDestroy(&destinationFile);
	return err;
	}


TVerdict TAudioInputCreateSourceBuffer1TestStep::DoTestStepL()
	{
	CMMFAudioInput* source = STATIC_CAST(CMMFAudioInput*, iSource);

	TMediaId mediaId;
	CMMFBuffer* buffer = source->CreateSourceBufferL(mediaId);

	TVerdict result = EPass;
	if (buffer->Type() != KUidMmfDescriptorBuffer)
		result = EFail;

	if (buffer->Status() != EAvailable)
		result = EFail;

	CMMFDescriptorBuffer* descriptorBuffer = STATIC_CAST(CMMFDescriptorBuffer*, buffer);
	if (descriptorBuffer->Data().MaxLength() != (TInt)KAudioInputDefaultFrameSize)
		result = EFail;

	delete buffer;
	return result;
	}

TVerdict TAudioInputCreateSourceBuffer2TestStep::DoTestStepL()
	{
	CMMFAudioInput* source = STATIC_CAST(CMMFAudioInput*, iSource);

	TMediaId mediaId;
	CMMFBuffer* buffer = source->CreateSourceBufferL(mediaId, *iSinkBuffer);

	TVerdict result = EPass;
	if (buffer->Type() != KUidMmfDescriptorBuffer)
		result = EFail;

	if (buffer->Status() != EAvailable)
		result = EFail;

	CMMFDescriptorBuffer* descriptorBuffer = STATIC_CAST(CMMFDescriptorBuffer*, buffer);
	if (descriptorBuffer->Data().MaxLength() != (TInt)KAudioInputDefaultFrameSize)
		result = EFail;

	delete buffer;
	return result;
	}

TVerdict TAudioInputFillBufferTestStep::DoTestStepL()
	{
	TMediaId mediaId;
	TRAPD(err, iSource->FillBufferL(iBuffer, iSink, mediaId));	// first call inits

	if (err == KErrNone)
		CActiveScheduler::Start();

	TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));

	if (err == KErrNone)
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioInputFillBuffer2TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	TRAPD(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioInputFillBuffer3TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	TInt requestCount = 0;
	TRAPD(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		{
		requestCount++;
		TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
		if (err == KErrNone)
			{
			requestCount++;
			TRAP(err, iSource->FillBufferL(iBuffer, iSink, mediaId));
			}
		}

	if (err == KErrNone)
		requestCount++;

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	if (requestCount)
		{
		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioInputFillBuffer4TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSource* source = STATIC_CAST(MDataSource*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	TMediaId mediaId;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(source);

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioInputFillBuffer5TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSource* source = STATIC_CAST(MDataSource*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	TMediaId mediaId;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	CleanupStack::PopAndDestroy(source);
	return EPass;
	}

TVerdict TAudioInputFillBuffer6TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSource* source = STATIC_CAST(MDataSource*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	TMediaId mediaId;
	TInt requestCount = 0;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		{
		requestCount++;
		TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
		if (err == KErrNone)
			{
			requestCount++;
			TRAP(err, source->FillBufferL(iBuffer, iSink, mediaId));
			}
		}

	if (err == KErrNone)
		requestCount++;

	if (requestCount)
		{
		dummySink->SetExpectedFillCount(requestCount);
		CActiveScheduler::Start();
		}
	CleanupStack::PopAndDestroy(source);

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioInputFillBuffer7TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSource* source = STATIC_CAST(MDataSource*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	TMediaId mediaId;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	source->SourcePauseL();
	source->FillBufferL(iBuffer, iSink, mediaId);	// this call just inits devsound
//DevSound WP source is active cannot dictate what buffers to fill
//	source->FillBufferL(iBuffer, iSink, mediaId);	// this call should leave with KErrNotReady

	CleanupStack::PopAndDestroy(source);
	return EPass;
	}

TVerdict TAudioInputFillBuffer8TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSource* source = STATIC_CAST(MDataSource*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupDeletePushL(source);

	CMMFDummySink* dummySink = STATIC_CAST(CMMFDummySink*, iSink);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	TMediaId mediaId;
	TRAPD(err, source->FillBufferL(iBuffer, iSink, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	TBool sinkBufferFilledOk = dummySink->BufferFilledOk();
	if ((err != KErrNone) && sinkBufferFilledOk)
		{
		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sinkBufferFilledOk)
		{
		if ((err = dummySink->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(source);
		return EFail;
		}

	source->SourceStopL();
	source->FillBufferL(iBuffer, iSink, mediaId);	// this call just inits devsound
	source->FillBufferL(iBuffer, iSink, mediaId);	// this call should leave with KErrNotReady

	CleanupStack::PopAndDestroy(source);
	return EPass;
	}


TVerdict TAudioOutputEmptyBufferTestStep::DoTestStepL()
	{
	TMediaId mediaId;
	iSink->SinkPlayL();

	TRAPD(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));

	if ((err == KErrNone) && iAsynchronousWrite)
		CActiveScheduler::Start();

	iSink->SinkStopL();

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioOutputEmptyBuffer2TestStep::DoTestStepL()
	{
	TMediaId mediaId;
	iSink->SinkPlayL();

	TRAPD(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));
	if ((err == KErrNone) && iAsynchronousWrite)
		CActiveScheduler::Start();

	iSink->SinkStopL();

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);
	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		{
		iSink->SinkStopL();
		User::Leave(err);
		}

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			{
			iSink->SinkStopL();
			User::Leave(err);
			}

		return EFail;
		}

	iSink->SinkPlayL();

	TRAP(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));
	if ((err == KErrNone) && iAsynchronousWrite)
		CActiveScheduler::Start();

	iSink->SinkStopL();

	sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		{
		iSink->SinkStopL();
		User::Leave(err);
		}

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			{
			iSink->SinkStopL();
			User::Leave(err);
			}

		return EFail;
		}

	iSink->SinkPlayL();

	TRAP(err, iSink->EmptyBufferL(iBuffer, iSource, mediaId));
	if ((err == KErrNone) && iAsynchronousWrite)
		CActiveScheduler::Start();

	iSink->SinkStopL();

	sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		return EFail;
		}

	return EPass;
	}

TVerdict TAudioOutputEmptyBuffer3TestStep::DoTestStepL()
	{
	// Fill in!
	return EPass;
	}

TVerdict TAudioOutputEmptyBuffer4TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSink* sink = STATIC_CAST(MDataSink*, MDataSink::NewSinkL(KUidMmfAudioOutput, configDes));
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);

	//DevSound WP Logon must come before Prime
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();
	//DevSound WP User::LeaveIfError(sink->SinkThreadLogon(*dummySource));

	sink->NegotiateL(iRealFormat);

	TMediaId mediaId;
	sink->SinkPlayL();
	TRAPD(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(sink);

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		return EFail;

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{//DevSound WP because the active scheduler can leave
		//we cannot assume that the source buffer has been emptied ok in the alloc test
		//	if ((err = dummySource->Error()) != KErrNone)
		//		User::Leave(err);

		//	return EFail;
		}

	return EPass;
	}

TVerdict TAudioOutputEmptyBuffer5TestStep::DoTestStepL()
	{
	TBuf8<1> configDes;
	MDataSink* sink = STATIC_CAST(MDataSink*, MDataSink::NewSinkL(KUidMmfAudioOutput, configDes));
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = STATIC_CAST(CMMFDummySource*, iSource);

	//DevSound WP Logon must come before Prime
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();
	//DevSound WP	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));

	sink->NegotiateL(iRealFormat);
	sink->SinkPlayL();

	TMediaId mediaId;
	TRAPD(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));

	CActiveScheduler::Start();

	TBool sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		{
		CleanupStack::PopAndDestroy(sink);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		/* DevSound WP may not be emptied ok if OOM in AS
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(sink);
		return EFail;
		*/
		}

	dummySource->SetActiveScheduler();
	TRAP(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		{
		CleanupStack::PopAndDestroy(sink);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(sink);
		return EFail;
		}

	TRAP(err, sink->EmptyBufferL(iBuffer, iSource, mediaId));
	if (err == KErrNone)
		CActiveScheduler::Start();

	sourceBufferEmptiedOk = dummySource->BufferEmptiedOk();
	if ((err != KErrNone) && sourceBufferEmptiedOk)
		{
		CleanupStack::PopAndDestroy(sink);
		return EFail;
		}

	if (err != KErrNone)
		User::Leave(err);

	if (!sourceBufferEmptiedOk)
		{
		if ((err = dummySource->Error()) != KErrNone)
			User::Leave(err);

		CleanupStack::PopAndDestroy(sink);
		return EFail;
		}

	CleanupStack::PopAndDestroy(sink);
	return EPass;
	}


//_LIT(KTest301CopyFileName, "c:\\mm\\mmf\\testfiles\\srssnk\\CopyOfMMFTestFile1.dat");	// EABI warning removal
_LIT(KTest301FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile1.wav");

TVerdict TPrimeStopTestStep::DoTestStepL()
	{
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest301FileName;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(source);

	TUid KWavReadUid = { KMmfUidFormatWAVRead };
	CMMFFormatDecode* format = CMMFFormatDecode::NewL(KWavReadUid, source);
	CleanupStack::PushL(format);

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Prime
	TRAPD(err, format->SourcePrimeL());

	if (err != KErrNone)
		{
		format->SourcePrimeL();
		}

	// This will leave during alloc fail test
	TAny* randomAlloc = User::AllocL(10);
	User::Free(randomAlloc);

	CleanupStack::PopAndDestroy(scheduler);
	CleanupStack::PopAndDestroy(format);
	CleanupStack::PopAndDestroy(source);

	return EPass;
	}


_LIT(KTest302FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile1.wav");

TVerdict TPrimeStopSinkTestStep::DoTestStepL()
	{
	// Create a new sink.
	TBuf8<1> dummyConfigDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, dummyConfigDes);
	CleanupDeletePushL(sink);

	// Create a real source.
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest302FileName;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(source);

	TUid KWavReadUid = { KMmfUidFormatWAVRead };
	CMMFFormatDecode* format = CMMFFormatDecode::NewL(KWavReadUid, source);
	CleanupStack::PushL(format);

	CMMFWavFormatRead* realFormat = static_cast<CMMFWavFormatRead*>(format);
	User::LeaveIfError(sink->SinkThreadLogon(*realFormat));
	sink->NegotiateL(*format);

	TRAPD(err, sink->SinkPrimeL());
	if (err != KErrNone)
		{
		sink->SinkPrimeL();
		}

	CleanupStack::PopAndDestroy(format);
	CleanupStack::PopAndDestroy(source);
	CleanupStack::PopAndDestroy(sink);

	return EPass;
	}

/**
 *
 * CTestStepSelfTest constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK::CTestStep_MMF_SRSSNK(TBool aIsAllocTest)
: iTestResult(EPass), iIsAllocTest(aIsAllocTest)
	{
	iHeapSize = 1048576; // 1M

	RFs theFs;
	if (theFs.Connect() == KErrNone)
		{
		if (theFs.SetErrorCondition(KErrNone) == KErrNone)
			iFileFailureTestingEnabled = ETrue;

		theFs.Close();
		}
	}

/**
 *
 * CTestStepSelfTest destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK::~CTestStep_MMF_SRSSNK()
	{
	}

void CTestStep_MMF_SRSSNK::DoTest(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount)
	{
	DoTestStep(aTestStep, aTestDescription, aExpectedTestResult, aHeapCellCount);
	DoTestStepWithAllocFail(aTestStep, aTestDescription, aExpectedTestResult, aHeapCellCount);
	}

void CTestStep_MMF_SRSSNK::DoFileTest(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult)
	{
	DoTestStep(aTestStep, aTestDescription, aExpectedTestResult);
	DoTestStepWithAllocFail(aTestStep, aTestDescription, aExpectedTestResult);

	DoTestStepWithFileFail(aTestStep, aTestDescription, aExpectedTestResult);

	// The test below is no longer run. Running it will cause an ASSERT.
	//DoTestStepWithAllocAndFileFail(aTestStep, aTestDescription, aExpectedTestResult);
	}

void CTestStep_MMF_SRSSNK::DoFileTestWithoutAllocFail(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount)
	{
	DoTestStep(aTestStep, aTestDescription, aExpectedTestResult, aHeapCellCount);
	DoTestStepWithFileFail(aTestStep, aTestDescription, aExpectedTestResult, aHeapCellCount);
	}

void CTestStep_MMF_SRSSNK::DoTestStep(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt /*__REQUIREDFORDEBUG(aHeapCellCount)*/)
	{
	TVerdict verdict = EPass;

	TRAPD(preErr, aTestStep.DoTestPreambleL());

	if (preErr != KErrNone)
		{
		INFO_PRINTF3(_L("%S: Test pre amble failed %d\n"), &aTestDescription, preErr);
		iTestResult = EFail;
		return;
		}

	TRAPD(leaveErr, verdict = aTestStep.DoTestStepL());

	if (verdict == EPass)
		{
		if (leaveErr != aExpectedTestResult)
			{
			INFO_PRINTF3(_L("DoTestStep failed: leaveErr (%d) != expected (%d)"), leaveErr, aExpectedTestResult);
			verdict = EFail;
			}
		else if (leaveErr == KErrNone)
			{
			TRAPD(postErr, verdict = aTestStep.DoTestPostambleL(ETrue));
			if (postErr != KErrNone)
				{
				INFO_PRINTF2(_L("DoTestStep failed: postErr (%d)"), postErr);
				verdict = EInconclusive;
				}
			}
		else
			{
			TRAPD(postErr, verdict = aTestStep.DoNegativeTestPostambleL());
			if (postErr != KErrNone)
				{
				INFO_PRINTF2(_L("DoTestStep failed: postErr (%d)"), postErr);
				verdict = EInconclusive;
				}
			}
		}

	if (verdict != EPass)
		{
		INFO_PRINTF3(_L("%S: Test verdict = %d\n"), &aTestDescription, verdict);
		INFO_PRINTF4(_L("%S: Expecting %d, actually got %d\n"), &aTestDescription, aExpectedTestResult, leaveErr);
		iTestResult = verdict;
		}
	}

void CTestStep_MMF_SRSSNK::DoTestStepWithAllocFail(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedResult, TInt /*__REQUIREDFORDEBUG(aHeapCellCount)*/)
	{
	TInt err;
	TInt postErr;
	TInt failPoint = 0;
	TBool finished = EFalse;
	TVerdict verdict;
	TVerdict postVerdict;

	iTestResult = EFail;
	do
		{
		// Do the preamble.
		TRAP(err, aTestStep.DoTestPreambleL());
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("%S preamble failed"), &aTestDescription);
			TRAP_IGNORE(aTestStep.DoNegativeTestPostambleL());
			return;
			}

		// Do the test step.
		failPoint = failPoint + 1;	// doing 'failPoint++' causes a compiler warning on ARMV5 UREL.
		__UHEAP_SETFAIL(RHeap::EFailNext, failPoint);
		verdict = EFail;
		TRAP(err, verdict = aTestStep.DoTestStepL());

		TAny* ptr = User::Alloc(1);
		finished = (ptr == NULL);
		User::Free(ptr);
		__UHEAP_RESET;

		// Do the postamble.
		postVerdict = EFail;
		if (err == KErrNone)
			{
			TRAP(postErr, postVerdict = aTestStep.DoTestPostambleL(ETrue));
			}
		else
			{
			TRAP(postErr, postVerdict = aTestStep.DoNegativeTestPostambleL());
			}

		// Check postamble results.
		if ((postErr != KErrNone) && (postVerdict != EPass))
			{
			INFO_PRINTF3(_L("Postamble failed: %d (postVerdict = %d)"), postErr, postVerdict);
			return;
			}
		}
	while (!finished);

	// Check we got the expected result.
	if ((aExpectedResult == KErrNone) && (verdict != EPass))
		{
		INFO_PRINTF2(_L("TestStep failed: %d"), verdict);
		return;
		}

	iTestResult = (err == aExpectedResult ? EPass : EFail);
	}

void CTestStep_MMF_SRSSNK::DoTestStepWithFileFail(TFileTestStep& aTestStep, TPtrC /*aTestDescription*/, TInt aExpectedResult, TInt /*__REQUIREDFORDEBUG(aHeapCellCount)*/)
	{
	RFs fs;
	TInt err;
	TInt postErr;
	TBool isExpectedResult;
	TVerdict verdict;
	TVerdict postVerdict = EPass;

	iTestResult = EPass;
	if (!iFileFailureTestingEnabled)
		{
		return;
		}

	err = fs.Connect();
	if (err != KErrNone)
		{
		User::Panic(_L("SRSSNK Test"), err);
		}

	aTestStep.iFileFailureExpected = ETrue;

	// Once Base's DEF103757 has been submitted to the MCL
	// the counter 'failAt' should be set to zero.
	// DP roms need to have this number set higher due to the
	// additional paging operations done by RFs.
	TInt failAt = 15;
	do
		{
		// Do the preamble.
		TRAP(err, aTestStep.DoTestPreambleL());
		if (err != KErrNone)
			{
			iTestStepResult = EInconclusive;
			fs.Close();
			return;
			}

		// Set the file failure point.
		fs.SetErrorCondition(KErrFileFail, failAt++);
		verdict = EFail;
		TRAP(err, verdict = aTestStep.DoTestStepL());
		fs.SetErrorCondition(KErrNone);

		isExpectedResult = (err == aExpectedResult);

		postVerdict = EFail;
		if (err == KErrNone)
			{
			TRAP(postErr, postVerdict = aTestStep.DoTestPostambleL(ETrue));
			}
		else
			{
			TRAP(postErr, postVerdict = aTestStep.DoNegativeTestPostambleL());
			}
		}
	while (err == KErrFileFail);

	fs.Close();

	// Decide the test result.
	if ((postErr != KErrNone) || (postVerdict != EPass))
		{
		iTestResult = postVerdict;
		return;
		}

	// If there's an expected failure, verdict will be EFail.
	// This doesn't mean the test has failed.
	if ((aExpectedResult == KErrNone) && (verdict != EPass))
		{
		iTestResult = verdict;
		return;
		}

	iTestResult = (isExpectedResult ? EPass : EFail);
	}

//
// The test is no longer run as its correctness is not proven.
// It is kept for reference purposes.
//
void CTestStep_MMF_SRSSNK::DoTestStepWithAllocAndFileFail(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedResult)
	{
	ASSERT(0);	// Don't run this test.

	if (!iFileFailureTestingEnabled)
		return;

	RFs theFs;
	TInt leaveErr = theFs.Connect();
	if (leaveErr != KErrNone)
		User::Panic(_L("SRSSNK Test"), 0);

	aTestStep.iFileFailureExpected = ETrue;
	TVerdict verdict = EPass;
	TBool allocTestingComplete;
	for (TInt failAt = 1, fileFailAt = KStartFileFailAt ; ; failAt++)
		{
		for (; ; fileFailAt++)
			{
			TRAPD(preErr, aTestStep.DoTestPreambleL());
			if (preErr != KErrNone)
			{
				INFO_PRINTF2(_L("%S: Test pre amble failed\n"), &aTestDescription);
				iTestResult = EFail;
				return;
			}

			__UHEAP_SETFAIL(RHeap::EFailNext, failAt);	// Leavescan will complain about this, although it isn't a leaving function
			theFs.SetErrorCondition(KErrFileFail, fileFailAt);
 			TRAP(leaveErr, verdict = aTestStep.DoTestStepL());
			theFs.SetErrorCondition(KErrNone);

			TAny* testAlloc = User::Alloc(1);
			allocTestingComplete = (testAlloc == NULL);
			User::Free(testAlloc);
			__UHEAP_SETFAIL(RHeap::ENone, 0);

			if (verdict == EPass)
				{
				if ((leaveErr != aExpectedResult) && (leaveErr != KErrFileFail) && (leaveErr != KErrNoMemory))
					verdict = EFail;
				else if (leaveErr == KErrNone)
					{
					TRAPD(postErr, verdict = aTestStep.DoTestPostambleL(ETrue));
					if (postErr != KErrNone)
						verdict = EInconclusive;
					}
				else
					{
					// ignore the verdict from DoNegativeTestPostambleL()
					// since the main test has failed it's likely to fail too
					TRAPD(postErr, aTestStep.DoNegativeTestPostambleL());
					if (postErr != KErrNone)
						verdict = EInconclusive;
					}
				}

			if (verdict != EPass)
				{
				INFO_PRINTF3(_L("%S + alloc+file fail: Test verdict = %d\n"), &aTestDescription, verdict);
				INFO_PRINTF4(_L("%S + alloc+file fail: Expecting %d, actually got %d"), &aTestDescription, aExpectedResult, leaveErr);

				iTestResult = verdict;
				break;
				}

			if ((leaveErr == aExpectedResult) || (leaveErr == KErrNoMemory))
				break;
			}

		if (verdict == EPass)
			{
			if (allocTestingComplete)
				{
				if (leaveErr != aExpectedResult)
					{
					INFO_PRINTF3(_L("%S + alloc fail: Test verdict = %d\n"), &aTestDescription, EFail);
					INFO_PRINTF4(_L("%S + alloc fail: Expecting %d, actually got %d"), &aTestDescription, aExpectedResult, leaveErr);

					iTestResult = EFail;
					}

				break;
				}
			}
		else
			{
				// added 17/01/03 - prevents looping on EFail
				iTestResult = verdict;
				break;
			}
		}
	aTestStep.iFileFailureExpected = EFalse;
	theFs.Close();
	}

TVerdict CTestStep_MMF_SRSSNK::OpenL()
	{
	if (iIsAllocTest)
		{
		// see if alloc fail stuff is enabled - ask if next alloc test will fail
		__UHEAP_SETFAIL(RHeap::EFailNext, 1);
		TAny *testAlloc = User::Alloc(1);
		TBool workingOk = testAlloc == NULL;
		User::Free(testAlloc);
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (!workingOk)
			{
			ERR_PRINTF1(_L("Alloc Testing does not work on this build"));
			return EInconclusive;
			}
		}

	return EPass;
	}

void CTestStep_MMF_SRSSNK::CopyFileL(RFs& aFs, const TDesC& aOld, const TDesC& aNew)
	{
	CFileMan* fileMan = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(aOld, aNew));
	CleanupStack::PopAndDestroy(fileMan);
	}

void CTestStep_MMF_SRSSNK::OpenFileL(const TDesC& aFileName, TInt aMode, TBool aShareProtected)
	{
	User::LeaveIfError(iFs.Connect());

	if (aShareProtected)
		{
		User::LeaveIfError(iFs.ShareProtected());
		}

	User::LeaveIfError(iFile.Open(iFs, aFileName, aMode));
	}



void CTestStep_MMF_SRSSNK::CloseFile()
	{
	iFile.Close();
	iFs.Close();
	}

void CTestStep_MMF_SRSSNK::DeleteFileSinkL(TSinkType aSinkType, RFs& aFs, TDesC& aFileName)
	{
	// Delete the test file.
	CMMFClip* sink = static_cast<CMMFClip*> (CreateFileSinkL(aSinkType, aFs, aFileName));
	
	CleanupStack::PushL(sink);
	TInt err = sink->Delete();
	CleanupStack::PopAndDestroy(sink);

		// can't use Delete from File Sink itself - when using file handles, must delete it once file is closed
	if (aSinkType == ESinkTypeFileHandle)
		{
		err = aFs.Delete(aFileName);
		}
			
	if (err != KErrNone && err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Error deleting file sink %d"), err);
		User::Leave(err);
		}	
	}

// Test support classes
CMMFDummySource::CMMFDummySource()
: MDataSource(KTestDummyUid), iExpectedEmptyCount(1), iActiveScheduler(CActiveScheduler::Current()), iFillBuffer(EFalse)
	{
	}

TFourCC CMMFDummySource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return iSourceDataTypeCode;
	}

void CMMFDummySource::FillBufferL(CMMFBuffer* /*aBuffer*/ , MDataSink* /*aConsumer*/, TMediaId /*aMediaId*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	}

void CMMFDummySource::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
	iEmptyCount++;
	if (iEmptyCount == iExpectedEmptyCount)
		iBufferEmptiedOk = ETrue;

	if ((iActiveScheduler != NULL) && iBufferEmptiedOk)
		CActiveScheduler::Stop();

	if (iFillBuffer)
		{
		CMMFDataBuffer* dataBuffer = STATIC_CAST(CMMFDataBuffer*, aBuffer);
		dataBuffer->Data().SetLength(KTestWriteSize);
		}

	iBuffer = aBuffer;	// save for test 208
	}

TInt CMMFDummySource::SendEventToClient(const TMMFEvent& aEvent)
	{
	if (aEvent.iErrorCode == KErrCancel) // Ignore stop.
		{
		return 0;
		}

	//DevSound WP active scheduler should also be stopped for OOM error
	//since OOM can occur during active scheduler
	if ((iActiveScheduler != NULL) && (aEvent.iErrorCode == KErrNoMemory))
		{
		iError = aEvent.iErrorCode;
		CActiveScheduler::Stop();
		return 0;
		}

	iError = aEvent.iErrorCode;
	iErrorCount++;

	iEmptyCount++;
	if ((iActiveScheduler != NULL) && (iEmptyCount == iExpectedEmptyCount))
		CActiveScheduler::Stop();

	return 0;
	}

TBool CMMFDummySource::CanCreateSourceBuffer()
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return EFalse;
	}

CMMFBuffer* CMMFDummySource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return NULL;
	}

void CMMFDummySource::ConstructSourceL(const TDesC8& /*aInitData*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	}


CMMFDummySink::CMMFDummySink()
: MDataSink(KTestDummyUid), iExpectedFillCount(1), iActiveScheduler(CActiveScheduler::Current())
	{
	}

TFourCC CMMFDummySink::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return iSinkDataTypeCode;
	}

void CMMFDummySink::EmptyBufferL(CMMFBuffer* /*aBuffer*/ , MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	}

void CMMFDummySink::BufferFilledL(CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;

	iFillCount++;
	if (iFillCount == iExpectedFillCount)
		iBufferFilledOk = ETrue;

	if ((iActiveScheduler != NULL) && iBufferFilledOk)
		CActiveScheduler::Stop();
	}

TInt CMMFDummySink::SendEventToClient(const TMMFEvent& aEvent)
	{
	iError = aEvent.iErrorCode;
	iErrorCount++;

	iFillCount++;
	if ((iActiveScheduler != NULL) && (iFillCount == iExpectedFillCount))
		CActiveScheduler::Stop();

	return 0;
	}

TBool CMMFDummySink::CanCreateSinkBuffer()
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return EFalse;
	}

CMMFBuffer* CMMFDummySink::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	return NULL;
	}

void CMMFDummySink::ConstructSinkL(const TDesC8& /*aInitData*/)
	{
	User::Panic(_L("SRSSNK Test"), 0);
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0001 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0001::CTestStep_MMF_SRSSNK_U_0001()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0001");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0001 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0001::~CTestStep_MMF_SRSSNK_U_0001()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0001
 *
 * Test creation/destruction of descriptor source.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0001::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create the actual descriptor source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Set it's length.
	dataPtr.SetLength(KTestBufferSize);

	// Initialise the config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Attempt to create a new descriptor source.
	// Use a valid uid and configuration descriptor.
	// The test should be successful.
	TNewSourceTestStep testStep(KUidMmfDescriptorSource, configDes);
	DoTest(testStep, _L("Construct descriptor source"), KErrNone);

	// Create a descriptor source for use with the other tests.
	// Mark the heap to enable us to test for leaks within the source object.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test that the source has been initialised correctly.
	// The test should be successful.
	TSourceTypeTestStep testStep2(source, KUidMmfDescriptorSource, KMMFFourCCCodeNULL, EFalse);
	DoTest(testStep2, _L("Descriptor source type"), KErrNone);

	// Test that the source cannot create a source buffer.
	// The test should be successful.
	TCanCreateSourceBufferTestStep testStep3(source, EFalse);
	DoTest(testStep3, _L("Descriptor CanCreateSourceBuffer"), KErrNone);

	// Test that the source cannot source sample convert.
	// The test should be successful.
	TSourceSampleConvertTestStep testStep4(source, EFalse);
	DoTest(testStep4, _L("Descriptor source SourceSampleConvert"), KErrNone);

	// Test that the clip is initialised correctly.
	CMMFClip *sourceClip = STATIC_CAST(CMMFClip*, source);

	// Test that the number of bytes free is what we would expect.
	// The test should be successful.
	TClipBytesFreeTestStep testStep5(sourceClip, KTestDataSize-KTestBufferSize);
	DoTest(testStep5, _L("Descriptor source BytesFree"), KErrNone);

	// Test that the size of the clip is what we would expect.
	// The test should be successful.
	TClipSizeTestStep testStep6(sourceClip, KTestBufferSize);
	DoTest(testStep6, _L("Descriptor source Size"), KErrNone);

	// Test that the size of the clip cannot be set.
	// The test should be successful.
	TClipSetSizeTestStep testStep7(sourceClip, ETrue);
	DoTest(testStep7, _L("Descriptor source SetSize"), KErrNone);

	// Attempt to delete the clip.
	// This test should fail, leaving with KErrNotSupported.
	TClipDeleteTestStep testStep8(sourceClip);
	DoTest(testStep8, _L("Descriptor source Delete"), KErrNotSupported);

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(dataDes);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0002 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0002::CTestStep_MMF_SRSSNK_U_0002()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0002");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0002 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0002::~CTestStep_MMF_SRSSNK_U_0002()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0002
 *
 * Attempt to create descriptor source using invalid config data.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0002::DoTestStepL()
	{
	__MM_HEAP_MARK;

	TBool failed = EFalse;

	// Create the actual descriptor source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes.SetLength(0);

	// Test invalid uid and configuration descriptor.
	TNewSourceTestStep testStep(KNullUid, configDes);
	DoTest(testStep, _L("Construct descriptor source 0002.1"), KErrArgument);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L(" - 0002.1 failed"));
		failed = ETrue;
		iTestResult = EPass;
		}

	// Test invalid configuration descriptor. (Zero length)
	TNewSourceTestStep testStep2(KUidMmfDescriptorSource, configDes);
	DoTest(testStep2, _L("Construct descriptor source 0002.2"), KErrGeneral);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L(" - 0002.2 failed"));
		failed = ETrue;
		iTestResult = EPass;
		}


	// Test invalid configuration descriptor. (Filled with zeros)
	Mem::FillZ(CONST_CAST(TUint8*, dataPtr.Ptr()), KTestDataSize);
	TNewSourceTestStep testStep3(KUidMmfDescriptorSource, dataPtr);
	DoTest(testStep3, _L("Construct descriptor source 0002.3"), KErrGeneral);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L(" - 0002.3 failed"));
		failed = ETrue;
		iTestResult = EPass;
		}


	// Test invalid uid.
	const TUid KInvalidUid = { 0x00690069 };
	configDes().iDes = &dataDes;
	configDes().iDesThreadId = RThread().Id();
	configDes.SetLength(sizeof(TMMFDescriptorParams));

	TNewSourceTestStep testStep4(KInvalidUid, configDes);
	DoTest(testStep4, _L("Construct descriptor source 0002.4"), KErrNotFound);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L(" - 0002.4 failed"));
		failed = ETrue;
		iTestResult = EPass;
		}


	// Test valid uid, invalid descriptor.
	// Note using a thread ID of 0 behaves differently on streaming so this
	// test has been removed.
	// configDes().iDesThreadId = 0;

	// TNewSourceTestStep testStep5(KUidMmfDescriptorSource, configDes);
	// DoTest(testStep5, _L("Construct descriptor source 5"), KErrNotFound);

	// Test invalid (sink) uid.
	// Note that ECom has no way of validating that Uids are of the correct type,
	// so this test is removed. (It causes an unhandled exception)
	// TNewSourceTestStep testStep6(KUidMmfDescriptorSink, configDes);
	// DoTest(testStep6, _L("Construct descriptor source"), KErrNotFound);

	CleanupStack::PopAndDestroy(dataDes);
	__MM_HEAP_MARKEND;

	return (failed ? EFail : EPass);
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0003 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0003::CTestStep_MMF_SRSSNK_U_0003()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0003");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0003 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0003::~CTestStep_MMF_SRSSNK_U_0003()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0003
 *
 * Test creation/destruction of descriptor sink.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0003::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create sink buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Set it's length.
	dataPtr.SetLength(KTestBufferSize);

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Test valid uid and configuration descriptor.
	TNewSinkTestStep testStep(KUidMmfDescriptorSink, configDes);
	DoTest(testStep, _L("Construct descriptor sink"), KErrNone);

	// Create a descriptor sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfDescriptorSink, configDes);
	CleanupDeletePushL(sink);

	// Test that the sink is initialised correctly.
	TSinkTypeTestStep testStep2(sink, KUidMmfDescriptorSink, KMMFFourCCCodeNULL, EFalse);
	DoTest(testStep2, _L("Descriptor sink type"), KErrNone);

	TCanCreateSinkBufferTestStep testStep3(sink, EFalse);
	DoTest(testStep3, _L("Descriptor sink CanCreateSinkBuffer"), KErrNone);

	// Test that the clip is initialised correctly.
	CMMFClip *sinkClip = STATIC_CAST(CMMFClip*, sink);
	TClipBytesFreeTestStep testStep4(sinkClip, KTestDataSize-KTestBufferSize);
	DoTest(testStep4, _L("Descriptor sink BytesFree"), KErrNone);

	TClipSizeTestStep testStep5(sinkClip, KTestBufferSize);
	DoTest(testStep5, _L("Descriptor sink Size"), KErrNone);

	TClipSetSizeTestStep testStep6(sinkClip, ETrue);
	DoTest(testStep6, _L("Descriptor sink SetSize"), KErrNone);

	TClipDeleteTestStep testStep7(sinkClip);
	DoTest(testStep7, _L("Descriptor sink Delete"), KErrNotSupported);

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(dataDes);
	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0004 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0004::CTestStep_MMF_SRSSNK_U_0004()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0004");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0004 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0004::~CTestStep_MMF_SRSSNK_U_0004()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0004
 *
 * Attempt to create descriptor sink using invalid config data.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0004::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create sink buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes.SetLength(0);

	// Test invalid uid and configuration descriptor.
	TNewSinkTestStep testStep(KNullUid, configDes);
	DoTest(testStep, _L("Construct descriptor sink 1"), KErrArgument);

	// Test invalid configuration descriptor. (Zero length)
	TNewSinkTestStep testStep2(KUidMmfDescriptorSink, configDes);
	DoTest(testStep2, _L("Construct descriptor sink 2"), KErrGeneral);

	// Test invalid configuration descriptor. (Filled with zeros)
	Mem::FillZ(CONST_CAST(TUint8*, dataPtr.Ptr()), KTestDataSize);
	TNewSinkTestStep testStep3(KUidMmfDescriptorSink, dataPtr);
	DoTest(testStep3, _L("Construct descriptor sink 3"), KErrGeneral);

	// Test invalid uid.
	const TUid KInvalidUid = { 0x00690069 };
	configDes().iDes = &dataDes;
	configDes().iDesThreadId = RThread().Id();
	configDes.SetLength(sizeof(TMMFDescriptorParams));

	TNewSinkTestStep testStep4(KInvalidUid, configDes);
	DoTest(testStep4, _L("Construct descriptor sink 4"), KErrNotFound);

	// Test valid uid, invalid descriptor.
	configDes().iDesThreadId = (TUint) -1;
	TNewSinkTestStep testStep5(KUidMmfDescriptorSink, configDes);
	DoTest(testStep5, _L("Construct descriptor sink 5"), KErrNotFound);

	// Test invalid (source) uid.
	// Note that ECom has no way of validating that Uids are of the correct type,
	// so this test is removed. (It causes an unhandled exception)
	// TNewSinkTestStep testStep6(KUidMmfDescriptorSource, configDes);
	// DoTest(testStep6, _L("Construct descriptor sink"), KErrNotFound);

	CleanupStack::PopAndDestroy(dataDes);
	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0005 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0005::CTestStep_MMF_SRSSNK_U_0005()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0005");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0005 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0005::~CTestStep_MMF_SRSSNK_U_0005()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0005
 *
 * Functional test of descriptor source.
 * Misc. functions test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0005::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create a dummy buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create a descriptor source.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test create source buffer.
	TCreateSourceBuffer1TestStep testStep(ESourceTypeDes, dummySink, KUidMmfDescriptorBuffer, ETrue);
	DoTest(testStep, _L("Descriptor source CreateSourceBuffer1"), KErrNotSupported);

	TCreateSourceBuffer2TestStep testStep2(ESourceTypeDes, dummySink, validBuffer, KUidMmfDescriptorBuffer, ETrue);
	DoTest(testStep2, _L("Descriptor source CreateSourceBuffer2"), KErrNotSupported);

	// Test default base class fns. (These should all do nothing)
	TSourceThreadLogonTestStep testStep3(source, dummySink);
	DoTest(testStep3, _L("Descriptor source SourceThreadLogon"), KErrNone);

	TNegotiateSourceTestStep testStep4(source, dummySink);
	DoTest(testStep4, _L("Descriptor source NegotiateSourceL"), KErrNone);

	source->SourcePrimeL();
	source->SourcePlayL();
	source->SourcePauseL();
	source->SourceStopL();

	TMMFPrioritySettings prioritySettings;
	source->SetSourcePrioritySettings(prioritySettings);

	CleanupStack::PopAndDestroy(source);


	CleanupStack::PopAndDestroy(3, dataDes); // dummySink, validBuffer, dataDes
	__MM_HEAP_MARKEND;

	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0006 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0006::CTestStep_MMF_SRSSNK_U_0006()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0006");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0006 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0006::~CTestStep_MMF_SRSSNK_U_0006()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0006
 *
 * Functional test of descriptor source.
 * FillBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0006::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Set it's length
	dataPtr.SetLength(KTestDataSize);

	// Randomise source buffer.
	TTime seedTime;
	seedTime.UniversalTime();
	TInt64 seed = seedTime.Int64();

	TInt i;
	for (i = 0 ; i<KTestDataSize ; i++)
		dataPtr[i] = TUint8(Math::Rand(seed));

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create zero length buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create a descriptor source.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Create test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test invalid FillBufferL.
	TFillBufferTestStep testStep(source, testBuffer, dummySink, dataPtr);
	DoTest(testStep, _L("Descriptor source FillBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test valid fill.
	TFillBufferTestStep testStep2(source, validBuffer, dummySink, dataPtr, KTestBufferSize);
	DoTest(testStep2, _L("Descriptor source FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(source);

	source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test complete fill.
	TFillBufferTestStep testStep3(source, validBuffer, dummySink, dataPtr);

	const TUint8* baseDataPtr = dataPtr.Ptr();
	const TUint8* sourcePtr = baseDataPtr;
	TInt bufferSize;
	do
		{
		testStep3.SetReadLength(Min(KTestBufferSize, KTestDataSize - (sourcePtr-baseDataPtr)));
		DoTestStep(testStep3, _L("Descriptor source FillBufferL"), KErrNone);

		bufferSize = validBuffer->BufferSize();
		sourcePtr += bufferSize;
		} while (bufferSize == KTestBufferSize);

	CleanupStack::PopAndDestroy(source);

	source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test complete fill with alloc fail.
	TFillBufferTestStep testStep4(source, validBuffer, dummySink, dataPtr);

	sourcePtr = baseDataPtr;
	do
		{
		testStep4.SetReadLength(Min(KTestBufferSize, KTestDataSize - (sourcePtr-baseDataPtr)));
		DoTestStepWithAllocFail(testStep4, _L("Descriptor source FillBufferL"), KErrNone);

		bufferSize = validBuffer->BufferSize();
		sourcePtr += bufferSize;
		} while (bufferSize == KTestBufferSize);

	// Test additional fill request.
	TFillBufferTestStep testStep5(source, validBuffer, dummySink, dataPtr);
	DoTest(testStep5, _L("Descriptor source FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(source);

	source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test zero length read.
	TFillBufferTestStep testStep6(source, zeroBuffer, dummySink, dataPtr);
	DoTest(testStep6, _L("Descriptor source FillBufferL"), KErrNone);

	// Test over size read.
	TFillBufferTestStep testStep7(source, bigBuffer, dummySink, dataPtr, KTestDataSize);
	DoTestStep(testStep7, _L("Descriptor source FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(source);

	source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	testStep7.SetPosition(0);
	DoTestStepWithAllocFail(testStep7, _L("Descriptor source FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(5, zeroBuffer); // source, dummySink, validBuffer, bigBuffer, zeroBuffer

	CleanupStack::PopAndDestroy(dataDes); // dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0007 constructor
 *
 * @xxxx
 *
 */

CTestStep_MMF_SRSSNK_U_0007::CTestStep_MMF_SRSSNK_U_0007()
: CTestStep_MMF_SRSSNK(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0007");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0007 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0007::~CTestStep_MMF_SRSSNK_U_0007()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0007
 *
 * Functional test of descriptor source.
 * BufferEmptiedL() panic test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0007::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// This test fails if running a debug StdSourcesAndSinks dll.
	// So only perform the test on target.

	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create source descriptor.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes);
	CleanupDeletePushL(source);

	// Test BufferEmptiedL
	source->BufferEmptiedL(validBuffer);

	CleanupStack::PopAndDestroy(2, validBuffer); // source, validBuffer

	CleanupStack::PopAndDestroy(dataDes);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0008 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0008::CTestStep_MMF_SRSSNK_U_0008()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0008");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0008 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0008::~CTestStep_MMF_SRSSNK_U_0008()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0008
 *
 * Functional test of descriptor source.
 * ReadBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0008::DoTestStepL()
	{
	__MM_HEAP_MARK;
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Set it's length.
	dataPtr.SetLength(KTestDataSize);

	TTime seedTime;
	seedTime.UniversalTime();
	TInt64 seed = seedTime.Int64();
	for (TInt i = 0 ; i<KTestDataSize ; i++)
		dataPtr[i] = TUint8(Math::Rand(seed));

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create zero length buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create source descriptor.
	CMMFClip* source = STATIC_CAST(CMMFClip*, MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes));
	CleanupStack::PushL(source);

	// Create test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test ReadBufferL (these tests should fail)
	TClipReadBufferTestStep testStep(source, testBuffer, 0, dummySink, dataPtr);
	DoTest(testStep, _L("Descriptor source ReadBufferL"), KErrNotSupported);

	TClipReadBuffer2TestStep testStep2(source, testBuffer, 0, dummySink, dataPtr, KTestBufferSize);
	DoTest(testStep2, _L("Descriptor source ReadBufferL"), KErrNotSupported);

	TClipReadBuffer3TestStep testStep3(source, testBuffer, 0, dataPtr, KTestBufferSize);
	DoTest(testStep3, _L("Descriptor source ReadBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test reading from the beginning, middle, and end of the descriptor.
	TClipReadBufferTestStep testStep4(source, validBuffer, 0, dummySink, dataPtr, KTestReadSize);
	DoTest(testStep4, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep4.SetPosition(1);
	DoTest(testStep4, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize/2);
	DoTest(testStep4, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep4, _L("Descriptor source ReadBufferL"), KErrNone);

	TClipReadBuffer4TestStep testStep5(source, validBuffer, KTestDataSize - 10, dummySink, dataPtr, KTestReadSize);
	testStep5.SetExpectedReadLength(10);
	DoTest(testStep5, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep5.SetPosition(KTestDataSize);
	testStep5.SetExpectedReadLength(0);
	DoTest(testStep5, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep5.SetPosition(KTestDataSize + 2);
	DoTest(testStep5, _L("Descriptor source ReadBufferL"), KErrNone);

	// Attempt to read nothing.
	TClipReadBufferTestStep testStep6(source, validBuffer, 0, dummySink, dataPtr);
	DoTest(testStep6, _L("Descriptor source ReadBufferL"), KErrNone);

	// Attempt to read more data than can fit in the buffer.
	testStep6.SetReadLength(KTestOverReadSize);
	DoTest(testStep6, _L("Descriptor source ReadBufferL"), KErrOverflow);

	// Attempt to read into a zero length buffer.
	TClipReadBufferTestStep testStep7(source, zeroBuffer, 0, dummySink, dataPtr);
	DoTest(testStep7, _L("Descriptor source ReadBufferL"), KErrNone);

	// Attempt to read more data than present in the source.
	TClipReadBuffer4TestStep testStep8(source, bigBuffer, 0, dummySink, dataPtr, KTestOverDataSize);
	testStep8.SetExpectedReadLength(KTestDataSize);
	DoTest(testStep8, _L("File source ReadBufferL"), KErrNone);

	// Repeat reads without supplying the length to read.
	TClipReadBuffer2TestStep testStep9(source, validBuffer, 0, dummySink, dataPtr, KTestBufferSize);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(1);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize - 10);
	testStep9.SetReadLength(10);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize);
	testStep9.SetReadLength(0);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize + 2);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrNone);

	TClipReadBuffer2TestStep testStep10(source, zeroBuffer, 0, dummySink, dataPtr, 0);
	DoTest(testStep10, _L("Descriptor source ReadBufferL"), KErrNone);

	TClipReadBuffer2TestStep testStep11(source, bigBuffer, 0, dummySink, dataPtr, KTestDataSize);
	DoTest(testStep11, _L("Descriptor source ReadBufferL"), KErrNone);

	// Repeat reads without supplying the length, or a sink.
	TClipReadBuffer3TestStep testStep12(source, validBuffer, 0, dataPtr, KTestBufferSize);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(1);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(KTestDataSize/2);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(KTestDataSize - 10);
	testStep12.SetReadLength(10);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(KTestDataSize);
	testStep12.SetReadLength(0);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	testStep12.SetPosition(KTestDataSize + 2);
	DoTest(testStep12, _L("Descriptor source ReadBufferL"), KErrNone);

	TClipReadBuffer3TestStep testStep13(source, zeroBuffer, 0, dataPtr, 0);
	DoTest(testStep13, _L("Descriptor source ReadBufferL"), KErrNone);

	TClipReadBuffer3TestStep testStep14(source, bigBuffer, 0, dataPtr, KTestDataSize);
	DoTest(testStep14, _L("Descriptor source ReadBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(5, zeroBuffer); // source, dummySink, validBuffer, bigBuffer, zeroBuffer

	CleanupStack::PopAndDestroy(dataDes); // dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0009 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0009::CTestStep_MMF_SRSSNK_U_0009()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0009");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0009 destructor
 *
 * @xxxx
 *
 */

CTestStep_MMF_SRSSNK_U_0009::~CTestStep_MMF_SRSSNK_U_0009()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0009
 *
 * Functional test of descriptor source.
 * Source custom commands test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0009::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;

	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSource(KUidMmfDescriptorSource, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSource, dummyArgs, dummyArgs);
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Descriptor source SourceCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(); // dummyController

	CleanupStack::PopAndDestroy(dataDes);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0010 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0010::CTestStep_MMF_SRSSNK_U_0010()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0010");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0010 destructor
 *
 * @xxxx
 *
 */

CTestStep_MMF_SRSSNK_U_0010::~CTestStep_MMF_SRSSNK_U_0010()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0010
 *
 * Functional test of descriptor sink.
 * Misc. functions test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0010::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create sink buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create a dummy sink.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create desciptor sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfDescriptorSink, configDes);
	CleanupDeletePushL(sink);

	// Test create sink buffer.
	TCreateSinkBufferTestStep0 testStep(sink, KUidMmfDescriptorBuffer, ETrue, NULL);
	DoTest(testStep, _L("Descriptor sink CreateSinkBuffer"), KErrNotSupported);

	// Test default base class fns. (These should all do nothing)
	TSinkThreadLogonTestStep testStep2(sink, dummySource);
	DoTest(testStep2, _L("Descriptor sink SinkThreadLogon"), KErrNone);

	TNegotiateTestStep testStep3(sink, dummySource);
	DoTest(testStep3, _L("Descriptor sink NegotiateL"), KErrNone);

	sink->SinkPrimeL();
	sink->SinkPlayL();
	sink->SinkPauseL();
	sink->SinkStopL();

	TMMFPrioritySettings prioritySettings;
	sink->SetSinkPrioritySettings(prioritySettings);

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(2, dataDes); // dummySource, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0011 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0011::CTestStep_MMF_SRSSNK_U_0011()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0011");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0011 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0011::~CTestStep_MMF_SRSSNK_U_0011()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0011
 *
 * Functional test of descriptor sink.
 * EmptyBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0011::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());
	TUint8* baseDataPtr = CONST_CAST(TUint8*, dataPtr.Ptr());

	HBufC8* copyOfDataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 copyOfDataPtr(copyOfDataDes->Des());

	// Randomise the data.
	TTime seedTime;
	seedTime.UniversalTime();
	TInt64 seed = seedTime.Int64();
	for (TInt i = 0 ; i<KTestDataSize ; i++)
		baseDataPtr[i] = TUint8(Math::Rand(seed));
	Mem::Copy(CONST_CAST(TUint8*, copyOfDataPtr.Ptr()), baseDataPtr, KTestDataSize);

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create zero length buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Set the length to write.
	bigBuffer->Data().SetLength(KTestOverDataSize);

	// Create a valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Set the length to write.
	validBuffer->Data().SetLength(KTestWriteSize);

	// Create descriptor sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfDescriptorSink, configDes);
	CleanupDeletePushL(sink);

	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test invalid EmptyBufferL.
	TEmptyBufferTestStep testStep(sink, testBuffer, dummySource, dataPtr, copyOfDataPtr);
	DoTest(testStep, _L("Descriptor sink EmptyBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test valid empty.
	TEmptyBufferTestStep testStep2(sink, validBuffer, dummySource, dataPtr, copyOfDataPtr);
	DoTestStep(testStep2, _L("Descriptor sink EmptyBufferL"), KErrNone);

	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	testStep2.SetPosition(0);
	DoTestStepWithAllocFail(testStep2, _L("Descriptor sink EmptyBufferL"), KErrNone);

	// Test complete empty.
	TEmptyBufferTestStep testStep3(sink, validBuffer, dummySource, dataPtr, copyOfDataPtr);

	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	const TUint8* sinkPtr = baseDataPtr;
	TInt bufferSize;
	do
		{
		bufferSize = Min(KTestWriteSize, KTestDataSize - (sinkPtr-baseDataPtr));
		validBuffer->Data().SetLength(bufferSize);

		testStep3.SetWriteLength(bufferSize);
		DoTestStep(testStep3, _L("Descriptor sink EmptyBufferL"), KErrNone);

		sinkPtr += bufferSize;
		} while (bufferSize == KTestWriteSize);

	// Test complete empty with alloc fail.
	TEmptyBufferTestStep testStep4(sink, validBuffer, dummySource, dataPtr, copyOfDataPtr);
	validBuffer->Data().SetLength(KTestWriteSize);

	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	sinkPtr = baseDataPtr;
	do
		{
		bufferSize = Min(KTestWriteSize, KTestDataSize - (sinkPtr-baseDataPtr));
		validBuffer->Data().SetLength(bufferSize);

		testStep4.SetWriteLength(bufferSize);
		DoTestStepWithAllocFail(testStep4, _L("Descriptor sink EmptyBufferL"), KErrNone);

		bufferSize = validBuffer->BufferSize();
		sinkPtr += bufferSize;
		} while (bufferSize == KTestWriteSize);

	// Test additional empty request.
	TEmptyBufferTestStep testStep5(sink, validBuffer, dummySource, dataPtr, copyOfDataPtr);

	testStep5.SetWriteLength(0);
	DoTest(testStep5, _L("Descriptor sink EmptyBufferL"), KErrNone);

	// Test zero length write.
	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	TEmptyBufferTestStep testStep6(sink, zeroBuffer, dummySource, dataPtr, copyOfDataPtr);
	DoTest(testStep6, _L("Descriptor sink EmptyBufferL"), KErrNone);

	// Test over size write.
	TEmptyBufferTestStep testStep7(sink, bigBuffer, dummySource, dataPtr, copyOfDataPtr);
	testStep7.SetWriteLength(KTestDataSize);
	DoTestStep(testStep7, _L("Descriptor sink EmptyBufferL"), KErrNone /* Should probably be KErrOverflow */);

	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	testStep7.SetPosition(0);
	DoTestStepWithAllocFail(testStep7, _L("Descriptor sink EmptyBufferL"), KErrNone /* Should probably be KErrOverflow */);

	CleanupStack::PopAndDestroy(5, zeroBuffer); // sink, dummySource, validBuffer, bigBuffer, zeroBuffer

	CleanupStack::PopAndDestroy(2, dataDes); // copyOfDataDes, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0012 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0012::CTestStep_MMF_SRSSNK_U_0012()
: CTestStep_MMF_SRSSNK(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0012");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0012 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0012::~CTestStep_MMF_SRSSNK_U_0012()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0012
 *
 * Functional test of descriptor sink.
 * BufferFilledL() panic test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0012::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// This test fails if running a debug StdSourcesAndSinks dll.
	// So only perform the test on target.

	// Create sink buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create descriptor sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfDescriptorSink, configDes);
	CleanupDeletePushL(sink);

	// Test BufferFilledL
	sink->BufferFilledL(validBuffer);

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(2, dataDes); // validBuffer, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0013 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0013::CTestStep_MMF_SRSSNK_U_0013()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0013");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0013 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0013::~CTestStep_MMF_SRSSNK_U_0013()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0013
 *
 * Functional test of descriptor sink.
 * WriteBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0013::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());
	TUint8* baseDataPtr = CONST_CAST(TUint8*, dataPtr.Ptr());

	HBufC8* copyOfDataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 copyOfDataPtr(copyOfDataDes->Des());

	// Randomise the data.
	TTime seedTime;
	seedTime.UniversalTime();
	TInt64 seed = seedTime.Int64();
	for (TInt i = 0 ; i<KTestDataSize ; i++)
		baseDataPtr[i] = TUint8(Math::Rand(seed));
	Mem::Copy(CONST_CAST(TUint8*, copyOfDataPtr.Ptr()), baseDataPtr, KTestDataSize);

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create valid Buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Set the length to write.
	validBuffer->Data().SetLength(KTestWriteSize);

	// Create dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create sink descriptor.
	CMMFClip* sink = STATIC_CAST(CMMFClip*, MDataSink::NewSinkL(KUidMmfDescriptorSink, configDes));
	CleanupStack::PushL(sink);

	// Create test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test WriteBufferL (these tests should fail)
	TDescriptorClipWriteBufferTestStep testStep(sink, testBuffer, 0, dummySource, dataPtr, copyOfDataPtr);
	DoTest(testStep, _L("Descriptor sink WriteBufferL"), KErrNotSupported);

	TDescriptorClipWriteBuffer2TestStep testStep2(sink, testBuffer, 0, dummySource, dataPtr, copyOfDataPtr, KTestBufferSize);
	DoTest(testStep2, _L("Descriptor sink WriteBufferL"), KErrNotSupported);

	TDescriptorClipWriteBuffer3TestStep testStep3(sink, testBuffer, 0, dataPtr, copyOfDataPtr, KTestBufferSize);
	DoTest(testStep3, _L("Descriptor sink WriteBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test writing to the beginning, middle, and end ofthe descriptor.
	TDescriptorClipWriteBufferTestStep testStep4(sink, validBuffer, 0, dummySource, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(1);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize/2);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize-10);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep4.SetWriteLength(11);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep4.SetWriteLength(10);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep4.SetWriteLength(0);
	DoTest(testStep4, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep4.SetPosition(KTestDataSize + 2);
	DoTest(testStep4, _L("Descriptor source WriteBufferL"), KErrArgument);

	// Attempt to write nothing.
	TDescriptorClipWriteBufferTestStep testStep5(sink, validBuffer, 0, dummySource, dataPtr, copyOfDataPtr);
	DoTest(testStep5, _L("Descriptor sink WriteBufferL"), KErrNone);

	// Attempt to write more data than is actually in the buffer.
	testStep5.SetWriteLength(KTestOverWriteSize);
	DoTest(testStep5, _L("Descriptor sink WriteBufferL"), KErrArgument);

	// Repeat writes with full length descriptor.
	dataPtr.SetLength(KTestDataSize);
	copyOfDataPtr.SetLength(KTestDataSize);

	TDescriptorClipWriteBufferTestStep testStep6(sink, validBuffer, 0, dummySource, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(1);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(KTestDataSize/2);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(KTestDataSize - 10);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep6.SetWriteLength(11);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep6.SetWriteLength(10);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(KTestDataSize);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	testStep6.SetWriteLength(0);
	DoTest(testStep6, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep6.SetPosition(KTestDataSize + 2);
	DoTest(testStep6, _L("Descriptor source WriteBufferL"), KErrArgument);

	// Reset descriptor.
	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);

	// Repeat writes without supplying the length to write.
	TDescriptorClipWriteBuffer2TestStep testStep7(sink, validBuffer, 0, dummySource, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(1);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(KTestDataSize/2);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(KTestDataSize - 10);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(11);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(10);
	testStep7.SetWriteLength(10);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(KTestDataSize);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(0);
	testStep7.SetWriteLength(0);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep7.SetPosition(KTestDataSize + 2);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrArgument);

	// Attempt to write nothing.
	testStep7.SetPosition(0);
	DoTest(testStep7, _L("Descriptor sink WriteBufferL"), KErrNone);

	// Repeat writes with full length descriptor.
	dataPtr.SetLength(KTestDataSize);
	copyOfDataPtr.SetLength(KTestDataSize);
	validBuffer->Data().SetLength(KTestWriteSize);

	TDescriptorClipWriteBuffer2TestStep testStep8(sink, validBuffer, 0, dummySource, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(1);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(KTestDataSize/2);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(KTestDataSize - 10);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(11);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(10);
	testStep8.SetWriteLength(10);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(KTestDataSize);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(0);
	testStep8.SetWriteLength(0);
	DoTest(testStep8, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep8.SetPosition(KTestDataSize + 2);
	DoTest(testStep8, _L("Descriptor source WriteBufferL"), KErrArgument);

	// Reset descriptor.
	dataPtr.SetLength(0);
	copyOfDataPtr.SetLength(0);
	validBuffer->Data().SetLength(KTestWriteSize);

	// Repeat writes without supplying the length, or a sink.
	TDescriptorClipWriteBuffer3TestStep testStep9(sink, validBuffer, 0, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep9.SetPosition(1);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize - 10);
	DoTest(testStep9, _L("Descriptor source ReadBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(11);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(10);
	testStep9.SetWriteLength(10);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	validBuffer->Data().SetLength(0);
	testStep9.SetPosition(KTestDataSize);
	testStep9.SetWriteLength(0);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep9.SetPosition(KTestDataSize + 2);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrArgument);

	// Attempt to write nothing.
	testStep9.SetPosition(0);
	DoTest(testStep9, _L("Descriptor sink WriteBufferL"), KErrNone);

	// Repeat writes with full length descriptor.
	dataPtr.SetLength(KTestDataSize);
	copyOfDataPtr.SetLength(KTestDataSize);
	validBuffer->Data().SetLength(KTestWriteSize);

	TDescriptorClipWriteBuffer3TestStep testStep10(sink, validBuffer, 0, dataPtr, copyOfDataPtr, KTestWriteSize);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(1);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(KTestDataSize/2);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(KTestDataSize/2 + 1);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(KTestDataSize - 10);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(11);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(10);
	testStep10.SetWriteLength(10);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(KTestDataSize);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrOverflow);

	validBuffer->Data().SetLength(0);
	testStep10.SetWriteLength(0);
	DoTest(testStep10, _L("Descriptor sink WriteBufferL"), KErrNone);

	testStep10.SetPosition(KTestDataSize + 2);
	DoTest(testStep10, _L("Descriptor source WriteBufferL"), KErrArgument);

	CleanupStack::PopAndDestroy(3, validBuffer); // sink, dummySource, validBuffer

	CleanupStack::PopAndDestroy(2, dataDes); // copyOfDataDes, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0014 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0014::CTestStep_MMF_SRSSNK_U_0014()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0014");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0014 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0014::~CTestStep_MMF_SRSSNK_U_0014()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0014
 *
 * Functional test of descriptor sink.
 * SinkCustomCommand() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0014::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create sink buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSink(KUidMmfDescriptorSink, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSink, dummyArgs, dummyArgs);
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Descriptor sink SinkCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(2, dataDes); // dummyController, dataDes.

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0015 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0015::CTestStep_MMF_SRSSNK_U_0015()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0015");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0015 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0015::~CTestStep_MMF_SRSSNK_U_0015()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0015
 *
 * CMMFDescriptor
 * Request size / SetLastBuffer test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0015::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Set it's length.
	dataPtr.SetLength(KTestDataSize);

	TTime seedTime;
	seedTime.UniversalTime();
	TInt64 seed = seedTime.Int64();
	for (TInt i = 0 ; i<KTestDataSize ; i++)
		dataPtr[i] = TUint8(Math::Rand(seed));

	// Initialise config data.
	TMMFDescriptorConfig configDes;
	configDes().iDes = &dataPtr;
	configDes().iDesThreadId = RThread().Id();

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Test fill last buffer
	TFillLastBufferTestStep testStep(KUidMmfDescriptorSource, configDes, dummySink, dataPtr);
	DoTest(testStep, _L("Descriptor source: Fill last buffer"), KErrNone);

	// Create a source.
	CMMFClip* source = STATIC_CAST(CMMFClip*, MDataSource::NewSourceL(KUidMmfDescriptorSource, configDes));
	CleanupStack::PushL(source);

	// Test read last buffer (API version 1)
	TReadLastBufferTestStep testStep2(source, dummySink, dataPtr);
	DoTest(testStep2, _L("Descriptor source: Read last buffer"), KErrNone);

	TReadLastBuffer2TestStep testStep3(source, dummySink, dataPtr);
	DoTest(testStep3, _L("Descriptor source: Read last buffer"), KErrNone);

	TReadLastBuffer3TestStep testStep4(source, dataPtr);
	DoTest(testStep4, _L("Descriptor source: Read last buffer"), KErrNone);

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(2, dataDes); // dummySink, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0101 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0101::CTestStep_MMF_SRSSNK_U_0101()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0101");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0101 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0101::~CTestStep_MMF_SRSSNK_U_0101()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0101
 *
 * Test creation/destruction of file source.
 *
 * @xxxx
 */
_LIT(KTest101CopyFileName, "c:\\mm\\mmf\\testfiles\\srssnk\\CopyOfMMFTestFile1.dat");
TVerdict CTestStep_MMF_SRSSNK_U_0101::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Get file info.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	// Get the size of the test file.
	RFile testFile;
	TFileName testFileName(KTest101FileName);
	User::LeaveIfError(testFile.Open(testFsSession, testFileName, EFileRead));
	CleanupClosePushL(testFile);

	// Force delete of the copied file - test will leave otherwise
	TInt err = testFsSession.Delete(KTest101CopyFileName);
	if (err == KErrAccessDenied)
		{
		// It's read-only, remove the read-only attribute
		err = testFsSession.SetAtt(KTest101CopyFileName, 0, KEntryAttReadOnly);
		if (err == KErrNone)
			{
			// Try deleting again
			err = testFsSession.Delete(KTest101CopyFileName);
			}
		User::LeaveIfError(err);
		}

	TInt fileSize;
	User::LeaveIfError(testFile.Size(fileSize));
	CleanupStack::PopAndDestroy(); // testFile

	// Copy the file. (Remaining tests work with the copy)
	CopyFileL(testFsSession, testFileName, KTest101CopyFileName);

	// Get the free space on the drive.
	testFileName = KTest101CopyFileName;
	TParsePtr parser(testFileName);
	TDriveUnit drive(parser.Drive());
	TVolumeInfo volInfo;
	User::LeaveIfError(testFsSession.Volume(volInfo, drive));

	TInt64 freeSpace = volInfo.iFree;


	// Set the read only attribute
	err = testFsSession.SetAtt(testFileName, KEntryAttReadOnly, 0);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error setting read-only attribute (%d)"), err);
		iTestResult = EFail;
		}

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = testFileName;

	// Test valid uid and configuration descriptor.
	TNewSourceTestStep testStep(KUidMmfFileSource, configDes);
	DoFileTest(testStep, _L("Construct file source"), KErrNone);

	// Test that the source has been initialised correctly.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(source);

	// Test that the file has been opened correctly.
	source->SourcePrimeL();
	RFile& file = (STATIC_CAST(CMMFFile*, source))->FileL();

	// Read some bytes from the file. (Check it is as we would expect)
	TBuf8<256> sourceBytes;
	file.Read(sourceBytes);
	for (TInt i = 0 ; i<256 ; i++)
		{
		if (sourceBytes[i] != i)
			{
			INFO_PRINTF3(_L("Construct file source: %d, actually got %d"), sourceBytes[i], i);
			iTestResult = EFail;
			}
		}

	// Test that the source is initialised correctly.
	TSourceTypeTestStep testStep2(source, KUidMmfFileSource, KMMFFourCCCodeNULL, EFalse);
	DoTest(testStep2, _L("File source type"), KErrNone);

	TCanCreateSourceBufferTestStep testStep3(source, EFalse);
	DoTest(testStep3, _L("File CanCreateSourceBuffer"), KErrNone);

	TSourceSampleConvertTestStep testStep4(source, EFalse);
	DoTest(testStep4, _L("File source SourceSampleConvert"), KErrNone);

	// Test that the clip is initialised correctly.
	CMMFClip *sourceClip = STATIC_CAST(CMMFClip*, source);

	// do a bytes free test
	TClipBytesFreeTestStep testStep5(sourceClip, freeSpace);
	DoFileTest(testStep5, _L("File source BytesFree"), KErrNone);

	TClipSizeTestStep testStep6(sourceClip, fileSize);
	DoFileTest(testStep6, _L("File source Size"), KErrNone);

	TClipSetSizeTestStep testStep7(sourceClip, ETrue);
	DoFileTest(testStep7, _L("File source SetSize"), KErrAccessDenied);

	TClipDeleteTestStep testStep8(sourceClip);
	DoFileTest(testStep8, _L("File source Delete"), KErrAccessDenied);

	CleanupStack::PopAndDestroy(source);

	// Remove the read only attribute.
	err = testFsSession.SetAtt(testFileName, 0, KEntryAttReadOnly);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	sourceClip = STATIC_CAST(CMMFClip*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
	CleanupStack::PushL(sourceClip);

	sourceClip->SourcePrimeL();
	TClipSetSizeTestStep testStep9(sourceClip, EFalse);
	DoFileTest(testStep9, _L("File source SetSize"), KErrAccessDenied);

	CleanupStack::PopAndDestroy(sourceClip);

/* Note - cannot set the size of a source
	// Check the size.
	User::LeaveIfError(testFile.Open(testFsSession, testFileName, EFileRead));

	TInt size;
	err = testFile.Size(size);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}
	testFile.Close();

	if (size != fileSize+6) // fileSize+6 is the result from SetSize test.
		{
		INFO_PRINTF3(_L("File source Size: Expecting %d, actually got %d"), fileSize+6, size);
		iTestResult = EFail;
		}
*/

	CMMFFile* sourceFile = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
	CleanupStack::PushL(sourceFile);

	const TDesC& fileName = sourceFile->FileName();
	if (fileName.Compare(parser.Name()))
		{
		INFO_PRINTF3(_L("File source FileName: Expecting %S, actually got %S"), parser.Name().Ptr(), fileName.Ptr());
		iTestResult = EFail;
		}

	const TDesC& extName = sourceFile->Extension();
	if (extName.Compare(parser.Ext()))
		{
		INFO_PRINTF3(_L("File source FileName: Expecting %S, actually got %S"), parser.Ext().Ptr(), extName.Ptr());
		iTestResult = EFail;
		}

	DoTestStep(testStep8, _L("File source Delete"), KErrNone);

	// Check that the file has actually been deleted.
	err = testFile.Open(testFsSession, testFileName, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File source Delete: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	// Copy the file (again).
	CopyFileL(testFsSession, KTest101FileName, KTest101CopyFileName);

	// Remove the read only attribute.
	err = testFsSession.SetAtt(testFileName, 0, KEntryAttReadOnly);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(sourceFile);

	sourceFile = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
	CleanupStack::PushL(sourceFile);

	TClipDeleteTestStep testStep10(sourceFile);
	DoTestStepWithAllocFail(testStep10, _L("File source Delete"), KErrNone);

	// Check that the file has actually been deleted.
	err = testFile.Open(testFsSession, testFileName, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File source Delete: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	// Copy the file (again).
	CopyFileL(testFsSession, KTest101FileName, KTest101CopyFileName);

	// Remove the read only attribute.
	err = testFsSession.SetAtt(testFileName, 0, KEntryAttReadOnly);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(sourceFile);

	if (iFileFailureTestingEnabled)
		{
		sourceFile = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
		CleanupStack::PushL(sourceFile);

		TClipDeleteTestStep testStep11(sourceFile);
		DoTestStepWithFileFail(testStep11, _L("File source Delete"), KErrNone);

		// Check that the file has actually been deleted.
		err = testFile.Open(testFsSession, testFileName, EFileRead);
		if (err != KErrNotFound)
			{
			testFile.Close();

			INFO_PRINTF2(_L("File source Delete: Expecting KErrNotFound, actually got %d"), err);
			iTestResult = EFail;
			}

		// Copy the file (again).
		CopyFileL(testFsSession, KTest101FileName, KTest101CopyFileName);

		// Remove the read only attribute.
		err = testFsSession.SetAtt(testFileName, 0, KEntryAttReadOnly);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
			iTestResult = EFail;
			}

		CleanupStack::PopAndDestroy(sourceFile);

/*
		// DoTestStepWithAllocAndFileFail test is no longer run.
		// Code kept for reference purposes.
		sourceFile = STATIC_CAST(CMMFFile*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
		CleanupStack::PushL(sourceFile);

		TClipDeleteTestStep testStep12(sourceFile);
		DoTestStepWithAllocAndFileFail(testStep12, _L("File source Delete"), KErrNone);

		// Check that the file has actually been deleted.
		err = testFile.Open(testFsSession, testFileName, EFileRead);
		if (err != KErrNotFound)
			{
			testFile.Close();

			INFO_PRINTF2(_L("File source Delete: Expecting KErrNotFound, actually got %d"), err);
			iTestResult = EFail;
			}

		CleanupStack::PopAndDestroy(sourceFile);
*/
		}

	CleanupStack::PopAndDestroy(); // testFsSession

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0102 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0102::CTestStep_MMF_SRSSNK_U_0102()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0102");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0102 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0102::~CTestStep_MMF_SRSSNK_U_0102()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0102
 *
 * Attempt to create file source using invalid config data.
 *
 * @xxxx
 */
_LIT(KTest102FileName1, "c:\\DefaultPathSource.dat"); // Valid but no path.
_LIT(KTest102FileName2, ""); // Definitely not valid.
TVerdict CTestStep_MMF_SRSSNK_U_0102::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest102FileName1;

	TNewSourceTestStep testStep(KUidMmfFileSource, configDes);
	DoFileTest(testStep, _L("Construct file source 102.1"), KErrNone);

	// Verify that the file has not been created.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	RFile testFile;
	TInt err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();
		User::LeaveIfError(testFsSession.Delete(configDes().iPath));

		INFO_PRINTF2(_L("File source create: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	TNewFileSourceTestStep testStep2(KUidMmfFileSource, configDes);
	DoFileTest(testStep2, _L("Construct file source 102.2"), KErrNotFound);

	// Verify that the file has not been created.
	err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();
		User::LeaveIfError(testFsSession.Delete(configDes().iPath));

		INFO_PRINTF2(_L("File source create: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	if (iFileFailureTestingEnabled)
		{
		// Repeat test with file failure only. (Checks for file failure on creation)
		DoTestStepWithFileFail(testStep2, _L("Construct file source 102.3"), KErrNotFound);

		// Verify that the file has not been created.
		err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
		if (err != KErrNotFound)
			{
			testFile.Close();
			User::LeaveIfError(testFsSession.Delete(configDes().iPath));

			INFO_PRINTF2(_L("File source create: Expecting KErrNotFound, actually got %d"), err);
			iTestResult = EFail;
			}
		}

	configDes().iPath = KTest102FileName2;
	TNewSourceTestStep testStep5(KUidMmfFileSource, configDes);
	DoFileTest(testStep5, _L("Construct file source 102.4"), KErrBadName);

	CleanupStack::PopAndDestroy(); // testFsSession

	__MM_HEAP_MARKEND;

	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0103 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0103::CTestStep_MMF_SRSSNK_U_0103()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0103");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0103 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0103::~CTestStep_MMF_SRSSNK_U_0103()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0103
 *
 * Test creation/destruction of file sink.
 *
 * @xxxx
 */
_LIT(KTest103FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile4.dat");
TVerdict CTestStep_MMF_SRSSNK_U_0103::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Get file info.
	TInt64 freeSpace = 0;

	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	// Get the free space on the drive.
	TFileName testFileName(KTest103FileName);
	TParsePtr parser(testFileName);
	TDriveUnit drive(parser.Drive());
	TVolumeInfo volInfo;
	if (testFsSession.Volume(volInfo, drive) == KErrNone)
		freeSpace = volInfo.iFree;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = testFileName;

	// Test valid uid and configuration descriptor.
	TNewSinkTestStep testStep(KUidMmfFileSink, configDes);
	DoFileTest(testStep, _L("Construst file sink 1"), KErrNone);

	// Test that the sink has been initialised correctly.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfFileSink, configDes);
	CleanupDeletePushL(sink);

	// Test that the file has been opened correctly.
	sink->SinkPrimeL();
	RFile& file = (STATIC_CAST(CMMFFile*, sink))->FileL();

	// Attempt to read some bytes from the file. (Check it is as we would expect)
	TBuf8<256> sourceBytes;
	TInt err = file.Read(sourceBytes);
	if ((err != KErrNone) || (sourceBytes.Length() != 0))
		{
		INFO_PRINTF3(_L("Construct file sink: Expecting KErrNone + zero length file, actually got %d + %d bytes"), err, sourceBytes.Length());
		iTestResult = EFail;
		}

	// Test that the file is writable.
	TUint att;
	err = testFsSession.Att(testFileName, att);
	if ((err != KErrNone) || (att & KEntryAttReadOnly))
		{
		INFO_PRINTF3(_L("Create file sink : Expecting KErrNone + KEntryAttReadOnly, actually got %d, %x"), err, att);
		iTestResult = EFail;
		}

	// Test that the sink is initialised correctly.
	TSinkTypeTestStep testStep2(sink, KUidMmfFileSink, KMMFFourCCCodeNULL, EFalse);
	DoTest(testStep2, _L("Descriptor sink type 2"), KErrNone);

 	TCanCreateSinkBufferTestStep testStep3(sink, EFalse);
	DoTest(testStep3, _L("Descriptor sink CanCreateSinkBuffer 3"), KErrNone);

	// Test that the clip is initialised correctly.
	CMMFClip *sinkClip = STATIC_CAST(CMMFClip*, sink);

	// do a bytes free test
	TClipBytesFreeTestStep testStep4(sinkClip, freeSpace);
	DoFileTest(testStep4, _L("Descriptor sink BytesFree 4"), KErrNone);

	TClipSizeTestStep testStep5(sinkClip, 0);
	DoFileTest(testStep5, _L("Descriptor sink Size 5"), KErrNone);

	TFileSinkSetSizeTestStep testStep6(sinkClip, ETrue);
	DoFileTest(testStep6, _L("Descriptor sink SetSize 6"), KErrNone);

	CleanupStack::PopAndDestroy(sink);

	// Set the read only attribute.
	err = testFsSession.SetAtt(testFileName, KEntryAttReadOnly, 0);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source SetSize: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	sinkClip = STATIC_CAST(CMMFClip*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sinkClip);

	sinkClip->SinkPrimeL();
	TFileSinkSetSizeTestStep testStep7(sinkClip, ETrue);
	DoFileTest(testStep7, _L("Descriptor sink SetSize 7"), KErrAccessDenied);

	TClipDeleteTestStep testStep8(sinkClip);
	DoFileTest(testStep8, _L("Descriptor sink Delete 8"), KErrAccessDenied);

	CleanupStack::PopAndDestroy(sinkClip);

	RFile testFile;
	// Check the filename.
	CMMFFile* sinkFile = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sinkFile);

	const TDesC& fileName = sinkFile->FileName();
	if (fileName.Compare(parser.Name()))
		{
		INFO_PRINTF3(_L("File sink FileName: Expecting %S, actually got %S"), parser.Name().Ptr(), fileName.Ptr());
		iTestResult = EFail;
		}

	const TDesC& extName = sinkFile->Extension();
	if (extName.Compare(parser.Ext()))
		{
		INFO_PRINTF3(_L("File sink FileName: Expecting %S, actually got %S"), parser.Ext().Ptr(), extName.Ptr());
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(sinkFile);

	// Delete the file.
	// Remove the read only attribute.
	err = testFsSession.SetAtt(testFileName, 0, KEntryAttReadOnly);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source Delete: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	sinkFile = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sinkFile);

	TClipDeleteTestStep testStep9(sinkFile);
	DoTestStep(testStep9, _L("File sink Delete 9"), KErrNone);

	CleanupStack::PopAndDestroy(sinkFile);

	// Check that the file has actually been deleted.
	err = testFile.Open(testFsSession, testFileName, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File sink Delete: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	// Re-create the file and delete it again.
	sinkFile = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sinkFile);
	sinkFile->SinkPrimeL();

	TClipDeleteTestStep testStep10(sinkClip);
	DoTestStepWithAllocFail(testStep10, _L("File sink Delete 10"), KErrNone);

	CleanupStack::PopAndDestroy(sinkFile);

	// Check that the file has actually been deleted.
	err = testFile.Open(testFsSession, testFileName, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File sink Delete: Expecting KErrNotFound, actually got %d"), err);
		iTestResult = EFail;
		}

	if (iFileFailureTestingEnabled)
		{

		// Re-create the file and delete it again.
		sinkFile = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
		CleanupStack::PushL(sinkFile);
		sinkFile->SinkPrimeL();

		TClipDeleteTestStep testStep11(sinkClip);
		DoTestStepWithFileFail(testStep11, _L("File sink Delete 11"), KErrNone);

		CleanupStack::PopAndDestroy(sinkFile);

		// Check that the file has actually been deleted.
		err = testFile.Open(testFsSession, testFileName, EFileRead);
		if (err != KErrNotFound)
			{
			testFile.Close();

			INFO_PRINTF2(_L("File sink Delete: Expecting KErrNotFound, actually got %d"), err);
			iTestResult = EFail;
			}

/*
		// DoTestStepWithAllocAndFileFail test is no longer run.
		// Code kept for reference purposes.

		// Re-create the file and delete it again.
		sinkFile = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
		CleanupStack::PushL(sinkFile);
		sinkFile->SinkPrimeL();

		TClipDeleteTestStep testStep12(sinkClip);
		DoTestStepWithAllocAndFileFail(testStep12, _L("File sink Delete"), KErrNone);

		CleanupStack::PopAndDestroy(sinkFile);

		// Check that the file has actually been deleted.
		err = testFile.Open(testFsSession, testFileName, EFileRead);
		if (err != KErrNotFound)
			{
			testFile.Close();

			INFO_PRINTF2(_L("File sink Delete: Expecting KErrNotFound, actually got %d"), err);
			iTestResult = EFail;
			}
*/
		}
	CleanupStack::PopAndDestroy(); // testFsSession

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0104 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0104::CTestStep_MMF_SRSSNK_U_0104()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0104");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0104 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0104::~CTestStep_MMF_SRSSNK_U_0104()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0104
 *
 * Attempt to create file sink using invalid config data.
 *
 * @xxxx
 */
_LIT(KTest104FileName1, "c:\\DefaultPathSink.dat"); // Valid but no path.
_LIT(KTest104FileName2, "c:\\DefaultPathNoExtSink"); // Valid but no path or ext.
_LIT(KTest104FileName3, ""); // Definitely not valid.
TVerdict CTestStep_MMF_SRSSNK_U_0104::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest104FileName1;

	// Test create the sink.
	TNewSinkTestStep testStep(KUidMmfFileSink, configDes);
	DoFileTest(testStep, _L("Construct file sink 104.1"), KErrNone);

	// Verify that the file has not been created.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	RFile testFile;
	TInt err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File sink create: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	TNewFileSinkTestStep testStep2(KUidMmfFileSink, configDes);
	DoFileTest(testStep2, _L("Construct file sink 104.2"), KErrNone);

	// Verify that the file has been created.
	err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File source create: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}
	testFile.Close();

	// Delete it.
	CMMFFile* sink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sink);
	User::LeaveIfError(sink->Delete());
	CleanupStack::PopAndDestroy(sink);

	if (iFileFailureTestingEnabled)
		{
		// Repeat test with file failure only. (Checks for file failure on creation)
		DoTestStepWithFileFail(testStep2, _L("Construct file sink 104.3"), KErrNone);

		// Delete it, now that we don't need it anymore.
		sink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
		CleanupStack::PushL(sink);
		User::LeaveIfError(sink->Delete());
		CleanupStack::PopAndDestroy(sink);
		}

	configDes().iPath = KTest104FileName2;
	TNewSinkTestStep testStep3(KUidMmfFileSink, configDes);
	DoFileTest(testStep3, _L("Construct file sink 104.4"), KErrNone);

	// Verify that the file has not been created.
	err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNotFound)
		{
		testFile.Close();

		INFO_PRINTF2(_L("File sink create: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}

	TNewFileSinkTestStep testStep4(KUidMmfFileSink, configDes);
	DoFileTest(testStep4, _L("Construct file sink 104.5"), KErrNone);

	// Verify that the file has been created.
	err = testFile.Open(testFsSession, configDes().iPath, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("File sink create: Expecting KErrNone, actually got %d"), err);
		iTestResult = EFail;
		}
	testFile.Close();

	// Delete it.
	sink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupStack::PushL(sink);
	User::LeaveIfError(sink->Delete());
	CleanupStack::PopAndDestroy(sink);

	if (iFileFailureTestingEnabled)
		{
		// Repeat test with file failure only. (Checks for file failure on creation)
		DoTestStepWithFileFail(testStep4, _L("Construct file sink 104.6"), KErrNone);

		// Delete it, now that we don't need it anymore.
		sink = STATIC_CAST(CMMFFile*, MDataSink::NewSinkL(KUidMmfFileSink, configDes));
		CleanupStack::PushL(sink);
		User::LeaveIfError(sink->Delete());
		CleanupStack::PopAndDestroy(sink);
		}

	configDes().iPath = KTest104FileName3;
	TNewSinkTestStep testStep5(KUidMmfFileSink, configDes);
	DoFileTest(testStep5, _L("Construct file sink 104.7"), KErrBadName);

	CleanupStack::PopAndDestroy(); // testFsSession

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSourceMisc constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileSourceMisc::CTestStep_MMF_SRSSNK_FileSourceMisc(TSourceType aSourceType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iSourceType = aSourceType;
	ASSERT(iSourceType == ESourceTypeFile || iSourceType == ESourceTypeFileHandle);
	if (iSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0105");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0120");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSourceMisc destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileSourceMisc::~CTestStep_MMF_SRSSNK_FileSourceMisc()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSourceMisc
 *
 * Functional test of file source.
 * Misc. functions test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileSourceMisc::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configFile;
	configFile().iPath = KTest101FileName;

	TInt fileSize;
	OpenFileL(KTest101FileName, EFileRead);
	User::LeaveIfError(iFile.Size(fileSize));
	CloseFile();
	User::LeaveIfError(iFs.Connect());

	// Create a valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create source.
	MDataSource* source = CreateFileSourceL(iSourceType, iFs, KTest101FileName);
	CleanupDeletePushL(source);

	// Test create source buffer.
	TCreateSourceBuffer1TestStep testStep(iSourceType, dummySink, KUidMmfDescriptorBuffer, ETrue);
	DoTest(testStep, _L("File source CreateSourceBuffer1"), KErrNotSupported);

	TCreateSourceBuffer2TestStep testStep2(iSourceType, dummySink, validBuffer, KUidMmfDescriptorBuffer, ETrue);
	DoTest(testStep2, _L("File source CreateSourceBuffer2"), KErrNotSupported);

	// Test default base class fns. (These should all do nothing)
	TSourceThreadLogonTestStep testStep3(source, dummySink);
	DoFileTest(testStep3, _L("File source SourceThreadLogon"), KErrNone);

	TNegotiateSourceTestStep testStep4(source, dummySink);
	DoTest(testStep4, _L("File source NegotiateSourceL"), KErrNone);

	source->SourcePrimeL();
	source->SourcePlayL();
	source->SourcePauseL();
	source->SourceStopL();

	TMMFPrioritySettings prioritySettings;
	source->SetSourcePrioritySettings(prioritySettings);

	// Test source log on.
	source->SourcePrimeL();
	TFileSourceThreadLogonTestStep testStep5(source, dummySink, fileSize);
	DoFileTest(testStep5, _L("File source SourceThreadLogon"), KErrNone);

	CleanupStack::PopAndDestroy(source); // source

	CleanupStack::PopAndDestroy(2, validBuffer); // dummySink, validBuffer

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileFillBuffer constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileFillBuffer::CTestStep_MMF_SRSSNK_FileFillBuffer(TSourceType aSourceType)
	{
	iSourceType = aSourceType;
	ASSERT(iSourceType == ESourceTypeFile || iSourceType == ESourceTypeFileHandle);
	if (iSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0106");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0121");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileFillBuffer destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileFillBuffer::~CTestStep_MMF_SRSSNK_FileFillBuffer()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileFillBuffer
 *
 * Functional test of descriptor source.
 * FillBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileFillBuffer::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());
	const TUint8* baseDataPtr = dataPtr.Ptr();

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Initialise config data.
	TFileName testFileName(KTest101FileName);

	// Read the data from the file.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	RFile testFile;
	User::LeaveIfError(testFile.Open(testFsSession, testFileName, EFileShareReadersOnly));
	CleanupClosePushL(testFile);

	User::LeaveIfError(testFile.Read(dataPtr));
	CleanupStack::PopAndDestroy(&testFile);

	// Create zero buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create file source.
	MDataSource* source = CreateFileSourceL(iSourceType, testFsSession, KTest101FileName);
	CleanupDeletePushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// Create test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupDeletePushL(testBuffer);

	// Test invalid FillBufferL.
	TFillBufferTestStep testStep(source, testBuffer, dummySink, dataPtr);
	DoFileTest(testStep, _L("File source FillBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test valid FillBufferL.

	TFillBufferTestStep testStep2(source, validBuffer, dummySink, dataPtr, KTestBufferSize);
	testStep2.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep2, _L("File source FillBufferL"), KErrNone, 2);

	CleanupStack::PopAndDestroy(source);

	TFileFillBufferTestStep testStep3(iSourceType, testFileName, validBuffer, dummySink, dataPtr, KTestBufferSize);
	testStep3.SetAsynchronousRead(ETrue);
	DoFileTest(testStep3, _L("File source FillBufferL"), KErrNone);

	// Test complete fill.
	source = CreateFileSourceL(iSourceType, testFsSession, KTest101FileName);
	CleanupDeletePushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TFillBufferTestStep testStep4(source, validBuffer, dummySink, dataPtr, KTestBufferSize);
	testStep4.SetAsynchronousRead(ETrue);

	const TUint8* sourcePtr = baseDataPtr;
	TInt bufferSize;
	do
		{
		testStep4.SetReadLength(Min(KTestBufferSize, KTestDataSize - (sourcePtr-baseDataPtr)));
		DoTestStep(testStep4, _L("File source FillBufferL"), KErrNone, 2);

		bufferSize = validBuffer->BufferSize();
		sourcePtr += bufferSize;
		} while (bufferSize == KTestBufferSize);

	if ((sourcePtr - dataPtr.Ptr()) != KTestDataSize)
		{
		INFO_PRINTF3(_L("File source FillBufferL: Source data ptr, expected %d, got %d"), KTestDataSize, sourcePtr - dataPtr.Ptr());
		iTestResult = EFail;
		}

	// Reset the source.
	source->SourceStopL();
	source->SourcePrimeL();

	if (iFileFailureTestingEnabled)
		{
		// Test complete fill with file fail.
		TFillBufferTestStep testStep5(source, validBuffer, dummySink, dataPtr, KTestBufferSize);
		testStep5.SetAsynchronousRead(ETrue);

		sourcePtr = baseDataPtr;
		do
			{
			testStep5.SetReadLength(Min(KTestBufferSize, KTestDataSize - (sourcePtr-baseDataPtr)));
			DoTestStepWithFileFail(testStep5, _L("File source FillBufferL"), KErrNone, 2);

			bufferSize = validBuffer->BufferSize();
			sourcePtr += bufferSize;
			} while (bufferSize == KTestBufferSize);

		if ((sourcePtr - dataPtr.Ptr()) != KTestDataSize)
			{
			INFO_PRINTF3(_L("File source FillBufferL: Source data ptr, expected %d, got %d"), KTestDataSize, sourcePtr - dataPtr.Ptr());
			iTestResult = EFail;
			}

		// Test additional fill request.
		testStep5.SetReadLength(0);
		DoFileTestWithoutAllocFail(testStep5, _L("File source FillBufferL"), KErrNone, 2);
		}
	CleanupStack::PopAndDestroy(source);

	// Test complete fill. (Single test version)
	TFileFillBuffer2TestStep testStep6(iSourceType, testFileName, validBuffer, dummySink, dataPtr);
	testStep6.SetAsynchronousRead(ETrue);
	DoFileTest(testStep6, _L("File source FillBufferL"), KErrNone);

	// Test multiple read requests.
	RPointerArray<CMMFBuffer> bufferArray;
	CleanupClosePushL(bufferArray);

	User::LeaveIfError(bufferArray.Append(validBuffer));
	TFileFillBuffer3TestStep testStep7(iSourceType, testFileName, &bufferArray, dummySink, dataPtr, KTestBufferSize);
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	CMMFDescriptorBuffer* validBuffer2 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer2);
	User::LeaveIfError(bufferArray.Append(validBuffer2));
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	CMMFDescriptorBuffer* validBuffer3 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer3);
	User::LeaveIfError(bufferArray.Append(validBuffer3));
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	CMMFDescriptorBuffer* validBuffer4 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer4);
	User::LeaveIfError(bufferArray.Append(validBuffer4));
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	CMMFDescriptorBuffer* validBuffer5 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer5);
	User::LeaveIfError(bufferArray.Append(validBuffer5));
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	// Test multiple read requests that do not get completed.
	testStep7.SetMaxRequestCount(3);
	DoFileTest(testStep7, _L("File source FillBufferL"), KErrNone);

	// Test multiple read requests, checking correct deletion of completed requests.
	source = CreateFileSourceL(iSourceType, testFsSession, KTest101FileName);
	CleanupDeletePushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TFileFillBuffer4TestStep testStep8(source, &bufferArray, dummySink, dataPtr, KTestBufferSize);
	DoTestStep(testStep8, _L("File source FillBufferL"), KErrNone, 4);

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(5, &bufferArray); // validBuffer5, 4, 3, 2, bufferArray

	// Test zero length read.
	TFileFillBufferTestStep testStep9(iSourceType, testFileName, zeroBuffer, dummySink, dataPtr);
	testStep9.SetAsynchronousRead(ETrue);
	DoFileTest(testStep9, _L("File source FillBufferL"), KErrNone);

	// Test over size read.
	TFileFillBufferTestStep testStep10(iSourceType, testFileName, bigBuffer, dummySink, dataPtr, KTestDataSize);
	testStep10.SetAsynchronousRead(ETrue);
	DoFileTest(testStep10, _L("File source FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(7, dataDes); // dummySink, validBuffer, bigBuffer, zeroBuffer, testFsSession, scheduler, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0107 constructor
 *
 * @xxxx
 *
 */

CTestStep_MMF_SRSSNK_U_0107::CTestStep_MMF_SRSSNK_U_0107()
: CTestStep_MMF_SRSSNK(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0107");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0107 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0107::~CTestStep_MMF_SRSSNK_U_0107()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0107
 *
 * Functional test of file source.
 * BufferEmptiedL() panic test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0107::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// This test fails if running a debug StdSourcesAndSinks dll.
	// So only perform the test on target.

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create source file.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(source);

	// Test BufferEmptiedL
	source->BufferEmptiedL(validBuffer);

	CleanupStack::PopAndDestroy(2, validBuffer); // source, validBuffer

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileReadBuffer constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileReadBuffer::CTestStep_MMF_SRSSNK_FileReadBuffer(TSourceType aSourceType)
	{
	ASSERT(aSourceType == ESourceTypeFile || aSourceType == ESourceTypeFileHandle);
	iSourceType = aSourceType;
	if (iSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0108");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0122");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileReadBuffer destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileReadBuffer::~CTestStep_MMF_SRSSNK_FileReadBuffer()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileReadBuffer
 *
 * Functional test of file source.
 * ReadBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileReadBuffer::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Initialise config data.
	TFileName testFileName(KTest101FileName);

	// Read the data from the file.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	RFile testFile;
	User::LeaveIfError(testFile.Open(testFsSession, testFileName, EFileShareReadersOnly));
	CleanupClosePushL(testFile);

	User::LeaveIfError(testFile.Read(dataPtr));
	CleanupStack::PopAndDestroy(&testFile);

	// Create zero buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create big buffer.
	CMMFDescriptorBuffer* bigBuffer = CMMFDescriptorBuffer::NewL(KTestOverDataSize);
	CleanupStack::PushL(bigBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create source file.
	CMMFClip* source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, testFsSession, KTest101FileName));
	CleanupStack::PushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// Test ReadBufferL (these tests should fail)
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	TClipReadBufferTestStep testStep(source, testBuffer, 0, dummySink, dataPtr);
	DoFileTest(testStep, _L("File source ReadBufferL(1)"), KErrNotSupported);

	TClipReadBuffer2TestStep testStep2(source, testBuffer, 0, dummySink, dataPtr, KTestBufferSize);
	DoFileTest(testStep2, _L("File source ReadBufferL(2)"), KErrNotSupported);

	TClipReadBuffer3TestStep testStep3(source, testBuffer, 0, dataPtr, KTestBufferSize);
	DoFileTest(testStep3, _L("File source ReadBufferL(3)"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test reading from the beginning, middle and end.
	TClipReadBufferTestStep testStep4(source, validBuffer, 0, dummySink, dataPtr, KTestReadSize);
	testStep4.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep4, _L("File source ReadBufferL(4)"), KErrNone, 2);
	CleanupStack::PopAndDestroy(source);

	TFileReadBufferTestStep testStep5(iSourceType, testFileName, validBuffer, 0, dummySink, dataPtr, KTestReadSize);
	testStep5.SetAsynchronousRead(ETrue);
	DoFileTest(testStep5, _L("File source ReadBufferL(5)"), KErrNone);

	testStep5.SetPosition(1);
	DoFileTest(testStep5, _L("File source ReadBufferL(6)"), KErrNone);

	testStep5.SetPosition(KTestDataSize/2);
	DoFileTest(testStep5, _L("File source ReadBufferL(7)"), KErrNone);

	testStep5.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep5, _L("File source ReadBufferL(8)"), KErrNone);

	TFileReadBuffer4TestStep testStep6(iSourceType, testFileName, validBuffer, KTestDataSize - 10, dummySink, dataPtr, KTestReadSize);
	testStep6.SetAsynchronousRead(ETrue);
	testStep6.SetExpectedReadLength(10);
	DoFileTest(testStep6, _L("File source ReadBufferL(9)"), KErrNone);

	testStep6.SetPosition(KTestDataSize);
	testStep6.SetExpectedReadLength(0);
	DoFileTest(testStep6, _L("File source ReadBufferL(10)"), KErrNone);

	testStep6.SetPosition(KTestDataSize + 2);
	DoFileTest(testStep6, _L("File source ReadBufferL(11)"), KErrNone);

	// Attempt to read nothing.
	testStep6.SetPosition(0);
	testStep6.SetReadLength(0);
	DoFileTest(testStep6, _L("File source ReadBufferL(12)"), KErrNone);

	// Attempt to read more data than can fit in the buffer.
	testStep6.SetReadLength(KTestOverReadSize);
	DoFileTest(testStep6, _L("File source ReadBufferL(13)"), KErrOverflow);

	// Attempt to read into a zero length buffer.
	TFileReadBufferTestStep testStep7(iSourceType, testFileName, zeroBuffer, 0, dummySink, dataPtr);
	testStep7.SetAsynchronousRead(ETrue);
	DoFileTest(testStep7, _L("File source ReadBufferL(14)"), KErrNone);

	// Attempt to read more data than present in the source.
	TFileReadBuffer4TestStep testStep8(iSourceType, testFileName, bigBuffer, 0, dummySink, dataPtr, KTestOverDataSize);
	testStep8.SetAsynchronousRead(ETrue);
	testStep8.SetExpectedReadLength(KTestDataSize);
	DoFileTest(testStep8, _L("File source ReadBufferL(15)"), KErrNone);

	// Repeat reads without supplying the length to read.
	source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, testFsSession, KTest101FileName));
	CleanupStack::PushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TClipReadBuffer2TestStep testStep9(source, validBuffer, 0, dummySink, dataPtr, KTestBufferSize);
	testStep9.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep9, _L("File source ReadBufferL(16)"), KErrNone, 2);
	CleanupStack::PopAndDestroy(source);

	TFileReadBuffer2TestStep testStep10(iSourceType, testFileName, validBuffer, 0, dummySink, dataPtr, KTestBufferSize);
	testStep10.SetAsynchronousRead(ETrue);
	DoFileTest(testStep10, _L("File source ReadBufferL(17)"), KErrNone);

	testStep10.SetPosition(1);
	DoFileTest(testStep10, _L("File source ReadBufferL(18)"), KErrNone);

	testStep10.SetPosition(KTestDataSize/2);
	DoFileTest(testStep10, _L("File source ReadBufferL(19)"), KErrNone);

	testStep10.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep10, _L("File source ReadBufferL(20)"), KErrNone);

	testStep10.SetPosition(KTestDataSize - 10);
	testStep10.SetReadLength(10);
	DoFileTest(testStep10, _L("File source ReadBufferL(21)"), KErrNone);

	testStep10.SetPosition(KTestDataSize);
	testStep10.SetReadLength(0);
	DoFileTest(testStep10, _L("File source ReadBufferL(22)"), KErrNone);

	testStep10.SetPosition(KTestDataSize + 2);
	DoFileTest(testStep10, _L("File source ReadBufferL(23)"), KErrNone);

	TFileReadBuffer2TestStep testStep11(iSourceType, testFileName, zeroBuffer, 0, dummySink, dataPtr, 0);
	testStep11.SetAsynchronousRead(ETrue);
	DoFileTest(testStep11, _L("File source ReadBufferL(24)"), KErrNone);

	TFileReadBuffer2TestStep testStep12(iSourceType, testFileName, bigBuffer, 0, dummySink, dataPtr, KTestDataSize);
	testStep12.SetAsynchronousRead(ETrue);
	DoFileTest(testStep12, _L("File source ReadBufferL(25)"), KErrNone);

	// Repeat reads without supplying the length, or a sink.
	source = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, testFsSession, KTest101FileName));
	CleanupStack::PushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	TClipReadBuffer3TestStep testStep13(source, validBuffer, 0, dataPtr, KTestBufferSize);
	DoFileTest(testStep13, _L("File source ReadBufferL(26)"), KErrNone);
	CleanupStack::PopAndDestroy(source);

	TFileReadBuffer3TestStep testStep14(iSourceType, testFileName, validBuffer, 0, dataPtr, KTestBufferSize);
	DoFileTest(testStep14, _L("File source ReadBufferL(27)"), KErrNone);

	testStep14.SetPosition(1);
	DoFileTest(testStep14, _L("File source ReadBufferL(28)"), KErrNone);

	testStep14.SetPosition(KTestDataSize/2);
	DoFileTest(testStep14, _L("File source ReadBufferL(29)"), KErrNone);

	testStep14.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep14, _L("File source ReadBufferL(30)"), KErrNone);

	testStep14.SetPosition(KTestDataSize - 10);
	testStep14.SetReadLength(10);
	DoFileTest(testStep14, _L("File source ReadBufferL(31)"), KErrNone);

	testStep14.SetPosition(KTestDataSize);
	testStep14.SetReadLength(0);
	DoFileTest(testStep14, _L("File source ReadBufferL(32)"), KErrNone);

	testStep14.SetPosition(KTestDataSize + 2);
	DoFileTest(testStep14, _L("File source ReadBufferL(33)"), KErrNone);

	TFileReadBuffer3TestStep testStep15(iSourceType, testFileName, zeroBuffer, 0, dataPtr, 0);
	DoFileTest(testStep15, _L("File source ReadBufferL(34)"), KErrNone);

	TFileReadBuffer3TestStep testStep16(iSourceType, testFileName, bigBuffer, 0, dataPtr, KTestDataSize);
	DoFileTest(testStep16, _L("File source ReadBufferL(35)"), KErrNone);

	// Test multiple read requests.
	RPointerArray<CMMFBuffer> bufferArray;
	CleanupClosePushL(bufferArray);

	User::LeaveIfError(bufferArray.Append(validBuffer));
	TFileReadBuffer5TestStep testStep17(iSourceType, testFileName, &bufferArray, 0, dummySink, dataPtr, KTestReadSize);
	TFileReadBuffer6TestStep testStep18(iSourceType, testFileName, &bufferArray, 0, dummySink, dataPtr, KTestBufferSize);
	DoFileTest(testStep17, _L("File source ReadBufferL(36)"), KErrNone);
	DoFileTest(testStep18, _L("File source ReadBufferL(37)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer2 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer2);
	User::LeaveIfError(bufferArray.Append(validBuffer2));
	DoFileTest(testStep17, _L("File source ReadBufferL(38)"), KErrNone);
	DoFileTest(testStep18, _L("File source ReadBufferL(39)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer3 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer3);
	User::LeaveIfError(bufferArray.Append(validBuffer3));
	DoFileTest(testStep17, _L("File source ReadBufferL(40)"), KErrNone);
	DoFileTest(testStep18, _L("File source ReadBufferL(41)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer4 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer4);
	User::LeaveIfError(bufferArray.Append(validBuffer4));
	DoFileTest(testStep17, _L("File source ReadBufferL(42)"), KErrNone);
	DoFileTest(testStep18, _L("File source ReadBufferL(43)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer5 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer5);
	User::LeaveIfError(bufferArray.Append(validBuffer5));
	DoFileTest(testStep17, _L("File source ReadBufferL(44)"), KErrNone);
	DoFileTest(testStep18, _L("File source ReadBufferL(45)"), KErrNone);

	// Test multiple read requests that do not get completed.
	testStep17.SetMaxRequestCount(3);
	DoFileTest(testStep17, _L("File source FillBufferL(46)"), KErrNone);

	testStep18.SetMaxRequestCount(3);
	DoFileTest(testStep18, _L("File source FillBufferL(47)"), KErrNone);

	// Test multiple read requests, checking correct deletion of completed requests.
	CMMFClip* clip = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, testFsSession, KTest101FileName));
	CleanupDeletePushL(clip);

	User::LeaveIfError(clip->SourceThreadLogon(*dummySink));
	clip->SourcePrimeL();

	TFileReadBuffer7TestStep testStep19(clip, &bufferArray, 0, dummySink, dataPtr, KTestBufferSize);
	DoTestStep(testStep19, _L("File source FillBufferL(48)"), KErrNone, 4);

	CleanupStack::PopAndDestroy(clip);

	clip = STATIC_CAST(CMMFClip*, CreateFileSourceL(iSourceType, testFsSession, KTest101FileName));
	CleanupDeletePushL(clip);

	User::LeaveIfError(clip->SourceThreadLogon(*dummySink));
	clip->SourcePrimeL();

	TFileReadBuffer8TestStep testStep20(clip, &bufferArray, 0, dummySink, dataPtr, KTestBufferSize);
	DoTestStep(testStep20, _L("File source FillBufferL(49)"), KErrNone, 4);

	CleanupStack::PopAndDestroy(clip);

	CleanupStack::PopAndDestroy(12, dataDes); // validBuffer5, 4, 3, 2, bufferArray, dummySink, bigBuffer, zeroBuffer, validBuffer, testFsSession, scheduler, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0109 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0109::CTestStep_MMF_SRSSNK_U_0109()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0109");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0109 destructor
 *
 * @xxxx
 *
 */

CTestStep_MMF_SRSSNK_U_0109::~CTestStep_MMF_SRSSNK_U_0109()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0109
 *
 * Functional test of file source.
 * Source custom commands test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0109::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSource(KUidMmfFileSource, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSource, dummyArgs, dummyArgs);
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("File source SourceCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(); // dummyController

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSinkMisc constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileSinkMisc::CTestStep_MMF_SRSSNK_FileSinkMisc(TSinkType aSinkType)
	:iSinkType(aSinkType)
	{
	if (iSinkType == ESinkTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0110");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0130");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSinkMisc destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileSinkMisc::~CTestStep_MMF_SRSSNK_FileSinkMisc()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileSinkMisc
 *
 * Functional test of file sink.
 * Misc. functions test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileSinkMisc::DoTestStepL()
	{
	__MM_HEAP_MARK;

	TFileName testFileName(KTest101FileName);

	// Get file info.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	// Get the size of the test file.
	RFile testFile;
	User::LeaveIfError(testFile.Open(testFsSession, testFileName, EFileRead));
	CleanupClosePushL(testFile);

	TInt fileSize;
	User::LeaveIfError(testFile.Size(fileSize));
	CleanupStack::PopAndDestroy(&testFile);

	// Create dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create sink.
	MDataSink* sink = CreateFileSinkL(iSinkType, testFsSession, testFileName);
	CleanupDeletePushL(sink);

	// Test create sink buffer. (This should fail)
	TCreateSinkBufferTestStep0 testStep(sink, KUidMmfDescriptorBuffer, ETrue, NULL);
	DoTest(testStep, _L("File sink CreateSinkBuffer"), KErrNotSupported);

	// Test default base class fns. (These should all do nothing)
	TSinkThreadLogonTestStep testStep2(sink, dummySource);
	DoFileTest(testStep2, _L("File sink SinkThreadLogon"), KErrNone);

	TNegotiateTestStep testStep3(sink, dummySource);
	DoTest(testStep3, _L("File sink NegotiateL"), KErrNone);

	sink->SinkPrimeL();
	sink->SinkPlayL();
	sink->SinkPauseL();
	sink->SinkStopL();

	TMMFPrioritySettings prioritySettings;
	sink->SetSinkPrioritySettings(prioritySettings);

	// Test sink log on.
	sink->SinkPrimeL();
	TFileSinkThreadLogonTestStep testStep5(sink, dummySource, fileSize);
	DoFileTest(testStep5, _L("File sink SinkThreadLogon"), KErrNone);

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(2, &testFsSession); //dummySource, testFsSession

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_FileEmptyBuffer constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileEmptyBuffer::CTestStep_MMF_SRSSNK_FileEmptyBuffer(TSinkType aSinkType)
	: iSinkType(aSinkType)
	{
	if (iSinkType == ESinkTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0111");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0131");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileEmptyBuffer destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileEmptyBuffer::~CTestStep_MMF_SRSSNK_FileEmptyBuffer()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileEmptyBuffer
 *
 * There was a flaw in that a failure in DoFileTest and its variants is overlooked.
 * Each time it is called the status of iTestResult must be checked before it's
 * overwritten. It is set to EPass again so that false negatives aren't recorded.
 *
 * The success of this test step depends on the 'failure' variable.
 *
 * EmptyBufferL() test.
 *
 * @xxxx
 */
_LIT(KTest111FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile4.dat");
TVerdict CTestStep_MMF_SRSSNK_FileEmptyBuffer::DoTestStepL()
	{
	__MM_HEAP_MARK;

	TBool failure = EFalse;

	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Set the filename and create a file session.
	TFileName testFileName(KTest111FileName);

	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Delete the test file (might exist from last time)
	testFsSession.Delete(testFileName);

	// Create a dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create a zero length buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create a valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Set the length to write.
	validBuffer->Data().SetLength(KTestWriteSize);

	// Create the sink.
	MDataSink* sink = CreateFileSinkL(iSinkType, testFsSession, testFileName);
	CleanupDeletePushL(sink);

	// Create a test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test EmptyBufferL.
	TFileEmptyBufferTestStep testStep(sink, testBuffer, dummySource, testFsSession, testFileName, dataPtr);
	DoFileTest(testStep, _L("File sink EmptyBufferL(1)"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(1) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Test valid EmptyBufferL.
	TFileEmptyBufferTestStep testStep2(sink, validBuffer, dummySource, testFsSession, testFileName, dataPtr);
	testStep2.SetAsynchronousWrite(ETrue);
	DoFileTestWithoutAllocFail(testStep2, _L("File sink EmptyBufferL(2)"), KErrNone, 2);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(2) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(sink);

	TFileEmptyBuffer2TestStep testStep3(iSinkType, validBuffer, dummySource, testFsSession, testFileName, dataPtr);
	testStep3.SetAsynchronousWrite(ETrue);
	DoFileTest(testStep3, _L("File sink EmptyBufferL(3)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(3) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Test complete empty.
	TFileEmptyBuffer3TestStep testStep4(iSinkType, validBuffer, dummySource, testFsSession, testFileName, dataPtr);
	testStep4.SetAsynchronousWrite(ETrue);
	DoFileTest(testStep4, _L("File sink EmptyBufferL(4)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(4) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Test multiple write requests.
	RPointerArray<CMMFBuffer> bufferArray;
	CleanupClosePushL(bufferArray);

	User::LeaveIfError(bufferArray.Append(validBuffer));
	TFileEmptyBuffer4TestStep testStep5(iSinkType, &bufferArray, dummySource, testFsSession, testFileName, dataPtr);
	testStep5.SetAsynchronousWrite(ETrue);
	testStep5.SetWriteLength(KTestWriteSize);
	DoFileTest(testStep5, _L("File sink EmptyBufferL(5)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(5) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Create valid buffer 2.
	CMMFDescriptorBuffer* validBuffer2 = CMMFDescriptorBuffer::NewL(KTestWriteSize);
	CleanupStack::PushL(validBuffer2);

	// Set the length to write.
	validBuffer2->Data().SetLength(KTestWriteSize);
	User::LeaveIfError(bufferArray.Append(validBuffer2));
	DoFileTest(testStep5, _L("File sink EmptyBufferL(6)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(6) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Create valid buffer 3.
	CMMFDescriptorBuffer* validBuffer3 = CMMFDescriptorBuffer::NewL(KTestWriteSize);
	CleanupStack::PushL(validBuffer3);

	// Set the length to write.
	validBuffer3->Data().SetLength(KTestWriteSize);

	User::LeaveIfError(bufferArray.Append(validBuffer3));
	DoFileTest(testStep5, _L("File sink EmptyBufferL(7)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(7) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Create valid buffer 4.
	CMMFDescriptorBuffer* validBuffer4 = CMMFDescriptorBuffer::NewL(KTestWriteSize);
	CleanupStack::PushL(validBuffer4);

	// Set the length to write.
	validBuffer4->Data().SetLength(KTestWriteSize);

	User::LeaveIfError(bufferArray.Append(validBuffer4));
	DoFileTest(testStep5, _L("File sink EmptyBufferL(8)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(8) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Create valid buffer 5.
	CMMFDescriptorBuffer* validBuffer5 = CMMFDescriptorBuffer::NewL(KTestWriteSize);
	CleanupStack::PushL(validBuffer5);

	// Set the length to write.
	validBuffer5->Data().SetLength(KTestWriteSize);

	User::LeaveIfError(bufferArray.Append(validBuffer5));
	DoFileTest(testStep5, _L("File sink EmptyBufferL(9)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(9) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	// Test multiple write requests that do not get completed.
	TFileEmptyBuffer5TestStep testStep6(iSinkType, &bufferArray, dummySource, testFsSession, testFileName, dataPtr);
	testStep6.SetAsynchronousWrite(ETrue);
	testStep6.SetWriteLength(KTestWriteSize);
	testStep6.SetMaxRequestCount(3);
	DoFileTest(testStep6, _L("File sink EmptyBufferL(10)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(10) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	dataPtr.SetLength(0);
	// Test multiple write requests, checking correct deletion of completed requests.
	sink = CreateFileSinkL(iSinkType, testFsSession, testFileName);
	CleanupDeletePushL(sink);

	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();

	TFileEmptyBuffer6TestStep testStep7(sink, &bufferArray, dummySource, testFsSession, testFileName, dataPtr);
	testStep7.SetAsynchronousWrite(ETrue);
	testStep7.SetWriteLength(KTestWriteSize);
	DoTestStep(testStep7, _L("File sink EmptyBufferL(11)"), KErrNone, 4);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(11) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(5, &bufferArray); // validBuffer5, 4, 3, 2, bufferArray.

	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	dataPtr.SetLength(0);
	// Test zero length write.
	TFileEmptyBuffer2TestStep testStep8(iSinkType, zeroBuffer, dummySource, testFsSession, testFileName, dataPtr);
	testStep8.SetAsynchronousWrite(ETrue);
	DoFileTest(testStep8, _L("File sink EmptyBufferL(12)"), KErrNone);
	if (iTestResult != EPass)
		{
		INFO_PRINTF1(_L("File Sink EmptyBufferL(12) failed"));
		failure = ETrue;
		iTestResult = EPass;
		}

	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	CleanupStack::PopAndDestroy(6, dataDes); // dummySource, validBuffer, zeroBuffer, testFsSession, scheduler, dataDes.

	__MM_HEAP_MARKEND;

	return (failure ? EFail : EPass);
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0112 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0112::CTestStep_MMF_SRSSNK_U_0112()
: CTestStep_MMF_SRSSNK(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0112");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0112 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0112::~CTestStep_MMF_SRSSNK_U_0112()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0112
 *
 * Functional test of file sink.
 * BufferFilledL() panic test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0112::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// This test fails if running a debug StdSourcesAndSinks dll.
	// So only perform the test on target.

	iTestResult = EFail;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create file sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfFileSink, configDes);
	CleanupDeletePushL(sink);

	// Test BufferFilledL
	TRAPD(err, sink->BufferFilledL(validBuffer));
	if (err == KErrNone)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(validBuffer);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileWriteBuffer constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileWriteBuffer::CTestStep_MMF_SRSSNK_FileWriteBuffer(TSinkType aSinkType)
	: iSinkType(aSinkType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	if (aSinkType == ESinkTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0113");
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0132");
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileWriteBuffer destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileWriteBuffer::~CTestStep_MMF_SRSSNK_FileWriteBuffer()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileWriteBuffer
 *
 * Functional test of descriptor sink.
 * WriteBufferL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileWriteBuffer::DoTestStepL()
	{
	__MM_HEAP_MARK;

	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Set the filename and create a file session.
	TFileName testFileName(KTest103FileName);

	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Initialise config data.
	TMMFFileConfig configFile;
	configFile().iPath = testFileName;

	// Create zero length buffer.
	CMMFDescriptorBuffer* zeroBuffer = CMMFDescriptorBuffer::NewL(0);
	CleanupStack::PushL(zeroBuffer);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Set it's length
	validBuffer->Data().SetLength(KTestWriteSize);

	// Create dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create sink.
	CMMFClip* sink = static_cast<CMMFClip*> (CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupStack::PushL(sink);

	// check that Size() can be called before SinkPrimeL()
	TInt size = sink->Size();

	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));

	// Create a test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Test WriteBufferL (these tests should fail)
	TFileWriteBufferTestStep testStep(sink, testBuffer, 0, dummySource, testFsSession, testFileName, dataPtr);
	DoFileTest(testStep, _L("File sink WriteBufferL(1)"), KErrNotSupported);

	TFileWriteBuffer2TestStep testStep2(sink, testBuffer, 0, dummySource, testFsSession, testFileName, dataPtr);
	DoFileTest(testStep2, _L("File sink WriteBufferL(2)"), KErrNotSupported);

	TFileWriteBuffer3TestStep testStep3(sink, testBuffer, 0, testFsSession, testFileName, dataPtr);
	DoFileTest(testStep3, _L("File sink WriteBufferL(3)"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);

	// Test writing to the beginning, middle and end.
	TFileWriteBufferTestStep testStep4(sink, validBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTestWithoutAllocFail(testStep4, _L("File sink WriteBufferL(4)"), KErrNone, 2);
	CleanupStack::PopAndDestroy(sink);

	TFileWriteBuffer4TestStep testStep5(iSinkType, testFileName, validBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep5, _L("File sink WriteBufferL(5)"), KErrNone);

	testStep5.SetPosition(1);
	DoFileTest(testStep5, _L("File sink WriteBufferL(6)"), KErrNone);

	testStep5.SetPosition(KTestDataSize/2);
	DoFileTest(testStep5, _L("File sink WriteBufferL(7)"), KErrNone);

	testStep5.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep5, _L("File sink WriteBufferL(8)"), KErrNone);

	testStep5.SetPosition(KTestDataSize - 10);
	testStep5.SetWriteLength(10);
	DoFileTest(testStep5, _L("File sink WriteBufferL(9)"), KErrNone);

	testStep5.SetPosition(KTestDataSize);
	testStep5.SetWriteLength(0);
	DoFileTest(testStep5, _L("File sink WriteBufferL(10)"), KErrNone);

	testStep5.SetPosition(KTestDataSize + 2);
	testStep5.SetWriteLength(0);
	DoFileTest(testStep5, _L("File sink WriteBufferL(11)"), KErrNone);

	// Attempt to write more data than is in the buffer.
	TFileWriteBuffer5TestStep testStep6(iSinkType, testFileName, validBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	testStep6.SetPosition(KTestDataSize);
	testStep6.SetWriteLength(KTestOverWriteSize);
	DoFileTest(testStep6, _L("File sink WriteBufferL(12)"), KErrNone);

	// Attempt to write a zero length buffer.
	TFileWriteBuffer4TestStep testStep7(iSinkType, testFileName, zeroBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	testStep7.SetWriteLength(0);
	DoFileTest(testStep7, _L("File sink WriteBufferL(13)"), KErrNone);


	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	// Repeat writes without supplying the length to write.
	dataPtr.SetLength(0);

	sink = static_cast<CMMFClip*> (CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupStack::PushL(sink);
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));

	TFileWriteBuffer2TestStep testStep8(sink, validBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTestWithoutAllocFail(testStep8, _L("File sink WriteBufferL(14)"), KErrNone, 2);
	CleanupStack::PopAndDestroy(sink);

	TFileWriteBuffer6TestStep testStep9(iSinkType, testFileName, validBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep9, _L("File sink WriteBufferL(15)"), KErrNone);

	testStep9.SetPosition(1);
	DoFileTest(testStep9, _L("File sink WriteBufferL(16)"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2);
	DoFileTest(testStep9, _L("File sink WriteBufferL(17)"), KErrNone);

	testStep9.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep9, _L("File sink WriteBufferL(18)"), KErrNone);

	testStep9.SetPosition(KTestDataSize - 10);
	testStep9.SetWriteLength(10);
	DoFileTest(testStep9, _L("File sink WriteBufferL(19)"), KErrNone);

	testStep9.SetPosition(KTestDataSize);
	testStep9.SetWriteLength(0);
	DoFileTest(testStep9, _L("File sink WriteBufferL(20)"), KErrNone);

	testStep9.SetPosition(KTestDataSize + 2);
	testStep9.SetWriteLength(0);
	DoFileTest(testStep9, _L("File sink WriteBufferL(21)"), KErrNone);

	// Attempt to write a zero length buffer.
	TFileWriteBuffer6TestStep testStep10(iSinkType, testFileName, zeroBuffer, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	testStep10.SetWriteLength(0);
	DoFileTest(testStep10, _L("File sink WriteBufferL(22)"), KErrNone);

	// Delete the test file.
	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	// Repeat writes without supplying the length, or a source.
	dataPtr.SetLength(0);

	sink = static_cast<CMMFClip*> (CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupStack::PushL(sink);
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));

	TFileWriteBuffer3TestStep testStep11(sink, validBuffer, 0, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep11, _L("File sink WriteBufferL(23)"), KErrNone);
	CleanupStack::PopAndDestroy(sink);

	TFileWriteBuffer7TestStep testStep12(iSinkType, testFileName, validBuffer, 0, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep12, _L("File sink WriteBufferL(24)"), KErrNone);

	testStep12.SetPosition(1);
	DoFileTest(testStep12, _L("File sink WriteBufferL(25)"), KErrNone);

	testStep12.SetPosition(KTestDataSize/2);
	DoFileTest(testStep12, _L("File sink WriteBufferL(26)"), KErrNone);

	testStep12.SetPosition(KTestDataSize/2 + 1);
	DoFileTest(testStep12, _L("File sink WriteBufferL(27)"), KErrNone);

	testStep12.SetPosition(KTestDataSize - 10);
	testStep12.SetWriteLength(10);
	DoFileTest(testStep12, _L("File sink WriteBufferL(28)"), KErrNone);

	testStep12.SetPosition(KTestDataSize);
	testStep12.SetWriteLength(0);
	DoFileTest(testStep12, _L("File sink WriteBufferL(29)"), KErrNone);

	testStep12.SetPosition(KTestDataSize + 2);
	testStep12.SetWriteLength(0);
	DoFileTest(testStep12, _L("File sink WriteBufferL(30)"), KErrNone);

	// Attempt to write a zero length buffer.
	TFileWriteBuffer7TestStep testStep13(iSinkType, testFileName, zeroBuffer, 0, testFsSession, testFileName, dataPtr, KTestWriteSize);
	testStep13.SetWriteLength(0);
	DoFileTest(testStep13, _L("File sink WriteBufferL(31)"), KErrNone);

	// Delete the test file.
	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	// Test multiple write requests.
	dataPtr.SetLength(0);
	RPointerArray<CMMFBuffer> bufferArray;
	CleanupClosePushL(bufferArray);

	User::LeaveIfError(bufferArray.Append(validBuffer));
	TFileWriteBuffer8TestStep testStep14(iSinkType, testFileName, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	TFileWriteBuffer9TestStep testStep15(iSinkType, testFileName, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep14, _L("File sink WriteBufferL(32)"), KErrNone);
	DoFileTest(testStep15, _L("File sink WriteBufferL(33)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer2 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer2);

	validBuffer2->Data().SetLength(KTestWriteSize);
	User::LeaveIfError(bufferArray.Append(validBuffer2));
	DoFileTest(testStep14, _L("File sink WriteBufferL(34)"), KErrNone);
	DoFileTest(testStep15, _L("File sink WriteBufferL(35)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer3 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer3);

	validBuffer3->Data().SetLength(KTestWriteSize);
	User::LeaveIfError(bufferArray.Append(validBuffer3));
	DoFileTest(testStep14, _L("File sink WriteBufferL(36)"), KErrNone);
	DoFileTest(testStep15, _L("File sink WriteBufferL(37)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer4 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer4);

	validBuffer4->Data().SetLength(KTestWriteSize);
	User::LeaveIfError(bufferArray.Append(validBuffer4));
	DoFileTest(testStep14, _L("File sink WriteBufferL(38)"), KErrNone);
	DoFileTest(testStep15, _L("File sink WriteBufferL(39)"), KErrNone);

	CMMFDescriptorBuffer* validBuffer5 = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer5);

	validBuffer5->Data().SetLength(KTestWriteSize);
	User::LeaveIfError(bufferArray.Append(validBuffer5));
	DoFileTest(testStep14, _L("File sink WriteBufferL(40)"), KErrNone);
	DoFileTest(testStep15, _L("File sink WriteBufferL(41)"), KErrNone);

	// Test multiple write requests that do not get completed.
	TFileWriteBuffer10TestStep testStep16(iSinkType, testFileName, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep16, _L("File sink WriteBufferL(42)"), KErrNone);

	TFileWriteBuffer11TestStep testStep17(iSinkType, testFileName, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoFileTest(testStep17, _L("File sink WriteBufferL(43)"), KErrNone);

	// Test multiple write requests, checking correct deletion of completed requests.
	CMMFClip* clip = static_cast<CMMFClip*> (CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupDeletePushL(clip);

	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	TFileWriteBuffer12TestStep testStep18(clip, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoTestStep(testStep18, _L("File sink WriteBufferL(44)"), KErrNone, 4);

	CleanupStack::PopAndDestroy(clip);

	clip = static_cast<CMMFClip*> (CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupStack::PushL(clip);

	User::LeaveIfError(clip->SinkThreadLogon(*dummySource));
	TFileWriteBuffer13TestStep testStep19(clip, &bufferArray, 0, dummySource, testFsSession, testFileName, dataPtr, KTestWriteSize);
	DoTestStep(testStep19, _L("File sink WriteBufferL(45)"), KErrNone, 4);

	CleanupStack::PopAndDestroy(clip);

	// Delete the test file.
	DeleteFileSinkL(iSinkType, testFsSession, testFileName);

	CleanupStack::PopAndDestroy(11, dataDes); // validBuffer5, 4, 3, 2, bufferArray, dummySource, validBuffer, zeroBuffer, testFsSession, scheduler, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0114 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0114::CTestStep_MMF_SRSSNK_U_0114()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0114");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0114 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0114::~CTestStep_MMF_SRSSNK_U_0114()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0114
 *
 * Functional test of file sink.
 * SinkCustomCommand() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0114::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSink(KUidMmfFileSink, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSink, dummyArgs, dummyArgs);
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("File sink SinkCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(&dummyController);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0115 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0115::CTestStep_MMF_SRSSNK_U_0115()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0115");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0115 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0115::~CTestStep_MMF_SRSSNK_U_0115()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0115
 *
 * CMMFFile
 * Request size / SetLastBuffer test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0115::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create source buffer.
	HBufC8* dataDes = HBufC8::NewLC(KTestDataSize);
	TPtr8 dataPtr(dataDes->Des());

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	// Read the data from the file.
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	RFile testFile;
	User::LeaveIfError(testFile.Open(testFsSession, configDes().iPath, EFileShareReadersOnly));
	CleanupClosePushL(testFile);

	User::LeaveIfError(testFile.Read(dataPtr));
	CleanupStack::PopAndDestroy(2); // testFile, testFsSession

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Test fill last buffer
	TFillLastBufferTestStep testStep(KUidMmfFileSource, configDes, dummySink, dataPtr);
	testStep.SetAsynchronousRead(ETrue);
	DoFileTest(testStep, _L("File source: Fill last buffer"), KErrNone);

	// Create a source.
	CMMFClip* source = STATIC_CAST(CMMFClip*, MDataSource::NewSourceL(KUidMmfFileSource, configDes));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// Test read last buffer (API version 1)
	TReadLastBufferTestStep testStep2(source, dummySink, dataPtr);
	testStep2.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep2, _L("File source: Read last buffer"), KErrNone, 2);

	TReadLastBuffer2TestStep testStep3(source, dummySink, dataPtr);
	testStep3.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep3, _L("File source: Read last buffer"), KErrNone, 2);

	TReadLastBuffer3TestStep testStep4(source, dataPtr);
	testStep4.SetAsynchronousRead(ETrue);
	DoFileTestWithoutAllocFail(testStep4, _L("File source: Read last buffer"), KErrNone, 2);

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(3, dataDes); // dummySink, scheduler, dataDes

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_FileEvaluateIntent constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileEvaluateIntent::CTestStep_MMF_SRSSNK_FileEvaluateIntent(TSourceType aSourceType)
	: iSourceType(aSourceType)
	{
	if (aSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0116");
		iSinkType = ESinkTypeFile;
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0140");
		iSinkType = ESinkTypeFileHandle;
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileEvaluateIntent destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileEvaluateIntent::~CTestStep_MMF_SRSSNK_FileEvaluateIntent()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileEvaluateIntent
 *
 * CMMFFile
 * Evaluate Intent
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileEvaluateIntent::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	TFileName testFileName(KTest101FileName);
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Create dummy source
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);


	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);


	// Create a source.
	CMMFClip* source = static_cast<CMMFClip*>(CreateFileSourceL(iSourceType, testFsSession, testFileName));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// we know it is a file source since we have just created it
	CMMFFile* file = static_cast<CMMFFile*>(source);

	// file open, for unprotected files this should always return KErrNone
	TInt err = file->EvaluateIntent(EPeek);
	if (err != KErrNone)
		iTestResult = EFail;

	source->SourceStopL();

	// file closed
	err = file->EvaluateIntent(EPeek);
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(source);

	// Create a sink.
	CMMFClip* sink = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupDeletePushL(sink);
	User::LeaveIfError(sink->SourceThreadLogon(*dummySource));
	sink->SinkPrimeL();

	// we know it is a file sink since we have just created it
	file = static_cast<CMMFFile*>(sink);

	// file open, for unprotected files this should always return KErrNone
	err = file->EvaluateIntent(EPeek);
	if (err != KErrNone)
		iTestResult = EFail;

	sink->SinkStopL();

	// file closed
	err = file->EvaluateIntent(EPeek);
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(sink);


	CleanupStack::PopAndDestroy(4, scheduler); // dummySink, dummySource, testFsSession, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_FileExecuteIntent constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileExecuteIntent::CTestStep_MMF_SRSSNK_FileExecuteIntent(TSourceType aSourceType)
	: iSourceType(aSourceType)
	{
	if (aSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0117");
		iSinkType = ESinkTypeFile;
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0141");
		iSinkType = ESinkTypeFileHandle;
		}

	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileExecuteIntent destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileExecuteIntent::~CTestStep_MMF_SRSSNK_FileExecuteIntent()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileExecuteIntent
 *
 * CMMFFile
 * Execute Intent test
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileExecuteIntent::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	TFileName testFileName(KTest101FileName);
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Create dummy source
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);


	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);


	// Create a source.
	CMMFClip* source = static_cast<CMMFClip*>(CreateFileSourceL(iSourceType, testFsSession, testFileName));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// we know it is a file source since we have just created it
	CMMFFile* file = static_cast<CMMFFile*>(source);

	// file open, for unprotected files this should always return KErrNone
	TInt err = file->ExecuteIntent(EPeek);
	if (err != KErrNone)
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrNone, err);
		}

	source->SourceStopL();

	// file closed
	err = file->ExecuteIntent(EPeek);
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), iSinkType == ESinkTypeFile ? KErrNotReady : KErrNone, err);
		}

	CleanupStack::PopAndDestroy(source);

	// Create a sink.
	CMMFClip* sink = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupDeletePushL(sink);
	User::LeaveIfError(sink->SourceThreadLogon(*dummySource));
	sink->SinkPrimeL();

	// we know it is a file sink since we have just created it
	file = static_cast<CMMFFile*>(sink);

	// file open, for unprotected files this should always return KErrNone
	err = file->ExecuteIntent(EPeek);
	if (err != KErrNone)
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrNone, err);
		}

	sink->SinkStopL();

	// file closed
	err = file->ExecuteIntent(EPeek);
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), iSinkType == ESinkTypeFile ? KErrNotReady : KErrNone, err);
		}

	CleanupStack::PopAndDestroy(sink);


	CleanupStack::PopAndDestroy(4, scheduler); // dummySink, dummySource, testFsSession, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_FileIsProtected constructor
 *
 * IsProtected Test
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileIsProtected::CTestStep_MMF_SRSSNK_FileIsProtected(TSourceType aSourceType)
	: iSourceType(aSourceType)
	{
	if (aSourceType == ESourceTypeFile)
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0118");
		iSinkType = ESinkTypeFile;
		}
	else
		{
		iTestStepName = _L("MM-MMF-SRSSNK-U-0142");
		iSinkType = ESinkTypeFileHandle;
		}
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileIsProtected destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_FileIsProtected::~CTestStep_MMF_SRSSNK_FileIsProtected()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_FileIsProtected
 *
 * CMMFFile
 * Execute Intent test
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_FileIsProtected::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	TFileName testFileName(KTest101FileName);
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Create dummy source
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);


	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);


	// Create a source.
	CMMFClip* source = static_cast<CMMFClip*>(CreateFileSourceL(iSourceType, testFsSession, testFileName));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// we know it is a file source since we have just created it
	CMMFFile* file = static_cast<CMMFFile*>(source);

	TBool isProtected = EFalse;
	TInt err = KErrNone;
	// file open, for unprotected files this should always be false
	TRAP(err, isProtected = file->IsProtectedL());
	if (err != KErrNone)
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrNone, err);
		}
	else if (isProtected)
		{
		iTestResult = EFail;
		INFO_PRINTF1(_L("Failure, expected unprotected file"));
		}

	source->SourceStopL();

	// file closed, should leave KErrNotReady
	TRAP(err, isProtected = file->IsProtectedL());
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), iSinkType == ESinkTypeFile ? KErrNotReady : KErrNone, err);
		}

	CleanupStack::PopAndDestroy(source);

	// Create a sink.
	CMMFClip* sink = static_cast<CMMFClip*>(CreateFileSinkL(iSinkType, testFsSession, testFileName));
	CleanupDeletePushL(sink);
	User::LeaveIfError(sink->SourceThreadLogon(*dummySource));
	sink->SinkPrimeL();

	// we know it is a file sink since we have just created it
	file = static_cast<CMMFFile*>(sink);

	// file open, for unprotected files this should always return EFalse
	TRAP(err, isProtected = file->IsProtectedL());
	if (err != KErrNone)
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrNone, err);
		}
	else if (isProtected)
		{
		iTestResult = EFail;
		INFO_PRINTF1(_L("Failure, expected unprotected file"));
		}

	sink->SinkStopL();

	// file closed, should leave KErrNotReady
	TRAP(err, isProtected = file->IsProtectedL());
	if (((iSinkType == ESinkTypeFile) && (err != KErrNotReady)) ||
		((iSinkType == ESinkTypeFileHandle) && (err != KErrNone)))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), iSinkType == ESinkTypeFile ? KErrNotReady : KErrNone, err);
		}

	CleanupStack::PopAndDestroy(sink);


	CleanupStack::PopAndDestroy(4, scheduler); // dummySink, dummySource, testFsSession, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0119 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0119::CTestStep_MMF_SRSSNK_U_0119()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0119");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0119 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0119::~CTestStep_MMF_SRSSNK_U_0119()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0119
 *
 * CMMFFile
 * Test file access denied
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0119::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EPass;
	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	// lock the file by opening it for writing for the duration of this test
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs,KTest101FileName, EFileWrite));
	CleanupClosePushL(file);

	// Create dummy source
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);


	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);


	// Create a source.
	CMMFClip* source = static_cast<CMMFClip*>(MDataSource::NewSourceL(KUidMmfFileSource, configDes));
	CleanupDeletePushL(source);
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	TRAPD(err, source->SourcePrimeL());
	if (err != KErrInUse)
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrInUse, err);
		}


	CleanupStack::PopAndDestroy(source);

	// Create a sink.
	CMMFClip* sink = static_cast<CMMFClip*>(MDataSink::NewSinkL(KUidMmfFileSink, configDes));
	CleanupDeletePushL(sink);
	User::LeaveIfError(sink->SourceThreadLogon(*dummySource));

	TRAP(err, sink->SinkPrimeL());
	if (err != KErrInUse)

		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Failure, expected error %d, received error %d"), KErrInUse, err);
		}


	CleanupStack::PopAndDestroy(sink);


	CleanupStack::PopAndDestroy(5, scheduler); // dummySink, dummySource, file, fs, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0201 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0201::CTestStep_MMF_SRSSNK_U_0201()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0201");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0201 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0201::~CTestStep_MMF_SRSSNK_U_0201()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0201
 *
 * Test creation/destruction of descriptor source.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0201::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Test construction/destruction.
	TBuf8<1> configDes;
	TNewSourceTestStep testStep(KUidMmfAudioInput, configDes);
	DoTest(testStep, _L("Construct audio source"), KErrNone);

	// Create dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create an audio source.
	MDataSource* source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);
	CleanupDeletePushL(source);

	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->SourcePrimeL();

	// Test that the source is initialised correctly.
	TSourceTypeTestStep testStep2(source, KUidMmfAudioInput, KMMFFourCCCodePCM16, ETrue);
	DoTest(testStep2, _L("Audio source type"), KErrNone);

	TCanCreateSourceBufferTestStep testStep3(source, ETrue);
	DoTest(testStep3, _L("Audio Can create source buffer"), KErrNone);

	TSourceSampleConvertTestStep testStep4(source, EFalse);
	DoTest(testStep4, _L("Audio source sample convert"), KErrNone);

	// Test that the audio source is initialised correctly.
	CMMFAudioInput* sourceAudioInput = STATIC_CAST(CMMFAudioInput*, source);

	// XXX These fns. go bang after construction. This is not good.
	// They are ok, provided we have logged on and called SourcePrimeL().
	#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if (sourceAudioInput->SoundDevice().Volume() != 0)
	#else
	if (sourceAudioInput->SoundDevice().Volume() != (sourceAudioInput->SoundDevice().MaxVolume() + 1) / 2)
	#endif
		{
		#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		INFO_PRINTF1(_L("Audio Input: Device volume != 0\n"));
		#else
		INFO_PRINTF2(_L("Audio Input: Device volume != %d\n"), (sourceAudioInput->SoundDevice().MaxVolume() + 1) / 2);
		#endif
		iTestResult = EFail;
		}

	if (sourceAudioInput->BytesPlayed() != 0)
		{
		INFO_PRINTF1(_L("Audio Input: Bytes played != 0\n"));
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(dummySink);
	CleanupStack::PopAndDestroy(scheduler);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0202 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0202::CTestStep_MMF_SRSSNK_U_0202()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0202");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0202 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0202::~CTestStep_MMF_SRSSNK_U_0202()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0202
 *
 * Test creation/destruction of audio sink.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0202::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Test construction/destruction.
	TBuf8<1> configDes;
	TNewSinkTestStep testStep(KUidMmfAudioOutput, configDes);
	DoTest(testStep, _L("Construct audio sink"), KErrNone);

	// Create a dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create an audio sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, configDes);
	CleanupDeletePushL(sink);

	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();

	// Test that the sink is initialised correctly.
	TSinkTypeTestStep testStep2(sink, KUidMmfAudioOutput, KMMFFourCCCodePCM16, ETrue);
	DoTest(testStep2, _L("Audio sink type"), KErrNone);

	TCanCreateSinkBufferTestStep testStep3(sink, ETrue);
	DoTest(testStep3, _L("Audio Can create sink buffer"), KErrNone);

	// Test that the audio sink is initialised correctly.
	CMMFAudioOutput* sinkAudioOutput = STATIC_CAST(CMMFAudioOutput*, sink);
	if (sinkAudioOutput->BytesPlayed() != 0)
		{
		INFO_PRINTF1(_L("Audio Input: Bytes played != 0\n"));
		iTestResult = EFail;
		}

	CleanupStack::PopAndDestroy(sink);

	CleanupStack::PopAndDestroy(dummySource);
	CleanupStack::PopAndDestroy(scheduler);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0203 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0203::CTestStep_MMF_SRSSNK_U_0203()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0203");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0203 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0203::~CTestStep_MMF_SRSSNK_U_0203()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0203
 *
 * Test audio source misc. fns.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0203::DoTestStepL()
	{
	__MM_HEAP_MARK;

	const TInt KDevSoundMaxFrameSize = 0x4000;  //16K from SoundDeviceBody.h

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create an audio source.
	TBuf8<1> configDes;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);
	CleanupDeletePushL(source);

	// Test create source buffer.
	TCreateSourceBuffer1TestStep testStep(ESourceTypeAudio, dummySink, KUidMmfPtrBuffer, ETrue, KDevSoundMaxFrameSize);

	DoTest(testStep, _L("Audio source CreateSourceBuffer1"), KErrNone);



	TCreateSourceBuffer2TestStep testStep2(ESourceTypeAudio, dummySink, NULL, KUidMmfPtrBuffer, ETrue, KDevSoundMaxFrameSize);



	DoTest(testStep2, _L("Audio source CreateSourceBuffer2"), KErrNone);

	CMMFAudioInput* sourceAudioInput = STATIC_CAST(CMMFAudioInput*, source);
	TAudioInputCreateSourceBuffer1TestStep testStep3(sourceAudioInput);
	DoTest(testStep3, _L("Audio source CreateSourceBuffer3"), KErrNone);

	TAudioInputCreateSourceBuffer2TestStep testStep4(sourceAudioInput, NULL);
	DoTest(testStep4, _L("Audio source CreateSourceBuffer4"), KErrNone);

	// Test the rest...
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));

	TFourCC fourCC = KMMFFourCCCodePCM16;
	TFourCC emptyFourCC;

	TBufC8<5> fourCCString(KEmptyFourCCCode);
	TPtr8 fourCCPtr = fourCCString.Des();
	TPtr8 fourCCPtr1(&fourCCPtr[0], 4);
	emptyFourCC.FourCC(&fourCCPtr1);
	source->NegotiateSourceL(*dummySink);

	TMediaId mediaId;
	if ((source->SourceDataTypeCode(mediaId) != fourCC) && (source->SourceDataTypeCode(mediaId) != emptyFourCC))
		{
		INFO_PRINTF1(_L("Audio Input: NegotiateL source code does not match\n"));
		iTestResult = EFail;
		}

	source->SourcePrimeL();
	source->SourcePlayL();
	source->SourcePauseL();
	source->SourceStopL();

	TMMFPrioritySettings prioritySettings;
	source->SetSourcePrioritySettings(prioritySettings);

	source->SourceThreadLogoff();

	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(dummySink);
	CleanupStack::PopAndDestroy(scheduler);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0204 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0204::CTestStep_MMF_SRSSNK_U_0204()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0204");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0204 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0204::~CTestStep_MMF_SRSSNK_U_0204()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0204
 *
 * Functional test of audio input
 * FillBuifferL test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0204::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create valid buffer.
	// NB: a valid buffer will be created by DevSound through asynchronous buffer creation.
	CMMFDescriptorBuffer* validBuffer = NULL;

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create an audio source.
	TBuf8<1> configDes;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);
	CleanupDeletePushL(source);

	// Test fill source buffer.
	User::LeaveIfError(source->SourceThreadLogon(*dummySink));
	source->NegotiateSourceL(*dummySink);
	source->SourcePrimeL();

	// Create a test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Invalid fill buffer.
	TAudioInputFillBufferTestStep testStep(source, testBuffer, dummySink);
	DoTest(testStep, _L("Audio Input: FillBufferL"), KErrNotSupported);
	CleanupStack::PopAndDestroy(testBuffer);


	// If we have buffer cycling switched on the DevSound, allocs will occur as
	// the buffers are switched over. This will cause the __MM_HEAP_MARKENDC() macro to fail.
	TUint expectedAllocs=0;

	// Valid fill buffer.
	TAudioInputFillBufferTestStep testStep2(source, validBuffer, dummySink);
	DoTest(testStep2, _L("Audio Input: FillBufferL"), KErrNone,expectedAllocs);

	// Valid fill buffer. (Make multiple requests)
	TAudioInputFillBuffer2TestStep testStep3(source, validBuffer, dummySink);
	DoTest(testStep3, _L("Audio Input: FillBufferL"), KErrNone,expectedAllocs);

	// XXX Valid fill buffer. (Make multiple requests at once)
	// removed below lines because the audio input currently doesn't support multiple requests.
	// TAudioInputFillBuffer3TestStep testStep4(source, validBuffer, dummySink);
	// DoTest(testStep4, _L("Audio Input: FillBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(source);

	// Repeat tests, creating a source each time.
	// Valid fill buffer.
	TAudioInputFillBuffer4TestStep testStep5(validBuffer, dummySink);
	DoTest(testStep5, _L("Audio Input: FillBufferL"), KErrNone);

	// Valid fill buffer. (Make multiple requests)
	TAudioInputFillBuffer5TestStep testStep6(validBuffer, dummySink);
	DoTest(testStep6, _L("Audio Input: FillBufferL"), KErrNone);

	// xxx Valid fill buffer. (Make multiple requests at once)
	// removed below lines because the audio input currently doesn't support multiple requests.
	// TAudioInputFillBuffer6TestStep testStep7(validBuffer, dummySink);
	// DoTest(testStep7, _L("Audio Input: FillBufferL"), KErrNone);

	// Valid fill buffer + pause + fill (should pass, we can now pause AudioInput and still request data already captured)
	TAudioInputFillBuffer7TestStep testStep8(validBuffer, dummySink);
	DoTest(testStep8, _L("Audio Input: FillBufferL"), KErrNone);

	// Valid fill buffer + stop + fill (should fail)
	TAudioInputFillBuffer8TestStep testStep9(validBuffer, dummySink);
	DoTest(testStep9, _L("Audio Input: FillBufferL"), KErrNotReady);

	CleanupStack::PopAndDestroy(2, scheduler); // dummySink, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0205 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0205::CTestStep_MMF_SRSSNK_U_0205()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0205");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0205 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0205::~CTestStep_MMF_SRSSNK_U_0205()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0205
 *
 * Functional test of audio input source.
 * BufferEmptiedL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0205::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create source file.
	TBuf8<1> configDes;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);
	CleanupDeletePushL(source);

	// Test BufferEmptiedL
	TRAPD(err, source->BufferEmptiedL(validBuffer));
	if (err == KErrNone)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(2, validBuffer); // source, validBuffer

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0206 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0206::CTestStep_MMF_SRSSNK_U_0206()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0206");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0206 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0206::~CTestStep_MMF_SRSSNK_U_0206()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0206
 *
 * Functional test of audio input source.
 * Source custom commands test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0206::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSource(KUidMmfAudioInput, dummyArgs));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSource, dummyArgs, dummyArgs);
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}
	else
		{
		INFO_PRINTF2(_L("File source SourceCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		}

	CleanupStack::PopAndDestroy(); // dummyController

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0207 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0207::CTestStep_MMF_SRSSNK_U_0207()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0207");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0207 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0207::~CTestStep_MMF_SRSSNK_U_0207()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0207
 *
 * Functional test of audio input source.
 * Misc. fns. test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0207::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create source file.
	TBuf8<1> configDes;
	iSource = STATIC_CAST(CMMFAudioInput*, MDataSource::NewSourceL(KUidMmfAudioInput, configDes));
	CleanupStack::PushL(iSource);

	User::LeaveIfError(iSource->SourceThreadLogon(*dummySink));
	iSource->SourcePrimeL();

	iSource->SoundDevice().InitializeL(*this, EMMFStatePlaying);
	CActiveScheduler::Start();	// wait for InitializeComplete callback

	//post InitializeComplete callback
	
	if(iError!=KErrNone)
		 iTestResult=EFail;
	

	CleanupStack::PopAndDestroy(3,scheduler); // dummySink, iSource, scheduler

	__MM_HEAP_MARKEND;
	return iTestResult;
		}

void CTestStep_MMF_SRSSNK_U_0207::InitializeComplete(TInt aError)
	{
	CActiveScheduler::Stop();
	iError=aError;
	if(iError==KErrNone)
		{
	 	TRAP(iError,iSource->SoundDevice().PlayInitL());
		}

	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0208 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0208::CTestStep_MMF_SRSSNK_U_0208()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0208");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0208 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0208::~CTestStep_MMF_SRSSNK_U_0208()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0208
 *
 * Functional test of audio output.
 * Misc. fns. test.
 *
 * @xxxx
 */
_LIT(KTest208FileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile1.wav");
_LIT(KTest208SWFileName, "c:\\mm\\mmf\\testfiles\\srssnk\\MMFTestFile2.wav");
// DevSound now calculates it's own buffer size depending on the Sample Rate,
// bytes per channel & channels etc :
const TUint KBytesPerSample8 = 2;
const TUint KChannels8 = 1;
const TUint KDevSoundDeltaFrameSize = 0x800; //2K
const TUint KAudioOutputDefaultFrameSize22K =
	(((22*1024*KBytesPerSample8*KChannels8)/4)+ (KDevSoundDeltaFrameSize-1)) &~ (KDevSoundDeltaFrameSize-1);
TVerdict CTestStep_MMF_SRSSNK_U_0208::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create a dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Create an audio sink.
// Test 1
	// Test create sink buffer.
	TCreateSinkBufferTestStep1 testStep(KUidMmfPtrBuffer, ETrue, NULL, KAudioOutputDefaultFrameSize22K, dummySource);


	DoTest(testStep, _L("Audio sink CreateSinkBuffer"), KErrNotSupported);

// Test 2
	TCreateSinkBufferTestStep2 testStep2(KUidMmfPtrBuffer, ETrue, NULL, KAudioOutputDefaultFrameSize22K);

	//DoTest(testStep2, _L("Audio sink CreateSinkBuffer"), KErrArgument);
	//DevSound WP  if we prime before the logon then the error should be KErrNotReady
	DoTest(testStep2, _L("Audio sink CreateSinkBuffer"), KErrNotReady);

// Test 3
	// Create a new sink.
	// This time configured correctly.
	// Create a real source.
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest208FileName;
	MDataSource* realSource = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(realSource);

	TUid KWavReadUid = { KMmfUidFormatWAVRead };
	CMMFFormatDecode* realFormat = CMMFFormatDecode::NewL(KWavReadUid, realSource);
	CleanupStack::PushL(realFormat);
	TCreateSinkBufferTestStep3 testStep3(KUidMmfPtrBuffer, ETrue, realFormat, KAudioOutputDefaultFrameSize22K, dummySource);

	DoTest(testStep3, _L("Audio sink CreateSinkBuffer"), KErrNone);

	CleanupStack::PopAndDestroy(2, realSource); // realFormat, realSource

// Test 4
	// Create a new sink.
	// This time configured correctly for software conversion.
	// Create a real source.
	// Initialise config data.
	configDes().iPath = KTest208SWFileName;
	realSource = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(realSource);

	realFormat = CMMFFormatDecode::NewL(KWavReadUid, realSource);
	CleanupStack::PushL(realFormat);

	TCreateSinkBufferTestStep4 testStep4(KUidMmfPtrBuffer, ETrue, realFormat, KAudioOutputDefaultFrameSize22K, dummySource);

	DoTest(testStep4, _L("Audio sink CreateSinkBuffer"), KErrNone);

	CleanupStack::PopAndDestroy(2, realSource); // realFormat, realSource

	CleanupStack::PopAndDestroy(dummySource);
	CleanupStack::PopAndDestroy(scheduler);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0209 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0209::CTestStep_MMF_SRSSNK_U_0209()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0209");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0209 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0209::~CTestStep_MMF_SRSSNK_U_0209()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0209
 *
 * Functional test of audio input
 * EmptyBuifferL test.
 *
 * @xxxx
 */
const TInt KDevSoundAllocs = 2;
TVerdict CTestStep_MMF_SRSSNK_U_0209::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Create a dummy source.
	CMMFDummySource* dummySource = new(ELeave) CMMFDummySource;
	CleanupStack::PushL(dummySource);

	// Set it to fill buffers it receives.
	dummySource->SetFillBuffer();

	// Create a real source.
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest208FileName;
	MDataSource* realSource = MDataSource::NewSourceL(KUidMmfFileSource, configDes);
	CleanupDeletePushL(realSource);

	TUid KWavReadUid = { KMmfUidFormatWAVRead };
	CMMFFormatDecode* realFormat = CMMFFormatDecode::NewL(KWavReadUid, realSource);
	CleanupStack::PushL(realFormat);

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create valid buffer.
	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create an audio sink.
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, configDes);
	CleanupDeletePushL(sink);

	// Test empty sink buffer.
	User::LeaveIfError(sink->SinkThreadLogon(*dummySource));
	sink->SinkPrimeL();
	sink->NegotiateL(*realFormat);

	// Create a test (invalid) buffer.
	CMMFBuffer* testBuffer = new(ELeave) CMMFTestBuffer;
	CleanupStack::PushL(testBuffer);

	// Invalid empty buffer.
	TAudioOutputEmptyBufferTestStep testStep(sink, testBuffer, dummySource);
	// Expect two allocs because CMMFDevSound::CBody::SetConfigL()
	// re-allocates it's buffer to match the sample rate of the input file:
	// one alloc is for the new CMMFDescriptorBuffer and the other is for
	// CMMFDescriptorBuffer's internal buffer.
	DoTest(testStep, _L("Audio Output: EmptyBufferL"), KErrNotSupported, KDevSoundAllocs);
	CleanupStack::PopAndDestroy(testBuffer);

	// Valid empty buffer.
	TAudioOutputEmptyBufferTestStep testStep2(sink, validBuffer, dummySource);
	testStep2.SetAsynchronousWrite();
	dummySource->SetActiveScheduler();
	DoTestStep(testStep2, _L("Audio Output: EmptyBufferL"), KErrNone, KDevSoundAllocs);

	// Abandon test if the previous step failed because the remaining tests rely on it's success.
	if (iTestResult != EPass)
		{
		CleanupStack::PopAndDestroy(6, dummySource); // sink, validBuffer, scheduler, realFormat, realSource, dummySource
		__MM_HEAP_MARKEND;
		return iTestResult;
		}

	testStep2.SetAsynchronousWrite();
	dummySource->SetActiveScheduler();
	DoTest(testStep2, _L("Audio Output: EmptyBufferL"), KErrNone, KDevSoundAllocs);

	// Valid empty buffer. (Make multiple requests)
	TAudioOutputEmptyBuffer2TestStep testStep3(sink, validBuffer, dummySource);
	DoTest(testStep3, _L("Audio Output: EmptyBufferL"), KErrNone, KDevSoundAllocs);

	// xxx Valid empty buffer. (Make multiple requests at once)
	// removed below lines because the audio output currently doesn't support multiple requests.
	// TAudioOutputFillBuffer3TestStep testStep4(sink, validBuffer, dummySource);
	// DoTest(testStep4, _L("Audio Output: EmptyBufferL"), KErrNone);

	CleanupStack::PopAndDestroy(sink);

	// These next tests create aand destroy their own sinks,
	// so the CMMFDevSound's buffer allocs shouldn't be counted (see above).

	// Repeat tests, creating a source each time.
	// Valid empty buffer.
	TAudioOutputEmptyBuffer4TestStep testStep5(validBuffer, dummySource, *realFormat);
	DoTest(testStep5, _L("Audio Output: EmptyBufferL"), KErrNone, 0);

	// Valid empty buffer. (Make multiple requests)
	//DevSound WP this test is invalid with HwDevice plugin devsound as you cannot pass
	//in your own buffer and ask the audio utput to empty it because the buffer
	//is owned by audio output.  ie the audio output is effectively active and
	//drives the low itself
	//	TAudioOutputEmptyBuffer5TestStep testStep6(validBuffer, dummySource, *realFormat);
	//	DoTest(testStep6, _L("Audio Output: EmptyBufferL"), KErrNone, 0);

	// xxx Valid empty buffer. (Make multiple requests at once)
	// removed below lines because the audio output currently doesn't support multiple requests.
	// TAudioOutputEmptyBuffer6TestStep testStep7(validBuffer, dummySource);
	// DoTest(testStep7, _L("Audio Output: EmptyBufferL"), KErrNone);

	// Repeat tests using software conversion.
	// Don't forget to call CreateSinkBufferL!

	CleanupStack::PopAndDestroy(5, dummySource); // validBuffer, scheduler, realFormat, realSource, dummySource

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0210 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0210::CTestStep_MMF_SRSSNK_U_0210()
: CTestStep_MMF_SRSSNK(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0210");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0210 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0210::~CTestStep_MMF_SRSSNK_U_0210()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0210
 *
 * Functional test of audio output source.
 * BufferFilledL() test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0210::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// This test fails if running a debug StdSourcesAndSinks dll.
	// So only perform the test on target.

	CMMFDescriptorBuffer* validBuffer = CMMFDescriptorBuffer::NewL(KTestBufferSize);
	CleanupStack::PushL(validBuffer);

	// Create sink.
	TBuf8<1> configDes;
	MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, configDes);
	CleanupDeletePushL(sink);

	// Test BufferFilledL
	sink->BufferFilledL(validBuffer);

	CleanupStack::PopAndDestroy(2, validBuffer); // sink, validBuffer

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0211 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0211::CTestStep_MMF_SRSSNK_U_0211()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0211");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0211 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0211::~CTestStep_MMF_SRSSNK_U_0211()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0211
 *
 * Functional test of audio output sink.
 * Sink custom commands test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0211::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSink(KUidMmfAudioOutput, dummyArgs));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSink, dummyArgs, dummyArgs);
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}
	else
		{
		INFO_PRINTF2(_L("Audio sink SinkCustomCommand: Expecting KErrNotSupported, actually got %d"), err);
		}

	CleanupStack::PopAndDestroy(); // dummyController

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0212 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0212::CTestStep_MMF_SRSSNK_U_0212()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0212");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0212 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0212::~CTestStep_MMF_SRSSNK_U_0212()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0212
 *
 * Functional test of audio output sink.
 * Misc. fns. test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0212::DoTestStepL()
	{
	// All the MDevSoundObserver fns. are untestable.
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0301 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0301::CTestStep_MMF_SRSSNK_U_0301()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0301");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0301 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0301::~CTestStep_MMF_SRSSNK_U_0301()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0301
 *
 * Functional test of audio output sink.
 * Misc. fns. test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0301::DoTestStepL()
	{
	__MM_HEAP_MARK;

	TPrimeStopTestStep testStep;
	DoFileTest(testStep, _L("PrimeStop"), KErrNone);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0302
 *
 * Functional test of audio output sink.
 * Misc. fns. test.
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0302::CTestStep_MMF_SRSSNK_U_0302()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0302");
	}

CTestStep_MMF_SRSSNK_U_0302::~CTestStep_MMF_SRSSNK_U_0302()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0302::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise thread active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	TPrimeStopSinkTestStep testStep;
	DoFileTest(testStep, _L("PrimeStopSinkTest"), KErrNone);

	CleanupStack::PopAndDestroy(scheduler);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0400
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

_LIT(KTestURLMpg, "http://www.symbian.com/clip/mpg");

CTestStep_MMF_SRSSNK_U_0400::CTestStep_MMF_SRSSNK_U_0400()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0400");
	}

CTestStep_MMF_SRSSNK_U_0400::~CTestStep_MMF_SRSSNK_U_0400()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0400::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));

	CleanupDeletePushL(source);

	TPtrC urlPtr = source->Url();
	if (urlPtr == _L("http://www.symbian.com/clip/mpg"))
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0405
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0405::CTestStep_MMF_SRSSNK_U_0405()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0405");
	}

CTestStep_MMF_SRSSNK_U_0405::~CTestStep_MMF_SRSSNK_U_0405()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0405::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TFourCC fourCCCode;
	TMediaId nullId;

	fourCCCode = source->SourceDataTypeCode(nullId);
	if (fourCCCode == KMMFFourCCCodeNULL)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0406
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0406::CTestStep_MMF_SRSSNK_U_0406()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0406");
	}

CTestStep_MMF_SRSSNK_U_0406::~CTestStep_MMF_SRSSNK_U_0406()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0406::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TFourCC fourCCCode;
	TMediaId nullMediaId;
	TInt returnValue = 0;

	returnValue = source->SetSourceDataTypeCode(fourCCCode, nullMediaId);
	if (returnValue == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0407
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0407::CTestStep_MMF_SRSSNK_U_0407()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0407");
	}

CTestStep_MMF_SRSSNK_U_0407::~CTestStep_MMF_SRSSNK_U_0407()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0407::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TMediaId nullId;

	TRAPD(err, source->FillBufferL(NULL, NULL, nullId));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0408
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0408::CTestStep_MMF_SRSSNK_U_0408()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0408");
	}

CTestStep_MMF_SRSSNK_U_0408::~CTestStep_MMF_SRSSNK_U_0408()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0408::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, source->BufferEmptiedL(NULL));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0409
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0409::CTestStep_MMF_SRSSNK_U_0409()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0409");
	}

CTestStep_MMF_SRSSNK_U_0409::~CTestStep_MMF_SRSSNK_U_0409()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0409::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TBool srcBuf = source->CanCreateSourceBuffer();
	if (!srcBuf)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0410
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0410::CTestStep_MMF_SRSSNK_U_0410()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0410");
	}

CTestStep_MMF_SRSSNK_U_0410::~CTestStep_MMF_SRSSNK_U_0410()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0410::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TMediaId nullId;
	TBool ref = ETrue;

	TRAPD(err, source->CreateSourceBufferL(nullId, ref));	// EABI warning removal
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0411
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0411::CTestStep_MMF_SRSSNK_U_0411()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0411");
	}

CTestStep_MMF_SRSSNK_U_0411::~CTestStep_MMF_SRSSNK_U_0411()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0411::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TMediaId nullId;
	CMMFDataBuffer* buf2 = CMMFDataBuffer::NewL();
	CleanupStack::PushL(buf2);
	TBool ref = ETrue;

	TRAPD(err, source->CreateSourceBufferL(nullId, *buf2, ref));	// EABI warning removal
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(4);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0412
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0412::CTestStep_MMF_SRSSNK_U_0412()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0412");
	}

CTestStep_MMF_SRSSNK_U_0412::~CTestStep_MMF_SRSSNK_U_0412()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0412::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	MAsyncEventHandler* s = NULL;
	TInt returnValue = source->SourceThreadLogon(*s);
	if (returnValue == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0413
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0413::CTestStep_MMF_SRSSNK_U_0413()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0413");
	}

CTestStep_MMF_SRSSNK_U_0413::~CTestStep_MMF_SRSSNK_U_0413()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0413::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	source->SourceThreadLogoff();

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0414
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0414::CTestStep_MMF_SRSSNK_U_0414()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0414");
	}

CTestStep_MMF_SRSSNK_U_0414::~CTestStep_MMF_SRSSNK_U_0414()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0414::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, source->NegotiateSourceL(*sink));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(4);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0415
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0415::CTestStep_MMF_SRSSNK_U_0415()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0415");
	}

CTestStep_MMF_SRSSNK_U_0415::~CTestStep_MMF_SRSSNK_U_0415()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0415::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TBool returnValue = ETrue;

	returnValue = source->SourceSampleConvert();
	if (returnValue == EFalse)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0416
 *
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0416::CTestStep_MMF_SRSSNK_U_0416()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0416");
	}

CTestStep_MMF_SRSSNK_U_0416::~CTestStep_MMF_SRSSNK_U_0416()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0416::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, source->SourcePrimeL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0417
 *
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0417::CTestStep_MMF_SRSSNK_U_0417()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0417");
	}

CTestStep_MMF_SRSSNK_U_0417::~CTestStep_MMF_SRSSNK_U_0417()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0417::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, source->SourcePlayL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0418
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0418::CTestStep_MMF_SRSSNK_U_0418()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0418");
	}

CTestStep_MMF_SRSSNK_U_0418::~CTestStep_MMF_SRSSNK_U_0418()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0418::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, source->SourcePauseL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0419
 * Functional Test of URL Source
 *
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0419::CTestStep_MMF_SRSSNK_U_0419()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0419");
	}

CTestStep_MMF_SRSSNK_U_0419::~CTestStep_MMF_SRSSNK_U_0419()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0419::DoTestStepL()
	{
	__MM_HEAP_MARK;

	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, source->SourceStopL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0420
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0420::CTestStep_MMF_SRSSNK_U_0420()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0420");
	}

CTestStep_MMF_SRSSNK_U_0420::~CTestStep_MMF_SRSSNK_U_0420()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0420::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);
	TMMFPrioritySettings settings;

	TRAPD(err, source->SetSourcePrioritySettings(settings));
	if (err == KErrNone)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0421
 * Functional Test of URL Source
 *
 *
 *
 * @xxxx
 */

CTestStep_MMF_SRSSNK_U_0421::CTestStep_MMF_SRSSNK_U_0421()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0421");
	}

CTestStep_MMF_SRSSNK_U_0421::~CTestStep_MMF_SRSSNK_U_0421()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0421::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSource(KUidMmfUrlSource, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSource, dummyArgs, dummyArgs);

	if (err == KErrNotSupported)
		{
		INFO_PRINTF2(_L("File source SourceCustomCommand: Expected KErrNotSupported, Error: %d"), err);
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(); // dummyController

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/*
 *    Sinks
 *
 *
 */

/**
 *
 * CTestStep_MMF_SRSSNK_U_0500
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0500::CTestStep_MMF_SRSSNK_U_0500()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0500");
	}

CTestStep_MMF_SRSSNK_U_0500::~CTestStep_MMF_SRSSNK_U_0500()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0500::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();
	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));

	CleanupDeletePushL(sink);

	TPtrC urlPtr = sink->Url();
	if (urlPtr == _L("http://www.symbian.com/clip/mpg"))
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0505
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0505::CTestStep_MMF_SRSSNK_U_0505()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0505");
	}

CTestStep_MMF_SRSSNK_U_0505::~CTestStep_MMF_SRSSNK_U_0505()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0505::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);
	TMediaId nullId;

	TFourCC fourCCCode = sink->SinkDataTypeCode(nullId);
	if (fourCCCode == KMMFFourCCCodeNULL)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0506
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0506::CTestStep_MMF_SRSSNK_U_0506()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0506");
	}

CTestStep_MMF_SRSSNK_U_0506::~CTestStep_MMF_SRSSNK_U_0506()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0506::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);
	TMediaId nullId;
	TFourCC fourCCCode;


	TInt returnValue = sink->SetSinkDataTypeCode(fourCCCode, nullId);
	if (returnValue == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0507
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0507::CTestStep_MMF_SRSSNK_U_0507()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0507");
	}

CTestStep_MMF_SRSSNK_U_0507::~CTestStep_MMF_SRSSNK_U_0507()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0507::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);
	TMediaId nullId;

	TRAPD(err, sink->EmptyBufferL(NULL, NULL, nullId));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0508
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0508::CTestStep_MMF_SRSSNK_U_0508()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0508");
	}

CTestStep_MMF_SRSSNK_U_0508::~CTestStep_MMF_SRSSNK_U_0508()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0508::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->BufferFilledL(NULL));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0509
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0509::CTestStep_MMF_SRSSNK_U_0509()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0509");
	}

CTestStep_MMF_SRSSNK_U_0509::~CTestStep_MMF_SRSSNK_U_0509()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0509::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TBool canCreate = sink->CanCreateSinkBuffer();
	if (canCreate == EFalse)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0510
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0510::CTestStep_MMF_SRSSNK_U_0510()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0510");
	}

CTestStep_MMF_SRSSNK_U_0510::~CTestStep_MMF_SRSSNK_U_0510()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0510::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);
	TMediaId nullId;
	TBool reference;

	TRAPD(err, sink->CreateSinkBufferL(nullId, reference));	// EABI warning removal
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0511
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0511::CTestStep_MMF_SRSSNK_U_0511()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0511");
	}

CTestStep_MMF_SRSSNK_U_0511::~CTestStep_MMF_SRSSNK_U_0511()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0511::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	MAsyncEventHandler* s = NULL;
	TInt returnValue = sink->SinkThreadLogon(*s);
	if (returnValue == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0512
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0512::CTestStep_MMF_SRSSNK_U_0512()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0512");
	}

CTestStep_MMF_SRSSNK_U_0512::~CTestStep_MMF_SRSSNK_U_0512()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0512::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->SinkThreadLogoff());
	if (err == KErrNone)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);


	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0513
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0513::CTestStep_MMF_SRSSNK_U_0513()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0513");
	}

CTestStep_MMF_SRSSNK_U_0513::~CTestStep_MMF_SRSSNK_U_0513()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0513::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	CMMFUrlSource* source = STATIC_CAST(CMMFUrlSource*, MDataSource::NewSourceL(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(source);

	TRAPD(err, sink->NegotiateL(*source));
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(4);


	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0514
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0514::CTestStep_MMF_SRSSNK_U_0514()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0514");
	}

CTestStep_MMF_SRSSNK_U_0514::~CTestStep_MMF_SRSSNK_U_0514()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0514::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->SinkPrimeL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0515
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0515::CTestStep_MMF_SRSSNK_U_0515()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0515");
	}

CTestStep_MMF_SRSSNK_U_0515::~CTestStep_MMF_SRSSNK_U_0515()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0515::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->SinkPlayL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0516
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0516::CTestStep_MMF_SRSSNK_U_0516()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0516");
	}

CTestStep_MMF_SRSSNK_U_0516::~CTestStep_MMF_SRSSNK_U_0516()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0516::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->SinkPauseL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0517
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0517::CTestStep_MMF_SRSSNK_U_0517()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0517");
	}

CTestStep_MMF_SRSSNK_U_0517::~CTestStep_MMF_SRSSNK_U_0517()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0517::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);

	TRAPD(err, sink->SinkStopL());
	if (err == KErrNotSupported)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0518
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0518::CTestStep_MMF_SRSSNK_U_0518()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0518");
	}

CTestStep_MMF_SRSSNK_U_0518::~CTestStep_MMF_SRSSNK_U_0518()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0518::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(KTestURLMpg, 1);
	CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

	CMMFUrlSink* sink = STATIC_CAST(CMMFUrlSink*, MDataSink::NewSinkL(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	CleanupDeletePushL(sink);
	TMMFPrioritySettings settings;

	TRAPD(err, sink->SetSinkPrioritySettings(settings));
	if (err == KErrNone)
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(3);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0519
 * Functional Test of URL Sink
 *
 *
 * @xxxx
 */
CTestStep_MMF_SRSSNK_U_0519::CTestStep_MMF_SRSSNK_U_0519()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0519");
	}

CTestStep_MMF_SRSSNK_U_0519::~CTestStep_MMF_SRSSNK_U_0519()
	{
	}

TVerdict CTestStep_MMF_SRSSNK_U_0519::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iTestResult = EFail;
	// Initialise config data.
	TMMFFileConfig configDes;
	configDes().iPath = KTest101FileName;

	TUid testControllerUid = { KMmfTestControllerUid };
	RMMFController dummyController;
	TMMFPrioritySettings prioritySettings;
	TMMFMessageDestinationPckg destination;
	User::LeaveIfError(dummyController.Open(testControllerUid, prioritySettings));
	CleanupClosePushL(dummyController);

	TBuf8<1> dummyArgs;
	User::LeaveIfError(dummyController.AddDataSink(KUidMmfUrlSink, configDes));
	TInt err = dummyController.CustomCommandSync(destination, KDescriptorSink, dummyArgs, dummyArgs);

	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("File source SourceCustomCommand: Expecting KErrNotSupported, Error: %d"), err);
		}
	else
		{
		iTestResult = EPass;
		}

	CleanupStack::PopAndDestroy(); // dummyController

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_MultipleStop constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_MultipleStop::CTestStep_MMF_SRSSNK_MultipleStop()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iSourceType = ESourceTypeFile;
	iTestStepName = _L("MM-MMF-SRSSNK-U-0520");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_MultipleStop destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_MultipleStop::~CTestStep_MMF_SRSSNK_MultipleStop()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_MultipleStop
 *
 * Functional test of file source.
 * Misc. functions test.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_MultipleStop::DoTestStepL()
	{
	__MM_HEAP_MARK;

	// Initialise config data.
	TMMFFileConfig configFile;
	configFile().iPath = KTest101FileName;

	TInt fileSize;
	OpenFileL(KTest101FileName, EFileRead);
	User::LeaveIfError(iFile.Size(fileSize));
	CloseFile();
	User::LeaveIfError(iFs.Connect());
	CleanupClosePushL(iFs);

	// Create source.
	MDataSource* source = CreateFileSourceL(iSourceType, iFs, KTest101FileName);
	CleanupDeletePushL(source);

	source->SourcePrimeL();
	source->SourcePlayL();
	source->SourcePauseL();

	// call stop twice make sure no leaves occur
	source->SourceStopL();
	source->SourceStopL();

	CleanupStack::PopAndDestroy(2, &iFs); // iFs, source

	__MM_HEAP_MARKEND;
	return iTestResult;
	}


/**
 *
 * CTestStep_MMF_SRSSNK_U_0600 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0600::CTestStep_MMF_SRSSNK_U_0600()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0600");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0600 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0600::~CTestStep_MMF_SRSSNK_U_0600()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0600
 *
 * CMMFFile::iFilePath and iFileDrive should be initialised for KMMFileHandleSourceUid
 * and KFileHandleUid
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0600::DoTestStepL()
	{
	__MM_HEAP_MARK;

	_LIT(KTestFileDrive, "C:");
	_LIT(KTestFilePath, "\\mm\\mmf\\testfiles\\srssnk\\");

	TFileName testFileName(KTest101FileName);
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Create a file source (KFileHandleUid)
	CMMFFile* source = static_cast<CMMFFile*>(CreateFileSourceL(ESourceTypeFileHandle, testFsSession, testFileName));
	CleanupStack::PushL(source);

	//Check that its FilePath & FileDrive have been populated
	if ((source->FilePath() != KTestFilePath) || (source->FileDrive() != KTestFileDrive))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Test failed for KFileHandleUid. Path=%S, Drive=%S"), &source->FilePath(), &source->FileDrive());
		}

	CleanupStack::PopAndDestroy(source);

	// Create a file handle source (KMMFileHandleSourceUid)
	source = static_cast<CMMFFile*>(CreateFileHandleSourceL(testFsSession, testFileName));
	CleanupStack::PushL(source);

	//Check that its FilePath & FileDrive have been populated
	if ((source->FilePath() != KTestFilePath) || (source->FileDrive() != KTestFileDrive))
		{
		iTestResult = EFail;
		INFO_PRINTF3(_L("Test failed for KMMFileHandleSourceUid. Path=%S, Drive=%S"), &source->FilePath(), &source->FileDrive());
		}

	CleanupStack::PopAndDestroy(2, &testFsSession); // testFsSession, source

	__MM_HEAP_MARKEND;
	return iTestResult;
	}



/**
 *
 * CTestStep_MMF_SRSSNK_U_0521 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0521::CTestStep_MMF_SRSSNK_U_0521()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0521");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0521 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0521::~CTestStep_MMF_SRSSNK_U_0521()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0521
 *
 * CR0854 - Provide access to the file data via the CData object
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0521::DoTestStepL()
	{
	__MM_HEAP_MARK;

	TFileName testFileName(KTest101FileName);
	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect());
	CleanupClosePushL(testFsSession);

	// Create a file source (KFileHandleUid)
	CMMFFile* source = static_cast<CMMFFile*>(CreateFileSourceL(ESourceTypeFileHandle, testFsSession, testFileName));
	CleanupStack::PushL(source);
	source->SourcePrimeL();
	CData *data = NULL;

	TInt err = source->Data(data);

	if (err != KErrNone)
		{
		iTestResult = EFail;
		INFO_PRINTF2(_L("Test failed err =%d"), err);
		}
	else
		{
		iTestResult = EPass;
		TInt value = 0;
		err = data->GetAttribute(EIsProtected, value);
		if (err != KErrNone || value)
			{
			INFO_PRINTF3(_L("Test failed err =%d value = %d"), err, value);
			iTestResult = EFail;
			}
		}

	CleanupStack::PopAndDestroy(2,&testFsSession); // source, testFsSession

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0522 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0522::CTestStep_MMF_SRSSNK_U_0522()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0522");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0522 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0522::~CTestStep_MMF_SRSSNK_U_0522()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0522
 *
 * CR0854 - Provide access to the file data via the CData object
 *
 * @xxxx
 */

TVerdict CTestStep_MMF_SRSSNK_U_0522::DoTestStepL()
	{
	__MM_HEAP_MARK;

	RFs testFsSession;
	User::LeaveIfError(testFsSession.Connect()) ;
	CleanupClosePushL(testFsSession);

	TFileName testFileName(KTest103FileName);
	CMMFFile* sink = static_cast<CMMFFile*>(CreateFileSinkL(ESinkTypeFile, testFsSession, testFileName));
	CleanupStack::PushL(sink);

	// Test that the file has been opened correctly.
	sink->SinkPrimeL();
	CData *data = NULL;
	TInt err = sink->Data(data);

	if(err != KErrNotSupported )
		{
		iTestResult = EFail;
		INFO_PRINTF2(_L("Test failed err =%d"), err);
		}
	else
		{
		iTestResult = EPass;
		}
	CleanupStack::PopAndDestroy(2,&testFsSession);

	__MM_HEAP_MARKEND;
	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0523 constructor
 *
 */
CTestStep_MMF_SRSSNK_U_0523::CTestStep_MMF_SRSSNK_U_0523()
	{
	iTestStepName = _L("MM-MMF-SRSSNK-U-0523");
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0523 destructor
 *
 */
CTestStep_MMF_SRSSNK_U_0523::~CTestStep_MMF_SRSSNK_U_0523()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0523
 *
 * PDEF102849 - Invalid Remaining Space value obtained for E:\ (memory card)
 *
 */
TVerdict CTestStep_MMF_SRSSNK_U_0523::DoTestStepL()
	{
	_LIT(KTestFileName, "E:\\unittest\\mmf\\srssnk\\MMFTestFile1.dat");

	__MM_HEAP_MARK;

	TInt64 freeSpace = 0;

	RFs fs;
	User::LeaveIfError(fs.Connect()) ;
	CleanupClosePushL(fs);

	// Create new e: drive to test fix if working on emulator
#ifdef __WINS__

	_LIT(KSubstPath, "C:\\");

	if (fs.SetSubst(KSubstPath, EDriveE) != KErrNone)
		{
		CleanupStack::PopAndDestroy(); // fs;
		__MM_HEAP_MARKEND;
		return EFail;
		}

#endif

	// Get the free space on the drive.
	TFileName testFileName(KTestFileName);
	TParsePtr parser(testFileName);
	TDriveUnit drive(parser.Drive());
	TVolumeInfo volInfo;

	if (fs.Volume(volInfo, drive) == KErrNone)
		{
		freeSpace = volInfo.iFree;
		}

	// Initialise config data.
	TMMFFileConfig configFile;
	configFile().iPath = KTestFileName;

	CMMFFile* source = static_cast<CMMFFile*>(CreateFileSourceL(ESourceTypeFile, fs, KTestFileName));
	CleanupStack::PushL(source);

	// do bytes free test
	TClipBytesFreeTestStep testStep(source, freeSpace);
	DoFileTest(testStep, _L("E: drive BytesFree"), KErrNone);

	CleanupStack::PopAndDestroy(2, &fs);

	__MM_HEAP_MARKEND;

	return iTestResult;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0524 constructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0524::CTestStep_MMF_SRSSNK_U_0524()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-SRSSNK-U-0524");
	iIsAllocTest = EFalse;
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0524 destructor
 *
 * @xxxx
 *
 */
CTestStep_MMF_SRSSNK_U_0524::~CTestStep_MMF_SRSSNK_U_0524()
	{
	}

/**
 *
 * CTestStep_MMF_SRSSNK_U_0524
 *
 * Test audio source misc. fns.
 *
 * @xxxx
 */
TVerdict CTestStep_MMF_SRSSNK_U_0524::DoTestStepL()
	{
	
	const TInt KDevSoundMaxFrameSize = 0x4000;  //16K from SoundDeviceBody.h

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create a dummy sink.
	CMMFDummySink* dummySink = new(ELeave) CMMFDummySink;
	CleanupStack::PushL(dummySink);

	// Create an audio source.
	TBuf8<1> configDes;
	MDataSource* source = MDataSource::NewSourceL(KUidMmfAudioInput, configDes);
	CleanupDeletePushL(source);

	TCreateSourceBuffer3TestStep testStep(ESourceTypeAudio, dummySink, KUidMmfDescriptorBuffer, ETrue, KDevSoundMaxFrameSize);
	DoTestStep(testStep, _L("Audio source CreateSourceBuffer1"), KErrNone);
	CleanupStack::PopAndDestroy(source);

	CleanupStack::PopAndDestroy(dummySink);
	CleanupStack::PopAndDestroy(scheduler);

	return iTestResult;
	}

TVerdict TCreateSourceBuffer3TestStep::DoTestStepL()
	{
	MDataSource* source = NULL;
	
	source = CreateSourceL(iSourceType, NULL);
	
	User::LeaveIfError(source->SourceThreadLogon(*this));

	source->SourcePrimeL();
	TRAPD(err,source->NegotiateSourceL(*iSink));
	
	return (err == KErrNone ? EPass : EFail);
	}
	
TInt TCreateSourceBuffer3TestStep::SendEventToClient(const TMMFEvent& aEvent)
	{
	if (aEvent.iErrorCode == KErrNoMemory)
		{
		CActiveScheduler::Stop();
		}		
	return KErrNone;
	}
