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


#include "audiorec_teststep.h"

_LIT8(KBlankSpace," ");
_LIT(KExpectedFourCCKey, "ExpectedFourCC%02d");

const TInt KMaximumParameterNameSize = 20;
const TInt KFourCCLenght = 4;


/****************************************
*   MM-MMF-ARCLNT-CHRTZ-I-0001
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0001
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0001
*   
*	Open an Audio file
*****************************************/
// static NewL()
RAudioRecTestStep* RAudioRecTestStep::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RAudioRecTestStep* self = new (ELeave) RAudioRecTestStep(aStepName, aFilename);
	return self;
	}

// private constructor
RAudioRecTestStep::RAudioRecTestStep(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RAudioRecTestStep::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# - This is just a simple Recorder test in all States"));
	}

void RAudioRecTestStep::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RAudioRecTestStep::Fsm(TAudioRecorderState iState)
	{

    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
            	//Open file
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened ------"));
                //Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF1(_L("Recording stop by Timer"));
                //Calling timer
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped ------"));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                // Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying ------ "));
     			SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Complete - with KErrNone"));
                StopTest(iError, EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************
*   MM-MMF-ARCLNT-CHRTZ-I-0002
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0002
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0002
*   
*	Open an Audio file while in Opened State
*****************************************/

// static NewL()
RTestStepAudioRecOpen* RTestStepAudioRecOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecOpen* self = new (ELeave) RTestStepAudioRecOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecOpen::RTestStepAudioRecOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.0 - Open an audio file in Opened state."));
	}
	
void RTestStepAudioRecOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
                INFO_PRINTF1(_L("iRecorder->OpenFileL in Opened state"));
                TRAP(iError,iRecorder->OpenFileL(iFileName));
                if(iError == KErrNone)
                	{
                	INFO_PRINTF1(_L("Open in Opened state have no operation"));
                	}
                else
                	{
                	INFO_PRINTF2(_L("Open in Opened state Failed:%d "),iError);
                	StopTest(iError);
                	}
      			StopTest(iError);
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}
	
/****************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0004
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0004
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0004
*   Play an Audio file in Playing state
*****************************************/
// static NewL()
RTestStepAudioRecPlayInPlaying* RTestStepAudioRecPlayInPlaying::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecPlayInPlaying* self = new (ELeave) RTestStepAudioRecPlayInPlaying(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecPlayInPlaying::RTestStepAudioRecPlayInPlaying(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecPlayInPlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.1 - Play an audio file in Palying state."));
	}
	
void RTestStepAudioRecPlayInPlaying::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened --------"));
                INFO_PRINTF1(_L("iRecorder->Play()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying, KErrNone);
                break;
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying --------"));
            	//Calling PlayL() in Playing state i,e EPlaying state
                INFO_PRINTF1(_L("iRecorder->Play() in Playing state"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlayWhilePlaying, KErrNotReady);
                break;
            case EARStatePlayWhilePlaying:
                INFO_PRINTF1(_L("iRecorder->EARStatePlayWhilePlaying in Playing state"));
                SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
                break;
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Complete - with KErrNone"));
                StopTest(iError,EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0005
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0005
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0005
*
*	Play an Audio file Before Open
*****************************************/
// static NewL()
RTestStepAudioRecPlayBeforeOpen* RTestStepAudioRecPlayBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecPlayBeforeOpen* self = new (ELeave) RTestStepAudioRecPlayBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecPlayBeforeOpen::RTestStepAudioRecPlayBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecPlayBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.1 - Play an audio file Before Open."));
	}
	
void RTestStepAudioRecPlayBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	//Opening an audio file
            	INFO_PRINTF1(_L("iRecorder->EARStateIdle"));
     		    INFO_PRINTF1(_L("Starting Play() Before Open "));
            	TRAP(iError,iRecorder->PlayL());
            	if (iError == KErrNotSupported)             
            		{
	                INFO_PRINTF2(_L("Playing before Open, returned Error(KErrNotSupported): %d "),iError);
	            	StopTest(iError, EPass);
            		}
            	else
            		{
	            	ERR_PRINTF2(_L("Playing before Open, returned Error : %d "),iError);
	                StopTest(iError, EFail);
            		}
            	break;	
            	}
			}
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0008
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0008
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0008
*
*   Record an Audio file Before Open 
*****************************************/
// static NewL()
RTestStepAudioRecRecordBeforeOpen* RTestStepAudioRecRecordBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecRecordBeforeOpen* self = new (ELeave) RTestStepAudioRecRecordBeforeOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecRecordBeforeOpen::RTestStepAudioRecRecordBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecRecordBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.3 - Record an audio file Before Open."));
	}
	
void RTestStepAudioRecRecordBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	//Opening an audio file
            	INFO_PRINTF1(_L("iRecorder->EARStateIdle"));
     		    INFO_PRINTF1(_L("Starting Recording Before Open "));
            	TRAP(iError,iRecorder->RecordL());
            	if (iError == KErrNotSupported)             
            		{
                	INFO_PRINTF2(_L("Recording before Open, returned Error(KErrNotSupported): %d "),iError);
            		StopTest(iError, EPass);
            		}
            	else
            		{
            		ERR_PRINTF2(_L("Recording before Open, returned Error : %d "),iError);
                	StopTest(iError, EFail);
            		}
            	}
             }
        }
	else 
		{
		StopTest(iError);
		}
	}

/********************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0009
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0009
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0009

*   Record an audio file with maximum size.
*
*********************************************/
// static NewL()
RTestStepAudioRecRecordMaxSize* RTestStepAudioRecRecordMaxSize::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecRecordMaxSize* self = new (ELeave) RTestStepAudioRecRecordMaxSize(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecRecordMaxSize::RTestStepAudioRecRecordMaxSize(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecRecordMaxSize::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.3 - Record an audio file with maximum size."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLength"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLength = %d"), iSetMaxWriteLength );
		}
	}
	
void RTestStepAudioRecRecordMaxSize::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
                INFO_PRINTF1(_L("iRecorder->SetMaxWriteLength() ..."));
                //Setting Maximum file size before record
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                INFO_PRINTF1(_L("EARStateRecording ------"));
            	SetExpectedEventAndError(EARStateRecordStopped,KErrEof);
                break;
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("EARStateRecordStopped ------"));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/*************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0010
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0010
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0010

*   Record an audio file While in Recording State.
*
**************************************************/
// static NewL()
RTestStepAudioRecRecordWhileInRecording* RTestStepAudioRecRecordWhileInRecording::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecRecordWhileInRecording* self = new (ELeave) RTestStepAudioRecRecordWhileInRecording(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecRecordWhileInRecording::RTestStepAudioRecRecordWhileInRecording(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecRecordWhileInRecording::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.3 - Record an Audio File While in Recording State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLengthT"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLengthT = %d"), iSetMaxWriteLength );
		}
	}

void RTestStepAudioRecRecordWhileInRecording::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened ------"));
                INFO_PRINTF1(_L("iRecorder->SetMaxWriteLength() ..."));
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                //Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF1(_L("iRecorder->RecordL() while in Recording"));
                //Calling RecordL() while in ERecording state.
                TRAP(iError,iRecorder->RecordL());
            	SetExpectedEventAndError(EARStateRecordInRecording,KErrNotReady);
                break;
            	}
            case EARStateRecordInRecording:
            	{
            	INFO_PRINTF1(_L("iRecorder->recording going on..."));
            	INFO_PRINTF1(_L("Recording stop by Timer"));
				iTimer->Start(KOneSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrEof);
                break;  
            	}
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError,EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/*************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0012,MM-MMF-ARCLNT-CHRTZ-I-0013,MM-MMF-ARCLNT-CHRTZ-I-0014
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0012,MM-MMF-ARCLNT-CHRTZ-AMR-I-0013,MM-MMF-ARCLNT-CHRTZ-AMR-I-0014
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0012,MM-MMF-ARCLNT-CHRTZ-AAC-I-0013,MM-MMF-ARCLNT-CHRTZ-AAC-I-0014
* 	Getting MaxVolume in Before Open and While Playing
*
**************************************************/
// static NewL()
RTestStepAudioRecMaxVolBeforeOpen* RTestStepAudioRecMaxVolBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMaxVolBeforeOpen* self = new (ELeave) RTestStepAudioRecMaxVolBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecMaxVolBeforeOpen::RTestStepAudioRecMaxVolBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecMaxVolBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.5 - Getting MaxVolume Before Open and While in Playing State."));
	}

void RTestStepAudioRecMaxVolBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("iRecorder->EARStateIdle ------"));
     		    INFO_PRINTF1(_L("Getting MaxVolume Before Open "));
            	iMaxVol = iRecorder->MaxVolume();
            	if (iMaxVol >= 0)             
            		{
                	INFO_PRINTF2(_L("MaxVolume Before Open Returned: %d  "),iMaxVol);
            		}
            	else
            		{
            		INFO_PRINTF2(_L("MaxVolume Failed Before Open,Returned: %d  "),iMaxVol);
                	StopTest(iMaxVol);
            		}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	INFO_PRINTF1(_L("Getting MaxVolume in Opened state"));
            	iMaxVol = iRecorder->MaxVolume();
            	if (iMaxVol >= 0)             
            		{
                	INFO_PRINTF2(_L("MaxVolume in Opened state,Returned: %d  "),iMaxVol);
            		}
            	else
            		{
            		INFO_PRINTF2(_L("MaxVolume in Opened state Failed,Returned: %d  "),iMaxVol);
                	StopTest(iMaxVol);
            		}
                INFO_PRINTF1(_L("iRecorder->Play()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying, KErrNone);
                break;
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
                INFO_PRINTF1(_L("iRecorder->MaxVolume() in Playing state"));
				iMaxVol = iRecorder->MaxVolume();
            	if (iMaxVol >= 0)             
            		{
                	INFO_PRINTF2(_L("MaxVolume in Playing, Returned: %d  "),iMaxVol);
            		}
            	else
            		{
            		INFO_PRINTF2(_L("MaxVolume failed in Playing state,Returned: %d  "),iMaxVol);
                	StopTest(iMaxVol);
            		}
                SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
                break;
            	}
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));
                StopTest(iError);
                break;	
             }
        }
	else 
		{
		StopTest(iError);
		}
	}

/*************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0017
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0017
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0017
*
*   Set and Get Volume Before Open an Audio file
*
**************************************************/
// static NewL()
RTestStepAudioRecSetGetVolBeforeOpen* RTestStepAudioRecSetGetVolBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetVolBeforeOpen* self = new (ELeave) RTestStepAudioRecSetGetVolBeforeOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecSetGetVolBeforeOpen::RTestStepAudioRecSetGetVolBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecSetGetVolBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.6,RECUTILITY1.7 - Set and Get Volume Before Open."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetVolume"),iSetVolume);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetVolume = %d"), iSetVolume );
		}
	}
	
void RTestStepAudioRecSetGetVolBeforeOpen::Fsm(TAudioRecorderState iState)
	{
  	TInt volume;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	//Opening an audio file
            	INFO_PRINTF1(_L("iRecorder->EARStateIdle ------"));
            	INFO_PRINTF2(_L("Setting Volume Before Open: %d "),iSetVolume);
            	iRecorder->SetVolume(iSetVolume);
            	iError = iRecorder->GetVolume(volume);
				INFO_PRINTF2(_L("Getting Volume Before Open : %d "),volume);
            	if (iError >= 0)             
            		{
                	INFO_PRINTF2(_L("Getting Volume Before Open Returned: %d  "),iError);
            		}
            	else
            		{
            		INFO_PRINTF2(_L("Getting Volume Failed,Returned(KErrNotSupported): %d  "),iError);
                	StopTest(EPass);
            		}
             }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0015,MM-MMF-ARCLNT-CHRTZ-I-0016, 
*	MM-MMF-ARCLNT-CHRTZ-I-0018, MM-MMF-ARCLNT-CHRTZ-I-0019
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0015,MM-MMF-ARCLNT-CHRTZ-AMR-I-0016, 
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0018, MM-MMF-ARCLNT-CHRTZ-AMR-I-0019
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0015,MM-MMF-ARCLNT-CHRTZ-AAC-I-0016, 
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0018, MM-MMF-ARCLNT-CHRTZ-AAC-I-0019

* 	Set Volume to Less than Zero,Greater than Zero 
* 	and Maximum Value while in Open/Playing state.
*
*********************************************************************************************(***/
// static NewL()
RTestStepAudioRecSetVolInOpen* RTestStepAudioRecSetVolInOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetVolInOpen* self = new (ELeave) RTestStepAudioRecSetVolInOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecSetVolInOpen::RTestStepAudioRecSetVolInOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecSetVolInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.6 - Set Volume to Less than Zero,Greater than Zero  ."));
	INFO_PRINTF1(_L("                          and Maximum Value while in Open/Playing state."));
	// reading TInt value from the config file
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetVolume"),iSetVolume);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetVolume = %d"), iSetVolume );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetNegativeVol"),iSetNegativeVol);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetNegativeVol = %d"), iSetNegativeVol );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetMaxVol"),iSetMaxVol);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetMaxVol = %d"), iSetMaxVol );
		}
	}
	
void RTestStepAudioRecSetVolInOpen::Fsm(TAudioRecorderState iState)
	{
   	TInt retVolume;
   	TInt err;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	INFO_PRINTF2(_L("Set Volume to any value,SetVolume : %d "),iSetVolume);
            	//Setting volume to any value
            	iRecorder->SetVolume(iSetVolume);
            	//Getting the volume which was set to any value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get Volume which was set to SetVolume:%d: GetVolume:%d "),iSetVolume,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("Th Error Value is %d "),err);
            		StopTest(err);
            		}
            	//Setting volume to any negative value
            	INFO_PRINTF2(_L("Set Volume to Less Than Zero is SetNegativeVol: %d "),iSetNegativeVol);
            	iRecorder->SetVolume(iSetNegativeVol);
            	//Getting the volume which was set to Negative value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get Less than Zero which was set to SetNegativeVol:%d: GetVolume:: %d "),iSetNegativeVol,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("Th Error Value is %d "),err);
            		StopTest(err);
            		}
            	//Setting volume to Maximum value
            	INFO_PRINTF2(_L("Set Volume to Maximum Value SetMaxVol:%d "),iSetMaxVol);
            	iRecorder->SetVolume(iSetMaxVol);
            	//Getting the volume which was set to Maximum value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get the Max Volume which was set to SetMaxVol:: %d, GetVolume:%d "),iSetMaxVol,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("Th Error Value is %d "),err);
            		StopTest(err);
            		}
            	INFO_PRINTF1(_L(" iRecorder->PlayL() "));
            	TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
            	INFO_PRINTF2(_L("Set Volume to any value SetVolume : %d "),iSetVolume);
            	//Setting volume to any value
            	iRecorder->SetVolume(iSetVolume);
            	//Getting the volume which was set to any value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get Volume which was set to SetVolume:%d: GetVolume:%d in Playing..."),iSetVolume,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("The Error Value is %d In playing"),err);
            		StopTest(err);
            		}
            	INFO_PRINTF2(_L("Set Volume to Less Than Zero is SetNegativeVol: %d , In Playing"),iSetNegativeVol);
            	//Setting volume to any negative value
            	iRecorder->SetVolume(iSetNegativeVol);
            	//Getting the volume which was set to Negative value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get Less than Zero which was set to SetNegativeVol:%d: GetVolume:: %d ,In Playing"),iSetNegativeVol,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("The Error Value is %d, In Playing"),err);
            		StopTest(err);
            		}
            	INFO_PRINTF2(_L("Set Volume to Maximum Value SetMaxVol:%d, In Playing"),iSetMaxVol);
            	//Setting volume to Maximum value
            	iRecorder->SetVolume(iSetMaxVol);
            	//Getting the volume which was set to Maximum value
            	err = iRecorder->GetVolume(retVolume);
            	INFO_PRINTF3(_L("Get the Max Volume which was set to SetMaxVol:: %d, GetVolume:%d,In Playing"),iSetMaxVol,retVolume);
            	if (err != KErrNone)
            		{
               		INFO_PRINTF2(_L("Th Error Value is %d, In Playing"),err);
            		StopTest(err);
            		}
     			SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
            	}
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));	
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/*************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0021, MM-MMF-ARCLNT-CHRTZ-I-0022
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0021, MM-MMF-ARCLNT-CHRTZ-AMR-I-0022
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0021, MM-MMF-ARCLNT-CHRTZ-AAC-I-0022

* 	Setting SetVolumeRamp Before Open and while in Playing an Audio file
*
***************************************************************/
// static NewL()
RTestStepAudioRecSetVolRampBeforeOpen* RTestStepAudioRecSetVolRampBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetVolRampBeforeOpen* self = new (ELeave) RTestStepAudioRecSetVolRampBeforeOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecSetVolRampBeforeOpen::RTestStepAudioRecSetVolRampBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecSetVolRampBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.8 - SetVolumeRamp Before Open and Playing an Audio file."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetRampDuration"),iSetRampDuration);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetRampDuration = %d"), iSetRampDuration );
		}
	}
	
void RTestStepAudioRecSetVolRampBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	INFO_PRINTF2(_L("SetVolumeRamp: %d, Before Open an Audio file "),iSetRampDuration);
            	//Setting Volume ramp 
            	TRAP(iError,iRecorder->SetVolumeRamp(TTimeIntervalMicroSeconds(iSetRampDuration)));
            	if (iError == KErrNone)             
            		{
                	INFO_PRINTF2(_L("SetVolumeRamp Before Open Returned: %d  "),iError);
            		}
            	else
            		{
            		INFO_PRINTF2(_L("SetVolumeRamp failed Returned: %d  "),iError);
                	StopTest(iError);
            		}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	INFO_PRINTF1(_L(" iRecorder->PlayL() "));
            	TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying ------ "));
            	INFO_PRINTF2(_L("SetVolumeRamp: %d, In Playing state"),iSetRampDuration);
            	//Setting volume Ramp
            	TRAP(iError,iRecorder->SetVolumeRamp(TTimeIntervalMicroSeconds(iSetRampDuration)));
            	if (iError == KErrNone)             
	            	{
	                INFO_PRINTF2(_L("SetVolumeRamp Returned: %d, In Playing State"),iError);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("SetVolumeRamp Failed,Returned: %d,In Playing State"),iError);
	                StopTest(iError);
	            	}
            	SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));	
                StopTest(iError);
                break;
             }
        }
	else 
		{
		StopTest(iError);
		}
	}

/*************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0023,MM-MMF-ARCLNT-CHRTZ-I-0024, MM-MMF-ARCLNT-CHRTZ-I-0025
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0023,MM-MMF-ARCLNT-CHRTZ-AMR-I-0024, MM-MMF-ARCLNT-CHRTZ-AMR-I-0025
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0023,MM-MMF-ARCLNT-CHRTZ-AAC-I-0024, MM-MMF-ARCLNT-CHRTZ-AAC-I-0025
* 	Getting Maximum Gain Before Open And while in Recording an Audio file
*
**************************************************/
// static NewL()
RTestStepAudioRecMaxGainBeforeOpen* RTestStepAudioRecMaxGainBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMaxGainBeforeOpen* self = new (ELeave) RTestStepAudioRecMaxGainBeforeOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecMaxGainBeforeOpen::RTestStepAudioRecMaxGainBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecMaxGainBeforeOpen::DoKickoffTestL()
	{
	//Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.9 - Getting Maximum Gain Before Open and while in ENotReady,EOpen,ERecording State."));
	}

void RTestStepAudioRecMaxGainBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
  	if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecMaxGainBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("iRecorder->EARStateIdle ------"));
            	INFO_PRINTF2(_L(" Getting Maximum Gain: %d, Before Open an Audio file "),iMaxGain);
            	//Getting Maximum Gain for the audio file
            	iMaxGain = iRecorder->MaxGain();
            	if (iMaxGain == KErrNone)             
	            	{
	                INFO_PRINTF2(_L("Get MaxGain Before Open Returned Error: %d  "),iMaxGain);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("Get MaxGain Failed,Returned Error: %d  "),iMaxGain);
	                StopTest(iMaxGain);
	            	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------ "));
            	INFO_PRINTF1(_L("iRecorder->MaxGain()"));
            	//Getting Maximum Gain for the audio file
                iMaxGain = iRecorder->MaxGain();
            	if (iMaxGain != KErrNone)             
	            	{
	                INFO_PRINTF2(_L("Get MaxGain While in Opened state Returned: %d"),iMaxGain);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("Get MaxGain Failed,While in Opened state Returned: %d  "),iMaxGain);
	                StopTest(iMaxGain);
	            	}
            	INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF1(_L("iRecorder->MaxGain()"));
                //Getting Maximum Gain for the audio file
                iMaxGain = iRecorder->MaxGain();
            	if (iMaxGain != KErrNone)             
	            	{
	                INFO_PRINTF2(_L("Get MaxGain While in Recording Returned: %d"),iMaxGain);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("<Else>Get MaxGain While in Recording Returned: %d  "),iMaxGain);
	                StopTest(iMaxGain);
	            	}
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
             }
        }
	else 
		{
		StopTest(iMaxGain);
		}
	}

/*************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0026, MM-MMF-ARCLNT-CHRTZ-I-0027, 
*   MM-MMF-ARCLNT-CHRTZ-I-0028, MM-MMF-ARCLNT-CHRTZ-I-0029,MM-MMF-ARCLNT-CHRTZ-I-0030
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0026, MM-MMF-ARCLNT-CHRTZ-AMR-I-0027, 
*   MM-MMF-ARCLNT-CHRTZ-AMR-I-0028, MM-MMF-ARCLNT-CHRTZ-AMR-I-0029,MM-MMF-ARCLNT-CHRTZ-AMR-I-0030
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0026, MM-MMF-ARCLNT-CHRTZ-AAC-I-0027, 
*   MM-MMF-ARCLNT-CHRTZ-AAC-I-0028, MM-MMF-ARCLNT-CHRTZ-AAC-I-0029,MM-MMF-ARCLNT-CHRTZ-AAC-I-0030

*   Set And Get gain in All Conditions in All States
*
**************************************************/
// static NewL()
RTestStepAudioRecSetAndGetGaininAllCond* RTestStepAudioRecSetAndGetGaininAllCond::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetAndGetGaininAllCond* self = new (ELeave) RTestStepAudioRecSetAndGetGaininAllCond(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecSetAndGetGaininAllCond::RTestStepAudioRecSetAndGetGaininAllCond(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecSetAndGetGaininAllCond::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.10, RECUTILITY1.10a - Set And Get Gain in All Conditions in All States."));
	// read in a TInt value from the config file
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetGain"),iSetGain);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetGain = %d"), iSetGain );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetGain"),iSetGain);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("Boundary SetGain = %d"), iSetGain );
		}
	}

void RTestStepAudioRecSetAndGetGaininAllCond::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
   	if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}
	
