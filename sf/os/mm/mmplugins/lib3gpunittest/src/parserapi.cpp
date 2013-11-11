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
#include "tsu3gplibraryapi.h"

using namespace ContentAccess;

// --------------------------
// C3GPLibParser_base 
// --------------------------
//
TVerdict C3GPLibParser_base::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	SetTestStepError(KErrNone);
	
	TPtrC testSection = ConfigSection();
	
	TInt err = iTestSection.Create(testSection.Length());
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Cannot create buffer for inputFilename. err = %d"), err);
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	iTestSection += testSection;
	
	TBuf<20> testFolderName;
	testFolderName.Zero();
	if (ConfigSection().Compare(_L("3GP-H264-MP4-LARGE")) == 0)
		{
 		#ifdef __WINSCW__
 		testFolderName.Append(_L("inputFileEmu"));
 		#else
 		testFolderName.Append(_L("inputFileHw"));
 		#endif		
		}
	else	
		{
 		testFolderName.Append(_L("inputFile"));
		}
	
	TPtrC inputFile;
	if(!GetStringFromConfig(ConfigSection(), testFolderName, inputFile))
		{
		ERR_PRINTF1(_L("Cannot read inputFile from ini file."));
		SetTestStepResult(ETestSuiteError);
		return TestStepResult();
		}
	err = iInputFile.Create(inputFile.Length() + 1);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Cannot create buffer for inputFilename. err = %d"), err);
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	iInputFile += inputFile;
	iInputFile.ZeroTerminate();
	
	return TestStepResult();
	}

TVerdict C3GPLibParser_base::doTestStepPostambleL()
	{
	iInputFile.Close();
	iTestSection.Close();
	
	// Close file
	iSourceFile.Close();
	iSourceFile64.Close();

    //Close the file server session
    iFsSession.Close();
    
	return TestStepResult();
	}

TInt C3GPLibParser_base::CheckError(TInt aError, TInt aExpectError, const TDesC& aFunction)
	{
	if (aError != aExpectError)
		{
		ERR_PRINTF4(_L("%S expected %d, but returns %d; "), &aFunction, aExpectError, aError);
		if (aError != KErrNone)
			{
			return aError;
			}
		// expected an error, but return KErrNone
		return KErrGeneral;
		}
	return KErrNone;
	}

void C3GPLibParser_base::ParserOpenL(C3GPParse* aParser, TInt aExpectedError)
	{
	ASSERT(aParser);
	
	TInt err = aParser->Open();
	if (aExpectedError == KErrInUse && err == KErrNone)
		{
		err = aParser->Open();
		}

	if (ShouldRunOOMTest() && err != KErrNone && err != aExpectedError)
		{
		INFO_PRINTF2(_L("Parser open using buffer mode (OOM), returns %d"), err);
		User::Leave(err);
		}

	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("Parser open using buffer mode, returns %d"), err);
		}

	User::LeaveIfError(CheckError(err, aExpectedError, _L("aParser->Open() buffer mode")));
	}

void C3GPLibParser_base::ParserOpenFileL(C3GPParse* aParser, TInt aExpectedError)
	{
	ASSERT(aParser);
	
	TInt err = aParser->Open(iInputFile);
	if (aExpectedError == KErrInUse && err == KErrNone)
		{
		err = aParser->Open(iInputFile);
		}

	if (ShouldRunOOMTest() && err != KErrNone && err != aExpectedError)
		{
		INFO_PRINTF2(_L("Parser open using file path (OOM), returns %d"), err);
		User::Leave(err);
		}

	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("Parser open using file path, returns %d"), err);
		}
	
	User::LeaveIfError(CheckError(err, aExpectedError, _L("aParser->Open() file path")));
	}

void C3GPLibParser_base::ParserCompleteL(C3GPParse* aParser)
	{
	ASSERT(aParser);
	
	TInt err = aParser->Complete();
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->Complete()")));
	}
	
void C3GPLibParser_base::FileOpenL(TUint aFileMode)	
	{
	iSourceFile.Close();
	//Create a connection to the file server    
    User::LeaveIfError(iFsSession.Connect());
    
    //Open the source file passing in the file server session handle, source 
    //file path and file access mode 
    User::LeaveIfError(iSourceFile.Open(iFsSession, iInputFile, aFileMode ));
	}

void C3GPLibParser_base::FileClose()	
	{
	// Close file
	iSourceFile.Close();

    //Close the file server session
    iFsSession.Close();
	}

void C3GPLibParser_base::FileOpen64L(TUint aFileMode)	
	{
	iSourceFile64.Close();
	//Create a connection to the file server    
    User::LeaveIfError(iFsSession.Connect());
    
    //Open the source file passing in the file server session handle, source 
    //file path and file access mode 
    User::LeaveIfError(iSourceFile64.Open(iFsSession, iInputFile, aFileMode ));
	}

void C3GPLibParser_base::FileClose64()	
	{
	// Close file
	iSourceFile64.Close();

    //Close the file server session
    iFsSession.Close();
	}

void C3GPLibParser_base::ParserOpenFileHandleL(C3GPParse* aParser, TUint aFileMode, TInt aExpectedError)
	{
	ASSERT(aParser);
   
	FileOpenL(aFileMode);
    TInt err = aParser->Open(iSourceFile);	
    if (aExpectedError == KErrInUse && err == KErrNone)
		{
		err = aParser->Open(iSourceFile);
		}

	if (err != KErrNone && err != aExpectedError)
		{
		FileClose();

		if (ShouldRunOOMTest())
			{
			INFO_PRINTF2(_L("Parser open using file handle (OOM), returns %d"), err);
			User::Leave(err);
			}
		}

	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("Parser open using file handle, returns %d"), err);		
		}

	if (err == aExpectedError ) 
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

void C3GPLibParser_base::ParserCompleteHandleL(C3GPParse* aParser)
	{
	ASSERT(aParser);
	
	TInt err = aParser->Complete();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("aParser->Complete() file handle failed with %d"), err);
		}
	
	FileClose();
    User::LeaveIfError(err);
	}

void C3GPLibParser_base::ParserOpenFileHandle64L(C3GPParse* aParser, TUint aFileMode, TInt aExpectedError)
	{
	ASSERT(aParser);
   
	FileOpen64L(aFileMode);
    TInt err = aParser->Open(iSourceFile64);	
    if (aExpectedError == KErrInUse && err == KErrNone)
		{
		err = aParser->Open(iSourceFile64);
		}

	if (err != KErrNone && err != aExpectedError)
		{
		FileClose();

		if (ShouldRunOOMTest())
			{
			INFO_PRINTF2(_L("Parser open using 64bit file handle (OOM), returns %d"), err);
			User::Leave(err);
			}
		}

	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("Parser open using 64bit file handle, returns %d"), err);		
		}

	if (err == aExpectedError ) 
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

void C3GPLibParser_base::ParserCompleteHandle64L(C3GPParse* aParser)
	{
	ASSERT(aParser);
	
	TInt err = aParser->Complete();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("aParser->Complete() 64bit file handle failed with %d"), err);
		}
	
	FileClose64();
    User::LeaveIfError(err);
	}

void C3GPLibParser_base::ParserOpenCafLC(C3GPParse* aParser, TInt aExpectedError)
	{
	ASSERT(aParser);
	CContent* content = CContent::NewLC(iInputFile, EContentShareReadWrite);	
	CData* data = content->OpenContentL(EPeek);	
	
	CleanupStack::PushL(data);

	TInt err = aParser->Open(*data);
	if (aExpectedError == KErrInUse && err == KErrNone)
		{
		err = aParser->Open(*data);
		}

	if (ShouldRunOOMTest() && err != KErrNone && err != aExpectedError)
		{
		INFO_PRINTF2(_L("Parser open using CAF (OOM), returns %d"), err);
		User::Leave(err);	
		}

	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("Parser open using CAF, returns %d"), err);
		}

	if (err == aExpectedError)
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

void C3GPLibParser_base::ParserCompleteCafL(C3GPParse* aParser)
	{
	ASSERT(aParser);
	TInt err = aParser->Complete();
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("parse->Complete() CAF failed with %d."), err);
		}

	CleanupStack::PopAndDestroy(2); // content, data
	User::LeaveIfError(err);
	}

// Retrieve both video & audio stream properties
TInt C3GPLibParser_base::GetClipProperties(C3GPParse* aParse, TInt& aVideoError, TUint& aVideoLength, TUint& aAudioLength)
	{
	TInt err = KErrCorrupt;
	T3GPVideoType videoType;
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	
	aVideoError = aParse->GetVideoProperties(videoType, aVideoLength, frameRate, avgBitRate, videoSize, timeScale);
	
	if (aVideoError == KErrNone || aVideoError == KErrNotSupported || aVideoError == KErr3gpLibMoreDataRequired)
		{
		T3GPAudioType audioType;
		TInt framesPerSample;
		err = aParse->GetAudioProperties(audioType, aAudioLength, framesPerSample, avgBitRate, timeScale);
		}	
	return err;
	}

// Get both audio and video decorder information
TInt C3GPLibParser_base::ParserGetDecoderInfo(C3GPParse* aParser)
	{
	TInt dsiSize;
	RBuf8 dsiBuffer;
	
	TInt err = aParser->GetAudioDecoderSpecificInfoSize(dsiSize);
	if ( err == KErrNone )
		{		
		err = dsiBuffer.Create(dsiSize);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("dsiBuffer.Create failed with %d"), err);
			dsiBuffer.Close();
			return err;
			}
		
		err = aParser->GetAudioDecoderSpecificInfo(dsiBuffer);
		}
	if ( err != KErrNone )
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF3(_L("GetAudioDecoderSpecificInfo/Size() failed with %d, size %d"), err, dsiSize);
			}
		dsiBuffer.Close();
		return err;
		}
	
	err = aParser->GetVideoDecoderSpecificInfoSize(dsiSize);
	if ( err != KErrNone && err != KErrNotSupported)
		{
		dsiBuffer.Close();
		ERR_PRINTF2(_L("parse->GetVideoDecoderSpecificInfoSize() failed with %d"), err);
		return err;
		}
	if (err != KErrNotSupported)
		{
		dsiBuffer.Close();
		err = dsiBuffer.Create(dsiSize);	
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("dsiBuffer.Create failed with %d"), err);
			dsiBuffer.Close();
			return err;
			}
		
		err = aParser->GetVideoDecoderSpecificInfo(dsiBuffer);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("VideoDecoderSpecificInfo() failed with %d"), err);
			}
		}
	dsiBuffer.Close();
	return err;
	}

