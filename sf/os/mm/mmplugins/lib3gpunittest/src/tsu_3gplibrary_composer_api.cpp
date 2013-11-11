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

#include <e32std.h>
#include "tsu_3gplibrary_composer_api.h"

_LIT8(KDummyData, "DummyData");
_LIT8(KFtypAtom, "ftyp");
_LIT8(K3g2Brand, "3g2a");	// 3g2a
_LIT8(K3gpBrand, "3gp");	// 3gp4, 3gp6
_LIT8(KMp4Brand, "mp42");	// mp42 

_LIT(KLargeVideoFile, "c:\\3gplibrary\\cube-xvid-640x480-10fps-10s.3gp");
const TInt KLargeFileWriteBufferSize = 16384;  // 16K
const TInt KLargeFileWriteBufferMaxCount = 15;
const TInt64 K3GigaBytes = 0xC0000000;

// -----------------------------------------------------------------------------
// C3GPLibComposeBase - this should not be used directly
// -----------------------------------------------------------------------------
//     
C3GPLibComposeBase::C3GPLibComposeBase()
	{
	}

TVerdict C3GPLibComposeBase::doTestStepPreambleL()
	{
	// ensure there's always a Active Scheduler for the Composer
	if (!CActiveScheduler::Current())
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);	
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict C3GPLibComposeBase::doTestStepPostambleL()
	{
	if (iScheduler)
		{
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		}
	
	return TestStepResult();
	}

TVerdict C3GPLibComposeBase::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	SetTestStepError(KErrNone);
	
	C3GPCompose* composer = doTestStepCreateComposerL(); 
	CleanupStack::PushL(composer);
	
	doTestStepComposeOpenL(*composer);
	if (TestStepResult() == EPass)
		{
		// set it to EFail, if both calls below does not leave, then it will be reset
		// back to EPass
		SetTestStepResult(EFail);
		doTestStepComposeWriteVideoAudioL(*composer);
		doTestStepComposeSetUserDataL(*composer);
		SetTestStepResult(EPass);
		}
		
	TInt err = composer->Complete();
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(composer);	
	
	return TestStepResult();
	}

C3GPCompose* C3GPLibComposeBase::doTestStepCreateComposerL()
	{
	return C3GPCompose::NewL();
	}

void C3GPLibComposeBase::doTestStepComposeWriteVideoAudioL(C3GPCompose& /* aComposer */)
	{
	}

void C3GPLibComposeBase::doTestStepComposeSetUserDataL(C3GPCompose& /* aComposer */)
	{
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeFilename
// -----------------------------------------------------------------------------
//     
C3GPLibComposeFilename::C3GPLibComposeFilename()
	{
	}

TVerdict C3GPLibComposeFilename::doTestStepPreambleL()
	{
	// ensure all basic setup is completed first
	C3GPLibComposeBase::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		TPtrC fileNamePtr;
		if (GetStringFromConfig(ConfigSection(), _L("filename"), fileNamePtr))
			{
			// save a copy of the filename specified for the test
			iFilename.CreateL(fileNamePtr);		
			}
		else 
			{
			// cannot find filename!
			ERR_PRINTF1(_L("Filename not specified.  Test cannot proceed."));
			SetTestStepResult(ETestSuiteError);
			}
		}
	
	return TestStepResult();
	}

TVerdict C3GPLibComposeFilename::doTestStepPostambleL()
	{
	iFilename.Close();
	
	// clean up all composer test base setups
	C3GPLibComposeBase::doTestStepPostambleL();
	
	return TestStepResult();
	}

void C3GPLibComposeFilename::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	T3GPAudioPropertiesMpeg4Audio audio(100, KDummyData);
	T3GPVideoPropertiesAvc video(100, TSize(100, 100), KDummyData);
	
	TInt err = aComposer.Open(E3GP3GP, &video, &audio, iFilename);
	if (err != KErrNone)
		{		
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("C3GPCompose::Open failed with error = %d"), err);
			}
		SetTestStepError(err);
		SetTestStepResult(EFail);
		}
	else 
		{
		INFO_PRINTF1(_L("C3GPCompose::Open completes with no error"));
		SetTestStepResult(EPass);
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeWithoutOpen
// -----------------------------------------------------------------------------
//     
C3GPLibComposeWithoutOpen::C3GPLibComposeWithoutOpen()
	{
	}

void C3GPLibComposeWithoutOpen::doTestStepComposeOpenL(C3GPCompose& /* aComposer */)
	{
	// This test needs the composer NOT initialised.  Do nothing here
	}

void C3GPLibComposeWithoutOpen::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	if (TestStepResult() != EPass)
		{
		return;
		}
	
	TInt err = aComposer.WriteVideoFrame(KDummyData, 100, ETrue);
	if (err != KErrNotReady)
		{
		ERR_PRINTF2(_L("C3GPCompose::WriteVideoFrame failed with error = %d instead of KErrNotReady"), err);
		SetTestStepResult(EFail);				
		}
	
	if (TestStepResult() == EPass)
		{
		T3GPFrameDependencies dependencies;	
		err = aComposer.WriteVideoFrame(KDummyData, 100, ETrue, dependencies);
		if (err != KErrNotReady)
			{
			ERR_PRINTF2(_L("C3GPCompose::WriteVideoFrame failed with error = %d instead of KErrNotReady"), err);
			SetTestStepResult(EFail);				
			}		
		}
	
	if (TestStepResult() == EPass)
		{
		err = aComposer.WriteAudioFrames(KDummyData, 100);
		if (err != KErrNotReady)
			{
			ERR_PRINTF2(_L("C3GPCompose::WriteAudioFrames failed with error = %d instead of KErrNotReady"), err);			
			SetTestStepResult(EFail);				
			}				
		}	
	}

void C3GPLibComposeWithoutOpen::doTestStepComposeSetUserDataL(C3GPCompose& aComposer)
	{
	if (TestStepResult() != EPass)
		{
		return;
		}
	
	TInt err = aComposer.SetUserData(E3GPUdtaMoov, KDummyData);
	if (err != KErrNotReady)
		{
		ERR_PRINTF2(_L("C3GPCompose::SetUserData failed with error = %d instead of KErrNotReady"), err);			
		SetTestStepResult(EFail);				
		}				
	
	if (TestStepResult() == EPass)
		{
		err = aComposer.SetUserData(E3GPUdtaVideoTrak, KDummyData);
		if (err != KErrNotReady)
			{
			ERR_PRINTF2(_L("C3GPCompose::SetUserData failed with error = %d instead of KErrNotReady"), err);			
			SetTestStepResult(EFail);				
			}
		}

	if (TestStepResult() == EPass)
		{
		err = aComposer.SetUserData(E3GPUdtaAudioTrak, KDummyData);
		if (err != KErrNotReady)
			{
			ERR_PRINTF2(_L("C3GPCompose::SetUserData failed with error = %d instead of KErrNotReady"), err);			
			SetTestStepResult(EFail);				
			}
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeFile
// -----------------------------------------------------------------------------
//     
C3GPLibComposeFile::C3GPLibComposeFile() :
	iInputFileFormat(E3GP3GP),
	iVideoType(E3GPNoVideo),
	iAudioType(E3GPNoAudio)
	{
	}

TVerdict C3GPLibComposeFile::doTestStepPreambleL()
	{
	// ensure the base class setup is completed first
	C3GPLibComposeBase::doTestStepPreambleL();
	if (TestStepResult() == EPass)
		{		
		TInt temp;
		if (GetIntFromConfig(ConfigSection(), _L("inputFormat"), temp))
			{
			iInputFileFormat = (T3GPFileFormatType)temp;
			
			if (GetIntFromConfig(ConfigSection(), _L("video"), temp))
				{
				iVideoType = (T3GPVideoType)temp;
				}
			
			if (GetIntFromConfig(ConfigSection(), _L("audio"), temp))
				{
				iAudioType = (T3GPAudioType)temp;
				}
				
			if (iVideoType == E3GPNoVideo && iAudioType == E3GPNoAudio)
				{
				// At least audio or video should be specified
				ERR_PRINTF1(_L("Specify at least video or audio"));
				SetTestStepResult(ETestSuiteError);
				}			
			else
				{
				#ifdef __WINSCW__
				_LIT(KTestFolderName, "testFolderNameEmu");
				#else
				_LIT(KTestFolderName, "testFolderNameHw");
				#endif

				TPtrC testFolderName;
				if (GetStringFromConfig(_L("general"), KTestFolderName, testFolderName))
					{
					TInt err = iFs.Connect();					
					if (err == KErrNone)
						{			
						iFs.MkDirAll(testFolderName);

						TFileName fileName;												
						// Create a temp file for the composer to write data into
						err = iFile.Temp(iFs, testFolderName, fileName, EFileShareExclusive);						
						if (err == KErrNone)
							{
							iFileName.CreateL(fileName);						
							}
						}
					
					if (err != KErrNone)
						{
						ERR_PRINTF1(_L("Fail to create temp file for testing."));
						SetTestStepResult(ETestSuiteError);
						}			
					}
				else
					{
					ERR_PRINTF1(_L("Fail to provide test directory."));			
					SetTestStepResult(ETestSuiteError);
					}
				}				
			}
		else 
			{
			// file format has to be specified 
			ERR_PRINTF1(_L("Specify file format of the file to be composed"));
			SetTestStepResult(ETestSuiteError);		
			}
		}

	return TestStepResult();
	}

TVerdict C3GPLibComposeFile::doTestStepPostambleL()
	{	
	// clean up of temp file
	iFile.Close();
	iFs.Delete(iFileName);	
	iFs.Close();
	
	iFileName.Close();
	
	// clean up all composer test base setups	
	C3GPLibComposeBase::doTestStepPostambleL();

	return TestStepResult();
	}

void C3GPLibComposeFile::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	T3GPVideoPropertiesBase* video = SetupVideoPropertiesL();
	CleanupStack::PushL(video);
	
	T3GPAudioPropertiesBase* audio = SetupAudioPropertiesL(); 
	CleanupStack::PushL(audio);

	TInt err = aComposer.Open(iInputFileFormat, video, audio, iFile);
	if (err == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	else 
		{		
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("C3GPLibComposeFile::doTestStepComposeOpenL => C3GPComposer::Open returns = %d"), err);
			}
		SetTestStepError(err);
		SetTestStepResult(EInconclusive);
		} 
	
	CleanupStack::PopAndDestroy(2);	// audio, video
	}

