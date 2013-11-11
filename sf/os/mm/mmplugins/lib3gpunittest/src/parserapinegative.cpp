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
//

#include <mmf/common/mmferrors.h>
#include <caf/caf.h>
using namespace ContentAccess;
#include "tsu3gplibraryapinegative.h"

// --------------------------
// C3GPLibParser_Negative 
// --------------------------
//
void C3GPLibParser_Negative::CallAudioApiL(C3GPParse* aParse, TInt aExpectedError)
	{
	TInt err = KErrNone;
	// Retrieve audio stream attributes
	T3GPAudioType audioType;
	TInt framesPerSample;
	TUint length; 
	TUint avgBitRate;
	TUint timeScale;
		
	err = aParse->GetAudioProperties(audioType, length, framesPerSample, avgBitRate, timeScale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("aParse->GetAudioProperties")));

	TBool available;
	T3GPFrameType type = E3GPAudio;
	err = aParse->GetFrameAvailability(type, available);
	if (aExpectedError == KErrNotSupported)
		{
		// GetFrameAvailability will return KErrNotFound when there is no audio in the 3gp files
		User::LeaveIfError(CheckError(err, KErrNotFound, _L("GetFrameAvailability")));
		}
	else
		{
		User::LeaveIfError(CheckError(err, aExpectedError, _L("GetFrameAvailability")));
		}
	
	TUint size;
	err = aParse->GetAudioFramesSize(size);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetAudioFramesSize")));
	
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	
	if (aExpectedError == KErrNone)
		{
		buffer.CreateL(size); // Use size retrieved from GetVideoFrameSize
		}
	else
		{
		buffer.CreateL(KBufferSize); // size from above may not be assigned since this is negative tests.
		}
	
	TUint timeStampInMs; 
	TUint timeStampInTimescale;
	TInt returnedFrames;
	err = aParse->ReadAudioFrames(buffer, returnedFrames, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("ReadAudioFrames")));
	CleanupStack::PopAndDestroy(&buffer);
	
	TUint index;
	err = aParse->GetAudioSampleEntryIndex(index);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetAudioSampleEntryIndex")));
	
	T3GPQcelpStorageMode mode;
	err = aParse->GetQcelpStorageMode(mode);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetQcelpStorageMode")));
	}

void C3GPLibParser_Negative::CallVideoApiL(C3GPParse* aParse, TInt aExpectedError)
	{
	TInt err = KErrNone;
	// Retrieve video & audio stream attributes
	T3GPVideoType videoType;
	TUint length; 
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	
	err = aParse->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoProperties")));
	
	TBool available;
	T3GPFrameType type = E3GPVideo;
	err = aParse->GetFrameAvailability(type, available);
	if (aExpectedError == KErrNotSupported)
		{
		// GetFrameAvailability will return KErrNotFound when there is no video in the 3gp files
		User::LeaveIfError(CheckError(err, KErrNotFound, _L("GetFrameAvailability")));
		}
	else
		{
		User::LeaveIfError(CheckError(err, aExpectedError, _L("GetFrameAvailability")));
		}
	
	TUint size;	
	err = aParse->GetVideoFrameSize(size);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameSize")));
		
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	if (aExpectedError == KErrNone)
		{
		buffer.CreateL(size); // Use size retrieved from GetVideoFrameSize
		}
	else
		{
		buffer.CreateL(KBufferSize); // size from above may not be assigned since this is negative tests.
		}
	TBool keyFrame;
	TUint timeStampInMs; 
	TUint timeStampInTimescale;
	err = aParse->ReadVideoFrame(buffer, keyFrame, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("ReadVideoFrame")));
	CleanupStack::PopAndDestroy(&buffer);
	
	err = aParse->GetVideoTimestamp(timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoTimestamp")));

	TUint num;
	err = aParse->GetNumberOfVideoFrames(num);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetNumberOfVideoFrames")));
	
	TUint index;
	err = aParse->GetVideoSampleEntryIndex(index);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoSampleEntryIndex")));
	
	err = aParse->GetVideoFrameSize(index, size);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameSize")));
	
	err = aParse->GetVideoFrameStartTime(index, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameStartTime")));
	
	err = aParse->GetVideoFrameKeyType(index, keyFrame);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameKeyType")));
	
	TInt level;
	err = aParse->GetH263VideoLevel(level);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetH263VideoLevel")));
	
	T3GPFrameDependencies dependencies;
	err = aParse->GetVideoFrameDependencies(dependencies);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameDependencies")));
	
	TUint numberOfFrames = 2;
	RArray<T3GPFrameInfoParameters> array;
	err = aParse->GetVideoFrameProperties(index, numberOfFrames, array);
	array.Close();
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoFrameProperties")));	
	}

void C3GPLibParser_Negative::CallVideoDecoderInfoApiL(C3GPParse* aParse, TInt aExpectedError)
	{
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	
	TInt dsiSize;
	TInt err = aParse->GetVideoDecoderSpecificInfoSize(dsiSize);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoDecoderSpecificInfoSize")));
	
	if (aExpectedError == KErrNone)
		{
		buffer.CreateL(dsiSize);
		}
	else
		{
		buffer.CreateL(KBufferSize); // size from above may not be assigned since this is negative tests.
		}
	err = aParse->GetVideoDecoderSpecificInfo(buffer);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetVideoDecoderSpecificInfo")));
	
	CleanupStack::PopAndDestroy(&buffer);
	}

void C3GPLibParser_Negative::CallAudioDecoderInfoApiL(C3GPParse* aParse, TInt aExpectedError)
	{
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	TInt dsiSize;
	TInt err = aParse->GetAudioDecoderSpecificInfoSize(dsiSize);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetAudioDecoderSpecificInfoSize")));
	
	if (aExpectedError == KErrNone)
		{
		buffer.CreateL(dsiSize);
		}
	else
		{
		buffer.CreateL(KBufferSize); // size from above may not be assigned since this is negative tests.
		}
	err = aParse->GetAudioDecoderSpecificInfo(buffer);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetAudioDecoderSpecificInfo")));
	
	CleanupStack::PopAndDestroy(&buffer);
	}

// Retrieve user data atom. 
void C3GPLibParser_Negative::CallUDTApiL(C3GPParse* aParse, TUint32 aUdtType, TUint& aIndex, TInt aExpectedError, T3GPUdtaLocation aLocation)
	{
	TInt err = KErrNone;
	TInt udtSize;
	TUint tempIndex = aIndex;	// use the copy of the aIndex so it does not get overwritten by GetUserDataAtomSize

	err = aParse->GetUserDataAtomSize(aUdtType, aLocation, tempIndex, udtSize);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetUserDataAtomSize")));
	
	RBuf8 buffer;	
	CleanupClosePushL(buffer);
	
	if (aExpectedError == KErrNone && err == KErrNone)
		{
		buffer.CreateL(udtSize); // Use size retrieved from GetUserDataAtomSize
		}
	else
		{
		buffer.CreateL(KBufferSize); // size from above may not be assigned since this is negative tests.
		}
	err = aParse->GetUserDataAtom(aUdtType, aLocation, buffer, aIndex);
	CleanupStack::PopAndDestroy(&buffer);
	
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetUserDataAtom")));
	}

void C3GPLibParser_Negative::CallCommonApiL(C3GPParse* aParse, TInt aExpectedError)
	{
	TInt err = KErrNone;
	
	TUint size;
	TUint avgBitRate;
	err = aParse->GetContainerProperties(size, avgBitRate);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetContainerProperties")));
	
	T3GPFrameType type;
	err = aParse->C3GPParse::GetFrameType(type);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetFrameType")));
	
	TBool streamable;
	err = aParse->GetStreamable(streamable);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("GetStreamable")));
	
	TUint position = 0;
	TBool keyFrame = EFalse;
	TUint audioPosition;
	TUint videoPosition;
	err = aParse->Seek(position, keyFrame, audioPosition, videoPosition);
	User::LeaveIfError(CheckError(err, aExpectedError, _L("Seek")));
	}