// Read data from file and insert it to the buffer
void C3GPLibParser_base::InsertDataL(C3GPParse* aParse, RFile aFile, TInt aBufferSize, TInt aPos)
	{
	TInt err = KErrNone;
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	buffer.CreateL(aBufferSize);
	err = aFile.Read(aPos, buffer); 
	if (err == KErrNone)
		{
		err = aParse->InsertData(buffer);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("parse->InsertData() failed with %d"), err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("aFile.Read(aPos, buffer) failed with %d"), err);
		}
	buffer.Close();
	CleanupStack::PopAndDestroy(&buffer);
	User::LeaveIfError(err);
	}

// Read audio or video frame(s) depending on the input parameter synchronously
TInt C3GPLibParser_base::ReadAudioVideoFrame(C3GPParse* aParse, TBool aRepeat, TBool aReadAudio)
	{
	TInt err = KErrNone;
	// If repeat, read all frames; otherwise, read only one frame
	while (err == KErrNone)
		{
		TUint size;		
		if (aReadAudio)
			{
			err = aParse->GetAudioFramesSize(size);
			}
		else
			{
			err = aParse->GetVideoFrameSize(size);
			}
		
		if (err == KErrNone)
			{
			TUint timeStampInMs;
			TUint timeStampInTimescale;
			RBuf8 buffer;
			err = buffer.Create(size);				
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("buffer.Create(size) failed with %d"), err);
				buffer.Close();
				return err;
				}
			
			if (aReadAudio)
				{				
				TInt returnedFrames;
				err = aParse->ReadAudioFrames(buffer, returnedFrames, timeStampInMs, timeStampInTimescale);
				}
			else
				{
				TBool keyframe;
				err = aParse->ReadVideoFrame(buffer, keyframe, timeStampInMs, timeStampInTimescale);
				}
			
			if (err != KErrNone && err != KErrNotFound && err != KErr3gpLibMoreDataRequired)
				{
				if (aReadAudio)
					{
					ERR_PRINTF3(_L("parse->GetAudioFrames() failed with %d; frame size: %d"), err, size);
					}
				else
					{
					ERR_PRINTF3(_L("parse->GetVideoFrame() failed with %d; frame size: %d"), err, size);
					}
				buffer.Close();
				return err;
				}
			buffer.Close();
			}
		
		if (!aRepeat)
			{
			break;
			}
		}
	return err;
	}

// Retrieve user data atom.
TInt C3GPLibParser_base::CallUDTApi(C3GPParse* aParse, TUint32 aUdtType, TUint& aIndex, T3GPUdtaLocation aLocation)
	{
	TInt err = KErrNone;
	TInt udtSize;
	
	// use a copy of the target index requested by the user, so it does not get overwritten
	// when calling the APIs GetUserDataAtomSize & GetUserDataAtom
	TUint tempIndex1 = aIndex;	
	TUint tempIndex2 = aIndex;	

	err = aParse->GetUserDataAtomSize(aUdtType, aLocation, tempIndex1, udtSize);
	if (err == KErrNone)
		{
		RBuf8 buffer;	
		
		err = buffer.Create(udtSize); // Use size retrieved from GetUserDataAtomSize
		if (err == KErrNone)
			{
			err = aParse->GetUserDataAtom(aUdtType, aLocation, buffer, tempIndex2);
			if (err == KErrNone && tempIndex1 != tempIndex2)
				{
				ERR_PRINTF1(_L("C3GPLibParser_base::CallUDTApi - Highest Index returned is different when calling GetUserDataAtomSize vs GetUserDataAtom"));	
				err = KErrGeneral;
				}
			}
		else
			{
			ERR_PRINTF2(_L("buffer.Create(size) failed with %d"), err);
			}
		buffer.Close();		
		}
	
	if (err == KErrNone)
		{
		// return the highest index found
		aIndex = tempIndex1;
		}
	
	return err;
	}

void C3GPLibParser_base::DoGetVideoAudioAttributesL(C3GPParse* aParser)
	{
	// Retrieve video & audio stream attributes
	T3GPVideoType videoType;
	TUint length; 
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	
	TInt err = aParser->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->GetVideoProperties()")));
	
	T3GPAudioType audioType;
	TInt framesPerSample;
	err = aParser->GetAudioProperties(audioType, length, framesPerSample, avgBitRate, timeScale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->GetAudioProperties()")));
	
	err = ParserGetDecoderInfo(aParser);
	User::LeaveIfError(CheckError(err, KErrNone, _L("ParserGetDecoderInfo(aParser)")));
	
	TUint numVideoFrames;
	err = aParser->GetNumberOfVideoFrames(numVideoFrames);
	User::LeaveIfError(CheckError(err, KErrNone, _L("parse->GetNumberOfVideoFrames")));
	
	TUint startIndex = 0;
	RArray<T3GPFrameInfoParameters> array;
	err = aParser->GetVideoFrameProperties(startIndex, numVideoFrames, array);	
	array.Close();
	User::LeaveIfError(CheckError(err, KErrNone, _L("parse->GetVideoFrameProperties")));
	}

TInt C3GPLibParser_base::DoReadFrame(C3GPParse* aParse)
	{
	TInt numBufferedBytes;
	TInt err = aParse->GetNumBufferedBytes(numBufferedBytes);
	if (err != KErrNone && numBufferedBytes != 0)
		{
		// when GetNumBufferedBytes returns error, numBufferedBytes is set to zero.
		return err;		
		}
	err = CheckError(err, KErrNotSupported, _L("parse->GetNumBufferedBytes()"));	
	if (err == KErrNone)
		{
		// read audio frame
		err = ReadAudioVideoFrame(aParse, ETrue, ETrue);
		// When finishing reading all frames, it returns KErrNotFound
		err = CheckError(err, KErrNotFound, _L("ReadAudioVideoFrame(aParse, ETrue)"));
		
		if (err == KErrNone)
			{
			// read video frame
			err = ReadAudioVideoFrame(aParse, ETrue, EFalse);
			// When finishing reading all frames, it returns KErrNotFound
			err = CheckError(err, KErrNotFound, _L("ReadAudioVideoFrame(aParse, EFalse)"));			
			}
		
		// After finishing reading all frame, the next video time stamp should return with KErrEof
		TUint timeStampInMs;
		TUint timeStampInTimescale;
		err = aParse->GetVideoTimestamp(timeStampInMs, timeStampInTimescale);
		err = CheckError(err, KErrEof, _L("GetVideoTimestamp()"));			
		}
		
	return err;
	}

void C3GPLibParser_base::DoSeekTestL(C3GPParse* aParser)
	{
	// testclip_h263_qcif_15fps_armnb_8khz_onlyvideo.3GP with some key frame
	TUint num; 
	TUint audioPos;
	TUint videoPos;
	TUint timeStampInMs = 0; // for key frame
	TUint timeStampInTimescale = 0;  // for key frame
	TUint timeStampInMs1 = 0;  // for non key frame immediately after key frame
	TUint timeStampInTimescale1 = 0;  // for non-key frame immediately after key frame
	TUint keyFrameIndex = 0;
	TBool keyFrame;
	TInt err = aParser->GetNumberOfVideoFrames(num);
	if (err == KErrNone)
		{
		for (TInt i = 0; i < num; i++)
			{			
			err = aParser->GetVideoFrameKeyType(i, keyFrame);
			if (err == KErrNone && keyFrame)
				{
				keyFrameIndex = i;
				err = aParser->GetVideoFrameStartTime(keyFrameIndex, timeStampInMs, timeStampInTimescale);
				}
			if (err == KErrNone && !keyFrame && keyFrameIndex > 0)
				{
				keyFrameIndex = i;
				err = aParser->GetVideoFrameStartTime(keyFrameIndex, timeStampInMs1, timeStampInTimescale1);
				break;
				}
			if (err != KErrNone)
				{
				ERR_PRINTF3(_L("aParser->GetVideoFrameKeyType() failed with %d at i %d"), err, i);
				User::Leave(err);
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("aParser->GetNumberOfVideoFrames() failed with %d"), err);
		User::Leave(err);
		}
	err = aParser->Seek(timeStampInMs, ETrue, audioPos, videoPos);
	if (err != KErrNone || timeStampInMs != videoPos)
		{
		ERR_PRINTF4(_L("aParser->Seek() return %d, timeStampInMs %d, videoPos %d"), err, timeStampInMs, videoPos);
		}
	if (err == KErrNone && timeStampInMs != videoPos)
		{
		User::LeaveIfError(KErrGeneral);
		}
	User::LeaveIfError(err);
	
	err = aParser->Seek(timeStampInMs1, EFalse, audioPos, videoPos);
	if (err != KErrNone || timeStampInMs1 != videoPos)
		{
		ERR_PRINTF4(_L("aParser->Seek() return %d, timeStampInMs1 %d, videoPos %d"), err, timeStampInMs1, videoPos);
		}
	if (err == KErrNone && timeStampInMs1 != videoPos)
		{
		User::LeaveIfError(KErrGeneral);
		}
	User::LeaveIfError(err);
	}

void C3GPLibParser_base::ReadFrameBufferModeL(C3GPParse* aParser)
	{
	ParserOpenL(aParser);

	TInt errReadAudio = KErrNone;
	TInt errReadVideo = KErrNone;
	// Insert 1K data into the library
	TInt pos = 0;
	TInt bufferSize = 1000;
	
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	errReadAudio = ReadAudioVideoFrame(aParser, ETrue, ETrue);
	errReadVideo = ReadAudioVideoFrame(aParser, ETrue, EFalse);

	while (errReadAudio == KErr3gpLibMoreDataRequired || errReadVideo == KErr3gpLibMoreDataRequired)
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		if (errReadAudio == KErr3gpLibMoreDataRequired)
			{
			errReadAudio = ReadAudioVideoFrame(aParser, ETrue, ETrue);
			}

		if (errReadVideo == KErr3gpLibMoreDataRequired)
			{
			errReadVideo = ReadAudioVideoFrame(aParser, ETrue, EFalse);
			}
		}
	
	// When last frame has been read, it should return KErrNotFound
	if (errReadAudio != KErrNotFound)
		{
		ERR_PRINTF1(_L("parse->ReadAudioFrames() expects KErrNotFound. "));
		User::Leave(errReadAudio);
		}
	if (errReadVideo != KErrNotFound)
		{
		ERR_PRINTF1(_L("parse->ReadVideoFrame expects KErrNotFound. "));
		User::Leave(errReadVideo);
		}
	
	ParserCompleteL(aParser);
	}

C3GPLib_AsyncObserver::C3GPLib_AsyncObserver()
	{	
	}

TVerdict C3GPLib_AsyncObserver::doTestStepPreambleL()
	{
	C3GPLibParser_base::doTestStepPreambleL();
	
	iSchedulerWait = new (ELeave) CActiveSchedulerWait;
	if(!iSchedulerWait)
		{
		SetTestStepResult(EInconclusive);
		}
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// C3GPLib_AsyncObserver for asynchronous read
// -----------------------------------------------------------------------------
//
TVerdict C3GPLib_AsyncObserver::doTestStepPostambleL()
	{
	C3GPLibParser_base::doTestStepPostambleL();
	if ( iSchedulerWait)
		{
		if (iSchedulerWait->IsStarted())
			{
			iSchedulerWait->AsyncStop();
			}
		delete iSchedulerWait;
		iSchedulerWait = NULL;
		}	
	return TestStepResult();
	}

// Read audio or video frame(s) depending on the input parameter asynchronously
TInt C3GPLib_AsyncObserver::ReadAudioVideoFrameAsync(C3GPParse* aParse, TBool aReadAudio, TBool aCancel)
	{
	TInt err = KErrNone;
	TUint size;	
	
	while (err == KErrNone)
		{
		if (aReadAudio)
			{
			err = aParse->GetAudioFramesSize(size);
			}
		else
			{
			err = aParse->GetVideoFrameSize(size);
			}
		
		RBuf8 buffer;
		if (err == KErrNone)
			{
			err = buffer.Create(size);				
			}
		else
			{
			err = buffer.Create(KBufferSize);	
			}
		
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("buffer.Create(KBufferSize) failed with %d"), err);
			buffer.Close();
			return err;
			}
			
		iWaitForNotification = ETrue;
		if (aReadAudio)
			{				
			aParse->ReadAudioFrames(*this, buffer);
			}
		else 
			{
			aParse->ReadVideoFrame(*this, buffer);
			}
		
		if (aCancel && iAsyncError == KErrNone)
			{
			aParse->CancelReadFrame();
			// Calling cancel again won't have effects
			aParse->CancelReadFrame();
			
			// Do read frame again
			if (aReadAudio)
				{				
				aParse->ReadAudioFrames(*this, buffer);
				}
			else 
				{
				aParse->ReadVideoFrame(*this, buffer);
				}
			}
		
		if (iWaitForNotification)
		    {
			iSchedulerWait->Start();
		    }
		
		err = iAsyncError; 
		buffer.Close(); 
		} 
	
	if ( iSchedulerWait->IsStarted() )
        {
        iSchedulerWait->AsyncStop();
	    }
			
	if (err != KErrNone && err != KErrNotFound)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("ReadAudioVideoFrameAsync failed with %d;"), err);
			}
		}
	else
		{
		err = KErrNone;
		}
	
	return err;
	}

