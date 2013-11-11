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

#include "tsu_3gplibrary_parse_compose.h"

const TUint16 KAudioModeSet = 0x81ff;
const TReal KMillisecondsInSecond = 1000;
const TReal KOneHalf = 0.5;
const TInt KLargeFileWriteBufferSize = 16384;  // 16K
const TInt KLargeFileWriteBufferMaxCount = 15;

_LIT( KAvcBaseline, "Baseline" );
_LIT( KAvcMain, "Main" );
_LIT( KAvcExtended, "Extended" );
_LIT( KAvcHigh, "High" );

const static TDesC* KAvcProfileNames[] = {
        &KAvcBaseline,
        &KAvcMain,
        &KAvcExtended,
        &KAvcHigh
    };


C3GPLibParseComposeFile::C3GPLibParseComposeFile() :
	iCompareOriginal(EFalse),
	iLargeFile(EFalse)	
	{
	}

TVerdict C3GPLibParseComposeFile::doTestStepPreambleL()
	{
	#ifdef __WINSCW__
	_LIT(KInputDir, "inputDirEmul");
	_LIT(KOutputDir, "outputDirEmul");
	#else
	_LIT(KInputDir, "inputDirHw");
	_LIT(KOutputDir, "outputDirHw");
	#endif
	// ensure test always starts with clean results
	SetTestStepResult(EPass);
	SetTestStepError(KErrNone);
	
	// ensure there's always a Active Scheduler for the Composer
	if (!CActiveScheduler::Current())
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);	
		}
	
	User::LeaveIfError(iFs.Connect());
	// retrieve the input files' directory 
	TPtrC inputFileDir;
	TBool composeFile = EFalse;
	TPtrC outputFileDir;					
	if (GetStringFromConfig(ConfigSection(), KInputDir, inputFileDir))
		{
		if (inputFileDir.Length() == 0)
			{
			ERR_PRINTF1(_L("Ensure input directory is specified."));
			SetTestStepResult(EInconclusive);			
			return TestStepResult();
			}
		else 
			{
			iInputDir.CreateL(inputFileDir);
			}
		
		GetIntFromConfig(ConfigSection(), _L("expectedFailure"), iExpectedNumberOfFailure);
		
		if (GetBoolFromConfig(ConfigSection(), _L("composeFile"), composeFile))
			{
			if (composeFile)
				{
				if (GetStringFromConfig(ConfigSection(), KOutputDir, outputFileDir))
					{
					if (outputFileDir.Length() == 0)
						{
						ERR_PRINTF1(_L("Ensure output directory is specified."));
						SetTestStepResult(EInconclusive);			
						return TestStepResult();
						}
					else if (outputFileDir == inputFileDir)
						{
						ERR_PRINTF1(_L("Ensure output directory is not the same as input directory."));
						SetTestStepResult(EInconclusive);			
						return TestStepResult();						
						}
					else if (!GetIntFromConfig(ConfigSection(), _L("fileFormat"), iFileFormat))
						{
						ERR_PRINTF1(_L("Ensure a output file format is specified."));
						SetTestStepResult(EInconclusive);			
						return TestStepResult();												
						}
					
					GetIntFromConfig(ConfigSection(), _L("composeFlag"), iComposeFlag);
					GetBoolFromConfig(ConfigSection(), _L("compareOriginal"), iCompareOriginal);
					GetBoolFromConfig(ConfigSection(), _L("largeFile"), iLargeFile);					
					}
				else
					{
					ERR_PRINTF1(_L("Ensure output directory is specified."));
					SetTestStepResult(EInconclusive);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("Ensure input directory is specified."));
		SetTestStepResult(EInconclusive);
		}
	
	if (TestStepResult() == EPass)
		{
		// Create a list of all files contained in the input directory
		User::LeaveIfError(iFs.GetDir(inputFileDir, KEntryAttNormal, ESortNone, iDirList));			
		
		if (!iDirList || iDirList->Count() == 0)
			{
			ERR_PRINTF1(_L("Input directory is empty."));
			SetTestStepResult(EInconclusive);		
			}
		}
	
	if (TestStepResult() == EPass)
		{
		iParser = C3GPParse::NewL();
		if (composeFile)
			{
			if (iLargeFile)
				{
				iComposer = C3GPCompose::NewL(KLargeFileWriteBufferSize, KLargeFileWriteBufferMaxCount);				
				}
			else
				{
				iComposer = C3GPCompose::NewL();				
				}

			iOutputDir.CreateL(outputFileDir);
			TInt err = iFs.MkDirAll(iOutputDir);
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}
			}
		}
	
	return TestStepResult();
	}		