// -----------------------------------------------------------------------------
// Create 3GP Parser to read zero size file content and call all APIs except
// asynchrounous read
// -----------------------------------------------------------------------------
//
C3GPLibParser_0101::C3GPLibParser_0101()
	{	
	}

TVerdict C3GPLibParser_0101::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, EmptyFileFailedCorruptL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0101::EmptyFileFailedCorruptL(C3GPParse* aParse)
	{
	// all api test with empty file using file path
	// When the parser is in use, calling parser open again should fail with KErrInUse
	ParserOpenFileL(aParse, KErrInUse);
	
	CallCommonApiL(aParse, KErrCorrupt);
	CallVideoApiL(aParse, KErrCorrupt);
	CallAudioApiL(aParse, KErrCorrupt);
	CallVideoDecoderInfoApiL(aParse, KErrCorrupt);
	CallAudioDecoderInfoApiL(aParse, KErrCorrupt);
	TUint32 udtType = 0x7469746c;
	TUint index;
	CallUDTApiL(aParse, udtType, index, KErrCorrupt);
	T3GPUdtaLocation location = E3GPUdtaAudioTrak;
	CallUDTApiL(aParse, udtType, index, KErrCorrupt, location);
	ParserCompleteL(aParse);	
	
	// all api test with empty file using file handle
	// When the parser is in use, calling parser open again should fail with KErrInUse
	ParserOpenFileHandleL(aParse, EFileRead | EFileShareReadersOnly, KErrInUse);

	CallCommonApiL(aParse, KErrCorrupt);
	CallVideoApiL(aParse, KErrCorrupt);
	CallAudioApiL(aParse, KErrCorrupt);
	CallVideoDecoderInfoApiL(aParse, KErrCorrupt);
	CallAudioDecoderInfoApiL(aParse, KErrCorrupt);
	CallUDTApiL(aParse, udtType, index, KErrCorrupt);
	CallUDTApiL(aParse, udtType, index, KErrCorrupt, location);
	ParserCompleteHandleL(aParse);

	// all api test with empty file using CAF
	// When the parser is in use, calling parser open again should fail with KErrInUse
	ParserOpenCafLC(aParse, KErrInUse);
	
	CallCommonApiL(aParse, KErrCorrupt);
	CallVideoApiL(aParse, KErrCorrupt);
	CallAudioApiL(aParse, KErrCorrupt);
	CallVideoDecoderInfoApiL(aParse, KErrCorrupt);
	CallAudioDecoderInfoApiL(aParse, KErrCorrupt);
	CallUDTApiL(aParse, udtType, index, KErrCorrupt);
	CallUDTApiL(aParse, udtType, index, KErrCorrupt, location);
	ParserCompleteCafL(aParse);
	
	// test open using buffer mode
	// When the parser is in use, calling parser open again should fail with KErrInUse
	ParserOpenL(aParse, KErrInUse);
	ParserCompleteL(aParse);
	// Call complete again, should pass with no error
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Initialise a 3GP file parse with an empty filename and file path pointing 
// at non-existent files
// -----------------------------------------------------------------------------
//
C3GPLibParser_0102::C3GPLibParser_0102()
	{	
	}

TVerdict C3GPLibParser_0102::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		// Initialise a 3GP file parse with an empty filename and it should fail with KErrUnderflow
		TInt err = parser->Open(KEmptyFileString);		
		if (err != KErrUnderflow)
			{
			ERR_PRINTF2(_L("parser->Open(KEmptyFileString) expects KErrUnderflow. Returns with %d"), err);
			SetTestStepResult(EFail);
			}
		
		// Setup 3GP Parser with a file path of a non-existing file.  This should fail with KErrAccessDenied.
		err = parser->Open(KInvalidFileString);
		if (err != KErrAccessDenied)
			{
			ERR_PRINTF2(_L("parser->Open(KEmptyFileString) expects KErrAccessDenied. Returns with %d"), err);
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// Create parser by passing in a file path of a file already opened in 
// EFileShareExclusive mode. 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0104::C3GPLibParser_0104()
	{	
	}

TVerdict C3GPLibParser_0104::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		// Open a valid mp4/3gp/3g2 file with EFileShareExclusive mode
		// Instantiate a 3GP parser object with the same file path already opened above
		FileOpenL(EFileShareExclusive);
		TInt err = parser->Open(iInputFile);
		if (err != KErrAccessDenied)
			{
			ERR_PRINTF2(_L("parser->Open(iInputFile) expects KErrAccessDenied. Returns with %d"), err);
			SetTestStepResult(EFail);
			}
		ParserCompleteL(parser);
		FileClose();

		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// Use 3GP Parser to parse file before it has been setup
// -----------------------------------------------------------------------------
//
C3GPLibParser_0106::C3GPLibParser_0106()
	{	
	}

TVerdict C3GPLibParser_0106::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);

		TRAPD(err, AllApiFailedNotReadyL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0106::AllApiFailedNotReadyL(C3GPParse* aParse)
	{
	CallCommonApiL(aParse, KErrNotReady);
	CallVideoApiL(aParse, KErrNotReady);
	CallAudioApiL(aParse, KErrNotReady);
	CallVideoDecoderInfoApiL(aParse, KErrNotReady);
	CallAudioDecoderInfoApiL(aParse, KErrNotReady);
	TUint32 udtType = 0x7469746c;
	TUint index;
	CallUDTApiL(aParse, udtType, index, KErrNotReady);
	
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	// create 0 size buffer for negative test when API is called before parse is opened
	buffer.CreateL(0); 
	TInt err = aParse->InsertData(buffer);
	User::LeaveIfError(CheckError(err, KErrNotReady, _L("aParse->InsertData")));
	CleanupStack::PopAndDestroy(&buffer);
	
	TInt num;
	err = aParse->GetNumBufferedBytes(num);
	User::LeaveIfError(CheckError(err, KErrNotReady, _L("aParse->InsertData")));
	}

// -----------------------------------------------------------------------------
// Use invalid frame type for all frame type API.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0107::C3GPLibParser_0107()
	{
	}

TVerdict C3GPLibParser_0107::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);

		TRAPD(err, InvalidFrameTypeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0107::InvalidFrameTypeL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	// Invalid type tests
	TInt invalidType = 10;	
	T3GPVideoType invalidVideoType = static_cast<T3GPVideoType>(invalidType);
	TUint length; 
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	 // should pass
	TInt err = aParse->GetVideoProperties(invalidVideoType, length, frameRate, avgBitRate, videoSize, timeScale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoProperties")));
	
	T3GPAudioType invalidAudioType = static_cast<T3GPAudioType>(invalidType);
	TInt framesPerSample;
	 // should pass
	err = aParse->GetAudioProperties(invalidAudioType, length, framesPerSample, avgBitRate, timeScale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParse->GetAudioProperties")));
	
	T3GPFrameType invalidFrameType = static_cast<T3GPFrameType>(invalidType);
	// should pass
	err = aParse->GetFrameType(invalidFrameType); 
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParse->GetAudioProperties")));
	
	T3GPQcelpStorageMode invalidMode = static_cast<T3GPQcelpStorageMode>(invalidType);
	 //Should pass
	err = aParse->GetQcelpStorageMode(invalidMode);
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParse->GetVideoProperties() invalid type")));

	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Use invalid frame index for video frame APIs and Get video frame properties for more 
// than the total number of video frames in the video clip
// -----------------------------------------------------------------------------
//
C3GPLibParser_0108::C3GPLibParser_0108()
	{	
	}

TVerdict C3GPLibParser_0108::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);

		TRAPD(err, AllInvalidFrameIndexL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0108::AllInvalidFrameIndexL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	TUint numOfVideoFrames;
	TInt err = aParse->GetNumberOfVideoFrames(numOfVideoFrames);
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParse->GetNumberOfVideoFrames()")));
	
	// Invalid index. 
	TUint size;
	err = aParse->GetVideoFrameSize(numOfVideoFrames + 100, size);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameSize() invalid index")));
	
	err = aParse->GetVideoFrameSize((KMaxTUint-15), size);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameSize() negative index")));
	
	TUint timeStampInMs;
	TUint timeStampInTimescale;
	err = aParse->GetVideoFrameStartTime(numOfVideoFrames + 10, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameStartTime() invalid index")));
	
	err = aParse->GetVideoFrameStartTime((KMaxTUint-15), timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameStartTime() negative index")));
	
	TBool keyFrame;
	err = aParse->GetVideoFrameKeyType(numOfVideoFrames + 10, keyFrame);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameKeyType() invalid index")));
	
	err = aParse->GetVideoFrameKeyType((KMaxTUint-15), keyFrame);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameKeyType() negative index")));
	
	TUint numberOfFrames = 2;
	RArray<T3GPFrameInfoParameters> array;
	CleanupClosePushL(array);

	err = aParse->GetVideoFrameProperties(numOfVideoFrames + 10, numberOfFrames, array);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameProperties() invalid index")));
	
	// Get video frame properties for more than the total number of video frames in the video clip
	err = aParse->GetVideoFrameProperties(0, numOfVideoFrames + 10, array);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameProperties() invalid number of frame")));
	
	err = aParse->GetVideoFrameProperties((KMaxTUint-1), numOfVideoFrames, array);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameProperties() with negative index")));
	
	err = aParse->GetVideoFrameProperties(15, (KMaxTUint-10), array);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameProperties() with negative numOfVideoFrames")));

	err = aParse->GetVideoFrameProperties((KMaxTUint-15), numOfVideoFrames + 10, array);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("aParse->GetVideoFrameProperties() with negative index and invalid numOfVideoFrames")));

	CleanupStack::PopAndDestroy(&array);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Instantiate a 3GP Parser and Call all Parser API that uses frame type as 
// input parameter with invalid frame types 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0109::C3GPLibParser_0109()
	{	
	}

TVerdict C3GPLibParser_0109::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);

		TRAPD(err, InvalidFrameTypePanicL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0109::InvalidFrameTypePanicL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	TInt invalidType = 10;	
	TBool available;
	T3GPFrameType invalidFrameType = static_cast<T3GPFrameType>(invalidType);
	 //Should panic
	aParse->GetFrameAvailability(invalidFrameType, available);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Get user data atom by passing in an invalid user atom location.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0110::C3GPLibParser_0110()
	{	
	}

TVerdict C3GPLibParser_0110::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);

		TRAPD(err, InvalidUdtLocationL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0110::InvalidUdtLocationL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	// Invalid type tests
	TInt invalidType = 10;	
	TUint index;
	TUint32 udtType = 0x7469746c;
	T3GPUdtaLocation invalidLocation = static_cast<T3GPUdtaLocation>(invalidType);	
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	buffer.CreateL(KBufferSize);	
	 //Should panic
	aParse->GetUserDataAtom(udtType, invalidLocation, buffer, index);
	CleanupStack::PopAndDestroy(&buffer);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Retrieve information when that info is not in the file or the info is not
// supported by the library
// -----------------------------------------------------------------------------
//
C3GPLibParser_0111::C3GPLibParser_0111()
	{	
	}

TVerdict C3GPLibParser_0111::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, AllVideoAudioApiFailedL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0111::AllVideoAudioApiFailedL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	TInt expectedErr = KErrNone;
	TUint32 udtType = 0x7469746c;
	TUint index;
	T3GPUdtaLocation location = E3GPUdtaVideoTrak;  
	if (iTestSection.Compare(KAudioOnly) == 0 || iTestSection.Compare(KUnsupportedVideo) == 0)
		{
		CallVideoApiL(aParse, KErrNotSupported);
		CallVideoDecoderInfoApiL(aParse, KErrNotSupported);
		}
	else if (iTestSection.Compare(KVideoOnly) == 0 || iTestSection.Compare(KUnsupportedAudioCodec) == 0 ||
			iTestSection.Compare(KUnsupportedAudioStream) == 0)
		{
		CallAudioApiL(aParse, KErrNotSupported);
		CallAudioDecoderInfoApiL(aParse, KErrNotSupported);
		
		location = E3GPUdtaAudioTrak;
		}
	else if (iTestSection.Compare(KWmaFile) == 0)
		{
		expectedErr = KErrCorrupt;
		CallVideoDecoderInfoApiL(aParse, expectedErr);
		CallUDTApiL(aParse, udtType, index, expectedErr, location); 
		}
	else if (iTestSection.Compare(KMisMatch) == 0)
		{
		// The test file doesn't contain MPEG-4 / AVC video stream. The decoder info should fail. 
		CallVideoDecoderInfoApiL(aParse, KErrNotSupported);
		CallUDTApiL(aParse, udtType, index, KErrNotFound, location); 
		}
	else if (iTestSection.Compare(KNoAudioNoVideo) == 0)
		{
		expectedErr = KErrNotSupported;
		CallVideoApiL(aParse, expectedErr);
		CallVideoDecoderInfoApiL(aParse, expectedErr);
		CallAudioApiL(aParse, expectedErr);
		CallAudioDecoderInfoApiL(aParse, expectedErr);
		CallCommonApiL(aParse, KErrGeneral);
		}
	if (iTestSection.Compare(KWmaFile) == 0 || iTestSection.Compare(KMisMatch) == 0)
		{
		CallVideoApiL(aParse, expectedErr);
		CallAudioApiL(aParse, expectedErr);
		CallAudioDecoderInfoApiL(aParse, expectedErr);
		CallCommonApiL(aParse, expectedErr);
		}
	else
		{
		CallUDTApiL(aParse, udtType, index, KErrNotFound, location); 
		}
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Seek to an invalid position of the file.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0119::C3GPLibParser_0119()
	{
	}

TVerdict C3GPLibParser_0119::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, SeekTestL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0119::SeekTestL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	TUint audioPos;
	TUint videoPos;
	TUint timeStampInMs = 0;
	
	// Seek to position 0.
	TInt err = KErrNone; 
	err = aParse->Seek(timeStampInMs, ETrue, audioPos, videoPos);
	if (iTestSection.Compare(KNoAudioNoVideo) == 0)
		{
		User::LeaveIfError(CheckError(err, KErrGeneral, 
				_L("aParser->Seek(timeStampInMs, ETrue, audioPos, videoPos) with file without audio nor video")));
		err = aParse->Seek(timeStampInMs, EFalse, audioPos, videoPos);
		User::LeaveIfError(CheckError(err, KErrGeneral, 
				_L("aParser->Seek(timeStampInMs, EFalse, audioPos, videoPos) with file without audio nor video")));
		}	
	else
		{
		User::LeaveIfError(CheckError(err, KErrNone, 
				_L("aParser->Seek(timeStampInMs, ETrue, audioPos, videoPos) with file with audio/video")));
		err = aParse->Seek(timeStampInMs, EFalse, audioPos, videoPos);
		User::LeaveIfError(CheckError(err, KErrNone, 
				_L("aParser->Seek(timeStampInMs, EFalse, audioPos, videoPos) with file with audio/video")));
		}
	
	if (iTestSection.Compare(KNoAudioNoVideo) != 0)
		{
		// Retrieve video & audio stream length
		TUint videoLength; 
		TUint audioLength; 
		TInt videoError;

		err = GetClipProperties(aParse, videoError, videoLength, audioLength);
		if (iTestSection.Compare(KAudioOnly) == 0)
			{
			// audio only, GetVideoProperties should fail with KErrNotSupported. Set video length to 0
			User::LeaveIfError(CheckError(videoError, KErrNotSupported, _L("GetClipProperties(aParse, videoError, videoLength, audioLength))")));
			videoLength = 0;
			}
		else if (iTestSection.Compare(KVideoOnly) == 0)
			{
			// video only, GetAudioProperties should fail with KErrNotSupported. Set audio length to 0
			User::LeaveIfError(CheckError(err, KErrNotSupported, _L("GetClipProperties(aParse, videoError, videoLength, audioLength)")));
			audioLength = 0;
			}
		else 
			{
			// All other files contains audio data
			User::LeaveIfError(CheckError(videoError, KErrNone, _L("GetClipProperties(aParse, videoError, videoLength, audioLength)")));
			User::LeaveIfError(CheckError(err, KErrNone, _L("GetClipProperties(aParse, videoError, videoLength, audioLength)")));
			}
		
		if (audioLength >= videoLength)
			{
			timeStampInMs = audioLength + 10000;
			}
		else
			{
			timeStampInMs = videoLength + 10000;
			}
		
		// Seek returns KErrNone even thought the position is more than the audio/video length. AudioPos/videoPos should
		// be the the last audio/video position
		err = aParse->Seek(timeStampInMs, EFalse, audioPos, videoPos);
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->Seek() to an invalid position")));
		
		timeStampInMs = 0;
		err = aParse->Seek(timeStampInMs, EFalse, audioPos, videoPos);
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->Seek(0, EFalse, audioPos, videoPos)")));
		}
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Read Video Decoder Info, video frame dependencies and user atom when these 
// info is not in the file
// -----------------------------------------------------------------------------
//
C3GPLibParser_0120::C3GPLibParser_0120()
	{
	}

TVerdict C3GPLibParser_0120::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		TInt readBufferSize = 1000;
		C3GPParse* parser = C3GPParse::NewL(readBufferSize);
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoAttributesL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0120::VideoAttributesL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	// Read Video Decoder Info from a mp4/3gp/3g2 file containing H263 video.
	CallVideoDecoderInfoApiL(aParse, KErrNotSupported);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Instantiate parser with  read buffer size < 0. 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0126::C3GPLibParser_0126()
	{	
	}

TVerdict C3GPLibParser_0126::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		TInt readBufferSize = -1;
		C3GPParse* parser = C3GPParse::NewL(readBufferSize);
		if (parser)
			{
			SetTestStepResult(EFail);
			delete parser;
			parser = NULL;
			}		
		}
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// Retrieve video and audio info using empty buffer and size 1 buffer for all 
// buffer related API.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0127::C3GPLibParser_0127()
	{
	}

TVerdict C3GPLibParser_0127::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		TInt readBufferSize = 1000;
		C3GPParse* parser = C3GPParse::NewL(readBufferSize);
		CleanupStack::PushL(parser);
		
		TRAPD(err, TestEmptyBufferL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0127::TestEmptyBufferL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	// Create an empty buffer
	TInt expectedErr = KErrOverflow;
	RBuf8 buffer;	
	CleanupClosePushL(buffer);
	
	TUint32 udtType = 0x6d657461; // The udt type of test file is 'meta'
	TUint index = 0;
	T3GPUdtaLocation location = E3GPUdtaVideoTrak;
	
	if (iTestSection.Compare(KAudioMore) != 0)
		{
		buffer.CreateL(1);
		}
	else
		{
		buffer.CreateL(0);
		}
	
	TInt err = aParse->GetUserDataAtom(udtType, location, buffer, index);
	User::LeaveIfError(CheckError(err, expectedErr, _L("GetUserDataAtom")));
	
	err = aParse->GetVideoDecoderSpecificInfo(buffer);
	User::LeaveIfError(CheckError(err, expectedErr, _L("GetVideoDecoderSpecificInfo")));
	
	err = aParse->GetAudioDecoderSpecificInfo(buffer);
	User::LeaveIfError(CheckError(err, expectedErr, _L("GetAudioDecoderSpecificInfo")));
	
	err = aParse->InsertData(buffer);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("InsertData")));
	
	TUint timeStampInMs;
	TUint timeStampInTimescale;
	TInt returnedFrames;
	err = aParse->ReadAudioFrames(buffer, returnedFrames, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, expectedErr, _L("ReadAudioFrames")));
	
	TBool keyframe;
	err = aParse->ReadVideoFrame(buffer, keyframe, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, expectedErr, _L("ReadVideoFrame")));
	
	iSchedulerWait = new (ELeave) CActiveSchedulerWait;
	if(!iSchedulerWait)
		{
		User::Leave(KErrNoMemory);
		}

	iWaitForNotification = ETrue;
	aParse->ReadVideoFrame(*this, buffer);
	
	if (iWaitForNotification)
	    {
		iSchedulerWait->Start();
	    }
	err = iAsyncError;
	User::LeaveIfError(CheckError(err, expectedErr, _L("ReadVideoFrame async")));
	
	iWaitForNotification = ETrue;
	aParse->ReadAudioFrames(*this, buffer);
	CleanupStack::PopAndDestroy(&buffer);
	
	if (iWaitForNotification)
	    {
		iSchedulerWait->Start();
	    }
	err = iAsyncError;
	User::LeaveIfError(CheckError(err, expectedErr, _L("ReadAudioFrames async")));
	
	if ( iSchedulerWait->IsStarted() )
        {
        iSchedulerWait->AsyncStop();
	    }
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Async read before the parser has been setup.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0130::C3GPLibParser_0130()
	{
	}