void RTestStepAudioRecSetAndGetGaininAllCond::Fsm(TAudioRecorderState iState)
	{
   	TInt retGain;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	INFO_PRINTF2(_L("Set Gain to : %d, while in Before Open"),iSetGain);
            	//Setting the Gain of an audio file
            	iRecorder->SetGain(iSetGain);
            	//Getting the corresponding Gain which was set
            	TInt err = iRecorder->GetGain(retGain);
            	INFO_PRINTF3(_L("Get Gain which was set to SetGain: %d: GetGain: %d in Before Open"),iSetGain,retGain);
            	if (err == KErrNotSupported)
	            	{
	               	INFO_PRINTF2(_L("The GetGain Value in Before Open(err) is %d "),err);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The GetGain Value Failed in Before Open,Returned %d "),err);
	            	StopTest(err);
	            	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------ "));
            	INFO_PRINTF2(_L("Set Gain to : %d, while in Opened state"),iSetGain);
            	//Setting the Gain of an audio file
            	iRecorder->SetGain(iSetGain);
            	//Getting the corresponding Gain which was set
            	TInt err = iRecorder->GetGain(retGain);
            	INFO_PRINTF3(_L("Get Gain which was set to: %d: GetGain: %d in Opened state"),iSetGain,retGain);
            	if (err == KErrNone)
	            	{
	               	INFO_PRINTF2(_L("The GetGain Value in Opened state is: %d "),err);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The GetGain Failed in Opened state returned: %d "),err);
	            	StopTest(err);
	            	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF2(_L("Set Gain to : %d, while in Recording"),iSetGain);
            	//Setting the Gain of an audio file
            	iRecorder->SetGain(iSetGain);
            	//Getting the corresponding Gain which was set
            	TInt err = iRecorder->GetGain(retGain);
            	INFO_PRINTF3(_L("Get Gain which was set to: %d: GetGain: %d in Recording"),iSetGain,retGain);
            	if (err == KErrNone)
	            	{
	               	INFO_PRINTF2(_L("The GetGain Value in Recording is %d "),err);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The GetGain Failed in Recording state,returned:%d "),err);
	            	StopTest(err);
	            	}
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0031,MM-MMF-ARCLNT-CHRTZ-I-0032,
*	MM-MMF-ARCLNT-CHRTZ-I-0033,MM-MMF-ARCLNT-CHRTZ-I-0034,MM-MMF-ARCLNT-CHRTZ-I-0126
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0031,MM-MMF-ARCLNT-CHRTZ-AMR-I-0032,
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0033,MM-MMF-ARCLNT-CHRTZ-AMR-I-0034,MM-MMF-ARCLNT-CHRTZ-AMR-I-0126
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0031,MM-MMF-ARCLNT-CHRTZ-AAC-I-0032,
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0033,MM-MMF-ARCLNT-CHRTZ-AAC-I-0034,MM-MMF-ARCLNT-CHRTZ-AAC-I-0126

*   Set And Get Position in All Conditions in All States
*
*********************************************************/
// static NewL()
RTestStepAudioRecSetAndGetPositionInAllCond* RTestStepAudioRecSetAndGetPositionInAllCond::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetAndGetPositionInAllCond* self = new (ELeave) RTestStepAudioRecSetAndGetPositionInAllCond(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecSetAndGetPositionInAllCond::RTestStepAudioRecSetAndGetPositionInAllCond(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecSetAndGetPositionInAllCond::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.11, RECUTILITY1.12 - Set And Get Position in All Conditions in All States."));
	// read in a TInt value from the config file
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPosition"),iSetPos);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetPosition = %d"),I64INT(TTimeIntervalMicroSeconds(iSetPos).Int64()));
		}
	}

void RTestStepAudioRecSetAndGetPositionInAllCond::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
   	if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetAndGetPositionInAllCond::Fsm(TAudioRecorderState iState)
	{
  TTimeIntervalMicroSeconds position;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	INFO_PRINTF2(_L("Set Position to : %d, while in Before Open"),I64INT(TTimeIntervalMicroSeconds(iSetPos).Int64()));
            	//Setting the Position
            	iRecorder->SetPosition(TTimeIntervalMicroSeconds(iSetPos));
            	//Getting the Position which was set
            	position = iRecorder->Position();
            	INFO_PRINTF2(_L("Get Position to: %d , in Before Open"),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
            	if (position == TTimeIntervalMicroSeconds(iSetPos))
	            	{
	               	INFO_PRINTF2(_L("The Position Value in Before Open is %d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The Position Value in Before Open is %d "),iError);
	            	StopTest(iError);
	            	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting the Position without setting it to any value
            	position = iRecorder->Position();
            	INFO_PRINTF2(_L("Getting Position : %d before Set Position "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	           	//Setting the Position to any value
	           	INFO_PRINTF2(_L("Settting Position to : %d, while in Opened state"),I64INT(TTimeIntervalMicroSeconds(iSetPos).Int64()));
            	iRecorder->SetPosition(TTimeIntervalMicroSeconds(iSetPos));
            	//Getting the Position which was set to any value
            	position = iRecorder->Position();
            	INFO_PRINTF2(_L("Get Position to: %d in Opened state."),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
            	if (position == TTimeIntervalMicroSeconds(iSetPos))
	            	{
	               	INFO_PRINTF2(_L("The Get Position Value in Opened state is:%d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The Get Position Failed in Opened state,returned: %d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	StopTest(iError);
	            	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                //Getting the Position without setting it to any value
                position = iRecorder->Position();
            	INFO_PRINTF2(_L("Getting Position : %d before Set Position "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	           	//Setting the Position to any value
	           	INFO_PRINTF2(_L("Settting Position to : %d, while in Recording state"),I64INT(TTimeIntervalMicroSeconds(iSetPos).Int64()));
            	iRecorder->SetPosition(TTimeIntervalMicroSeconds(iSetPos));
            	//Getting the Position which was set to any value
            	position = iRecorder->Position();
            	INFO_PRINTF2(_L("Get Position to: %d in Recording state."),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
            	if (position != TTimeIntervalMicroSeconds(iSetPos))
	            	{
	            	INFO_PRINTF2(_L("The Get Position Value in Recording state is: %d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	}
            	else
	            	{
	               	INFO_PRINTF2(_L("The Get Position Failed in Recording state,returned:%d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	INFO_PRINTF2(_L("The Get Position Failed in Recording state,returned(iError):%d "),iError);
	            	StopTest(iError);
	            	}
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped ------"));
            	INFO_PRINTF1(_L("iRecorder->Stop() ------"));
                TRAP(iError,iRecorder->Stop());
                // Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("iRecorder->EARStatePlaying ... "));
                //Getting the Position without setting it to any value
                position = iRecorder->Position();
            	INFO_PRINTF2(_L("Getting Position : %d before Set Position "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	           	//Setting the Position to any value
	           	INFO_PRINTF2(_L("Settting Position to : %d, while in Playing state"),I64INT(TTimeIntervalMicroSeconds(iSetPos).Int64()));
            	iRecorder->SetPosition(TTimeIntervalMicroSeconds(iSetPos));
            	//Getting the Position which was set to any value
            	position = iRecorder->Position();
            	INFO_PRINTF2(_L("Get Position to: %d ,Playing state"),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
            	if (position == TTimeIntervalMicroSeconds(iSetPos))
	            	{
	               	INFO_PRINTF2(_L("The Get Position Value in Playing state is: %d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	}
            	else if (TTimeIntervalMicroSeconds(iSetPos) < 0 ) //Checking for any Negative value
	            	{
	               	INFO_PRINTF2(_L("The Get Position Value (Position<0) in Playing state is: %d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	}
	            else	
	            	{
	            	INFO_PRINTF2(_L("The Get Position Failed in Playing state,returned:%d "),I64INT(TTimeIntervalMicroSeconds(position).Int64()));
	            	StopTest(iError);
	            	}
     			SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
            	}
           case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0036, MM-MMF-ARCLNT-CHRTZ-I-0037
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0036, MM-MMF-ARCLNT-CHRTZ-AMR-I-0037
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0036, MM-MMF-ARCLNT-CHRTZ-AAC-I-0037
*   Record Time Available while in Recorind and while in before open an audio file.
*
***********************************************************************************/
// static NewL()
RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen* RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen* self = new (ELeave) RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.14 - Record Time Available before Opean an audio file and while in Recording."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLengthT"),iSetMaxWriteLength);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetMaxWriteLengthT = %d"), iSetMaxWriteLength );
		}
	}

void RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
   	if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	TTimeIntervalMicroSeconds recTime;
            	//Getting the Record time vailable
            	recTime = iRecorder->RecordTimeAvailable();
            	TInt64 time = recTime.Int64();
            	INFO_PRINTF2(_L("Getting RecordTime Available : %ld, while in Before Open"),time);
            	if (recTime == time )
	            	{
	               	INFO_PRINTF2(_L("The RecordTimeAvailable Value in Before Open is:%d "),time);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The RecordTimeAvailable Failed in Before Open, returned:%d "),iError);
	            	StopTest(iError);
	            	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            		
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	// Getting RecordTimeAvailable() in Open state
            	TTimeIntervalMicroSeconds recTime3;
            	recTime3 = iRecorder->RecordTimeAvailable();
            	TInt64 time3 = recTime3.Int64();
            	INFO_PRINTF2(_L("Getting RecordTime Available : %ld, while in Opened"),time3);
            	// Getting RecordTimeAvailable() with initilizing Time to 1
            	TTimeIntervalMicroSeconds recTime1(1);
            	recTime1 = iRecorder->RecordTimeAvailable();
            	TInt64 time1 = recTime1.Int64();
            	INFO_PRINTF2(_L("Getting RecordTimeAvailable with initilize time recTime1(1): %ld, while in Opened"),time1);
            	// Recording starts
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
               	SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                	
                INFO_PRINTF1(_L("EARStateRecording ------"));
            	TTimeIntervalMicroSeconds recTime2;
            	recTime2 = iRecorder->RecordTimeAvailable();
            	TInt64 time2 = recTime2.Int64();
            	//Cant set,while in Recording become it generates PANIC !!!
            	//INFO_PRINTF2(_L("iRecorder->SetMaxWriteLength() ...in Recording %d "),iSetMaxWriteLength);
                //iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
            	INFO_PRINTF2(_L("Getting RecordTime Available : %ld, while in recording"),time2);
	            	if (recTime2 == time2 )
	            	{
	               	INFO_PRINTF2(_L("The RecordTimeAvailable Value in recording is %ld "),time2);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The RecordTimeAvailable Failed in recording state,Returned%d "),iError);
	            	StopTest(iError);
	            	}
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0035
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0035
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0035
*   Record Time Available while in before Open,Opened and Recording of an audio file.
*   with SetMaxWriteLength
*
***********************************************************************************/
// static NewL()
RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength* RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength* self = new (ELeave) RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.14 - RecordTimeAvailable with SetMaxWriteLength an audio file while in Opened State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLengthT"),iSetMaxWriteLength);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetMaxWriteLengthT = %d"), iSetMaxWriteLength );
		}
	}

void RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	TTimeIntervalMicroSeconds recTime;
            	//Getting the Record time vailable
            	recTime = iRecorder->RecordTimeAvailable();
            	TInt64 time = recTime.Int64();
            	if (recTime == time )
	            	{
	               	INFO_PRINTF2(_L("The RecordTimeAvailable Value in Before Open is:%ld "),time);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The RecordTimeAvailable Failed in Before Open,Returned:%ld "),time);
	            	StopTest(iError);
	            	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	TTimeIntervalMicroSeconds recTime1;
            	//Getting RecordTimeAvailable Before initializing time
            	recTime1 = iRecorder->RecordTimeAvailable();
            	TInt64 time1 = recTime1.Int64();
            	INFO_PRINTF2(_L("Getting RecordTimeAvailable Before initializing time: %ld, while in Opened"),time1);
            	//Getting RecordTimeAvailable with initializing time to 1
            	TTimeIntervalMicroSeconds recTime2(1);
            	recTime2 = iRecorder->RecordTimeAvailable();
            	time1 = recTime2.Int64();
            	INFO_PRINTF2(_L("Getting RecordTime Available After setting time recTime1(1): %ld, while in Opened"),time1);
            	
            	//Setting SetMaxWriteLength() FOR Recording State ONLY
            	INFO_PRINTF2(_L("iRecorder->SetMaxWriteLength() ...in Opened state:%d "),iSetMaxWriteLength);
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                TTimeIntervalMicroSeconds recTime3;
            	recTime3 = iRecorder->RecordTimeAvailable();
            	TInt64 time3 = recTime3.Int64();
            	INFO_PRINTF2(_L("Getting RecordTime Available : %ld, while in Opened state"),time3);
              	// Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
               	SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("In Recording ------"));
            	TTimeIntervalMicroSeconds recTime4;
            	//Getting the Record time vailable
            	recTime4 = iRecorder->RecordTimeAvailable();
              	TInt64 time2 = recTime4.Int64();
            	if (recTime4 == time2 )
	            	{
	               	INFO_PRINTF2(_L("The RecordTimeAvailable Value in recording state:%ld "),time2);
	            	}
            	else
	            	{
	            	INFO_PRINTF2(_L("The RecordTimeAvailable Failed in recording state,Returned:%d "),time2);
	            	StopTest(iError);
	            	}
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrEof);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError, EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0038,MM-MMF-ARCLNT-CHRTZ-I-0039,
*	MM-MMF-ARCLNT-CHRTZ-I-0040,MM-MMF-ARCLNT-CHRTZ-I-0041
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0038,MM-MMF-ARCLNT-CHRTZ-AMR-I-0039,
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0040,MM-MMF-ARCLNT-CHRTZ-AMR-I-0041
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0038,MM-MMF-ARCLNT-CHRTZ-AAC-I-0039,
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0040,MM-MMF-ARCLNT-CHRTZ-AAC-I-0041

*   Getting Duration in all states i,e. while in before open,Recording and Playing an audio file.
*
*******************************************************************************************************/
// static NewL()
RTestStepAudioRecDurationBeforeOpenRecordingPlaying* RTestStepAudioRecDurationBeforeOpenRecordingPlaying::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecDurationBeforeOpenRecordingPlaying* self = new (ELeave) RTestStepAudioRecDurationBeforeOpenRecordingPlaying(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecDurationBeforeOpenRecordingPlaying::RTestStepAudioRecDurationBeforeOpenRecordingPlaying(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecDurationBeforeOpenRecordingPlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.15 - Getting Duration, while in before open,Opened,Recording and Playing an audio file."));
	}

void RTestStepAudioRecDurationBeforeOpenRecordingPlaying::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}
	
void RTestStepAudioRecDurationBeforeOpenRecordingPlaying::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle -------"));
            	TTimeIntervalMicroSeconds recTime;
            	//Getting Duration of an audio file
            	recTime = iRecorder->Duration();
	            INFO_PRINTF2(_L("The Duration Value in Before Open is:%d "),I64INT(recTime.Int64()));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	TTimeIntervalMicroSeconds recTime1(0);
            	//Getting Duration of an audio file
            	recTime1 = iRecorder->Duration();
            	INFO_PRINTF2(_L("Getting Duration Available : %d, while in Opened"),I64INT(recTime1.Int64()));
            	// Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
               	SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
            	TTimeIntervalMicroSeconds recTime2;
            	//Getting Duration of an audio file
            	recTime2 = iRecorder->Duration();
				INFO_PRINTF2(_L("The Duration Value in recording is:%d "),I64INT(recTime2.Int64()));
            	//Call Timer iCallBack to Stop Recording
            	INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KTwoSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped ------"));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                // Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
            	TTimeIntervalMicroSeconds recTime3;
            	//Getting Duration of an audio file
            	recTime3 = iRecorder->Duration();
	            INFO_PRINTF2(_L("The Duration Value in Playing is:%d "),I64INT(recTime3.Int64()));
     			SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
            	}
            case EARStatePlayStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0042,MM-MMF-ARCLNT-CHRTZ-I-0043,
*	MM-MMF-ARCLNT-CHRTZ-I-0044,MM-MMF-ARCLNT-CHRTZ-I-0045
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0042,MM-MMF-ARCLNT-CHRTZ-AMR-I-0043,
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0044,MM-MMF-ARCLNT-CHRTZ-AMR-I-0045
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0042,MM-MMF-ARCLNT-CHRTZ-AAC-I-0043,
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0044,MM-MMF-ARCLNT-CHRTZ-AAC-I-0045

* 	User wants to define the window(SetPlayWindow()) on the audio sample data i,e. while in before open,Opened and Playing an audio file.
*
*******************************************************************************************************/
// static NewL()
RTestStepAudioRecSetPlayWindowOpenAndPlaying* RTestStepAudioRecSetPlayWindowOpenAndPlaying::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetPlayWindowOpenAndPlaying* self = new (ELeave) RTestStepAudioRecSetPlayWindowOpenAndPlaying(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecSetPlayWindowOpenAndPlaying::RTestStepAudioRecSetPlayWindowOpenAndPlaying(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecSetPlayWindowOpenAndPlaying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.16 - SetPlayWindow,while in before open, Opened and Playing an audio file."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowStart"),iSetPlaywindowStart);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlaywindowStart = %d"), iSetPlaywindowStart );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowEnd"),iSetPlaywindowEnd);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetPlaywindowEnd = %d"), iSetPlaywindowEnd );
		}
	iPlayduration1 = 0;
	iPlayduration2 = 0;
  	iStart1 = 0;
  	iStop1 = 0;
	iStart2 = 0;
  	iStop2 = 0;
  	//Count initializing to Zero
  	iCount = 0;
	}

void RTestStepAudioRecSetPlayWindowOpenAndPlaying::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStatePlayStopped;
	TRAP(iError, Fsm(EARStatePlayStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}
	
void RTestStepAudioRecSetPlayWindowOpenAndPlaying::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting Duration of the Audio file
            	TUint duration = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration = %U  in Opened state"), duration);
            	
            	//Initializing Start and End Time from Config file
            	TTimeIntervalMicroSeconds startTime1(iSetPlaywindowStart * KOneSec);
            	TTimeIntervalMicroSeconds endTime1(iSetPlaywindowEnd * KOneSec);
            	
            	// Calculate the expected play duration for each test
            	if(((iSetPlaywindowEnd * KOneSec) <= duration) && (iSetPlaywindowStart >= 0))
	            	{
	            	//This will give the actual Duration between Endtime and Starttime
	            	iPlayduration1 = (I64INT(endTime1.Int64()) - I64INT(startTime1.Int64()));
	            	INFO_PRINTF4(_L("-- StartTime1 = %d , EndTime1 = %d , iPlayduration1 = %U "),I64INT(startTime1.Int64()), I64INT(endTime1.Int64()), iPlayduration1);
	            	}
            	else if ((iSetPlaywindowEnd * KOneSec) > duration)
            		{
            		//Setting Maximum Value, which is set the Duration
	            	iPlayduration1 = (duration - I64INT(startTime1.Int64()));
	            	INFO_PRINTF2(_L("-- iPlayduration1 Duration = %U  in Opened state"), iPlayduration1);
            		}
            	else
            		{
            		//Setting Less than Zero, which is set to Zero only
            		iPlayduration1 = I64INT(endTime1.Int64());
					INFO_PRINTF2(_L("iPlayduration1 Duration = %U  in Opened state"), iPlayduration1);
            		}
                
                //Setting SetPlayWindow for the audio file
                INFO_PRINTF1(_L("iRecorder->SetPlayWindow() ...in opened state"));
                iRecorder->SetPlayWindow(startTime1,endTime1);
				
				// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                
                //Counter iCount is tell you, How many times PlayL() is called
                iCount = iCount + 1;
                INFO_PRINTF2(_L("The Count is = %d "),iCount);
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %U in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
        		SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
        		break;
            	}
            case EARStatePlayStopped:
               	{
               	if(iCount == 1)
	               	{
	               	iCount = iCount + 1;
	               	INFO_PRINTF2(_L("The Count is = %d "),iCount);
	               	// Get the stopped time (for first play)
	            	iStop1.HomeTime();
					INFO_PRINTF2(_L("Stop1.HomeTime = %U "),I64INT(iStop1.Int64()));
					//Getting the actual duration
					TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
					INFO_PRINTF2(_L("-- The Actual Duration1 = %U "),actualDuration1);
					if(TimeComparison(actualDuration1, iPlayduration1, KExpectedDeviation))
						{
						INFO_PRINTF1(_L("The Time Comparison of iPlayduration with actualDuration1 succeeded (Play Window set in opened state)"));
						}
					else
						{
						ERR_PRINTF1(_L("Mismatch in duration (Play Window set in opened state)..."));
						StopTest(iError, EFail);
						break;
						}
					
					//Getting Duration of the Audio file
	            	TUint duration_play = I64INT(iRecorder->Duration().Int64());
	            	INFO_PRINTF2(_L("Get Duration = %U  after first play"), duration_play);
            	
					// Reset play windows to +2
					iSetPlaywindowStart = iSetPlaywindowStart + 2;
					iSetPlaywindowEnd = iSetPlaywindowEnd + 2;
					
					// Set the new play window position for the second time...
					TTimeIntervalMicroSeconds startTime2((iSetPlaywindowStart * KOneSec));
            		TTimeIntervalMicroSeconds endTime2((iSetPlaywindowEnd * KOneSec));
            		
            		// Calculate the expected play duration for each test
	            	if(((iSetPlaywindowEnd * KOneSec) <= duration_play) && (iSetPlaywindowStart >= 0))
		            	{
		            	//This will give the actual Duration between Endtime and Starttime
		            	iPlayduration2 = (I64INT(endTime2.Int64()) - I64INT(startTime2.Int64()));
		            	INFO_PRINTF4(_L("-- startTime2 = %d , endTime2 = %d , iPlayduration2 = %U "),I64INT(startTime2.Int64()), I64INT(endTime2.Int64()), iPlayduration2);
		            	}
	            	else if ((iSetPlaywindowEnd * KOneSec) > duration_play)
	            		{
	            		//Setting Maximum Value, which is set the Duration
		            	iPlayduration2 = (duration_play - I64INT(startTime2.Int64()));
		            	INFO_PRINTF2(_L("-- iPlayduration2 Duration(Max Value) = %U  in Opened state"), iPlayduration2);
	            		}
	            	else
	            		{
	            		//Setting Less than Zero, which is set to Zero only
	            		iPlayduration2 = I64INT(endTime2.Int64());
						INFO_PRINTF2(_L("iPlayduration2 Duration(Less than Zero) = %U  in Opened state"), iPlayduration2);
	            		}
					
					// Called as wav controller gives -6 for test with negative start position, upon trying to alter the position without clearing.
            		//iRecorder->ClearPlayWindow(); 
            		            		
            		//Setting SetPlayWindow for the audio file
            		INFO_PRINTF1(_L("iRecorder->SetPlayWindow() in Playing state"));
            		iRecorder->SetPlayWindow(startTime2, endTime2);
            		
                	//Starts Playing
                	INFO_PRINTF1(_L("iRecorder->PlayL() in Playing state"));
                	TRAP(iError,iRecorder->PlayL());
                	//Initializing the starting time
                	iStart2.HomeTime();
					INFO_PRINTF2(_L("Start2.HomeTime = %U in Opened state"),I64INT(iStart2.Int64()));
                	SetExpectedEventAndError(EARStatePlaying, KErrNone);
	               	}
               	else
               		{
               		// Get the stopped time (for second play)
					iStop2.HomeTime();
					INFO_PRINTF2(_L("Stop2.HomeTime = %d "),I64INT(iStop2.Int64()));
					//Getting the actual duration
					TUint actualDuration2 = I64INT(iStop2.MicroSecondsFrom(iStart2).Int64());
					INFO_PRINTF2(_L("-- The Actual Duration2 = %U "),actualDuration2);
				
					//Comarison for actualDuration1
					if(TimeComparison(actualDuration2, iPlayduration2, KExpectedDeviation))
						{
						INFO_PRINTF1(_L("The Time Comparison of iPlayduration2 with actualDuration2 succeeded (Play Window set in playing state)"));
						StopTest(iError);
						}
					else
						{
						INFO_PRINTF1(_L("The TimeComparison() Play in Playing state failed"));
						StopTest(iError, EFail);
						}
					}
				break;
               	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0046
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0046
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0046
* 	User wants to define the window(SetPlayWindow()) on the audio sample data i,e. while in before open.
*
*******************************************************************************************************/
// static NewL()
RTestStepAudioRecSetPlayWindowbeforeOpen* RTestStepAudioRecSetPlayWindowbeforeOpen::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetPlayWindowbeforeOpen* self = new (ELeave) RTestStepAudioRecSetPlayWindowbeforeOpen(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecSetPlayWindowbeforeOpen::RTestStepAudioRecSetPlayWindowbeforeOpen(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecSetPlayWindowbeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.16 - SetPlayWindow, while in before open."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowStart"),iSetPlaywindowStart);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlaywindowStart = %d"), iSetPlaywindowStart );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowEnd"),iSetPlaywindowEnd);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlaywindowEnd = %d"), iSetPlaywindowEnd );
		}
	iPlayduration1 = 0;
  	iStart1 = 0;
  	iStop1 = 0;
	}
	
void RTestStepAudioRecSetPlayWindowbeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	
            	//Initializing Start and End Time from Config file
            	TTimeIntervalMicroSeconds startTime1(iSetPlaywindowStart * KOneSec);
            	TTimeIntervalMicroSeconds endTime1(iSetPlaywindowEnd * KOneSec);
            	
            	//Getting actual duration between endTime1 and startTime1
            	iPlayduration1 = (I64INT(endTime1.Int64()) - I64INT(startTime1.Int64()));
            	INFO_PRINTF4(_L("StartTime = %d EndTime = %d Playduration1 = %d "),I64INT(startTime1.Int64()), I64INT(endTime1.Int64()), iPlayduration1);
            	
            	//Setting SetPlayWindow
            	INFO_PRINTF1(_L("iRecorder->SetPlayWindow() ...in before open."));
            	iRecorder->SetPlayWindow(startTime1,endTime1);
                
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting Duration of the Audio Clip
            	iPlayduration1 = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration = %d  in Before Open state"),iPlayduration1);

				// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                INFO_PRINTF2(_L("The Play Error is = %d "),iError);
                //Initialization done from here of iStart1
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %d in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
            	SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
            	break;
            	}
            case EARStatePlayStopped:
            	{
            	INFO_PRINTF1(_L("EARStatePlayStopped ------"));
               	iStop1.HomeTime();
				INFO_PRINTF2(_L("Stop1.HomeTime = %d "),I64INT(iStop1.Int64()));
				//Getting the actual Duration of Play
				TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
				INFO_PRINTF2(_L("The Actual Duration1 = %d "),actualDuration1);
				if(TimeComparison(actualDuration1, iPlayduration1, KExpectedDeviation))
					{
					INFO_PRINTF1(_L("Duration Matches - End of test"));
                	StopTest(iError);
                	}
				else
					{
                	ERR_PRINTF1(_L("The TimeComparison() actual duration1 Failed"));
					StopTest(iError, EFail);
					}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0047, MM-MMF-ARCLNT-CHRTZ-I-0048
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0047, MM-MMF-ARCLNT-CHRTZ-AMR-I-0048
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0047, MM-MMF-ARCLNT-CHRTZ-AAC-I-0048
* 	User wants to Remove the window(ClearPlayWindow()) on the audio sample data i,e. while in before open,Opened and Playing an audio file.
*
*******************************************************************************************************/
// static NewL()
RTestStepAudioRecClearPlayWindow* RTestStepAudioRecClearPlayWindow::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecClearPlayWindow* self = new (ELeave) RTestStepAudioRecClearPlayWindow(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecClearPlayWindow::RTestStepAudioRecClearPlayWindow(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecClearPlayWindow::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.17 - ClearPlayWindow,while in before open, Opened and Playing an audio file."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowStart"),iSetPlaywindowStart);
	if (returnValue)
		{
		INFO_PRINTF2(_L("iSetPlaywindowStart = %d"), iSetPlaywindowStart );
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowEnd"),iSetPlaywindowEnd);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("iSetPlaywindowEnd = %d"), iSetPlaywindowEnd );
		}
	//Initializing class variables
	iPlayduration = 0;
  	iStart1 = 0;
  	iStop1 = 0;
	iStart2 = 0;
  	iStop2 = 0;
  	//Count initializiing to Zero
  	iCount = 0;
	}
	
void RTestStepAudioRecClearPlayWindow::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting the Duration of the audio clip
            	iPlayduration = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("-- The entire Duration of the file = %U "), iPlayduration);
            	
            	//Initializing the Start and End time values from config file
            	TTimeIntervalMicroSeconds startTime1(iSetPlaywindowStart);
            	TTimeIntervalMicroSeconds endTime1(iSetPlaywindowEnd);
            	//Getting Difference for actual value (expected play duration in opened state)
            	INFO_PRINTF3(_L("-- StartTime = %d EndTime = %d "),I64INT(startTime1.Int64()), I64INT(endTime1.Int64()));
                
                //Setting SetPlayWindow
                INFO_PRINTF1(_L("iRecorder->SetPlayWindow() ...in opened."));
                iRecorder->SetPlayWindow(startTime1, endTime1);
                //Clearing all SetPlayWindow
                INFO_PRINTF1(_L("iRecorder->ClearPlayWindow() in Opened state..."));
                iRecorder->ClearPlayWindow();
				
				// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                //The Counter iCount is tell you, How many times PlayL() is called
                iCount = iCount + 1;
                INFO_PRINTF2(_L("The Count is = %d "),iCount);
                INFO_PRINTF2(_L("The Play Error is = %d "),iError);
                //Initializing the starting duration by iStart1
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %d in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
        		SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
        		break;
            	}
            case EARStatePlayStopped:
               	{
	            if(iCount == 1)
	               	{
	               	iCount = iCount + 1;
	               	// Get the stopped time (for first play)
	            	iStop1.HomeTime();
					INFO_PRINTF2(_L("Stop1.HomeTime = %d "),I64INT(iStop1.Int64()));
					//Getting the actual duration
					TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
					INFO_PRINTF2(_L("-- The Actual Duration1 = %U "),actualDuration1);
					if(TimeComparison(actualDuration1, iPlayduration, KExpectedDeviation))
						{
						INFO_PRINTF1(_L("The Time Comparison of iPlayduration with actualDuration1 succeeded (Play Window set in opened state)"));
						}
					else
						{
						ERR_PRINTF1(_L("Mismatch in duration (Play Window set in opened state)..."));
						StopTest(iError, EFail);
						break;
						}
					// Set the new play window position for the second time...
					TTimeIntervalMicroSeconds startTime2(iSetPlaywindowStart+2000000);
            		TTimeIntervalMicroSeconds endTime2(iSetPlaywindowEnd+2000000);
            		//Getting the actual duration between endTime2 and startTime2 (expected play duration in playing state)
            		INFO_PRINTF3(_L("-- StartTime2 = %d EndTime2 = %d"),I64INT(startTime2.Int64()), I64INT(endTime2.Int64()));
            		
            		//Setting SetPlayWindow
            		INFO_PRINTF1(_L("iRecorder->SetPlayWindow() in Playing state"));
            		iRecorder->SetPlayWindow(startTime2,endTime2);
            		//Clearing SetPlayWindow
            		INFO_PRINTF1(_L("ClearPlayWindow in Playing state..."));
            		iRecorder->ClearPlayWindow();
                	
                	//Starts Playing
                	INFO_PRINTF1(_L("iRecorder->PlayL() in Playing state"));
                	TRAP(iError,iRecorder->PlayL());
                	//Initializing the starting time
                	iStart2.HomeTime();
					INFO_PRINTF2(_L("Start2.HomeTime = %d in Opened state"),I64INT(iStart2.Int64()));
                	SetExpectedEventAndError(EARStatePlaying, KErrNone);
	               	}
               	else
               		{
               		// Get the stopped time (for second play)
					iStop2.HomeTime();
					INFO_PRINTF2(_L("Stop2.HomeTime = %d "),I64INT(iStop2.Int64()));
					//Getting the actual duration
					TUint actualDuration2 = I64INT(iStop2.MicroSecondsFrom(iStart2).Int64());
					INFO_PRINTF2(_L("-- The Actual Duration2 = %U "),actualDuration2);
					
					// Checking for Duration
					if(TimeComparison(actualDuration2, iPlayduration, KExpectedDeviation))
						{
						INFO_PRINTF1(_L("The Time Comparison of iPlayduration with actualDuration2 succeeded (Play Window set in playing state)"));
						StopTest(iError);
						}
					else
						{
						ERR_PRINTF1(_L("Mismatch in duration (Play Window set in playing state)..."));
						StopTest(iError, EFail);
						}
               		}
               	break;
               	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0049
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0049
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0049
* 	User wants to Remove the window(ClearPlayWindow()) on the audio sample data i,e. while in before open.
*
*******************************************************************************************************/

// static NewL()
RTestStepAudioRecClearPlayWindowBeforeOpen* RTestStepAudioRecClearPlayWindowBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecClearPlayWindowBeforeOpen* self = new (ELeave) RTestStepAudioRecClearPlayWindowBeforeOpen(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecClearPlayWindowBeforeOpen::RTestStepAudioRecClearPlayWindowBeforeOpen(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecClearPlayWindowBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.16 - SetPlayWindow & ClearPlayWindow, while in before open."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowStart"),iSetPlaywindowStart);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlaywindowStart = %d"), iSetPlaywindowStart );	
		}
	returnValue = GetIntFromConfig(iSectName,_L("SetPlaywindowEnd"),iSetPlaywindowEnd);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetPlaywindowEnd = %d"), iSetPlaywindowEnd );
		}
	//Initializing the class variable to avoid Junk values
	iPlayduration1 = 0;
  	iStart1 = 0;
  	iStop1 = 0;
	}
	
void RTestStepAudioRecClearPlayWindowBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));

            	//Initializing the Start and End time values from config file
            	TTimeIntervalMicroSeconds startTime1(iSetPlaywindowStart * KOneSec);
            	TTimeIntervalMicroSeconds endTime1(iSetPlaywindowEnd * KOneSec);
            	//Getting Actual duration between endTime1 and startTime1
            	iPlayduration1 = (I64INT(endTime1.Int64()) - I64INT(startTime1.Int64()));
            	INFO_PRINTF4(_L("StartTime = %d EndTime = %d Playduration1 = %U "),I64INT(startTime1.Int64()), I64INT(endTime1.Int64()), iPlayduration1);
            	
            	//Setting SetPlayWindow
            	INFO_PRINTF1(_L("iRecorder->SetPlayWindow() ...in before open."));
            	iRecorder->SetPlayWindow(startTime1,endTime1);
            	
            	//Removing SetPlayWindow
            	INFO_PRINTF1(_L("iRecorder->ClearPlayWindow() ...in before open."));
            	iRecorder->ClearPlayWindow();
            	
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting the Duration (to compare, since play window is cleared)
            	iPlayduration1 = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration = %U  in Before Open state"), iPlayduration1);
            	
				// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                INFO_PRINTF2(_L("The Play Error is = %d "),iError);
                // Get the start time
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %d in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
            	SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
            	break;
            case EARStatePlayStopped:
            	{
            	INFO_PRINTF1(_L("EARStatePlayStopped ------"));
            	// Get the stop time
               	iStop1.HomeTime();
				INFO_PRINTF2(_L("Stop1.HomeTime = %d "),I64INT(iStop1.Int64()));
				//Getting the actual Duration
				TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
				INFO_PRINTF2(_L("The Actual Duration1 = %d "),actualDuration1);
				
				// Check the duration & pass / fail the test
				if(TimeComparison(actualDuration1, iPlayduration1, KExpectedDeviation))
					{
					INFO_PRINTF1(_L("The TimeComparison() actual duration1 succeed"));	
					StopTest(iError);
					}
				else
					{
					ERR_PRINTF1(_L("The TimeComparison() actual duration1 failed"));
					StopTest(iError, EFail);
					}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0050
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0050
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0050
*	SetRepeats()
*	Sets the number of times the audio sample is to be repeated while in Opened state.
*
*******************************************************************************************************/

// static NewL()
RTestStepAudioRecSetRepeatsInOpen* RTestStepAudioRecSetRepeatsInOpen::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetRepeatsInOpen* self = new (ELeave) RTestStepAudioRecSetRepeatsInOpen(aStepName, aSectName);
	return self;
	}

// private constructor
RTestStepAudioRecSetRepeatsInOpen::RTestStepAudioRecSetRepeatsInOpen(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
	
// start test
void RTestStepAudioRecSetRepeatsInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.18 - Set the Number of Times To be repeated while in Open."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("RepeatNumberOfTimes"),iRepeatNumberOfTimes);
	if (returnValue)
		{
		INFO_PRINTF2(_L("RepeatNumberOfTimes = %d"), iRepeatNumberOfTimes );
		}
	iDuration = 0;
	iPlayduration1 = 0;
	iStart1 = 0;
	iStop1 = 0;
	}

void RTestStepAudioRecSetRepeatsInOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting the Duration of the audio clip
            	iDuration = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration = %d  in Opened state"),iDuration);
            	
            	TTimeIntervalMicroSeconds zero(0);
                INFO_PRINTF1(_L("SetRepeats in Opened state..."));
                iRecorder->SetRepeats(iRepeatNumberOfTimes,zero);
				//Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                INFO_PRINTF2(_L("The Play Error is = %d "),iError);
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %d in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
              	SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
            	break;
            case EARStatePlayStopped:
            	{
            	INFO_PRINTF1(_L("EARStatePlayStopped ------"));
               	iStop1.HomeTime();
				INFO_PRINTF2(_L("Stop1.HomeTime = %d "),I64INT(iStop1.Int64()));
				//Getting actual duration of Play
				TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
				INFO_PRINTF2(_L("The Actual Duration1 = %d "),actualDuration1);
				// Compare the duration and pass / fail the test
				if(TimeComparison(actualDuration1, iDuration * (iRepeatNumberOfTimes + 1), KExpectedDeviation * iRepeatNumberOfTimes))
					{
					INFO_PRINTF1(_L("The TimeComparison() actual duration1 with duration succeeded with deviation"));
					StopTest(iError);
					}
				else
					{
					INFO_PRINTF1(_L("The TimeComparison() actual duration1 with Duration failed with deviation"));
					StopTest(iError, EFail);
					}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
* 	MM-MMF-ARCLNT-CHRTZ-I-0051,MM-MMF-ARCLNT-CHRTZ-I-0052
* 	MM-MMF-ARCLNT-CHRTZ-AMR-I-0051,MM-MMF-ARCLNT-CHRTZ-AMR-I-0052
* 	MM-MMF-ARCLNT-CHRTZ-AAC-I-0051,MM-MMF-ARCLNT-CHRTZ-AAC-I-0052
*	SetRepeats()
*	Sets the number of times the audio sample is to be repeated during the Playing state 
* 	and Boundary state(less than Zero).
*******************************************************************************************************/

// static NewL()
RTestStepAudioRecSetRepeatsInPlay* RTestStepAudioRecSetRepeatsInPlay::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetRepeatsInPlay* self = new (ELeave) RTestStepAudioRecSetRepeatsInPlay(aStepName, aSectName);
	return self;
	}

// private constructor
RTestStepAudioRecSetRepeatsInPlay::RTestStepAudioRecSetRepeatsInPlay(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
	
// start test
void RTestStepAudioRecSetRepeatsInPlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.18 - Set the Number of Times To be repeated while in Play."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("RepeatNumberOfTimes"),iRepeatNumberOfTimes);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("RepeatNumberOfTimes = %d"), iRepeatNumberOfTimes );
		}
	iPlayduration2 = 0;
	iStart2 = 0;
	iStop2 = 0;
	iCount = 0;
	}

void RTestStepAudioRecSetRepeatsInPlay::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//Getting the Duration of the audio clip
            	iDuration = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration = %d  in Opened state"),iDuration);
            	// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                //Counter iCount tell you,How many times the PlayL() is called
               	iCount = iCount + 1;
                INFO_PRINTF2(_L("The Count is = %d "),iCount);
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	{
            	INFO_PRINTF1(_L("EARStatePlaying -----"));
            	if(iCount == 1)
            		{
            		iCount = iCount + 1;
            		INFO_PRINTF1(_L("iRecorder->stop() in Playing state"));
                	TRAP(iError,iRecorder->Stop());
                	//Initialize to Zero
                	TTimeIntervalMicroSeconds zero(0);
            	 	INFO_PRINTF1(_L("SetRepeats in Playing state..."));
                	iRecorder->SetRepeats(iRepeatNumberOfTimes,zero);
                	INFO_PRINTF1(_L("iRecorder->PlayL() in Playing state"));
                	TRAP(iError,iRecorder->PlayL());
                	iStart2.HomeTime();
					INFO_PRINTF2(_L("Start2.HomeTime = %d in Playing state"),I64INT(iStart2.Int64()));
                	SetExpectedEventAndError(EARStatePlaying,KErrNone);
              		}
              	else 
              		{
            		SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
              		}
            	break;
            	}
            case EARStatePlayStopped:
            	{
            	INFO_PRINTF1(_L("EARStatePlayStopped -----"));
               	iStop2.HomeTime();
				INFO_PRINTF2(_L("Stop2.HomeTime = %d "),I64INT(iStop2.Int64()));
				//Getting Actual Duration of Play
				TUint actualDuration2 = I64INT(iStop2.MicroSecondsFrom(iStart2).Int64());
				INFO_PRINTF2(_L("The Actual Duration2 = %d "),actualDuration2);
				if(TimeComparison(actualDuration2, iDuration* (iRepeatNumberOfTimes+1), KExpectedDeviation* (iRepeatNumberOfTimes))) 
					{
					INFO_PRINTF1(_L("The TimeComparison() iPlayduration2,Play in Playing, succeed with deviation to Get Duration"));
					StopTest(iError);
					}
				else
					{
					ERR_PRINTF1(_L("The TimeComparison() iPlayduration2,Play in Playing, failed with deviation to Get Duration"));
					StopTest(iError, EFail);
					}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/******************************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0130
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0130
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0130
*	SetRepeats()
*	Sets the number of times the audio sample is to be repeated before Open an Audio file.
*
*******************************************************************************************************/
// static NewL()
RTestStepAudioRecSetRepeatsBeforeOpen* RTestStepAudioRecSetRepeatsBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RTestStepAudioRecSetRepeatsBeforeOpen* self = new (ELeave) RTestStepAudioRecSetRepeatsBeforeOpen(aStepName, aSectName);
	return self;
	}
// private constructor
RTestStepAudioRecSetRepeatsBeforeOpen::RTestStepAudioRecSetRepeatsBeforeOpen(const TDesC& aStepName, const TDesC& aSectName) :
	RAudioRecTest(aStepName, aSectName)
	{
	}
// start test
void RTestStepAudioRecSetRepeatsBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.18 - Set the Number of Times To be repeated while Before Open."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("RepeatNumberOfTimes"),iRepeatNumberOfTimes);
	if (returnValue)
		{
		INFO_PRINTF2(_L("RepeatNumberOfTimes = %d"), iRepeatNumberOfTimes );
		}
	iDuration = 0;
	iPlayduration1 = 0;
	iStart1 = 0;
	iStop1 = 0;
	}
	
void RTestStepAudioRecSetRepeatsBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	// set repeats
            	TTimeIntervalMicroSeconds zero(0);
            	INFO_PRINTF1(_L("SetRepeats() in Before Open state..."));
                iRecorder->SetRepeats(iRepeatNumberOfTimes, zero);
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	//getting duration of the audio clip
            	iDuration = I64INT(iRecorder->Duration().Int64());
            	INFO_PRINTF2(_L("Get Duration() = %d  in Before Open state"),iDuration);
                // Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                INFO_PRINTF2(_L("The Play Error is = %d "),iError);
               	iStart1.HomeTime();
				INFO_PRINTF2(_L("Start1.HomeTime = %d in Opened state"),I64INT(iStart1.Int64()));
     			SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
            	INFO_PRINTF1(_L("EARStatePlaying ------"));
              	SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
            	break;
            case EARStatePlayStopped:
            	{
            	INFO_PRINTF1(_L("EARStatePlayStopped ------"));
               	iStop1.HomeTime();
				INFO_PRINTF2(_L("Stop1.HomeTime = %d "),I64INT(iStop1.Int64()));
				TUint actualDuration1 = I64INT(iStop1.MicroSecondsFrom(iStart1).Int64());
				INFO_PRINTF2(_L("The Actual Duration1 = %d "),actualDuration1);
				if(TimeComparison(actualDuration1, iDuration , KExpectedDeviation))
					{
					INFO_PRINTF1(_L("The TimeComparison() actual duration1 with Duration succeeded with deviation"));	
					StopTest(iError);
					}
				else
					{
					ERR_PRINTF1(_L("The TimeComparison() actual duration1 with Duration failed with deviation"));
					StopTest(iError, EFail);
					}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0055
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0055
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0055
* 	Sets the Maximum Size Less than ZERO for a file that is being recorded 
* 	while in Open State.
*
***********************************************************/
// static NewL()
RTestStepAudioRecMinSizeInOpen* RTestStepAudioRecMinSizeInOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMinSizeInOpen* self = new (ELeave) RTestStepAudioRecMinSizeInOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecMinSizeInOpen::RTestStepAudioRecMinSizeInOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecMinSizeInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.19 - Setting Maximum file Size Less than ZERO while in Opened state."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLength"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLength = %d"), iSetMaxWriteLength );
		}
	}
void RTestStepAudioRecMinSizeInOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}
	
void RTestStepAudioRecMinSizeInOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
				INFO_PRINTF1(_L("EARStateOpened ------"));
                //This test generates PANIC in Emulator UDEB mode, but PASSES in UREL mode & Hardware.
                INFO_PRINTF2(_L("iRecorder->SetMaxWriteLength(%d)"), iSetMaxWriteLength);
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                // Start Recording
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
            	{
            	INFO_PRINTF1(_L("EARStateRecording ------"));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
            	}
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0053
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0053
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0053
* 	Sets the maximum for a file that is being recorded 
*  	while in Open State.
*
***********************************************************/

// static NewL()
RTestStepAudioRecMaxSizeInOpen* RTestStepAudioRecMaxSizeInOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMaxSizeInOpen* self = new (ELeave) RTestStepAudioRecMaxSizeInOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecMaxSizeInOpen::RTestStepAudioRecMaxSizeInOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecMaxSizeInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.19 - Setting Maximum file size while in Opened state."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLength"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLength = %d"), iSetMaxWriteLength );
		}
	}

void RTestStepAudioRecMaxSizeInOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}


void RTestStepAudioRecMaxSizeInOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is EARStateOpened ------"));
                INFO_PRINTF1(_L("iRecorder->SetMaxWriteLength() ...in Opened State"));
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("State is EARStateRecording ------"));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	//SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrEof);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError,EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0054
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0054
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0054
*  	Sets the maximum size for a file that is being recorded 
*  	while in Recording State.
*
***********************************************************/

// static NewL()
RTestStepAudioRecMaxSizeInRecord* RTestStepAudioRecMaxSizeInRecord::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMaxSizeInRecord* self = new (ELeave) RTestStepAudioRecMaxSizeInRecord(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecMaxSizeInRecord::RTestStepAudioRecMaxSizeInRecord(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecMaxSizeInRecord::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.19 - Setting Maximum file size while in Recording state."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLength"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLength = %d"), iSetMaxWriteLength );
		}

	}
	
void RTestStepAudioRecMaxSizeInRecord::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecMaxSizeInRecord::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("EARStateOpened ------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                //By setting the SetMaxWriteLength() in Recording State generates PANIC !!!,
                //This test generates PANIC in Emulator UDEB mode, but PASSES in UREL mode & Hardware.
                INFO_PRINTF2(_L("iRecorder->SetMaxWriteLength(%d)"), iSetMaxWriteLength);
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                //INFO_PRINTF1(_L("[Test Skipped]- > SetMaxWriteLength() ...generates PANIC  !!! in Recording state"));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped ...."));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0056
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0056
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0056
*  	Sets the maximum size for a file that is being recorded 
*  	Before Open an Audio file.
*
***********************************************************/

// static NewL()
RTestStepAudioRecMaxSizeInBeforeOpen* RTestStepAudioRecMaxSizeInBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecMaxSizeInBeforeOpen* self = new (ELeave) RTestStepAudioRecMaxSizeInBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecMaxSizeInBeforeOpen::RTestStepAudioRecMaxSizeInBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecMaxSizeInBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.19 - Setting Maximum file size while in Before Open an Audio File."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetMaxWriteLength"),iSetMaxWriteLength);
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetMaxWriteLength = %d"), iSetMaxWriteLength );
		}
	}
	
void RTestStepAudioRecMaxSizeInBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecMaxSizeInBeforeOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->SetMaxWriteLength() ..."));
                iRecorder->SetMaxWriteLength(iSetMaxWriteLength);
                //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
                INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0058
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0058
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0058
* 	SetPriority()
*  	Sets the Priority a file that is being recorded while in Open state.
*  
***********************************************************/
// static NewL()
RTestStepAudioRecSetPriorityInOpen* RTestStepAudioRecSetPriorityInOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetPriorityInOpen* self = new (ELeave) RTestStepAudioRecSetPriorityInOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetPriorityInOpen::RTestStepAudioRecSetPriorityInOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetPriorityInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.20 - Sets the Priority a file that is being recorded while in Open state."));
	}

void RTestStepAudioRecSetPriorityInOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}


void RTestStepAudioRecSetPriorityInOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
             	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	// TO DO, as an object of CMMFMdaAudioRecorderUtility cant be created 
            	// and doesnt have access rights of its class variable iPrioritySettings
            	// in GetAudRecorderPrioritySettings() 
                //Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop() Recording"));
                TRAP(iError,iRecorder->Stop());
                // Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            case EARStatePlaying:
            	INFO_PRINTF1(_L("iRecorder->EARStatePlaying ... "));
     			SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
           case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));
                StopTest(iError);
                break;
            
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0063
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0063
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0063
*  	Set and Get Play Back Balance for a file that is being recorded 
*  	Before Open an Audio file.
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetPlayBackBalBeforeOpen* RTestStepAudioRecSetPlayBackBalBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetPlayBackBalBeforeOpen* self = new (ELeave) RTestStepAudioRecSetPlayBackBalBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetPlayBackBalBeforeOpen::RTestStepAudioRecSetPlayBackBalBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetPlayBackBalBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 - Set Play Back Balance for a file, Before Open an Audio file.."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlayBackBalance"),iSetPlayBackBalance );
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlayBackBalance = %d"), iSetPlayBackBalance );
		}
	}

void RTestStepAudioRecSetPlayBackBalBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetPlayBackBalBeforeOpen::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Setting SetPlaybackBalance for the audio clip
            	iReturn = iRecorder->SetPlaybackBalance(iSetPlayBackBalance);
            	if(iReturn== KErrNotSupported)
            		{
            		INFO_PRINTF2(_L("SetPlaybackBalance is not set with return(KErrNotSupported): %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("SetPlaybackBalance failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
				//Getting PlaybackBalance for the audio clip            	            	
            	iReturn = iRecorder->GetPlaybackBalance(getReturn);
            	if(iReturn== KErrNotSupported)
            		{
            		INFO_PRINTF2(_L("GetPlaybackBalance is getting(KErrNotSupported) : %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("GetPlaybackBalance failed to set, returned error is %d"),getReturn);
					StopTest(iReturn);
					}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0064
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0064
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0064
*  	Set and Get Play Back Balance for a file that is being recorded 
*  	While in Open State (Setting Maximum Value).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetPlayBackBalInOpenedMaxValue* RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetPlayBackBalInOpenedMaxValue* self = new (ELeave) RTestStepAudioRecSetPlayBackBalInOpenedMaxValue(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::RTestStepAudioRecSetPlayBackBalInOpenedMaxValue(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 -(Setting Maximum Value)Set Play Back Balance for a file in Opened State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlayBackMaxBalance"),iSetPlayBackMaxBalance );
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetPlayBackMaxBalance = %d"), iSetPlayBackMaxBalance );
		}
	}

void RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetPlayBackBalInOpenedMaxValue::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("EARStateIdle ---------"));
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ---------"));
            	//Setting PlaybackBalance for the audio clip
            	iReturn = iRecorder->SetPlaybackBalance(iSetPlayBackMaxBalance);
            	if(iReturn== KErrNone)
            		{
            		INFO_PRINTF2(_L("SetPlaybackBalance(Maximum Value)  is set with return: %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("SetPlaybackBalance(Maximum Value)  failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
            	//Getting PlaybackBalance for the audio clip            	
            	iReturn = iRecorder->GetPlaybackBalance(getReturn);
            	if((iReturn != KErrNone) ||(getReturn != KMMFBalanceMaxRight) )
            		{
            		INFO_PRINTF2(_L("GetPlaybackBalance(Maximum Value)  is Failed, Return with : %d"),iReturn);
            		StopTest(iReturn);
            		}
            	else
            		{
					INFO_PRINTF3(_L("GetPlaybackBalance(Maximum Value) is returned: %d ,was set to: %d"),getReturn,iSetPlayBackMaxBalance);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0065
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0065
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0065
*  	Set And Get Play Back Balance for a file that is being recorded 
*  	While in Open State(Setting Minimum Value).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetPlayBackBalInOpenedMinValue* RTestStepAudioRecSetPlayBackBalInOpenedMinValue::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetPlayBackBalInOpenedMinValue* self = new (ELeave) RTestStepAudioRecSetPlayBackBalInOpenedMinValue(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetPlayBackBalInOpenedMinValue::RTestStepAudioRecSetPlayBackBalInOpenedMinValue(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetPlayBackBalInOpenedMinValue::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 -(Setting Minimum Value)Set Play Back Balance for a file in Opened State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetPlayBackMinBalance"),iSetPlayBackMinBalance );
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetPlayBackMinBalance = %d"), iSetPlayBackMinBalance );
		}
	}

void RTestStepAudioRecSetPlayBackBalInOpenedMinValue::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetPlayBackBalInOpenedMinValue::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	//Setting PlaybackBalance for the audio clip
            	iReturn = iRecorder->SetPlaybackBalance(iSetPlayBackMinBalance);
            	if(iReturn== KErrNone)
            		{
            		INFO_PRINTF2(_L("SetPlaybackBalance(Minimum Value) is set with return: %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("SetPlaybackBalance(Minimum Value) failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
            	//Getting PlaybackBalance for the audio clip            	
            	iReturn = iRecorder->GetPlaybackBalance(getReturn);
            	if((iReturn != KErrNone) ||(getReturn != KMMFBalanceMaxLeft) )
            		{
            		INFO_PRINTF2(_L("GetPlaybackBalance(Minimum Value) is Failed, Return with : %d"),iReturn);
            		StopTest(iReturn);
            		}
            	else
            		{
					INFO_PRINTF3(_L("GetPlaybackBalance(Minimum Value) is returned: %d ,was set to: %d"),getReturn,iSetPlayBackMinBalance);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0068
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0068
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0068
*  	Set and Get the current recording balance setting between the left and right stereo microphones 
*  	Before Open an Audio file.
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetRecordBalBeforeOpen* RTestStepAudioRecSetRecordBalBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetRecordBalBeforeOpen* self = new (ELeave) RTestStepAudioRecSetRecordBalBeforeOpen(aStepName, aFilename);
	return self;
	}
// private constructor
RTestStepAudioRecSetRecordBalBeforeOpen::RTestStepAudioRecSetRecordBalBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
// start test
void RTestStepAudioRecSetRecordBalBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.23,RECUTILITY1.24 - Set Record Balance for a file, Before Open an Audio file.."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetRecordBalance"),iSetRecordBalance );
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetRecordBalance = %d"), iSetRecordBalance );
		}
	}

void RTestStepAudioRecSetRecordBalBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}
	
void RTestStepAudioRecSetRecordBalBeforeOpen::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Setting RecordBalance of the audio clip
            	iReturn = iRecorder->SetRecordBalance(iSetRecordBalance);
            	if(iReturn== KErrNotSupported)
            		{
            		INFO_PRINTF2(_L("SetRecordBalance is not set with return(KErrNotSupported): %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("SetRecordBalance failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
            	//Getting RecordBalance of the audio clip           	
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if(iReturn== KErrNotSupported)
            		{
            		INFO_PRINTF2(_L("GetRecordBalance is getting(KErrNotSupported) : %d"),iReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("GetRecordBalance failed to set, returned error is %d"),getReturn);
					StopTest(iReturn);
					}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0066,MM-MMF-ARCLNT-CHRTZ-I-0070
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0066,MM-MMF-ARCLNT-CHRTZ-AMR-I-0070
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0066,MM-MMF-ARCLNT-CHRTZ-AAC-I-0070
*  	Set and Get Play Back Balance for a file that is being recorded 
*  	While in Open State (Setting Maximum Value).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetRecordBalInOpenMaxValue* RTestStepAudioRecSetRecordBalInOpenMaxValue::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetRecordBalInOpenMaxValue* self = new (ELeave) RTestStepAudioRecSetRecordBalInOpenMaxValue(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetRecordBalInOpenMaxValue::RTestStepAudioRecSetRecordBalInOpenMaxValue(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetRecordBalInOpenMaxValue::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 -(Setting Maximum Value)Set Play Back Balance for a file in Opened State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetRecordBalMaxValue"),iSetRecordBalMaxValue );
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetRecordBalance to Maximum Value = %d"), iSetRecordBalMaxValue );
		}
	}

void RTestStepAudioRecSetRecordBalInOpenMaxValue::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetRecordBalInOpenMaxValue::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	//Getting RecordBalance without setting it to any value of the audio clip
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if((iReturn != KErrNone) ||(getReturn != KMMFBalanceMaxRight) )
            		{
            		INFO_PRINTF2(_L("GetRecordBalance(Before Setting it) is, Return with : %d"),iReturn);
            		INFO_PRINTF2(_L("GetRecordBalance(Before Setting it) is, Return with getReturn: %d"),getReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("GetRecordBalance--(Before Setting it) is returned: %d "),getReturn);
					StopTest(iReturn);
					}
				//Setting RecordBalance of the audio clip to the Maximum value
            	iReturn = iRecorder->SetRecordBalance(iSetRecordBalMaxValue);
            	if(iReturn== KErrArgument)
            		{
            		INFO_PRINTF2(_L("SetRecordBalance(Maximum Value)  is set with return(KErrArgument): %d"),iReturn);
            		}
            	else if (iReturn== KErrNone)
            		{
					INFO_PRINTF3(_L("SetRecordBalance(Maximum Value) Returns:%d : and Sets: %d"),iReturn,iSetRecordBalMaxValue);
					}
				else
					{
					INFO_PRINTF2(_L("SetRecordBalance(Maximum Value)  failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
            	//Getting RecordBalance of the audio clip which was set to the Maximum value            	
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if((iReturn == KErrNone) ||(getReturn <= KMMFBalanceMaxRight) )
            		{
            		INFO_PRINTF2(_L("GetRecordBalance(Maximum Value)  is, Return with : %d"),iReturn);
            		INFO_PRINTF3(_L("GetRecordBalance(Maximum Value)  is, Return with getReturn: %d,was set to: %d"),getReturn,iSetRecordBalMaxValue);
            		}
            	else
            		{
					INFO_PRINTF3(_L("GetRecordBalance(Maximum Value) is returned: %d ,was set to: %d"),getReturn,iSetRecordBalMaxValue);
            		StopTest(iReturn);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0069
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0069
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0069
*  	Set and Get Play Back Balance for a file that is being recorded 
*  	While in Open State (Setting Minimum Value).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetRecordBalInOpenMinValue* RTestStepAudioRecSetRecordBalInOpenMinValue::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetRecordBalInOpenMinValue* self = new (ELeave) RTestStepAudioRecSetRecordBalInOpenMinValue(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetRecordBalInOpenMinValue::RTestStepAudioRecSetRecordBalInOpenMinValue(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetRecordBalInOpenMinValue::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 -(Setting Minimum Value)Set Play Back Balance for a file in Opened State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetRecordBalMinValue"),iSetRecordBalMinValue );
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetRecordBalance to Minimum Value = %d"), iSetRecordBalMinValue );
		}
	}

void RTestStepAudioRecSetRecordBalInOpenMinValue::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetRecordBalInOpenMinValue::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	//Getting RecordBalance of the audio clip without setting it any value
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if((iReturn != KErrNone) ||(getReturn != KMMFBalanceMaxRight) )
            		{
            		INFO_PRINTF2(_L("GetRecordBalance(Before Setting it) is, Return with : %d"),iReturn);
            		INFO_PRINTF2(_L("GetRecordBalance(Before Setting it) is, Return with getReturn: %d"),getReturn);
            		}
            	else
            		{
					INFO_PRINTF2(_L("GetRecordBalance--(Before Setting it) is returned: %d "),getReturn);
					StopTest(iReturn);
					}
				//Setting RecordBalance of the audio clip to the Minimum value
            	iReturn = iRecorder->SetRecordBalance(iSetRecordBalMinValue);
            	if(iReturn== KErrArgument)
            		{
            		INFO_PRINTF2(_L("SetRecordBalance(Minimum Value)  is set with return(KErrArgument): %d"),iReturn);
            		}
            	else if (iReturn== KErrNone)
            		{
					INFO_PRINTF3(_L("SetRecordBalance(Minimum Value) Returns:%d : and Sets: %d"),iReturn,iSetRecordBalMinValue);
					}
				else
					{
					INFO_PRINTF2(_L("SetRecordBalance(Minimum Value)  failed to set, returned error is %d"),iReturn);
					StopTest(iReturn);
					}
            	//Getting RecordBalance of the audio clip which was set to minimum value       	
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if((iReturn == KErrNone) ||(getReturn >= KMMFBalanceMaxLeft) )
            		{
            		INFO_PRINTF2(_L("GetRecordBalance(Minimum Value)  is, iReturn with : %d"),iReturn);
            		INFO_PRINTF3(_L("GetRecordBalance(Minimum Value)  is, Return with getReturn: %d,was set to: %d"),getReturn,iSetRecordBalMinValue);
            		}
            	else
            		{
					INFO_PRINTF3(_L("GetRecordBalance(Minimum Value) is returned: %d ,was set to: %d"),getReturn,iSetRecordBalMinValue);
					StopTest(iReturn);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L(" Stop Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(KErrNone);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0067
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0067
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0067
*  	Set and Get Play Back Balance for a file that is being recorded 
*  	While in Recording State.
*
***********************************************************/

// static NewL()
RTestStepAudioRecSetRecordBalInRecording* RTestStepAudioRecSetRecordBalInRecording::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetRecordBalInRecording* self = new (ELeave) RTestStepAudioRecSetRecordBalInRecording(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetRecordBalInRecording::RTestStepAudioRecSetRecordBalInRecording(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetRecordBalInRecording::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.21,RECUTILITY1.22 -Set Play Back Balance for a file in Recording State."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetRecordBalance "),iSetRecordBalance  );
	if (returnValue)
		{
		INFO_PRINTF2(_L("SetRecordBalance to = %d"), iSetRecordBalance  );
		}
	}

void RTestStepAudioRecSetRecordBalInRecording::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetRecordBalInRecording::Fsm(TAudioRecorderState iState)
	{
  	TInt getReturn;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Setting RecordBalance of the audio clip
            	INFO_PRINTF2(_L("SetRecordBalance To: %d"),iSetRecordBalance );                
            	iReturn = iRecorder->SetRecordBalance(iSetRecordBalance);
            	if(iReturn!= KErrNone)
                    {
                    ERR_PRINTF2(_L("SetRecordBalance failed to set, returned error is: %d in Recording state"), iReturn);
                    StopTest(iReturn, EFail);            		
                    }
                else
                    {
                    INFO_PRINTF1(_L("SetRecordBalance successful in Recording state"));
                    }
            	//Getting RecordBalance of the audio clip which was set           	
            	iReturn = iRecorder->GetRecordBalance(getReturn);
            	if((iReturn == KErrNone) && ((getReturn >= KMMFBalanceMaxLeft ) && (getReturn <= KMMFBalanceMaxRight) ))
            		{
            		INFO_PRINTF3(_L("GetRecordBalance returns : %d, was set to: %d in Recording state"),getReturn,iSetRecordBalance);
            		}
            	else
                    {
                    ERR_PRINTF3(_L("GetRecordBalance = %d, Error = %d in Recording state"), getReturn, iReturn);
                    StopTest(iReturn, EFail);
                    }
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Stopped Recording by Timer ..."));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
                INFO_PRINTF1(_L("iRecorder->Stop()"));
                iRecorder->Stop();
                StopTest(iReturn, EPass);                
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0071,MM-MMF-ARCLNT-CHRTZ-I-0072
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0071,MM-MMF-ARCLNT-CHRTZ-AMR-I-0072
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0071,MM-MMF-ARCLNT-CHRTZ-AAC-I-0072
*	Returns a list of the supported codecs for the conversion destination.
*  	While in Recording State.
*
***********************************************************/
// static NewL()
RTestStepAudioRecGetSupportDestDataTypeInRecording* RTestStepAudioRecGetSupportDestDataTypeInRecording::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetSupportDestDataTypeInRecording* self = new (ELeave) RTestStepAudioRecGetSupportDestDataTypeInRecording(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetSupportDestDataTypeInRecording::RTestStepAudioRecGetSupportDestDataTypeInRecording(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecGetSupportDestDataTypeInRecording::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.25 -Returns a list of the supported codecs for the conversion destination while in Opened and Recording state"));
	
	// Read which fourCC are expected to be supported
	TInt err = ReadExpectedFourCC(iExpectedFourCCToBeSupported);
	if(err != KErrNone)
		{
		StopTest(KErrNone, EInconclusive);
		}
	}

// Read expected fourcc from configuration file to be supported for this format
TInt RTestStepAudioRecGetSupportDestDataTypeInRecording::ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC)
	{
	TBuf<KMaximumParameterNameSize> variableName;
	TBuf8<KFourCCLenght> theFourCCString;
	TPtrC fourCCString;
	TInt err = KErrNone;
	
	// ExpectedFourCC paramters are enumerated starting at 1
	// and ExpectedFourcc parameters number is unknow (maximum 99)
	TUint i=1;
	variableName.Format(KExpectedFourCCKey, i);
	TBool sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
	
	while(sectionFound)
		{
		// Complete the fourCC to have a valid fourcc (4 chars)
		theFourCCString.Copy(fourCCString);
		while(theFourCCString.Length() < 4)
			{
			theFourCCString.Insert(0, KBlankSpace);
			}
		
		TFourCC fourCC(theFourCCString);
		INFO_PRINTF3(_L("%S %S"), &variableName, &fourCCString);
		// Add fourCC to array
		aExpectedFourCC.Append(fourCC);
		i++;
		variableName.Format(KExpectedFourCCKey, i);
		sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
		}
	
	if(aExpectedFourCC.Count() == 0)
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected fourCC values from Config."));
		err = KErrNotFound;
		}
	return err;
	}

void RTestStepAudioRecGetSupportDestDataTypeInRecording::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetSupportDestDataTypeInRecording::Fsm(TAudioRecorderState iState)
	{
  	TUint8 theBuffer8[10];
  	TBuf<10> bufCodec;
  	TInt numCodecs = 0;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is EARStateOpened -------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedDestinationDataTypesL() while in Opened State"));
            	TRAP(iError, iRecorder->GetSupportedDestinationDataTypesL(supFourCC));

				if (iError != KErrNone )
					{
					INFO_PRINTF2(_L("iRecorder->GetSupportedDestinationDataTypesL() failed Error:%d"), iError);	
					StopTest(iError); 
					}
				else if(supFourCC.Count() <= 0)
					{
					INFO_PRINTF1(_L("iRecorder->GetSupportedDestinationDataTypesL() doesn't return any datatypes"));	
					StopTest(KErrNotFound); 
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->GetSupportedDestinationDataTypesL() return %d datatypes"), supFourCC.Count());	
					}
				
				INFO_PRINTF1(_L("GetSupportedDestinationDataTypesL() while in Opened State"));
				
				// Making generic check expected fourcc are supported
				// Replace such statements as supFourCC.Find(TFourCC(KMMFFourCCCodeALAW)) < 0 
				TInt indexFourCCSupported = KErrNotFound;
				for(TInt i=0; i < iExpectedFourCCToBeSupported.Count(); i++)
					{
					if (supFourCC.Find(iExpectedFourCCToBeSupported[i]) > KErrNotFound) 
						{
						indexFourCCSupported = i;
						}
					}
					
				if (indexFourCCSupported == KErrNotFound) 
					{
					INFO_PRINTF1(_L("None of the expected fourCC were found "));
					StopTest(KErrNotFound); 
					}

				numCodecs = supFourCC.Count();
				for (int i=0;i<numCodecs;i++)
					{
					TPtr8 ptrFour(theBuffer8, 10);
					supFourCC[i].FourCC(&ptrFour);
                 	bufCodec.Copy(ptrFour);
                	INFO_PRINTF3(_L("GetSupportedDestinationDataTypesL are %d:(%S)"),i+1,&bufCodec);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF1(_L("iRecorder->GetSupportedDestinationDataTypesL() while in Recording State"));
            	TRAP(iError, iRecorder->GetSupportedDestinationDataTypesL(supFourCC));

				INFO_PRINTF1(_L("GetSupportedDestinationDataTypesL() while in Opened State"));
				
				// Making generic check expected fourcc are supported
				// Replace such statements as supFourCC.Find(TFourCC(KMMFFourCCCodeALAW)) < 0 
				TInt indexFourCCSupported = KErrNotFound;
				for(TInt i=0; i < iExpectedFourCCToBeSupported.Count(); i++)
					{
					if (supFourCC.Find(iExpectedFourCCToBeSupported[i]) > KErrNotFound) 
						{
						indexFourCCSupported = i;
						}
					}

				if (indexFourCCSupported == KErrNotFound) 
					{
					INFO_PRINTF1(_L("None of the expected fourCC were found "));
					StopTest(KErrNotFound); 
					}

				numCodecs = supFourCC.Count();
				for (int i=0;i<numCodecs;i++)
					{
					TPtr8 ptrFour(theBuffer8, 10);
					supFourCC[i].FourCC(&ptrFour);
                 	bufCodec.Copy(ptrFour);
                	INFO_PRINTF3(_L("GetSupportedDestinationDataTypesL are %d:(%S)"),i+1,&bufCodec);
					}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0073
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0073
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0073
* 	Returns a list of the supported codecs for the conversion destination.
*  	While before Open an Audio file.
*
***********************************************************/
// static NewL()
RTestStepAudioRecGetSupportDestDataTypeBeforeOpen* RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetSupportDestDataTypeBeforeOpen* self = new (ELeave) RTestStepAudioRecGetSupportDestDataTypeBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::RTestStepAudioRecGetSupportDestDataTypeBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}

// start test
void RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.25 -Returns a list of the supported codecs for the conversion destination.."));
	// Read which fourCC are expected to be supported
	TInt err = ReadExpectedFourCC(iExpectedFourCCToBeSupported);
	if(err != KErrNone)
		{
		StopTest(KErrNone, EInconclusive);
		}
	}

// Read expected fourcc from configuration file to be supported for this format
TInt RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC)
	{
	TBuf<KMaximumParameterNameSize> variableName;
	TBuf8<KFourCCLenght> theFourCCString;
	TPtrC fourCCString;
	TInt err = KErrNone;
	
	// ExpectedFourCC paramters are enumerated starting at 1
	// and ExpectedFourcc parameters number is unknow (maximum 99)
	TUint i=1;
	variableName.Format(KExpectedFourCCKey, i);
	TBool sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
	
	while(sectionFound)
		{
		// Complete the fourCC to have a valid fourcc (4 chars)
		theFourCCString.Copy(fourCCString);
		while(theFourCCString.Length() < 4)
			{
			theFourCCString.Insert(0, KBlankSpace);
			}
		
		TFourCC fourCC(theFourCCString);
		INFO_PRINTF3(_L("%S %S"), &variableName, &fourCCString);
		// Add fourCC to array
		aExpectedFourCC.Append(fourCC);
		i++;
		variableName.Format(KExpectedFourCCKey, i);
		sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
		}
	
	if(aExpectedFourCC.Count() == 0)
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected fourCC values from Config."));
		err = KErrNotFound;
		}
	return err;
	}

void RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetSupportDestDataTypeBeforeOpen::Fsm(TAudioRecorderState iState)
	{
  	TUint8 theBuffer8[10];
  	TBuf<10> bufCodec;
  	TInt numCodecs = 0;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedDestinationDataTypesL() Before Open State"));
            	TRAP(iError, iRecorder->GetSupportedDestinationDataTypesL(supFourCC));
            	
				if (iError != KErrNone )
					{
					INFO_PRINTF2(_L("iRecorder->GetSupportedDestinationDataTypesL() failed Error:%d"), iError);	
					}
				else if(supFourCC.Count() <= 0)
					{
					INFO_PRINTF1(_L("iRecorder->GetSupportedDestinationDataTypesL() doesn't return any datatypes"));	
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->GetSupportedDestinationDataTypesL() %d"), supFourCC.Count());	
					}

				INFO_PRINTF1(_L("GetSupportedDestinationDataTypesL() while in Opened State"));
				
				// Making generic for checking expected fourcc that should be supported
				// Replace such statements as supFourCC.Find(TFourCC(KMMFFourCCCodeALAW)) < 0 
				TInt indexFourCCSupported = KErrNotFound;
				for(TInt i=0; i < iExpectedFourCCToBeSupported.Count(); i++)
					{
					if (supFourCC.Find(iExpectedFourCCToBeSupported[i]) > KErrNotFound) 
						{
						indexFourCCSupported = i;
						}
					}
					
				if (indexFourCCSupported == KErrNotFound) 
					{
					INFO_PRINTF1(_L("None of the expected fourCC were found "));
					}
				
				numCodecs = supFourCC.Count();
				for (int i=0;i<numCodecs;i++)
					{
					TPtr8 ptrFour(theBuffer8, 10);
					supFourCC[i].FourCC(&ptrFour);
                 	bufCodec.Copy(ptrFour);
                	INFO_PRINTF3(_L("GetSupportedDestinationDataTypesL are %d:(%S)"),i+1,&bufCodec);
					}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0074, MM-MMF-ARCLNT-CHRTZ-I-0075
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0074, MM-MMF-ARCLNT-CHRTZ-AMR-I-0075
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0074, MM-MMF-ARCLNT-CHRTZ-AAC-I-0075
*  Set and Get the codec to use with the record controller.
*  Before Open an Audio file(SetDestinationDataTypeL(),DestinationDataTypeL()).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetGetDestnDataTypeBeforeOpen* RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetDestnDataTypeBeforeOpen* self = new (ELeave) RTestStepAudioRecSetGetDestnDataTypeBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::RTestStepAudioRecSetGetDestnDataTypeBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.26,RECUTILITY1.27 -Set and Get the codec to use with the record controller in b4 Open and Opened state."));
	// Read which fourCC are expected to be supported
	TInt err = ReadExpectedFourCC(iExpectedFourCCToBeSupported);
	if(err != KErrNone)
		{
		StopTest(KErrNone, EInconclusive);
		}
	}

// Read expected fourcc from configuration file to be supported for this format
TInt RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC)
	{
	TBuf<KMaximumParameterNameSize> variableName;
	TBuf8<KFourCCLenght> theFourCCString;
	TPtrC fourCCString;
	TInt err = KErrNone;
	
	// ExpectedFourCC paramters are enumerated starting at 1
	// and ExpectedFourcc parameters number is unknow (maximum 99)
	TUint i=1;
	variableName.Format(KExpectedFourCCKey, i);
	TBool sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
	
	while(sectionFound)
		{
		// Complete the fourCC to have a valid fourcc (4 chars)
		theFourCCString.Copy(fourCCString);
		while(theFourCCString.Length() < 4)
			{
			theFourCCString.Insert(0, KBlankSpace);
			}
		
		TFourCC fourCC(theFourCCString);
		INFO_PRINTF3(_L("%S %S"), &variableName, &fourCCString);
		// Add fourCC to array
		aExpectedFourCC.Append(fourCC);
		i++;
		variableName.Format(KExpectedFourCCKey, i);
		sectionFound = GetStringFromConfig(iSectName, variableName, fourCCString);
		}
	
	if(aExpectedFourCC.Count() == 0)
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected fourCC values from Config."));
		err = KErrNotFound;
		}
	return err;
	}

void RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetGetDestnDataTypeBeforeOpen::Fsm(TAudioRecorderState iState)
	{
 	TFourCC aDataType;
	TUint8 theBuffer8[10];
	TBuf<10> bufFourCC;
	TPtr8 ptrFour(theBuffer8, 10);
	iExpectedFourCCToBeSupported[0].FourCC(&ptrFour);
	bufFourCC.Copy(ptrFour);

    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Setting destination data type
            	INFO_PRINTF2(_L("Setting SetDestinationDataTypeL(%S) Before Open State"), &bufFourCC);
				TRAP(iError, iRecorder->SetDestinationDataTypeL(iExpectedFourCCToBeSupported[0]));
                if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("-- SetDestinationDataTypeL() leaved(KErrNotSupported):%d:for codec Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("-- SetDestinationDataTypeL() failed Error:%d:for codec Before Open State"),iError);		
                	StopTest(iError);
                	}
                // Getting destination data type
                INFO_PRINTF1(_L("Getting DestinationDataTypeL() Before Open State"));
				TRAP(iError, aDataType = iRecorder->DestinationDataTypeL());
                if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("-- GetDestinationDataTypeL() return (KErrNotSupported):%d:for codec Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("-- GetDestinationDataTypeL() failed Error:%d:for codec Before Open State"),iError);		
                	StopTest(iError);
                	}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is EARStateOpened ---------"));

				INFO_PRINTF2(_L("iRecorder->SetDestinationDataTypeL(%S) in Opened State"), &bufFourCC);
				//Setting destination data type
				TRAP(iError, iRecorder->SetDestinationDataTypeL(iExpectedFourCCToBeSupported[0]));
				if (iError == KErrNone)
					{
					INFO_PRINTF2(_L("SetDestinationDataTypeL() returned : %d, in Opened State"), iError);
					}
				else
					{
					ERR_PRINTF2(_L("SetDestinationDataTypeL() failed with Error : %d, in Opened State"), iError);
					StopTest(iError);
					}

                // Getting destination data type
                INFO_PRINTF1(_L("iRecorder->DestinationDataTypeL() Before Open State"));
				TRAP(iError, aDataType = iRecorder->DestinationDataTypeL());
                if (iError == KErrNone)
                	{
                	INFO_PRINTF2(_L("-- GetDestinationDataTypeL() returned : %d, in Opened State"), iError);
                	}
                else
                	{
                	ERR_PRINTF2(_L("-- GetDestinationDataTypeL() failed with Error : %d, in Opened State"), iError);
                	StopTest(iError, EFail);
                	break;
                	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError, iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording, KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0076
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0076
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0076
* 	Get the codec to use with the record controller.
* 	While in Recording State(DestinationDataTypeL()).
*
***********************************************************/
// static NewL()
RTestStepAudioRecGetDestnDataTypeInRecording* RTestStepAudioRecGetDestnDataTypeInRecording::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetDestnDataTypeInRecording* self = new (ELeave) RTestStepAudioRecGetDestnDataTypeInRecording(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetDestnDataTypeInRecording::RTestStepAudioRecGetDestnDataTypeInRecording(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecGetDestnDataTypeInRecording::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.27 -Returns the codec that is currently set for the destination audio clip"));
	}

void RTestStepAudioRecGetDestnDataTypeInRecording::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetDestnDataTypeInRecording::Fsm(TAudioRecorderState iState)
	{
  	TUint8 theBuffer8[10];
  	TBuf<10> bufCodec;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	INFO_PRINTF1(_L("State is Opened ---------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            case EARStateRecording:
	           	{
	           	INFO_PRINTF1(_L("Recording State---------"));
                TFourCC aDataType;
                //Getting destination data type
				TRAP(iError, aDataType = iRecorder->DestinationDataTypeL());
				TPtr8 ptrFour(theBuffer8, 10); //Putting into buffer
				aDataType.FourCC(&ptrFour);
                bufCodec.Copy(ptrFour);
                if (iError == KErrNone)
                	{
                	INFO_PRINTF2(_L("Get DestinationDataTypeL() return with codec : %S : while in Recording State"),&bufCodec);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("Get DestinationDataTypeL() failed Error for codec:%d: Recording State"),iError);		
                	StopTest(iError);
                	}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
	           	}
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0077, MM-MMF-ARCLNT-CHRTZ-I-0078
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0077, MM-MMF-ARCLNT-CHRTZ-AMR-I-0078
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0077, MM-MMF-ARCLNT-CHRTZ-AAC-I-0078
* 	Sets the bit rate of the data sink in bits per second
*  	Before Open an Audio file(SetDestinationBitRateL(),DestinationBitRateL).
*
***********************************************************/
// static NewL()
RTestStepAudioRecSetGetDestnBitRateBeforeOpen* RTestStepAudioRecSetGetDestnBitRateBeforeOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetDestnBitRateBeforeOpen* self = new (ELeave) RTestStepAudioRecSetGetDestnBitRateBeforeOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetGetDestnBitRateBeforeOpen::RTestStepAudioRecSetGetDestnBitRateBeforeOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetGetDestnBitRateBeforeOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.28,RECUTILITY1.29 -Set and Get the codec to use with the record controller."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetDestinationBitRate"),iSetDestinationBitRate);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetDestinationBitRate = %d"), iSetDestinationBitRate );
		}
	}

void RTestStepAudioRecSetGetDestnBitRateBeforeOpen::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetGetDestnBitRateBeforeOpen::Fsm(TAudioRecorderState iState)
	{
	RArray<TUint> bitRates;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	TUint bitRatesReturn =0;
            	//Setting Destination Bit Rate
            	INFO_PRINTF1(_L("iRecorder->SetDestinationBitRateL() Before Open State"));
				TRAP(iError,iRecorder->SetDestinationBitRateL(iSetDestinationBitRate));
				if( iError == KErrNotSupported )
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() leaves with(KErrNotSupported) :%d "), iError);	
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() Failed, leaves with :%d "), iError);	
					StopTest(iError);
					}
				//Getting Destination Bit Rate which was set
				INFO_PRINTF1(_L("iRecorder->DestinationBitRateL() Before Open State"));
				TRAP(iError,bitRatesReturn = iRecorder->DestinationBitRateL());
				if (iError == KErrNotSupported )
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() leaves with(KErrNotSupported) :%d "), iError);	
					}
				else
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() Failed, leaves with :%d "), bitRatesReturn);	
					StopTest(iError);
					}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	TUint bitRatesReturn =0;
            	TInt err;
            	INFO_PRINTF1(_L("iRecorder->SetDestinationBitRateL() in Opened State"));
				//Setting Destination Bit Rate 
				TRAP(err,iRecorder->SetDestinationBitRateL(iSetDestinationBitRate));
				if( err == KErrNotSupported )
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() leaves with(KErrNotSupported) :%d "), err);	
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() Failed, leaves with :%d "), err);	
					StopTest(err);
					}
				//Getting Destination Bit Rate which was set
				INFO_PRINTF1(_L("iRecorder->DestinationBitRateL() in Opened State"));
				TRAP(iError,bitRatesReturn = iRecorder->DestinationBitRateL());
				if ((iError == KErrNone ) || ( bitRatesReturn > 0 ))
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() leaves with:%d "), bitRatesReturn);	
					}
				else
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() Failed, leaves with :%d "), iError);	
					StopTest(iError);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on..."));
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                INFO_PRINTF1(_L(" issue !!! Controller level not Supported,TestResult Fail"));
                StopTest(iError,EKnownFailure);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0079
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0079
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0079
* 	Gets the bit rate of the data sink in bits per second
*  	While in recording State(DestinationBitRateL).
*
***********************************************************/
// static NewL()
RTestStepAudioRecGetDestnBitRateInRecording* RTestStepAudioRecGetDestnBitRateInRecording::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetDestnBitRateInRecording* self = new (ELeave) RTestStepAudioRecGetDestnBitRateInRecording(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetDestnBitRateInRecording::RTestStepAudioRecGetDestnBitRateInRecording(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecGetDestnBitRateInRecording::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.28,RECUTILITY1.29 -Set and Get the codec to use with the record controller."));
	TBool returnValue = GetIntFromConfig(iSectName,_L("SetDestinationBitRate"),iSetDestinationBitRate);
	if (returnValue) 
		{
		INFO_PRINTF2(_L("SetDestinationBitRate = %d"), iSetDestinationBitRate );
		}
	}

void RTestStepAudioRecGetDestnBitRateInRecording::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetDestnBitRateInRecording::Fsm(TAudioRecorderState iState)
	{
 	TUint bitRatesReturn = 0;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ------"));
                //Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->recording going on ------ "));
                INFO_PRINTF1(_L("iRecorder->SetDestinationBitRateL()  While in Recording State"));
                //Setting Destination Bit Rate
				TRAP(iError,iRecorder->SetDestinationBitRateL(iSetDestinationBitRate));
				if( iError == KErrNotReady )
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() leaves with(KErrNotReady) :%d "), iError);	
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationBitRateL() Failed, leaves with :%d "), iError);	
					StopTest(iError);
					}
                //Getting Destination Bit Rate which was set
                INFO_PRINTF1(_L("iRecorder->DestinationBitRateL() While in Recording State"));
				TRAP(iError,bitRatesReturn = iRecorder->DestinationBitRateL());
				if((iError == KErrNone )|| (iError > 0 ) || (iSetDestinationBitRate == bitRatesReturn ))
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() :%d "), bitRatesReturn);	
					}
				else
					{
					INFO_PRINTF2(_L("Get DestinationBitRateL() Failed, leaves with :%d "), iError);	
					StopTest(iError);
					}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped ------ "));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError,EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0080,MM-MMF-ARCLNT-CHRTZ-I-0081,MM-MMF-ARCLNT-CHRTZ-I-0082
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0080,MM-MMF-ARCLNT-CHRTZ-AMR-I-0081,MM-MMF-ARCLNT-CHRTZ-AMR-I-0082
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0080,MM-MMF-ARCLNT-CHRTZ-AAC-I-0081,MM-MMF-ARCLNT-CHRTZ-AAC-I-0082
*
* 	Returns a list of the supported bit rates for recording to the data sink.
* 	While in Before Open an Audio file and While in recording State
* 	(GetSupportedBitRatesL())
***********************************************************/
// static NewL()
RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec* RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec* self = new (ELeave) RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::DoKickoffTestL()
	{
	//Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.30-supported bit rates while in Before Open and While Recording an Audio File."));
	}

void RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec::Fsm(TAudioRecorderState iState)
	{
 	RArray<TUint> supBitRates;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedBitRatesL() Before Open State"));
            	//Getting Supported Bit Rate
				TRAP(iError,iRecorder->GetSupportedBitRatesL(supBitRates));
                if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() return(KErrNotSupported) with codec : %d : Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() failed Error for codec:%d: Before Open State"),iError);		
                	StopTest(iError);
                	}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("EARStateOpened ------"));
            	TInt err;
            	INFO_PRINTF1(_L("iRecorder->GetSupportedBitRatesL() while in Opened State"));
				//Getting Supported Bit Rate
				TRAP(err,iRecorder->GetSupportedBitRatesL(supBitRates));
            	if ( err == KErrNotSupported )
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() return(KErrNotSupported) with codec : %d : Opened State"),err);		
                	}
                else if (err == KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() for codec:%d: Opened State"),err);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() failed Error for codec:%d: Opened State"),err);		
                	StopTest(err);
                	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                TInt err;
                INFO_PRINTF1(_L("EARStateRecording ------"));
                INFO_PRINTF1(_L("iRecorder->GetSupportedBitRatesL() while in Recording State"));
				//Getting Supported Bit Rate
				TRAP(err,iRecorder->GetSupportedBitRatesL(supBitRates));
            	if ( err == KErrNotSupported )
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() return(KErrNotSupported) with codec : %d : Recording State"),err);		
                	}
                else if (err == KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedBitRatesL() for codec:%d: Recording State"),err);		
                	}
               	else
               		{
               		INFO_PRINTF2(_L("GetSupportedBitRatesL() failed Error for codec:%d: Recording State"),err);		
               		StopTest(err);
               		}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	//SetExpectedEventAndError(EARStateRecordStopped,KErrNotSupported);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped state ------"));
                INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
            	INFO_PRINTF1(_L("Getting a list of supported bit rates is NOT SUPPORTED !!!."));
                StopTest(iError,EPass);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/**********************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0083,MM-MMF-ARCLNT-CHRTZ-I-0084,MM-MMF-ARCLNT-CHRTZ-I-0085
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0083,MM-MMF-ARCLNT-CHRTZ-AMR-I-0084,MM-MMF-ARCLNT-CHRTZ-AMR-I-0085
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0083,MM-MMF-ARCLNT-CHRTZ-AAC-I-0084,MM-MMF-ARCLNT-CHRTZ-AAC-I-0085
*
* 	Returns the bit rate of the audio clip While Before Open an 
* 	Audio file and While in Playing State.
* 	SourceBitRateL()
**********************************************************/
// static NewL()
RTestStepAudioRecSourceBitRateBeforeOpenAndPalying* RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSourceBitRateBeforeOpenAndPalying* self = new (ELeave) RTestStepAudioRecSourceBitRateBeforeOpenAndPalying(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::RTestStepAudioRecSourceBitRateBeforeOpenAndPalying(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.31-Returns SourceBitRateL() of the audio clip while in ENotReady and EPlaying State."));
	}

void RTestStepAudioRecSourceBitRateBeforeOpenAndPalying::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->SourceBitRateL() Before Open State"));
            	//getting source bit rate of the audio clip
				TRAP(iError,iRetSource = iRecorder->SourceBitRateL());
				if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("Getting SourceBitRateL() leave(KErrNotSupported) : %d : Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("Getting SourceBitRateL() failed Error for codec:%d: Before Open State"),iError);		
                	StopTest(iError);
                	}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
				INFO_PRINTF1(_L("State is Opened ---------"));
				INFO_PRINTF1(_L("iRecorder->SourceBitRateL() while in Opened State"));
				//getting source bit rate of the audio clip
				TRAP(iError,iRetSource = iRecorder->SourceBitRateL());
				if (iError == KErrNone)
                	{
                	INFO_PRINTF2(_L("Getting SourceBitRateL() leave(-----) : %d : while in Opened State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("Getting SourceBitRateL() failed Error for codec:%d: while in Opened State"),iError);		
                	StopTest(iError);
                	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
            	}
            case EARStatePlaying:
                {
				INFO_PRINTF1(_L("iRecorder->Playing going on ----------"));
                INFO_PRINTF1(_L("iRecorder->SourceBitRateL() Playing State"));
				//getting source bit rate of the audio clip
				TRAP(iError,iRetSource = iRecorder->SourceBitRateL());
				if (iError == KErrNone)
                	{
                	INFO_PRINTF2(_L("Getting SourceBitRateL() returned : %d : Playing State"), iRetSource);
                	}
                else
                	{
                	INFO_PRINTF2(_L("SourceBitRateL() failed. Leave code : %d : Playing State"), iError);
                	StopTest(iError);
                	}
                SetExpectedEventAndError(EARStatePlayStopped,KErrNone);
                break;
                }
            case EARStatePlayStopped:
            	INFO_PRINTF1(_L("Play Completed - End of test"));
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0086,MM-MMF-ARCLNT-CHRTZ-I-0087,MM-MMF-ARCLNT-CHRTZ-I-0088
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0086,MM-MMF-ARCLNT-CHRTZ-AMR-I-0087,MM-MMF-ARCLNT-CHRTZ-AMR-I-0088
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0086,MM-MMF-ARCLNT-CHRTZ-AAC-I-0087,MM-MMF-ARCLNT-CHRTZ-AAC-I-0088
*
* 	Sets the sample rate for the conversion destination data sink 
* 	Gets the sample rate in samples/second that the destination data sink is 
* 	currently set to an Audio file ,While Before Open State and While in Recording State.
* 
* 	SetDestinationSampleRateL()/DestinationSampleRateL()
*
**************************************************************************88*************/
// static NewL()
RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec* RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec* self = new (ELeave) RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	iNumCodecsOpen = 0;
	iNumCodecsRecording = 0;
	}
	
// start test
void RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.32,RECUTILITY1.33-SetDestinationSampleRateL()/DestinationSampleRateL()"));
	INFO_PRINTF1(_L("of the audio clip while in ENotReady and EPlaying State."));
	iRetSource = 0;
	}

 void RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError != KErrNone)
		{
		StopTest(iError);
		}
	}

void RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->SetDestinationSampleRateL() Before Open State"));
				//Sets Destination Sample rate for the audio clip
				TRAP(iError,iRecorder->SetDestinationSampleRateL(8000));
				if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("SetDestinationSampleRateL() leave(KErrNotSupported) : %d : Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("SetDestinationSampleRateL() failed Error for codec:%d: Before Open State"),iError);		
                	StopTest(iError);
                	}
                //Gets Destination Sample rate for the audio clip
                INFO_PRINTF1(_L("iRecorder->DestinationSampleRateL() Before Open State"));
				TRAP(iError,iRetSource = iRecorder->DestinationSampleRateL());
				if (iError == KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("Getting DestinationSampleRateL() leave(KErrNotSupported) : %d : Before Open State"),iError);		
                	}
                else
                	{
                	INFO_PRINTF2(_L("Getting DestinationSampleRateL() failed Error for codec:%d: Before Open State"),iError);		
                	StopTest(iError);
                	}
            	 //Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened, KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	//Getting Supported Sample rates
            	INFO_PRINTF1(_L("iRecorder->GetSupportedSampleRatesL() Opened State"));
            	TRAP(iError, iRecorder->GetSupportedSampleRatesL(iSupSampleRates));
            	iNumCodecsOpen = iSupSampleRates.Count();
            	INFO_PRINTF2(_L("-- Count of supported sample rates (Opened State) = %d)"), iNumCodecsOpen);
				for (int i=0;i<iNumCodecsOpen;i++)
					{
					TUint sr = iSupSampleRates[i];
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL are : %d"), sr);
					}
            	if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() failed. Leave code : %d (Opened State)"), iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF1(_L("GetSupportedSampleRatesL() did not leave (Opened State)"));
                	}
                //Sets Destination Sample rate for the audio clip 
                INFO_PRINTF2(_L("Set iRecorder->SetDestinationSampleRateL(%d) (Opened State)"), iSupSampleRates[3]);
				TRAP(iError, iRecorder->SetDestinationSampleRateL(iSupSampleRates[3]));
				if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("SetDestinationSampleRateL() failed. Leave code : %d (Opened State)"), iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF1(_L("SetDestinationSampleRateL() did not leave (Opened State)"));
                	}
                //Gets Destination Sample rate for the audio clip
                INFO_PRINTF1(_L("iRecorder->DestinationSampleRateL() Opened State"));
				TRAP(iError,iRetSource = iRecorder->DestinationSampleRateL());
				if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("DestinationSampleRateL() failed. Leave code : %d (Opened State)"), iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF2(_L("DestinationSampleRateL() returned : %d (Opened State)"), iRetSource);
                	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->Recording going on ------"));
                
            	//Gets Supported Sample rates
            	INFO_PRINTF1(_L("iRecorder->GetSupportedSampleRatesL() (Recording State)"));
            	iSupSampleRates.Reset();
            	TRAP(iError, iRecorder->GetSupportedSampleRatesL(iSupSampleRates));
            	iNumCodecsRecording = iSupSampleRates.Count();
            	INFO_PRINTF2(_L("-- Count of supported sample rates (Recording State) = %d)"), iNumCodecsRecording);
				for (int i=0;i<iNumCodecsRecording;i++)
					{
					TUint sr1 = iSupSampleRates[i];
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL are : %d"), sr1);
					}
            	if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() failed leave : %d : Recording State"), iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() leave : %d (Recording State)"),iError);
                	}
                //Sets Destination Sample rate for the audio clip
                INFO_PRINTF2(_L("Set iRecorder->SetDestinationSampleRateL(%d) (Recording State)"), iSupSampleRates[0]);
				TRAP(iError,iRecorder->SetDestinationSampleRateL(iSupSampleRates[0]));
				if (iError == KErrNotReady)
                	{
                	INFO_PRINTF2(_L("SetDestinationSampleRateL() leave(KErrNotReady) : %d (Recording State)"), iError);
                	}
                else if (iError == KErrNone)
                	{
                	INFO_PRINTF1(_L("SetDestinationSampleRateL() for codec did not leave (Recording State)"));		
                	}
                else
                	{
                	INFO_PRINTF2(_L("SetDestinationSampleRateL() failed. Leave code : %d (Recording State)"), iError);		
                	StopTest(iError);
                	}
                //Gets Destination Sample rate for the audio clip
                INFO_PRINTF1(_L("iRecorder->DestinationSampleRateL() Recording State"));
				TRAP(iError, iRetSource = iRecorder->DestinationSampleRateL());
				if (iError == KErrNone)
                	{
                	INFO_PRINTF2(_L("DestinationSampleRateL() returned : %d (Recording State)"), iRetSource);
                	}
                else
                	{
                	INFO_PRINTF2(_L("DestinationSampleRateL() failed. Leave code : %d (Recording State)"), iError);
                	StopTest(iError);
                	}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
                SetExpectedEventAndError(EARStateRecordStopped, KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
				TRAP(iError, iRecorder->Stop());
				if (iNumCodecsOpen >= iNumCodecsRecording)
					{
					INFO_PRINTF1(_L("Number of codecs returned in Opened State should be greater than that of Recording State..."));
					StopTest(iError);
					}
				else
					{
					ERR_PRINTF1(_L("Number of codecs returned in Opened State less than that of Recording State..."));
					StopTest(iError, EFail);
					}
				break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0089,MM-MMF-ARCLNT-CHRTZ-I-0090, MM-MMF-ARCLNT-CHRTZ-I-0091
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0089,MM-MMF-ARCLNT-CHRTZ-AMR-I-0090, MM-MMF-ARCLNT-CHRTZ-AMR-I-0091
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0089,MM-MMF-ARCLNT-CHRTZ-AAC-I-0090, MM-MMF-ARCLNT-CHRTZ-AAC-I-0091
* 	Returns a list of supported recording sample rates
* 	of an Audio file ,While Before Open State and While in Recording State.
* 
* 	GetSupportedSampleRatesL()
*
**************************************************************************88*************/
// static NewL()
RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec* RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec* self = new (ELeave) RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.34-GetSupportedSampleRatesL()"));
	INFO_PRINTF1(_L("of the audio clip while in ENotReady and ERecording State."));
	}

void RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec::Fsm(TAudioRecorderState iState)
	{
  	TInt numCodecs = 0;
  	
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedSampleRatesL() Before Open State"));
            	//Gets Supported Sample rates of the audio clip
            	TRAP(iError, iRecorder->GetSupportedSampleRatesL(iSupSampleRates));
            	numCodecs = iSupSampleRates.Count();
				for (int i=0;i<numCodecs;i++)
					{
					TUint sr = iSupSampleRates[i];
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL are %d:"),sr);
					}
            	if (iError != KErrNotSupported)
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() failed leave() : %d : Before Open State"),iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() leave(KErrNotSupported) : %d : Before Open State"),iError);
                	}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
                INFO_PRINTF1(_L("iRecorder->GetSupportedSampleRatesL() Opened State"));
            	//Gets Supported Sample rates of the audio clip
            	TRAP(iError, iRecorder->GetSupportedSampleRatesL(iSupSampleRates));
            	numCodecs = iSupSampleRates.Count();
				for (int i=0;i<numCodecs;i++)
					{
					TUint sr = iSupSampleRates[i];
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL are %d:"),sr);
					}
            	if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() failed leave() : %d : Opened State"),iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() leave(KErrNotSupported) : %d : Opened State"),iError);
                	}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->Recording going on ----------"));
                INFO_PRINTF1(_L("iRecorder->GetSupportedSampleRatesL() Recording State"));
            	//Gets Supported Sample rates of the audio clip
            	TRAP(iError, iRecorder->GetSupportedSampleRatesL(iSupSampleRates));
            	numCodecs = iSupSampleRates.Count();
				for (TInt i=0;i<numCodecs;i++)
					{
					TUint sr = iSupSampleRates[i];
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL are %d:"),sr);
					}
            	if (iError != KErrNone)
                	{
                	INFO_PRINTF2(_L("GetSupportedSampleRatesL() failed leave() : %d : Recording State"),iError);
                	StopTest(iError);
                	}
                else
                	{
                	INFO_PRINTF1(_L("GetSupportedSampleRatesL() returned No Error"));
                	}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
                SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0092,MM-MMF-ARCLNT-CHRTZ-I-0093, MM-MMF-ARCLNT-CHRTZ-I-0094
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0092,MM-MMF-ARCLNT-CHRTZ-AMR-I-0093, MM-MMF-ARCLNT-CHRTZ-AMR-I-0094
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0092,MM-MMF-ARCLNT-CHRTZ-AAC-I-0093, MM-MMF-ARCLNT-CHRTZ-AAC-I-0094
* 	Sets the format of the audio clip. This can only be done if the audio clip does not exist.
* 	While Before Open State and While in Recording State.
* 
* 	SetDestinationFormatL()/DestinationFormatL()
*
**************************************************************************88*************/
// static NewL()
RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec* RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec* self = new (ELeave) RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.35,RECUTILITY1.36-SetDestinationFormatL()/DestinationFormatL()"));
	INFO_PRINTF1(_L("of an audio file while in ENotReady and ERecording State."));
	}

void RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->DestinationFormatL() Before Open State"));
            	//Gets Destination Format level of the audio clip without setting it to any value 
            	TRAP(iError,iFormat = iRecorder->DestinationFormatL());
            	if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("DestinationFormatL leave with(KErrNotSupported) %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}	
				//Sets Destination Format level of the audio clip to any value
				TRAP(iError,iRecorder->SetDestinationFormatL(iFormat));
				if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("SetDestinationFormatL leave with(KErrNotSupported) %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Gets Destination Format level of the audio clip which was set 
				TRAP(iError,iFormat = iRecorder->DestinationFormatL());
				TUid format2 = iFormat ;
				if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("Getting DestinationFormatL after Setting,leave with(KErrNotSupported) %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("Getting DestinationFormatL leave Failed with %u"),format2);
					StopTest(iError);
					}
				
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	INFO_PRINTF1(_L("iRecorder->DestinationFormatL() in Opened State"));
            	//Gets Destination Format level of the audio clip without setting it to any value 
            	TRAP(iError,iFormat = iRecorder->DestinationFormatL());
            	if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("DestinationFormatL leave with():%d"),iFormat);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Sets Destination Format level of the audio clip to any value
				TRAP(iError,iRecorder->SetDestinationFormatL(iFormat));
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("SetDestinationFormatL leave with(----):%d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				TUid format2;
				//Gets Destination Format level of the audio clip which was set
				TRAP(iError,format2 = iRecorder->DestinationFormatL());
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("Getting DestinationFormatL after Setting,leave with():%d"),format2);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Checking the Set and Get value
				if(format2==iFormat)
					{
					INFO_PRINTF1(_L("Set and Get DestinationFormatL (format==iFormat)passed"));
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->Recording going on ----------"));
                INFO_PRINTF1(_L("iRecorder->DestinationFormatL() in Recording State"));
            	//Gets Destination Format level of the audio clip without setting it to any value
            	TRAP(iError,iFormat = iRecorder->DestinationFormatL());
            	if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("DestinationFormatL leave with():%d"),iFormat);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Sets Destination Format level of the audio clip to any value
				TRAP(iError,iRecorder->SetDestinationFormatL(iFormat));
				if(iError == KErrNotReady)
					{
					INFO_PRINTF2(_L("SetDestinationFormatL leave with(KErrNotReady):%d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				TUid format2;
				//Gets Destination Format level of the audio clip which was set
				TRAP(iError,format2 = iRecorder->DestinationFormatL());
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("Getting DestinationFormatL after Setting,leave with():%d"),format2);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationFormatL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Checking the Set and Get value
				if(format2==iFormat)
					{
					INFO_PRINTF1(_L("Set and Get DestinationFormatL (format==iFormat)passed"));
					}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
                SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
	            {
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
	            }
        	}
		}
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0096, MM-MMF-ARCLNT-CHRTZ-I-0097
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0096, MM-MMF-ARCLNT-CHRTZ-AMR-I-0097
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0096, MM-MMF-ARCLNT-CHRTZ-AAC-I-0097
*
* 	Sets the number of channels for the recorded audio clip.
* 	of an Audio file ,While Before Open State.
* 
* 	SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL()
*
**************************************************************************88*************/
// static NewL()
RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec* RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec* self = new (ELeave) RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.37,RECUTILITY1.38-SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL()"));
	INFO_PRINTF1(_L("		 of an audio file while in ENotReady, Opened and Recording State."));
	iChannel = 0;
	}

void RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec::Fsm(TAudioRecorderState iState)
	{
  	RArray<TUint> channels;
	
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Before Open State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave with(KErrNotSupported) : %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//We cant access the address here,which causes Panic in Before Open an Audio file
				/*
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<1;i++)
					{
					TUint sr = channels[i];
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are %d:"),sr);
					}
				*/
				INFO_PRINTF1(_L("iRecorder->SetDestinationNumberOfChannelsL() Before Open State"));
				//Sets Destination Number of channels of the audio clip
				TRAP(iError,iRecorder->SetDestinationNumberOfChannelsL(1));
				if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave with(KErrNotSupported) : %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//Gets Destination Number of channels of the audio clip which was set
				TRAP(iError,iChannel = iRecorder->DestinationNumberOfChannelsL());
				if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("Getting DestinationNumberOfChannelsL after Setting,leave with(KErrNotSupported) : %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened --------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Opened State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNone)
					{
					INFO_PRINTF1(_L("GetSupportedNumberOfChannelsL leves with Zero "));
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<numCodecs;i++)
					{
					TUint sr = channels[i];
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are : %d"),sr);
					}
				//Sets Destination Number of channels of the audio clip
				INFO_PRINTF1(_L("iRecorder->SetDestinationNumberOfChannelsL() Opened State"));
				TRAP(iError,iRecorder->SetDestinationNumberOfChannelsL(channels[0]));
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL : %d"),iError);
					}
				else if(iError == KErrAlreadyExists)
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave Failed with : %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave Failed with : %d"),iError);
					StopTest(iError);
					}
				//Gets Destination Number of channels of the audio clip which was set
				INFO_PRINTF1(_L("iRecorder->DestinationNumberOfChannelsL()"));
				TRAP(iError,iChannel = iRecorder->DestinationNumberOfChannelsL());
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("Getting DestinationNumberOfChannelsL after Setting,leave with(): %d"),iChannel);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("EARStateRecording --------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Recording State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNone)
					{
					INFO_PRINTF1(_L("GetSupportedNumberOfChannelsL leves with Zero "));
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//gets number of channels
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<numCodecs;i++)
					{
					TUint sr = channels[i];
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are : %d"),sr);
					}
				//Sets Destination Number of channels of the audio clip
				INFO_PRINTF1(_L("iRecorder->SetDestinationNumberOfChannelsL() Recording State"));
				TRAP(iError,iRecorder->SetDestinationNumberOfChannelsL(channels[0]));
				if(iError == KErrNotReady)
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave with(KErrNotReady): %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("SetDestinationNumberOfChannelsL leave Failed with : %d"),iError);
					StopTest(iError);
					}
				//Gets Destination Number of channels of the audio clip which was set
				INFO_PRINTF1(_L("iRecorder->DestinationNumberOfChannelsL()"));
				TRAP(iError,iChannel = iRecorder->DestinationNumberOfChannelsL());
				if(iError == KErrNone)
					{
					INFO_PRINTF2(_L("Getting DestinationNumberOfChannelsL after Setting,leave with(): %d"),iChannel);
					}
				else
					{
					INFO_PRINTF2(_L("DestinationNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
                //Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
                SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
	            {
	        	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;            	
	            }
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0098, MM-MMF-ARCLNT-CHRTZ-I-0099, MM-MMF-ARCLNT-CHRTZ-I-0100
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0098, MM-MMF-ARCLNT-CHRTZ-AMR-I-0099, MM-MMF-ARCLNT-CHRTZ-AMR-I-0100
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0098, MM-MMF-ARCLNT-CHRTZ-AAC-I-0099, MM-MMF-ARCLNT-CHRTZ-AAC-I-0100
* 	Returns a list of the supported number of channels for recording of an Audio file.
* 	While Before Open and While in Recording State.
* 
* 	GetSupportedNumberOfChannelsL()
*
**************************************************************************88*************/
// static NewL()
RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec* RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec* self = new (ELeave) RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.39-GetSupportedNumberOfChannelsL of an audio file while in ENotReady State."));
	iChannel = 0;
	}

void RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec::Fsm(TAudioRecorderState iState)
	{
  	RArray<TUint> channels;
	
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Before Open State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNotSupported)
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave with(KErrNotSupported) : %d"),iError);
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				//We cant access the address which causes Panic in Before Open an Audio file
				/*
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<1;i++)
				{
					TUint sr = channels[i];
                 	
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are %d:"),sr);
				}
				*/
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("State is Opened ---------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Opened State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNone)
					{
					INFO_PRINTF1(_L("GetSupportedNumberOfChannelsL()"));
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<numCodecs;i++)
					{
					TUint sr = channels[i];
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are : %d"),sr);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("iRecorder->Recording going on ----------"));
            	INFO_PRINTF1(_L("iRecorder->GetSupportedNumberOfChannelsL() Recording State"));
            	//Gets Supprted Number of channels of the audio clip without setting it to any value
            	TRAP(iError,iRecorder->GetSupportedNumberOfChannelsL(channels));
            	if(iError == KErrNone)
					{
					INFO_PRINTF1(_L("GetSupportedNumberOfChannelsL() "));
					}
				else
					{
					INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL leave Failed with %d"),iError);
					StopTest(iError);
					}
				TInt numCodecs = channels.Count();
				for (TInt i=0;i<numCodecs;i++)
					{
					TUint sr = channels[i];
                	INFO_PRINTF2(_L("GetSupportedNumberOfChannelsL are : %d"),sr);
					}
				//Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
                SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
                StopTest(iError);
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0102, MM-MMF-ARCLNT-CHRTZ-I-0103
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0102, MM-MMF-ARCLNT-CHRTZ-AMR-I-0103
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0102, MM-MMF-ARCLNT-CHRTZ-AAC-I-0103
* 	Returns the number of meta data entries associated with this clip.
* 	While Before Open, Opened, Recording and While in Playing State.
* 
* 	GetNumberOfMetaDataEntries()
*
**************************************************************************88*************/
// static NewL()
RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec* RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec* self = new (ELeave) RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.40 - GetNumberOfMetaDataEntries Before Open,Opened,Recording and while Playing an audio file."));
	iMetaData = 0;

	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenIdle"), iExpErrorWhenIdle))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	else
		{
		INFO_PRINTF2(_L("ExpErrorWhenIdle = %d"), iExpErrorWhenIdle );
		}	
		
	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenOpened"), iExpErrorWhenOpened))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	else
		{
		INFO_PRINTF2(_L("ExpErrorWhenOpened = %d"), iExpErrorWhenOpened );
		}	

	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenPlaying"), iExpErrorWhenPlaying))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	else
		{
		INFO_PRINTF2(_L("ExpErrorWhenPlaying = %d"), iExpErrorWhenPlaying );
		}	
		
	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenRecording"), iExpErrorWhenRecording))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	else
		{
		INFO_PRINTF2(_L("ExpErrorWhenRecording = %d"), iExpErrorWhenRecording );
		}	
	}

void RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
	if (iError!=KErrNone)
		{
		StopTest (iError);
		}
	}

void RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
            	INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Before Open State"));
            	//Gets number of metadata entries of the audio clip
            	TInt ret1 = iRecorder->GetNumberOfMetaDataEntries(iMetaData);
            	if(ret1 == iExpErrorWhenIdle)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries return %d as expected"),ret1);
					}
				else
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries failed with %d"),ret1);
					StopTest(ret1);
					}
            	//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName3);
     			TRAP(iError,iRecorder->OpenFileL(iFileName3));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("Opened State -----------"));

				INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Opened State"));
				// Gets number of metadata entries of the audio clip
				TInt ret2 = iRecorder->GetNumberOfMetaDataEntries(iMetaData);
				
				// If is the expected error
				if(ret2 == iExpErrorWhenOpened)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries return %d as expected"),ret2);
					}
				else
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries failed with %d"),ret2);
					StopTest(ret2);
					}
				// Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break;
				}
            case EARStatePlaying:
                {
                INFO_PRINTF1(_L("Playing state -----------"));
				
				INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Opened State"));
				//Gets number of metadata entries of the audio clip
				TInt ret4 = iRecorder->GetNumberOfMetaDataEntries(iMetaData);
				
				// And is the expected error
				if(ret4 == iExpErrorWhenPlaying)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries return %d as expected"),ret4);
					}
				else
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries failed with %d"),ret4);
					StopTest(ret4);
					}
                SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
                break;
                }
            case EARStatePlayStopped:
	            {
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError,iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording,KErrNone);
                break;
            	}
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("Recording State -----------"));

				INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Opened State"));
				//Gets number of metadata entries of the audio clip
				TInt ret3 = iRecorder->GetNumberOfMetaDataEntries(iMetaData);
				if(ret3 == iExpErrorWhenRecording)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries return %d as expected"),ret3);
					}
				else
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries failed with %d"),ret3);
					StopTest(ret3);
					}

				//Call Timer iCallBack to Stop
                INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped,KErrNone);
                break;
                }
            case EARStateRecordStopped:
				{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                TUid format2;
                INFO_PRINTF1(_L("iRecorder->DestinationFormatL()"));

				TRAP(iError,format2 = iRecorder->DestinationFormatL());
                if (format2 == TUid::Uid(KWavController)) 
	                {
					StopTest(iError,EKnownFailure);  //For WAV Controller
    	            }
        	        else
            	    {
                	StopTest(iError,EPass); //For other controllers
                	}
				break;
				}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0105, MM-MMF-ARCLNT-CHRTZ-I-0106
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0105, MM-MMF-ARCLNT-CHRTZ-AMR-I-0106
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0105, MM-MMF-ARCLNT-CHRTZ-AAC-I-0106
* 	Returns the specified meta data entry from the current audio clip.
* 	While Before Open and While in Playing State.
* 
* 	GetMetaDataEntryL()
*
**************************************************************************88*************/
// static NewL()
RTestAudioRecGetMetaDataEntB4OpenAndPly* RTestAudioRecGetMetaDataEntB4OpenAndPly::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecGetMetaDataEntB4OpenAndPly* self = new (ELeave) RTestAudioRecGetMetaDataEntB4OpenAndPly(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecGetMetaDataEntB4OpenAndPly::RTestAudioRecGetMetaDataEntB4OpenAndPly(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecGetMetaDataEntB4OpenAndPly::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.41 - GetMetaDataEntryL Before Open,recording and while Playing an audio file."));

	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenIdle"), iExpErrorWhenIdle))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenPlaying"), iExpErrorWhenPlaying))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenRecording"), iExpErrorWhenRecording))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	}