void C3GPLibComposeFile::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	// add a dummy video frame.  The content is of no importance for this test case, 
	// only the file format is of interest
	TInt err = KErrNone;
	
	if (iVideoType != E3GPNoVideo)
		{
		err = aComposer.WriteVideoFrame(KDummyData, 10, ETrue);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Fail to write video frame, err = %d"), err);
			}
		}
	
	if (err == KErrNone && iAudioType != E3GPNoAudio)
		{
		// add a dummy audio frame.  The content is of no importance for this test case, 
		// only the file format is of interest
		err = aComposer.WriteAudioFrames(KDummyData, 10);
		if (err != KErrNone)
			{			
			ERR_PRINTF2(_L("Fail to write audio frame, err = %d"), err);
			} 	
		}

	if (err != KErrNone)
		{			
		SetTestStepResult(EInconclusive); 
		} 		
	}

T3GPVideoPropertiesBase* C3GPLibComposeFile::SetupVideoPropertiesL()
	{
	if (iVideoType == E3GPNoVideo)
		{
		return NULL;
		}

	// create a dummy video property for setting the composer
	T3GPVideoPropertiesBase* video = NULL;
	switch(iVideoType)
		{
		case E3GPMpeg4Video:
			video = SetupMpeg4VideoL();		
			break;
			
		case E3GPAvcProfileBaseline:
		case E3GPAvcProfileMain:
		case E3GPAvcProfileExtended:
		case E3GPAvcProfileHigh:
			video = SetupAvcVideoL();
			break;
			
		case E3GPH263Profile0:
		case E3GPH263Profile3:
			video = SetupH263VideoL();
			break;
			
		default:
			User::Leave(KErrUnknown);
		}
	
	return video;
	}

T3GPVideoPropertiesBase* C3GPLibComposeFile::SetupAvcVideoL()
	{
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesAvc(1000, TSize(100, 100), KDummyData);
	return video;
	}

T3GPVideoPropertiesBase* C3GPLibComposeFile::SetupH263VideoL()
	{
	T3GPVideoPropertiesH263::TProfile profile = T3GPVideoPropertiesH263::EProfile0;
	if (iVideoType == E3GPH263Profile3)
		{
		profile = T3GPVideoPropertiesH263::EProfile3;
		}
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesH263(1000, TSize(100, 100), 10, profile);	
	return video;
	}

T3GPVideoPropertiesBase* C3GPLibComposeFile::SetupMpeg4VideoL()
	{
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesMpeg4Video(1000, TSize(100, 100), 4000, 400, KDummyData);
	return video;
	}

T3GPAudioPropertiesBase* C3GPLibComposeFile::SetupAudioPropertiesL()
	{
	if (iAudioType == E3GPNoAudio)
		{
		return NULL;
		}

	// create a dummy audio property for setting the composer
	T3GPAudioPropertiesBase* audio = NULL;
	switch(iAudioType)
		{
		case E3GPMpeg4Video:
			audio = SetupMpeg4AudioL();
			break;
			
		case E3GPQcelp13K:
			audio = SetupQcelpAudioL();
			break;
			
		case E3GPAmrNB:			
		case E3GPAmrWB:
			audio = SetupAmrAudioL();
			break;
			
		default:
			User::Leave(KErrUnknown);
			break;
		}
	
	return audio;
	}

T3GPAudioPropertiesBase* C3GPLibComposeFile::SetupQcelpAudioL()
	{
	// for this test's purpose, just use the default QCELP sample entry storage mode 
	T3GPAudioPropertiesBase* audio = new (ELeave) T3GPAudioPropertiesQcelp(1000, 10);
	return audio;
	}

T3GPAudioPropertiesBase* C3GPLibComposeFile::SetupAmrAudioL()
	{
	T3GPAudioPropertiesAmr::TSpeechCodec codec = T3GPAudioPropertiesAmr::EAmrWB;
	if (iAudioType == E3GPAmrNB)
		{
		codec = T3GPAudioPropertiesAmr::EAmrNB;
		}
	T3GPAudioPropertiesBase* audio = new (ELeave) T3GPAudioPropertiesAmr(1000, 10, 150, codec);
	return audio;
	}

T3GPAudioPropertiesBase* C3GPLibComposeFile::SetupMpeg4AudioL()
	{
	T3GPAudioPropertiesBase* audio = new (ELeave)T3GPAudioPropertiesMpeg4Audio(1000, KDummyData);
	return audio;
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeFileWithFileFormatCheck
// -----------------------------------------------------------------------------
//     
C3GPLibComposeFileWithFileFormatCheck::C3GPLibComposeFileWithFileFormatCheck() :
	iOutputFileFormat(E3GP3GP)
	{
	}

TVerdict C3GPLibComposeFileWithFileFormatCheck::doTestStepPreambleL()
	{
	// ensure base class setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();
	
	if (TestStepResult() == EPass)
		{
		TInt temp;
		if (!GetIntFromConfig(ConfigSection(), _L("outputFormat"), temp))
			{
			ERR_PRINTF1(_L("outputFormat not specified"));
			SetTestStepResult(ETestSuiteError);
			}
		else 
			{	
			iOutputFileFormat = (T3GPFileFormatType)temp;		
			}
		}
	
	return TestStepResult();
	}

TVerdict C3GPLibComposeFileWithFileFormatCheck::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	
	// this triggers: 
	// 1. Create a composer
	// 2. Open the composer - C3GPLibComposeFileFormatCheck::doTestStepComposeOpenL
	// 3. Write Video & Audio Data - C3GPLibComposeFileFormatCheck::doTestStepComposeWriteVideoAudioL
	// 4. Write User Data - C3GPLibComposeFileFormatCheck::doTestStepComposeSetUserDataL
	// 5. Complete the composer 
	TVerdict verdict = C3GPLibComposeFile::doTestStepL();
	if (verdict == EPass)
		{
		// once the file is composed, it can be verified if the file composed is the same as the 
		// expected file format 
		verdict = VerifyFileFormatL(iFile);
		}
	else
		{
		INFO_PRINTF1(_L("<C3GPLibComposeFileWithFileFormatCheck> C3GPLibComposeFile::doTestStepL returns failed result"));		
		}
	SetTestStepResult(verdict);
		
	if (!ShouldRunOOMTest())
		{
		INFO_PRINTF2(_L("C3GPLibComposeFile::doTestStepL returns %d"), TestStepResult());	
		}
	return TestStepResult();		
	}