TVerdict C3GPLibParser_0130::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0130::doTestStepPostambleL()
	{
	C3GPLibParser_base::doTestStepPostambleL();
	if( iScheduler )
		{
		INFO_PRINTF1(_L("delete iScheduler"));
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		iScheduler = NULL;
		}

	return TestStepResult();
	}

TVerdict C3GPLibParser_0130::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameAsyncWoOpenL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0130::ReadFrameAsyncWoOpenL(C3GPParse* aParser)
	{
	TInt err = ReadAudioVideoFrameAsync(aParser, ETrue);
	User::LeaveIfError(CheckError(err, KErrNotReady, _L("ReadAudioVideoFrameAsync audio")));

	err = ReadAudioVideoFrameAsync(aParser, EFalse);
	User::LeaveIfError(CheckError(err, KErrNotReady, _L("ReadAudioVideoFrameAsync video")));
	
	aParser->CancelReadFrame();
	}

// -----------------------------------------------------------------------------
// Async read negative test after the parser has been setup.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0131::C3GPLibParser_0131()
	{
	}

TVerdict C3GPLibParser_0131::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0131::doTestStepPostambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPostambleL();
	if( iScheduler )
		{
		INFO_PRINTF1(_L("delete iScheduler"));
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		iScheduler = NULL;
		}

	return TestStepResult();
	}