void C3GPLib_AsyncObserver::AudioFramesAvailable(TInt aError, TUint aReturnedFrames, 
			TUint aTimeStampInMs, TUint aTimeStampInTimescale)
	{
	iWaitForNotification = EFalse;
	iAsyncLastNotificationFromAudio = EFalse;
	iAsyncError = aError;
	iAsyncAudioTimeStampInMs = aTimeStampInMs; 
	iAsyncAudioReturnedFrames = aReturnedFrames;
	iAsyncAudioTimestampInTimescale = aTimeStampInTimescale;
	if (iSchedulerWait->IsStarted())
		{
		iSchedulerWait->AsyncStop();
		}

	}

void C3GPLib_AsyncObserver::VideoFrameAvailable(TInt aError, TBool aKeyFrame, TUint 
			aTimeStampInMs, TUint aTimeStampInTimescale)
	{
	iWaitForNotification = EFalse;
	iAsyncLastNotificationFromAudio = EFalse;
	iAsyncError = aError;
	iAsyncVideoTimeStampInMs = aTimeStampInMs; 
	iAsyncVideoKeyFrame = aKeyFrame;
	iAsyncVideoTimestampInTimescale = aTimeStampInTimescale;
	
	if (iSchedulerWait->IsStarted())
		{
		iSchedulerWait->AsyncStop();
		}

	}



// -----------------------------------------------------------------------------
// Setup parser to read file by file path.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0001::C3GPLibParser_0001()
	{
	}

TVerdict C3GPLibParser_0001::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		OpenCompleteL(parser);
	
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0001::OpenCompleteL(C3GPParse* aParser)
	{
	// Test open and complete with default file access mode
	ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	
	FileOpenL();
    ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	ParserCompleteL(aParser); // Should not fail when calling complete twice
	FileClose();
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-write)
	FileOpenL(EFileShareReadersOrWriters | EFileWrite);
    ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	ParserCompleteL(aParser);
	FileClose();
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-write)
	FileOpenL(EFileShareReadersOrWriters | EFileRead);
    ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	FileClose();
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-write)
	FileOpenL(EFileShareAny  | EFileWrite);
    ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	FileClose();
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-write)
	FileOpenL(EFileShareAny | EFileRead);
    ParserOpenFileL(aParser);
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Setup parser to read file by buffer mode.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0002::C3GPLibParser_0002()
	{
	}

TVerdict C3GPLibParser_0002::doTestStepL()
	{	
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		OpenCompleteBufferModeL(parser);
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0002::OpenCompleteBufferModeL(C3GPParse* aParser)
	{
	// buffer mode
	FileOpenL();
	ParserOpenL(aParser);
	
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Setup parser to read file by file handle.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0003::C3GPLibParser_0003()
	{	
	}

TVerdict C3GPLibParser_0003::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		OpenCompleteFileHandleL(parser);
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0003::OpenCompleteFileHandleL(C3GPParse* aParser)
	{
	ParserOpenFileHandleL(aParser);
	ParserCompleteHandleL(aParser);
	
    //Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-only)
	ParserOpenFileHandleL(aParser, EFileRead | EFileShareReadersOnly);
	ParserCompleteHandleL(aParser);
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-only)
	ParserOpenFileHandleL(aParser, EFileShareReadersOrWriters | EFileWrite);
	ParserCompleteHandleL(aParser);
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-only)
	ParserOpenFileHandleL(aParser, EFileShareReadersOrWriters | EFileRead);
	ParserCompleteHandleL(aParser);
	
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-only)
	ParserOpenFileHandleL(aParser, EFileShareAny  | EFileWrite);
	ParserCompleteHandleL(aParser);
    
	//Open the source file passing in the file server session handle, source 
    //file path and file access mode (read-only)
	ParserOpenFileHandleL(aParser, EFileShareAny | EFileRead);
	ParserCompleteHandleL(aParser);
	}

// -----------------------------------------------------------------------------
// Setup parser to read file by CAF.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0004::C3GPLibParser_0004()
	{	
	}

TVerdict C3GPLibParser_0004::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		CContent* content = CContent::NewLC(iInputFile, EContentShareReadWrite);	
		
		CData* data = content->OpenContentL(EPeek);			
		CleanupStack::PushL(data);
		
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TInt err = parser->Open(*data);

		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			CleanupStack::PopAndDestroy(parser);
			CleanupStack::PopAndDestroy(2); // content, data
			return TestStepResult();
			}
		
		err = parser->Complete();
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		CleanupStack::PopAndDestroy(2); // content, data
		}
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// Retrieve video & audio stream attributes from a mp4/3gp/3g2 file with supported 
// video and audio streams using file mode - file path / file handle and CAF object.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0005::C3GPLibParser_0005()
	{	
	}