TVerdict C3GPLibComposeFileWithFileFormatCheck::VerifyFileFormatL(const RFile& aFile) 
	{
	// Seek to the beginning of the file
	TInt pos = 0;	
	User::LeaveIfError(aFile.Seek(ESeekStart, pos));
	
	TVerdict verdict = EFail;
	TBuf8<4> atom;

	while(ETrue)
		{
		// read the file content and search for the 'ftyp' atom to check
		// the 32bit value of brand attribute immediately following the 'ftyp' type.
		User::LeaveIfError(aFile.Read(atom));
		if (atom.Length() < atom.MaxLength())
			{
			break;
			}
			
		// read 4 bytes at a time, if 'ftyp' is found
		if (Mem::Compare((&KFtypAtom)->Ptr(), (&KFtypAtom)->Length(), atom.Ptr(), atom.Length()) == 0)
			{
			// read 4 more bytes, and it should contain the brand value
			User::LeaveIfError(aFile.Read(atom));
			if (atom.Length() < atom.MaxLength())
				{
				// cannot read brand value, test failed.
				ERR_PRINTF1(_L("Failed to read enough data for comparison."));						
				break;
				}
			
			switch(iOutputFileFormat)
				{
				case E3GP3G2:
					{
					// check if file type is 3G2
					if (!ShouldRunOOMTest())
						{
						INFO_PRINTF1(_L("Check 3GP2 file format."));
						}
					if (Mem::Compare((&K3g2Brand)->Ptr(), (&K3g2Brand)->Length(), atom.Ptr(), atom.Length()) == 0)
						{
						verdict = EPass;
						break;
						}
					}
				break;
			
				case E3GPMP4:
					{
					if (!ShouldRunOOMTest())
						{
						INFO_PRINTF1(_L("Check MP4 file format."));
						}
					// check if file type is MP4
					if (Mem::Compare((&KMp4Brand)->Ptr(), (&KMp4Brand)->Length(), atom.Ptr(), atom.Length()) == 0)
						{
						verdict = EPass;
						}
					}
				break;
				
				case E3GP3GP:
					{
					if (!ShouldRunOOMTest())
						{
						INFO_PRINTF1(_L("Check 3GP file format."));
						}
					// check if file type is 3GP
					// Both 3gp4 & 3gp6 are valid brands, thus checking ONLY "3gp" instead of the full atom size 
					if (Mem::Compare((&K3gpBrand)->Ptr(), (&K3gpBrand)->Length(), atom.Ptr(), (&K3gpBrand)->Length()) == 0)						
						{
						verdict = EPass;
						}					
					}
				break;		
					
				default:
					INFO_PRINTF1(_L("Unknown format expected."));																					
					verdict = EInconclusive;	
				break;	
				}
			
			// ftyp is found, regardless of the result, finish reading the file
			break;
			}
		}
		
	return verdict;
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeFrameDependency
// -----------------------------------------------------------------------------
//     
C3GPLibComposeFrameDependency::C3GPLibComposeFrameDependency()
	{
	}

void C3GPLibComposeFrameDependency::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	TInt err = KErrNone;
	T3GPFrameDependencies dep;
		
	for (TInt i = 0; i <= (TInt)E3GPDependencyNone; i++)
		{
		for (TInt j = 0; j <= (TInt)E3GPDependencyNone; j++)
			{
			for (TInt k = 0; k <= (TInt)E3GPRedundancyNone; k++)
				{
				dep.iDependsOn = (T3GPVideoFrameDependency)i;
				dep.iIsDependedOn = (T3GPVideoFrameDependency)j;
				dep.iHasRedundancy = (T3GPVideoFrameRedundancy)k;

				// add a dummy video frame.  The content is of no importance for this test case, 
				// only the different dependency/redundancy is the focus of this test
				err = aComposer.WriteVideoFrame(KDummyData, 100, ETrue, dep);
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("C3GPCompose::WriteVideoFrame (with frame dependency) failed with error = %d"), err);
					SetTestStepResult(EFail);
					break;
					}
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeVideoProperties
// -----------------------------------------------------------------------------
//     
C3GPLibComposeVideoProperties::C3GPLibComposeVideoProperties()
	{	
	}

TVerdict C3GPLibComposeVideoProperties::doTestStepPreambleL()
	{
	C3GPLibComposeFile::doTestStepPreambleL();
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
		
	TInt temp;
	TBool result = ETrue;
	result = GetIntFromConfig(ConfigSection(), _L("width"), temp);
	if (result)
		{
		iSize.iWidth = temp;
		result = GetIntFromConfig(ConfigSection(), _L("height"), temp); 
		if (result)
			{
			iSize.iHeight = temp;
			result = GetIntFromConfig(ConfigSection(), _L("timescale"), temp);
			if (result)
				{
				iTimescale = temp;
				}			
			}
		}
	
	if (result && 
	        (iVideoType == E3GPMpeg4Video ||
	         iVideoType == E3GPAvcProfileBaseline ||
	         iVideoType == E3GPAvcProfileMain ||
	         iVideoType == E3GPAvcProfileExtended ||
	         iVideoType == E3GPAvcProfileHigh )
        )
		{
		TPtrC decoder;
		result = GetStringFromConfig(ConfigSection(), _L("decoderSpecInfo"), decoder);
		if (result)
			{	
			iDecoderSpecInfo.CreateL(decoder.Length());
			iDecoderSpecInfo.Copy(decoder);
			}
		}
	
	if (result && (iVideoType == E3GPMpeg4Video))
		{
		result = GetIntFromConfig(ConfigSection(), _L("avgBR"), temp);
		if (result)
			{
			iAvgBitRate = temp;
			result = GetIntFromConfig(ConfigSection(), _L("maxBR"), temp);
			if (result)
				{
				iMaxBitRate = temp;
				}			
			}
		}	
	
	if (result && (iVideoType == E3GPH263Profile0 || iVideoType == E3GPH263Profile3))
		{
		result = GetIntFromConfig(ConfigSection(), _L("level"), temp);
		if (result)
			{
			iVideoLevel = temp;
			}
		}
	
	if (!result)
		{
		ERR_PRINTF1(_L("Cannot retrieve necessary video properties to complete this test."));
		SetTestStepResult(ETestSuiteError);		
		}

	return TestStepResult();
	}

TVerdict C3GPLibComposeVideoProperties::doTestStepPostambleL()
	{	
	iDecoderSpecInfo.Close();
	
	// clean up base class setups	
	C3GPLibComposeFile::doTestStepPostambleL();
	
	return TestStepResult();
	}

T3GPVideoPropertiesBase* C3GPLibComposeVideoProperties::SetupAvcVideoL()
	{
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesAvc(iTimescale, iSize, iDecoderSpecInfo);
	return video;
	}

T3GPVideoPropertiesBase* C3GPLibComposeVideoProperties::SetupH263VideoL()
	{
	T3GPVideoPropertiesH263::TProfile profile = T3GPVideoPropertiesH263::EProfile0;
	if (iVideoType == E3GPH263Profile3)
		{
		profile = T3GPVideoPropertiesH263::EProfile3;
		}
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesH263(iTimescale, iSize, iVideoLevel, profile);	
	return video;
	}

T3GPVideoPropertiesBase* C3GPLibComposeVideoProperties::SetupMpeg4VideoL()
	{
	T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesMpeg4Video(iTimescale, 
																				iSize, 
																				iMaxBitRate, 
																				iAvgBitRate, 
																				iDecoderSpecInfo);
	return video;
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeWithSpecificBufferSize
// -----------------------------------------------------------------------------
//     
C3GPLibComposeWithSpecificBufferSize::C3GPLibComposeWithSpecificBufferSize()
	{	
	}

TVerdict C3GPLibComposeWithSpecificBufferSize::doTestStepPreambleL()
	{
	TVerdict verdict = C3GPLibComposeFile::doTestStepPreambleL();
	if (verdict != EPass)
		{
		return verdict;
		}
		
	TInt temp;
	if (GetIntFromConfig(ConfigSection(), _L("bufferSize"), temp))
		{
		iBufferSize = temp;		
		if (GetIntFromConfig(ConfigSection(), _L("bufferCount"), temp)) 
			{
			iBufferMaxCount = temp;
			}
		else
			{
			ERR_PRINTF1(_L("C3GPLibComposeWithSpecificBufferSize cannot read Buffer Max Count"));		
			SetTestStepResult(ETestSuiteError);			
			}
		}
	else
		{
		ERR_PRINTF1(_L("C3GPLibComposeWithSpecificBufferSize cannot read Buffer Size"));		
		SetTestStepResult(ETestSuiteError);
		}
	
	return TestStepResult(); 
	}
	
C3GPCompose* C3GPLibComposeWithSpecificBufferSize::doTestStepCreateComposerL()
	{
	return C3GPCompose::NewL(iBufferSize, iBufferMaxCount);
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeUserData
// -----------------------------------------------------------------------------
//     
C3GPLibComposeUserData::C3GPLibComposeUserData() :
	iUdtaLocation(-1)
	{
	}

TVerdict C3GPLibComposeUserData::doTestStepPreambleL()	
	{
	// ensure all basic setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		GetBoolFromConfig(ConfigSection(), _L("udtaMoov"), iUdtaMoov);
		GetBoolFromConfig(ConfigSection(), _L("udtaVideo"), iUdtaVideo);
		GetBoolFromConfig(ConfigSection(), _L("udtaAudio"), iUdtaAudio);
		GetIntFromConfig(ConfigSection(), _L("udtaLocation"), iUdtaLocation);
		
		if (!iUdtaMoov && !iUdtaVideo && !iUdtaAudio && iUdtaLocation == -1)
			{
			ERR_PRINTF1(_L("No user data location is specified.  Test cannot proceed."));
			SetTestStepResult(ETestSuiteError);												
			}
		else
			{
			TPtrC userDataPtr;
			if (GetStringFromConfig(ConfigSection(), _L("userData"), userDataPtr))
				{					
				iUserData.CreateL(userDataPtr.Length());
				iUserData.Copy(userDataPtr);
				}
			else
				{
				// user data not specified, use dummy data
				iUserData.CreateL(KDummyData);
				}
			}
		}
	
	return TestStepResult();
	}
	
TVerdict C3GPLibComposeUserData::doTestStepPostambleL()
	{	
	iUserData.Close();
	
	// clean up base class setups	
	C3GPLibComposeFile::doTestStepPostambleL();
	
	return TestStepResult();
	}

void C3GPLibComposeUserData::doTestStepComposeSetUserDataL(C3GPCompose& aComposer)
	{
	TInt err = KErrNone;
	if (iUdtaVideo)
		{
		err = aComposer.SetUserData(E3GPUdtaVideoTrak, iUserData);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("C3GPComposer::SetUserData(E3GPUdtaVideoTrak) fails with %d"), err);
			SetTestStepError(err);
			}
		}
	if (iUdtaAudio)
		{
		err = aComposer.SetUserData(E3GPUdtaAudioTrak, iUserData);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("C3GPComposer::SetUserData(E3GPUdtaAudioTrak) fails with %d"), err);
			SetTestStepError(err);
			}
		}
	if (iUdtaMoov)
		{
		err = aComposer.SetUserData(E3GPUdtaMoov, iUserData);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("C3GPComposer::SetUserData(E3GPUdtaMoov) fails with %d"), err);
			SetTestStepError(err);
			}
		}
	
	if (iUdtaLocation != -1)
		{
		// if a specific user data location has been specified, try it!
		err = aComposer.SetUserData((T3GPUdtaLocation)iUdtaLocation, iUserData);
		if (err != KErrNone)
			{
			ERR_PRINTF3(_L("C3GPComposer::SetUserData(%d) fails with %d"), iUdtaLocation, err);
			SetTestStepError(err);
			}		
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeWithFlag
// -----------------------------------------------------------------------------
//     
C3GPLibComposeWithFlag::C3GPLibComposeWithFlag()
	{
	}

TVerdict C3GPLibComposeWithFlag::doTestStepPreambleL()
	{
	// ensure the base class setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();
	if (TestStepResult() == EPass)
		{		
		if (!GetIntFromConfig(ConfigSection(), _L("flag"), iComposeFlag))
			{
			ERR_PRINTF1(_L("C3GPLibComposeWithFlag::doTestStepPreambleL cannot retrieve any composer flags."));
			SetTestStepResult(ETestSuiteError);
			}
		}
	return TestStepResult();
	}

void C3GPLibComposeWithFlag::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	T3GPVideoPropertiesBase* video = SetupVideoPropertiesL();
	CleanupStack::PushL(video);
	
	T3GPAudioPropertiesBase* audio = SetupAudioPropertiesL(); 
	CleanupStack::PushL(audio);

	TInt err = aComposer.Open(iInputFileFormat, video, audio, iFile, iComposeFlag);
	if (err == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	else 
		{		
		ERR_PRINTF2(_L("C3GPLibComposeFile::doTestStepComposeOpenL => C3GPComposer::Open returns = %d"), err);
		SetTestStepResult(EInconclusive);
		} 
	
	CleanupStack::PopAndDestroy(2);	// audio, video
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeAudioProperties
// -----------------------------------------------------------------------------
//     
C3GPLibComposeAudioProperties::C3GPLibComposeAudioProperties()
	{	
	}

TVerdict C3GPLibComposeAudioProperties::doTestStepPreambleL()
	{
	C3GPLibComposeFile::doTestStepPreambleL();
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
		
	TInt temp;
	TBool result = ETrue;
	result = GetIntFromConfig(ConfigSection(), _L("timescale"), temp);
	if (result)
		{
		iTimescale = temp;
		}			
	
	if (result && (iAudioType == E3GPQcelp13K))
		{
		result = GetIntFromConfig(ConfigSection(), _L("storage"), temp);
		if (result)
			{
			iQCelpMode = (T3GPQcelpStorageMode) temp;
			if (iQCelpMode == E3GPMP4AudioDescriptionBox)
				{
				TPtrC decoder;
				result = GetStringFromConfig(ConfigSection(), _L("decoderSpecInfo"), decoder);
				if (result)
					{	
					iDecoderSpecInfo.CreateL(decoder.Length());
					iDecoderSpecInfo.Copy(decoder);
					}				
				}
			}
		
		if (result)
			{
			result = GetIntFromConfig(ConfigSection(), _L("fps"), iFps);
			}		
		}

	if (result && (iAudioType == E3GPMpeg4Audio))
		{
		TPtrC decoder;
		result = GetStringFromConfig(ConfigSection(), _L("decoderSpecInfo"), decoder);
		if (result)
			{	
			iDecoderSpecInfo.CreateL(decoder.Length());
			iDecoderSpecInfo.Copy(decoder);
			}						
		}
	
	if (result && (iAudioType == E3GPAmrNB || iAudioType == E3GPAmrWB))
		{
		result = GetIntFromConfig(ConfigSection(), _L("modeSet"), iModeSet);
		if (result)
			{
			result = GetIntFromConfig(ConfigSection(), _L("fps"), iFps);
			}
		}
		
	if (!result)
		{
		//  
		ERR_PRINTF1(_L("Cannot retrieve necessary a properties to complete this test."));
		SetTestStepResult(ETestSuiteError);		
		}

	return TestStepResult();
	}

TVerdict C3GPLibComposeAudioProperties::doTestStepPostambleL()
	{	
	iDecoderSpecInfo.Close();
	
	// clean up base class setups	
	C3GPLibComposeFile::doTestStepPostambleL();
	
	return TestStepResult();
	}

T3GPAudioPropertiesBase* C3GPLibComposeAudioProperties::SetupQcelpAudioL()
	{
	T3GPAudioPropertiesBase* audio = NULL;
	
	if (iQCelpMode == E3GPQcelpSampleEntryBox)
		{		
		audio = new (ELeave) T3GPAudioPropertiesQcelp(iTimescale, iFps);
		}
	else if (iQCelpMode == E3GPMP4AudioDescriptionBox)
		{
		audio = new (ELeave) T3GPAudioPropertiesQcelp(iTimescale, iFps, iDecoderSpecInfo);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	
	return audio;
	}

T3GPAudioPropertiesBase* C3GPLibComposeAudioProperties::SetupAmrAudioL()
	{
	T3GPAudioPropertiesAmr::TSpeechCodec codec = T3GPAudioPropertiesAmr::EAmrWB;
	if (iAudioType == E3GPAmrNB)
		{
		codec = T3GPAudioPropertiesAmr::EAmrNB;
		}
	T3GPAudioPropertiesBase* audio = new (ELeave) T3GPAudioPropertiesAmr(iTimescale, iFps, iModeSet, codec);
	return audio;
	}

T3GPAudioPropertiesBase* C3GPLibComposeAudioProperties::SetupMpeg4AudioL()
	{
	T3GPAudioPropertiesBase* audio = new (ELeave)T3GPAudioPropertiesMpeg4Audio(iTimescale, iDecoderSpecInfo);
	return audio;
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeWithNoAudioVideo
// -----------------------------------------------------------------------------
//     
C3GPLibComposeWithNoAudioVideo::C3GPLibComposeWithNoAudioVideo()
	{
	}

TVerdict C3GPLibComposeWithNoAudioVideo::doTestStepPreambleL()
	{
	// ensure the base class setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();
	if (TestStepResult() == EPass)
		{
		// reset the video / audio type both of No Video & No Audio
		iVideoType = E3GPNoVideo;
		iAudioType = E3GPNoAudio;		
		}
	
	return TestStepResult();
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeAudioFrames
// -----------------------------------------------------------------------------
//     
C3GPLibComposeAudioFrames::C3GPLibComposeAudioFrames()
	{
	}

TVerdict C3GPLibComposeAudioFrames::doTestStepPreambleL()
	{
	// ensure all basic setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		if (GetIntFromConfig(ConfigSection(), _L("duration"), iDuration))
			{
			TPtrC audioFramePtr;
			if (GetStringFromConfig(ConfigSection(), _L("audioFrame"), audioFramePtr))
				{					
				iAudioFrames.CreateL(audioFramePtr.Length());
				iAudioFrames.Copy(audioFramePtr);					
				}
			else
				{
				// cannot find expected error!
				ERR_PRINTF1(_L("audioFrame not specified.  Test cannot proceed."));
				SetTestStepResult(ETestSuiteError);									
				}
			}
		else
			{
			// cannot find expected error!
			ERR_PRINTF1(_L("duration not specified.  Test cannot proceed."));
			SetTestStepResult(ETestSuiteError);				
			}
		} 
	
	return TestStepResult();
	}

TVerdict C3GPLibComposeAudioFrames::doTestStepPostambleL()
	{
	// cleanup
	iAudioFrames.Close();
	
	// ensure all base cleanup is done
	C3GPLibComposeFile::doTestStepPostambleL();
	
	return TestStepResult();
	}

void C3GPLibComposeAudioFrames::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	// add the audio frame  
	TInt err = aComposer.WriteAudioFrames(iAudioFrames, iDuration); 
	if (err != KErrNone)
		{			
		if (!ShouldRunOOMTest())
			{
			INFO_PRINTF2(_L("C3GPCompose::WriteAudioFrames returns %d"), err);		
			}
		SetTestStepError(err); 
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeVideoFrame
// -----------------------------------------------------------------------------
//     
C3GPLibComposeVideoFrame::C3GPLibComposeVideoFrame()
	{
	}

TVerdict C3GPLibComposeVideoFrame::doTestStepPreambleL()
	{
	// ensure all basic setup is completed first
	C3GPLibComposeFile::doTestStepPreambleL();	
	if (TestStepResult() == EPass)
		{
		if (GetBoolFromConfig(ConfigSection(), _L("keyFrame"), iKeyFrame))
			{
			if (GetIntFromConfig(ConfigSection(), _L("duration"), iDuration))
				{
				TPtrC framePtr;
				if (GetStringFromConfig(ConfigSection(), _L("videoFrame"), framePtr))
					{					
					iVideoFrame.CreateL(framePtr.Length());
					iVideoFrame.Copy(framePtr);

					TInt dependsOn;
					TInt isDependedOn;
					TInt hasRedundancy;
					if (GetIntFromConfig(ConfigSection(), _L("dependsOn"), dependsOn) &&
						GetIntFromConfig(ConfigSection(), _L("isDependedOn"), isDependedOn) && 							
						GetIntFromConfig(ConfigSection(), _L("hasRedundancy"), hasRedundancy))
						{
						iDependencies = new (ELeave) T3GPFrameDependencies();
						iDependencies->iDependsOn = (T3GPVideoFrameDependency) dependsOn;
						iDependencies->iIsDependedOn = (T3GPVideoFrameDependency) isDependedOn;
						iDependencies->iHasRedundancy = (T3GPVideoFrameRedundancy) hasRedundancy;
						}
					}
				else
					{
					// cannot find expected error!
					ERR_PRINTF1(_L("videoFrame not specified.  Test cannot proceed."));
					SetTestStepResult(ETestSuiteError);									
					}
				}
			else
				{
				// cannot find expected error!
				ERR_PRINTF1(_L("duration not specified.  Test cannot proceed."));
				SetTestStepResult(ETestSuiteError);				
				}
			}
		else 
			{
			// cannot find expected error!
			ERR_PRINTF1(_L("keyFrame not specified.  Test cannot proceed."));
			SetTestStepResult(ETestSuiteError);
			}
		}
	
	return TestStepResult();
	}

TVerdict C3GPLibComposeVideoFrame::doTestStepPostambleL()
	{
	// cleanup
	iVideoFrame.Close();
	delete iDependencies;
	
	// ensure all base cleanup is done
	C3GPLibComposeFile::doTestStepPostambleL();
	
	return TestStepResult();
	}

void C3GPLibComposeVideoFrame::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	// add the video frame
	TInt err = KErrNone;
	if (iDependencies)
		{
		err = aComposer.WriteVideoFrame(iVideoFrame, iDuration, iKeyFrame, *iDependencies);		
		}
	else
		{
		err = aComposer.WriteVideoFrame(iVideoFrame, iDuration, iKeyFrame);		
		}
	
	if (err != KErrNone)
		{			
		INFO_PRINTF2(_L("C3GPCompose::WriteVideoFrame returns %d"), err);		
		SetTestStepError(err); 
		} 		
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeOpenAgain
// -----------------------------------------------------------------------------
//     
C3GPLibComposeOpenAgain::C3GPLibComposeOpenAgain()
	{	
	}

void C3GPLibComposeOpenAgain::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	C3GPLibComposeFile::doTestStepComposeOpenL(aComposer);
	if (TestStepResult() == EPass)
		{		
		// ensure the composer is opened, then try open it again!		
		T3GPVideoPropertiesBase* video = SetupVideoPropertiesL();
		CleanupStack::PushL(video);
		
		T3GPAudioPropertiesBase* audio = SetupAudioPropertiesL(); 
		CleanupStack::PushL(audio);
		
		// retrieve the current location of the temp file created when 
		// opening the composer the first time in C3GPLibComposeFile::doTestStepComposeOpenL
		RBuf folderName;
		folderName.CreateL(KMaxFileName);
		CleanupClosePushL(folderName);
		User::LeaveIfError(iFile.FullName(folderName));
				
		TParsePtrC folderParse(folderName);
		TPtrC folder = folderParse.DriveAndPath();
		
		// create another file temp file within the same folder
		RFile file;
		TFileName fileName; 
		User::LeaveIfError(file.Temp(iFs, folder, fileName, EFileWrite));

		TInt err = aComposer.Open(iInputFileFormat, video, audio, file);		
		TInt err2 = aComposer.Open(iInputFileFormat, video, audio, fileName);
		if (err != KErrNone && err == err2)
			{
			SetTestStepError(err);
			}
		else
			{
			INFO_PRINTF1(_L("C3GPLibComposeOpenAgain C3GPCompose::Open returns inconsistent error."));			
			}
		
		// clean up
		file.Close();
		iFs.Delete(fileName);
		
		CleanupStack::PopAndDestroy(3);	// audio, video, folderName
		}
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeOpenReadOnlyFile
// -----------------------------------------------------------------------------
//     
C3GPLibComposeOpenReadOnlyFile::C3GPLibComposeOpenReadOnlyFile()
	{	
	}

void C3GPLibComposeOpenReadOnlyFile::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	RBuf fileName;
	fileName.CreateL(KMaxFilename);
	CleanupClosePushL(fileName);

	// change the file to be opened to be a READ-ONLY file
	User::LeaveIfError(iFile.FullName(fileName));
	iFile.Close();
	
	User::LeaveIfError(iFile.Open(iFs, fileName, EFileShareReadersOnly));
	CleanupStack::PopAndDestroy(&fileName);
	
	C3GPLibComposeWithFlag::doTestStepComposeOpenL(aComposer);
	}

TVerdict C3GPLibComposeOpenReadOnlyFile::doTestStepL()
	{	
	TVerdict verdict(EFail);
	
	TRAP_IGNORE(verdict = C3GPLibComposeWithFlag::doTestStepL());
	
	return verdict;
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeOpenedFile
// -----------------------------------------------------------------------------
//     
C3GPLibComposeOpenedFile::C3GPLibComposeOpenedFile()
	{	
	}

void C3GPLibComposeOpenedFile::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	RBuf fileName;
	fileName.CreateL(KMaxFilename);
	CleanupClosePushL(fileName);

	// change the file to be opened to be a READ-ONLY file
	User::LeaveIfError(iFile.FullName(fileName));
	iFile.Close();
	
	User::LeaveIfError(iFile.Open(iFs, fileName, EFileShareAny));
	
	T3GPVideoPropertiesBase* video = SetupVideoPropertiesL();
	CleanupStack::PushL(video);
	
	T3GPAudioPropertiesBase* audio = SetupAudioPropertiesL(); 
	CleanupStack::PushL(audio);

	TInt err1 = aComposer.Open(iInputFileFormat, video, audio, fileName);
	if (err1 != KErrNone)
		{
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("C3GPLibComposeOpenedFile::doTestStepComposeOpenL => C3GPComposer::Open returns = %d"), err1);		
			}
		
		iFile.Close();
		User::LeaveIfError(iFile.Open(iFs, fileName, EFileShareReadersOrWriters));
		TInt err2 = aComposer.Open(iInputFileFormat, video, audio, fileName);
		
		if (err1 == err2)
			{
			SetTestStepError(err1);
			}
		}
	
	CleanupStack::PopAndDestroy(3);	// audio, video, fileName
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeMultiComposeOnSameFile
// -----------------------------------------------------------------------------
//     
C3GPLibComposeMultiComposeOnSameFile::C3GPLibComposeMultiComposeOnSameFile()
	{	
	}

void C3GPLibComposeMultiComposeOnSameFile::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	C3GPLibComposeFilename::doTestStepComposeOpenL(aComposer);
	if (TestStepError() == KErrNone && TestStepResult() == EPass)
		{
		T3GPAudioPropertiesMpeg4Audio audio(100, KDummyData);
		T3GPVideoPropertiesAvc video(100, TSize(100, 100), KDummyData);
		C3GPCompose* composer2 = C3GPCompose::NewL();
		
		TInt err = composer2->Open(E3GP3GP, &video, &audio, iFilename);
		if (err != KErrNone)
			{		
			SetTestStepError(err);
			}
			
		// close the composer
		composer2->Complete();
		delete composer2;
		} 
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeMultiComposers
// -----------------------------------------------------------------------------
//     
C3GPLibComposeMultiComposers::C3GPLibComposeMultiComposers()
	{	
	}

void C3GPLibComposeMultiComposers::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	C3GPLibComposeFilename::doTestStepComposeOpenL(aComposer);
	if (TestStepError() == KErrNone && TestStepResult() == EPass)
		{
		#ifdef __WINSCW__
		_LIT(KTestFolderName, "testFolderNameEmu");
		#else
		_LIT(KTestFolderName, "testFolderNameHw");
		#endif

		TPtrC testFolderName;
		if (!GetStringFromConfig(_L("general"), KTestFolderName, testFolderName))
			{
			User::Leave(KErrNotFound);
			}
		
		TFileName fileName;
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);

		// Composer 2
		T3GPAudioPropertiesMpeg4Audio audio1(100, KDummyData);
		T3GPVideoPropertiesAvc video1(100, TSize(100, 100), KDummyData);
		
		RFile file1;
		CleanupClosePushL(file1);		
		User::LeaveIfError(file1.Temp(fs, testFolderName, fileName, EFileShareExclusive));
		
		C3GPCompose* composer2 = C3GPCompose::NewL();		
		CleanupStack::PushL(composer2);
		User::LeaveIfError(composer2->Open(E3GP3GP, &video1, &audio1, file1));
			
		// composer 3
		T3GPAudioPropertiesMpeg4Audio audio2(50, KDummyData);
		T3GPVideoPropertiesAvc video2(50, TSize(20, 20), KDummyData);
		
		RFile file2;
		CleanupClosePushL(file2);		
		User::LeaveIfError(file2.Temp(fs, testFolderName, fileName, EFileShareExclusive));
		
		C3GPCompose* composer3 = C3GPCompose::NewL();		
		CleanupStack::PushL(composer3);
		User::LeaveIfError(composer3->Open(E3GP3GP, &video2, &audio2, file2));
		
		// write some data into composer 1, then 2, then 3
		User::LeaveIfError(aComposer.WriteVideoFrame(KDummyData, 10, EFalse));
		User::LeaveIfError(composer2->WriteVideoFrame(KDummyData, 10, EFalse));
		User::LeaveIfError(composer3->WriteVideoFrame(KDummyData, 10, EFalse));
		
		// clean up for composer 3
		User::LeaveIfError(composer3->Complete());
		CleanupStack::PopAndDestroy(composer3);		
		CleanupStack::PopAndDestroy(&file2);	
		
		// Parse file2
		C3GPParse* parser = C3GPParse::NewL();
		CleanupStack::PushL(parser);
		User::LeaveIfError(parser->Open(fileName));
		
		// write some data into composer 1 & 2
		User::LeaveIfError(aComposer.WriteVideoFrame(KDummyData, 10, EFalse));				
		User::LeaveIfError(composer2->WriteVideoFrame(KDummyData, 10, EFalse));
		
		// close parser and delete the file
		User::LeaveIfError(parser->Complete());
		CleanupStack::PopAndDestroy(parser);
		User::LeaveIfError(fs.Delete(fileName));	// delete the temp file created
		
		// clean up for composer 2
		User::LeaveIfError(composer2->Complete());	
		file1.FullName(fileName);					// retrieve the full name of file1
		CleanupStack::PopAndDestroy(composer2);	
		CleanupStack::PopAndDestroy(&file1);
		User::LeaveIfError(fs.Delete(fileName));	// delete the temp file created
		
		// cleanup
		CleanupStack::PopAndDestroy(&fs);
		
		// write some data into composer 1
		User::LeaveIfError(aComposer.WriteVideoFrame(KDummyData, 10, EFalse));		
		} 
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeCloseComposerWithoutComplete
// -----------------------------------------------------------------------------
//     
C3GPLibComposeCloseComposerWithoutComplete::C3GPLibComposeCloseComposerWithoutComplete()
	{	
	}

void C3GPLibComposeCloseComposerWithoutComplete::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	// close the composer, then delete the file
	aComposer.Complete();
		
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
		
	// ignore any error
	fs.Delete(iFilename);

	// Reopens it
	T3GPAudioPropertiesMpeg4Audio audio1(100, KDummyData);
	T3GPVideoPropertiesAvc video1(100, TSize(100, 100), KDummyData);

	C3GPCompose* composer = C3GPCompose::NewL();		
	CleanupStack::PushL(composer);
	User::LeaveIfError(composer->Open(E3GP3GP, &video1, &audio1, iFilename));
		
	// write some data into composer 1, then 2, then 3
	User::LeaveIfError(composer->WriteVideoFrame(KDummyData, 10, EFalse));
	
	// delete the composer before calling Complete
	CleanupStack::PopAndDestroy(composer);
	
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs, iFilename, EFileRead));
	
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	if (fileSize == 0)
		{
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2);	// file, fs

	// continue the test
	C3GPLibComposeFilename::doTestStepComposeOpenL(aComposer);
	}