TVerdict C3GPLibParser_0131::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameAsyncAfterOpenL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0131::ReadFrameAsyncAfterOpenL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	
	TInt videoError;
	TUint videoLength;
	TUint audioLength;
	TInt AudioErr = GetClipProperties(aParser, videoError, videoLength, audioLength);
	
	if (AudioErr != KErrNone && AudioErr != KErrNotSupported && AudioErr == KErrCorrupt)
		{
		SetTestStepError(AudioErr);
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("audioErr %d;  "), AudioErr);
			}
		ParserCompleteL(aParser);
		return;
		}
	if (videoError != KErrNone && videoError != KErrNotSupported && videoError != KErrCorrupt)
		{
		SetTestStepError(videoError);
		ERR_PRINTF2(_L("videoError %d;  "), videoError);
		ParserCompleteL(aParser);
		return;
		}
	
	TInt err = KErrNone;
	
	// Call read audio frame async when there is no audio. Expects error
	if (AudioErr == KErrNotSupported || AudioErr == KErrCorrupt)
		{
		err = ReadAudioVideoFrameAsync(aParser, ETrue);
		}
	
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("ReadAudioVideoFrameAsync audio err %d "), err);
			}
		SetTestStepError(err);
		}

	// Call read video frame async when there is no video. Expects error
	if (videoError == KErrNotSupported && AudioErr != KErrNotSupported)
		{
		err = ReadAudioVideoFrameAsync(aParser, EFalse);
		if (err != KErrNone)
			{
			if (!ShouldRunOOMTest())
				{
				ERR_PRINTF2(_L("ReadAudioVideoFrameAsync video err %d "), err);
				}
			SetTestStepError(err);
			}
		}
	
	// call Cancel Async read  when there is no outstanding async read request
	aParser->CancelReadFrame();
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Retrieve video & audio frames async in buffer mode.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0136::C3GPLibParser_0136()
	{
	}