TVerdict C3GPLibParser_0005::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetVideoAudioAttributesL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0005::GetVideoAudioAttributesL(C3GPParse* aParser)
	{
	// Open parser in File path
	ParserOpenFileL(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteL(aParser);
	
	// open parser in file handle
	ParserOpenFileHandleL(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteHandleL(aParser);
	
	// Open parser in CAF
	ParserOpenCafLC(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteCafL(aParser);
	}

// -----------------------------------------------------------------------------
// Retrieve video & audio stream attributes from a mp4/3gp/3g2 file 
// with supported video and audio streams using buffer mode.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0006::C3GPLibParser_0006()
	{	
	}

TVerdict C3GPLibParser_0006::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoAudioAttributesBufferModeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			FileClose();
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0006::VideoAudioAttributesBufferModeL(C3GPParse* aParser)
	{
	// buffer mode
	FileOpenL();
	ParserOpenL(aParser);

	TInt err = KErrNone;
	// Retrieve number of buffered bytes before and after data is supplied to the parser.
	err = GetVideoAudioAttributes(aParser, ENumBufferedBytes);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoAudioAttributes(aParser, ENumBufferedBytes)")));
	
	ParserCompleteL(aParser);
	FileClose();
	
	// Open the parser again
	FileOpenL();
	ParserOpenL(aParser);
	
	// Retrieve video and audio descriptions before and after data is supplied to the parser.
	err = GetVideoAudioAttributes(aParser, EClipProperties);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoAudioAttributes(aParser, EClipProperties)")));
	
	ParserCompleteL(aParser);
	FileClose();
	
	// Open the parser again
	FileOpenL();
	ParserOpenL(aParser);
	
	// Retrieve video and audio decoder before and after data is supplied to the parser.
	err = GetVideoAudioAttributes(aParser, EDecoderInfo);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoAudioAttributes(aParser, EDecoderInfo)")));
	
	ParserCompleteL(aParser);
	FileClose();
	
	// Open the parser again
	FileOpenL();
	ParserOpenL(aParser);
	
	
	// Retrieve video frame properties and number of frames before and after data is supplied to the parser.

	err = GetVideoAudioAttributes(aParser, EVideoFrameProp);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoAudioAttributes(aParser, EVideoFrameProp)")));
	
	ParserCompleteL(aParser);
	FileClose();
	}

TInt C3GPLibParser_0006::CallTest(C3GPParse* aParser, TBufferModeTest aTest, TInt aExpectedNumBuffered)
	{
	TInt err = KErrNone;
	TInt numBufferedBytes;

	// Before insert any data. Retrieving video and audio attributes should fail
	switch (aTest)
		{
		case (ENumBufferedBytes):
			err = aParser->GetNumBufferedBytes(numBufferedBytes);
			if (err == KErrNone && numBufferedBytes != aExpectedNumBuffered)
				{
				err = KErrGeneral;
				}
		break;
		case (EClipProperties):
			TInt videoError;
			TUint videoLength;
			TUint audioLength;
			err = GetClipProperties(aParser, videoError, videoLength, audioLength);
			if ((videoError != KErrNone && videoError != KErr3gpLibMoreDataRequired) || 
					(err != KErrNone && err != KErr3gpLibMoreDataRequired))
				{
				err = KErrGeneral;
				}
		break;
		case (EDecoderInfo):
			err = ParserGetDecoderInfo(aParser);
		break;
		case (EVideoFrameProp):
			if (aExpectedNumBuffered == 0)
				{
				err = GetVideoFramePropAndNumVideoFrame(aParser, EFalse);
				}
			else
				{
				err = GetVideoFramePropAndNumVideoFrame(aParser, ETrue);
				}
		break;
		default:
		break;
		}
	return err;
	}

TInt C3GPLibParser_0006::GetVideoAudioAttributes(C3GPParse* aParser, TBufferModeTest aTest)
	{
	TInt err = KErrNone;
	
	// Before insert any data. Retrieving video and audio attributes should fail
	err = CallTest(aParser, aTest, 0);
	if (aTest != ENumBufferedBytes)
		{
		err = CheckError(err, KErr3gpLibMoreDataRequired, _L("CallTest(aParser, aTest, 0)"));
		}
	else
		{
		err = CheckError(err, KErrNone, _L("CallTest()"));
		}
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetVideoAudioAttributes failed with %d; "), err);
		return KErrGeneral;
		}
	
	// Insert 1K data to buffer	
	TInt bufferSize = 0;
	TInt pos = 0; // from begining of the file
	err = KErr3gpLibMoreDataRequired;
	// Insert data until enough data has been inserted
	while (err == KErr3gpLibMoreDataRequired)
		{
		// Insert more data to buffer	
		pos = bufferSize + pos;
		bufferSize = KBufferSize;
		RBuf8 buffer;
		err = buffer.Create(bufferSize);
		if (err == KErrNone)
			{
			err = iSourceFile.Read(pos, buffer); 
			if (err == KErrNone)
				{
				err = aParser->InsertData(buffer);
				if ( err == KErrNone )
					{
					err = CallTest(aParser, aTest, (bufferSize + pos));
					}
				}
			}
		buffer.Close();
		}
	
	err = CheckError(err, KErrNone, _L("CallTest()"));
	return err;
	}

TInt C3GPLibParser_0006::GetVideoFramePropAndNumVideoFrame(C3GPParse* aParse, TBool aExpectPass)
	{
	TInt err = KErrNone;
	TUint numVideoFrames;
	err = aParse->GetNumberOfVideoFrames(numVideoFrames);
	if (aExpectPass)
		{
		if ( err == KErrNone )
			{
			// Only when GetNumberOfVideoFrames returns KErrNone, the method GetVideoFrameProperties
			// can be called
			TUint startIndex = 0;
			RArray<T3GPFrameInfoParameters> array;
			err = aParse->GetVideoFrameProperties(startIndex, numVideoFrames, array);
			array.Close();
			}		
		}
	else
		{
		// When there is no data in the handler, GetNumberOfVideoFrames will fail with KErrGeneral, 
		// not the KErr3gpLibMoreDataRequired
		if ( err == KErr3gpLibMoreDataRequired || err == KErrGeneral)
			{
			if (!ShouldRunOOMTest())
				{
				INFO_PRINTF2(_L("Expect parse->GetNumberOfVideoFrames() failed with %d"), err);
				}
			err = KErr3gpLibMoreDataRequired;
			}
		// expected failure, but get KErrNone.
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("parse->GetNumberOfVideoFrames() should fail due to buffer too small"));
			err = KErrGeneral;
			}
		}
	
	return err;
	}

// -----------------------------------------------------------------------------
// Check if the clip is streamable in file mode from a mp4/3gp/3g2 file 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0007::C3GPLibParser_0007()
	{	
	}

TVerdict C3GPLibParser_0007::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetstreamableL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0007::GetstreamableL(C3GPParse* aParser)
	{
	// Open parser in File mode
	ParserOpenFileL(aParser);
	
	// Check if the file is streamable
	TBool streamable;
	TInt err = aParser->GetStreamable(streamable);
	if (err != KErrNone)
		{
		SetTestStepError(err);
		}
	else if (!streamable)
		{
		INFO_PRINTF2(_L("aParser->GetStreamable() returns %d"), streamable);
		SetTestStepError(KErrNotSupported);
		}
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Check if the clip is streamable in buffer mode from a mp4/3gp/3g2 file 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0008::C3GPLibParser_0008()
	{	
	}

TVerdict C3GPLibParser_0008::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetstreamableL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0008::GetstreamableL(C3GPParse* aParser)
	{
	// buffer mode
	FileOpenL();
	ParserOpenL(aParser);

	// Check if the file is streamable before the data is inserted
	TBool streamable;
	TInt err = aParser->GetStreamable(streamable);
	
	// Insert 1K data to buffer	
	TInt bufferSize = 0;
	TInt pos = 0; // from begining of the file

	// Insert data until enough data has been inserted
	while (err == KErr3gpLibMoreDataRequired)
		{
		// Insert more data to buffer	
		pos = bufferSize + pos;
		bufferSize = KBufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		
		err = aParser->GetStreamable(streamable);
		}
	
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("aParser->GetStreamable() returns %d"), err);
			}
		SetTestStepError(err);
		}
	
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Retrieve buffered bytes, and video & audio frames synchronously in file mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0011::C3GPLibParser_0011()
	{	
	}

TVerdict C3GPLibParser_0011::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0011::ReadFrameL(C3GPParse* aParse)
	{
	// Parser open using File path 
	ParserOpenFileL(aParse);
	User::LeaveIfError(DoReadFrame(aParse));
	ParserCompleteL(aParse);
	
	// Parser open using file handler
	ParserOpenFileHandleL(aParse);
	User::LeaveIfError(DoReadFrame(aParse));
	ParserCompleteHandleL(aParse);
    
	// Parser open using CAF
	ParserOpenCafLC(aParse);
	User::LeaveIfError(DoReadFrame(aParse));
	ParserCompleteCafL(aParse);
	}

// -----------------------------------------------------------------------------
// Parse video & audio frames from a mp4/3gp file asynchronously in file mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0014::C3GPLibParser_0014()
	{	
	}

TVerdict C3GPLibParser_0014::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0014::doTestStepPostambleL()
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

TVerdict C3GPLibParser_0014::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameAsyncL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0014::ReadFrameAsyncL(C3GPParse* aParser)
	{
	ParserOpenFileL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL()"));
		}
	ParserCompleteL(aParser);

	ParserOpenFileL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser, ETrue));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL(), then cancel the read"));
		}
	ParserCompleteL(aParser);
	
	ParserOpenFileHandleL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL() using file handler"));
		}
	ParserCompleteHandleL(aParser);

	ParserOpenCafLC(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL() using CAF"));
		}
	ParserCompleteCafL(aParser);
	}

TInt C3GPLibParser_0014::DoReadFrameAsync(C3GPParse* aParser, TBool aCancel)
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
// Parse video & audio frames from a mp4/3gp file synchronously
// -----------------------------------------------------------------------------
//
C3GPLibParser_0015::C3GPLibParser_0015()
	{	
	}

TVerdict C3GPLibParser_0015::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0015::ReadFrameL(C3GPParse* aParser)
	{
	FileOpenL();
	ReadFrameBufferModeL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Get frame type using file path
// -----------------------------------------------------------------------------
//
C3GPLibParser_0016::C3GPLibParser_0016()
	{	
	}


TVerdict C3GPLibParser_0016::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetFrameTypeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0016::GetFrameTypeL(C3GPParse* aParser)
	{
	ParserOpenFileL(aParser);
	
	T3GPFrameType type;
	TInt err = aParser->GetFrameType(type);
	if (err == KErrNone)
		{
		TUint num;
		err = aParser->GetNumberOfVideoFrames(num);
		if (err == KErrNone)
			{
			// There is number of video in the file. Calling GetFrameType num+1 time won't affect 
			//the parser’s current position. It should return the same results
			for (TInt i = 0; i < num; i++)
				{
				err = aParser->GetFrameType(type);
				User::LeaveIfError(err);
				}
			err = aParser->GetFrameType(type);
			User::LeaveIfError(err);
			}

		if (!ShouldRunOOMTest())
			{
			INFO_PRINTF2(_L("GetFrameType return type %d. "), type);
			}
		if (type == E3GPAudio)
			{
			// read 1 audio frame		
			err = ReadAudioVideoFrame(aParser, EFalse, ETrue);
			User::LeaveIfError(CheckError(err, KErrNone, _L("ReadAudioVideoFrame")));

			err = aParser->GetFrameType(type);
			if (!ShouldRunOOMTest())
				{
				INFO_PRINTF3(_L("After read 1 audio frame, GetFrameType return err %d; type %d. "), err, type);
				}
			SetTestStepError(err);
			}
		if (err == KErrNone && type == E3GPVideo)
			{
			// read 1 video frame
			err = ReadAudioVideoFrame(aParser, EFalse, EFalse);
			User::LeaveIfError(CheckError(err, KErrNone, _L("ReadAudioVideoFrame")));
			
			err = aParser->GetFrameType(type);
			SetTestStepError(err);
			}
		}
	else
		{
		SetTestStepError(err);
		}
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Get the QCELP storage mode in file path and buffer mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0017::C3GPLibParser_0017()
	{	
	}

TVerdict C3GPLibParser_0017::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetQcelModeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0017::GetQcelModeL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	TInt err = KErrNone;
	T3GPQcelpStorageMode mode;
	err = aParser->GetQcelpStorageMode(mode);
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF3(_L("aParser->GetQcelpStorageMode(mode) file mode return %d; mode %d. "), err, mode);
			}
		SetTestStepError(err);
		}

	if ((iTestSection.Compare(KEsds) == 0 && mode != E3GPQcelpSampleEntryBox ) ||
			(iTestSection.Compare(KSqcp) == 0 && mode != E3GPMP4AudioDescriptionBox ))
		{
		User::Leave(KErrGeneral);
		}
	
	ParserCompleteL(aParser);
	
	// buffer mode
	ParserOpenL(aParser);
	FileOpenL();
	
	// Insert 1K data into the library
	TInt pos = 0;
	TInt bufferSize = 1000;
	
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	err = aParser->GetQcelpStorageMode(mode);
	
	while (err == KErr3gpLibMoreDataRequired )
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		err = aParser->GetQcelpStorageMode(mode);
		}
	
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF3(_L("aParser->GetQcelpStorageMode(mode) file mode return %d; mode %d. "), err, mode);
			}
		SetTestStepError(err);
		}
	
	if ((iTestSection.Compare(KEsds) == 0 && mode != E3GPQcelpSampleEntryBox ) ||
			(iTestSection.Compare(KSqcp) == 0 && mode != E3GPMP4AudioDescriptionBox ))
		{
		User::Leave(KErrGeneral);
		}
	
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Get H263 video leve in file path and buffer mode.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0021::C3GPLibParser_0021()
	{	
	}