// -----------------------------------------------------------------------------
// C3GPLibComposePanic
// -----------------------------------------------------------------------------
//     
C3GPLibComposePanic::C3GPLibComposePanic()
	{	
	}

TVerdict C3GPLibComposePanic::doTestStepPreambleL()
	{
	TVerdict verdict = C3GPLibComposeFilename::doTestStepPreambleL();
	if (TestStepResult() == EPass)
		{
		if (!GetBoolFromConfig(ConfigSection(), _L("panic"), iPanic))
			{
			ERR_PRINTF1(_L("Test setting panic not specified.  Test cannot proceed."));			
			SetTestStepResult(ETestSuiteError);
			}
		else 
			{
			TPtrC strPtr;
			if (GetStringFromConfig(ConfigSection(), _L("dir"), strPtr))
				{
				// save a copy of the dir specified for the test
				iDir.CreateL(strPtr);		
				}
			else 
				{
				// cannot find filename!
				ERR_PRINTF1(_L("Dir not specified.  Test cannot proceed."));
				SetTestStepResult(ETestSuiteError);
				}
			}
		}
	
	return TestStepResult();
	}

void C3GPLibComposePanic::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	// close the composer, then delete the file
	aComposer.Complete();
		
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
		
	// retrieve the number of items in the directory
	CDir* dirList = NULL;
	User::LeaveIfError(fs.GetDir(iDir, KEntryAttNormal, ESortNone, dirList));			
	TInt orgCount = dirList->Count();
	delete dirList;

	// Reopens it
	T3GPAudioPropertiesMpeg4Audio audio1(100, KDummyData);
	T3GPVideoPropertiesAvc video1(100, TSize(100, 100), KDummyData);

	C3GPCompose* composer = C3GPCompose::NewL();		
	CleanupStack::PushL(composer);
	User::LeaveIfError(composer->Open(E3GP3GP, &video1, &audio1, iFilename));
		
	// write some data into composer 1, then 2, then 3
	User::LeaveIfError(composer->WriteVideoFrame(KDummyData, 10, EFalse));
	
	if (iPanic)
		{
		// force a panic so that the composed file and the temporary file is left in the 
		// directory
		User::Panic(_L("Testing"), KErrArgument);
		}
	
	// delete the composer before calling Complete
	composer->Complete();
	CleanupStack::PopAndDestroy(composer);
	
	User::LeaveIfError(fs.GetDir(iDir, KEntryAttNormal, ESortNone, dirList));			
	TInt newCount = dirList->Count();
	delete dirList;
	
	// When cleaned up properly, only the composed file should be left within the directory.
	if (!(newCount == 1 && orgCount > 0))
		{
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy();	// fs

	// continue the test
	C3GPLibComposeFilename::doTestStepComposeOpenL(aComposer);
	}