TVerdict C3GPLibParseComposeFile::doTestStepL()
	{
	if( TestStepResult() == EPass)
		{
		TInt failCount = 0;
		TInt fileCount = 0;
		RBuf filePath;
		CleanupClosePushL(filePath);
		filePath.CreateL(KMaxFileName);
		
		for (TInt i = 0; i < iDirList->Count(); i++)
			{
			const TEntry& entry = (*iDirList)[i];
			if (!entry.IsDir())
				{
				fileCount++;
				
				filePath.Copy(iInputDir);
				filePath.Append(entry.iName);
				
				INFO_PRINTF2(_L("ParseAndComposeL: file=%S"), &filePath);				
				TRAPD(err, ParseFileL(filePath));
				INFO_PRINTF2(_L("ParseAndComposeL returns: err = %d"), err);
				if( err != KErrNone )
					{
					iParser->Complete();
					if (iComposer)
						{
						iComposer->Complete();
						}
					failCount++;
					}
				else
					{
					if (iCompareOriginal && iComposer)
						{
						if (!CompareInputOuputFileL(filePath))
							{
							ERR_PRINTF1(_L("Input & output file is not the same"));
							failCount ++;
							}
						else
							{
							INFO_PRINTF1(_L("Input & output file is the same."));
							}
						}
					}				
				}
			}
		
		// clean up
		CleanupStack::PopAndDestroy(&filePath);
		
		INFO_PRINTF3(_L("%d of %d files failed"), failCount, fileCount);
		if (failCount != iExpectedNumberOfFailure)
			{
			ERR_PRINTF2(_L("Expected failure: %d - Test Failed"), iExpectedNumberOfFailure);
			SetTestStepResult(EFail);
			}		
		}
	
	return TestStepResult();
	}

TVerdict C3GPLibParseComposeFile::doTestStepPostambleL()
	{
	if( iScheduler )
		{
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		}
	
	iOutputDir.Close();
	iInputDir.Close();
	iComposedFile.Close();
	
	delete iDirList;
	
	iParsedFileHandle64.Close();
	iComposedFileHandle64.Close();
	iFs.Close();
			
	delete iParser;	
	if (iComposer)
		{
		delete iComposer;
		}	
	
	return TestStepResult();
	}

void C3GPLibParseComposeFile::ParseFileL(const TDesC& aInputFile)
	{
	ASSERT(iParser);
	TInt err = 0;
	
	if (iLargeFile)
		{
		iParsedFileHandle64.Close();
		err = iParsedFileHandle64.Open(iFs, aInputFile, EFileShareReadersOrWriters);
		if (err == KErrNone)
			{
			err = iParser->Open(iParsedFileHandle64);		
			}
		}
	else
		{
		err = iParser->Open(aInputFile);		
		}
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("C3GPParse::Open() returns %d"), err);
		User::Leave(err);
		}
	
	//
	// Retrieve Video Properties
	//	
	T3GPVideoPropertiesBase* videoProperties = NULL;
	CleanupStack::PushL(videoProperties);
	
	RBuf8 videoDecoderSpecificInfo;
	CleanupClosePushL(videoDecoderSpecificInfo);
	
	TUint videoLengthInMs = 0;
		
	err = ParseVideoProperties(*iParser, videoDecoderSpecificInfo, videoProperties, videoLengthInMs);
	if (err != KErrNone && err != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseVideoProperties failed: err = %d"), err);
		User::Leave(err);
		}
	
	//
	// Retrieve Audio Properties
	//
	T3GPAudioPropertiesBase* audioProperties = NULL;
	CleanupStack::PushL(audioProperties);	

	RBuf8 audioDecoderSpecificInfo;
	CleanupClosePushL(audioDecoderSpecificInfo);
	
	TUint audioLengthInMs = 0;
	err = ParseAudioProperties(*iParser, audioDecoderSpecificInfo, audioProperties, audioLengthInMs);
	if( err != KErrNone && err != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseAudioProperties failed: err = %d"), err);		
		User::Leave(err);
		}

	if (!videoProperties && !audioProperties)
		{
		ERR_PRINTF1(_L("File contains neither video nor audio data"));
		User::Leave(KErrNotFound);		
		}
	
	//
	// Prepare for the Composer if file output is expected 
	//	
	if (iComposer)
		{
		RBuf outputFile;
		CleanupClosePushL(outputFile);
		
		outputFile.CreateL(KMaxFileName);
		outputFile.Copy(iOutputDir);
		
		TParsePtrC parsePtr(aInputFile);
		TPtrC name = parsePtr.NameAndExt();
				
		outputFile.Append(parsePtr.NameAndExt());

		if (iLargeFile)
			{
			iComposedFileHandle64.Close();
			err = iComposedFileHandle64.Create(iFs, outputFile, EFileShareAny|EFileStream|EFileWrite);
			if (err == KErrAlreadyExists)
				{
				err = iComposedFileHandle64.Replace(iFs, outputFile, EFileShareAny|EFileStream|EFileWrite);
				}
			
			if (err == KErrNone)
				{
				err = iComposer->Open((T3GPFileFormatType)iFileFormat, videoProperties, audioProperties, iComposedFileHandle64, iComposeFlag);
				}
			}
		else
			{			
			err = iComposer->Open((T3GPFileFormatType)iFileFormat, videoProperties, audioProperties, outputFile, iComposeFlag);
			}
				
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("C3GPCompose::Open failed: err = %d"), err);		
			User::Leave(err);			
			}
		
		CleanupStack::PopAndDestroy(&outputFile);
		}
	
	//
	// Parse and compose (if specified) video / audio frame data 
	//
	ReadWriteAudioVideoFramesL(videoProperties, audioProperties, videoLengthInMs, audioLengthInMs);
	
	//
	// Clean up
	//
	CleanupStack::PopAndDestroy(4);	 
									// audioDecoderSpecificInfo
									// audioProperties 
									// videoDecoderSpecificInfo 
									// videoProperties
	
	if (iComposer)
		{
		err = iComposer->Complete();
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("aComposer->Complete() failed"));
			User::Leave(err);
			}
			
		if (iLargeFile)
			{
			iComposedFileHandle64.Close();
			}
		}
	
	err = iParser->Complete();
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("aParser->Complete() failed"));
		User::Leave(err);
		}

	if (iLargeFile)
		{
		iParsedFileHandle64.Close();
		}
	
	INFO_PRINTF1(_L("C3GPLibParseComposeFile::ParseAndComposeL OUT"));		
	}