void RTestAudioRecGetMetaDataEntB4OpenAndPly::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
         
      	if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestAudioRecGetMetaDataEntB4OpenAndPly::Fsm(TAudioRecorderState iState)
	{
	TInt err = KErrNone;
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	CMMFMetaDataEntry* testData=NULL;
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
            	INFO_PRINTF1(_L("iRecorder->GetMetaDataEntryL() Before Open State"));
            	//Gets specified metadata of the audio clip
				TRAPD(err, testData = iRecorder->GetMetaDataEntryL(0));
				if(err == iExpErrorWhenIdle) 
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Leaves with:%d"), err);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Failed,Leaves with:%d"), testData);
					StopTest(err);
					}
				//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName3);
     			TRAP(iError,iRecorder->OpenFileL(iFileName3));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("Opened State -----------"));
            	//Play starts
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError,iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying,KErrNone);
                break; 
            case EARStatePlaying:
                {
                CMMFMetaDataEntry* testData=NULL;
                INFO_PRINTF1(_L("Playing state -----------"));
               	INFO_PRINTF1(_L("iRecorder->GetMetaDataEntryL() Playing State For Controller"));
				//Gets specified metadata of the audio clip
				TRAP(iError,testData = iRecorder->GetMetaDataEntryL(0));
				if(iError == iExpErrorWhenPlaying) 
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Leaves with(KErrNotSupported):%d"),iError);
					}
				else
					{
					INFO_PRINTF3(_L("iRecorder->GetMetaDataEntryL() Failed,with error:%d, testData:%d"),iError,testData);
					StopTest(iError);
					}

				SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
                break;
                }
            case EARStatePlayStopped:
            	{
            	TUid format2;
            	INFO_PRINTF1(_L("iRecorder->DestinationFormatL()"));
				TRAP(iError,format2 = iRecorder->DestinationFormatL());
                if (format2 == TUid::Uid(KWavController)) 
                	{
	                //Recording started
	                INFO_PRINTF1(_L("iRecorder->RecordL()"));
	                TRAP(err, iRecorder->RecordL());
	                SetExpectedEventAndError(EARStateRecording, KErrNone);
                	}
                else
                	{
                	// Appending to an ogg-vorbis file, gives KErrUnderflow, so we don't record for Ogg-Vorbis. Test ends here...
                	INFO_PRINTF1(_L(""));
            		INFO_PRINTF1(_L("AUDIO CONTROLLER SUPPORTS METADATA "));
                	INFO_PRINTF1(_L(""));
                	StopTest(iError,EPass); //For OGG Controller
                	}
                break;
            	}
            case EARStateRecording:
                {
                CMMFMetaDataEntry* testData=NULL;
                
                INFO_PRINTF1(_L("Recording State -----------"));

           		INFO_PRINTF1(_L("GetMetaDataEntryL() in Recording State For Controller"));
				//Gets specified metadata of the audio clip
				TRAP(err, testData = iRecorder->GetMetaDataEntryL(0));
				if(err == iExpErrorWhenRecording) 
					{
					INFO_PRINTF3(_L("iRecorder->GetMetaDataEntryL() Leaves(testData):%d:%d"),testData,err);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Failed,Leaves with:%d"), err);
					StopTest(err);
					}

				//Call Timer iCallBack to Stop
				INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
            	iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped, KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
                TRAP(iError,iRecorder->Stop());
                
            	TUid format2;
            	INFO_PRINTF1(_L("iRecorder->DestinationFormatL()"));
				TRAP(iError,format2 = iRecorder->DestinationFormatL());
                if (format2 == TUid::Uid(KWavController)) 
                	{
                	INFO_PRINTF1(_L(""));
            		INFO_PRINTF1(_L("NOTE : AUDIO CONTROLLER DOES NOT SUPPORT METADATA !!! "));
                	INFO_PRINTF1(_L(""));
					StopTest(iError,EKnownFailure);  //For WAV Controller
                	}
                else
                	{
                	INFO_PRINTF1(_L(""));
            		INFO_PRINTF1(_L("OGG AUDIO CONTROLLER SUPPORTS METADATA "));
                	INFO_PRINTF1(_L(""));
                	StopTest(iError,EPass); //For OGG Controller
                	}
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0108, MM-MMF-ARCLNT-CHRTZ-I-0109
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0108, MM-MMF-ARCLNT-CHRTZ-AMR-I-0109
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0108, MM-MMF-ARCLNT-CHRTZ-AAC-I-0109
* 	Adds a meta data entry to an Audio file ,While Before Open and While in Recording State.
* 
* 	AddMetaDataEntryL()
*
**************************************************************************88*************/
// static NewL()
RTestAudioRecAddMetaDataEntB4OpenAndRec* RTestAudioRecAddMetaDataEntB4OpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecAddMetaDataEntB4OpenAndRec* self = new (ELeave) RTestAudioRecAddMetaDataEntB4OpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecAddMetaDataEntB4OpenAndRec::RTestAudioRecAddMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecAddMetaDataEntB4OpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.42 - AddMetaDataEntryL Before Open and While in recording an audio file."));
	iMetadata[0] = CMMFMetaDataEntry::NewL(name,data);
	iMetadata[1] = CMMFMetaDataEntry::NewL(name2,data2);

	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenIdle"), iExpErrorWhenIdle))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	if(!GetIntFromConfig(iSectName, _L("ExpErrorWhenRecording"), iExpErrorWhenRecording))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get expected error from config"));
		StopTest(KErrNone, EInconclusive);
		}
	}

void RTestAudioRecAddMetaDataEntB4OpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestAudioRecAddMetaDataEntB4OpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
            	INFO_PRINTF1(_L("iRecorder->AddMetaDataEntryL() Before Open State"));
				//Adding new metadata to the audio clip
				TRAPD(err, iRecorder->AddMetaDataEntryL(*iMetadata[0]));
				if(err == iExpErrorWhenIdle) 
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[0]) Leaves with %d"), err);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[0]) Failed,Leaves with:%d"), err);
					StopTest(err);
					}
				//Adding new metadata to the audio clip
				TRAPD(err1, iRecorder->AddMetaDataEntryL(*iMetadata[1]));
				if(err1 == iExpErrorWhenIdle) 
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[1]) Leaves with :%d"), err1);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[1]) Failed,Leaves with:%d"), err1);
					StopTest(err1);
					}
				//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("Opened State -----------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError, iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording, KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("Recording State -----------"));
            	INFO_PRINTF1(_L("iRecorder->AddMetaDataEntryL() Before Open State"));
				//Adding metadata to the audio clip
				TRAPD(err2, iRecorder->AddMetaDataEntryL(*iMetadata[0]));
				if(err2 == iExpErrorWhenRecording) 
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[0]) Leaves with(KErrNotSupported):%d"), err2);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[0]) Failed,Leaves with:%d"), err2);
					StopTest(err2);
					}
				//Adding metadata to the audio clip
				TRAPD(err3, iRecorder->AddMetaDataEntryL(*iMetadata[1]));
				if(err3 == iExpErrorWhenRecording) 
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[1]) Leaves with(KErrNotSupported):%d"), err3);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->AddMetaDataEntryL(*metaData[1]) Failed,Leaves with:%d"), err3);
					StopTest(err3);
					}
				//Call Timer iCallBack to Stop
				INFO_PRINTF1(_L("Call Timer iCallBack to Stop Recording"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped, KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("EARStateRecordStopped state ----------"));
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
                INFO_PRINTF1(_L(""));
            	INFO_PRINTF1(_L("NOTE : AUDIO CONTROLLER DOES NOT SUPPORT METADATA !!!"));
                INFO_PRINTF1(_L(""));
                INFO_PRINTF1(_L("KNOWN FAILURE - End of test"));
                StopTest(iError,EKnownFailure);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0111, MM-MMF-ARCLNT-CHRTZ-I-0112
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0111, MM-MMF-ARCLNT-CHRTZ-AMR-I-0112
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0111, MM-MMF-ARCLNT-CHRTZ-AAC-I-0112
* 	Removes a specified meta data entry from the audio clip.While Before Open and While in Recording State.
* 
* 	RemoveMetaDataEntry()
*
**************************************************************************88*************/
// static NewL()
RTestAudioRecRemvMetaDataEntB4OpenAndRec* RTestAudioRecRemvMetaDataEntB4OpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecRemvMetaDataEntB4OpenAndRec* self = new (ELeave) RTestAudioRecRemvMetaDataEntB4OpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecRemvMetaDataEntB4OpenAndRec::RTestAudioRecRemvMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecRemvMetaDataEntB4OpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.43 - RemoveMetaDataEntry Before Open and While in recording an audio file."));
	}

void RTestAudioRecRemvMetaDataEntB4OpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestAudioRecRemvMetaDataEntB4OpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
            	INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Before Open State"));
				TInt numEntries;
				//Gets number of metadata entries of the audio clip
				TInt err = iRecorder->GetNumberOfMetaDataEntries(numEntries);
				if(err != iExpErrorWhenIdle)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries returned error %d"), err);
					numEntries = 0; // so we don't try and remove anything 
					StopTest(err);
					}
				else
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries returned(KErrNotSupported) : %d"), err);
					}
				//Removing number of metadata entries of the audio clip	
				err = iRecorder->RemoveMetaDataEntry(0);	
				if(err != iExpErrorWhenIdle)
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry(%d) returned error: %d"), 1, err);
					StopTest(err);
					}
				else
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry (%d) returned (KErrNotSupported) : %d"), 1, err);
					}	
				//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("Opened State -----------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError, iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording, KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("Recording State -----------"));
            	INFO_PRINTF1(_L("iRecorder->GetNumberOfMetaDataEntries() Recording State"));
				TInt numEntries2;
				//Gets number of metadata entries of the audio clip
				TInt err2 = iRecorder->GetNumberOfMetaDataEntries(numEntries2);
				if(err2 == KErrNone)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries are: %d"), numEntries2);
					numEntries2 = 0; // so we don't try and remove anything 
					}
				else if(err2 == KErrNotSupported)
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries returned error(KErrNotSupported): %d"), err2);
					}
				else if(err2 == KErrNotReady) //Expected result for WAV file
					{
					INFO_PRINTF2(_L("GetNumberOfMetaDataEntries returned error(KErrNotReady): %d"), err2);
					}
				else
					{
					INFO_PRINTF2(_L("Error: %d"), err2);
					StopTest(err2);
					}
				//Remove number of metadata entries of the audio clip
				INFO_PRINTF1(_L("iRecorder->RemoveMetaDataEntry(0)"));
				err2 = iRecorder->RemoveMetaDataEntry(0);
				if(err2 == KErrNone)
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry (%d) returned : %d"), 1, err2);
					}
				else if ( err2 == KErrNotSupported ) //Expected result for OGG file
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry (%d) returned error(KErrNotSupported): %d"), 1, err2);
					}
				else if(err2 == KErrNotReady) //Expected result for WAV file
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry (%d) returned error(KErrNotReady): %d"), 1, err2);
					}
				else
					{
					INFO_PRINTF2(_L(" Unknown Error !!!!! : %d "),err2);
					StopTest(err2);
					}
				//Call Timer iCallBack to Stop
				INFO_PRINTF1(_L("Stopping by Timer"));
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped, KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
            	INFO_PRINTF1(_L(""));
            	INFO_PRINTF1(_L("NOTE : AUDIO CONTROLLER DOES NOT SUPPORT METADATA !!!"));
                INFO_PRINTF1(_L(""));
                INFO_PRINTF1(_L("KNOWN FAILURE - End of test"));
                StopTest(iError,EKnownFailure);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0114, MM-MMF-ARCLNT-CHRTZ-I-0115
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0114, MM-MMF-ARCLNT-CHRTZ-AMR-I-0115
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0114, MM-MMF-ARCLNT-CHRTZ-AAC-I-0115
* 	Replaces the specified meta data entry with a new entry.While Before Open and While in Recording State.
* 
* 	ReplaceMetaDataEntryL()
*
***************************************************************************************/
// static NewL()
RTestAudioRecRepMetaDataEntB4OpenAndRec* RTestAudioRecRepMetaDataEntB4OpenAndRec::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecRepMetaDataEntB4OpenAndRec* self = new (ELeave) RTestAudioRecRepMetaDataEntB4OpenAndRec(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecRepMetaDataEntB4OpenAndRec::RTestAudioRecRepMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecRepMetaDataEntB4OpenAndRec::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.44 - ReplaceMetaDataEntry Before Open and While in recording an audio file."));
	}

void RTestAudioRecRepMetaDataEntB4OpenAndRec::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	iState = EARStateRecordStopped;
	TRAP(iError, Fsm(EARStateRecordStopped));
    if (iError!=KErrNone)
		{
		StopTest (iError);	
		}
	}

void RTestAudioRecRepMetaDataEntB4OpenAndRec::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	{
            	CMMFMetaDataEntry* testData=NULL;
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
            	INFO_PRINTF1(_L("iRecorder->GetMetaDataEntryL() Before Open State"));
				//Gets meta data of the audio clip
				TRAPD(err, testData = iRecorder->GetMetaDataEntryL(0));
				if(err == KErrNotSupported) 
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Leaves with(KErrNotSupported):%d"), err);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->GetMetaDataEntryL() Failed,Leaves with:%d"), testData);
					StopTest(err);
					}
				//Replace the metadata of the audio clip
            	INFO_PRINTF1(_L("iRecorder->ReplaceMetaDataEntryL(0,*testData)) Before Open State"));
				TRAPD(err2,iRecorder->ReplaceMetaDataEntryL(0,*testData));
				if(err2 != KErrNotSupported)
					{
					INFO_PRINTF2(_L("ReplaceMetaDataEntry returned error %d"), err2);
					StopTest(err2);
					}
				else
					{
					INFO_PRINTF2(_L("ReplaceMetaDataEntry returned(KErrNotSupported) : %d"), err2);
					}
				//Remove metadata of the audio clip	
				err = iRecorder->RemoveMetaDataEntry(0);	
				if(err != KErrNotSupported)
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry(%d) returned error: %d"), 1, err);
					StopTest(err);
					}
				else
					{
					INFO_PRINTF3(_L("RemoveMetaDataEntry (%d) returned (KErrNotSupported) : %d"), 1, err);
					}	
				//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName);
     			TRAP(iError,iRecorder->OpenFileL(iFileName));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            	}
            case EARStateOpened:
            	INFO_PRINTF1(_L("Opened State -----------"));
                // Recording started
                INFO_PRINTF1(_L("iRecorder->RecordL()"));
                TRAP(iError, iRecorder->RecordL());
                SetExpectedEventAndError(EARStateRecording, KErrNone);
                break;
            case EARStateRecording:
                {
                INFO_PRINTF1(_L("Recording State -----------"));
                INFO_PRINTF1(_L("iRecorder->GetMetaDataEntryL() Recording State"));
				CMMFMetaDataEntry* testData1=NULL;
				//Gets metadata of the audio clip
				TRAPD(err3, testData1 = iRecorder->GetMetaDataEntryL(0));
				if(err3 == iExpError) 
					{
					INFO_PRINTF2(_L("GetMetaDataEntryL() Leaves with expected error (%d)"), err3);
					}
				else if (err3 == KErrNone)
					{
					INFO_PRINTF1(_L("GetMetaDataEntryL() did not Leave"));
					}
				else
					{
					INFO_PRINTF2(_L("GetMetaDataEntryL() Failed, Leaves with : %d"), testData1);
					StopTest(err3);
					}
				INFO_PRINTF1(_L("ReplaceMetaDataEntryL(0,*testData1)) in Recording State"));
				INFO_PRINTF1(_L("Can't relace a non-existent entry, need to use AddMetaDataL() instead"));

				//Call Timer iCallBack to Stop
				iTimer->Start(KFourSec, 0, iCallBack);
            	SetExpectedEventAndError(EARStateRecordStopped, KErrNone);
                break;
                }
            case EARStateRecordStopped:
            	{
            	INFO_PRINTF1(_L("iRecorder->Stop()"));
            	TRAP(iError,iRecorder->Stop());
        		
            	INFO_PRINTF1(_L(""));
        		INFO_PRINTF1(_L("OGG / WAV AUDIO CONTROLLER DOES NOT SUPPORT REPLACE METADATA"));
            	INFO_PRINTF1(_L(""));
            	INFO_PRINTF1(_L("KNOWN FAILURE - End of test"));
				StopTest(iError, EKnownFailure);
                break;
            	}
            }
        }
	else 
		{
		StopTest(iError);
		}
	}

/****************************************************************************************
*	MM-MMF-ARCLNT-CHRTZ-I-0124
*	MM-MMF-ARCLNT-CHRTZ-AMR-I-0124
*	MM-MMF-ARCLNT-CHRTZ-AAC-I-0124
* 	Waits for the client to resume the play even after the default timer expires.
* 	While in Open State
* 	WillResumePlay()
*
**************************************************************************88*************/
RTestAudioRecWillResumePlayInOpen* RTestAudioRecWillResumePlayInOpen::NewL(const TDesC& aStepName, const TDesC& aFilename)
	{
	RTestAudioRecWillResumePlayInOpen* self = new (ELeave) RTestAudioRecWillResumePlayInOpen(aStepName, aFilename);
	return self;
	}

// private constructor
RTestAudioRecWillResumePlayInOpen::RTestAudioRecWillResumePlayInOpen(const TDesC& aStepName, const TDesC& aFileName) :
	RAudioRecTest(aStepName, aFileName)
	{
	}
	
// start test
void RTestAudioRecWillResumePlayInOpen::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE-> RECUTILITY1.47 - WillResumePlay while in Open an audio file."));
	}

void RTestAudioRecWillResumePlayInOpen::Fsm(TAudioRecorderState iState)
	{
    if (ValidateState())
        {
        switch(iState)
            {
            case EARStateIdle:
            	INFO_PRINTF1(_L("State is EARStateIdle -----------"));
				//Opening an audio file
            	INFO_PRINTF2(_L("iRecorder->OpenFileL() %S"), &iFileName2);
     			TRAP(iError,iRecorder->OpenFileL(iFileName2));
     			SetExpectedEventAndError(EARStateOpened,KErrNone);
                break;
            case EARStateOpened:
            	{
            	INFO_PRINTF1(_L("Opened State -----------"));
            	INFO_PRINTF1(_L("iRecorder->WillResumePlay()"));
                TInt retErr = iRecorder->WillResumePlay();
				if(retErr == KErrNone)
					{
					INFO_PRINTF2(_L("iRecorder->WillResumePlay return :%d"),retErr);
					}
				else
					{
					INFO_PRINTF2(_L("iRecorder->WillResumePlay Failed return :%d"),retErr);
					StopTest(retErr);
					}
                // Recording started
                INFO_PRINTF1(_L("iRecorder->PlayL()"));
                TRAP(iError, iRecorder->PlayL());
                SetExpectedEventAndError(EARStatePlaying, KErrNone);
                break;
            	}
            case EARStatePlaying:
                INFO_PRINTF1(_L("Playing State -----------"));
            	SetExpectedEventAndError(EARStatePlayStopped, KErrNone);
                break;
            case EARStatePlayStopped:
                INFO_PRINTF1(_L("Play Completed - End of test"));
                StopTest(iError);
                break;
            }
        }
	else 
		{
		StopTest(iError);
		}
	}