TVerdict C3GPLibComposePanic::doTestStepPostambleL()
	{
	iDir.Close();
	return 	C3GPLibComposeFilename::doTestStepPostambleL();
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeLargeFile
// -----------------------------------------------------------------------------
//     
C3GPLibComposeLargeFile::C3GPLibComposeLargeFile()
	{	
	}

TVerdict C3GPLibComposeLargeFile::doTestStepPreambleL()
	{
	// ensure the base class setup is completed first
	C3GPLibComposeBase::doTestStepPreambleL();
	if (TestStepResult() == EPass)
		{		
		TInt temp;
		if (GetIntFromConfig(ConfigSection(), _L("inputFormat"), temp))
			{
			iInputFileFormat = (T3GPFileFormatType)temp;
			
			if (GetIntFromConfig(ConfigSection(), _L("video"), temp))
				{
				iVideoType = (T3GPVideoType)temp;
				}
			
			if (GetIntFromConfig(ConfigSection(), _L("audio"), temp))
				{
				iAudioType = (T3GPAudioType)temp;
				}
				
			if (iVideoType == E3GPNoVideo && iAudioType == E3GPNoAudio)
				{
				// At least audio or video should be specified
				ERR_PRINTF1(_L("Specify at least video or audio"));
				SetTestStepResult(ETestSuiteError);
				}			
			else
				{
				#ifdef __WINSCW__
				_LIT(KOutputDir, "filenameEmu");
				#else
				_LIT(KOutputDir, "filenameHw");
				#endif

				TPtrC fileNamePtr;
				if (GetStringFromConfig(ConfigSection(), KOutputDir, fileNamePtr))
					{
					iFileName.CreateL(fileNamePtr);
					User::LeaveIfError(iFs.Connect());

					TInt err = iFs.MkDirAll(iFileName);
					if (err != KErrNone && err != KErrAlreadyExists)
						{
						User::Leave(err);
						}

					err = iFile64.Create(iFs, iFileName, EFileShareAny|EFileStream|EFileWrite);
					if (err == KErrAlreadyExists)
						{
						User::LeaveIfError(iFile64.Replace(iFs, iFileName, EFileShareAny|EFileStream|EFileWrite));
						}
					else
						{
						User::LeaveIfError(err);
						}	
					}
				else 
					{
					// cannot find filename!
					ERR_PRINTF1(_L("Filename not specified.  Test cannot proceed."));
					SetTestStepResult(ETestSuiteError);
					}
				}				
			}
		else 
			{
			// file format has to be specified 
			ERR_PRINTF1(_L("Specify file format of the file to be composed"));
			SetTestStepResult(ETestSuiteError);		
			}

		if (!GetIntFromConfig(ConfigSection(), _L("flag"), iComposeFlag))
			{
			iComposeFlag = E3GPNoFlag;
			}
		}

	return TestStepResult();
	}

TVerdict C3GPLibComposeLargeFile::doTestStepPostambleL()
	{	
	// clean up of temp file
	iFile64.Close();
	iFs.Delete(iFileName);
	iFs.Close();
	
	iFileName.Close();
	
	// clean up all composer test base setups	
	C3GPLibComposeBase::doTestStepPostambleL();

	return TestStepResult();
	}

C3GPCompose* C3GPLibComposeLargeFile::doTestStepCreateComposerL()
	{
	return C3GPCompose::NewL(KLargeFileWriteBufferSize, KLargeFileWriteBufferMaxCount);
	}

void C3GPLibComposeLargeFile::doTestStepComposeOpenL(C3GPCompose& aComposer)
	{
	T3GPVideoPropertiesBase* video = SetupVideoPropertiesL();
	CleanupStack::PushL(video);
	
	T3GPAudioPropertiesBase* audio = SetupAudioPropertiesL(); 
	CleanupStack::PushL(audio);

	TInt err = aComposer.Open(iInputFileFormat, video, audio, iFile64, iComposeFlag);
	if (err == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	else 
		{		
		if (!ShouldRunOOMTest())
			{
			ERR_PRINTF2(_L("C3GPLibComposeFile::doTestStepComposeOpenL => C3GPComposer::Open returns = %d"), err);
			}
		SetTestStepError(err);
		SetTestStepResult(EInconclusive);
		} 
	
	CleanupStack::PopAndDestroy(2);	// audio, video
	}

void C3GPLibComposeLargeFile::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	RBuf8 videoBuf1;
	CleanupClosePushL(videoBuf1);		
	TBool keyFrame1;
	TUint videoTimestampMS = 0;	
	TUint videoTS1;
	TUint frameSize = 0;
	T3GPFrameDependencies aDependencies;		
	
	RFile fileName;
	CleanupClosePushL(fileName);		
	User::LeaveIfError(fileName.Open(iFs, KLargeVideoFile(), EFileRead));	
	
	C3GPParse* parser = C3GPParse::NewL();
	CleanupStack::PushL(parser);
	User::LeaveIfError(parser->Open(fileName));
	User::LeaveIfError(parser->GetVideoFrameSize(frameSize));
	User::LeaveIfError(videoBuf1.Create((TInt)frameSize));
	User::LeaveIfError(parser->ReadVideoFrame(videoBuf1, keyFrame1, videoTimestampMS, videoTS1));

	TInt error = parser->GetVideoFrameDependencies(aDependencies);
	if (error != KErrNone)
		{
		aDependencies.iDependsOn = E3GPDependencyUnknown;
		aDependencies.iIsDependedOn = E3GPDependencyUnknown;
		aDependencies.iHasRedundancy = E3GPRedundancyUnknown;
		}

	TInt err = KErrNone;
	TInt64 size = 0;
	TTime startTime;
	TTime endTime;
	startTime.UniversalTime();				

	// create a 3GB video file
	TInt videoLength = videoBuf1.Length();
	while (size + videoLength < K3GigaBytes)
	    {  				
		err = aComposer.WriteVideoFrame(videoBuf1, 100, keyFrame1, aDependencies);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Fail to write video frame, err = %d"), err);
			break;
			}			
		iFile64.Size(size);
	    }

	endTime.UniversalTime();
	TTimeIntervalMinutes composeTime;				
	endTime.MinutesFrom(startTime, composeTime);	 
	INFO_PRINTF3(_L("Took %d minutes to compose a video file of size %d bytes"), composeTime.Int(), size);   		    		

	if (err != KErrNone)
		{			
		SetTestStepResult(EFail); 
		}

	User::LeaveIfError(parser->Complete());
	CleanupStack::PopAndDestroy(3, &videoBuf1); 		
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeLargeFileUserData
// -----------------------------------------------------------------------------
//     
C3GPLibComposeLargeFileUserData::C3GPLibComposeLargeFileUserData()
	{	
	}

void C3GPLibComposeLargeFileUserData::doTestStepComposeSetUserDataL(C3GPCompose& aComposer)
	{
	TInt bufferUdtaSize = 0;
	TBuf8<256> bufferUdta;
	TUint8* ptr = const_cast<TUint8*>(bufferUdta.Ptr());
	TInt size = 0;
				
	_LIT8(KTitle,"titl");
	_LIT8(KData1,"This Udta titl test data 01!!<end>");
	size = KTitle().Length() + KData1().Length() + 4;
	WriteInt32(ptr, size);	
	ptr += 4;
	Mem::Copy(ptr, KTitle().Ptr(), KTitle().Length());
	ptr += KTitle().Length();
	Mem::Copy(ptr, KData1().Ptr(), KData1().Length());
	ptr += KData1().Length();
	bufferUdtaSize += size;

	_LIT8(KTitle1,"titl");
	_LIT8(KData2,"This Udta titl test data 02!!blahblahblahbla<end>");
	size = KTitle1().Length() + KData2().Length() + 4;
	WriteInt32(ptr, size);	
	ptr += 4;
	Mem::Copy(ptr, KTitle1().Ptr(), KTitle1().Length());
	ptr += KTitle1().Length();
	Mem::Copy(ptr, KData2().Ptr(), KData2().Length());
	ptr += KData2().Length();
	bufferUdtaSize += size;

	_LIT8(KTitle2,"titl");
	_LIT8(KData3,"This Udta titl test data 03!!Moreblahblahblahblahblahblahblahblahblah<end>");
	size = KTitle2().Length() + KData3().Length() + 4;
	WriteInt32(ptr, size);	
	ptr += 4;
	Mem::Copy(ptr, KTitle2().Ptr(), KTitle2().Length());
	ptr += KTitle2().Length();
	Mem::Copy(ptr, KData3().Ptr(), KData3().Length());
	ptr += KData3().Length();
	bufferUdtaSize += size;
		
	bufferUdta.SetLength(bufferUdtaSize);			
	TInt error = aComposer.SetUserData(E3GPUdtaVideoTrak, bufferUdta);

	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("C3GPComposer::SetUserData failed with %d"), error);
		SetTestStepError(error);
		}
	}