TVerdict C3GPLibParser_0021::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetH263LevelL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0021::GetH263LevelL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	TInt err = KErrNone;
	TInt level;
	err = aParser->GetH263VideoLevel(level);
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF3(_L("aParser->GetH263VideoLevel() file mode return %d; level %d. "), err, level);
			}
		SetTestStepError(err);
		}
	
	ParserCompleteL(aParser);
	
	// buffer mode
	ParserOpenL(aParser);
	FileOpenL();
	
	// Insert 1K data into the library
	TInt pos = 0;
	TInt bufferSize = KBufferSize;
	
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	err = aParser->GetH263VideoLevel(level);;
	
	while (err == KErr3gpLibMoreDataRequired )
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		err = aParser->GetH263VideoLevel(level);;
		}
	
	if (err != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF3(_L("aParser->GetH263VideoLevel() file mode return %d; level %d. "), err, level);
			}
		SetTestStepError(err);
		}
	
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Read Video and audio Decoder Info size and info in file path and buffer mode.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0023::C3GPLibParser_0023()
	{	
	}

TVerdict C3GPLibParser_0023::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetDecoderInfoL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0023::GetDecoderInfoL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);

	TInt errReadAudio = KErrNone;
	TInt errReadVideo = KErrNone;
	// Retrieve the info with buffer size = 0. Should fail with KErrOverflow
	RBuf8 dsiBuffer;
	CleanupClosePushL(dsiBuffer);
	dsiBuffer.CreateL(0);
	errReadAudio = aParser->GetAudioDecoderSpecificInfo(dsiBuffer);
	errReadVideo = aParser->GetVideoDecoderSpecificInfo(dsiBuffer);
	
	if (errReadAudio == KErrOverflow && errReadVideo == KErrOverflow)
		{
		errReadAudio = ParserGetDecoderInfo(aParser);
		
		if (errReadAudio != KErrNone)
			{
			ERR_PRINTF2(_L("ParserGetDecoderInfo file mode return %d; "), errReadAudio);
			User::Leave(errReadAudio);
			}
		}
	else
		{
		ERR_PRINTF2(_L("ParserGetDecoderInfo file mode expected KErrOverflow, but return %d; "), errReadAudio);
		User::Leave(errReadAudio);
		}
	
	ParserCompleteL(aParser);
	
	// buffer mode
	ParserOpenL(aParser);
	FileOpenL();
	
	// Insert 1K data into the library
	TInt pos = 0;
	TInt bufferSize = KBufferSize;
	
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	
	// Retrieve the info with buffer size = 0. Should fail with KErrOverflow
	dsiBuffer.Close();
	dsiBuffer.CreateL(0);
	errReadAudio = aParser->GetAudioDecoderSpecificInfo(dsiBuffer);
	errReadVideo = aParser->GetVideoDecoderSpecificInfo(dsiBuffer);
	CleanupStack::PopAndDestroy(&dsiBuffer);
	
	if (errReadAudio != KErrOverflow || errReadVideo != KErrOverflow)
		{
		ERR_PRINTF2(_L("ParserGetDecoderInfo buffer mode expected KErrOverflow, but return %d; "), errReadAudio);
		User::Leave(errReadAudio);
		}
	errReadAudio = ParserGetDecoderInfo(aParser);
	
	while (errReadAudio == KErr3gpLibMoreDataRequired )
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		errReadAudio = ParserGetDecoderInfo(aParser);
		}
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("ParserGetDecoderInfo(aParser) buffer mode return %d. "), errReadAudio);
		}
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Seek different position of a video/audio file
// -----------------------------------------------------------------------------
//
C3GPLibParser_0027::C3GPLibParser_0027()
	{
	}

TVerdict C3GPLibParser_0027::doTestStepL()
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

void C3GPLibParser_0027::SeekTestL(C3GPParse* aParse)
	{	
	// file mode
	ParserOpenFileL(aParse);
	
	TUint audioPos;
	TUint videoPos;
	TUint timeStampInMs = 0;
	
	// Seek to position 0.
	TInt err = KErrNone; 
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
			// All other files contains audio and video data
			User::LeaveIfError(CheckError(videoError, KErrNone, _L("GetClipProperties(aParse, videoError, videoLength, audioLength)")));
			User::LeaveIfError(CheckError(err, KErrNone, _L("GetClipProperties(aParse, videoError, videoLength, audioLength)")));
			}
		
		if (audioLength >= videoLength)
			{
			timeStampInMs = audioLength;
			}
		else
			{
			timeStampInMs = videoLength;
			}
		
		if (iTestSection.Compare(KSameLenAudioVideo) == 0 || iTestSection.Compare(KAudioOnly) == 0)
			{
			err = ParserSeek(aParse, timeStampInMs, audioPos, videoPos);
			User::LeaveIfError(err);
			}
		
		if (iTestSection.Compare(KAudioMore) == 0 || iTestSection.Compare(KVideoMore) == 0 )
			{
			timeStampInMs = (audioLength + videoLength)/2;
			err = aParse->Seek(timeStampInMs, EFalse, audioPos, videoPos);
			User::LeaveIfError(err);
			}
		}
	
	ParserCompleteL(aParse);
	}

// Seek position 0, half the length and the length of the stream
TInt C3GPLibParser_0027::ParserSeek(C3GPParse* aParser, TInt aLen, TUint& aAudioPos, TUint& aVideoPos)
	{
	TInt err = KErrNone;
	TUint position = 0;
	while (err == KErrNone && position <= aLen)
		{
		err = aParser->Seek(position, ETrue, aAudioPos, aVideoPos);
		if (err == KErrNone)
			{
			err = aParser->Seek(position, EFalse, aAudioPos, aVideoPos);
			}
		position = position + aLen/2;
		}

	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("aParser->Seek() failed with %d when positon is %d"), err, position);
		}
	return err;
	}

// -----------------------------------------------------------------------------
// Seek positions after any keyframe and immediately after a non-key frame.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0031::C3GPLibParser_0031()
	{	
	}

TVerdict C3GPLibParser_0031::doTestStepL()
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

void C3GPLibParser_0031::SeekTestL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);	
	DoSeekTestL(aParser);			
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Check frame availability using file path
// -----------------------------------------------------------------------------
//
C3GPLibParser_0032::C3GPLibParser_0032()
	{	
	}

TVerdict C3GPLibParser_0032::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetFrameAvailabilityL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0032::GetFrameAvailabilityL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	DoGetFrameAvailabilityL(aParser);
	ParserCompleteL(aParser);
	}

void C3GPLibParser_0032::DoGetFrameAvailabilityL(C3GPParse* aParser)
	{
	T3GPFrameType audioType = E3GPAudio;
	T3GPFrameType videoType = E3GPVideo;
	TBool audioAvailable;
	TBool videoAvailable;
	TUint num;
	TInt audioErr = aParser->GetFrameAvailability(audioType, audioAvailable);
	if (audioErr != KErrNone && audioErr != KErrNotFound)
		{
		SetTestStepError(audioErr);
		User::LeaveIfError(audioErr);
		}
	
	TInt videoErr = aParser->GetFrameAvailability(videoType, videoAvailable);
	if (videoErr != KErrNone && videoErr != KErrNotFound)
		{
		SetTestStepError(videoErr);
		User::LeaveIfError(videoErr);
		}
	
	TInt err = aParser->GetNumberOfVideoFrames(num);
	if (err != KErrNone && err != KErrNotSupported)
		{
		SetTestStepError(err);
		User::LeaveIfError(err);
		}
	
	if (iTestSection.Compare(KNoAudioNoVideo) == 0)
		{
		if (audioErr != KErrNotFound || videoErr != KErrNotFound 
				|| audioAvailable || videoAvailable)
			{
			ERR_PRINTF2(_L("GetFrameAvailability() audio failed with %d, expected KErrNotFound"), audioErr);
			ERR_PRINTF2(_L("GetFrameAvailability() expected audioAvailable returns false, actual %d "), audioAvailable);
			ERR_PRINTF2(_L("GetFrameAvailability() video failed with %d, expected KErrNotFound;"), videoErr);
			ERR_PRINTF2(_L("GetFrameAvailability() expected videoAvailable returns false, actual %d "), videoAvailable);
			User::Leave(KErrGeneral);  
			}				
		User::LeaveIfError(CheckError(err, KErrNotSupported, _L("aParser->GetNumberOfVideoFrames(num)")));
		}
	else if (iTestSection.Compare(KAudioOnly) == 0)
		{
		if (audioErr != KErrNone || videoErr != KErrNotFound 
				|| !audioAvailable || videoAvailable)
			{
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() audio failed with %d, expected KErrNone"), audioErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected audioAvailable returns True, actual %d "), audioAvailable);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() video failed with %d, expected KErrNotFound;"), videoErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected videoAvailable returns false, actual %d"), videoAvailable);
			User::Leave(KErrGeneral);
			}
		User::LeaveIfError(CheckError(err, KErrNotSupported, _L("aParser->GetNumberOfVideoFrames(num)")));
		}
	else if (iTestSection.Compare(KVideoOnly) == 0)
		{
		if (audioErr != KErrNotFound || videoErr != KErrNone 
				|| audioAvailable || !videoAvailable)
			{
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() audio failed with %d, expected KErrNotFound"), audioErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected audioAvailable returns false, actual %d "), audioAvailable);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() video failed with %d, expected KErrNone;"), videoErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected videoAvailable returns true, actual %d"), videoAvailable);
			User::Leave(KErrGeneral);
			}
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->GetNumberOfVideoFrames(num)")));
		if (num != 265)
			{
			ERR_PRINTF2(_L("file h263-no-audio.MP4 has 265 video frames. Got %d instread"), num);
			User::Leave(KErrGeneral);
			}
		}
	else // File contains both valid audio and video data
		{
		// If file contains avc profile main or extended, videoErr could be KErrNotFound
		if (audioErr != KErrNone || !audioAvailable)
			{
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() audio failed with %d, expected KErrNone"), audioErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected audioAvailable returns True, actual %d "), audioAvailable);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() video failed with %d, expected KErrNone;"), videoErr);
			ERR_PRINTF2(_L("aParser->GetFrameAvailability() expected videoAvailable returns True, actual %d"), videoAvailable);
			User::Leave(KErrGeneral);
			}
		if (videoErr != KErrNone)
			{
			SetTestStepError(videoErr);
			}
		else
			{
			User::LeaveIfError(CheckError(err, KErrNone, _L("aParser->GetNumberOfVideoFrames(num)")));
			}
		}
	}