TVerdict C3GPLibParser_0136::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0136::doTestStepPostambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPostambleL();
	if( iScheduler )
		{
		INFO_PRINTF1(_L("delete iScheduler"));
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		iScheduler = NULL;
		}

	return TestStepResult();
	}

TVerdict C3GPLibParser_0136::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameAsyncInBufferModeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0136::OpenFileInBufferModeL(C3GPParse& aParser)
	{
	// open parser in buffer mode
	User::LeaveIfError(aParser.Open());
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	User::LeaveIfError(file.Open(fs, iInputFile, EFileRead | EFileStream));
	CleanupClosePushL(file);
	
	RBuf8 buffer;
	buffer.CreateL(1000);
	CleanupClosePushL(buffer);

	TInt bufferRead = 0;
	do 
		{
		User::LeaveIfError(file.Read(buffer));
		bufferRead = buffer.Length();
		if (bufferRead > 0)
			{
			User::LeaveIfError(aParser.InsertData(buffer));
			}
		} while (bufferRead > 0);
			
	CleanupStack::PopAndDestroy(&buffer);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);			
	}

void C3GPLibParser_0136::ReadFrameAsyncInBufferModeL(C3GPParse* aParser)
	{
	ASSERT(aParser);
	
	// buffer mode
	OpenFileInBufferModeL(*aParser);
	
	// Get Video & Audio properties to ensure both source exist
	T3GPVideoType videoType;
	T3GPAudioType audioType;	
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	TInt framesPerSample;
	TUint length;	
	
	User::LeaveIfError(aParser->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale));	
	User::LeaveIfError(aParser->GetAudioProperties(audioType, length, framesPerSample, avgBitRate, timeScale));
	
	TInt audioFrameErr = ReadAudioVideoFrameAsync(aParser, ETrue);
	TInt videoFrameErr = ReadAudioVideoFrameAsync(aParser, EFalse);
	if (audioFrameErr != videoFrameErr)
		{
		ERR_PRINTF1(_L("Unexpected error returned"));
		SetTestStepResult(EInconclusive);
		}
	else 
		{
		SetTestStepError(audioFrameErr);
		}
	
	// call Cancel Async read  when there is no outstanding async read request
	aParser->CancelReadFrame();
	
	ParserCompleteL(aParser);
	}