void C3GPLibComposeLargeFileUserData::WriteInt32(TUint8* aPtr, TInt32 aData)
	{
	aPtr[0] = TUint8(aData>>24);
	aPtr[1] = TUint8(aData>>16);
	aPtr[2] = TUint8(aData>>8);
	aPtr[3] = TUint8(aData);
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeLargeFile32bitAPI
// -----------------------------------------------------------------------------
//     
C3GPLibComposeLargeFile32bitAPI::C3GPLibComposeLargeFile32bitAPI()
	{	
	}

C3GPCompose* C3GPLibComposeLargeFile32bitAPI::doTestStepCreateComposerL()
	{
	return C3GPCompose::NewL(KLargeFileWriteBufferSize, KLargeFileWriteBufferMaxCount);
	}

void C3GPLibComposeLargeFile32bitAPI::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	RBuf8 videoBuf1;
	CleanupClosePushL(videoBuf1);		
	TBool keyFrame1;
	TUint videoTimestampMS = 0;	
	TUint videoTS1;
	TUint frameSize = 0;
	T3GPFrameDependencies aDependencies;		
	
	RFile fileName;
	CleanupClosePushL(fileName);		
	User::LeaveIfError(fileName.Open(iFs, KLargeVideoFile(), EFileRead));	
	
	C3GPParse* parser = C3GPParse::NewL();
	CleanupStack::PushL(parser);
	User::LeaveIfError(parser->Open(fileName));
	User::LeaveIfError(parser->GetVideoFrameSize(frameSize));
	User::LeaveIfError(videoBuf1.Create((TInt)frameSize));
	User::LeaveIfError(parser->ReadVideoFrame(videoBuf1, keyFrame1, videoTimestampMS, videoTS1));

	TInt error = parser->GetVideoFrameDependencies(aDependencies);
	if (error != KErrNone)
		{
		aDependencies.iDependsOn = E3GPDependencyUnknown;
		aDependencies.iIsDependedOn = E3GPDependencyUnknown;
		aDependencies.iHasRedundancy = E3GPRedundancyUnknown;
		}

	TInt err = KErrNone;
	TInt size = 0;
	TTime startTime;
	TTime endTime;
	startTime.UniversalTime();				

	// create a 3GB video file
	TInt videoLength = videoBuf1.Length();
	while (size + videoLength < K3GigaBytes)
	    {  				
		err = aComposer.WriteVideoFrame(videoBuf1, 100, keyFrame1, aDependencies);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Fail to write video frame, err = %d"), err);
			break;
			}			
		iFile.Size(size);
	    }

	endTime.UniversalTime();
	TTimeIntervalMinutes composeTime;				
	endTime.MinutesFrom(startTime, composeTime);	 
	INFO_PRINTF3(_L("Took %d minutes to compose a video file of size %d bytes"), composeTime.Int(), size);   		    		

	err = aComposer.Complete();
	if (err != KErrNone)
		{			
		SetTestStepError(err); 
		}

	User::LeaveIfError(parser->Complete());
	CleanupStack::PopAndDestroy(3, &videoBuf1); 		
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeLargeFileDiskFull
// -----------------------------------------------------------------------------
//     
C3GPLibComposeLargeFileDiskFull::C3GPLibComposeLargeFileDiskFull()
	{	
	}