// -----------------------------------------------------------------------------
// Retrieve video frame size & start time without parsing the content file frame 
// by frame in file mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0033::C3GPLibParser_0033()
	{	
	}

TVerdict C3GPLibParser_0033::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoInfoL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0033::VideoInfoL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	
	TUint num; 
	TInt num1 = 0;
	TUint timeStampInMs = 0; 
	TUint timeStampInTimescale = 0; 
	TUint timeStampInMs1 = 0; 
	TUint timeStampInTimescale1 = 0; 
	TUint frameIndex;
	TUint frameSize;
	TBool keyFrame;
	User::LeaveIfError(aParser->GetNumberOfVideoFrames(num));
	
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	buffer.CreateL(0);
	TInt err = aParser->ReadVideoFrame(buffer, keyFrame, timeStampInMs, timeStampInTimescale);
	if (err != KErrOverflow)
		{
		ERR_PRINTF2(_L("aParser->ReadVideoFrame() expected KErrOverflow, actual %d"), err);
		User::Leave(err);
		}
	User::LeaveIfError(aParser->GetVideoSampleEntryIndex(frameIndex));
	CleanupStack::PopAndDestroy(&buffer);
	
	
	err = KErrNone;
	while (err == KErrNone)
		{
		timeStampInMs = 0; 
		timeStampInTimescale = 0; 
		timeStampInMs1 = 0; 
		timeStampInTimescale1 = 0; 

		if (num1 < num)
			{
			User::LeaveIfError(aParser->GetVideoFrameSize(num1, frameSize));
			User::LeaveIfError(aParser->GetVideoFrameStartTime(num1, timeStampInMs, timeStampInTimescale));
			}
		else
			{
			err = aParser->GetVideoFrameStartTime(num1, timeStampInMs, timeStampInTimescale);
			err = aParser->GetVideoFrameSize(num1, frameSize);
			}
		RBuf8 buffer1;
		CleanupClosePushL(buffer1);
		buffer1.CreateL(frameSize);
		err = aParser->ReadVideoFrame(buffer1, keyFrame, timeStampInMs1, timeStampInTimescale1);
		
		if (timeStampInMs != timeStampInMs1)
			{
			ERR_PRINTF3(_L("Two timestamps should be the same. timeStampInMs %d, timeStampInMs1 %d"), timeStampInMs, timeStampInMs1);
			User::Leave(KErrGeneral);
			}
		CleanupStack::PopAndDestroy(&buffer1);
		num1++;
		}
	
	if (err != KErrNotFound)
		{
		ERR_PRINTF2(_L("expected KErrNotFound when finishing reading video frames. Actual err %d"), err);
		User::Leave(KErrGeneral);
		}
	
	if ((num1 - 1) != num)
		{
		ERR_PRINTF3(_L("Two numOfFrame should be the same. num %d, num1 %d"), num, num1);
		User::Leave(KErrGeneral);
		}
	
	// use an invalid index to attempt retrieving video frame size & start time.  Both should failed with error.
	err = aParser->GetVideoFrameSize((num + 100), frameSize);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("GetVideoFrameSize")));
	
	err = aParser->GetVideoFrameStartTime((num + 5), timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, KErrGeneral, _L("GetVideoFrameStartTime")));
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Retrieve video frame size & start time without parsing the content file frame 
// by frame in buffe mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0034::C3GPLibParser_0034()
	{	
	}

TVerdict C3GPLibParser_0034::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoInfoBufferModeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0034::VideoInfoBufferModeL(C3GPParse* aParser)
	{
	// buffer mode
	ParserOpenL(aParser);
	FileOpenL();
	
	TUint num; 
	TUint timeStampInMs = 0; 
	TUint timeStampInTimescale = 0; 
	TUint frameSize;
	
	// Insert 1K data into the library
	TInt pos = 0;
	TInt bufferSize = KBufferSize;
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	TInt err = aParser->GetNumberOfVideoFrames(num);
	if (err != KErrNone && err != KErrNotSupported && err != KErr3gpLibMoreDataRequired)
		{
		SetTestStepError(err);
		User::LeaveIfError(err);
		}
	
	while (err == KErr3gpLibMoreDataRequired )
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		err = aParser->GetNumberOfVideoFrames(num);
		}

	if (err != KErrNone)
		{
		SetTestStepError(err);
		}
	else 
		{
		for (TUint i = 0; i < num; i++)
			{
			User::LeaveIfError(aParser->GetVideoFrameSize(i, frameSize));
			User::LeaveIfError(aParser->GetVideoFrameStartTime(i, timeStampInMs, timeStampInTimescale));	
			}
		}
	
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Retrieve user atoms in all the possible locations 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0035::C3GPLibParser_0035()
	{	
	}

TVerdict C3GPLibParser_0035::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetUserDataAtomL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0035::GetUserDataAtomL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);

	TUint32 udtType = 0x6d657461;  // The udt type of test files is 'meta'
	T3GPUdtaLocation location = E3GPUdtaAudioTrak;
	TUint indexMoov = 0;
	TUint indexAudio = 0;
	TUint indexVideo = 0;
	
	TInt err = CallUDTApi(aParse, udtType, indexAudio, location); 
	if (err == KErrNone)
		{
		location = E3GPUdtaMoov;
		err = CallUDTApi(aParse, udtType, indexMoov, location); 
		
		if (err == KErrNone)
			{
			// By default, the location is video track
			err = CallUDTApi(aParse, udtType, indexVideo); 
			}
		if (err == KErrNone)
			{
			// Test file KMultiUdta contains 1 udta at E3GPUdtaMoov, 5 udta at E3GPUdtaAudioTrak and 
			// 3 udta at E3GPUdtaVideoTrak			
			if (iTestSection.Compare(KMultiUdta) == 0 && (indexMoov != 0 || indexAudio != 4 || indexVideo != 2))
				{
				SetTestStepError(KErrNotFound);
				}
			}
		}

	SetTestStepError(err);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Retrieve user atoms from a valid mp4/3gp file containing multiple entries of 
// the same user atom type within a user atom
// -----------------------------------------------------------------------------
//
C3GPLibParser_0036::C3GPLibParser_0036()
	{	
	}

TVerdict C3GPLibParser_0036::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, GetUserDataAtomL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0036::GetUserDataAtomL(C3GPParse* aParse)
	{
	// file mode
	ParserOpenFileL(aParse);
	
	TUint index;
	TUint32 udtType = 0x6d657461;  // The udt type of test files could be 'meta'
	T3GPUdtaLocation location = E3GPUdtaMoov; // The test files should not contain udta in audio trak
	
	// First read user data atom with empty buffer. It should return KErrOverflow
	RBuf8 buffer;	
	CleanupClosePushL(buffer);
	buffer.CreateL(0);	
	TInt err = aParse->GetUserDataAtom(udtType, location, buffer, index);
	buffer.Close();
	CleanupStack::PopAndDestroy(&buffer);
	
	if (err != KErrOverflow)
		{
		ERR_PRINTF2(_L("GetUserDataAtom with empty buffer should return KErrOverflow, failed with err %d"), err);
		User::Leave(err);
		}
	
	err = CallUDTApi(aParse, udtType, index); 	// By default, the location is videoTrack
	
	if (err == KErrNotFound || err == KErrNone)
		{
		err = CallUDTApi(aParse, udtType, index, location);
		
		if (err == KErrNotFound || err == KErrNone)
			{
			location = E3GPUdtaAudioTrak;
			err = CallUDTApi(aParse, udtType, index, location); 
			}
		}

	SetTestStepError(err);
	
	ParserCompleteL(aParse);
	}

// -----------------------------------------------------------------------------
// Retrieve video frame dependencies  
// -----------------------------------------------------------------------------
//
C3GPLibParser_0037::C3GPLibParser_0037()
	{	
	}

TVerdict C3GPLibParser_0037::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoDependencyL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0037::VideoDependencyL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	
	TUint num; 
	User::LeaveIfError(aParser->GetNumberOfVideoFrames(num));
	
	TInt err = KErrNone;
	T3GPFrameDependencies dependencies;
	for (TInt i = 0; i < num; i++)
		{
		err = aParser->GetVideoFrameDependencies(dependencies);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("GetVideoFrameDependencies failed with %d"), err);
			SetTestStepError(err);
			break;
			}
		}
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Get Video Frame Properties 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0038::C3GPLibParser_0038()
	{	
	}