void C3GPLibParseComposeFile::ReadWriteAudioVideoFramesL(const T3GPVideoPropertiesBase* aVideoProperties, 
														 const T3GPAudioPropertiesBase* aAudioProperties,
														 TUint aVideoLengthInMs,
														 TUint aAudioLengthInMs)
	{
	ASSERT(iParser);

	TUint videoLengthInTS = 0;
	TUint audioLengthInTS = 0;
	if (aVideoProperties)
		{
        // The "+KOneHalf" in the following calculation has the effect of rounding
        // to the nearest integer instead of just flooring it when converting from
        // TReal to TUint.
        // This is to avoid videoLengthInTS being off by one and leading to false
        // positives when the video properties are compared later on.
		videoLengthInTS = (TUint) ((TReal)(aVideoLengthInMs * aVideoProperties->iTimescale) / KMillisecondsInSecond + KOneHalf );
		}
	if (aAudioProperties)
		{
		// The "+KOneHalf" in the following calculation has the effect of rounding
		// to the nearest integer instead of just flooring it when converting from
		// TReal to TUint.
		// This is to avoid audioLengthInTS being off by one and leading to false
		// positives when the audio properties are compared later on.
		audioLengthInTS = (TUint) ((TReal)(aAudioLengthInMs * aAudioProperties->iTimescale) / KMillisecondsInSecond + KOneHalf );
		}
	
	TInt framePerSample1 = 0;
	TInt framePerSample2 = 0;
	TUint audioTS1 = 0;
	TUint audioTS2 = 0;
	RBuf8 audioBuf1;	
	RBuf8 audioBuf2;
	CleanupClosePushL(audioBuf1);
	CleanupClosePushL(audioBuf2);

	TUint videoTS1;
	TUint videoTS2;
	TBool keyFrame1;
	TBool keyFrame2;
	T3GPFrameDependencies dep1;
	T3GPFrameDependencies dep2;
	RBuf8 videoBuf1;
	RBuf8 videoBuf2;
	CleanupClosePushL(videoBuf1);		
	CleanupClosePushL(videoBuf2);	

	TInt err = KErrNone;
	T3GPFrameType frameType;
	
	RArray<T3GPFrameType> frameTypeArray(10);
	CleanupClosePushL(frameTypeArray);
	
	TInt videoFrameCount = 0;
	TInt audioFrameCount = 0;
	
	while (err == KErrNone)
		{
		err = iParser->GetFrameType(frameType);
		if (err == KErrNone)
			{		
			switch(frameType)
				{
				case E3GPAudio:
					audioFrameCount++;
					frameTypeArray.AppendL(frameType);		
					err = ReadAudioFrames(*iParser, audioBuf1, framePerSample1, audioTS1);
					audioBuf1.Close();				
					break;
					
				case E3GPVideo:
					videoFrameCount++;
					frameTypeArray.AppendL(frameType);
					err = ReadVideoFrame(*iParser, videoBuf1, keyFrame1, videoTS1, dep1);
					videoBuf1.Close();				
					break;
					
				default:
					ERR_PRINTF1(_L("GetFrameType retrieves an unsupported frame type"));
					User::Leave(KErrUnknown);
					break;
				}
			
			// if there's anything wrong w/ retrieving the frame, exit the function now
			User::LeaveIfError(err);
			}
		}
	
	INFO_PRINTF2(_L("ReadWriteAudioVideoFramesL read %d video frames."), videoFrameCount);
	INFO_PRINTF2(_L("ReadWriteAudioVideoFramesL read %d audio frames."), audioFrameCount);
		
	// reset the video and audio cursor at the beginning of the clip 
	TUint videoPos = 0;
	TUint audioPos = 0;
	iParser->Seek(0, EFalse, videoPos, audioPos);
	
	videoFrameCount = 0;
	audioFrameCount = 0;
	
	if (iComposer)
		{	
		// if the data should be supplied into a composer to construct an output file
		TInt index = 0;	
		while (index < frameTypeArray.Count())
			{
			frameType = frameTypeArray[index++]; 
			switch (frameType)
				{
				case E3GPAudio:
					if (audioBuf1.Length() == 0)
						{					
						User::LeaveIfError(ReadAudioFrames(*iParser, audioBuf1, framePerSample1, audioTS1));
						}
					err = ReadAudioFrames(*iParser, audioBuf2, framePerSample2, audioTS2);
					
					if (audioBuf1.Length() > 0 && audioBuf2.Length() > 0 || 
						audioBuf1.Length() > 0 && err == KErrNotFound)
						{
						TUint duration = 0;
						if (audioBuf2.Length() == 0)
							{
							// duration for the last batch of audio frames = total audio length - the 2nd last audio frame
							duration = audioLengthInTS - audioTS1;
							}
						else
							{
							duration = audioTS2 - audioTS1;			
							}	
							
						User::LeaveIfError(iComposer->WriteAudioFrames(audioBuf1, duration));
						audioFrameCount++;
						
						audioBuf1.Close();
						audioBuf1.Swap(audioBuf2);
						framePerSample1 = framePerSample2;
						audioTS1 = audioTS2;
						}
					break;
					
				case E3GPVideo:
					if (videoBuf1.Length() == 0)
						{
						User::LeaveIfError(ReadVideoFrame(*iParser, videoBuf1, keyFrame1, videoTS1, dep1));
						}					
					err = ReadVideoFrame(*iParser, videoBuf2, keyFrame2, videoTS2, dep2);
					
					if (videoBuf1.Length() > 0 && videoBuf2.Length() > 0 || 
						videoBuf1.Length() > 0 && err == KErrNotFound)
						{
						TUint duration = 0;
						if (videoBuf2.Length() == 0)
							{
							// duration for the last batch of audio frames = total audio length - the 2nd last audio frame
							duration = videoLengthInTS - videoTS1;
							}
						else
							{
							duration = videoTS2 - videoTS1;			
							}	
							
						User::LeaveIfError(iComposer->WriteVideoFrame(videoBuf1, duration, keyFrame1, dep1));
						videoFrameCount++;
						
						videoBuf1.Close();
						videoBuf1.Swap(videoBuf2);
						keyFrame1 = keyFrame2;
						videoTS1 = videoTS2;
						dep1.iDependsOn = dep2.iDependsOn;
						dep1.iIsDependedOn = dep2.iIsDependedOn;
						dep1.iHasRedundancy = dep2.iHasRedundancy;		
						}
					break;
					
				default:
					ERR_PRINTF1(_L("Unknown frame type detected."));
					User::Leave(KErrUnknown);
					break;
				}
			}	// end of while loop
		}	// if (iComposer)
	
	CleanupStack::PopAndDestroy(5);
	
	INFO_PRINTF2(_L("ReadWriteAudioVideoFramesL wrote %d video frames."), videoFrameCount);
	INFO_PRINTF2(_L("ReadWriteAudioVideoFramesL wrote %d audio frames."), audioFrameCount);	
	}