void C3GPLibComposeLargeFileDiskFull::doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer)
	{
	RBuf8 videoBuf1;
	CleanupClosePushL(videoBuf1);		
	TBool keyFrame1;
	TUint videoTimestampMS = 0;	
	TUint videoTS1;
	TUint frameSize = 0;
	T3GPFrameDependencies aDependencies;		
	
	RFile fileName;
	CleanupClosePushL(fileName);		
	User::LeaveIfError(fileName.Open(iFs, KLargeVideoFile(), EFileRead));	
	
	C3GPParse* parser = C3GPParse::NewL();
	CleanupStack::PushL(parser);
	User::LeaveIfError(parser->Open(fileName));
	User::LeaveIfError(parser->GetVideoFrameSize(frameSize));
	User::LeaveIfError(videoBuf1.Create((TInt)frameSize));
	User::LeaveIfError(parser->ReadVideoFrame(videoBuf1, keyFrame1, videoTimestampMS, videoTS1));

	TInt error = parser->GetVideoFrameDependencies(aDependencies);
	if (error != KErrNone)
		{
		aDependencies.iDependsOn = E3GPDependencyUnknown;
		aDependencies.iIsDependedOn = E3GPDependencyUnknown;
		aDependencies.iHasRedundancy = E3GPRedundancyUnknown;
		}

	TInt err = KErrNone;
	TInt64 size = 0;
	TTime startTime;
	TTime endTime;
	startTime.UniversalTime();				

	// create a 3GB video file
	TInt videoLength = videoBuf1.Length();
	while (size + videoLength < K3GigaBytes)
	    {  				
		err = aComposer.WriteVideoFrame(videoBuf1, 100, keyFrame1, aDependencies);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Fail to write video frame, err = %d"), err);
			break;
			}			
		iFile64.Size(size);
	    }

	endTime.UniversalTime();
	TTimeIntervalMinutes composeTime;				
	endTime.MinutesFrom(startTime, composeTime);	 
	INFO_PRINTF3(_L("Took %d minutes to compose a video file of size %d bytes"), composeTime.Int(), size);   		    		

	if (err != KErrNone)
		{			
		SetTestStepError(err); 
		}

	User::LeaveIfError(parser->Complete());
	CleanupStack::PopAndDestroy(3, &videoBuf1); 
	
	// ignore the error
	aComposer.Complete();		
	}