TVerdict C3GPLibParser_0038::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, VideoInfoL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0038::VideoInfoL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	
	TUint num; 
	TUint timeStampInMs = 0; 
	TUint timeStampInTimescale = 0; 
	TUint frameIndex;
	TBool keyFrame;
	User::LeaveIfError(aParser->GetNumberOfVideoFrames(num));
	User::LeaveIfError(aParser->GetVideoSampleEntryIndex(frameIndex));
	
	TInt err = KErrNone;
	for (TInt i = 0; i < num; i++)
		{
		err = aParser->GetVideoFrameKeyType(i, keyFrame);
		User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoFrameKeyType()")));
		
		err = aParser->GetVideoFrameStartTime(i, timeStampInMs, timeStampInTimescale);
		User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoFrameStartTime()")));	
		}
	
	TUint startIndex = 0;
	RArray<T3GPFrameInfoParameters> array;
	CleanupClosePushL(array);
	
	err = aParser->GetVideoFrameProperties(startIndex, num, array);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoFrameProperties")));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("aParser->GetVideoFrameProperties passed."));
		}
	
	RArray<T3GPFrameInfoParameters> array1;
	CleanupClosePushL(array1);
	
	// Filled in array with some data, say array with size 3
	TInt arraySize = 3;
	T3GPFrameInfoParameters fip;
	for (TInt i = 0; i < arraySize; i++)
		{
		fip.iSize = i;
		fip.iStartTime = 1000;
		fip.iIsRandomAccessPoint = ETrue;
		array1.AppendL(fip);
		}
	
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("Filled in array with some junk data and call aParser->GetVideoFrameProperties() ago."));
		}
	
	err = aParser->GetVideoFrameProperties(startIndex, num, array1);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoFrameProperties")));
	
	// Check two arrays are the same
	if (array.Count() != array1.Count())
		{
		ERR_PRINTF3(_L("array1.Count() is %d; expected array.Count() is %d."), array1.Count(), array.Count());
		User::Leave(KErrGeneral);
		}
	for (TInt j = 0; j < array.Count(); j++)
		{
		if (array1[j].iSize != array[j].iSize || array1[j].iStartTime != array1[j].iStartTime ||
				array1[j].iIsRandomAccessPoint != array1[j].iIsRandomAccessPoint)
			{
			ERR_PRINTF1(_L("Two array are not the same."));
			User::Leave(KErrGeneral);
			}
		}
	
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("Two arrays are the same."));	
		}
	CleanupStack::PopAndDestroy(&array1);
	CleanupStack::PopAndDestroy(&array);
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Read a audio and a video frame into a buffer of filled with junk content and 
// ensure the buffer gets properly reset when retrieve video / audio frame data.
// -----------------------------------------------------------------------------
//
C3GPLibParser_0039::C3GPLibParser_0039()
	{	
	}

TVerdict C3GPLibParser_0039::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, AudioAndVideoFrameL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0039::AudioAndVideoFrameL(C3GPParse* aParser)
	{
	// file mode
	ParserOpenFileL(aParser);
	
	TUint size;	
	TInt err = aParser->GetVideoFrameSize(size);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetVideoFrameSize")));
		
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	buffer.CreateL(size + 15); // Create a buffer bigger than the size returned by GetVideoFrameSize
	// Filled in junk data into the buffer
	TChar c = 'a';
	buffer.AppendFill(c, (size + 15));
	
	TBool keyFrame;
	TUint timeStampInMs; 
	TUint timeStampInTimescale;
	err = aParser->ReadVideoFrame(buffer, keyFrame, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("ReadVideoFrame")));
	// Check the buffer size is the same as size returned by GetVideoFrameSize
	if (buffer.Length() != size)
		{
		ERR_PRINTF3(_L("buffer size is %d; expected size is %d."), buffer.Length(), size);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(&buffer);
	
	err = aParser->GetAudioFramesSize(size);
	User::LeaveIfError(CheckError(err, KErrNone, _L("GetAudioFramesSize")));
			
	RBuf8 bufferBig;
	CleanupClosePushL(bufferBig);
	bufferBig.CreateL(size + 10); // Create a buffer bigger than the size returned by GetVideoFrameSize
	// Filled in junk data into the buffer
	bufferBig.AppendFill(c, (size + 10));
	
	TInt returnedFrames;
	err = aParser->ReadAudioFrames(bufferBig, returnedFrames, timeStampInMs, timeStampInTimescale);
	User::LeaveIfError(CheckError(err, KErrNone, _L("ReadAudioFrames")));
	// Check the buffer size is the same as size returned by GetVideoFrameSize
	if (bufferBig.Length() != size)
		{
		ERR_PRINTF3(_L("buffer size is %d; expected size is %d."), buffer.Length(), size);
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(&bufferBig);
	
	ParserCompleteL(aParser);
	}

// -----------------------------------------------------------------------------
// Multiple parser working simultaneously. 
// -----------------------------------------------------------------------------
//
C3GPLibParser_0041::C3GPLibParser_0041()
	{
	}

TVerdict C3GPLibParser_0041::doTestStepPreambleL()
	{
	// Add the names of the new files to the following file list array.
	TInt numOfFiles = 2; // This is the array number. Should be the same as the number of the array.
	const TText* SelectedFiles[] = 
		{
		_S("mpeg4-aac.3gp"), // File with both audio and video
		_S("arm-nb-acelp-only.3gp")  // File with audio only
		};

	for (TInt index = 0; index < numOfFiles; index++)
		{
		// Create a zero terminated version of the file name
		TFileName inputFile(K3GPLibTestFilePath);
		inputFile += TPtrC(SelectedFiles[index]);
		inputFile.ZeroTerminate();
		
		if (index == 0)
			{
			User::LeaveIfError(iInputFile1.Create(inputFile.Length() + 1));
			iInputFile1.Copy(inputFile);	
			}
		else
			{
			User::LeaveIfError(iInputFile2.Create(inputFile.Length() + 1));
			iInputFile2.Copy(inputFile);	
			}
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0041::doTestStepPostambleL()
	{
	iInputFile1.Close();
	iInputFile2.Close();
	return TestStepResult();
	}

TVerdict C3GPLibParser_0041::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser1 = C3GPParse::NewL();
		CleanupStack::PushL(parser1);
		
		TInt err = parser1->Open(iInputFile1);
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser1->Open")));
		
		C3GPParse* parser2 = C3GPParse::NewL();
		CleanupStack::PushL(parser2);
		
		err = parser2->Open(iInputFile1);
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser2->Open")));
		
		C3GPParse* parser3 = C3GPParse::NewL();
		CleanupStack::PushL(parser3);
		
		err = parser3->Open(iInputFile2);
		User::LeaveIfError(CheckError(err, KErrNone, _L("aParser3->Open")));
		
		TRAP(err, MultipleParserL(parser1, parser2, parser3));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		ParserCompleteL(parser1);
		ParserCompleteL(parser2);
		ParserCompleteL(parser3);
		
		CleanupStack::PopAndDestroy(parser3);
		CleanupStack::PopAndDestroy(parser2);
		CleanupStack::PopAndDestroy(parser1);
		}
	return TestStepResult();
	}

void C3GPLibParser_0041::MultipleParserL(C3GPParse* aParser1, C3GPParse* aParser2, C3GPParse* aParser3)
	{
	TInt err = ReadAudioVideoFrame(aParser1);
	User::LeaveIfError(CheckError(err, KErrNotFound, _L("ReadAudioVideoFrame(aParser1)")));
	
	// Read one video frame with parser2
	err = ReadAudioVideoFrame(aParser2, EFalse, EFalse);
	User::LeaveIfError(CheckError(err, KErrNone, _L("ReadAudioVideoFrame(aParser2)")));
	
	// Read video properties with parser3. Expected failed with KErrNotSupported
	T3GPVideoType videoType;
	TUint length; 
	TReal frameRate;
	TUint avgBitRate;
	TSize videoSize;
	TUint timeScale;
	
	err = aParser3->GetVideoProperties(videoType, length, frameRate, avgBitRate, videoSize, timeScale);
	User::LeaveIfError(CheckError(err, KErrNotSupported, _L("aParser3->GetVideoProperties")));
	}

// -----------------------------------------------------------------------------
// Seek position in buffer mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0043::C3GPLibParser_0043()
	{
	}

TVerdict C3GPLibParser_0043::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, SeekBufferModeL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0043::SeekBufferModeL(C3GPParse* aParser)
	{
	// buffer mode
	ParserOpenL(aParser);
	FileOpenL();
	
	// Insert buffer data into the library
	TInt pos = 0;
	TInt bufferSize = KBufferSize;
	InsertDataL(aParser, iSourceFile, bufferSize, pos);
	
	TUint audioPos; 
	TUint videoPos;
	TUint timeStampInMs = 0; 
	
	// Seek the position
	TInt err = aParser->Seek(timeStampInMs, EFalse, audioPos, videoPos);
	while (err == KErr3gpLibMoreDataRequired)
		{
		pos = pos + bufferSize;
		InsertDataL(aParser, iSourceFile, bufferSize, pos);
		err = aParser->Seek(timeStampInMs, EFalse, audioPos, videoPos);
		}
	
	if (err != KErrNone)
		{
		SetTestStepError(err);
		}
	ParserCompleteL(aParser);
	FileClose();
	}

// -----------------------------------------------------------------------------
// Test C3GPParse with large file (>2GB)
// -----------------------------------------------------------------------------
//
C3GPLibParser_0045::C3GPLibParser_0045()
	{
	}

TVerdict C3GPLibParser_0045::doTestStepPreambleL()
	{
	C3GPLib_AsyncObserver::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
	return TestStepResult();
	}

TVerdict C3GPLibParser_0045::doTestStepPostambleL()
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

TVerdict C3GPLibParser_0045::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, TestParserWithLargeFileL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0045::TestParserWithLargeFileL(C3GPParse* aParser)
	{
	INFO_PRINTF1(_L("Test Parser With Large File (>2GB)"));
	INFO_PRINTF1(_L("Open and complete parser with 64 bit file handle"));

	ParserOpenFileHandle64L(aParser);
	ParserCompleteHandle64L(aParser);
	
	ParserOpenFileHandle64L(aParser, EFileRead | EFileShareReadersOnly);
	ParserCompleteHandle64L(aParser);
	
	ParserOpenFileHandle64L(aParser, EFileShareReadersOrWriters | EFileWrite);
	ParserCompleteHandle64L(aParser);
	
	ParserOpenFileHandle64L(aParser, EFileShareReadersOrWriters | EFileRead);
	ParserCompleteHandle64L(aParser);
	
	ParserOpenFileHandle64L(aParser, EFileShareAny  | EFileWrite);
	ParserCompleteHandle64L(aParser);
    
	ParserOpenFileHandle64L(aParser, EFileShareAny | EFileRead);
	ParserCompleteHandle64L(aParser);

	INFO_PRINTF1(_L("Test parser with 64 bit file handle"));

	// Read audio/video attributes
	ParserOpenFileHandle64L(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteHandle64L(aParser);

	// Read audio/video frames
	ParserOpenFileHandle64L(aParser);
	User::LeaveIfError(DoReadFrame(aParser));
	ParserCompleteHandle64L(aParser);

	// Asynchronous Read of audio/video frames
	ParserOpenFileHandle64L(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0045::ReadFrameAsyncL() using 64 bit file handle"));
		}
	ParserCompleteHandle64L(aParser);
	
	// Seek positions after any keyframe and immediately after a non-key frame
	ParserOpenFileHandle64L(aParser);
	DoSeekTestL(aParser);
	ParserCompleteHandle64L(aParser);	

	INFO_PRINTF1(_L("Test parser in File path"));

	// Read audio/video attributes
	ParserOpenFileL(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteL(aParser);
	
	// Read audio/video frames
	ParserOpenFileL(aParser);
	User::LeaveIfError(DoReadFrame(aParser));
	ParserCompleteL(aParser);
	
	// Asynchronous Read of audio/video frames
	ParserOpenFileL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL()"));
		}
	ParserCompleteL(aParser);

	ParserOpenFileL(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser, ETrue));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL(), then cancel the read"));
		}
	ParserCompleteL(aParser);

	INFO_PRINTF1(_L("Test parser in CAF"));
	
	// Read audio/video attributes
	ParserOpenCafLC(aParser);
	DoGetVideoAudioAttributesL(aParser);
	ParserCompleteCafL(aParser);
		    
	// Read audio/video frames
	ParserOpenCafLC(aParser);
	User::LeaveIfError(DoReadFrame(aParser));
	ParserCompleteCafL(aParser);
	
	// Asynchronous Read of audio/video frames
	ParserOpenCafLC(aParser);
	User::LeaveIfError(DoReadFrameAsync(aParser));
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF1(_L("C3GPLibParser_0014::ReadFrameAsyncL() using CAF"));
		}
	ParserCompleteCafL(aParser);	
	}

