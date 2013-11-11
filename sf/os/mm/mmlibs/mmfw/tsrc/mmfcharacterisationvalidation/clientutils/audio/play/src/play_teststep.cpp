// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "play_teststep.h"
#include <mda/common/resource.h>

/******************************************************************************************************
Open an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0001, MM-MMF-APLCLNT-CHRTZ-OGG-I-0001
MM-MMF-APLCLNT-CHRTZ-AMR-I-0001, MM-MMF-APLCLNT-CHRTZ-MP3-I-0001, MM-MMF-APLCLNT-CHRTZ-AAC-I-0001
*******************************************************************************************************/

RTestStepAPOpenFile* RTestStepAPOpenFile::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenFile* self = new (ELeave) RTestStepAPOpenFile(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenFile::RTestStepAPOpenFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenFile::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.0 - Open an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPOpenFile::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNone, for audio file: %S"), &iFileName);
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Open a corrupt audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0002, MM-MMF-APLCLNT-CHRTZ-OGG-I-0002
MM-MMF-APLCLNT-CHRTZ-AMR-I-0002, MM-MMF-APLCLNT-CHRTZ-MP3-I-0002, MM-MMF-APLCLNT-CHRTZ-AAC-I-0002
*******************************************************************************************************/

RTestStepAPOpenCorruptFile* RTestStepAPOpenCorruptFile::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenCorruptFile* self = new (ELeave) RTestStepAPOpenCorruptFile(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenCorruptFile::RTestStepAPOpenCorruptFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenCorruptFile::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.0 - Open a corrupt audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPOpenCorruptFile::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNotSupported);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNotSupported, for audio file: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Open an audio file with no controller to support the audio file format.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0003
*******************************************************************************************************/

RTestStepAPOpenFileNoController* RTestStepAPOpenFileNoController::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenFileNoController* self = new (ELeave) RTestStepAPOpenFileNoController(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenFileNoController::RTestStepAPOpenFileNoController(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenFileNoController::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.0 - Open an audio file with no controller to support the audio file format."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPOpenFileNoController::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
					
				// Set next state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNotSupported);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNotSupported, for audio file: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Open an audio file providing the incorrect path i.e. no such file exists in that path.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0004
*******************************************************************************************************/

RTestStepAPOpenNoFile* RTestStepAPOpenNoFile::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenNoFile* self = new (ELeave) RTestStepAPOpenNoFile(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenNoFile::RTestStepAPOpenNoFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenNoFile::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.0 - Open an audio file providing the incorrect path i.e. no such file exists in that path."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPOpenNoFile::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
					
				// Set next state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNotFound);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNotFound, for audio file: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Open an audio clip from a descriptor.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0006, MM-MMF-APLCLNT-CHRTZ-OGG-I-0006
MM-MMF-APLCLNT-CHRTZ-AMR-I-0006, MM-MMF-APLCLNT-CHRTZ-MP3-I-0006, MM-MMF-APLCLNT-CHRTZ-AAC-I-0006
*******************************************************************************************************/

RTestStepAPOpenDes* RTestStepAPOpenDes::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenDes* self = new (ELeave) RTestStepAPOpenDes(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenDes::RTestStepAPOpenDes(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName), iData(NULL, 0, 0)
	{
	iHeapSize = 500000;
	}

// Start test
void RTestStepAPOpenDes::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.1 - Open an audio clip from a descriptor."));
	
	// Get the filename
	InitialiseFileName();

	// Connect to file system and open file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	User::LeaveIfError( file.Open( fs, iFileName, EFileRead ) );
	CleanupClosePushL(file);
	
	INFO_PRINTF2( _L("File open successful %S"), &iFileName);
		
	// Set HBuf size
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
   	INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143

	TInt err = KErrNone;
	TRAP(err, iAudio = HBufC8::NewMaxL(size));
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("ERROR: HBufC8::NewMaxL(%d) failed"), size);
		}
	else
		{
		// Read data into Hbuf
		iData.Set(iAudio->Des());
		User::LeaveIfError(file.Read(iData));
		}
	
	CleanupStack::PopAndDestroy(2, &fs); // file, fs
	}

//State Machine Function
void RTestStepAPOpenDes::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				TRAP(err, iUtility->OpenDesL(iData));
				if (err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: Could not open descriptor"));
					StopTest(err);
					break;
					}
												  
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNone, for descriptor: %S"), &iFileName);
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Open an audio clip from a descriptor with no controller to support the audio file format.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0007, MM-MMF-APLCLNT-CHRTZ-OGG-I-0007
MM-MMF-APLCLNT-CHRTZ-AMR-I-0007, MM-MMF-APLCLNT-CHRTZ-MP3-I-0007, MM-MMF-APLCLNT-CHRTZ-AAC-I-0007
*******************************************************************************************************/

RTestStepAPOpenDesNoController* RTestStepAPOpenDesNoController::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenDesNoController* self = new (ELeave) RTestStepAPOpenDesNoController(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenDesNoController::RTestStepAPOpenDesNoController(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenDesNoController::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.1 - Open an audio clip from a descriptor with no controller to support the audio file format."));
	
	// Get the filename
	InitialiseFileName();
	
	// Connect to file system and open file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	User::LeaveIfError( file.Open( fs, iFileName, EFileRead ) );
	CleanupClosePushL(file);
	
	INFO_PRINTF2( _L("File open successful %S"), &iFileName);
		
	// Set HBuf size
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
   	INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143

	TInt err = KErrNone;
	TRAP(err, iAudio = HBufC8::NewMaxL(size));
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("ERROR: HBufC8::NewMaxL(%d) failed"), size);
		StopTest(err);
		}
	else
		{
		// Read data into Hbuf
		TPtr8 bufferDes(iAudio->Des());
		User::LeaveIfError(file.Read(bufferDes));
		}	
	
	CleanupStack::PopAndDestroy(2, &fs); // file, fs
	}

//State Machine Function
void RTestStepAPOpenDesNoController::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				TPtr8 bufferDes(iAudio->Des());
				TRAP(err, iUtility->OpenDesL(bufferDes));
				
				if (err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: Could not open descriptor"));
					StopTest(err);
					break;
					}
												  
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNotSupported);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNotSupported, for descriptor: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Open an audio clip file from a URL.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0009
*******************************************************************************************************/

RTestStepAPOpenURL* RTestStepAPOpenURL::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPOpenURL* self = new (ELeave) RTestStepAPOpenURL(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPOpenURL::RTestStepAPOpenURL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPOpenURL::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.2 - Open an audio clip file from a URL."));
	
	// Get the urlname
	// Note as currently the client does not support open from URL we will get KErrNotSupported
	// regardless of what the URL is.  Therefore just use a dummy one.
	TPtrC urlname;
	if(!GetStringFromConfig(iSectName, iKeyName, urlname))
		{
		ERR_PRINTF1(_L("ERROR: Cannot Get value fro Config."));
		}
  
	iFileName.Append(urlname);
	}

//State Machine Function
void RTestStepAPOpenURL::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenUrlL() %S"), &iFileName);
				TRAP(err, iUtility->OpenUrlL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not URL: %S"), &iFileName);
					StopTest(err);
					break;
					}
					
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNotSupported);
				break;
				}
			case EAPOpenComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received open complete callback with KErrNotSupported, for URL: %S"), &iFileName);
				StopTest(err, EKnownFailure);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Play an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0011, MM-MMF-APLCLNT-CHRTZ-OGG-I-0011
MM-MMF-APLCLNT-CHRTZ-AMR-I-0011, MM-MMF-APLCLNT-CHRTZ-MP3-I-0011, MM-MMF-APLCLNT-CHRTZ-AAC-I-0011
*******************************************************************************************************/

RTestStepAPPlayAudioFile* RTestStepAPPlayAudioFile::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPPlayAudioFile* self = new (ELeave) RTestStepAPPlayAudioFile(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPPlayAudioFile::RTestStepAPPlayAudioFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPPlayAudioFile::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.3 - Play an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPPlayAudioFile::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
					
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
				}
			case EAPPlayComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received for play complete callback with KErrNone, for audio file: %S"), &iFileName);
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Play an audio file while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0012, MM-MMF-APLCLNT-CHRTZ-OGG-I-0012
MM-MMF-APLCLNT-CHRTZ-AMR-I-0012, MM-MMF-APLCLNT-CHRTZ-MP3-I-0012, MM-MMF-APLCLNT-CHRTZ-AAC-I-0012
*******************************************************************************************************/

RTestStepAPPlayAudioFileWhilePlaying* RTestStepAPPlayAudioFileWhilePlaying::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPPlayAudioFileWhilePlaying* self = new (ELeave) RTestStepAPPlayAudioFileWhilePlaying(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPPlayAudioFileWhilePlaying::RTestStepAPPlayAudioFileWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPPlayAudioFileWhilePlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.3 - Play an audio file while an audio file while in the EPlaying state."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPPlayAudioFileWhilePlaying::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
							   
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call Play() again
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				INFO_PRINTF1(_L("iUtility->Play() 2nd"));
				// 2nd Play
				iUtility->Play();
				SetExpectedState(EAPPlayComplete, KErrNotReady);
				break;
				}
			case EAPPlayComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received for play complete callback with KErrNotReady, for audio file: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPPlayAudioFileWhilePlaying::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
		
/******************************************************************************************************
Play an audio file before opening the clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0013, MM-MMF-APLCLNT-CHRTZ-OGG-I-0013
MM-MMF-APLCLNT-CHRTZ-AMR-I-0013, MM-MMF-APLCLNT-CHRTZ-MP3-I-0013, MM-MMF-APLCLNT-CHRTZ-AAC-I-0013
*******************************************************************************************************/

RTestStepAPPlayAudioFileBeforeOpenClip* RTestStepAPPlayAudioFileBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPPlayAudioFileBeforeOpenClip* self = new (ELeave) RTestStepAPPlayAudioFileBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPPlayAudioFileBeforeOpenClip::RTestStepAPPlayAudioFileBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPPlayAudioFileBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.3 - Play an audio file  before opening the clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPPlayAudioFileBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				
				SetExpectedState(EAPPlayComplete, KErrNotReady);
				break;
				}
			case EAPPlayComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received for play complete callback with KErrNotReady, for audio file: %S"), &iFileName);
				StopTest(err, EPass);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Stop while play an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0014, MM-MMF-APLCLNT-CHRTZ-OGG-I-0014
MM-MMF-APLCLNT-CHRTZ-AMR-I-0014, MM-MMF-APLCLNT-CHRTZ-MP3-I-0014, MM-MMF-APLCLNT-CHRTZ-AAC-I-0014
*******************************************************************************************************/