// -----------------------------------------------------------------------------
// Get frame type using file path
// -----------------------------------------------------------------------------
//
C3GPLibParser_0135::C3GPLibParser_0135()
	{	
	}


TVerdict C3GPLibParser_0135::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetFrameTypeNegativeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0135::GetFrameTypeNegativeL(C3GPParse* aParser)
	{
	ParserOpenFileL(aParser);
	
	T3GPFrameType type;
	TInt err = KErrNone;
	TInt videoError;
	TUint videoLength;
	TUint audioLength;
	TInt AudioErr = GetClipProperties(aParser, videoError, videoLength, audioLength);

	if ((AudioErr != KErrNone && AudioErr != KErrNotSupported))
		{
		SetTestStepError(AudioErr);
		ERR_PRINTF2(_L("audioErr %d;  "), AudioErr);
		ParserCompleteL(aParser);
		return;
		}
	if ((videoError != KErrNone && videoError != KErrNotSupported))
		{
		SetTestStepError(videoError);
		ERR_PRINTF2(_L("videoError %d;  "), videoError);
		ParserCompleteL(aParser);
		return;
		}
	
	if (AudioErr == KErrNone)
		{
		// read all audio frames and then check the next frame type
		err = ReadAudioVideoFrame(aParser, ETrue, ETrue);
		User::LeaveIfError(CheckError(err, KErrNotFound, _L("ReadAudioVideoFrame")));
		}
	
	if (videoError == KErrNone )
		{	
		// read all video frames
		err = ReadAudioVideoFrame(aParser, ETrue);
		User::LeaveIfError(CheckError(err, KErrNotFound, _L("ReadAudioVideoFrame")));
		}

	if (err == KErrNotFound && (AudioErr == KErrNone || videoError == KErrNone))
		{
		err = aParser->GetFrameType(type);
		}
	
	if (err != KErrNone)
		{
		SetTestStepError(err);
		}
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Get audio/video attributes of a large file (>2GB) using 32bit APIs
// -----------------------------------------------------------------------------
//
C3GPLibParser_0137::C3GPLibParser_0137()
	{	
	}

TVerdict C3GPLibParser_0137::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetVideoAudioAttributesL(parser));
		if (err != KErrNone)
			{
			SetTestStepError(err); 
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0137::GetVideoAudioAttributesL(C3GPParse* aParser)
	{
	// open parser in 32bit file handle
	ParserOpenFileHandleL(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteHandleL(aParser);
	}

// -----------------------------------------------------------------------------
// Get audio/video frames of a large file (>2GB) using 32bit APIs
// -----------------------------------------------------------------------------
//
C3GPLibParser_0138::C3GPLibParser_0138()
	{	
	}

TVerdict C3GPLibParser_0138::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameL(parser));
		if (err != KErrNone)
			{
			SetTestStepError(err); 
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0138::ReadFrameL(C3GPParse* aParser)
	{
	// open parser in 32bit file handle
	ParserOpenFileHandleL(aParser);
	User::LeaveIfError(DoReadFrame(aParser));
	ParserCompleteHandleL(aParser);
	}

// -----------------------------------------------------------------------------
// Get audio/video frames of a large file (>2GB) using 32bit APIs
// -----------------------------------------------------------------------------
//
C3GPLibParser_0139::C3GPLibParser_0139()
	{	
	}

TVerdict C3GPLibParser_0139::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0139::doTestStepPostambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPostambleL();
	if( iScheduler )
		{
		INFO_PRINTF1(_L("delete iScheduler"));
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		iScheduler = NULL;
		}

	return TestStepResult();
	}

TVerdict C3GPLibParser_0139::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameAsyncL(parser));
		if (err != KErrNone)
			{
			SetTestStepError(err); 
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0139::ReadFrameAsyncL(C3GPParse* aParser)
	{	
	ParserOpenFileHandleL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL() using file handler"));
		}
	ParserCompleteHandleL(aParser);
	}