TInt C3GPLibParseComposeFile::GetAudioDecoderSpecificInfo(RBuf8& aBuffer)
	{
	ASSERT(iParser);
	
	TInt size;
	TInt err = iParser->GetAudioDecoderSpecificInfoSize(size);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("GetAudioDecoderSpecificInfoSize() failed: %d"), err);
		return err;
		}

	err = aBuffer.Create(size);
	if( err != KErrNone )
		{
		ERR_PRINTF1(_L("Create buffer failed"));
		return err;
		}

	err = iParser->GetAudioDecoderSpecificInfo(aBuffer);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("GetAudioDecoderSpecificInfo() failed: %d"), err);
		}
	
	return err;
	}

TInt C3GPLibParseComposeFile::ParseVideoProperties(C3GPParse& aParser,
												   RBuf8& aDecoderSpecificInfo,
												   T3GPVideoPropertiesBase*& aProperties,
												   TUint& aLengthInMs) 
	{
	ASSERT(iParser);
	
	T3GPVideoType type; 
	TReal frameRate;
	TUint avgBitRate;
	TSize size;
	TUint timeScale;	
	
	TInt err = aParser.GetVideoProperties(type, aLengthInMs, frameRate, avgBitRate, size, timeScale);
	if ((err != KErrNone) && (err != KErrNotSupported))
		{
		ERR_PRINTF2(_L("aParser->GetVideoProperties failed: %d"), err);
		return err;
		}

	switch(type)
		{
		case E3GPMpeg4Video:
			{
			INFO_PRINTF1(_L("Video Type: Mpeg4"));			
			err = GetVideoDecoderSpecificInfo(aDecoderSpecificInfo);		
			if (err == KErrNone)
				{
				aProperties = new T3GPVideoPropertiesMpeg4Video(timeScale, size,
						64000, avgBitRate, aDecoderSpecificInfo);
				if (!aProperties)
					{
					ERR_PRINTF1(_L("T3GPVideoPropertiesMpeg4Video allocation failed"));
					err = KErrNoMemory;
					}
				}
			break;			
			}
		
		case E3GPAvcProfileBaseline:
		case E3GPAvcProfileMain:
		case E3GPAvcProfileExtended:
		case E3GPAvcProfileHigh:
			{
			err = GetVideoDecoderSpecificInfo(aDecoderSpecificInfo);
			if (err == KErrNone)
				{
				INFO_PRINTF2(_L("Video Type: Avc Profile %S"), KAvcProfileNames[type-E3GPAvcProfileBaseline] );
				aProperties = new T3GPVideoPropertiesAvc(timeScale, size, aDecoderSpecificInfo);
				if (!aProperties)
					{
					ERR_PRINTF1(_L("T3GPVideoPropertiesAvc allocation failed"));
					err = KErrNoMemory;
					}
                else
                    {
                    // T3GPVideoPropertiesAvc defaults the video type to AVC baseline profile.
                    // Need to override that here because we want to check for the specific
                    // profile in this test.
                    aProperties->iType = type;
                    }
				}
			break;
			}
			
		case E3GPH263Profile0:
		case E3GPH263Profile3:
			{
			INFO_PRINTF1(_L("Video Type: H263"));			
			T3GPVideoPropertiesH263::TProfile profile = T3GPVideoPropertiesH263::EProfile0;
			if (type == E3GPH263Profile3)
				{
				profile = T3GPVideoPropertiesH263::EProfile3;
				}
			
			TInt videoLevel;
			err = iParser->GetH263VideoLevel(videoLevel);
			if( err != KErrNone )
				{
				ERR_PRINTF1(_L("aParser->GetH263VideoLevel() failed"));
				}
			else
				{			
				aProperties = new T3GPVideoPropertiesH263(timeScale, size, videoLevel, profile);
				if( !aProperties )
					{
					ERR_PRINTF1(_L("T3GPVideoPropertiesH263 allocation failed"));
					err = KErrNoMemory;
					}
				}
			break;			
			}
		
		case E3GPNoVideo:
			INFO_PRINTF1(_L("Video Type: None"));
			break;
			
		default:
			err = KErrNotSupported;
			break;
		}
	
	return err;
	}