TInt C3GPLibParser_0045::DoReadFrameAsync(C3GPParse* aParser, TBool aCancel)
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
// Parse video & audio frames from >2GB file synchronously in buffer mode
// -----------------------------------------------------------------------------
//
C3GPLibParser_0047::C3GPLibParser_0047()
	{	
	}

TVerdict C3GPLibParser_0047::doTestStepL()
	{
	if(TestStepResult() == EPass)
		{
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		
		TRAPD(err, ReadFrameL(parser));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(parser);
		}
	return TestStepResult();
	}

void C3GPLibParser_0047::ReadFrameL(C3GPParse* aParser)
	{
	FileOpen64L();
	ReadFrameBufferModeL(aParser);
	FileClose64();
	}
	
// -----------------------------------------------------------------------------
// Retrieve User Data Atoms with Multiple atoms
// -----------------------------------------------------------------------------
//
_LIT(KAtomFileNamePrefix, "AtomFile");

C3GPUserDataAtomRetrieval::C3GPUserDataAtomRetrieval()
	{		
	}
		
TVerdict C3GPUserDataAtomRetrieval::doTestStepL()
	{
	// ensure test step error & result reset before each test
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	SetTestStepError(KErrNone);
		
	C3GPParse* parser = C3GPParse::NewL();
	CleanupStack::PushL(parser);

	if (iLargeFile)
		{
		User::LeaveIfError(parser->Open(iInputFile64));		
		}
	else
		{
		User::LeaveIfError(parser->Open(iInputFile));		
		}
	
	TBool keepLooking = ETrue;
	TInt atomFound = 0;
	TInt atomSize = 0;
	TUint atomIndex = 0;
	TUint atomHighestIndex1 = 0;
	TUint atomHighestIndex2 = 0;
	TInt err = KErrNone;
	RFile atomFile;
	RBuf8 atomContent;
	RBuf8 fileContent;
	CleanupClosePushL(fileContent);	
	CleanupClosePushL(atomFile);
	CleanupClosePushL(atomContent);		
	
	while(keepLooking && err == KErrNone)
		{
		TInt atomIndexInput = atomIndex;
		err = parser->GetUserDataAtomSize(iAtomType, iAtomLocation, atomIndex, atomSize);
		if (err == KErrNone)
			{
			atomHighestIndex1 = atomIndex;
			atomIndex = atomIndexInput;
			atomContent.CreateL(atomSize);
			
			err = parser->GetUserDataAtom(iAtomType, iAtomLocation, atomContent, atomIndex);
			if (err == KErrNone)
				{
				if (atomFound == 0)
					{
					// remember the highest atom index found during first run
					atomHighestIndex2 = atomIndex;
					if (atomHighestIndex1 != atomHighestIndex2)
						{
						if (!ShouldRunOOMTest())
							{
							ERR_PRINTF1(_L("Highest Atom Index returned is different when calling GetUserDataAtomSize vs GetUserDataAtom"));							
							}
						}
					}
				atomFound++;
				
				User::LeaveIfError(atomFile.Open(iFs, (*iAtomFileArray)[atomIndexInput], EFileRead | EFileStream));
				TInt fileSize = 0;
				User::LeaveIfError(atomFile.Size(fileSize));
				if (atomContent.Size() != fileSize)
					{
					if (!ShouldRunOOMTest())
						{
						ERR_PRINTF4(_L("Atom %d retrieved is of different size as expected.  Expected=%d Retrieved=%d"), atomIndexInput, fileSize, atomContent.Size());							
						}
					SetTestStepResult(EFail);			
					keepLooking = EFalse;					
					}
				else
					{
					fileContent.CreateL(fileSize);
					User::LeaveIfError(atomFile.Read(fileContent));
					if (fileContent != atomContent)	
						{
						if (!ShouldRunOOMTest())
							{						
							ERR_PRINTF4(_L("Atom %d retrieved is of different content.  Expected=%S Retrieved=%S"), atomIndexInput, &fileContent, &atomContent);						
							}
						SetTestStepResult(EFail);			
						keepLooking = EFalse;
						}
					else
						{
						if (atomIndex != atomHighestIndex2)	
							{
							// seems like there's an error occurred, as the highest index for this atom
							// type should stay the same throughout
							keepLooking = EFalse;
							if (!ShouldRunOOMTest())
								{							
								ERR_PRINTF3(_L("Previous highest atom index is %d but now it is %d.  Data inconsistency."), atomHighestIndex2, atomIndex);
								}
							SetTestStepResult(EFail);
							}
						else
							{
							// look for the next atom of the same type now	
							atomIndex = atomIndexInput + 1;
							}	
						} 
					fileContent.Close();	
					}	
				atomFile.Close();	
				}
			else
				{
				if (!ShouldRunOOMTest())
					{				
					ERR_PRINTF4(_L("Fail to get user atom: index = %d, location = %d, type = %d"), atomIndex, iAtomLocation, iAtomType);
					}
				User::Leave(err);
				}	
				
			atomContent.Close();					
			}			
		}
	if (TestStepResult() == EPass && atomFound != iExpectedNumAtom && atomFound == (atomHighestIndex2 + 1))	
		{
		if (!ShouldRunOOMTest())
			{		
			ERR_PRINTF3(_L("Number of atoms found (%d) does not match expected # of atoms (%d)."), atomFound, iExpectedNumAtom);
			}
		SetTestStepResult(EFail);	
		}	
		
	CleanupStack::PopAndDestroy(&atomContent);
	CleanupStack::PopAndDestroy(&atomFile);
	CleanupStack::PopAndDestroy(&fileContent);
	
	parser->Complete();
		
	CleanupStack::PopAndDestroy(parser);				
	return TestStepResult();
	}
	
TVerdict C3GPUserDataAtomRetrieval::doTestStepPreambleL()
	{	
	SetTestStepResult(ETestSuiteError);					

 	#ifdef __WINSCW__
 	_LIT(KFileToParse, "FileToParseEmu");
 	#else
 	_LIT(KFileToParse, "FileToParseHw");
 	#endif		
	
	TPtrC inputFilename;
	if (!GetStringFromConfig(ConfigSection(), KFileToParse, inputFilename))
		{
		// file need to be specified
		ERR_PRINTF1(_L("Specify the file to be parsed."));
		return TestStepResult();
		}
	
	if (!GetBoolFromConfig(ConfigSection(), _L("largeFile"), iLargeFile))
		{
		ERR_PRINTF1(_L("Specify whether the file is larger than 2GB."));
		return TestStepResult();
		}
		
	User::LeaveIfError(iFs.Connect());			
	if (iLargeFile)
		{
		User::LeaveIfError(iInputFile64.Open(iFs, inputFilename, EFileRead | EFileStream | EFileShareReadersOnly));
		}
	else
		{
		User::LeaveIfError(iInputFile.Open(iFs, inputFilename, EFileRead | EFileStream | EFileShareReadersOnly));		
		}
	
	if (!GetIntFromConfig(ConfigSection(), _L("ExpectedNumOfAtom"), iExpectedNumAtom))
		{
		// Number of expected number of atoms needs to be specified
		ERR_PRINTF1(_L("Specify the number of atoms to be expected."));
		return TestStepResult();
		}
	
	iAtomFileArray = new CDesCArrayFlat(4);	
	TInt temp;
	if (!GetIntFromConfig(ConfigSection(), _L("AtomType"), temp))
		{
		// atom type to retrieve needs to be specified
		ERR_PRINTF1(_L("Specify the atoms type to be retrieved."));
		return TestStepResult();
		}
	else
		{
		iAtomType = (TUint32) temp;	
		}
		
	if (!GetIntFromConfig(ConfigSection(), _L("AtomLocation"), temp))		
		{
		// atom location to retrieve needs to be specified
		ERR_PRINTF1(_L("Specify the atoms location for the atoms to be retrieved."));
		return TestStepResult();
		}
	else
		{
		iAtomLocation = (T3GPUdtaLocation) temp;					
		}

	if (iExpectedNumAtom > 0)
		{
		TBuf<50> keyName;	
		TPtrC fileName;		
		for (TInt i = 1; i <= iExpectedNumAtom; i++)	
			{
			keyName.Zero();
			keyName.AppendFormat(_L("%S%d"), &KAtomFileNamePrefix, i);			

			if (!GetStringFromConfig(ConfigSection(), keyName, fileName))
				{
				// atom content file needs to be specified
				ERR_PRINTF1(_L("Specify the file name that contents the matching atom content."));
				return TestStepResult();					
				}
			
			iAtomFileArray->AppendL(fileName);
			}		
		}
		
	SetTestStepResult(EPass);							
	return TestStepResult();
	}
		
TVerdict C3GPUserDataAtomRetrieval::doTestStepPostambleL()
	{
	if (iAtomFileArray)
		{
		iAtomFileArray->Reset();
		delete iAtomFileArray;			
		}
	
	if (iLargeFile)
		{	
		iInputFile64.Close();		
		}
	else
		{
		iInputFile.Close();		
		}
	iFs.Close();
	
	return TestStepResult();
	}