TInt C3GPLibParser_0139::DoReadFrameAsync(C3GPParse* aParser, TBool aCancel)
	{    
    TInt err = KErrNone;
    
    if (!aCancel)
    	{
    	err = ReadAudioVideoFrameAsync(aParser, ETrue);
    	if (err == KErrNone)
    		{
    		err = ReadAudioVideoFrameAsync(aParser, EFalse);
    		} 
    	}
    else
    	{
    	err = ReadAudioVideoFrameAsync(aParser, EFalse, ETrue);
    	}
 
    return err;
    }

// -----------------------------------------------------------------------------
// Test the retrieval of Get video / audio properties when the properties are filled
// in but the file does not contain any actual data
// -----------------------------------------------------------------------------
//
TVerdict C3GPSingleDataSourceTest::doTestStepPreambleL()
	{
	TPtrC inputFile;
	if(!GetStringFromConfig(ConfigSection(),_L("inputFile"), inputFile))
		{		
		SetTestStepResult(EFail);
		}
	else 
		{
		iParser = C3GPParse::NewL();
		User::LeaveIfError(iParser->Open(inputFile));			
		SetTestStepResult(EPass);
		SetTestStepError(KErrNone);
		}
		
	return TestStepResult();
	}

TVerdict C3GPSingleDataSourceTest::doTestStepPostambleL()
	{
	if (iParser)
	{
		iParser->Complete();
		delete iParser;		
		
	}
	
	return TestStepResult();	
	}
	
	