TInt C3GPLibParseComposeFile::GetVideoDecoderSpecificInfo(RBuf8& aBuffer)
	{	
	TInt size;
	TInt err = iParser->GetVideoDecoderSpecificInfoSize(size);
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("GetVideoDecoderSpecificInfoSize() failed"));
		return err;
		}
	
	err = aBuffer.Create(size);
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Create buffer failed"));
		return err;
		}

	err = iParser->GetVideoDecoderSpecificInfo(aBuffer);
	if( err != KErrNone )
		{
		ERR_PRINTF1(_L("GetVideoDecoderSpecificInfo() failed"));
		}
	
	return err;
	}

TInt C3GPLibParseComposeFile::ParseAudioProperties(C3GPParse& aParser, 
												   RBuf8& aAudioDecoderSpecificInfo,
												   T3GPAudioPropertiesBase*& aAudioProperties,
												   TUint& aLength)
	{
	
	ASSERT(iParser);
	
	T3GPAudioType type; 
	TInt audioFPS; 
	TUint audioAvgBitRate; 
	TUint timeScale;	
	
	TInt err = aParser.GetAudioProperties(type, aLength, audioFPS, audioAvgBitRate, timeScale);
	if(( err != KErrNone ) && ( err != KErrNotSupported ))
		{
		ERR_PRINTF2(_L("GetAudioProperties() failed: %d"), err);
		return err;
		}

	switch(type)
		{
		case E3GPMpeg4Audio:
			{
			INFO_PRINTF1(_L("Audio Type: Mpeg4"));
			err = GetAudioDecoderSpecificInfo(aAudioDecoderSpecificInfo);
			if (err == KErrNone)
				{
				aAudioProperties = new T3GPAudioPropertiesMpeg4Audio(timeScale, aAudioDecoderSpecificInfo);
				if( !aAudioProperties )
					{
					ERR_PRINTF1(_L("T3GPAudioPropertiesMpeg4Audio allocation failed"));
					err = KErrNoMemory;
					}
				}
			break;
			}
			
		case E3GPQcelp13K:
			{
			INFO_PRINTF1(_L("Audio Type: Qcelp13K"));
			T3GPQcelpStorageMode mode;
			err = iParser->GetQcelpStorageMode(mode);
			if (err != KErrNone)
				{
				ERR_PRINTF1(_L("GetQcelpStorageMode failed"));
				}
			else 
				{
				if( mode == E3GPMP4AudioDescriptionBox)
					{
					err = GetAudioDecoderSpecificInfo(aAudioDecoderSpecificInfo);
					aAudioProperties = new T3GPAudioPropertiesQcelp(timeScale, audioFPS, aAudioDecoderSpecificInfo);
					}
				else
					{
					aAudioProperties = new T3GPAudioPropertiesQcelp(timeScale, audioFPS);
					}
				
				if( !aAudioProperties )
					{
					ERR_PRINTF1(_L("T3GPAudioPropertiesQcelp allocation failed"));
					err = KErrNoMemory;
					}
				}
			break;
			}
			
		case E3GPAmrNB:
			{
			INFO_PRINTF1(_L("Audio Type: AMR NB"));
			aAudioProperties = new T3GPAudioPropertiesAmr(timeScale, audioFPS, KAudioModeSet, T3GPAudioPropertiesAmr::EAmrNB);
			if( !aAudioProperties )
				{
				ERR_PRINTF1(_L("T3GPAudioPropertiesAmr allocation failed"));
				err = KErrNoMemory;
				}
			break;
			}
			
		case E3GPAmrWB:
			{
			INFO_PRINTF1(_L("Audio Type: AMR WB"));
			aAudioProperties = new T3GPAudioPropertiesAmr(timeScale, audioFPS, KAudioModeSet, T3GPAudioPropertiesAmr::EAmrWB);
			if( !aAudioProperties )
				{
				ERR_PRINTF1(_L("T3GPAudioPropertiesAmr allocation failed"));
				err = KErrNoMemory;
				}
			break;
			}
			
		case E3GPNoAudio:
			INFO_PRINTF1(_L("Audio Type: None"));			
			break;
			
		default:
			INFO_PRINTF1(_L("Audio Type: Unrecognized!"));
			err = KErrNotSupported;
			break;
		}
	
	return err;
	}