RTestStepAPStopWhilePlayingClip* RTestStepAPStopWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPStopWhilePlayingClip* self = new (ELeave) RTestStepAPStopWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPStopWhilePlayingClip::RTestStepAPStopWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPStopWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.4 - Stop while play an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPStopWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Stop iUtility
				INFO_PRINTF1(_L("iUtility->Stop()"));
				iUtility->Stop();
				
				TTimeIntervalMicroSeconds currentPosition(0);
				TTimeIntervalMicroSeconds startPosition(0);

				err = iUtility->GetPosition(currentPosition);

				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
					
				if(currentPosition != startPosition)
					{
					ERR_PRINTF1(_L("ERROR: Position not reset to start."));
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2(_L("Stop() has reset position back to start correctly, for audio file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPStopWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Pause while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0015, MM-MMF-APLCLNT-CHRTZ-OGG-I-0015
MM-MMF-APLCLNT-CHRTZ-AMR-I-0015, MM-MMF-APLCLNT-CHRTZ-MP3-I-0015, MM-MMF-APLCLNT-CHRTZ-AAC-I-0015
*******************************************************************************************************/

RTestStepAPPauseWhilePlayingClip* RTestStepAPPauseWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPPauseWhilePlayingClip* self = new (ELeave) RTestStepAPPauseWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPPauseWhilePlayingClip::RTestStepAPPauseWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPPauseWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.5 - Pause while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPPauseWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds position1(0);
				TTimeIntervalMicroSeconds position2(0);
				TTimeIntervalMicroSeconds startPosition(0);

				err = iUtility->GetPosition(position1);
				
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
				
				// Wait for 1 second
				User::After(KDelay);
				
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
					
				err = iUtility->GetPosition(position2);

				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
				
				// Check that the position has not been reset to zero.
				if (position1 == startPosition)
					{
					ERR_PRINTF1(_L("ERROR: Position reset to start."));
					StopTest(err, EFail);
					break;
					}
				
				// Check that the position remains the same while paused.	
				if (position1 != position2)
					{
					ERR_PRINTF1(_L("ERROR: Position changed during pause."));
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2(_L("The position remains the same after a 1 second delay, for audio file: %S"), &iFileName);
					StopTest(err);
					break;
					}				
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPPauseWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Close while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0016, MM-MMF-APLCLNT-CHRTZ-OGG-I-0016
MM-MMF-APLCLNT-CHRTZ-AMR-I-0016, MM-MMF-APLCLNT-CHRTZ-MP3-I-0016, MM-MMF-APLCLNT-CHRTZ-AAC-I-0016
*******************************************************************************************************/

RTestStepAPCloseWhilePlayingClip* RTestStepAPCloseWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPCloseWhilePlayingClip* self = new (ELeave) RTestStepAPCloseWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPCloseWhilePlayingClip::RTestStepAPCloseWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName),
	iSecondOpen(EFalse)
	{
	}

// Start test
void RTestStepAPCloseWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.6 - Close while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPCloseWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				if (iSecondOpen)
					{
					// As callback was received therefore the audio file was closed
					// Test Finished
					INFO_PRINTF2(_L("Received open complete callback with KErrNone, for audio file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				else
					{
					// Set flag in preparation for next time round.
					iSecondOpen = ETrue;

					// Play iUtility
					INFO_PRINTF1(_L("iUtility->Play()"));
					iUtility->Play();
				
					// Set state
					iState = EAPPlaying;
					SetExpectedState(EAPPlaying, KErrNone);
								
					// Start the timer to call stop
					TCallBack callback (TimerCallback, this);
					iTimer->Start(KDelay, KDelay, callback);
					}
				break;
				}
			case EAPPlaying:
				{
				// Close audio file
				INFO_PRINTF1(_L("iUtility->Close()"));
				iUtility->Close();
				
				// To verify that the audio file has been closed try to open it again.
				// Should receive the open complete callback with no error code.
				
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
					
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPCloseWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Get the max volume.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0017, MM-MMF-APLCLNT-CHRTZ-OGG-I-0017
MM-MMF-APLCLNT-CHRTZ-AMR-I-0017, MM-MMF-APLCLNT-CHRTZ-MP3-I-0017, MM-MMF-APLCLNT-CHRTZ-AAC-I-0017
*******************************************************************************************************/

RTestStepAPGetMaxVolume* RTestStepAPGetMaxVolume::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetMaxVolume* self = new (ELeave) RTestStepAPGetMaxVolume(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMaxVolume::RTestStepAPGetMaxVolume(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetMaxVolume::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.7 - Get the max volume."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetMaxVolume::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt maxVolume = 0;
				
				// Get the maximum volume
				INFO_PRINTF1(_L("iUtility->MaxVolume()"));
				maxVolume = iUtility->MaxVolume();
				
				INFO_PRINTF2(_L("Max volume is: "), maxVolume);
				
				// Check that can't set volume greater than maximum.
				TInt greaterMaxVolume = maxVolume + 10;
				
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(greaterMaxVolume);
				
				TInt currentVolume = 0;		 
				
				INFO_PRINTF1(_L("iUtility->GetVolume()"));
				err = iUtility->GetVolume(currentVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				// Volume should remain at maximum level.
				if (currentVolume != maxVolume)
					{
					ERR_PRINTF3(_L("ERROR: GetVolume() did not maximum volume. Actual volume = %d, Expected volume = %d"), currentVolume, maxVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF1(_L("Get maximum volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the max volume while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0018, MM-MMF-APLCLNT-CHRTZ-OGG-I-0018
MM-MMF-APLCLNT-CHRTZ-AMR-I-0018, MM-MMF-APLCLNT-CHRTZ-MP3-I-0018, MM-MMF-APLCLNT-CHRTZ-AAC-I-0018
*******************************************************************************************************/

RTestStepAPGetMaxVolumeWhilePlayingClip* RTestStepAPGetMaxVolumeWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetMaxVolumeWhilePlayingClip* self = new (ELeave) RTestStepAPGetMaxVolumeWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMaxVolumeWhilePlayingClip::RTestStepAPGetMaxVolumeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetMaxVolumeWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.7 - Get the max volume while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetMaxVolumeWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt maxVolume = 0;
				
				// Get the maximum volume
				INFO_PRINTF1(_L("iUtility->MaxVolume()"));
				maxVolume = iUtility->MaxVolume();
				
				INFO_PRINTF2(_L("Max volume is: "), maxVolume);
				
				// Check that can't set volume greater than maximum.
				TInt greaterMaxVolume = maxVolume + 10;
				
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(greaterMaxVolume);
				
				TInt currentVolume = 0;		 
				
				INFO_PRINTF1(_L("iUtility->GetVolume()"));
				err = iUtility->GetVolume(currentVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				// Volume should remain at maximum level.
				if (currentVolume != maxVolume)
					{
					ERR_PRINTF3(_L("ERROR: GetVolume() did not maximum volume. Actual volume = %d, Expected volume = %d"), currentVolume, maxVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF1(_L("Get maximum volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetMaxVolumeWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Get the max volume before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0019
*******************************************************************************************************/

RTestStepAPGetMaxVolumeBeforeOpenClip* RTestStepAPGetMaxVolumeBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetMaxVolumeBeforeOpenClip* self = new (ELeave) RTestStepAPGetMaxVolumeBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMaxVolumeBeforeOpenClip::RTestStepAPGetMaxVolumeBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetMaxVolumeBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.7 - Get the max volume before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPGetMaxVolumeBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt maxVolume = 0;
				
				// Get the maximum volume
				INFO_PRINTF1(_L("iUtility->MaxVolume()"));
				maxVolume = iUtility->MaxVolume();
				
				INFO_PRINTF2(_L("Max volume is: "), maxVolume);
				
				// Expect the call to MaxVolume() to generate a panic, so if not then fail.
				StopTest(err, EFail);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Get the current playback volume.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0020, MM-MMF-APLCLNT-CHRTZ-OGG-I-0020
MM-MMF-APLCLNT-CHRTZ-AMR-I-0020, MM-MMF-APLCLNT-CHRTZ-MP3-I-0020, MM-MMF-APLCLNT-CHRTZ-AAC-I-0020
*******************************************************************************************************/

RTestStepAPGetPlaybackVolume* RTestStepAPGetPlaybackVolume::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetPlaybackVolume* self = new (ELeave) RTestStepAPGetPlaybackVolume(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetPlaybackVolume::RTestStepAPGetPlaybackVolume(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetPlaybackVolume::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.8 - Get the current playback volume."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetPlaybackVolume::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt currentVolume = 0;		 
				
				// Get the current playback volume
				INFO_PRINTF1(_L("iUtility->GetVolume()"));
				err = iUtility->GetVolume(currentVolume);
				
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume is: %d"), currentVolume);
					INFO_PRINTF1(_L("Get volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the current playback volume while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0021, MM-MMF-APLCLNT-CHRTZ-OGG-I-0021
MM-MMF-APLCLNT-CHRTZ-AMR-I-0021, MM-MMF-APLCLNT-CHRTZ-MP3-I-0021, MM-MMF-APLCLNT-CHRTZ-AAC-I-0021
*******************************************************************************************************/

RTestStepAPGetPlaybackVolumeWhilePlayingClip* RTestStepAPGetPlaybackVolumeWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetPlaybackVolumeWhilePlayingClip* self = new (ELeave) RTestStepAPGetPlaybackVolumeWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetPlaybackVolumeWhilePlayingClip::RTestStepAPGetPlaybackVolumeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetPlaybackVolumeWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.8 - Get the current playback volume while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetPlaybackVolumeWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt currentVolume = 0;		 
				
				// Get the current playback volume
				INFO_PRINTF1(_L("iUtility->GetVolume()"));
				err = iUtility->GetVolume(currentVolume);
				
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume is: %d"), currentVolume);
					INFO_PRINTF1(_L("Get volume successful"));
					StopTest(err);
					break;
					}
					
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetPlaybackVolumeWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
		
/******************************************************************************************************
Get the current playback volume before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0022
*******************************************************************************************************/

RTestStepAPGetPlaybackVolumeBeforeOpenClip* RTestStepAPGetPlaybackVolumeBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetPlaybackVolumeBeforeOpenClip* self = new (ELeave) RTestStepAPGetPlaybackVolumeBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetPlaybackVolumeBeforeOpenClip::RTestStepAPGetPlaybackVolumeBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetPlaybackVolumeBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.8 - Get the current playback volume before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPGetPlaybackVolumeBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt currentVolume = 0;		 
				
				// Get the current playback volume
				INFO_PRINTF1(_L("iUtility->GetVolume()"));
				err = iUtility->GetVolume(currentVolume);
				
				// Expect the call to GetVolume() to return KErrNotReady.
				if(err != KErrNotReady)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume is: %d"), currentVolume);
					INFO_PRINTF1(_L("Get volume successful"));
					StopTest(err, EPass);
					break;
					}			
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current playback volume to zero.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0023, MM-MMF-APLCLNT-CHRTZ-OGG-I-0023
MM-MMF-APLCLNT-CHRTZ-AMR-I-0023, MM-MMF-APLCLNT-CHRTZ-MP3-I-0023, MM-MMF-APLCLNT-CHRTZ-AAC-I-0023
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToZero* RTestStepAPSetPlaybackVolumeToZero::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToZero* self = new (ELeave) RTestStepAPSetPlaybackVolumeToZero(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToZero::RTestStepAPSetPlaybackVolumeToZero(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToZero::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to zero."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToZero::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt zeroVolume = 0;
				TInt actualVolume = 0;
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(zeroVolume);
											  
				// Check that volume was set properly
				err = iUtility->GetVolume(actualVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				if (actualVolume != zeroVolume)
					{
					ERR_PRINTF2(_L("ERROR: Volume was not set to zero.  Actual volume = %d"), actualVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume was set to: %d"), actualVolume);
					INFO_PRINTF1(_L("Set volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current playback volume to the maximum value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0024, MM-MMF-APLCLNT-CHRTZ-OGG-I-0024
MM-MMF-APLCLNT-CHRTZ-AMR-I-0024, MM-MMF-APLCLNT-CHRTZ-MP3-I-0024, MM-MMF-APLCLNT-CHRTZ-AAC-I-0024
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToMax* RTestStepAPSetPlaybackVolumeToMax::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToMax* self = new (ELeave) RTestStepAPSetPlaybackVolumeToMax(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToMax::RTestStepAPSetPlaybackVolumeToMax(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToMax::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to the maximum value."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToMax::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt maxVolume = iUtility->MaxVolume();
				TInt actualVolume = 0;
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(maxVolume);
											  
				// Check that volume was set properly
				err = iUtility->GetVolume(actualVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				if (actualVolume != maxVolume)
					{
					ERR_PRINTF3(_L("ERROR: Volume was not set to maximum. Actual volume = %d, Max volume = %d"), actualVolume, maxVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume was set to: %d"), actualVolume);
					INFO_PRINTF1(_L("Set volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the current playback volume to zero while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0025, MM-MMF-APLCLNT-CHRTZ-OGG-I-0025
MM-MMF-APLCLNT-CHRTZ-AMR-I-0025, MM-MMF-APLCLNT-CHRTZ-MP3-I-0025, MM-MMF-APLCLNT-CHRTZ-AAC-I-0025
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip* RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip* self = new (ELeave) RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to zero while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt zeroVolume = 0;
				TInt actualVolume = 0;
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(zeroVolume);
											  
				// Check that volume was set properly
				err = iUtility->GetVolume(actualVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				if (actualVolume != zeroVolume)
					{
					ERR_PRINTF2(_L("ERROR: Volume was not set to zero.  Actual volume = %d"), actualVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume was set to: %d"), actualVolume);
					INFO_PRINTF1(_L("Set volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback volume to the maximum value while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0026, MM-MMF-APLCLNT-CHRTZ-OGG-I-0026
MM-MMF-APLCLNT-CHRTZ-AMR-I-0026, MM-MMF-APLCLNT-CHRTZ-MP3-I-0026, MM-MMF-APLCLNT-CHRTZ-AAC-I-0026
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip* RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip* self = new (ELeave) RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to the maximum value while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt maxVolume = iUtility->MaxVolume();
				TInt actualVolume = 0;
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				iUtility->SetVolume(maxVolume);
											  
				// Check that volume was set properly
				err = iUtility->GetVolume(actualVolume);
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: GetVolume() returned with error code: %d"), err);
					StopTest(err);
					break;
					}
					
				if (actualVolume != maxVolume)
					{
					ERR_PRINTF3(_L("ERROR: Volume was not set to maximum. Actual volume = %d, Max volume = %d"), actualVolume, maxVolume);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2( _L("The curent volume was set to: %d"), actualVolume);
					INFO_PRINTF1(_L("Set volume successful"));
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback volume to zero before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0027
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip* RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip* self = new (ELeave) RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip::RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to zero before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt volumeLevel = 0;		 
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				err = iUtility->SetVolume(volumeLevel);
											  
				// Expect the call to SetVolume() to return KErrNotReady
				if (err == KErrNotReady)
					{
					INFO_PRINTF2(_L("-- iUtility->SetVolume() before OpenFileL() returned expected error (KErrNotReady): %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("-- iUtility->SetVolume() returned unexpected error : %d"), err);
					StopTest(err, EFail);
					}
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current playback volume to the maximum before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0028
*******************************************************************************************************/

RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip* RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip* self = new (ELeave) RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip::RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.9 - Set the current playback volume to the maximum before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Get the Maximum playback volume
				INFO_PRINTF1(_L("iUtility->MaxVolume()"));
				TInt maxVolume = iUtility->MaxVolume();
				
				// Set the current playback volume
				INFO_PRINTF1(_L("iUtility->SetVolume()"));
				err = iUtility->SetVolume(maxVolume);
							  
				// Expect the call to SetVolume() to generate a panic, so if not then fail (Do the usual checking here).
				if (err == KErrNotReady)
					{
					INFO_PRINTF2(_L("-- iUtility->SetVolume() before OpenFileL() returned expected error (KErrNotReady): %d"), err);
					StopTest(err, EPass);
					}
				else
					{
					ERR_PRINTF2(_L("-- iUtility->SetVolume() returned unexpected error : %d"), err);
					StopTest(err, EFail);
					}
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the volume ramp.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0029, MM-MMF-APLCLNT-CHRTZ-OGG-I-0029
MM-MMF-APLCLNT-CHRTZ-AMR-I-0029, MM-MMF-APLCLNT-CHRTZ-MP3-I-0029, MM-MMF-APLCLNT-CHRTZ-AAC-I-0029
*******************************************************************************************************/

RTestStepAPSetVolumeRamp* RTestStepAPSetVolumeRamp::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetVolumeRamp* self = new (ELeave) RTestStepAPSetVolumeRamp(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetVolumeRamp::RTestStepAPSetVolumeRamp(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetVolumeRamp::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.10 - Set the volume ramp."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetVolumeRamp::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TTimeIntervalMicroSeconds ramp(100);
								
				// Set the period over which the volume level is to rise smoothly from nothing to the
				// normal volume level
				INFO_PRINTF1(_L("iUtility->SetVolumeRamp()"));
				iUtility->SetVolumeRamp(ramp);
				
				// Test Finished
				INFO_PRINTF2(_L("Volume ramp set to: %d"), I64INT(ramp.Int64()));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the volume ramp while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0030, MM-MMF-APLCLNT-CHRTZ-OGG-I-0030
MM-MMF-APLCLNT-CHRTZ-AMR-I-0030, MM-MMF-APLCLNT-CHRTZ-MP3-I-0030, MM-MMF-APLCLNT-CHRTZ-AAC-I-0030
*******************************************************************************************************/

RTestStepAPSetVolumeRampWhilePlayingClip* RTestStepAPSetVolumeRampWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetVolumeRampWhilePlayingClip* self = new (ELeave) RTestStepAPSetVolumeRampWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetVolumeRampWhilePlayingClip::RTestStepAPSetVolumeRampWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetVolumeRampWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.10 - Set the volume ramp while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetVolumeRampWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds ramp(100);
								
				// Set the period over which the volume level is to rise smoothly from nothing to the
				// normal volume level
				INFO_PRINTF1(_L("iUtility->SetVolumeRamp()"));
				iUtility->SetVolumeRamp(ramp);

				// Test Finished
				INFO_PRINTF2(_L("Volume ramp set to: %d"), I64INT(ramp.Int64()));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetVolumeRampWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0032, MM-MMF-APLCLNT-CHRTZ-OGG-I-0032
MM-MMF-APLCLNT-CHRTZ-AMR-I-0032, MM-MMF-APLCLNT-CHRTZ-MP3-I-0032, MM-MMF-APLCLNT-CHRTZ-AAC-I-0032
*******************************************************************************************************/

RTestStepAPSetRepeatsToZero* RTestStepAPSetRepeatsToZero::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToZero* self = new (ELeave) RTestStepAPSetRepeatsToZero(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToZero::RTestStepAPSetRepeatsToZero(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToZero::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to zero."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetRepeatsToZero::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt zeroRepeat = 0;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(zeroRepeat, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: 0, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0033, MM-MMF-APLCLNT-CHRTZ-OGG-I-0033
MM-MMF-APLCLNT-CHRTZ-AMR-I-0033, MM-MMF-APLCLNT-CHRTZ-MP3-I-0033, MM-MMF-APLCLNT-CHRTZ-AAC-I-0033
*******************************************************************************************************/

RTestStepAPSetRepeatsToForever* RTestStepAPSetRepeatsToForever::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToForever* self = new (ELeave) RTestStepAPSetRepeatsToForever(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToForever::RTestStepAPSetRepeatsToForever(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToForever::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to KMdaRepeatForever."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetRepeatsToForever::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt repeatForever = KMdaRepeatForever;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(repeatForever, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: KMdaRepeatForever, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0034, MM-MMF-APLCLNT-CHRTZ-OGG-I-0034
MM-MMF-APLCLNT-CHRTZ-AMR-I-0034, MM-MMF-APLCLNT-CHRTZ-MP3-I-0034, MM-MMF-APLCLNT-CHRTZ-AAC-I-0034
*******************************************************************************************************/

RTestStepAPSetRepeatsToZeroWhilePlayingClip* RTestStepAPSetRepeatsToZeroWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToZeroWhilePlayingClip* self = new (ELeave) RTestStepAPSetRepeatsToZeroWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToZeroWhilePlayingClip::RTestStepAPSetRepeatsToZeroWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToZeroWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to zero while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetRepeatsToZeroWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt zeroRepeat = 0;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(zeroRepeat, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: 0, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetRepeatsToZeroWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0035, MM-MMF-APLCLNT-CHRTZ-OGG-I-0035
MM-MMF-APLCLNT-CHRTZ-AMR-I-0035, MM-MMF-APLCLNT-CHRTZ-MP3-I-0035, MM-MMF-APLCLNT-CHRTZ-AAC-I-0035
*******************************************************************************************************/

RTestStepAPSetRepeatsToForeverWhilePlayingClip* RTestStepAPSetRepeatsToForeverWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToForeverWhilePlayingClip* self = new (ELeave) RTestStepAPSetRepeatsToForeverWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToForeverWhilePlayingClip::RTestStepAPSetRepeatsToForeverWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToForeverWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to KMdaRepeatForever while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetRepeatsToForeverWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt repeatForever = KMdaRepeatForever;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(repeatForever, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: KMdaRepeatForever, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetRepeatsToForeverWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0036
*******************************************************************************************************/

RTestStepAPSetRepeatsToZeroBeforeOpenClip* RTestStepAPSetRepeatsToZeroBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToZeroBeforeOpenClip* self = new (ELeave) RTestStepAPSetRepeatsToZeroBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToZeroBeforeOpenClip::RTestStepAPSetRepeatsToZeroBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToZeroBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to zero before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPSetRepeatsToZeroBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt zeroRepeat = 0;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(zeroRepeat, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: 0, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0037
*******************************************************************************************************/

RTestStepAPSetRepeatsToForeverBeforeOpenClip* RTestStepAPSetRepeatsToForeverBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetRepeatsToForeverBeforeOpenClip* self = new (ELeave) RTestStepAPSetRepeatsToForeverBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetRepeatsToForeverBeforeOpenClip::RTestStepAPSetRepeatsToForeverBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetRepeatsToForeverBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.11 - Set the number of times the audio clip is to be repeated to KMdaRepeatForever before opening the audio clip."));
	}

//State Machine Function
void RTestStepAPSetRepeatsToForeverBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt repeatForever = KMdaRepeatForever;
				TTimeIntervalMicroSeconds zeroTrailingSilence(0); // No gap between repeats
				
				// Set the number of times the audio sample is to be repeated during the playback operation.
				INFO_PRINTF1(_L("iUtility->SetRepeats()"));
				iUtility->SetRepeats(repeatForever, zeroTrailingSilence);

				// Test Finished
				INFO_PRINTF1(_L("Repeats set to: KMdaRepeatForever, Trailing silence: 0"));
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the priority for playback.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0038, MM-MMF-APLCLNT-CHRTZ-OGG-I-0038
MM-MMF-APLCLNT-CHRTZ-AMR-I-0038, MM-MMF-APLCLNT-CHRTZ-MP3-I-0038, MM-MMF-APLCLNT-CHRTZ-AAC-I-0038
*******************************************************************************************************/

RTestStepAPSetPriority* RTestStepAPSetPriority::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPriority* self = new (ELeave) RTestStepAPSetPriority(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPriority::RTestStepAPSetPriority(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPriority::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.12 - Set the priority for playback."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPriority::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Set the priority for the playback
				INFO_PRINTF1(_L("iUtility->SetPriority()"));
				err = iUtility->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceNone);
				
				// Test Finished
				INFO_PRINTF1(_L("Priority set to: EMdaPriorityMin, preference: EMdaPriorityPreferenceNone"));			  
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the priority for playback while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0039, MM-MMF-APLCLNT-CHRTZ-OGG-I-0039
MM-MMF-APLCLNT-CHRTZ-AMR-I-0039, MM-MMF-APLCLNT-CHRTZ-MP3-I-0039, MM-MMF-APLCLNT-CHRTZ-AAC-I-0039
*******************************************************************************************************/

RTestStepAPSetPriorityWhilePlayingClip* RTestStepAPSetPriorityWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPriorityWhilePlayingClip* self = new (ELeave) RTestStepAPSetPriorityWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPriorityWhilePlayingClip::RTestStepAPSetPriorityWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPriorityWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.12 - Set the priority for playback while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPriorityWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Set the priority for the playback
				INFO_PRINTF1(_L("iUtility->SetPriority()"));
				err = iUtility->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceNone);
				
				// Test Finished
				INFO_PRINTF1(_L("Priority set to: EMdaPriorityMin, preference: EMdaPriorityPreferenceNone"));

				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetPriorityWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the priority for playback before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0040
*******************************************************************************************************/

RTestStepAPSetPriorityBeforeOpenClip* RTestStepAPSetPriorityBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPriorityBeforeOpenClip* self = new (ELeave) RTestStepAPSetPriorityBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPriorityBeforeOpenClip::RTestStepAPSetPriorityBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPriorityBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.12 - Set the priority for playback before opening the audio clip."));
	}

// State Machine Function
void RTestStepAPSetPriorityBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:		  	
				{
				// Set the priority for the playback
				INFO_PRINTF1(_L("iUtility->SetPriority()"));
				err = iUtility->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceNone);

				// Expect error code KErrNotReady otherwise test failed
				if (err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0041, MM-MMF-APLCLNT-CHRTZ-OGG-I-0041
MM-MMF-APLCLNT-CHRTZ-AMR-I-0041, MM-MMF-APLCLNT-CHRTZ-MP3-I-0041, MM-MMF-APLCLNT-CHRTZ-AAC-I-0041
*******************************************************************************************************/

RTestStepAPGetPosition* RTestStepAPGetPosition::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetPosition* self = new (ELeave) RTestStepAPGetPosition(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetPosition::RTestStepAPGetPosition(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetPosition::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.13 - Get the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetPosition::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TTimeIntervalMicroSeconds position = 100;
				
				// Get the current head position
				INFO_PRINTF1(_L("iUtility->GetPosition()"));
				err = iUtility->GetPosition(position);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Head position should be zero as not playing, otherwise test failed.
				if (position == 0)
					{
					INFO_PRINTF1(_L("Head position was as expected"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Expected head position: 0, Actual head position: %d"), I64INT(position.Int64()));
					StopTest(err, EFail);
					break;
					}					
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the current head position while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0042, MM-MMF-APLCLNT-CHRTZ-OGG-I-0042
MM-MMF-APLCLNT-CHRTZ-AMR-I-0042, MM-MMF-APLCLNT-CHRTZ-MP3-I-0042, MM-MMF-APLCLNT-CHRTZ-AAC-I-0042
*******************************************************************************************************/

RTestStepAPGetPositionWhilePlayingClip* RTestStepAPGetPositionWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetPositionWhilePlayingClip* self = new (ELeave) RTestStepAPGetPositionWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetPositionWhilePlayingClip::RTestStepAPGetPositionWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetPositionWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.13 - Get the current head position while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetPositionWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set next state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call pause
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds position = 0;
				
				// Get the current head position
				INFO_PRINTF1(_L("iUtility->GetPosition()"));
				err = iUtility->GetPosition(position);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
			   
				if (TimeComparison(position.Int64(), KDelay, KExpectedDeviation)) 
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Retrieved head position successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Head position not set correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), position.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), KDelay);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetPositionWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0043, MM-MMF-APLCLNT-CHRTZ-OGG-I-0043
MM-MMF-APLCLNT-CHRTZ-AMR-I-0043, MM-MMF-APLCLNT-CHRTZ-MP3-I-0043, MM-MMF-APLCLNT-CHRTZ-AAC-I-0043
*******************************************************************************************************/

RTestStepAPSetPosition* RTestStepAPSetPosition::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPosition* self = new (ELeave) RTestStepAPSetPosition(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPosition::RTestStepAPSetPosition(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPosition::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPosition::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TTimeIntervalMicroSeconds position = 100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
	 
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (TimeComparison(actualPosition.Int64(), position.Int64(), KExpectedDeviation))
					{
					// Head position should be as set
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(actualPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), &position);
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current head position to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0044, MM-MMF-APLCLNT-CHRTZ-OGG-I-0044
MM-MMF-APLCLNT-CHRTZ-AMR-I-0044, MM-MMF-APLCLNT-CHRTZ-MP3-I-0044, MM-MMF-APLCLNT-CHRTZ-AAC-I-0044
*******************************************************************************************************/

RTestStepAPSetPositionNegative* RTestStepAPSetPositionNegative::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionNegative* self = new (ELeave) RTestStepAPSetPositionNegative(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionNegative::RTestStepAPSetPositionNegative(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionNegative::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position to a negative value."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionNegative::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TTimeIntervalMicroSeconds position = -100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
							  
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (TimeComparison(actualPosition.Int64(), 0, KExpectedDeviation))
					{
					// Head position should be start
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(actualPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF1(_L("  Expected: 0"));
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the current head position to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0045, MM-MMF-APLCLNT-CHRTZ-OGG-I-0045
MM-MMF-APLCLNT-CHRTZ-AMR-I-0045, MM-MMF-APLCLNT-CHRTZ-MP3-I-0045, MM-MMF-APLCLNT-CHRTZ-AAC-I-0045
*******************************************************************************************************/

RTestStepAPSetPositionPastDuration* RTestStepAPSetPositionPastDuration::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionPastDuration* self = new (ELeave) RTestStepAPSetPositionPastDuration(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionPastDuration::RTestStepAPSetPositionPastDuration(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionPastDuration::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position to a value greater than the duration."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionPastDuration::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TTimeIntervalMicroSeconds duration = iUtility->Duration();
				
				TInt64 pos_val = duration.Int64();
				pos_val += 100;
				
				TTimeIntervalMicroSeconds position = pos_val;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
				
				// Get the current head position
				err = iUtility->GetPosition(position);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Current head position should be set to the end of the duration otherwise test failed.				
				if (position == duration)
					{
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), &duration);
					ERR_PRINTF2(_L("  Actual: %d"), &position);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current head position while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0046, MM-MMF-APLCLNT-CHRTZ-OGG-I-0046
MM-MMF-APLCLNT-CHRTZ-AMR-I-0046, MM-MMF-APLCLNT-CHRTZ-MP3-I-0046, MM-MMF-APLCLNT-CHRTZ-AAC-I-0046
*******************************************************************************************************/

RTestStepAPSetPositionWhilePlayingClip* RTestStepAPSetPositionWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionWhilePlayingClip* self = new (ELeave) RTestStepAPSetPositionWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionWhilePlayingClip::RTestStepAPSetPositionWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds position = 100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
				
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Head position should be as set to specified position
				if (TimeComparison(actualPosition.Int64(), position.Int64(), KExpectedDeviation))
					{
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), &position);
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current head position while playing is in progress to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0047, MM-MMF-APLCLNT-CHRTZ-OGG-I-0047
MM-MMF-APLCLNT-CHRTZ-AMR-I-0047, MM-MMF-APLCLNT-CHRTZ-MP3-I-0047, MM-MMF-APLCLNT-CHRTZ-AAC-I-0047
*******************************************************************************************************/

RTestStepAPSetPositionNegativeWhilePlayingClip* RTestStepAPSetPositionNegativeWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionNegativeWhilePlayingClip* self = new (ELeave) RTestStepAPSetPositionNegativeWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionNegativeWhilePlayingClip::RTestStepAPSetPositionNegativeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionNegativeWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing is in progress to a negative value."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionNegativeWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds position = -100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
							  
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (TimeComparison(actualPosition.Int64(), 0, KExpectedDeviation))
					{
					// Head position should be start
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF1(_L("  Expected: 0"));
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionNegativeWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current head position while playing is in progress to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0048, MM-MMF-APLCLNT-CHRTZ-OGG-I-0048
MM-MMF-APLCLNT-CHRTZ-AMR-I-0048, MM-MMF-APLCLNT-CHRTZ-MP3-I-0048, MM-MMF-APLCLNT-CHRTZ-AAC-I-0048
*******************************************************************************************************/

RTestStepAPSetPositionPastDurationWhilePlayingClip* RTestStepAPSetPositionPastDurationWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionPastDurationWhilePlayingClip* self = new (ELeave) RTestStepAPSetPositionPastDurationWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionPastDurationWhilePlayingClip::RTestStepAPSetPositionPastDurationWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionPastDurationWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing is in progress to a value greater than the duration."));
	
	iExpState = EAPIdle;
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionPastDurationWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds duration = iUtility->Duration();
				
				TInt64 pos_val = duration.Int64();
				pos_val += 100;
				
				TTimeIntervalMicroSeconds position = pos_val;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);

				// Get the current head position
				err = iUtility->GetPosition(position);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Head position should be set to the end of the duration.				
				if (position == duration)
					{
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), I64INT(duration.Int64()));
					ERR_PRINTF2(_L("  Actual: %d"), I64INT(position.Int64()));
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionPastDurationWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current head position while playing has been paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0049, MM-MMF-APLCLNT-CHRTZ-OGG-I-0049
MM-MMF-APLCLNT-CHRTZ-AMR-I-0049, MM-MMF-APLCLNT-CHRTZ-MP3-I-0049, MM-MMF-APLCLNT-CHRTZ-AAC-I-0049
*******************************************************************************************************/

RTestStepAPSetPositionWhileClipPaused* RTestStepAPSetPositionWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionWhileClipPaused* self = new (ELeave) RTestStepAPSetPositionWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionWhileClipPaused::RTestStepAPSetPositionWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing has been paused."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call play
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds position = 100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
			   
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Head position should be as set to specified position
				if (TimeComparison(actualPosition.Int64(), position.Int64(), KExpectedDeviation))
					{
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(actualPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), &position);
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current head position while playing has been paused to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0050, MM-MMF-APLCLNT-CHRTZ-OGG-I-0050
MM-MMF-APLCLNT-CHRTZ-AMR-I-0050, MM-MMF-APLCLNT-CHRTZ-MP3-I-0050, MM-MMF-APLCLNT-CHRTZ-AAC-I-0050
*******************************************************************************************************/

RTestStepAPSetPositionNegativeWhileClipPaused* RTestStepAPSetPositionNegativeWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionNegativeWhileClipPaused* self = new (ELeave) RTestStepAPSetPositionNegativeWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionNegativeWhileClipPaused::RTestStepAPSetPositionNegativeWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionNegativeWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing has been paused to a negative value."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionNegativeWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call play
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds position = -100;
				TTimeIntervalMicroSeconds actualPosition = 0;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
							  
				// Get the current head position
				err = iUtility->GetPosition(actualPosition);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (TimeComparison(actualPosition.Int64(), 0, KExpectedDeviation))
					{
					// Head position should be start
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(actualPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF1(_L("  Expected: 0"));
					ERR_PRINTF2(_L("  Actual: %d"), &actualPosition);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionNegativeWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current head position while playing has been paused to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0051, MM-MMF-APLCLNT-CHRTZ-OGG-I-0051
MM-MMF-APLCLNT-CHRTZ-AMR-I-0051, MM-MMF-APLCLNT-CHRTZ-MP3-I-0051, MM-MMF-APLCLNT-CHRTZ-AAC-I-0051
*******************************************************************************************************/

RTestStepAPSetPositionPastDurationWhileClipPaused* RTestStepAPSetPositionPastDurationWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPositionPastDurationWhileClipPaused* self = new (ELeave) RTestStepAPSetPositionPastDurationWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPositionPastDurationWhileClipPaused::RTestStepAPSetPositionPastDurationWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPositionPastDurationWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.14 - Set the current head position while playing has been paused to a value greater than the duration."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPositionPastDurationWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call play
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds duration = iUtility->Duration();
				
				TInt64 pos_val = duration.Int64();
				pos_val += 100;
				
				TTimeIntervalMicroSeconds position = pos_val;
				
				// Set the current head position
				INFO_PRINTF1(_L("iUtility->SetPosition()"));
				iUtility->SetPosition(position);
				
				// Get the current head position
				err = iUtility->GetPosition(position);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Head position should be set to the end of the duration.				
				if (position == duration)
					{
					INFO_PRINTF2(_L("Head position set successful, head position: %d"), I64INT(position.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Position not set correctly"));
					ERR_PRINTF2(_L("  Expected: %d"), &duration);
					ERR_PRINTF2(_L("  Actual: %d"), &position);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPositionPastDurationWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback window.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0052, MM-MMF-APLCLNT-CHRTZ-OGG-I-0052
MM-MMF-APLCLNT-CHRTZ-AMR-I-0052, MM-MMF-APLCLNT-CHRTZ-MP3-I-0052, MM-MMF-APLCLNT-CHRTZ-AAC-I-0052
*******************************************************************************************************/

RTestStepAPSetPlayWindow* RTestStepAPSetPlayWindow::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindow* self = new (ELeave) RTestStepAPSetPlayWindow(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindow::RTestStepAPSetPlayWindow(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindow::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindow::Fsm()
	{
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds start = 0;
	TTimeIntervalMicroSeconds end = 4000000;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
				// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Start of play
				iStart.HomeTime();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
		  		}
			case EAPPlayComplete:
				{
				TTimeIntervalMicroSeconds position = 0;
				
				// End of play
				iStop.HomeTime();
				
				TUint actualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				if (TimeComparison(actualDuration, end.Int64(), KExpectedDeviation))
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window set successful, duration: %d"), actualDuration);
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected duration: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual duration: %d"), actualDuration);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the current playback window while playing is in progress and the start is after the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0053, MM-MMF-APLCLNT-CHRTZ-OGG-I-0053
MM-MMF-APLCLNT-CHRTZ-AMR-I-0053, MM-MMF-APLCLNT-CHRTZ-MP3-I-0053, MM-MMF-APLCLNT-CHRTZ-AAC-I-0053
*******************************************************************************************************/

const TInt KOggPageTimeDuration = 600000;

RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip* RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip* self = new (ELeave) RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is in progress and the start is after the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL(%S)"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds oldPosition = 0;
				
				err = iUtility->GetPosition(oldPosition);
				INFO_PRINTF2(_L("iUtility->GetPosition() returned %d"), I64INT(oldPosition.Int64()));
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				TInt64 startValue = I64INT(oldPosition.Int64()) + 2000000;		   	
				TInt64 endValue = I64INT(oldPosition.Int64()) + 4000000;
				
				iStart = TTimeIntervalMicroSeconds(startValue);
				iEnd = TTimeIntervalMicroSeconds(endValue);
					
				// Set the playback window
				INFO_PRINTF3(_L("iUtility->SetPlayWindow(%d, %d)"), I64INT(iStart.Int64()), I64INT(iEnd.Int64()));
				err = iUtility->SetPlayWindow(iStart, iEnd);

				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Test position on subsequent playback
				INFO_PRINTF1(_L("iUtility->Stop()"));
				iUtility->Stop();
				
				User::After(1000000);
				
				// Play again, to check if play window has been set ?
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				iState = EAPPlayingAgain;
				SetExpectedState(EAPPlayingAgain, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlayingAgain:
				{
				TTimeIntervalMicroSeconds newPosition = 0;
				
				err = iUtility->GetPosition(newPosition);
				INFO_PRINTF2(_L("iUtility->GetPosition() returned %d"), I64INT(newPosition.Int64()));
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				// OggSeeking logic may set the window to start of a oggpage.
				// Taking into consideration the time equivalent of 1 ogg page in microseconds when checking position.
				if (newPosition.Int64()+KOggPageTimeDuration >= iStart.Int64())	
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window set successful, position : %d"), I64INT(newPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), iStart.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), newPosition.Int64());
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback window while playing is in progress and the end is before the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0054, MM-MMF-APLCLNT-CHRTZ-OGG-I-0054
MM-MMF-APLCLNT-CHRTZ-AMR-I-0054, MM-MMF-APLCLNT-CHRTZ-MP3-I-0054, MM-MMF-APLCLNT-CHRTZ-AAC-I-0054
*******************************************************************************************************/

RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip* RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip* self = new (ELeave) RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is in progress and the end is before the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KLongDelay, KLongDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds oldPosition = 0;
				
				err = iUtility->GetPosition(oldPosition);
 
				if (err != KErrNone)
					{
					StopTest(err, EFail);
					break;
					}
					
				TInt64 endValue = oldPosition.Int64() - 4000000;		   			   	
				TTimeIntervalMicroSeconds start = 0;
				TTimeIntervalMicroSeconds end = endValue;
					
				// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);

				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Test position on subsequent playback					
				iUtility->Stop();
				iUtility->Play();
				
				TTimeIntervalMicroSeconds newPosition = 0;
				
				err = iUtility->GetPosition(newPosition);
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (newPosition.Int64() <= end.Int64())
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window set successful, position: %d"), I64INT(newPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), newPosition.Int64());
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback window while playing is in progress and the start is greater than the end.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0055, MM-MMF-APLCLNT-CHRTZ-OGG-I-0055
MM-MMF-APLCLNT-CHRTZ-AMR-I-0055, MM-MMF-APLCLNT-CHRTZ-MP3-I-0055, MM-MMF-APLCLNT-CHRTZ-AAC-I-0055
*******************************************************************************************************/

RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip* RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip* self = new (ELeave) RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is in progress and the start is greater than the end."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				TTimeIntervalMicroSeconds start = iUtility->Duration();
				TTimeIntervalMicroSeconds end = 0;
					
				// Set the playback window, should swap them round.
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);

 				// Expect KErrArgument to be returned
				if (err != KErrArgument)
					{
					ERR_PRINTF2(_L("ERROR: Expect error code KErrArgument, Actual error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("Received expected error code: KErrArgument"));
					StopTest(err, EPass);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback window while playing is paused and the start is after the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0056, MM-MMF-APLCLNT-CHRTZ-OGG-I-0056
MM-MMF-APLCLNT-CHRTZ-AMR-I-0056, MM-MMF-APLCLNT-CHRTZ-MP3-I-0056, MM-MMF-APLCLNT-CHRTZ-AAC-I-0056
*******************************************************************************************************/

RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused* RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused* self = new (ELeave) RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is paused and the start is after the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds oldPosition = 0;
				
				err = iUtility->GetPosition(oldPosition);
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				TInt64 startValue = oldPosition.Int64() + 2000000;		   	
				TInt64 endValue = startValue + 2000000;
				
				TTimeIntervalMicroSeconds start = startValue;
				TTimeIntervalMicroSeconds end = endValue;
					
				// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);

				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				// Test position on subsequent playback					
				iUtility->Stop();
				iUtility->Play();
				
				TTimeIntervalMicroSeconds newPosition = 0;
				
				err = iUtility->GetPosition(newPosition);
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				//In case of oggvorbis, the actual position set in the controller can be less than the position set by the client.
				//So the current position can be less than the play window start but it should be under the expected deviation.
				if (newPosition.Int64() >= start.Int64() || start.Int64() - newPosition.Int64() < KExpectedDeviation)
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window set successful, position: %d"), I64INT(newPosition.Int64()));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), start.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), newPosition.Int64());
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback window while playing is paused and the end is before the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0057, MM-MMF-APLCLNT-CHRTZ-OGG-I-0057
MM-MMF-APLCLNT-CHRTZ-AMR-I-0057, MM-MMF-APLCLNT-CHRTZ-MP3-I-0057, MM-MMF-APLCLNT-CHRTZ-AAC-I-0057
*******************************************************************************************************/

RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused* RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused* self = new (ELeave) RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is paused and the end is before the current head position."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KLongDelay, KLongDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds oldPosition = 0;
				
				err = iUtility->GetPosition(oldPosition);
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				TInt64 endValue = oldPosition.Int64() - 4000000;
				TTimeIntervalMicroSeconds start = 0;
				TTimeIntervalMicroSeconds end = endValue;
					
				// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);

				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Test position on subsequent playback					
				iUtility->Stop();
				iUtility->Play();
				
				TTimeIntervalMicroSeconds newPosition = 0;
				
				err = iUtility->GetPosition(newPosition);
 
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (newPosition.Int64() <= end.Int64())
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window set successful, position: %d"), I64INT(newPosition.Int64()));
					StopTest(err);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), newPosition.Int64());
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback window while playing is paused and the start is greater than the end.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0058, MM-MMF-APLCLNT-CHRTZ-OGG-I-0058
MM-MMF-APLCLNT-CHRTZ-AMR-I-0058, MM-MMF-APLCLNT-CHRTZ-MP3-I-0058, MM-MMF-APLCLNT-CHRTZ-AAC-I-0058
*******************************************************************************************************/

RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused* RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused* self = new (ELeave) RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.15 - Set the current playback window while playing is paused and the start is greater than the end."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
						   	
				TTimeIntervalMicroSeconds start = iUtility->Duration();
				TTimeIntervalMicroSeconds end = 0;
					
				// Set the playback window, should swap them round.
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);

				// Expect KErrArgument to be returned
				if (err != KErrArgument)
					{
					ERR_PRINTF2(_L("ERROR: Expect error code KErrArgument, Actual error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF1(_L("Received expected error code: KErrArgument"));
					StopTest(err, EPass);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Clear the current playback window.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0059, MM-MMF-APLCLNT-CHRTZ-OGG-I-0059
MM-MMF-APLCLNT-CHRTZ-AMR-I-0059, MM-MMF-APLCLNT-CHRTZ-MP3-I-0059, MM-MMF-APLCLNT-CHRTZ-AAC-I-0059
*******************************************************************************************************/

RTestStepAPClearPlayWindow* RTestStepAPClearPlayWindow::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPClearPlayWindow* self = new (ELeave) RTestStepAPClearPlayWindow(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPClearPlayWindow::RTestStepAPClearPlayWindow(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPClearPlayWindow::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.16 - Clear the current playback window."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPClearPlayWindow::Fsm()
	{
	TInt err = KErrNone;
	
	TTimeIntervalMicroSeconds start = 2000000;
	TTimeIntervalMicroSeconds end = 4000000;
	TTimeIntervalMicroSeconds expected_position = 0;
	TTimeIntervalMicroSeconds position = 0;
	TTimeIntervalMicroSeconds expected_duration = iUtility->Duration();

	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);
				
				// Clear the playback window
				INFO_PRINTF1(_L("iUtility->ClearPlayWindow()"));
				err = iUtility->ClearPlayWindow();
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Start of play
				iStart.HomeTime();
				
				err = iUtility->GetPosition(position);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (!(position >= expected_position && position < start))
					{
					// Playback window not cleared correctly
					ERR_PRINTF1(_L("ERROR: Playback window not cleared correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), expected_position.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), position.Int64());
					
					StopTest(err, EPass);
					break;
					}
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
		  		}
			case EAPPlayComplete:
				{
				// End of play
				iStop.HomeTime();
				
				TUint actualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				if (TimeComparison(actualDuration, expected_duration.Int64(), KExpectedDeviation)) 
					{
					// Playback window set correctly
					INFO_PRINTF2(_L("Playback window cleared successful, duration: %d"), actualDuration);
					StopTest(err);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected duration: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual duration: %d"), actualDuration);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Clear the current playback window while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0060, MM-MMF-APLCLNT-CHRTZ-OGG-I-0060
MM-MMF-APLCLNT-CHRTZ-AMR-I-0060, MM-MMF-APLCLNT-CHRTZ-MP3-I-0060, MM-MMF-APLCLNT-CHRTZ-AAC-I-0060
*******************************************************************************************************/

RTestStepAPClearPlayWindowWhilePlayingClip* RTestStepAPClearPlayWindowWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPClearPlayWindowWhilePlayingClip* self = new (ELeave) RTestStepAPClearPlayWindowWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPClearPlayWindowWhilePlayingClip::RTestStepAPClearPlayWindowWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPClearPlayWindowWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.16 - Clear the current playback window while playing is in progress."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPClearPlayWindowWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	TTimeIntervalMicroSeconds start = 2000000;
	TTimeIntervalMicroSeconds end = 4000000;
	TTimeIntervalMicroSeconds expected_position = 0;
	TTimeIntervalMicroSeconds position = 0;
	TTimeIntervalMicroSeconds expected_duration = iUtility->Duration();
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					}
				
				// Set state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{
		  		// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);
				
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
		  		}
			case EAPPlaying:
				{			
				// Clear the playback window
				INFO_PRINTF1(_L("iUtility->ClearPlayWindow()"));
				err = iUtility->ClearPlayWindow();
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				 // Stop
				INFO_PRINTF1(_L("iUtility->Stop()"));
				iUtility->Stop();
				
				// Play
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Start of play
				iStart.HomeTime();
				
				err = iUtility->GetPosition(position);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				if (!(position >= expected_position && position < start))
					{
					// Playback window not cleared correctly
					ERR_PRINTF1(_L("ERROR: Playback window not cleared correctly"));
					ERR_PRINTF2(_L("  Expected position: %d"), expected_position.Int64());
					ERR_PRINTF2(_L("  Actual position: %d"), position.Int64());
					
					StopTest(err, EFail);
					break;
					}
				
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
				}
			case EAPPlayComplete:
				{
				// End of play
				iStop.HomeTime();
				
				TUint actualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				if (TimeComparison(actualDuration, expected_duration.Int64(), KExpectedDeviation)) 
					{
					// Playback window cleared correctly
					INFO_PRINTF2(_L("Playback window cleared successful, duration: %d"), actualDuration);
					StopTest(err);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected duration: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual duration: %d"), actualDuration);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPClearPlayWindowWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Clear the current playback window before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0061
*******************************************************************************************************/

RTestStepAPClearPlayWindowBeforeOpenClip* RTestStepAPClearPlayWindowBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPClearPlayWindowBeforeOpenClip* self = new (ELeave) RTestStepAPClearPlayWindowBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPClearPlayWindowBeforeOpenClip::RTestStepAPClearPlayWindowBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPClearPlayWindowBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Clear the current playback window before opening the audio clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPClearPlayWindowBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	TTimeIntervalMicroSeconds start = 2000000;
	TTimeIntervalMicroSeconds end = 4000000;
	TTimeIntervalMicroSeconds expected_position = 0;
	TTimeIntervalMicroSeconds position = 0;
	TTimeIntervalMicroSeconds expected_duration = iUtility->Duration();
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:		  	
				{
				// Set the playback window
				INFO_PRINTF1(_L("iUtility->SetPlayWindow()"));
				err = iUtility->SetPlayWindow(start, end);
				
				// Clear the current playback window
				INFO_PRINTF1(_L("iUtility->ClearPlayWindow()"));
				err = iUtility->ClearPlayWindow();

				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					}
				
				// Set state
				iState = EAPOpening;
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
		  		{			
		  		// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Start of play
				iStart.HomeTime();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
				}
			case EAPPlayComplete:
				{
				// End of play
				iStop.HomeTime();
				
				TUint actualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				if (TimeComparison(actualDuration, expected_duration.Int64(), KExpectedDeviation)) 
					{
					// Playback window cleared correctly
					INFO_PRINTF2(_L("Playback window cleared successful, duration: %d"), actualDuration);
					StopTest(err);
					break;
					}
				else
					{
					ERR_PRINTF1(_L("ERROR: Playback window not set correctly"));
					ERR_PRINTF2(_L("  Expected duration: %d"), end.Int64());
					ERR_PRINTF2(_L("  Actual duration: %d"), actualDuration);
					
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current playback balance.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0062, MM-MMF-APLCLNT-CHRTZ-OGG-I-0062
MM-MMF-APLCLNT-CHRTZ-AMR-I-0062, MM-MMF-APLCLNT-CHRTZ-MP3-I-0062, MM-MMF-APLCLNT-CHRTZ-AAC-I-0062
*******************************************************************************************************/

RTestStepAPSetBalance* RTestStepAPSetBalance::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalance* self = new (ELeave) RTestStepAPSetBalance(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalance::RTestStepAPSetBalance(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalance::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalance::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt balance = 80;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (balance != actualBalance)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), balance);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0063, MM-MMF-APLCLNT-CHRTZ-OGG-I-0063
MM-MMF-APLCLNT-CHRTZ-AMR-I-0063, MM-MMF-APLCLNT-CHRTZ-MP3-I-0063, MM-MMF-APLCLNT-CHRTZ-AAC-I-0063
*******************************************************************************************************/

RTestStepAPSetBalanceLessMaxLeft* RTestStepAPSetBalanceLessMaxLeft::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceLessMaxLeft* self = new (ELeave) RTestStepAPSetBalanceLessMaxLeft(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceLessMaxLeft::RTestStepAPSetBalanceLessMaxLeft(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceLessMaxLeft::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to less than KMMFBalanceMaxLeft."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceLessMaxLeft::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt balance = KMMFBalanceMaxLeft - 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxLeft)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxLeft);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0064, MM-MMF-APLCLNT-CHRTZ-OGG-I-0064
MM-MMF-APLCLNT-CHRTZ-AMR-I-0064, MM-MMF-APLCLNT-CHRTZ-MP3-I-0064, MM-MMF-APLCLNT-CHRTZ-AAC-I-0064
*******************************************************************************************************/

RTestStepAPSetBalanceGreaterMaxRight* RTestStepAPSetBalanceGreaterMaxRight::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceGreaterMaxRight* self = new (ELeave) RTestStepAPSetBalanceGreaterMaxRight(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceGreaterMaxRight::RTestStepAPSetBalanceGreaterMaxRight(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceGreaterMaxRight::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to greater than KMMFBalanceMaxRight."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceGreaterMaxRight::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt balance = KMMFBalanceMaxRight + 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxRight)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxRight);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Set the current playback balance while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0065, MM-MMF-APLCLNT-CHRTZ-OGG-I-0065
MM-MMF-APLCLNT-CHRTZ-AMR-I-0065, MM-MMF-APLCLNT-CHRTZ-MP3-I-0065, MM-MMF-APLCLNT-CHRTZ-AAC-I-0065
*******************************************************************************************************/

RTestStepAPSetBalanceWhilePlayingClip* RTestStepAPSetBalanceWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceWhilePlayingClip* self = new (ELeave) RTestStepAPSetBalanceWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceWhilePlayingClip::RTestStepAPSetBalanceWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance while playing is in progress."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt balance = 80;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (balance != actualBalance)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), &balance);
					ERR_PRINTF2(_L("  Actual balance: %d"), &actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0066, MM-MMF-APLCLNT-CHRTZ-OGG-I-0066
MM-MMF-APLCLNT-CHRTZ-AMR-I-0066, MM-MMF-APLCLNT-CHRTZ-MP3-I-0066, MM-MMF-APLCLNT-CHRTZ-AAC-I-0066
*******************************************************************************************************/

RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip* RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip* self = new (ELeave) RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to less than KMMFBalanceMaxLeft while playing is in progress."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt balance = KMMFBalanceMaxLeft - 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxLeft)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxLeft);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
 
// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
		
/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0067, MM-MMF-APLCLNT-CHRTZ-OGG-I-0067
MM-MMF-APLCLNT-CHRTZ-AMR-I-0067, MM-MMF-APLCLNT-CHRTZ-MP3-I-0067, MM-MMF-APLCLNT-CHRTZ-AAC-I-0067
*******************************************************************************************************/

RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip* RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip* self = new (ELeave) RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to greater than KMMFBalanceMaxRight while playing is in progress."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt balance = KMMFBalanceMaxRight + 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxRight)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxRight);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Set the current playback balance while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0068, MM-MMF-APLCLNT-CHRTZ-OGG-I-0068
MM-MMF-APLCLNT-CHRTZ-AMR-I-0068, MM-MMF-APLCLNT-CHRTZ-MP3-I-0068, MM-MMF-APLCLNT-CHRTZ-AAC-I-0068
*******************************************************************************************************/

RTestStepAPSetBalanceWhileClipPaused* RTestStepAPSetBalanceWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceWhileClipPaused* self = new (ELeave) RTestStepAPSetBalanceWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceWhileClipPaused::RTestStepAPSetBalanceWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance while playing is paused."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TInt balance = 80;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (balance != actualBalance)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), balance);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0069, MM-MMF-APLCLNT-CHRTZ-OGG-I-0069
MM-MMF-APLCLNT-CHRTZ-AMR-I-0069, MM-MMF-APLCLNT-CHRTZ-MP3-I-0069, MM-MMF-APLCLNT-CHRTZ-AAC-I-0069
*******************************************************************************************************/

RTestStepAPSetBalanceLessMaxLeftWhileClipPaused* RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceLessMaxLeftWhileClipPaused* self = new (ELeave) RTestStepAPSetBalanceLessMaxLeftWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::RTestStepAPSetBalanceLessMaxLeftWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to less than KMMFBalanceMaxLeft while playing is paused."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TInt balance = KMMFBalanceMaxLeft - 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxLeft)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxLeft);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
 
// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceLessMaxLeftWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
		
/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0070, MM-MMF-APLCLNT-CHRTZ-OGG-I-0070
MM-MMF-APLCLNT-CHRTZ-AMR-I-0070, MM-MMF-APLCLNT-CHRTZ-MP3-I-0070, MM-MMF-APLCLNT-CHRTZ-AAC-I-0070
*******************************************************************************************************/

RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused* RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused* self = new (ELeave) RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance to greater than KMMFBalanceMaxRight while playing is paused."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TInt balance = KMMFBalanceMaxRight + 20;
				TInt actualBalance = 0;
				
				// Set the playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance(balance);
							  
				if (err != KErrNone)
					{			  	
					StopTest(err);
					break;
					}
				
				err = iUtility->GetBalance(actualBalance);
				
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				if (actualBalance != KMMFBalanceMaxRight)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: Playback balance was not set correctly"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceMaxRight);
					ERR_PRINTF2(_L("  Actual balance: %d"), actualBalance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Playback balance set successful, balance: %d"), actualBalance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Set the current playback balance before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0071
*******************************************************************************************************/

RTestStepAPSetBalanceBeforeOpenClip* RTestStepAPSetBalanceBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPSetBalanceBeforeOpenClip* self = new (ELeave) RTestStepAPSetBalanceBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPSetBalanceBeforeOpenClip::RTestStepAPSetBalanceBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPSetBalanceBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.17 - Set the current playback balance before opening the clip."));
	}

//State Machine Function
void RTestStepAPSetBalanceBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:		  	
				{
				// Set the current playback balance
				INFO_PRINTF1(_L("iUtility->SetBalance()"));
				err = iUtility->SetBalance();
							  
				if (err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Get the current playback balance.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0072, MM-MMF-APLCLNT-CHRTZ-OGG-I-0072
MM-MMF-APLCLNT-CHRTZ-AMR-I-0072, MM-MMF-APLCLNT-CHRTZ-MP3-I-0072, MM-MMF-APLCLNT-CHRTZ-AAC-I-0072
*******************************************************************************************************/

RTestStepAPGetBalance* RTestStepAPGetBalance::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBalance* self = new (ELeave) RTestStepAPGetBalance(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBalance::RTestStepAPGetBalance(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBalance::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.18 - Get the current playback balance."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetBalance::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt balance = 50;
				
				// Get the current playback balance
				INFO_PRINTF1(_L("iUtility->GetBalance()"));
				err = iUtility->GetBalance(balance);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Defect value for balance is KMMFBalanceCenter
				if (balance != KMMFBalanceCenter)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: GetBalance() did not return default value"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceCenter);
					ERR_PRINTF2(_L("  Actual balance: %d"), balance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Get playback balance successful, balance: %d"), balance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the current playback balance while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0073, MM-MMF-APLCLNT-CHRTZ-OGG-I-0073
MM-MMF-APLCLNT-CHRTZ-AMR-I-0073, MM-MMF-APLCLNT-CHRTZ-MP3-I-0073, MM-MMF-APLCLNT-CHRTZ-AAC-I-0073
*******************************************************************************************************/

RTestStepAPGetBalanceWhilePlayingClip* RTestStepAPGetBalanceWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBalanceWhilePlayingClip* self = new (ELeave) RTestStepAPGetBalanceWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBalanceWhilePlayingClip::RTestStepAPGetBalanceWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBalanceWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.18 - Get the current playback balance while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetBalanceWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt balance = 0;
				
				// Get the current playback balance
				INFO_PRINTF1(_L("iUtility->GetBalance()"));
				err = iUtility->GetBalance(balance);

				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
												  
				// Defect value for balance is KMMFBalanceCenter
				if (balance != KMMFBalanceCenter)
					{
					// Playback balance was not set correctly
					ERR_PRINTF1(_L("ERROR: GetBalance() did not return default value"));
					ERR_PRINTF2(_L("  Expected balance: %d"), KMMFBalanceCenter);
					ERR_PRINTF2(_L("  Actual balance: %d"), balance);
					
					StopTest(err, EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Get playback balance successful, balance: %d"), balance);
					StopTest(err);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetBalanceWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Get the current playback balance before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0074
*******************************************************************************************************/

RTestStepAPGetBalanceBeforeOpenClip* RTestStepAPGetBalanceBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBalanceBeforeOpenClip* self = new (ELeave) RTestStepAPGetBalanceBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBalanceBeforeOpenClip::RTestStepAPGetBalanceBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBalanceBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.18 - Get the current playback balance before opening the clip."));
	}

//State Machine Function
void RTestStepAPGetBalanceBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt BalanceValue = 0;
				
				// Get the current playback balance
				INFO_PRINTF1(_L("iUtility->GetBalance()"));
				err = iUtility->GetBalance(BalanceValue);
							  
				if (err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the bit rate of the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0075, MM-MMF-APLCLNT-CHRTZ-OGG-I-0075
MM-MMF-APLCLNT-CHRTZ-AMR-I-0075, MM-MMF-APLCLNT-CHRTZ-MP3-I-0075, MM-MMF-APLCLNT-CHRTZ-AAC-I-0075
*******************************************************************************************************/

RTestStepAPGetBitrate* RTestStepAPGetBitrate::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBitrate* self = new (ELeave) RTestStepAPGetBitrate(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBitrate::RTestStepAPGetBitrate(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBitrate::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.19 - Get the bit rate of the audio clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetBitrate::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TUint bitrate = 0;
				
				// Get the current playback balance
				INFO_PRINTF1(_L("iUtility->GetBitRate()"));
				err = iUtility->GetBitRate(bitrate);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				INFO_PRINTF2(_L("Get bitrate successful, bitrate: %d"), bitrate);
			   	StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Get the bit rate of the audio clip while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0076, MM-MMF-APLCLNT-CHRTZ-OGG-I-0076
MM-MMF-APLCLNT-CHRTZ-AMR-I-0076, MM-MMF-APLCLNT-CHRTZ-MP3-I-0076, MM-MMF-APLCLNT-CHRTZ-AAC-I-0076
*******************************************************************************************************/

RTestStepAPGetBitrateWhilePlayingClip* RTestStepAPGetBitrateWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBitrateWhilePlayingClip* self = new (ELeave) RTestStepAPGetBitrateWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBitrateWhilePlayingClip::RTestStepAPGetBitrateWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBitrateWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.19 - Get the bit rate of the audio clip while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetBitrateWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TUint bitrate = 0;
				
				// Get the current playback balance
				INFO_PRINTF1(_L("iUtility->GetBitRate()"));
				err = iUtility->GetBitRate(bitrate);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
				
				INFO_PRINTF2(_L("Get bitrate successful, bitrate: %d"), bitrate);
			   	StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetBitrateWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Get the bit rate of the audio clip before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0077
*******************************************************************************************************/

RTestStepAPGetBitrateBeforeOpenClip* RTestStepAPGetBitrateBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetBitrateBeforeOpenClip* self = new (ELeave) RTestStepAPGetBitrateBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetBitrateBeforeOpenClip::RTestStepAPGetBitrateBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetBitrateBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.19 - Get the bit rate of the audio clip before opening the clip."));
	}

//State Machine Function
void RTestStepAPGetBitrateBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TUint BitrateValue = 0;
				
				// Get the bitrate
				INFO_PRINTF1(_L("iUtility->GetBitRate()"));
				err = iUtility->GetBitRate(BitrateValue);
					   
				// Expected error code is KErrNotReady
				if (err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Register the event for notification.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0078, MM-MMF-APLCLNT-CHRTZ-OGG-I-0078
MM-MMF-APLCLNT-CHRTZ-AMR-I-0078, MM-MMF-APLCLNT-CHRTZ-MP3-I-0078, MM-MMF-APLCLNT-CHRTZ-AAC-I-0078
*******************************************************************************************************/

RTestStepAPRegisterEventNotification* RTestStepAPRegisterEventNotification::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPRegisterEventNotification* self = new (ELeave) RTestStepAPRegisterEventNotification(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPRegisterEventNotification::RTestStepAPRegisterEventNotification(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPRegisterEventNotification::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.20 - Register the event for notification."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPRegisterEventNotification::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Register the event for notification while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0079, MM-MMF-APLCLNT-CHRTZ-OGG-I-0079
MM-MMF-APLCLNT-CHRTZ-AMR-I-0079, MM-MMF-APLCLNT-CHRTZ-MP3-I-0079, MM-MMF-APLCLNT-CHRTZ-AAC-I-0079
*******************************************************************************************************/

RTestStepAPRegisterEventNotificationWhilePlayingClip* RTestStepAPRegisterEventNotificationWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPRegisterEventNotificationWhilePlayingClip* self = new (ELeave) RTestStepAPRegisterEventNotificationWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPRegisterEventNotificationWhilePlayingClip::RTestStepAPRegisterEventNotificationWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPRegisterEventNotificationWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.20 - Register the event for notification while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPRegisterEventNotificationWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPRegisterEventNotificationWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Register the event for notification before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0080, MM-MMF-APLCLNT-CHRTZ-OGG-I-0080
MM-MMF-APLCLNT-CHRTZ-AMR-I-0080, MM-MMF-APLCLNT-CHRTZ-MP3-I-0080, MM-MMF-APLCLNT-CHRTZ-AAC-I-0080
*******************************************************************************************************/

RTestStepAPRegisterEventNotificationBeforeOpenClip* RTestStepAPRegisterEventNotificationBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPRegisterEventNotificationBeforeOpenClip* self = new (ELeave) RTestStepAPRegisterEventNotificationBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPRegisterEventNotificationBeforeOpenClip::RTestStepAPRegisterEventNotificationBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPRegisterEventNotificationBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.20 - Register the event for notification before opening the audio clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPRegisterEventNotificationBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlayComplete, KErrNone);
				break;
				}
			case EAPPlayComplete:
				{
				// Test Finished
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Register the event for notification with an event Uid that is not supported.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0082, MM-MMF-APLCLNT-CHRTZ-OGG-I-0082
MM-MMF-APLCLNT-CHRTZ-AMR-I-0082, MM-MMF-APLCLNT-CHRTZ-MP3-I-0082, MM-MMF-APLCLNT-CHRTZ-AAC-I-0082
*******************************************************************************************************/

RTestStepAPRegisterEventNotificationUidNotSupported* RTestStepAPRegisterEventNotificationUidNotSupported::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPRegisterEventNotificationUidNotSupported* self = new (ELeave) RTestStepAPRegisterEventNotificationUidNotSupported(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPRegisterEventNotificationUidNotSupported::RTestStepAPRegisterEventNotificationUidNotSupported(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPRegisterEventNotificationUidNotSupported::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.20 - Register the event for notification with an event Uid that is not supported."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPRegisterEventNotificationUidNotSupported::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TUid uidNotSuported;
				uidNotSuported.iUid = KNullUidValue;
				
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, uidNotSuported);
							  
				if (err != KErrNotSupported)
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotSupported, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF1(_L("Received expected error code KErrNotSupported"));
					StopTest(err, EPass);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Register the event for notification with an event Uid that is already registered.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0083, MM-MMF-APLCLNT-CHRTZ-OGG-I-0083
MM-MMF-APLCLNT-CHRTZ-AMR-I-0083, MM-MMF-APLCLNT-CHRTZ-MP3-I-0083, MM-MMF-APLCLNT-CHRTZ-AAC-I-0083
*******************************************************************************************************/

RTestStepAPRegisterEventNotificationUidAlreadyRegistered* RTestStepAPRegisterEventNotificationUidAlreadyRegistered::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPRegisterEventNotificationUidAlreadyRegistered* self = new (ELeave) RTestStepAPRegisterEventNotificationUidAlreadyRegistered(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPRegisterEventNotificationUidAlreadyRegistered::RTestStepAPRegisterEventNotificationUidAlreadyRegistered(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPRegisterEventNotificationUidAlreadyRegistered::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.20 - Register the event for notification with an event Uid that is already registered."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPRegisterEventNotificationUidAlreadyRegistered::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}

				// Try to register for the same event.  This should fail.					
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err == KErrAlreadyExists)
					{
					INFO_PRINTF1(_L("Received expected error code KErrAlreadyExists"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrAlreadyExists, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}										
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Cancel the registered notification event.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0084, MM-MMF-APLCLNT-CHRTZ-OGG-I-0084
MM-MMF-APLCLNT-CHRTZ-AMR-I-0084, MM-MMF-APLCLNT-CHRTZ-MP3-I-0084, MM-MMF-APLCLNT-CHRTZ-AAC-I-0084
*******************************************************************************************************/

RTestStepAPCancelRegisteredNotificationEvent* RTestStepAPCancelRegisteredNotificationEvent::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPCancelRegisteredNotificationEvent* self = new (ELeave) RTestStepAPCancelRegisteredNotificationEvent(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPCancelRegisteredNotificationEvent::RTestStepAPCancelRegisteredNotificationEvent(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPCancelRegisteredNotificationEvent::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.21 - Cancel the registered notification event."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPCancelRegisteredNotificationEvent::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Cancel the registration
				INFO_PRINTF1(_L("iUtility->CancelRegisterAudioResourceNotification()"));
				err = iUtility->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
							  
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Cancel the registered notification event while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0085, MM-MMF-APLCLNT-CHRTZ-OGG-I-0085
MM-MMF-APLCLNT-CHRTZ-AMR-I-0085, MM-MMF-APLCLNT-CHRTZ-MP3-I-0085, MM-MMF-APLCLNT-CHRTZ-AAC-I-0085
*******************************************************************************************************/

RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip* RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip* self = new (ELeave) RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.21 - Cancel the registered notification event while playing an audio file."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Cancel the registration
				INFO_PRINTF1(_L("iUtility->CancelRegisterAudioResourceNotification()"));
				err = iUtility->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
							  
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}

/******************************************************************************************************
Cancel the registered notification event with an event Uid that is not matched.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0086, MM-MMF-APLCLNT-CHRTZ-OGG-I-0086
MM-MMF-APLCLNT-CHRTZ-AMR-I-0086, MM-MMF-APLCLNT-CHRTZ-MP3-I-0086, MM-MMF-APLCLNT-CHRTZ-AAC-I-0086
*******************************************************************************************************/

RTestStepAPCancelRegisteredNotificationEventUidNotMatched* RTestStepAPCancelRegisteredNotificationEventUidNotMatched::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPCancelRegisteredNotificationEventUidNotMatched* self = new (ELeave) RTestStepAPCancelRegisteredNotificationEventUidNotMatched(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPCancelRegisteredNotificationEventUidNotMatched::RTestStepAPCancelRegisteredNotificationEventUidNotMatched(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPCancelRegisteredNotificationEventUidNotMatched::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.21 - Cancel the registered notification event with an event Uid that is not matched."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPCancelRegisteredNotificationEventUidNotMatched::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Cancel the registration
				INFO_PRINTF1(_L("iUtility->CancelRegisterAudioResourceNotification()"));
				err = iUtility->CancelRegisterAudioResourceNotification(KUidInterfaceMMFAudioPlayDevice);
							  
				if (err == KErrNotSupported)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotSupported"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotSupported, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

/******************************************************************************************************
Cancel the registered notification event with an event Uid of NULL.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0087, MM-MMF-APLCLNT-CHRTZ-OGG-I-0087
MM-MMF-APLCLNT-CHRTZ-AMR-I-0087, MM-MMF-APLCLNT-CHRTZ-MP3-I-0087, MM-MMF-APLCLNT-CHRTZ-AAC-I-0087
*******************************************************************************************************/

RTestStepAPCancelRegisteredNotificationEventUidNULL* RTestStepAPCancelRegisteredNotificationEventUidNULL::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPCancelRegisteredNotificationEventUidNULL* self = new (ELeave) RTestStepAPCancelRegisteredNotificationEventUidNULL(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPCancelRegisteredNotificationEventUidNULL::RTestStepAPCancelRegisteredNotificationEventUidNULL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPCancelRegisteredNotificationEventUidNULL::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.21 - Cancel the registered notification event with an event Uid of NULL."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPCancelRegisteredNotificationEventUidNULL::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Register event for notification
				INFO_PRINTF1(_L("iUtility->RegisterAudioResourceNotification()"));
				err = iUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable);
							  
				if (err != KErrNone)
					{
					StopTest(err);
					break;
					}
					
				// Cancel the registration
				TUid NULLUid;
				NULLUid.iUid = KNullUidValue;
				
				INFO_PRINTF1(_L("iUtility->CancelRegisterAudioResourceNotification()"));
				err = iUtility->CancelRegisterAudioResourceNotification(NULLUid);
							  
				if (err == KErrNotSupported)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotSupported"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotSupported, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}   

/******************************************************************************************************
Try to get the number of meta data entries.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0089, MM-MMF-APLCLNT-CHRTZ-OGG-I-0089
MM-MMF-APLCLNT-CHRTZ-AMR-I-0089, MM-MMF-APLCLNT-CHRTZ-MP3-I-0089, MM-MMF-APLCLNT-CHRTZ-AAC-I-0089
*******************************************************************************************************/

RTestStepAPGetNumberOfMetaDataEntries* RTestStepAPGetNumberOfMetaDataEntries::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError)
	{
	RTestStepAPGetNumberOfMetaDataEntries* self = new (ELeave) RTestStepAPGetNumberOfMetaDataEntries(aStepName, aSectName, aKeyName, aTestExpError);
	return self;
	}
	 
// Private constructor
RTestStepAPGetNumberOfMetaDataEntries::RTestStepAPGetNumberOfMetaDataEntries(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	iTestExpError = aTestExpError;
	}

// Start test
void RTestStepAPGetNumberOfMetaDataEntries::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Try to get the number of meta data entries."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetNumberOfMetaDataEntries::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);	
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				TInt numMetaDataEntries = 0;
				
				INFO_PRINTF1(_L("iUtility->GetnumMetaDataEntries()"));
				err = iUtility->GetNumberOfMetaDataEntries(numMetaDataEntries);
				
				// Not all controllers support metadata so iTestExpError is checked to determine expected behaviour.
				if(err == iTestExpError)
					{
					if (iTestExpError != KErrNone)
						{
						INFO_PRINTF2(_L("Received expected error code: %d"), iTestExpError);
						StopTest(err, EKnownFailure);
						break;
						}
					else
						{
						INFO_PRINTF2(_L("Get number of metadata entries sucessful, number of metadata entries: %d"), numMetaDataEntries);
						StopTest(err);
						break;
						}
					}
				else
					{
					ERR_PRINTF3(_L("ERROR: Expected error code: %d, Actual Error code: %d"), iTestExpError, err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Try to get a meta data entry.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0090, MM-MMF-APLCLNT-CHRTZ-OGG-I-0090
MM-MMF-APLCLNT-CHRTZ-AMR-I-0090, MM-MMF-APLCLNT-CHRTZ-MP3-I-0090, MM-MMF-APLCLNT-CHRTZ-AAC-I-0090
*******************************************************************************************************/

RTestStepAPGetMetaDataEntry* RTestStepAPGetMetaDataEntry::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError)
	{
	RTestStepAPGetMetaDataEntry* self = new (ELeave) RTestStepAPGetMetaDataEntry(aStepName, aSectName, aKeyName, aTestExpError);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMetaDataEntry::RTestStepAPGetMetaDataEntry(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	iTestExpError = aTestExpError;
	}

// Start test
void RTestStepAPGetMetaDataEntry::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Try to get a meta data entry."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetMetaDataEntry::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:		  	
				{
				CMMFMetaDataEntry* metaEntry = NULL;
				
				TRAP(err, metaEntry = iUtility->GetMetaDataEntryL(0));
				CleanupStack::PushL(metaEntry);
				
				// Not all controllers support metadata so iTestExpError is checked to determine expected behaviour.
				if(err == iTestExpError)
					{
					if (iTestExpError != KErrNone)
						{
						INFO_PRINTF2(_L("Received expected error code: %d"), iTestExpError);
						CleanupStack::PopAndDestroy(metaEntry);
						StopTest(err, EKnownFailure);
						break;
						}
					else
						{
						TPtrC entryName = metaEntry->Name();
						TPtrC entryValue = metaEntry->Value();
						INFO_PRINTF3(_L("Get metadata entry sucessful, metadata entry (Name:Value): %S:%S"), &entryName, &entryValue);
						CleanupStack::PopAndDestroy(metaEntry);
						StopTest(err);
						break;
						}
					}
				else
					{
					ERR_PRINTF3(_L("ERROR: Expected error code: %d, Actual Error code: %d"), iTestExpError, err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Try to get the number of meta data entries while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0091, MM-MMF-APLCLNT-CHRTZ-OGG-I-0091
MM-MMF-APLCLNT-CHRTZ-AMR-I-0091, MM-MMF-APLCLNT-CHRTZ-MP3-I-0091, MM-MMF-APLCLNT-CHRTZ-AAC-I-0091
*******************************************************************************************************/

RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying* RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError)
	{
	RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying* self = new (ELeave) RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying(aStepName, aSectName, aKeyName, aTestExpError);
	return self;
	}
	 
// Private constructor
RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	iTestExpError = aTestExpError;
	}

// Start test
void RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Try to get the number of meta data entries while in the EPlaying state."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				TInt numMetaDataEntries = 0;
				
				INFO_PRINTF1(_L("iUtility->GetnumMetaDataEntries()"));
				err = iUtility->GetNumberOfMetaDataEntries(numMetaDataEntries);
				
				// Not all controllers support metadata so iTestExpError is checked to determine expected behaviour.
				if(err == iTestExpError)
					{
					if (iTestExpError != KErrNone)
						{
						INFO_PRINTF2(_L("Received expected error code: %d"), iTestExpError);
						StopTest(err, EKnownFailure);
						break;
						}
					else
						{
						INFO_PRINTF2(_L("Get number of metadata entries sucessful, number of metadata entries: %d"), numMetaDataEntries);
						StopTest(err);
						break;
						}
					}
				else
					{
					ERR_PRINTF3(_L("ERROR: Expected error code: %d, Actual Error code: %d"), iTestExpError, err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Try to get a meta data entry while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0092, MM-MMF-APLCLNT-CHRTZ-OGG-I-0092
MM-MMF-APLCLNT-CHRTZ-AMR-I-0092, MM-MMF-APLCLNT-CHRTZ-MP3-I-0092, MM-MMF-APLCLNT-CHRTZ-AAC-I-0092
*******************************************************************************************************/

RTestStepAPGetMetaDataEntryWhilePlaying* RTestStepAPGetMetaDataEntryWhilePlaying::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError)
	{
	RTestStepAPGetMetaDataEntryWhilePlaying* self = new (ELeave) RTestStepAPGetMetaDataEntryWhilePlaying(aStepName, aSectName, aKeyName, aTestExpError);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMetaDataEntryWhilePlaying::RTestStepAPGetMetaDataEntryWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	iTestExpError = aTestExpError;
	}

// Start test
void RTestStepAPGetMetaDataEntryWhilePlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Try to get a meta data entry while in the EPlaying state."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetMetaDataEntryWhilePlaying::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set next state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				CMMFMetaDataEntry* metaEntry = NULL;
				
				TRAP(err, metaEntry = iUtility->GetMetaDataEntryL(0));
				CleanupStack::PushL(metaEntry);
				
				// Not all controllers support metadata so iTestExpError is checked to determine expected behaviour.
				if(err == iTestExpError)
					{
					if (iTestExpError != KErrNone)
						{
						INFO_PRINTF2(_L("Received expected error code: %d"), iTestExpError);
						CleanupStack::PopAndDestroy(metaEntry);
						StopTest(err, EKnownFailure);
						break;
						}
					else
						{
						TPtrC entryName = metaEntry->Name();
						TPtrC entryValue = metaEntry->Value();
						INFO_PRINTF3(_L("Get metadata entry sucessful, metadata entry (Name:Value): %S:%S"), &entryName, &entryValue);
						CleanupStack::PopAndDestroy(metaEntry);
						StopTest(err);
						break;
						}
					}
				else
					{
					ERR_PRINTF3(_L("ERROR: Expected error code: %d, Actual Error code: %d"), iTestExpError, err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}

// Callback handler for CPeriodicTimer
void RTestStepAPGetMetaDataEntryWhilePlaying::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
	
/******************************************************************************************************
Get the the number of meta data entries before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0093, MM-MMF-APLCLNT-CHRTZ-OGG-I-0093
MM-MMF-APLCLNT-CHRTZ-AMR-I-0093, MM-MMF-APLCLNT-CHRTZ-MP3-I-0093, MM-MMF-APLCLNT-CHRTZ-AAC-I-0093
*******************************************************************************************************/

RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip* RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip* self = new (ELeave) RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Get the the number of meta data entries before opening the audio clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				TInt numMetaDataEntries = 0;
				
				INFO_PRINTF1(_L("iUtility->GetnumMetaDataEntries()"));
				err = iUtility->GetNumberOfMetaDataEntries(numMetaDataEntries);
				
				// Expect KErrNotReady
				if(err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
/******************************************************************************************************
Get the meta data entries before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0094, MM-MMF-APLCLNT-CHRTZ-OGG-I-0094
MM-MMF-APLCLNT-CHRTZ-AMR-I-0094, MM-MMF-APLCLNT-CHRTZ-MP3-I-0094, MM-MMF-APLCLNT-CHRTZ-AAC-I-0094
*******************************************************************************************************/

RTestStepAPGetMetaDataEntryBeforeOpenClip* RTestStepAPGetMetaDataEntryBeforeOpenClip::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPGetMetaDataEntryBeforeOpenClip* self = new (ELeave) RTestStepAPGetMetaDataEntryBeforeOpenClip(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPGetMetaDataEntryBeforeOpenClip::RTestStepAPGetMetaDataEntryBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPGetMetaDataEntryBeforeOpenClip::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE -> PLAYUTILITY1.23 - Get the meta data entries before opening the audio clip."));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPGetMetaDataEntryBeforeOpenClip::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:		 	
				{
				CMMFMetaDataEntry* metaEntry = NULL;
				
				TRAP(err, metaEntry = iUtility->GetMetaDataEntryL(0));
				
				// EABI warning removal - "metaEntry was set but never used"
				// Not interested in the meta data entry just the API return code.
				delete metaEntry;
				metaEntry = NULL;

				// Expect error code KErrNotReady.
				if(err == KErrNotReady)
					{
					INFO_PRINTF1(_L("Received expected error code KErrNotReady"));
					StopTest(err, EPass);
					break;
					}
				else
					{
					ERR_PRINTF2(_L("ERROR: Expected error code: KErrNotReady, Actual Error code: %d"), err);
					StopTest(err, EFail);
					break;
					}
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}


/******************************************************************************************************
Test added as part of CR1566 (TruePause)
Test should work in both A3F and non-A3F even when the DevSound implementation doesn't support true pause
Play-Pause-Play.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0095, MM-MMF-APLCLNT-CHRTZ-OGG-I-0095
MM-MMF-APLCLNT-CHRTZ-AMR-I-0095, MM-MMF-APLCLNT-CHRTZ-MP3-I-0095, MM-MMF-APLCLNT-CHRTZ-AAC-I-0095
*******************************************************************************************************/

RTestStepAPPlayPausePlay* RTestStepAPPlayPausePlay::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	RTestStepAPPlayPausePlay* self = new (ELeave) RTestStepAPPlayPausePlay(aStepName, aSectName, aKeyName);
	return self;
	}
	 
// Private constructor
RTestStepAPPlayPausePlay::RTestStepAPPlayPausePlay(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName) :
	RTestBaseAudioPlay(aStepName, aSectName, aKeyName)
	{
	}

// Start test
void RTestStepAPPlayPausePlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("CR1566 - Play-Pause-Play"));
	
	// Get the filename
	InitialiseFileName();
	}

//State Machine Function
void RTestStepAPPlayPausePlay::Fsm()
	{
	TInt err = KErrNone;
	
	if(iExpState == iState)
		{
		switch (iState)
			{
			case EAPIdle:
				{
				// Open iUtility
				INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFileName);
				TRAP(err, iUtility->OpenFileL(iFileName));
				
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR: Could not open file: %S"), &iFileName);
					StopTest(err);
					break;
					}
				
				// Set state
				iState = EAPOpening;
				
				SetExpectedState(EAPOpenComplete, KErrNone);
				break;
				}
			case EAPOpenComplete:
				{
				// Play iUtility
				INFO_PRINTF1(_L("iUtility->Play()"));
				iUtility->Play();
				
				// Set state
				iState = EAPPlaying;
				SetExpectedState(EAPPlaying, KErrNone);
								
				// Start the timer to call stop
				TCallBack callback (TimerCallback, this);
				iTimer->Start(KDelay, KDelay, callback);
				break;
				}
			case EAPPlaying:
				{
				// Pause iUtility
				INFO_PRINTF1(_L("iUtility->Pause()"));
				iUtility->Pause();
				
				TTimeIntervalMicroSeconds position1(0);
				TTimeIntervalMicroSeconds position2(0);
				TTimeIntervalMicroSeconds startPosition(0);

				err = iUtility->GetPosition(position1);
				
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
				
				// Wait for 1 second
				User::After(KDelay);
				
				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
					
				err = iUtility->GetPosition(position2);

				if(err != KErrNone)
					{
					ERR_PRINTF1(_L("ERROR: GetPosition() returned with error."));
					StopTest(err);
					break;
					}
				
				// Check that the position has not been reset to zero.
				if (position1 == startPosition)
					{
					ERR_PRINTF1(_L("ERROR: Position reset to start."));
					StopTest(err, EFail);
					break;
					}
				
				// Check that the position remains the same while paused.	
				if (position1 != position2)
					{
					ERR_PRINTF1(_L("ERROR: Position changed during pause."));
					StopTest(err, EFail);
					break;
					}
				else
					{
					// Test Finished
					INFO_PRINTF2(_L("The position remains the same after a 1 second delay, for audio file: %S"), &iFileName);
					// Play iUtility
					INFO_PRINTF1(_L("iUtility->Play()"));
					iUtility->Play();
					iUtility->GetPosition(position2);
					// Check that the position remains the same while paused.	
					if (Abs(position2.Int64()-position1.Int64()) <= KDelay/2)
						{
						INFO_PRINTF3(_L("Resume position is between expected threshold. Expected %Ld Retrieved %Ld"), position1.Int64(), position2.Int64());
						}
					else
						{
						ERR_PRINTF3(_L("ERROR: Resume position is not between expected threshold. Expected %Ld Retrieved %Ld"), position1.Int64(), position2.Int64());
						StopTest(err, EFail);
						break;
						}
				
					// Set next state
					iState = EAPPlaying;
				
					SetExpectedState(EAPPlayComplete, KErrNone);
					break;
					}
				}
			case EAPPlayComplete:
				{
				// Test Finished
				INFO_PRINTF2(_L("Received for play complete callback with KErrNone, for audio file: %S"), &iFileName);
				StopTest(err);
				break;
				}
			default:
				{
				// Shouldn't be here so test failed.
				StopTest(err, EFail);
				break;
				}
			}
		}
	else
		{
		WrongState(err);
		}
	}
	
// Callback handler for CPeriodicTimer
void RTestStepAPPlayPausePlay::DoTimerCallback()
	{
	// Cancel the timer as only needed one time.
	iTimer->Cancel();
	
	// All of the logic should be handled in the state machine.
	Fsm();
	}