TVerdict C3GPSingleDataSourceTest::doTestStepL()
	{
	if (TestStepResult() != EPass)	
		{
		return TestStepResult();		
		}
	
	if (ConfigSection() == _L("MM-3GP-PARSE-U-0112-CP-2"))
		{
		// for a video only file containing audio properties but no real audio data		
		T3GPVideoType videoType;
		TUint length; 
		TReal frameRate;
		TUint avgBitRate;
		TSize videoSize;
		TUint timeScale;
		TInt err = iParser->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale);
		if (err == KErrNone)
			{
			T3GPAudioType audioType;
			TInt framesPerSample;
			TUint length; 
			TUint avgBitRate;
			TUint timeScale;
				
			err = iParser->GetAudioProperties(audioType, length, framesPerSample, avgBitRate, timeScale);
			if (err == KErrNone)
				{
				if (audioType == E3GPMpeg4Audio)
					{
					TInt size = 0;
					err = iParser->GetAudioDecoderSpecificInfoSize(size);
					if (err != KErrNone)
						{						
						if (!ShouldRunOOMTest())
							{						
							ERR_PRINTF2(_L("C3GPParse::GetAudioDecoderSpecificInfoSize returns %d."), err);						
							}
						User::Leave(err);
						}
					}

				TBool available = EFalse;	
				err = iParser->GetFrameAvailability(E3GPAudio, available);				
				if (err != KErrNone && err != KErrNotFound)
					{	
					if (!ShouldRunOOMTest())
						{										
						ERR_PRINTF2(_L("C3GPParse::GetFrameAvailability returns %d."), err);
						}
					User::Leave(err);
					}
					
				TUint frameSize = 0;
				err = iParser->GetAudioFramesSize(frameSize);
				if (err == KErrNone)
					{
					// Expect retrieval of audio properties to fail, doesn't matter what error code 
					// is returned
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF1(_L("C3GPParse::GetAudioFramesSize expects to fail but does not"));
						}
					SetTestStepResult(EFail);
					}
					
				if (TestStepResult() == EPass)	
					{
					TUint frameIndex = 0;					
					err = iParser->GetAudioSampleEntryIndex(frameIndex);
					if (err == KErrNone)
						{
						// Expect retrieval of audio properties to fail, doesn't matter what error code 
						// is returned
						if (!ShouldRunOOMTest())
							{						
							ERR_PRINTF1(_L("C3GPParse::GetAudioSampleEntryIndex expects to fail but does not"));
							}
						SetTestStepResult(EFail);
						}						
					}
				}
			else
				{
				if (!ShouldRunOOMTest())
					{				
					ERR_PRINTF2(_L("C3GPParse::GetAudioProperties returns %d"), err);
					}
				User::Leave(err);
				}	
			}			
		}
	else if (ConfigSection() == _L("MM-3GP-PARSE-U-0111-CP-2"))
		{
		// for a file containing audio data & video properties and a video track, but the video track
		// contains NO video data
		T3GPAudioType audioType;
		TInt framesPerSample;
		TUint length; 
		TUint avgBitRate;
		TUint timeScale;
			
		TInt err = iParser->GetAudioProperties(audioType, length, framesPerSample, avgBitRate, timeScale);
		if (err == KErrNone)
			{
			T3GPVideoType videoType;
			TUint length; 
			TReal frameRate;
			TUint avgBitRate;
			TSize videoSize;
			TUint timeScale;
			err = iParser->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale);
			if (err == KErrNone)
				{
				if (videoType == E3GPMpeg4Video || videoType == E3GPAvcProfileBaseline)
					{
					TInt size = 0;
					err = iParser->GetVideoDecoderSpecificInfoSize(size);
					if (err != KErrNone)
						{		
						if (!ShouldRunOOMTest())
							{										
							ERR_PRINTF2(_L("C3GPParse::GetVideoDecoderSpecificInfoSize returns %d."), err);
							}
						User::Leave(err);
						}
					}
				}
			else
				{
				if (!ShouldRunOOMTest())
					{				
					ERR_PRINTF2(_L("C3GPParse::GetVideoProperties returns %d"), err);
					}
				User::Leave(err);
				}	
				
			TBool available = EFalse;	
			err = iParser->GetFrameAvailability(E3GPVideo, available);				
			if (err != KErrNone && err != KErrNotFound)
				{		
				if (!ShouldRunOOMTest())
					{								
					ERR_PRINTF2(_L("C3GPParse::GetFrameAvailability returns %d."), err);
					}
				User::Leave(err);
				}
				
			TUint numberOfFrames = 0;
			if (TestStepResult() == EPass)
				{
				err = iParser->GetNumberOfVideoFrames(numberOfFrames);	
				if (err != KErrNone)
					{
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF2(_L("C3GPParse::GetNumberOfVideoFrames returns %d"), err);
						}
					User::Leave(err);
					}
				if (numberOfFrames != 0)
					{
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF2(_L("C3GPParse::GetNumberOfVideoFrames retrieves %d frames when expecting 0"), numberOfFrames);
						}
					SetTestStepResult(EFail);
					}
				}
				
			if (TestStepResult() == EPass)
				{
				TUint frameSize;
				TInt err = iParser->GetVideoFrameSize(frameSize);
				if (err == KErrNone)
					{
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF1(_L("C3GPParse::GetVideoFrameSize expects to fail but does not"));
						}
					SetTestStepResult(EFail);
					}
				}
				
			if (TestStepResult() == EPass)
				{
				TUint tsInMs = 0;
				TUint tsInTs = 0;
				TInt err = iParser->GetVideoTimestamp(tsInMs, tsInTs);
				if (err == KErrNone)
					{
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF1(_L("C3GPParse::GetVideoTimestamp expects to fail but does not"));
						}
					SetTestStepResult(EFail);
					}
				}
				
			if (TestStepResult() == EPass)
				{
				TUint index = 0;
				TInt err = iParser->GetVideoSampleEntryIndex(index);
				if (err == KErrNone)
					{
					if (!ShouldRunOOMTest())
						{					
						ERR_PRINTF1(_L("C3GPParse::GetVideoSampleEntryIndex expects to fail but does not"));
						}
					SetTestStepResult(EFail);
					}
				}
			}
		}
	else
		{
		SetTestStepResult(ETestSuiteError);
		}
		
	return TestStepResult();
	}
	