TInt C3GPLibParseComposeFile::ReadVideoFrame(C3GPParse& aParser,
											 RBuf8& aVideoBuffer, 
											 TBool& aVideoKeyFrame, 
											 TUint& aVideoTimestampInTS, 
											 T3GPFrameDependencies& aDependencies)
	{
	ASSERT(iParser);
	
	// flush all data in the buffer
	aVideoBuffer.Close();
	
	TInt err = aParser.GetVideoFrameDependencies(aDependencies);
	if (err != KErrNone)
		{
		aDependencies.iDependsOn = E3GPDependencyUnknown;
		aDependencies.iIsDependedOn = E3GPDependencyUnknown;
		aDependencies.iHasRedundancy = E3GPRedundancyUnknown;
		}
	
	TUint frameSize = 0;
	err = aParser.GetVideoFrameSize(frameSize);
	if (err == KErrNone)
		{
		if (frameSize > KMaxTInt / 2)
			{
			ERR_PRINTF1(_L("Video Frame too large!"));
			err = KErrOverflow;
			}
		else
			{
			err = aVideoBuffer.Create((TInt)frameSize);
			if (err == KErrNone)
				{
				TUint videoTimestampMS = 0;
				err = aParser.ReadVideoFrame(aVideoBuffer, aVideoKeyFrame, videoTimestampMS, aVideoTimestampInTS);				
				if (err == KErrNone)
					{
					if (err == KErrNotSupported)
						{
						// Not supported error is OK
						err = KErrNone;
						}
					else if (err != KErrNone)
						{
						ERR_PRINTF2(_L("GetVideoFrameDependencies failed with err = %d"), err);					
						}
					}
				else
					{
					ERR_PRINTF2(_L("ReadVideoFrame failed with err = %d"), err);					
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("GetVideoFrameSize fails with %d"), err);		
		}
	
	return err;	
	}

TInt C3GPLibParseComposeFile::ReadAudioFrames(C3GPParse& aParser, 
											  RBuf8& aBuffer, 
											  TInt& aFramesInSample, 
											  TUint& aTimestampInTS)
	{
	// flush all existing data
	aBuffer.Close();
	
	TUint size = 0;
	TInt err = aParser.GetAudioFramesSize(size);
	if (err == KErrNone)
		{
		if (size > KMaxTInt / 2)	
			{
			// cannot create RBuf of size > KMaxTInt / 2 
			ERR_PRINTF1(_L("Audio Frames too large!"));
			err = KErrOverflow;
			}
		else
			{
			err = aBuffer.Create((TInt)size);
			if (err == KErrNone)
				{
				TUint audioTimestampMS = 0;
				err = aParser.ReadAudioFrames(aBuffer, aFramesInSample, audioTimestampMS, aTimestampInTS);
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ReadAudioFrames failed with err = %d"), err);							
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(_L("GetAudioFramesSize failed with err = %d"), err);		
		}
	
	return err;
	}

TBool C3GPLibParseComposeFile::CompareInputOuputFileL(const TDesC& aInputFile)
	{
	ASSERT(iParser);
	TInt err = 0;

	if (iLargeFile)
		{
		iParsedFileHandle64.Close();
		err = iParsedFileHandle64.Open(iFs, aInputFile, EFileShareReadersOrWriters);
		if (err == KErrNone)
			{
			err = iParser->Open(iParsedFileHandle64);		
			}
		}
	else
		{
		err = iParser->Open(aInputFile);		
		}

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("C3GPParse 1 Open() returns %d"), err);
		User::Leave(err);
		}
	
	RBuf outputFile;
	CleanupClosePushL(outputFile);
	
	outputFile.CreateL(KMaxFileName);
	outputFile.Copy(iOutputDir);
	
	TParsePtrC parsePtr(aInputFile);
	TPtrC name = parsePtr.NameAndExt();	
	outputFile.Append(parsePtr.NameAndExt());

	C3GPParse* parser2 = C3GPParse::NewL();
	CleanupStack::PushL(parser2);

	RFile64 file2;
	if (iLargeFile)
		{
		err = file2.Open(iFs, outputFile, EFileShareReadersOrWriters);
		if (err == KErrNone)
			{
			err = parser2->Open(file2);		
			}
		}
	else
		{
		err = parser2->Open(outputFile);		
		}
		
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("C3GPParse 2 Open() returns %d"), err);
		User::Leave(err);
		}
	
	TBool result = EFalse;	
	if (CompareVideoPropertiesL(*iParser, *parser2))
		{
		if (CompareAudioPropertiesL(*iParser, *parser2))
			{
			if (CompareAudioVideoData(*iParser, *parser2))
				{
				result = ETrue;
				}			
			else
				{
				ERR_PRINTF1(_L("Audio/Video Data not matching"));
				}					
			}
		else
			{
			ERR_PRINTF1(_L("Audio Properites not matching"));
			}		
		}
	else
		{
		ERR_PRINTF1(_L("Video Properites not matching"));
		}
	
	parser2->Complete();
	iParser->Complete();
	
	if (iLargeFile)
		{
		file2.Close();
		iParsedFileHandle64.Close();
		}
	
	CleanupStack::PopAndDestroy(2);		// outputFile, parser2
	
	return result;
	}