// -----------------------------------------------------------------------------
// C3GPLibComposeFileWithAvcProfileCheck
// -----------------------------------------------------------------------------
//

const TInt KAvcDecoderConfigRecordLength = 7;

_LIT8(KAvcCTag, "avcC");

const TUint8 KAvcConfigVersion = 1;

const TUint8 KAvcProfileByteBaseline = 66;
const TUint8 KAvcProfileByteExtended = 77;
const TUint8 KAvcProfileByteMain = 88;
const TUint8 KAvcProfileByteHigh = 100;

C3GPLibComposeFileWithAvcProfileCheck::C3GPLibComposeFileWithAvcProfileCheck()
    : iAvcDecoderConfigRecord( KAvcDecoderConfigRecordLength )
    {
    }

TVerdict C3GPLibComposeFileWithAvcProfileCheck::doTestStepL()
    {
    if (TestStepResult() != EPass)
        {
        return TestStepResult();
        }
    
    // this triggers: 
    // 1. Create a composer
    // 2. Open the composer - C3GPLibComposeFileFormatCheck::doTestStepComposeOpenL
    // 3. Write Video & Audio Data - C3GPLibComposeFileFormatCheck::doTestStepComposeWriteVideoAudioL
    // 4. Write User Data - C3GPLibComposeFileFormatCheck::doTestStepComposeSetUserDataL
    // 5. Complete the composer 
    TVerdict verdict = C3GPLibComposeFile::doTestStepL();
    if (verdict == EPass)
        {
        // once the file is composed, it can be verified if the file composed has the same
        // AVC profile.
        verdict = VerifyAvcProfileL(iFile);
        }
    else
        {
        INFO_PRINTF1(_L("<C3GPLibComposeFileWithAvcProfileCheck> C3GPLibComposeFile::doTestStepL returns failed result"));      
        }
    SetTestStepResult(verdict);
        
    if (!ShouldRunOOMTest())
        {
        INFO_PRINTF2(_L("C3GPLibComposeFileWithAvcProfileCheck::doTestStepL returns %d"), TestStepResult());   
        }
    return TestStepResult();        
    }

/*
 * The aDecoderSpecificInfo argument is used by 3GPlib to populate the
 * avcC box. As this contains details of the AVC profile which is being
 * checked by this test we cannot use KDummyData as the other tests do.
 * 
 */
T3GPVideoPropertiesBase* C3GPLibComposeFileWithAvcProfileCheck::SetupAvcVideoL()
    {
    // See ISO 14496-15, chapter 5.2.4.1.1 for details of these...
    // config version (always 1)
    iAvcDecoderConfigRecord[0] = KAvcConfigVersion;
    // constraint flags
    iAvcDecoderConfigRecord[2] = 0; // dummy data 
    // level
    iAvcDecoderConfigRecord[3] = 0; // dummy data
    // reserved (6bits) + lengthSizeMinusOne (2bits)
    iAvcDecoderConfigRecord[4] = 0xFC; // 111111 00
    // reserved (3bits) + numOfSeqParamSets (5bits)
    iAvcDecoderConfigRecord[5] = 0xE0; // 111 00000
    // numOfPicParamSets
    iAvcDecoderConfigRecord[6] = 0;
    
    switch( iVideoType )
        {
        case E3GPAvcProfileBaseline:
            iAvcDecoderConfigRecord[1] = KAvcProfileByteBaseline;
            break;    

        case E3GPAvcProfileMain:
            iAvcDecoderConfigRecord[1] = KAvcProfileByteMain;
            break;

        case E3GPAvcProfileExtended:
            iAvcDecoderConfigRecord[1] = KAvcProfileByteExtended;
            break;

        case E3GPAvcProfileHigh:
            iAvcDecoderConfigRecord[1] = KAvcProfileByteHigh;
            break;
            
        default:
            User::Leave(KErrUnknown);
        }
    T3GPVideoPropertiesBase* video = new (ELeave) T3GPVideoPropertiesAvc(1000, TSize(100, 100), iAvcDecoderConfigRecord );
    return video;
    }

TVerdict C3GPLibComposeFileWithAvcProfileCheck::VerifyAvcProfileL(const RFile& aFile) 
    {
    // Seek to the beginning of the file
    TInt pos = 0;   
    User::LeaveIfError(aFile.Seek(ESeekStart, pos));
    
    TVerdict verdict = EFail;
    TBuf8<4> atom;
    TBuf8<1> byte;
    TInt rewind = -1 * ( atom.MaxLength() - 1 );
    TInt tmp = rewind;
    
    while(ETrue)
        {
        // Search for the 'avcC' atom to check the profile that follows it.
        
        // Read in next 4 bytes
        User::LeaveIfError(aFile.Read(atom));
        if (atom.Length() < atom.MaxLength())
            {
            break;
            }
            
        // Check if 'avcC' was found
        if (Mem::Compare((&KAvcCTag)->Ptr(), (&KAvcCTag)->Length(), atom.Ptr(), atom.Length()) == 0)
            {
            // read next byte, and it should contain config version (always 1)
            User::LeaveIfError(aFile.Read(byte));
            if( byte[0] != KAvcConfigVersion )
                {
                ERR_PRINTF2(_L("avcC atom config type not as expected! (%d instead of 1)"), byte[0] );                      
                break;
                }
            
            // read next byte, and it should contain profile indication
            User::LeaveIfError(aFile.Read(byte));
            switch( iVideoType )
                {
                case E3GPAvcProfileBaseline:
                    if( byte[0] == KAvcProfileByteBaseline )
                        {
                        verdict = EPass;
                        }
                    else
                        {
                        ERR_PRINTF3(_L("avcC atom config type not as expected! (%d instead of %d)"), byte[0], KAvcProfileByteBaseline );                      
                        }
                    break;
                    

                case E3GPAvcProfileMain:
                    if( byte[0] == KAvcProfileByteMain )
                        {
                        verdict = EPass;
                        }
                    else
                        {
                        ERR_PRINTF3(_L("avcC atom config type not as expected! (%d instead of %d)"), byte[0], KAvcProfileByteMain );                      
                        }
                    break;
                

                case E3GPAvcProfileExtended:
                    if( byte[0] == KAvcProfileByteExtended )
                        {
                        verdict = EPass;
                        }
                    else
                        {
                        ERR_PRINTF3(_L("avcC atom config type not as expected! (%d instead of %d)"), byte[0], KAvcProfileByteExtended );                      
                        }
                    break;
                    

                case E3GPAvcProfileHigh:
                    if( byte[0] == KAvcProfileByteHigh )
                        {
                        verdict = EPass;
                        }
                    else
                        {
                        ERR_PRINTF3(_L("avcC atom config type not as expected! (%d instead of %d)"), byte[0], KAvcProfileByteHigh );                      
                        }
                    break;
                    
                default:
                    ERR_PRINTF2(_L("avcC has unknown profile indication: %d"), byte[0] );
                }         
            
            // 'avcC' was found, regardless of the result, finish reading the file
            break;
            }
        else
            {
            // The 4 bytes read were not 'avcC'.
            // Need to rewind by 3 bytes since we need to step through the file
            // byte-by-byte...
            tmp = rewind; // Avoid rewind value being changed by Seek() function
            User::LeaveIfError( aFile.Seek( ESeekCurrent, tmp ) );
            }
        }
        
    return verdict;
    }