TBool C3GPLibParseComposeFile::CompareVideoPropertiesL(C3GPParse& aParser1, 
													   C3GPParse& aParser2)
	{
	//
	// Retrieve Video Properties from Parser 1
	//	
	T3GPVideoPropertiesBase* videoProperties1 = NULL;
	CleanupStack::PushL(videoProperties1);
	
	RBuf8 videoDecoderSpecificInfo1;
	CleanupClosePushL(videoDecoderSpecificInfo1);
	
	TUint videoLengthInMs1 = 0;
		
	TInt err1 = ParseVideoProperties(aParser1, videoDecoderSpecificInfo1, videoProperties1, videoLengthInMs1);
	if (err1 != KErrNone && err1 != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseVideoProperties 1 failed: err = %d"), err1);
		User::Leave(err1);
		}
	
	//
	// Retrieve Video Properties from Parser 2
	//	
	T3GPVideoPropertiesBase* videoProperties2 = NULL;
	CleanupStack::PushL(videoProperties2);
	
	RBuf8 videoDecoderSpecificInfo2;
	CleanupClosePushL(videoDecoderSpecificInfo2);
	
	TUint videoLengthInMs2 = 0;
		
	TInt err2 = ParseVideoProperties(aParser2, videoDecoderSpecificInfo2, videoProperties2, videoLengthInMs2);
	if (err2 != KErrNone && err2 != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseVideoProperties 2 failed: err = %d"), err2);
		User::Leave(err2);
		}

	TBool result = EFalse;
	if (err1 == err2 && err1 != KErrNotSupported)
		{
		if (videoLengthInMs1 == videoLengthInMs2)
			{
			if (videoProperties1 && videoProperties2)
				{
				result = (videoProperties1->iType == videoProperties2->iType) &&
						 (videoProperties1->iSize == videoProperties2->iSize) &&
						 (videoProperties1->iTimescale == videoProperties2->iTimescale);
					
				if (result)
					{
					switch(videoProperties1->iType)
						{
						case E3GPMpeg4Video:
							{
							T3GPVideoPropertiesMpeg4Video* video1 = (T3GPVideoPropertiesMpeg4Video*)videoProperties1;
							T3GPVideoPropertiesMpeg4Video* video2 = (T3GPVideoPropertiesMpeg4Video*)videoProperties2;
							
							// NOTE: Comparison of the avg bit rate is omitted.  The reason is that the avg. bit 
							//       rate is calculated by: 
							//       video avg bit rate = stream avg bit rate - audio avg bit rate
							//       However, stream avg. bit rate is not be properly set by the 3GP composer
							//       as it is not very important.  Thus leading to the video avg. bit rate  
							//       retrieved from the output file to NOT match the original.
							result = ((video1->iMaxBitRate == video2->iMaxBitRate) && 
									 //(video1->iAvgBitRate == video2->iAvgBitRate) &&
									 (video1->iDecoderSpecificInfo == video2->iDecoderSpecificInfo)); 
							break;			
							}
								
						case E3GPAvcProfileBaseline:
						case E3GPAvcProfileMain:
						case E3GPAvcProfileExtended:
						case E3GPAvcProfileHigh:
							{
							T3GPVideoPropertiesAvc* video1 = (T3GPVideoPropertiesAvc*)videoProperties1;
							T3GPVideoPropertiesAvc* video2 = (T3GPVideoPropertiesAvc*)videoProperties2;
							
							result = (video1->iDecoderSpecificInfo == video2->iDecoderSpecificInfo); 
							break;
							}
									
						case E3GPH263Profile0:
						case E3GPH263Profile3:
							{
							T3GPVideoPropertiesH263* video1 = (T3GPVideoPropertiesH263*)videoProperties1;
							T3GPVideoPropertiesH263* video2 = (T3GPVideoPropertiesH263*)videoProperties2;

							result = (video1->iVideoLevel == video2->iVideoLevel);
							break;			
							}
						}
					}
				}			
			}		
		}	
	else if (err1 == KErrNotSupported && err2 == err1)
		{
		result = ETrue;
		}

	CleanupStack::PopAndDestroy(4);		// videoProperties1, 
										// videoProperties2, 
										// videoDecoderSpecificInfo1, 
										// videoDecoderSpecificInfo2
	
	return result;
	}

TBool C3GPLibParseComposeFile::CompareAudioPropertiesL(C3GPParse& aParser1, 
													   C3GPParse& aParser2)
	{
	//
	// Retrieve Audio Properties 1
	//
	T3GPAudioPropertiesBase* audioProperties1 = NULL;
	CleanupStack::PushL(audioProperties1);	

	RBuf8 audioDecoderSpecificInfo1;
	CleanupClosePushL(audioDecoderSpecificInfo1);
	
	TUint audioLengthInMs1 = 0;
	TInt err1 = ParseAudioProperties(aParser1, audioDecoderSpecificInfo1, audioProperties1, audioLengthInMs1);
	if( err1 != KErrNone && err1 != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseAudioProperties failed: err = %d"), err1);		
		User::Leave(err1);
		}

	//
	// Retrieve Audio Properties 2
	//
	T3GPAudioPropertiesBase* audioProperties2 = NULL;
	CleanupStack::PushL(audioProperties2);	

	RBuf8 audioDecoderSpecificInfo2;
	CleanupClosePushL(audioDecoderSpecificInfo2);
	
	TUint audioLengthInMs2 = 0;
	TInt err2 = ParseAudioProperties(aParser2, audioDecoderSpecificInfo2, audioProperties2, audioLengthInMs2);
	if( err2 != KErrNone && err2 != KErrNotSupported)
		{
		ERR_PRINTF2(_L("ParseAudioProperties failed: err = %d"), err2);		
		User::Leave(err2);
		}
	
	TBool result = EFalse;
	if (err1 == err2 && err1 != KErrNotSupported)
		{
		if (audioLengthInMs2 == audioLengthInMs1)
			{
			if (audioProperties1 && audioProperties2)
				{
				result = (audioProperties1->iType == audioProperties2->iType) &&
						 (audioProperties1->iFramesPerSample == audioProperties2->iFramesPerSample) &&
						 (audioProperties1->iTimescale == audioProperties2->iTimescale);
				
				if (result)
					{
					switch(audioProperties1->iType)
						{
						case E3GPMpeg4Audio:
							{
							T3GPAudioPropertiesMpeg4Audio* audio1 = (T3GPAudioPropertiesMpeg4Audio*) audioProperties1;
							T3GPAudioPropertiesMpeg4Audio* audio2 = (T3GPAudioPropertiesMpeg4Audio*) audioProperties2;
							result = (audio1->iDecoderSpecificInfo == audio2->iDecoderSpecificInfo);
							}
							break;							
							
						case E3GPAmrNB:
						case E3GPAmrWB:
							{
							T3GPAudioPropertiesAmr* audio1 = (T3GPAudioPropertiesAmr*) audioProperties1;
							T3GPAudioPropertiesAmr* audio2 = (T3GPAudioPropertiesAmr*) audioProperties2;
							result = (audio1->iModeSet == audio2->iModeSet);
							}
							break;														
							
						case E3GPQcelp13K:
							{
							T3GPAudioPropertiesQcelp* audio1 = (T3GPAudioPropertiesQcelp*) audioProperties1;
							T3GPAudioPropertiesQcelp* audio2 = (T3GPAudioPropertiesQcelp*) audioProperties2;
							result = ((audio1->iDecoderSpecificInfo == audio2->iDecoderSpecificInfo) &&
									 (audio1->iMode == audio2->iMode));
							}
							break;														
						}
					}
				}
			}
		}	
	else if (err1 == KErrNotSupported && err2 == err1)
		{
		result = ETrue;
		}

	CleanupStack::PopAndDestroy(4);		// audioProperties1, audioProperties2, audioDecoderSpecificInfo1, audioDecoderSpecificInfo2
	
	return result;
	}

TBool C3GPLibParseComposeFile::CompareAudioVideoData(C3GPParse& aParser1, C3GPParse& aParser2)
	{
	T3GPFrameType frameType1;
	T3GPFrameType frameType2;
	TInt err1 = KErrNone;
	TInt err2 = KErrNone;
	TBool keepLooping = ETrue;
	TBool result = EFalse;
	
	while (keepLooping)
		{
		err1 = aParser1.GetFrameType(frameType1);
		err2 = aParser2.GetFrameType(frameType2);
		
		if (err1 != err2)
			{
			break;
			}		
		else
			{
			if (err1 == KErrNotFound)
				{
				// completed looping thru all the frames
				break;
				}
			
			if (err1 != KErrNone)
				{
				// if the error is not KErrNotFound, something's wrong!
				result = EFalse;
				}			
			else if (frameType1 != frameType2)
				{			
				result = EFalse;
				}		
			else if (frameType1 == E3GPVideo)
				{
				result = CompareVideoFrame(aParser1, aParser2);
				}				
			else if (frameType1 == E3GPAudio)			
				{
				result = CompareAudioFrames(aParser1, aParser2);
				}
			
			if (!result)
				{
				break;
				}
			}
		} 
	
	return result;
	}

TBool C3GPLibParseComposeFile::CompareVideoFrame(C3GPParse& aParser1, C3GPParse& aParser2)
	{
	TUint timestamp1 = 0;
	TUint timestamp2 = 0;
	RBuf8 buf1;
	RBuf8 buf2;
	TBool keyFrame1 = EFalse;
	TBool keyFrame2 = EFalse;
	T3GPFrameDependencies dep1;
	T3GPFrameDependencies dep2;
	
	TInt err1 = ReadVideoFrame(aParser1, buf1, keyFrame1, timestamp1, dep1);
	TInt err2 = ReadVideoFrame(aParser2, buf2, keyFrame2, timestamp2, dep2);

	TBool result = EFalse;
	if (err1 == err2)
		{
		if (keyFrame1 == keyFrame2 && timestamp1 == timestamp2)
			{
			if (dep1.iDependsOn == dep2.iDependsOn && 
				dep1.iHasRedundancy == dep2.iHasRedundancy && 
				dep1.iIsDependedOn == dep2.iIsDependedOn)
				{
				if (buf1 == buf2)
					{
					result = ETrue;
					}
				else 
					{
					result = EFalse;
					}
				}
			else 
				{
				result = EFalse;
				}			
			}
		else 
			{
			result = EFalse;
			}		
		}
	else 
		{
		result = EFalse;
		}	

	// cleanup
	buf1.Close();
	buf2.Close();
	
	return result;
	}

TBool C3GPLibParseComposeFile::CompareAudioFrames(C3GPParse& aParser1, C3GPParse& aParser2)
	{
	TInt fps1 = 0;
	TInt fps2 = 0;
	TUint timestamp1 = 0;
	TUint timestamp2 = 0;
	RBuf8 buf1;
	RBuf8 buf2;
	
	TInt err1 = ReadAudioFrames(aParser1, buf1, fps1, timestamp1);
	TInt err2 = ReadAudioFrames(aParser2, buf2, fps2, timestamp2);

	TBool result = EFalse;
	if (err1 == err2)
		{
		if (fps1 == fps2 && timestamp1 == timestamp2)
			{
			if (buf1 == buf2)
				{
				result = ETrue;
				}
			else 
				{
				result = EFalse;
				}			
			}
		else 
			{
			result = EFalse;
			}		
		}
	else 
		{
		result = EFalse;
		}
	
	// cleanup	
	buf1.Close();
	buf2.Close();
	
	return result;
	}
