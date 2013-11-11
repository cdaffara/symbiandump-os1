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
#include "char_videoplay_common.h"
#include <badesca.h>

#ifdef SYMBIAN_BUILD_GCE
#include <gceavailable.h>
#endif

const TInt KPlayStartTime = 1000000;
const TInt KPlayEndTime = 2000000;
const TInt KDelay = 1000000; // 1s

const TReal32 KScaleWidth = 1.5f;
const TReal32 KScaleHeight = 1.65f;

const TInt KCropRegionRectTop = 1;
const TInt KCropRegionRectLeft =3;
const TInt KCropRegionRectRight = 5;
const TInt KCropRegionRectBottom = 7;
_LIT(KVideoTestControllerName, "AviVideoPlayController");


// Open a video file
// MM-MMF-VCLNT-CHRTZ-I-0001
// static NewL()
RVPlyrOpenFileTest* RVPlyrOpenFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVPlyrOpenFileTest* self = new (ELeave) RVPlyrOpenFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVPlyrOpenFileTest::RVPlyrOpenFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVPlyrOpenFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# VIDEOPLAYERUTILITY 1.0 - Opening a video file"));
	}

/**
 * RVPlyrOpenFileTest::Fsm
 */
void RVPlyrOpenFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                     StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                StopTest(iError, EPass);  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Open a video file with Invalid format
// MM-MMF-VCLNT-CHRTZ-I-0002,0002a,0002b,0003
// static NewL()
RVidPlyrOpnFileInvFmtTest* RVidPlyrOpnFileInvFmtTest::NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpError)
	{
	RVidPlyrOpnFileInvFmtTest* self = new (ELeave) RVidPlyrOpnFileInvFmtTest(aStepName, aSectName, aKeyName, aExpError);
	return self;
	}

// private constructor
RVidPlyrOpnFileInvFmtTest::RVidPlyrOpnFileInvFmtTest(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpError) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	iKeyName = aKeyName;
	iExpError = aExpError;
	}
	
// start test
void RVidPlyrOpnFileInvFmtTest::DoKickoffTestL()
	{
	TPtrC invalidfilename;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.0  - Open a video file with Invalid format"));
	
	//Getting invalidfilename from ini file
	if(!GetStringFromConfig(iSectName, iKeyName, invalidfilename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(invalidfilename);
	}

/**
 * RVidPlyrOpnFileInvFmtTest::Fsm
 */
void RVidPlyrOpnFileInvFmtTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                     StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, iExpError);
                break;
            case EVPOpenComplete:
                StopTest(iError, EPass);  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Open a video file with controller Uid
// MM-MMF-VCLNT-CHRTZ-I-0004
// static NewL()
RVidPlyrOpnFileCntrlrUidTest* RVidPlyrOpnFileCntrlrUidTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrOpnFileCntrlrUidTest* self = new (ELeave) RVidPlyrOpnFileCntrlrUidTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrOpnFileCntrlrUidTest::RVidPlyrOpnFileCntrlrUidTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrOpnFileCntrlrUidTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.0  - Open a video file with Controller Uid"));
	}

/**
 * RVidPlyrOpnFileCntrlrUidTest::Fsm
 */
void RVidPlyrOpnFileCntrlrUidTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename, KMmfTestAviPlayControllerUid));
                if (iError!= KErrNone)
                    {
                     StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Open a video file with Invalid controller Uid
// MM-MMF-VCLNT-CHRTZ-I-0005
// static NewL()
RVidPlyrOpnFileInvCntrlrUidTest* RVidPlyrOpnFileInvCntrlrUidTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrOpnFileInvCntrlrUidTest* self = new (ELeave) RVidPlyrOpnFileInvCntrlrUidTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrOpnFileInvCntrlrUidTest::RVidPlyrOpnFileInvCntrlrUidTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrOpnFileInvCntrlrUidTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.0  - Open a video file with Invalid Controller Uid"));
	}

/**
 * RVidPlyrOpnFileInvCntrlrUidTest::Fsm
 */
void RVidPlyrOpnFileInvCntrlrUidTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename, KMmfTestAviRecordControllerUid));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNotSupported);
                break;
            case EVPOpenComplete:
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// MM-MMF-VCLNT-CHRTZ-I-0006
// Play a video file
// static NewL()
RVideoPlayTest* RVideoPlayTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVideoPlayTest* self = new (ELeave) RVideoPlayTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVideoPlayTest::RVideoPlayTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVideoPlayTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.1  - play a video file"));
	}

/**
 * RVideoPlayTest::Fsm
 */
void RVideoPlayTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iUtility
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play iUtility
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNone);
                break;
            case EVPPlayComplete:
            	// nothing to do, stop the test, default is EPass
                StopTest(iError);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play a video file while video is playing
// MM-MMF-VCLNT-CHRTZ-I-0007
// static NewL()
RVidPlyrPlayAgainTest* RVidPlyrPlayAgainTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayAgainTest* self = new (ELeave) RVidPlyrPlayAgainTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayAgainTest::RVidPlyrPlayAgainTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayAgainTest::DoKickoffTestL()
	{
	TPtrC longvidfile;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.1  - Play a video file multiple times"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("longvidfile"), longvidfile))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(longvidfile);
	}

void RVidPlyrPlayAgainTest::DoTimerCallback()
	{
	#ifndef __EABI__
	INFO_PRINTF1(_L("Timer event occured"));
	#endif
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}

/**
 * RVidPlyrPlayAgainTest::Fsm
 */
void RVidPlyrPlayAgainTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);                
                break;
            case EVPPlaying1:
                iTimer->Cancel();
                #ifndef __EABI__
                INFO_PRINTF1(_L("iUtility->Play() again"));
                #endif
                iUtility->Play();
                iState = EVPPlayResumed;
                SetExpectedState(EVPPlayComplete, KErrNotReady);
                break;
            case EVPPlayComplete:
                INFO_PRINTF1(_L("Playing a video file in playing state recevies KErrNotReady through MvpuoPlayComplete"));            
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play a video file without opening it
// MM-MMF-VCLNT-CHRTZ-I-0008

// static NewL()
RVidPlyrPlayFileWithoutOpenTest* RVidPlyrPlayFileWithoutOpenTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayFileWithoutOpenTest* self = new (ELeave) RVidPlyrPlayFileWithoutOpenTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayFileWithoutOpenTest::RVidPlyrPlayFileWithoutOpenTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayFileWithoutOpenTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.1  - Play a video file without opening it"));
	}

/**
 * RVidPlyrPlayFileWithoutOpenTest::Fsm
 */
void RVidPlyrPlayFileWithoutOpenTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNotReady);
                // Prepare the clips without opening the file 
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                break;
            case EVPPrepareComplete:
                //  Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNotReady);
                break;
            case EVPPlayComplete:
                StopTest(iError, EFail);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play a video file without preparing the clips 
// MM-MMF-VCLNT-CHRTZ-I-0009
// static NewL()
RVidPlyrPlayWithoutPrepareTest* RVidPlyrPlayWithoutPrepareTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayWithoutPrepareTest* self = new (ELeave) RVidPlyrPlayWithoutPrepareTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayWithoutPrepareTest::RVidPlyrPlayWithoutPrepareTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayWithoutPrepareTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.1  - Play the video file without Prepare"));
	}

/**
 * RVidPlyrPlayWithoutPrepareTest::Fsm
 */
void RVidPlyrPlayWithoutPrepareTest::Fsm()
    {
   if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Play iUtility
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNone);
                break;
            case EVPPlayComplete:
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EPass);  
                    }
                else
                    {
                    INFO_PRINTF1(_L("Shouldn't play video as video clip is not prepared."));
                    StopTest(iError, EFail);   
                    }
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play a video file with start and end points
// MM-MMF-VCLNT-CHRTZ-I-0010
// static NewL()
RVidPlyrPlayFileStEndptsTest* RVidPlyrPlayFileStEndptsTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayFileStEndptsTest* self = new (ELeave) RVidPlyrPlayFileStEndptsTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayFileStEndptsTest::RVidPlyrPlayFileStEndptsTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayFileStEndptsTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Open a video file with start and End points"));
	}

/**
 * RVidPlyrPlayFileStEndptsTest::Fsm
 */
void RVidPlyrPlayFileStEndptsTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play(TTimeIntervalMicroSeconds(KPlayStartTime), TTimeIntervalMicroSeconds(KPlayEndTime));
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNotSupported);
                break;
            case EVPPlayComplete:
                INFO_PRINTF1(_L("Controller doesn't support seeking"));            
                StopTest(iError, EKnownFailure);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Stop playing video while video file is playing
// MM-MMF-VCLNT-CHRTZ-I-0011
// static NewL()
RVidPlyrStopVidPlayTest* RVidPlyrStopVidPlayTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStopVidPlayTest* self = new (ELeave) RVidPlyrStopVidPlayTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStopVidPlayTest::RVidPlyrStopVidPlayTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStopVidPlayTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.2 - Stop playing video while video file is playing"));
	}

void RVidPlyrStopVidPlayTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    //INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPStopped;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrStopVidPlayTest::Fsm
 */
void RVidPlyrStopVidPlayTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                iTimer->Cancel();
    		    INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Stop playing video in paused state
// MM-MMF-VCLNT-CHRTZ-I-0012
// static NewL()
RVidPlyrStopVidPlayInPauseTest* RVidPlyrStopVidPlayInPauseTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStopVidPlayInPauseTest* self = new (ELeave) RVidPlyrStopVidPlayInPauseTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStopVidPlayInPauseTest::RVidPlyrStopVidPlayInPauseTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStopVidPlayInPauseTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.2  - Stop playing video in paused state"));
	}

void RVidPlyrStopVidPlayInPauseTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    //INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
/**
 * RVidPlyrStopVidPlayInPauseTest::Fsm
 */
void RVidPlyrStopVidPlayInPauseTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPaused, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPaused: 
                iTimer->Cancel();
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                else
                    {
                    //Stop video play
                    INFO_PRINTF1(_L("iUtility->Stop()"));
                    iUtility->Stop();
                    StopTest(iError, EPass);  
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Stop playing video after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0013
// static NewL()
RVidPlyrStopAftrOpnFileTest* RVidPlyrStopAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStopAftrOpnFileTest* self = new (ELeave) RVidPlyrStopAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStopAftrOpnFileTest::RVidPlyrStopAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStopAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.2  - Stop playing after opening video file"));
	}

/**
 * RVidPlyrStopAftrOpnFileTest::Fsm
 */
void RVidPlyrStopAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call Stop() 
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Pause video play while file is playing 
// MM-MMF-VCLNT-CHRTZ-I-0014
// static NewL()
RVidPlyrPauseVidPlayFilPlngTest* RVidPlyrPauseVidPlayFilPlngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPauseVidPlayFilPlngTest* self = new (ELeave) RVidPlyrPauseVidPlayFilPlngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPauseVidPlayFilPlngTest::RVidPlyrPauseVidPlayFilPlngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPauseVidPlayFilPlngTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.3  - Pause video play while file is playing"));
	}

// Timer Callback
void RVidPlyrPauseVidPlayFilPlngTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}

/**
 * RVidPlyrPauseVidPlayFilPlngTest::Fsm
 */
void RVidPlyrPauseVidPlayFilPlngTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPaused, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPaused:
                iTimer->Cancel(); 
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
 // Pause video play after opening the file 
// MM-MMF-VCLNT-CHRTZ-I-0015
// static NewL()
RVidPlyrPauseVidPlayAftrOpnFilTest* RVidPlyrPauseVidPlayAftrOpnFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPauseVidPlayAftrOpnFilTest* self = new (ELeave) RVidPlyrPauseVidPlayAftrOpnFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPauseVidPlayAftrOpnFilTest::RVidPlyrPauseVidPlayAftrOpnFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPauseVidPlayAftrOpnFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.3  - Pause video play after opening the  file"));
	}

/**
 * RVidPlyrPauseVidPlayAftrOpnFilTest::Fsm
 */
void RVidPlyrPauseVidPlayAftrOpnFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Pause video play after stop
// MM-MMF-VCLNT-CHRTZ-I-0016
// static NewL()
RVidPlyrPauseVidPlayAftrStopTest* RVidPlyrPauseVidPlayAftrStopTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPauseVidPlayAftrStopTest* self = new (ELeave) RVidPlyrPauseVidPlayAftrStopTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPauseVidPlayAftrStopTest::RVidPlyrPauseVidPlayAftrStopTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPauseVidPlayAftrStopTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.3  - Pause video play after stop"));
	}

// Timer Callback
void RVidPlyrPauseVidPlayAftrStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event has occured"));		
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPStopped;
			Fsm();
			break; 
		case EVPStopped:
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrPauseVidPlayAftrStopTest::Fsm
 */
void RVidPlyrPauseVidPlayAftrStopTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                // Stop the video file
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                iState = EVPStopped;
                SetExpectedState(EVPPaused, KErrNone);
                break;
            case EVPPaused:
                iTimer->Cancel(); 
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
  
 // Pause video play before opening the video file
// MM-MMF-VCLNT-CHRTZ-I-0017
// static NewL()
RVidPlyrPauseVidPlayB4OpngFilTest* RVidPlyrPauseVidPlayB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPauseVidPlayB4OpngFilTest* self = new (ELeave) RVidPlyrPauseVidPlayB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPauseVidPlayB4OpngFilTest::RVidPlyrPauseVidPlayB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPauseVidPlayB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.3  - Pause video play before opening the file"));
	}

/**
 * RVidPlyrPauseVidPlayB4OpngFilTest::Fsm
 */
void RVidPlyrPauseVidPlayB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);
                    }
                else
                    {
                    INFO_PRINTF2(_L("PauseL() called before opening the file leaves with %d"),iError);
                    StopTest(iError, EPass);   
                    }
                break;  
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set volume after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0018
// static NewL()
RVidPlyrSetVolAftrOpngVidFilTest* RVidPlyrSetVolAftrOpngVidFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolAftrOpngVidFilTest* self = new (ELeave) RVidPlyrSetVolAftrOpngVidFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolAftrOpngVidFilTest::RVidPlyrSetVolAftrOpngVidFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolAftrOpngVidFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4 - Set volume after opening video file"));
	}

/**
 * RVidPlyrSetVolAftrOpngVidFilTest::Fsm
 */
void RVidPlyrSetVolAftrOpngVidFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TInt volume;	
                INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                volume = iUtility->MaxVolume(); 
                TRAP(iError, iUtility->SetVolumeL(volume));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TInt volume1;
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    volume1 = iUtility->Volume();
                    if (volume1 == volume)
                        {
                        StopTest(iError, EPass);     
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set volume during video play
// MM-MMF-VCLNT-CHRTZ-I-0019
// static NewL()
RVidPlyrSetVolDuringPlayTest* RVidPlyrSetVolDuringPlayTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolDuringPlayTest* self = new (ELeave) RVidPlyrSetVolDuringPlayTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolDuringPlayTest::RVidPlyrSetVolDuringPlayTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolDuringPlayTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4  - Set volume during play"));
	}

// Timer Callback
void RVidPlyrSetVolDuringPlayTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
/**
 * RVidPlyrSetVolDuringPlayTest::Fsm
 */
void RVidPlyrSetVolDuringPlayTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                iTimer->Cancel(); 
                INFO_PRINTF1(_L("iUtility->SetVolumeL() - Set the volume to maxvolume"));
                TInt volume = iUtility->MaxVolume();
                TRAP(iError, iUtility->SetVolumeL(volume));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TInt volume1;
                    // Get the volume
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    volume1 = iUtility->Volume();
                    if(volume1 == volume)
                        {
                        StopTest(iError, EPass);     
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set volume in paused state
// MM-MMF-VCLNT-CHRTZ-I-0020
// static NewL()
RVidPlyrSetVolInPauseTest* RVidPlyrSetVolInPauseTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolInPauseTest* self = new (ELeave) RVidPlyrSetVolInPauseTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolInPauseTest::RVidPlyrSetVolInPauseTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolInPauseTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4  - Set volume in paused state"));
	}

// Timer Callback
void RVidPlyrSetVolInPauseTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
/**
 * RVidPlyrSetVolInPauseTest::Fsm
 */
void RVidPlyrSetVolInPauseTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPaused, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPaused:
                iTimer->Cancel(); 
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                else
                    {
                    INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                    TInt volume = iUtility->MaxVolume();
                    TRAP(iError, iUtility->SetVolumeL(volume));
                    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);  
                        }
                    else
                        {
                        TInt volume1;
                        // Get the volume
                        INFO_PRINTF1(_L("iUtility->Volume()"));
                        volume1 = iUtility->Volume();
                        if(volume1 == volume)
                            {
                            StopTest(iError, EPass);    
                            }
                        }   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set volume in stopped state
// MM-MMF-VCLNT-CHRTZ-I-0021
// static NewL()
RVidPlyrSetVolInStopTest* RVidPlyrSetVolInStopTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolInStopTest* self = new (ELeave) RVidPlyrSetVolInStopTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolInStopTest::RVidPlyrSetVolInStopTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolInStopTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4  - Set volume in stopped state"));
	}

// Timer Callback
void RVidPlyrSetVolInStopTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer enabled"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPStopped;
			Fsm();
			break; 
		case EVPStopped:
            iState = EVPStopped1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrSetVolInStopTest::Fsm
 */
void RVidPlyrSetVolInStopTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                iState = EVPStopped;
                SetExpectedState(EVPStopped1, KErrNone);
                break;
            case EVPStopped1:
                iTimer->Cancel(); 
                INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                TInt volume = iUtility->MaxVolume(); 
                TRAP(iError, iUtility->SetVolumeL(volume));
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF1(_L("Volume set is unsuccessful in stopped state"));
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF1(_L("Volume set is successful in stopped state"));
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    TInt volume1 = iUtility->Volume();
                    if (volume1 == volume)
                        {
                        StopTest(iError, EPass);     
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set volume in a video only file
// MM-MMF-VCLNT-CHRTZ-I-0022

// static NewL()
RVidPlyrSetVolAftrOpngVidOnlyFilTest* RVidPlyrSetVolAftrOpngVidOnlyFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolAftrOpngVidOnlyFilTest* self = new (ELeave) RVidPlyrSetVolAftrOpngVidOnlyFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolAftrOpngVidOnlyFilTest::RVidPlyrSetVolAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolAftrOpngVidOnlyFilTest::DoKickoffTestL()
	{
	TPtrC vidonlyfilname;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4 - Set volume after opening a video only file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("vidonlyfile"), vidonlyfilname))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(vidonlyfilname);
	}

/**
 * RVidPlyrSetVolAftrOpngVidOnlyFilTest::Fsm
 */
void RVidPlyrSetVolAftrOpngVidOnlyFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->AudioEnabledL()"));
                TBool audEnabled;
                TRAP(iError, audEnabled = iUtility->AudioEnabledL());
                if (audEnabled)
                    {
                    ERR_PRINTF1(_L("Audio is enabled in this file"));
                    StopTest(iError, EFail);
                    }
                else
                    {
                    INFO_PRINTF1(_L("Audio is not enabled in this file"));
                    TInt volume;	
                    INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                    volume = iUtility->MaxVolume(); 
                    TRAP(iError, iUtility->SetVolumeL(volume));
                    if (iError!= KErrNone)
                        {
                        INFO_PRINTF2(_L("iUtility->SetVolumeL() leaves with %d"), iError);
                        StopTest(iError, EPass);  
                        }
                    else
                        {
                        TInt volume1;
                        INFO_PRINTF1(_L("iUtility->Volume()"));
                        volume1 = iUtility->Volume();
                        if(volume1 == volume)
                            {
                            StopTest(iError, EPass);    
                            }
                        else
                            {
                            StopTest(iError, EFail);       
                            }                       
                        }   
                    } 
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
  
/* Volume to be set with some invalid parameter
 (less than 0 and greater than maximum volume)
 during playing the video file */
// MM-MMF-VCLNT-CHRTZ-I-0023
// static NewL()
RVidPlyrSetVolWithInvPmtsTest* RVidPlyrSetVolWithInvPmtsTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVolWithInvPmtsTest* self = new (ELeave) RVidPlyrSetVolWithInvPmtsTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVolWithInvPmtsTest::RVidPlyrSetVolWithInvPmtsTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVolWithInvPmtsTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.4 - Set volume during playing video file with Invalid parameter"));
	}
	
// Timer Callback
void RVidPlyrSetVolWithInvPmtsTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}

/**
 * RVidPlyrSetVolWithInvPmtsTest::Fsm
 */
void RVidPlyrSetVolWithInvPmtsTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                iTimer->Cancel(); 
                TInt volume, volume1;
                TBool valid;	
                INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                volume = iUtility->MaxVolume();
                volume1 = volume + 100;
                INFO_PRINTF3(_L("volume = %d , volume1 = %d"), volume, volume1);
                TRAP(iError, iUtility->SetVolumeL(volume));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TInt volume2;
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    volume2 = iUtility->Volume();
                    if (volume2 == volume)
                        {
                        INFO_PRINTF2(_L("volume2 = %d"), volume2);
                        valid = ETrue;
                        //StopTest(iError, EPass);     
                        }
                    else
                        {
                        INFO_PRINTF2(_L("volume2 = %d"), volume2);
                        INFO_PRINTF1(_L("Set and get value doesn't match"));
                        StopTest(iError, EFail);        
                        }
                    }
                TRAP(iError, iUtility->SetVolumeL(-1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TInt volume4;
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    volume4 = iUtility->Volume();
                    if ((volume4 == 0) && valid)
                        {
                        INFO_PRINTF2(_L("volume4 = %d"), volume4);
                        StopTest(iError, EPass);     
                        }
                    else
                        {
                        INFO_PRINTF2(_L("volume4 = %d"), volume4);
                        INFO_PRINTF1(_L("Set and get value doesn't match"));
                        StopTest(iError, EFail);        
                        }   
                    }    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
 // Get maximum volume in different states
// MM-MMF-VCLNT-CHRTZ-I-0024
// static NewL()
RVidPlyrMaxVolInDfrntStTest* RVidPlyrMaxVolInDfrntStTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrMaxVolInDfrntStTest* self = new (ELeave) RVidPlyrMaxVolInDfrntStTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrMaxVolInDfrntStTest::RVidPlyrMaxVolInDfrntStTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrMaxVolInDfrntStTest::DoKickoffTestL()
	{
	iValid = EFalse;
    iValid1 = EFalse;		
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.5  - Get max volume in different states"));
	}

// Timer Callback
void RVidPlyrMaxVolInDfrntStTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer Event Occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		case EVPPlaying2:
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrMaxVolInDfrntStTest::Fsm
 */
void RVidPlyrMaxVolInDfrntStTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                {
                TInt volume = iUtility->MaxVolume();
                INFO_PRINTF2(_L("MaxVolume returned = %d"), volume);
                if (volume == 0) // default is 0 - no-op
                    {
                    iValid = ETrue;
                    }
                else
                    {
                    iValid = EFalse;   
                    }
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);    
                }
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                //iTimer->Cancel();
                TInt volume1; 
                volume1 = iUtility->MaxVolume();
                INFO_PRINTF2(_L("volume returned = %d"), volume1);
                if (volume1)
                    {
                    iValid1 = ETrue;   
                    }
                else
                    {
                    iValid1 = EFalse;   
                    }
                iState = EVPPlaying2;
                SetExpectedState(EVPPaused, KErrNone);
                break;
            case EVPPaused:
                iTimer->Cancel();
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF1(_L("PauseL() is not supported"));
                    if(iValid && iValid1 && (iError == KErrNotSupported))
                        {
                        StopTest(iError, EPass);
                        }
                    else
                        {
                        StopTest(iError, EFail);
                        }
                    }
                else
                    {
                    //Make the test fail
                    TInt volume1; 
                    volume1 = iUtility->MaxVolume();
                    INFO_PRINTF2(_L("volume returned = %d"), volume1);
                    StopTest(iError, EPass);  
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Query Audio enabled or not in different(play and stopped) states
// MM-MMF-VCLNT-CHRTZ-I-0025
// static NewL()
RVidPlyrAudEnbldInDfrntStTest* RVidPlyrAudEnbldInDfrntStTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrAudEnbldInDfrntStTest* self = new (ELeave) RVidPlyrAudEnbldInDfrntStTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrAudEnbldInDfrntStTest::RVidPlyrAudEnbldInDfrntStTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrAudEnbldInDfrntStTest::DoKickoffTestL()
	{
	iValid = ETrue;
    iValid1 = ETrue;	
    iValid2 = ETrue;		
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.6  - Query audio is enabled or not in different(playing and stopped) states"));
	}

// Timer Callback
void RVidPlyrAudEnbldInDfrntStTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer Event Occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		case EVPPlaying2:
            iState = EVPStopped;
			Fsm();
			break;
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrAudEnbldInDfrntStTest::Fsm
 */
void RVidPlyrAudEnbldInDfrntStTest::Fsm()
    {
	TBool audEnabld = EFalse;
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                //TBool audEnabld;
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() after opening file"));
                TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF2(_L("AudioEnabledL() leaves with %d after opening file"), iError);
                    iValid = EFalse;
                    } 
                if (!audEnabld)
                    {
                    iValid = EFalse;
                    INFO_PRINTF1(_L("Audio is not enabled"));
                    StopTest(iError, EFail);  
                    }            
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
				//TBool audEnabld;
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() during play"));
                TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    } 
                if (!audEnabld)
                    {
                    iValid1 = EFalse;
                    INFO_PRINTF1(_L("Audio is not enabled"));
                    StopTest(iError, EFail);  
                    }
                iState = EVPPlaying2;
                SetExpectedState(EVPStopped, KErrNone);
                break;
            case EVPStopped:
                iTimer->Cancel();            
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() in stopped state"));
                TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                if (!audEnabld)
                    {
                    iValid2 = EFalse;
                    INFO_PRINTF1(_L("Audio is not enabled"));
                    StopTest(iError, EFail);  
                    }                
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                if(iValid && iValid1 && iValid2)
                    {
                    StopTest(iError, EPass);
                    }
                else
                    {
                    StopTest(iError, EFail);
                    }
                break;
             }
        }                     
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Query Audio enabled or not in paused state
// MM-MMF-VCLNT-CHRTZ-I-0025a
// static NewL()
RVidPlyrAudEnbldInPausedStTest* RVidPlyrAudEnbldInPausedStTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrAudEnbldInPausedStTest* self = new (ELeave) RVidPlyrAudEnbldInPausedStTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrAudEnbldInPausedStTest::RVidPlyrAudEnbldInPausedStTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrAudEnbldInPausedStTest::DoKickoffTestL()
	{
	iValid = ETrue;
    iValid1 = ETrue;	
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.6  - Query audio is enabled or not in paused state"));
	}

// Timer Callback
void RVidPlyrAudEnbldInPausedStTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer Event Occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		case EVPPlaying2:
            iState = EVPPaused;
			Fsm();
			break;
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrAudEnbldInPausedStTest::Fsm
 */
void RVidPlyrAudEnbldInPausedStTest::Fsm()
    {
	TBool audEnabld = EFalse;
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                //TBool audEnabld;
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() after opening file"));
                TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF2(_L("AudioEnabledL() leaves with %d after opening file"), iError);
                    iValid = EFalse;
                    } 
                if (!audEnabld)
                    {
                    iValid = EFalse;
                    INFO_PRINTF1(_L("Audio is not enabled"));
                    StopTest(iError, EFail);  
                    }            
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
				//TBool audEnabld;
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() during play"));
                TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    } 
                if (!audEnabld)
                    {
                    iValid1 = EFalse;
                    INFO_PRINTF1(_L("Audio is not enabled"));
                    StopTest(iError, EFail);  
                    }
                iState = EVPPlaying2;
                SetExpectedState(EVPPaused, KErrNone);
                break;
            case EVPPaused:
                iTimer->Cancel();
                // Pause the video play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF1(_L("PauseL() is not supported"));
                    if(iValid && iValid1 && (iError == KErrNotSupported))
                        {
                        StopTest(iError, EPass);
                        }
                    else
                        {
                        StopTest(iError, EFail);
                        }
                    }
                else
                    {
                    //TBool audEnabld;
                    INFO_PRINTF1(_L("iUtility->AudioEnabledL()"));
                    TRAP(iError, audEnabld = iUtility->AudioEnabledL());
                    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);  
                        } 
                    if (!audEnabld)
                        {
                        INFO_PRINTF1(_L("Audio is not enabled"));
                        StopTest(iError, EFail);  
                        }
                    //Make the test Pass after implementation of PauseL();
                    StopTest(iError, EPass);  
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Query Audio enabled or not before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0026
// static NewL()
RVidPlyrAudEnbldBfrOpngFilTest* RVidPlyrAudEnbldBfrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrAudEnbldBfrOpngFilTest* self = new (ELeave) RVidPlyrAudEnbldBfrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrAudEnbldBfrOpngFilTest::RVidPlyrAudEnbldBfrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrAudEnbldBfrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.6  - Query audio is enabled or not before opening the file"));
	}

	
/**
 * RVidPlyrAudEnbldBfrOpngFilTest::Fsm
 */
void RVidPlyrAudEnbldBfrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->AudioEnabledL() before opening file"));
                TRAP(iError, iUtility->AudioEnabledL());
                if (iError!= KErrNotReady)
                    {
                    INFO_PRINTF2(_L("AudioEnabledL() leaves with %d after opening file"), iError);
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("AudioEnabledL() leaves with %d after opening file"), iError);
                    StopTest(iError, EPass);     
                    } 
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// MM-MMF-VCLNT-CHRTZ-I-0027
// Get the volume without setting any volume after opening the file

// static NewL()
RVidPlyrGetVolAftrOpngFilTest* RVidPlyrGetVolAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetVolAftrOpngFilTest* self = new (ELeave) RVidPlyrGetVolAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetVolAftrOpngFilTest::RVidPlyrGetVolAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetVolAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.7  - Get volume without setting any volume after opening file"));
	}

/**
 * RVideoPlayTest::Fsm
 */
void RVidPlyrGetVolAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TInt vol;
                // Get the volume 
                INFO_PRINTF1(_L("iUtility->Volume()"));
                vol = iUtility->Volume();
                if (vol)
                    {
                    INFO_PRINTF2(_L("Volume returned is %d"), vol);
                    StopTest(iError, EPass);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("Volume returned is %d"), vol);
                    StopTest(iError, EFail);   
                    }
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set and get volume before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0028
// static NewL()
RVidPlyrSetNGetVolB4OpngFilTest* RVidPlyrSetNGetVolB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetVolB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetVolB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetVolB4OpngFilTest::RVidPlyrSetNGetVolB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetVolB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.7  - Set and get volume before opening the file"));
	}

/**
 * RVidPlyrSetNGetVolB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetVolB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->SetVolumeL()"));
                TInt volume = iUtility->MaxVolume();
                INFO_PRINTF2(_L("Volume to be set is %d"), volume);
                TRAP(iError, iUtility->SetVolumeL(volume));
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    // Get the volume
                    INFO_PRINTF1(_L("iUtility->Volume()"));
                    TInt volume1 = iUtility->Volume();
                    INFO_PRINTF2(_L("Volume got by volume() API is %d"), volume);
                    if(volume1 == volume)
                        {
                        StopTest(iError, EPass);     
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the duration of video file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0029
// static NewL()
RVidPlyrGetDrnAftrOpngFilTest* RVidPlyrGetDrnAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetDrnAftrOpngFilTest* self = new (ELeave) RVidPlyrGetDrnAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetDrnAftrOpngFilTest::RVidPlyrGetDrnAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetDrnAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.9 - Get the duration of video file after opening it"));
	}


/**
 * RVidPlyrGetDrnAftrOpngFilTest::Fsm
 */
void RVidPlyrGetDrnAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TInt duration;
    		    INFO_PRINTF1(_L("iUtility->DurationL()"));
    		    TRAP(iError, duration = I64INT(iUtility->DurationL().Int64()));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF2(_L("Duration returned after opening the file is %d"), duration);
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Get the duration of video file in playing state
// MM-MMF-VCLNT-CHRTZ-I-0030
// static NewL()
RVidPlyrGetDrnPlyngTest* RVidPlyrGetDrnPlyngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetDrnPlyngTest* self = new (ELeave) RVidPlyrGetDrnPlyngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetDrnPlyngTest::RVidPlyrGetDrnPlyngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetDrnPlyngTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.9 - Get the duration of video file in playing state"));
	}

void RVidPlyrGetDrnPlyngTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrGetDrnPlyngTest::Fsm
 */
void RVidPlyrGetDrnPlyngTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                TInt duration;
                iTimer->Cancel();
    		    INFO_PRINTF1(_L("iUtility->DurationL()"));
    		    TRAP(iError, duration = I64INT(iUtility->DurationL().Int64()));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF2(_L("Duration returned in playing state is %d"), duration);
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the duration of video file in paused state
// MM-MMF-VCLNT-CHRTZ-I-0031
// static NewL()
RVidPlyrGetDrnPausedTest* RVidPlyrGetDrnPausedTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetDrnPausedTest* self = new (ELeave) RVidPlyrGetDrnPausedTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetDrnPausedTest::RVidPlyrGetDrnPausedTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetDrnPausedTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.9 - Get the duration of video file in paused state"));
	}

void RVidPlyrGetDrnPausedTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrGetDrnPausedTest::Fsm
 */
void RVidPlyrGetDrnPausedTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPaused, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPaused:
                iTimer->Cancel();
                // Pause the video Play
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                TRAP(iError, iUtility->PauseL());
                if(iError!= KErrNone)
                    {
                    if (iError == KErrNotSupported)
                        {
                        INFO_PRINTF1(_L("PauseL() is not supported"));
                        StopTest(iError, EKnownFailure);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("PauseL() left with %d"), iError);
                        StopTest(iError, EFail);
                        }
                    }
                else
                    {
        		    INFO_PRINTF1(_L("iUtility->DurationL()"));
        		    TRAP(iError, iUtility->DurationL());
        		    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);    
                        }
            		else
                        {
                        StopTest(iError, EPass);   
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the duration of video file in Stopped state
// MM-MMF-VCLNT-CHRTZ-I-0032
// static NewL()
RVidPlyrGetDrnStpdTest* RVidPlyrGetDrnStpdTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetDrnStpdTest* self = new (ELeave) RVidPlyrGetDrnStpdTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetDrnStpdTest::RVidPlyrGetDrnStpdTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetDrnStpdTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.9 - Get the duration of video file in stopped state"));
	}

void RVidPlyrGetDrnStpdTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPStopped;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrGetDrnStpdTest::Fsm
 */
void RVidPlyrGetDrnStpdTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                iTimer->Cancel();
                // Stop the video Play
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                TInt duration;
        		INFO_PRINTF1(_L("iUtility->DurationL()"));
        		TRAP(iError, duration = I64INT(iUtility->DurationL().Int64()));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF2(_L("Duration returned in stopped state is %d"), duration);
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Get the duration of video file before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0033
// static NewL()
RVidPlyrGetDrnB4OpngFilTest* RVidPlyrGetDrnB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetDrnB4OpngFilTest* self = new (ELeave) RVidPlyrGetDrnB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetDrnB4OpngFilTest::RVidPlyrGetDrnB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetDrnB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.9 - Get the duration of video file before opening the file"));
	}

/**
 * RVidPlyrGetDrnB4OpngFilTest::Fsm
 */
void RVidPlyrGetDrnB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                TInt duration;
        		INFO_PRINTF1(_L("iUtility->DurationL()"));
        		TRAP(iError, duration = I64INT(iUtility->DurationL().Int64()));
    		    if (iError!= KErrNotReady)
                    {
                    INFO_PRINTF2(_L("Duration returned before opening the file is %d"), duration);
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF2(_L("Duration returned before opening the file is %d"), duration);
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set and get the position of playback after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0034
// static NewL()
RVidPlyrSetNGetPosAftrOpngFilTest* RVidPlyrSetNGetPosAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetPosAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosAftrOpngFilTest::RVidPlyrSetNGetPosAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosAftrOpngFilTest::DoKickoffTestL()
	{
	iValid = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Set and get position of playback after opening the file"));
	}


/**
 * RVidPlyrSetNGetPosAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetPosAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TTimeIntervalMicroSeconds position;
                //Set position in the middle of the clip
                TRAP(iError, position = I64INT(iUtility->DurationL().Int64()) / 2);
				if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
    		    INFO_PRINTF2(_L("Position to be set by SetPositionL() is %d"), I64INT(position.Int64()));
    		    TRAP(iError, iUtility->SetPositionL(position));
    		    if (iError!= KErrNotSupported)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF1(_L("SetPositionL() leaves with -5 ,(DEF088334 raised)"));
                    TTimeIntervalMicroSeconds position1;
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError != KErrNone)
                        {
                        INFO_PRINTF2(_L("PositionL() left with error : %d - DEF100146"), iError);
                        StopTest(iError, EFail);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                        if(I64INT(position1.Int64()) >= 0 && iValid)
                            {
                            StopTest(iError, EPass);
                            }
                        else
                            {
                            StopTest(iError, EFail);
                            }
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set and get the position of video play in playing state
// MM-MMF-VCLNT-CHRTZ-I-0035
// static NewL()
RVidPlyrSetNGetPosInPlyngTest* RVidPlyrSetNGetPosInPlyngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosInPlyngTest* self = new (ELeave) RVidPlyrSetNGetPosInPlyngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosInPlyngTest::RVidPlyrSetNGetPosInPlyngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosInPlyngTest::DoKickoffTestL()
	{
	TPtrC longvidfile;
	iValid = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Set and get the position of video play in playing state"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("longvidfile"), longvidfile))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(longvidfile);
	}

void RVidPlyrSetNGetPosInPlyngTest::DoTimerCallback()
	{
	#ifndef __EABI__
	INFO_PRINTF1(_L("Timer event occured"));
	#endif
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrSetNGetPosInPlyngTest::Fsm
 */
void RVidPlyrSetNGetPosInPlyngTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                iTimer->Cancel();
    		    TTimeIntervalMicroSeconds position;
                //Set position in the middle of the clip
                TRAP(iError, position = I64INT(iUtility->DurationL().Int64()) / 2);
    			if (iError!= KErrNone)
        		    {
        		    StopTest(iError, EFail);    
        		    }
        		#ifndef __EABI__
    		    INFO_PRINTF2(_L("Position to be set is %d"), I64INT(position.Int64()));
    		    #endif
    		    TRAP(iError, iUtility->SetPositionL(position));
    		    if (iError!= KErrNotSupported)
        		    {
        		    iValid = EFalse; 
        		    StopTest(iError, EFail);    
        		    }
        		else
            		{
            		#ifndef __EABI__
                    INFO_PRINTF1(_L("SetPositionL() leaves with -5 ,(DEF088334 raised)"));
                    #endif
                    TTimeIntervalMicroSeconds position1;
                    #ifndef __EABI__
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    #endif
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);    
                        }
            		 else
                		 {
                		#ifndef __EABI__
                        INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                        #endif
                        if(I64INT(position1.Int64()) == I64INT(position.Int64()) && iValid)
                            {
                            StopTest(iError, EPass);      
                            }
                        else
                            {
                            StopTest(iError, EKnownFailure);       
                            }
                		 }
            		 }
                break;
            }
        }
    else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set and get the position of video play in paused state
// MM-MMF-VCLNT-CHRTZ-I-0036
// static NewL()
RVidPlyrSetNGetPosPausedTest* RVidPlyrSetNGetPosPausedTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosPausedTest* self = new (ELeave) RVidPlyrSetNGetPosPausedTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosPausedTest::RVidPlyrSetNGetPosPausedTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosPausedTest::DoKickoffTestL()
	{
	TPtrC longvidfile;
	iValid = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Get the position of video file in paused state"));
		//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("longvidfile"), longvidfile))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(longvidfile);
	}

void RVidPlyrSetNGetPosPausedTest::DoTimerCallback()
	{
	#ifndef __EABI__
	INFO_PRINTF1(_L("Timer event occured"));
	#endif
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPaused;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrSetNGetPosPausedTest::Fsm
 */
void RVidPlyrSetNGetPosPausedTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPaused, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPaused:
                iTimer->Cancel();
                // Pause the video Play
                #ifndef __EABI__
                INFO_PRINTF1(_L("iUtility->PauseL()"));
                #endif
                TRAP(iError, iUtility->PauseL());
                if(iError!= KErrNotSupported)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);   
                    }
                else
                    {
                    #ifndef __EABI__
                    INFO_PRINTF1(_L("PauseL() is not supported"));
                    #endif
                    TTimeIntervalMicroSeconds position;
                    //Set position in the middle of the clip
                    TRAP(iError, position = I64INT(iUtility->DurationL().Int64()) / 2);
					if (iError!= KErrNone)
            		    {
            		    StopTest(iError, EFail);    
            		    }
            		#ifndef __EABI__
        		    INFO_PRINTF2(_L("Position to be set is %d"), I64INT(position.Int64()));
        		    #endif
        		    TRAP(iError, iUtility->SetPositionL(position));
        		    if (iError!= KErrNotSupported)
                        {
                        iValid = EFalse; 
                        StopTest(iError, EFail);    
                        }
            		else
                        {
                        #ifndef __EABI__
                        INFO_PRINTF1(_L("SetPositionL() leaves with -5 ,(DEF088334 raised)"));
                        #endif
                        TTimeIntervalMicroSeconds position1;
                        #ifndef __EABI__
                        INFO_PRINTF1(_L("iUtility->PositionL()"));
                        #endif
                        TRAP(iError, position1 = iUtility->PositionL());
                        if (iError!= KErrNone)
                            {
                            StopTest(iError, EFail);    
                            }
                        else
                            {
                            #ifndef __EABI__
                            INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                            #endif
                            if(I64INT(position1.Int64()) == I64INT(position.Int64()) && iValid)
                                {
                                StopTest(iError, EPass);      
                                }
                            else
                                {
                                StopTest(iError, EKnownFailure);       
                                }
                            }
                        }
                    }
                break;                
            }
        }
    	else
            {
        	StopTest(iError, EFail);
            }
    }
  
// Set and get the position of video play in stopped state
// MM-MMF-VCLNT-CHRTZ-I-0037
// static NewL()
RVidPlyrSetNGetPosInStpdStTest* RVidPlyrSetNGetPosInStpdStTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosInStpdStTest* self = new (ELeave) RVidPlyrSetNGetPosInStpdStTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosInStpdStTest::RVidPlyrSetNGetPosInStpdStTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosInStpdStTest::DoKickoffTestL()
	{
	iValid = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Set and Get the position of video play in stopped state"));
	}

void RVidPlyrSetNGetPosInStpdStTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPStopped;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrSetNGetPosInStpdStTest::Fsm
 */
void RVidPlyrSetNGetPosInStpdStTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                iTimer->Cancel();
                // Stop the video Play
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                TTimeIntervalMicroSeconds position;
                //Set position in the middle of the clip
                TRAP(iError, position = I64INT(iUtility->DurationL().Int64()) / 2);
				if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
    		    INFO_PRINTF2(_L("Position to be set is %d"), I64INT(position.Int64()));
    		    TRAP(iError, iUtility->SetPositionL(position));
    		    if (iError!= KErrNotSupported)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF1(_L("SetPositionL() leaves with -5 ,(DEF088334 raised)"));
                    TTimeIntervalMicroSeconds position1;
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError!= KErrNotReady)
                        {
                        StopTest(iError, EFail);    
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                        if(I64INT(position1.Int64()) == I64INT(position.Int64()) && iValid)
                            {
                            StopTest(iError, EPass);      
                            }
                        else
                            {
                            StopTest(iError, EKnownFailure);       
                            }
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Set and get the position of video play before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0038
// static NewL()
RVidPlyrSetNGetPosB4OpngFilTest* RVidPlyrSetNGetPosB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetPosB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosB4OpngFilTest::RVidPlyrSetNGetPosB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosB4OpngFilTest::DoKickoffTestL()
	{
	iValid = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Set and Get the position of video play before opening the file"));
	}

/**
 * RVidPlyrSetNGetPosB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetPosB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                //Set position in the middle of the clip
                // The position is hard coded as the DurationL() leaves with KErrNotReady
                TTimeIntervalMicroSeconds position = 1300000;
                // TRAP(iError, position = I64INT(iUtility->DurationL().Int64()) / 2;
    		    INFO_PRINTF2(_L("Position to be set is %d"), I64INT(position.Int64()));
    		    TRAP(iError, iUtility->SetPositionL(position));
    		    if (iError!= KErrNotReady)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF1(_L("PositionL() leaves with KErrNotReady if called before opening file"));
                    TTimeIntervalMicroSeconds position1;
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError != KErrNotReady)
                        {
                        StopTest(iError, EFail);    
                        }
                    else
                        {
                        StopTest(iError, EPass);    
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set position of playback with some invalid value and get those, after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0039
// static NewL()
RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest* RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::DoKickoffTestL()
	{
	iValid = ETrue;
	iValid1 = ETrue;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.10 - Set and get position with invalid value after opening the file"));
	}


/**
 * RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Set the position to Zero
    		    TRAP(iError, iUtility->SetPositionL(0));
    		    if (iError!= KErrNotSupported)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    TTimeIntervalMicroSeconds position1;
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);    
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                        if((I64INT(position1.Int64()) == 0) && iValid)
                            {
                            iValid1 = EFalse ;
                            StopTest(iError, EPass);      
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Set and get position check for position = 0 is complete "));
                            }
                        }
                    }            
                TTimeIntervalMicroSeconds position;
                // Get the duration of the clip
                TRAP(iError, position = I64INT(iUtility->DurationL().Int64()));
				if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
                INFO_PRINTF2(_L("Duration of the clip is %d"), I64INT(position.Int64()));
                position = I64INT(position.Int64()) + 100;
    		    INFO_PRINTF2(_L("Position to be set is %d"), I64INT(position.Int64()));
    		    TRAP(iError, iUtility->SetPositionL(position));
    		    if (iError!= KErrNotSupported)
                    {
                    iValid = EFalse; 
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF1(_L("SetPositionL() leaves with -5 ,(DEF088334 raised)"));
                    TTimeIntervalMicroSeconds position1;
                    INFO_PRINTF1(_L("iUtility->PositionL()"));
                    TRAP(iError, position1 = iUtility->PositionL());
                    if (iError!= KErrNotReady)
                        {
                        StopTest(iError, EFail);    
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Current position is %d"), I64INT(position1.Int64()));
                        if(I64INT(position1.Int64()) == I64INT(position.Int64()) && iValid1)
                            {
                            StopTest(iError, EPass);      
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Set and get position check for \"position > duration\" is complete "));
                            StopTest(iError, EKnownFailure);       
                            }
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Query the audio type of video file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0040
// static NewL()
RVidPlyrQryAudTypVidFilTest* RVidPlyrQryAudTypVidFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudTypVidFilTest* self = new (ELeave) RVidPlyrQryAudTypVidFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudTypVidFilTest::RVidPlyrQryAudTypVidFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudTypVidFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.11 - Query the audio type of video file"));
	}

/**
 * RVidPlyrQryAudTypVidFilTest::Fsm
 */
void RVidPlyrQryAudTypVidFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->AudioTypeL()"));
                TFourCC audtyp;
                TRAP(iError, audtyp = iUtility->AudioTypeL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TUint8 char1 = TUint8( audtyp.FourCC() ) ;
                    TUint8 char2 = TUint8( audtyp.FourCC() >> 8 ) ;
                    TUint8 char3 = TUint8( audtyp.FourCC() >> 16 ) ;
                    TUint8 char4 = TUint8( audtyp.FourCC() >> 24 ) ;
                    INFO_PRINTF5(_L("AudioType is = %1c%1c%1c%1c"), char1, char2, char3, char4);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
 
// Query the audio type of video file before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0044
// static NewL()
RVidPlyrQryAudTypB4OpngFilTest* RVidPlyrQryAudTypB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudTypB4OpngFilTest* self = new (ELeave) RVidPlyrQryAudTypB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudTypB4OpngFilTest::RVidPlyrQryAudTypB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudTypB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.11 - Query the audio type of video file before opening it"));
	}

/**
 * RVidPlyrQryAudTypB4OpngFilTest::Fsm
 */
void RVidPlyrQryAudTypB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->AudioTypeL()"));
                TFourCC audtyp;
                TRAP(iError, audtyp = iUtility->AudioTypeL());
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("iUtility->AudioTypeL() leaves with %d before opening file"), iError);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Query the audio type of video only file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0045
// static NewL()
RVidPlyrQryAudTypVidOnlyFilTest* RVidPlyrQryAudTypVidOnlyFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudTypVidOnlyFilTest* self = new (ELeave) RVidPlyrQryAudTypVidOnlyFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudTypVidOnlyFilTest::RVidPlyrQryAudTypVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudTypVidOnlyFilTest::DoKickoffTestL()
	{
	TPtrC vidonlyfilname;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.11 - Query the audio type of video only file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("vidonlyfile"), vidonlyfilname))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(vidonlyfilname);
	}

/**
 * RVidPlyrQryAudTypVidOnlyFilTest::Fsm
 */
void RVidPlyrQryAudTypVidOnlyFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->AudioTypeL()"));
                TFourCC audtyp;
                TRAP(iError, audtyp = iUtility->AudioTypeL());
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF1(_L("AudioType should not be returned for video only file"));                    
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Query the audio bitrate of video file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0046
// static NewL()
RVidPlyrQryAudBitRateVidFilTest* RVidPlyrQryAudBitRateVidFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudBitRateVidFilTest* self = new (ELeave) RVidPlyrQryAudBitRateVidFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudBitRateVidFilTest::RVidPlyrQryAudBitRateVidFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudBitRateVidFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.12 - Query the audio bitrate of video file"));
	}

/**
 * RVidPlyrQryAudBitRateVidFilTest::Fsm
 */
void RVidPlyrQryAudBitRateVidFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->AudioBitRateL()"));
                TInt audbitrate;
                //iUtility->AudioBitrateL();
                TRAP(iError, audbitrate = iUtility->AudioBitRateL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("Audiobitrate is = %d"), audbitrate);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
  
// Query the audio bitrate of video file before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0046
// static NewL()
RVidPlyrQryAudBitRateB4OpngFilTest* RVidPlyrQryAudBitRateB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudBitRateB4OpngFilTest* self = new (ELeave) RVidPlyrQryAudBitRateB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudBitRateB4OpngFilTest::RVidPlyrQryAudBitRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudBitRateB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.12 - Query the audio bitrate before opening the video file"));
	}

/**
 * RVidPlyrQryAudBitRateB4OpngFilTest::Fsm
 */
void RVidPlyrQryAudBitRateB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->AudioBitRateL()"));
                //iUtility->AudioBitrateL();
                TRAP(iError, iUtility->AudioBitRateL());
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("AudioBitRateL() leaves with = %d"), iError);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 

// Query the audio bitrate of video only file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0051
// static NewL()
RVidPlyrQryAudBitRateVidOnlyFilTest* RVidPlyrQryAudBitRateVidOnlyFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryAudBitRateVidOnlyFilTest* self = new (ELeave) RVidPlyrQryAudBitRateVidOnlyFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryAudBitRateVidOnlyFilTest::RVidPlyrQryAudBitRateVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryAudBitRateVidOnlyFilTest::DoKickoffTestL()
	{
	TPtrC vidonlyfilname;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.12 - Query the audio type of video only file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("vidonlyfile"), vidonlyfilname))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(vidonlyfilname);
	}

/**
 * RVidPlyrQryAudBitRateVidOnlyFilTest::Fsm
 */
void RVidPlyrQryAudBitRateVidOnlyFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->AudioBitRateL()"));
                TRAP(iError, iUtility->AudioBitRateL());
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("AudioBitRateL() leaves with %d"), iError);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Query the video bitrate of video file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0052
// static NewL()
RVidPlyrQryVidBitRateVidFilTest* RVidPlyrQryVidBitRateVidFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryVidBitRateVidFilTest* self = new (ELeave) RVidPlyrQryVidBitRateVidFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryVidBitRateVidFilTest::RVidPlyrQryVidBitRateVidFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryVidBitRateVidFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.13 - Query the video bitrate of video file"));
	}

/**
 * RVidPlyrQryVidBitRateVidFilTest::Fsm
 */
void RVidPlyrQryVidBitRateVidFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->VideoBitRateL()"));
                TRAP(iError, iUtility->VideoBitRateL());
                if (iError!= KErrNone)
                    {
                    INFO_PRINTF2(_L("VideoBitRateL() leaves with %d"), iError);
                    StopTest(iError, EKnownFailure);  
                    }
                else
                    {
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Query the video bitrate of video file before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0056
// static NewL()
RVidPlyrQryVidBitRateB4OpngFilTest* RVidPlyrQryVidBitRateB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryVidBitRateB4OpngFilTest* self = new (ELeave) RVidPlyrQryVidBitRateB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryVidBitRateB4OpngFilTest::RVidPlyrQryVidBitRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryVidBitRateB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.13 - Query the video bitrate before opening the video file"));
	}

/**
 * RVidPlyrQryVidBitRateB4OpngFilTest::Fsm
 */
void RVidPlyrQryVidBitRateB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->VideoBitRateL()"));
                TRAP(iError, iUtility->VideoBitRateL());
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("VideoBitRateL() leaves with = %d before opening the file"), iError);
                    StopTest(iError, EPass);    
                    }                
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 

// Get the video format MIME type of video file after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0057
// static NewL()
RVidPlyrQryVidFmtMimeTypTest* RVidPlyrQryVidFmtMimeTypTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryVidFmtMimeTypTest* self = new (ELeave) RVidPlyrQryVidFmtMimeTypTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryVidFmtMimeTypTest::RVidPlyrQryVidFmtMimeTypTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrQryVidFmtMimeTypTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.14 - Query the video format MIME type"));
	}

/**
 * RVidPlyrQryVidFmtMimeTypTest::Fsm
 */
void RVidPlyrQryVidFmtMimeTypTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->VideoFormatMimeType()"));
                TPtrC8 vidfmtmimtyp = iUtility->VideoFormatMimeType();
                // take it to a 16-bit string
                TBuf<64> mimeType16;
                mimeType16.Copy(vidfmtmimtyp);
                INFO_PRINTF2(_L("Mime type : \'%S\'"), &mimeType16);
                
                // Check if its valid
                if (mimeType16.Compare(_L("XVID")) == 0)
                    {
                    INFO_PRINTF1(_L("MIME Type match"));
                    StopTest();
                    }
                else
                    {
                    INFO_PRINTF1(_L("MIME Type doesn't match"));
                    StopTest(iError, EFail);   
                    }   
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
      
// Get the video frame size 
// MM-MMF-VCLNT-CHRTZ-I-0060
// static NewL()
RVidPlyrQryVidFrameSizTest* RVidPlyrQryVidFrameSizTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryVidFrameSizTest* self = new (ELeave) RVidPlyrQryVidFrameSizTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryVidFrameSizTest::RVidPlyrQryVidFrameSizTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	iFrameSize.iWidth = 0;
    iFrameSize.iHeight = 0;
	}
	
// start test
void RVidPlyrQryVidFrameSizTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.15 - Query the video frame size after opening the file"));
	}

/**
 * RVidPlyrQryVidFrameSizTest::Fsm
 */
void RVidPlyrQryVidFrameSizTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                INFO_PRINTF1(_L("iUtility->VideoFrameSizeL()"));
                TRAP(iError, iUtility->VideoFrameSizeL(iFrameSize));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF3(_L("Frame size : (%d, %d)"), iFrameSize.iWidth, iFrameSize.iHeight);
                    if ( iFrameSize.iWidth >= 0 && iFrameSize.iHeight >= 0 )
                        {
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        StopTest(iError, EFail);     
                        }   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
  
  
// Get the video frame size before opening the file 
// MM-MMF-VCLNT-CHRTZ-I-0063
// static NewL()
RVidPlyrQryVidFrameSizB4OpngFilTest* RVidPlyrQryVidFrameSizB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrQryVidFrameSizB4OpngFilTest* self = new (ELeave) RVidPlyrQryVidFrameSizB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrQryVidFrameSizB4OpngFilTest::RVidPlyrQryVidFrameSizB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	iFrameSize.iWidth = 0;
    iFrameSize.iHeight = 0;
	}
	
// start test
void RVidPlyrQryVidFrameSizB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.15 - Get the video frame size before opening the file"));
	}

/**
 * RVidPlyrQryVidFrameSizB4OpngFilTest::Fsm
 */
void RVidPlyrQryVidFrameSizB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->VideoFrameSizeL()"));
                TRAP(iError, iUtility->VideoFrameSizeL(iFrameSize));
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("VideoFrameSizeL() leaves with %d before opening the file"), iError);
                    StopTest(iError, EPass);     
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set the video frame rate 
// MM-MMF-VCLNT-CHRTZ-I-0064
// static NewL()
RVidPlyrSetVidFrameRateTest* RVidPlyrSetVidFrameRateTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVidFrameRateTest* self = new (ELeave) RVidPlyrSetVidFrameRateTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVidFrameRateTest::RVidPlyrSetVidFrameRateTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVidFrameRateTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.16 - Set video frame rate after opening the file"));
	}

/**
 * RVidPlyrSetVidFrameRateTest::Fsm
 */
void RVidPlyrSetVidFrameRateTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 framesPerSecond = 10.00;
                INFO_PRINTF1(_L("iUtility->SetVideoFrameRateL()"));
                TRAP(iError, iUtility->SetVideoFrameRateL(framesPerSecond));
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("SetVideoFrameRateL() leaves with %d "), iError);
                    StopTest(iError, EPass);     
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
  
// Get the video frame rate 
// MM-MMF-VCLNT-CHRTZ-I-0065
// static NewL()
RVidPlyrGetVidFrameRateTest* RVidPlyrGetVidFrameRateTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetVidFrameRateTest* self = new (ELeave) RVidPlyrGetVidFrameRateTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetVidFrameRateTest::RVidPlyrGetVidFrameRateTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetVidFrameRateTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.17 - Get video frame rate after opening the file"));
	}

/**
 * RVidPlyrGetVidFrameRateTest::Fsm
 */
void RVidPlyrGetVidFrameRateTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 framesPerSecond = 10.00;
                INFO_PRINTF1(_L("iUtility->SetVideoFrameRateL()"));
                TRAP(iError, iUtility->SetVideoFrameRateL(framesPerSecond));
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    TReal32 getframesPerSecond = 0.0;
                    INFO_PRINTF2(_L("SetVideoFrameRateL() leaves with %d "), iError);
                    TRAP(iError, getframesPerSecond = iUtility->VideoFrameRateL());
                    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);      
                        }
                    else
                        {
                        INFO_PRINTF2(_L("GetVideoFrameRate returned is %d "), TInt(getframesPerSecond)); 
                        if(getframesPerSecond == framesPerSecond)
                            {
                            INFO_PRINTF1(_L("Value set and get matches"));
                            StopTest(iError, EPass);        
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Value set and get mismatche"));
                            StopTest(iError, EFail);         
                            }
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Get the video frame rate before opening the file 
// MM-MMF-VCLNT-CHRTZ-I-0069
// static NewL()
RVidPlyrGetVidFrameRateB4OpngFilTest* RVidPlyrGetVidFrameRateB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetVidFrameRateB4OpngFilTest* self = new (ELeave) RVidPlyrGetVidFrameRateB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetVidFrameRateB4OpngFilTest::RVidPlyrGetVidFrameRateB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetVidFrameRateB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.17 - Get the video frame rate before opening the file"));
	}

/**
 * RVidPlyrGetVidFrameRateB4OpngFilTest::Fsm
 */
void RVidPlyrGetVidFrameRateB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->VideoFrameRateL()"));
                TRAP(iError, iUtility->VideoFrameRateL());
                if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("VideoFrameRateL() leaves with %d before opening the file"), iError);
                    StopTest(iError, EPass);     
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Redraw the current frame in playing state
// MM-MMF-VCLNT-CHRTZ-I-0070
// static NewL()
RVidPlyrRfrshFrmPlyngTest* RVidPlyrRfrshFrmPlyngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrRfrshFrmPlyngTest* self = new (ELeave) RVidPlyrRfrshFrmPlyngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrRfrshFrmPlyngTest::RVidPlyrRfrshFrmPlyngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrRfrshFrmPlyngTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.18 - Redraw the current frame in playing state"));
	}

void RVidPlyrRfrshFrmPlyngTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrRfrshFrmPlyngTest::Fsm
 */
void RVidPlyrRfrshFrmPlyngTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
                iTimer->Cancel();
    		    INFO_PRINTF1(_L("iUtility->RefreshFrameL()"));
    		    TRAP(iError, iUtility->RefreshFrameL());
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF1(_L("Redrawing of current frame is successful"));
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Redraw the current frame before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0072
// static NewL()
RVidPlyrRfrshFrmB4OpngFilTest* RVidPlyrRfrshFrmB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrRfrshFrmB4OpngFilTest* self = new (ELeave) RVidPlyrRfrshFrmB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrRfrshFrmB4OpngFilTest::RVidPlyrRfrshFrmB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrRfrshFrmB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.18 - Redraw the current frame before opening the file"));
	}

/**
 * RVidPlyrRfrshFrmB4OpngFilTest::Fsm
 */
void RVidPlyrRfrshFrmB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
    		    INFO_PRINTF1(_L("iUtility->RefreshFrameL()"));
    		    TRAP(iError, iUtility->RefreshFrameL());
    		    if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    INFO_PRINTF2(_L("RefreshFrameL leaves with %d before opening the file"), iError);
                    StopTest(iError, EPass);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Get the current frame in playing state
// MM-MMF-VCLNT-CHRTZ-I-0073
// static NewL()
RVidPlyrGetCurrentFrmPlyngTest* RVidPlyrGetCurrentFrmPlyngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCurrentFrmPlyngTest* self = new (ELeave) RVidPlyrGetCurrentFrmPlyngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCurrentFrmPlyngTest::RVidPlyrGetCurrentFrmPlyngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCurrentFrmPlyngTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.19 - Get the current frame in playing state"));
	}

void RVidPlyrGetCurrentFrmPlyngTest::DoTimerCallback()
	{
	INFO_PRINTF1(_L("Timer event occured"));
	switch (iState)
		{
		case EVPPlaying:
            iState = EVPPlaying1;
			Fsm();
			break;
	    case EVPFrameRequested:
            iState = EVPStopped;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}


/**
 * RVidPlyrGetCurrentFrmPlyngTest::Fsm
 */
void RVidPlyrGetCurrentFrmPlyngTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPPlaying1, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPPlaying1:
    		    INFO_PRINTF1(_L("iUtility->GetFrameL()"));
    		    TRAP(iError, iUtility->GetFrameL(EColor16M));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }    		    
    		    iState = EVPFrameRequested;
    		    SetExpectedState(EVPStopped, KErrNone);
                break;
            case EVPStopped:
                iTimer->Cancel();                                             
            	INFO_PRINTF1(_L("Got the current frame successfully"));
            	StopTest(iError, EPass);   
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the current frame before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0075
// static NewL()
RVidPlyrGetCurrentFrmB4OpngFilTest* RVidPlyrGetCurrentFrmB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCurrentFrmB4OpngFilTest* self = new (ELeave) RVidPlyrGetCurrentFrmB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCurrentFrmB4OpngFilTest::RVidPlyrGetCurrentFrmB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCurrentFrmB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.19 - Get the current frame before opening the file"));
	}
	
/**
 * RVidPlyrGetCurrentFrmB4OpngFilTest::Fsm
 */
void RVidPlyrGetCurrentFrmB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
    		    INFO_PRINTF1(_L("iUtility->GetFrameL()"));
    		    TRAP(iError, iUtility->GetFrameL(EColor16M));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		iState = EVPFrameRequested;
    		    SetExpectedState(EVPStopped, KErrNotReady);
                break;
            case EVPStopped:
            	INFO_PRINTF1(_L("GetFrameL() leaves with KErrNotReady"));
            	StopTest(iError, EPass);   
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Set and get the priority after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0076
// static NewL()
RVidPlyrSetNGetPrtyAftrOpnFileTest* RVidPlyrSetNGetPrtyAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPrtyAftrOpnFileTest* self = new (ELeave) RVidPlyrSetNGetPrtyAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPrtyAftrOpnFileTest::RVidPlyrSetNGetPrtyAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPrtyAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.20  - Set and get priority after opening video file"));
	}

/**
 * RVidPlyrSetNGetPrtyAftrOpnFileTest::Fsm
 */
void RVidPlyrSetNGetPrtyAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call SetPriorityL() 
                INFO_PRINTF1(_L("iUtility->SetPriorityL()"));
                TInt priority = EMdaPriorityMax;
        		INFO_PRINTF2(_L("Priority set is %d"), priority);
                TRAP(iError, iUtility->SetPriorityL(priority, EMdaPriorityPreferenceQuality));
                    {
        		    if (iError!= KErrNone)
                        {
                        StopTest(iError, EFail);    
                        }
            		else
                        {
                        TInt getThePrty = 0;
                        TMdaPriorityPreference getThePref; 
                        INFO_PRINTF1(_L("Set the Priority successfully"));  
                        TRAP(iError, iUtility->PriorityL(getThePrty, getThePref));
                        if(iError !=KErrNone)
                            {
                            StopTest(iError, EFail);
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Got the Priority successfully"));   
                            if(getThePrty == priority)
                                {
                                INFO_PRINTF1(_L("Priority set and got matches"));
                                INFO_PRINTF4(_L("Priority set = %d , Priority got =%d, Preference = %d"), priority, getThePrty, getThePref);
                                StopTest(iError, EPass);   
                                }
                            else
                                {
                                INFO_PRINTF1(_L("Priority set and got doesn't match"));
                                INFO_PRINTF3(_L("Priority set = %d and Priority got =%d"), priority, getThePrty);
                                StopTest(iError, EFail);   
                                }
                            }
                        }            		    
            		}                    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
 
// Set and get the priority before opening video file
// MM-MMF-VCLNT-CHRTZ-I-0078
// static NewL()
RVidPlyrSetNGetPrtyB4OpnFileTest* RVidPlyrSetNGetPrtyB4OpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPrtyB4OpnFileTest* self = new (ELeave) RVidPlyrSetNGetPrtyB4OpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPrtyB4OpnFileTest::RVidPlyrSetNGetPrtyB4OpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPrtyB4OpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.20  - Set and get priority before opening video file"));
	}

/**
 * RVidPlyrSetNGetPrtyB4OpnFileTest::Fsm
 */
void RVidPlyrSetNGetPrtyB4OpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Call SetPriorityL() 
                INFO_PRINTF1(_L("iUtility->SetPriorityL()"));
                TInt priority = EMdaPriorityMax;
        		INFO_PRINTF2(_L("Priority set is %d"), priority);
                TRAP(iError, iUtility->SetPriorityL(priority, EMdaPriorityPreferenceQuality));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
        		    INFO_PRINTF2(_L("SetPriorityL() leaves with %d before opening the file"), iError);                    
                    TInt getThePrty = 0;
                    TMdaPriorityPreference getThePref; 
                    TRAP(iError, iUtility->PriorityL(getThePrty, getThePref));
                    if(iError !=KErrNone)
                        {
                        StopTest(iError, EFail);
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Got the Priority = %d"), getThePrty);
                        if(getThePrty == priority)
                            {
                            INFO_PRINTF1(_L("Priority set and got matches"));
                            INFO_PRINTF4(_L("Priority set = %d , Priority got =%d, Preference = %d"), priority, getThePrty, getThePref);
                            StopTest(iError, EPass);   
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Priority set and got doesn't match"));
                            INFO_PRINTF3(_L("Priority set = %d and Priority got =%d"), priority, getThePrty);
                            StopTest(iError, EFail);   
                            }                           
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set and get the playback balance after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0081
// static NewL()
RVidPlyrSetNGetBalanceAftrOpnFileTest* RVidPlyrSetNGetBalanceAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetBalanceAftrOpnFileTest* self = new (ELeave) RVidPlyrSetNGetBalanceAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetBalanceAftrOpnFileTest::RVidPlyrSetNGetBalanceAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetBalanceAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.22  - Set and get the playback balance after opening video file"));
	}

/**
 * RVidPlyrSetNGetBalanceAftrOpnFileTest::Fsm
 */
void RVidPlyrSetNGetBalanceAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call SetBalanceL() 
                INFO_PRINTF1(_L("iUtility->SetBalanceL()"));
                TInt balance = KMMFBalanceMaxRight;
        		INFO_PRINTF2(_L("Balance to be  set is %d"), balance);
                TRAP(iError, iUtility->SetBalanceL(balance));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    TInt getTheBalance = 0;
                    INFO_PRINTF1(_L("Set the Balance successfully"));  
                    getTheBalance = iUtility->Balance();
                    INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
                    if(getTheBalance == balance)
                        {
                        INFO_PRINTF1(_L("Balance set and got matches"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Balance set and got doesn't match"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EFail);   
                        }
                    }            		    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set and get balance before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0085
// static NewL()
RVidPlyrSetNGetBalanceB4OpngFilTest* RVidPlyrSetNGetBalanceB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetBalanceB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetBalanceB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetBalanceB4OpngFilTest::RVidPlyrSetNGetBalanceB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetBalanceB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.22  - Set and get Balance before opening the file"));
	}

/**
 * RVidPlyrSetNGetBalanceB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetBalanceB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->SetBalanceL()"));
                TInt balance = KMMFBalanceMaxRight;
        		INFO_PRINTF2(_L("Balance to be  set is %d"), balance);
                TRAP(iError, iUtility->SetBalanceL(balance));
    		    if (iError!= KErrNotReady)
                    {
                    StopTest(iError, EFail);    
                    }
                else
                    {
                    TInt getTheBalance = 0;
                    ERR_PRINTF2(_L("SetBalanceL() leaves with %d "), iError);  
                    getTheBalance = iUtility->Balance();
                    INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
                    if(getTheBalance == 0)
                        {
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Balance set and got doesn't match"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EFail);   
                        }
                    }            		    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set the playback balance in a video only file
// MM-MMF-VCLNT-CHRTZ-I-0086

// static NewL()
RVidPlyrSetBalanceAftrOpngVidOnlyFilTest* RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetBalanceAftrOpngVidOnlyFilTest* self = new (ELeave) RVidPlyrSetBalanceAftrOpngVidOnlyFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::RVidPlyrSetBalanceAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::DoKickoffTestL()
	{
	TPtrC vidonlyfilname;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.22 - Set playback balance after opening a video only file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("vidonlyfile"), vidonlyfilname))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(vidonlyfilname);
	}

/**
 * RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::Fsm
 */
void RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call SetBalanceL() 
                INFO_PRINTF1(_L("iUtility->SetBalanceL()"));
                TInt balance = KMMFBalanceMaxRight;
        		INFO_PRINTF2(_L("Balance to be  set is %d"), balance);
                TRAP(iError, iUtility->SetBalanceL(balance));
    		    if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    TInt getTheBalance = 0;
                    ERR_PRINTF2(_L("SetBalanceL() leaves with %d "), iError);  
                    getTheBalance = iUtility->Balance();
                    INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
                    if(getTheBalance == 0)
                        {
                        INFO_PRINTF1(_L("Balance set leaves with -5 and Balance got = 0"));
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Balance set and got doesn't match"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EFail);   
                        }
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the playback balance after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0087
// static NewL()
RVidPlyrGetBalanceWthoutSetAftrOpnFileTest* RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetBalanceWthoutSetAftrOpnFileTest* self = new (ELeave) RVidPlyrGetBalanceWthoutSetAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::RVidPlyrGetBalanceWthoutSetAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.23  - Get the playback balance without setting any value after opening video file"));
	}

/**
 * RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::Fsm
 */
void RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TInt balance = 0;  // default value
        		TInt getTheBalance = 0;
        		INFO_PRINTF1(_L("iUtility->Balance()"));  
        		getTheBalance = iUtility->Balance();
            	INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
            	if(getTheBalance == balance)
                    {
                    INFO_PRINTF3(_L("Balance expected = %d , Balance got =%d"), balance, getTheBalance);
                    StopTest(iError, EPass);   
                    }
            	else
                    {
                    INFO_PRINTF1(_L("Balance set and got doesn't match"));
                    INFO_PRINTF3(_L("Balance expected = %d , Balance got =%d"), balance, getTheBalance);
                    StopTest(iError, EFail);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
   

// Get the playback balance before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0091
// static NewL()
RVidPlyrGetBalanceB4OpngFilTest* RVidPlyrGetBalanceB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetBalanceB4OpngFilTest* self = new (ELeave) RVidPlyrGetBalanceB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetBalanceB4OpngFilTest::RVidPlyrGetBalanceB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetBalanceB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.23  - Get Balance before opening the file"));
	}

/**
 * RVidPlyrGetBalanceB4OpngFilTest::Fsm
 */
void RVidPlyrGetBalanceB4OpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                INFO_PRINTF1(_L("iUtility->Balance()"));
                TInt balance = 0;
        		TInt getTheBalance = 0;
        		getTheBalance = iUtility->Balance();
            	INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
            	if(getTheBalance == 0)
                    {
                    StopTest(iError, EPass);   
                    }
            	else
                    {
                    INFO_PRINTF1(_L("Balance expected and got doesn't match"));
                    INFO_PRINTF3(_L("Balance expected = %d , Balance got =%d"), balance, getTheBalance);
                    StopTest(iError, EFail);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Get the playback balance in a video only file
// MM-MMF-VCLNT-CHRTZ-I-0092

// static NewL()
RVidPlyrGetBalanceAftrOpngVidOnlyFilTest* RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetBalanceAftrOpngVidOnlyFilTest* self = new (ELeave) RVidPlyrGetBalanceAftrOpngVidOnlyFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::RVidPlyrGetBalanceAftrOpngVidOnlyFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::DoKickoffTestL()
	{
	TPtrC vidonlyfilname;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.23 - Get playback balance after opening a video only file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("vidonlyfile"), vidonlyfilname))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(vidonlyfilname);
	}

/**
 * RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::Fsm
 */
void RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
        		 TInt getTheBalance = 0;
        		 TInt balance = 0;
        		 ERR_PRINTF1(_L("iUtility->Balance()"));  
        		 getTheBalance = iUtility->Balance();
            	 INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
            	 if(getTheBalance == 0)
                    {
                    INFO_PRINTF1(_L("Balance matches with default value(0)"));
                    StopTest(iError, EPass);   
                    }
            	 else
                    {
                    INFO_PRINTF1(_L("Balance expected and got doesn't match"));
                    INFO_PRINTF3(_L("Balance expected = %d , Balance got =%d"), balance, getTheBalance);
                    StopTest(iError, EFail);   
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set and get the rotation after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0093

// static NewL()
RVidPlyrSetNGetRotationAftrOpngFilTest* RVidPlyrSetNGetRotationAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetRotationAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetRotationAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetRotationAftrOpngFilTest::RVidPlyrSetNGetRotationAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetRotationAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.25 - Set and get the rotation"));
	}

/**
 * RVidPlyrSetNGetRotationAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetRotationAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call RotationL() without setting any rotation value 
                INFO_PRINTF1(_L("iUtility->RotationL()"));
                TVideoRotation getrotation = EVideoRotationNone;
                TVideoRotation setrotation = EVideoRotationClockwise90;
                TRAP(iError, getrotation = iUtility->RotationL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);   
                    }
        		INFO_PRINTF2(_L("Rotation applied to the current image is %d"), getrotation);
        		// Call SetRotationL()
        		INFO_PRINTF1(_L("iUtility->SetRotationL()"));
                TRAP(iError, iUtility->SetRotationL(setrotation));
    		    if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    // Again call RotationL() after setting the rotation value
                    INFO_PRINTF1(_L("iUtility->RotationL()"));
                    TRAP(iError, getrotation = iUtility->RotationL());
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Rotation set to the current image is %d"), getrotation);   
                        if(getrotation == setrotation)
                            {
                            INFO_PRINTF1(_L("Rotation is successful"));
                            StopTest(iError, EPass);   
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Rotation doesn't match"));
                            INFO_PRINTF3(_L("Rotation set = %d , Rotation got =%d"), setrotation, getrotation);
                            StopTest(iError, EKnownFailure);   
                            }  
                        }

                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
   
   
// Set and get the rotation before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0096

// static NewL()
RVidPlyrSetNGetRotationB4OpngFilTest* RVidPlyrSetNGetRotationB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetRotationB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetRotationB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetRotationB4OpngFilTest::RVidPlyrSetNGetRotationB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetRotationB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.25 - Set and get the rotation before opening the file"));
	}
	
/**
 * RVidPlyrSetNGetRotationB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetRotationB4OpngFilTest::Fsm()
    {
    // the entire test is done in Idle state, so no state machine is required.
    // Call RotationL() without setting any rotation value 
    TVideoRotation setrotation = EVideoRotationClockwise90;
    INFO_PRINTF1(_L("iUtility->RotationL()"));
    TRAP(iError, iUtility->RotationL());
    INFO_PRINTF2(_L("-- Error : %d"), iError);
    if (iError != KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
    else
        {
        INFO_PRINTF2(_L("RotationL() leaves with %d before opening the file"), iError);                    
        // Call SetRotationL()
        INFO_PRINTF2(_L("iUtility->SetRotationL(%d)"), setrotation);
        TRAP(iError, iUtility->SetRotationL(setrotation));
        INFO_PRINTF2(_L("-- Error : %d"), iError);
        if (iError != KErrNotReady)
            {
            StopTest(iError, EFail);
            }
        else
            {
            // Again call RotationL() after setting the rotation value
            INFO_PRINTF1(_L("iUtility->RotationL()"));
            TRAP(iError, iUtility->RotationL());
            INFO_PRINTF2(_L("-- Error : %d"), iError);
            if (iError != KErrNotReady)
                {
                StopTest(iError, EFail);
                }
            else
                {
                StopTest(EPass);
                }
            }
        }
    }

// Set and get rotation in stopped state
// MM-MMF-VCLNT-CHRTZ-I-0096a
// static NewL()
RVidPlyrSetNGetRotationInStopTest* RVidPlyrSetNGetRotationInStopTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetRotationInStopTest* self = new (ELeave) RVidPlyrSetNGetRotationInStopTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetRotationInStopTest::RVidPlyrSetNGetRotationInStopTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetRotationInStopTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.25  - Set and get rotation in stopped state"));
	}

// Timer Callback
void RVidPlyrSetNGetRotationInStopTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPStopped;
			Fsm();
			break; 
		case EVPStopped:
            iState = EVPStopped1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrSetNGetRotationInStopTest::Fsm
 */
void RVidPlyrSetNGetRotationInStopTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                iState = EVPStopped;
                SetExpectedState(EVPStopped1, KErrNone);
                break;
            case EVPStopped1:
                iTimer->Cancel(); 
                TVideoRotation setrotation = EVideoRotationClockwise90;
                // Call SetRotationL()
        		INFO_PRINTF2(_L("iUtility->SetRotationL(%d)"), setrotation);
                TRAP(iError, iUtility->SetRotationL(setrotation));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
    		    if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    // Again call RotationL() after setting the rotation value
                    INFO_PRINTF1(_L("iUtility->RotationL()"));
                    TRAP(iError, iUtility->RotationL());
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        StopTest(iError, EKnownFailure);   
                        }
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set and get the scale factor after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0097

// static NewL()
RVidPlyrSetNGetScaleFactorAftrOpngFilTest* RVidPlyrSetNGetScaleFactorAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetScaleFactorAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetScaleFactorAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetScaleFactorAftrOpngFilTest::RVidPlyrSetNGetScaleFactorAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetScaleFactorAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.26 - Set and get scale factor"));
	}

/**
 * RVidPlyrSetNGetScaleFactorAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetScaleFactorAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 widthScaleFactor = 0.0f;
                TReal32 heightScaleFactor = 0.0f;
                TBool antiAliasFilter = EFalse;
                // Call SetScaleFactorL() without setting any rotation value 
                INFO_PRINTF3(_L("iUtility->SetScaleFactorL(%f, %f ETrue)"),KScaleWidth, KScaleHeight);
                TRAP(iError, iUtility->SetScaleFactorL(KScaleWidth, KScaleHeight, ETrue));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
                    {
                    // call GetScaleFactorL() 
                    INFO_PRINTF1(_L("iUtility->GetScaleFactorL()"));
                    TRAP(iError, iUtility->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Scale factor is not supported currently"));
                        StopTest(iError, EKnownFailure);   
                        }  
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set and get scale factor in stopped state
// MM-MMF-VCLNT-CHRTZ-I-0100
// static NewL()
RVidPlyrSetNGetScaleFactorInStopTest* RVidPlyrSetNGetScaleFactorInStopTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetScaleFactorInStopTest* self = new (ELeave) RVidPlyrSetNGetScaleFactorInStopTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetScaleFactorInStopTest::RVidPlyrSetNGetScaleFactorInStopTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetScaleFactorInStopTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.26  - Set and get scale factor in stopped state"));
	}

// Timer Callback
void RVidPlyrSetNGetScaleFactorInStopTest::DoTimerCallback()
	{
	switch (iState)
		{
		case EVPPlaying:
		    INFO_PRINTF1(_L("Timer enabled"));
            iState = EVPStopped;
			Fsm();
			break; 
		case EVPStopped:
            iState = EVPStopped1;
			Fsm();
			break; 
		default:
			ASSERT(EFalse); // nothing else expected
		}
	}
	
/**
 * RVidPlyrSetNGetScaleFactorInStopTest::Fsm
 */
void RVidPlyrSetNGetScaleFactorInStopTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                iState = EVPPlaying;
                SetExpectedState(EVPStopped, KErrNone);
                iTimer->Start(KDelay, KDelay, iCallBack);
                break;
            case EVPStopped:
                INFO_PRINTF1(_L("iUtility->Stop()"));
                iUtility->Stop();
                iState = EVPStopped;
                SetExpectedState(EVPStopped1, KErrNone);
                break;
            case EVPStopped1:
                iTimer->Cancel(); 
                TReal32 widthScaleFactor = 0.0f;
                TReal32 heightScaleFactor = 0.0f;
                TBool antiAliasFilter = EFalse;
                // Call SetScaleFactorL() without setting any rotation value 
                INFO_PRINTF3(_L("iUtility->SetScaleFactorL(%f, %f ETrue)"),KScaleWidth, KScaleHeight);
                TRAP(iError, iUtility->SetScaleFactorL(KScaleWidth, KScaleHeight, ETrue));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
                    {
                    // call GetScaleFactorL() 
                    INFO_PRINTF1(_L("iUtility->GetScaleFactorL()"));
                    TRAP(iError, iUtility->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Scale factor is not supported currently"));
                        StopTest(iError, EKnownFailure);   
                        }  
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get the scale factor without setting any factor after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0101

// static NewL()
RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest* RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest* self = new (ELeave) RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.27 - Get the scale factor without setting any factor"));
	}

/**
 * RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::Fsm
 */
void RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 widthScaleFactor = 0.0f;
                TReal32 heightScaleFactor = 0.0f;
                TBool antiAliasFilter = EFalse;
                // Call SetScaleFactorL() without setting any rotation value 
                INFO_PRINTF1(_L("iUtility->GetScaleFactorL()"));
                TRAP(iError, iUtility->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
            		{
                    INFO_PRINTF1(_L("Get Scale factor is not supported currently"));
            		StopTest(iError, EKnownFailure);   
            		}            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set and get the scaling factor before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0102

// static NewL()
RVidPlyrSetNGetScalingFactorB4OpngFilTest* RVidPlyrSetNGetScalingFactorB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetScalingFactorB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetScalingFactorB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetScalingFactorB4OpngFilTest::RVidPlyrSetNGetScalingFactorB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetScalingFactorB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.27 - Set and get the scale factor before opening the file"));
	}
	
/**
 * RVidPlyrSetNGetScalingFactorB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetScalingFactorB4OpngFilTest::Fsm()
    {
    // the entire test is done in Idle state, so no state machine is required.
    TReal32 widthScaleFactor = 0.0f;
    TReal32 heightScaleFactor = 0.0f;
    TBool antiAliasFilter = EFalse;
    // Call SetScaleFactorL() without setting any rotation value 
    INFO_PRINTF3(_L("iUtility->SetScaleFactorL(%f, %f ETrue)"),KScaleWidth, KScaleHeight);
    TRAP(iError, iUtility->SetScaleFactorL(KScaleWidth, KScaleHeight, ETrue));
    INFO_PRINTF2(_L("-- Error : %d"), iError);
    if (iError!= KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
	else
        {
        INFO_PRINTF2(_L("SetScaleFactorL() leaves with %d before opening the file"), iError);                            
        // call GetScaleFactorL() 
        INFO_PRINTF1(_L("iUtility->GetScaleFactorL()"));
        TRAP(iError, iUtility->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
        INFO_PRINTF2(_L("-- Error : %d"), iError);
        if (iError!= KErrNotReady)
            {
            StopTest(iError, EFail);   
            }
        else
            {
            INFO_PRINTF1(_L("GetScaleFactorL() leaves with KErrNotReady before opening the file"));
            StopTest(iError, EPass);   
            }  
        }
    }


// Set and get the cropped region after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0103

// static NewL()
RVidPlyrSetNGetCropRegionAftrOpngFilTest* RVidPlyrSetNGetCropRegionAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetCropRegionAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetCropRegionAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetCropRegionAftrOpngFilTest::RVidPlyrSetNGetCropRegionAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetCropRegionAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.28 - Set and get Crop region"));
	}

/**
 * RVidPlyrSetNGetCropRegionAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetCropRegionAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TRect cropRegion(KCropRegionRectLeft, KCropRegionRectTop, KCropRegionRectRight, KCropRegionRectBottom);
	            TRect region;
                // Call SetCropRegionL() 
                INFO_PRINTF1(_L("iUtility->SetCropRegionL()"));
                TRAP(iError, iUtility->SetCropRegionL(cropRegion));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
                    {
                    // call GetCropRegionL() 
                    INFO_PRINTF1(_L("iUtility->GetCropRegionL()"));
                    TRAP(iError, iUtility->GetCropRegionL(region));
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Crop region is not supported currently"));
                        StopTest(iError, EKnownFailure);   
                        }  
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set and get crop region before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0104

// static NewL()
RVidPlyrSetNGetCropRegionB4OpngFilTest* RVidPlyrSetNGetCropRegionB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetCropRegionB4OpngFilTest* self = new (ELeave) RVidPlyrSetNGetCropRegionB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetCropRegionB4OpngFilTest::RVidPlyrSetNGetCropRegionB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetCropRegionB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.28 - Set and get the crop region before opening the file"));
	}
	
/**
 * RVidPlyrSetNGetCropRegionB4OpngFilTest::Fsm
 */
void RVidPlyrSetNGetCropRegionB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    TRect cropRegion(KCropRegionRectLeft, KCropRegionRectTop, KCropRegionRectRight, KCropRegionRectBottom);
    TRect region;
    // Call SetCropRegionL() 
    INFO_PRINTF1(_L("iUtility->SetCropRegionL()"));
    TRAP(iError, iUtility->SetCropRegionL(cropRegion));
    INFO_PRINTF2(_L("-- Error : %d"), iError);
    if (iError!= KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
	else
        {
        INFO_PRINTF2(_L("SetCropRegionL() leaves with %d before opening the file"), iError);                    
        // call GetCropRegionL() 
        INFO_PRINTF1(_L("iUtility->GetCropRegionL()"));
        TRAP(iError, iUtility->GetCropRegionL(region));
        INFO_PRINTF2(_L("-- Error : %d"), iError);
        if (iError!= KErrNotReady)
            {
            StopTest(iError, EFail);   
            }
        else
            {
            INFO_PRINTF1(_L("Both SetCropRegionL() and GetCropRegionL() leaves with KErrNotReady"));
            StopTest(iError, EPass);   
            }  
        }            	
    }


// Get the cropped region without setting any crop region after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0105

// static NewL()
RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest* RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest* self = new (ELeave) RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.29 - Get the crop region without setting the crop region"));
	}

/**
 * RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::Fsm
 */
void RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
	            TRect region;
        		// call GetCropRegionL() 
        		INFO_PRINTF1(_L("iUtility->GetCropRegionL()"));
                TRAP(iError, iUtility->GetCropRegionL(region));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                
#ifdef SYMBIAN_BUILD_GCE
                const TInt KExpectedError = GCEAvailable() ? KErrNone : KErrNotSupported;
#else
				const TInt KExpectedError = KErrNotSupported;
#endif
                
                if (iError != KExpectedError)
                    {
                    INFO_PRINTF3(_L("Get Crop region returned unexpected: %d expected %d"), iError, KExpectedError);
                    StopTest(iError, EFail);   
                    }
                 else
                    {
                    INFO_PRINTF2(_L("GetCropRegionL returned expected: %d"), iError);
                    StopTest(iError, EPass);   
                    }  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get crop region before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0106

// static NewL()
RVidPlyrGetCropRegionWthOutSetB4OpngFilTest* RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCropRegionWthOutSetB4OpngFilTest* self = new (ELeave) RVidPlyrGetCropRegionWthOutSetB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::RVidPlyrGetCropRegionWthOutSetB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.29 - Get the crop region before opening the file"));
	}
	
/**
 * RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::Fsm
 */
void RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    TRect region;
	// call GetCropRegionL() 
	INFO_PRINTF1(_L("iUtility->GetCropRegionL()"));
    TRAP(iError, iUtility->GetCropRegionL(region));
    INFO_PRINTF2(_L("-- Error : %d"), iError);
    if (iError!= KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
     else
        {
        INFO_PRINTF1(_L("GetCropRegionL() leaves with KErrNotReady"));
        StopTest(iError, EPass);   
        }  
    }


// Get the the number of metadata entries after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0107

// static NewL()
RVidPlyrGetNumMetaDataAftrOpngFilTest* RVidPlyrGetNumMetaDataAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetNumMetaDataAftrOpngFilTest* self = new (ELeave) RVidPlyrGetNumMetaDataAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetNumMetaDataAftrOpngFilTest::RVidPlyrGetNumMetaDataAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetNumMetaDataAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.30 - Get the number of metadata entries after opening the file"));
	}

/**
 * RVidPlyrGetNumMetaDataAftrOpngFilTest::Fsm
 */
void RVidPlyrGetNumMetaDataAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
	             // there are zero meta data entries now.
                TInt numOfMetaData = -1;
                TRAP(iError, numOfMetaData = iUtility->NumberOfMetaDataEntriesL() );
                INFO_PRINTF3(_L("Error : %d Entries = %d"), iError, numOfMetaData);
                if (iError != KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
                 else
                    {
                    INFO_PRINTF1(_L("There is no support for number of metadata currently"));
                    StopTest(iError, EKnownFailure);   
                    }  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

  
// Get the number of metadata entries before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0111

// static NewL()
RVidPlyrGetNumberOfMetadataB4OpngFilTest* RVidPlyrGetNumberOfMetadataB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetNumberOfMetadataB4OpngFilTest* self = new (ELeave) RVidPlyrGetNumberOfMetadataB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetNumberOfMetadataB4OpngFilTest::RVidPlyrGetNumberOfMetadataB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetNumberOfMetadataB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.30 - Get the number of metadata entries before opening the file"));
	}
	
/**
 * RVidPlyrGetNumberOfMetadataB4OpngFilTest::Fsm
 */
void RVidPlyrGetNumberOfMetadataB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    TInt numOfMetaData = -1;
    TRAP(iError, numOfMetaData = iUtility->NumberOfMetaDataEntriesL());
    INFO_PRINTF3(_L("Error : %d Entries = %d"), iError, numOfMetaData);
    if (iError != KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
     else
        {
        INFO_PRINTF1(_L("Device is not ready"));
        StopTest(iError, EPass);   
        }  
    }


// Get metadata entries after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0112

// static NewL()
RVidPlyrGetMetaDataEntryAftrOpngFilTest* RVidPlyrGetMetaDataEntryAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetMetaDataEntryAftrOpngFilTest* self = new (ELeave) RVidPlyrGetMetaDataEntryAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetMetaDataEntryAftrOpngFilTest::RVidPlyrGetMetaDataEntryAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetMetaDataEntryAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.31 - Get metadata entries after opening the file"));
	}

/**
 * RVidPlyrGetMetaDataEntryAftrOpngFilTest::Fsm
 */
void RVidPlyrGetMetaDataEntryAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // attempt to get an entry when no entry exists.
                TRAP(iError, iUtility->MetaDataEntryL(1));
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
                 else
                    {
                    INFO_PRINTF1(_L("There is no support for metadata entry currently"));                    
                    StopTest(iError, EKnownFailure);   
                    }  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get metadata entries before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0116

// static NewL()
RVidPlyrGetMetadataEntryB4OpngFilTest* RVidPlyrGetMetadataEntryB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetMetadataEntryB4OpngFilTest* self = new (ELeave) RVidPlyrGetMetadataEntryB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetMetadataEntryB4OpngFilTest::RVidPlyrGetMetadataEntryB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetMetadataEntryB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.31 - Get metadata entries before opening the file"));
	}
	
/**
 * RVidPlyrGetMetadataEntryB4OpngFilTest::Fsm
 */
void RVidPlyrGetMetadataEntryB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    // attempt to get an entry when no entry exists.
    TRAP(iError, iUtility->MetaDataEntryL(1));
    INFO_PRINTF2(_L("Error : %d"), iError);
    if (iError != KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
     else
        {
        INFO_PRINTF1(_L("Device is not ready"));
        StopTest(iError, EPass);   
        }  
    }

// Get the controller implementation information after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0117

// static NewL()
RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest* RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest* self = new (ELeave) RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.32 - Get the controller info after opening the file"));
	}

/**
 * RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::Fsm
 */
void RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                const CMMFControllerImplementationInformation* info = NULL;
                TRAP(iError, info = &iUtility->ControllerImplementationInformationL());
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNone)
                    {
                    StopTest(iError, EFail);   
                    }
                 else
                    {
                    INFO_PRINTF1(_L("Display the controller Info...."));
                    if (info->DisplayName().Compare(KVideoTestControllerName)==0)
                		{
                		const RMMFFormatImplInfoArray& playFormats = info->PlayFormats();
                		INFO_PRINTF1(_L("Play Format Support"));
                		
                		TBuf<256> ext;
                		TBuf<256> mime;
                		TInt i,j;
                		if (playFormats.Count() >0 )
                			{
                			for (i = 0;i<playFormats.Count();i++)
                				{
                				INFO_PRINTF2(_L("supported format %d: "),i);
                				const CDesC8Array& extensions = playFormats[i]->SupportedFileExtensions();
                				for (j = 0;j < extensions.Count(); j++)
                					{
                					ext.Copy(extensions[j]);
                					INFO_PRINTF2(_L("extension: %S"), &ext);
                					}
                				const CDesC8Array& mimeTypes = playFormats[i]->SupportedMimeTypes();
                				for (j = 0;j < mimeTypes.Count(); j++)
                					{
                					mime.Copy(mimeTypes[j]);
                					INFO_PRINTF2(_L("mime type: %S"), &mime);
                					}
                				
                				}
                			}
                		}
                        StopTest(iError, EPass);   
                    }  
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Get controller info before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0121

// static NewL()
RVidPlyrGetCntrlrInfoB4OpngFilTest* RVidPlyrGetCntrlrInfoB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrGetCntrlrInfoB4OpngFilTest* self = new (ELeave) RVidPlyrGetCntrlrInfoB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrGetCntrlrInfoB4OpngFilTest::RVidPlyrGetCntrlrInfoB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrGetCntrlrInfoB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.32 - Get controller info before opening the file"));
	}
	
/**
 * RVidPlyrGetCntrlrInfoB4OpngFilTest::Fsm
 */
void RVidPlyrGetCntrlrInfoB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    TRAP(iError, &iUtility->ControllerImplementationInformationL());
    INFO_PRINTF2(_L("Error : %d"), iError);
    if (iError != KErrNotReady)
        {
        StopTest(iError, EFail);   
        }
     else
        {
        INFO_PRINTF1(_L("Device is not ready"));
        StopTest(iError, EPass);   
        }  
    }

// Stop and start direct screen access after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0122

// static NewL()
RVidPlyrStartDrctScrnAcsAftrOpngFilTest* RVidPlyrStartDrctScrnAcsAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStartDrctScrnAcsAftrOpngFilTest* self = new (ELeave) RVidPlyrStartDrctScrnAcsAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStartDrctScrnAcsAftrOpngFilTest::RVidPlyrStartDrctScrnAcsAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStartDrctScrnAcsAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.33 - Stop and start direct screen access after opening the file"));
	}

/**
 * RVidPlyrStartDrctScrnAcsAftrOpngFilTest::Fsm
 */
void RVidPlyrStartDrctScrnAcsAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TBool valid = EFalse;
                TRAP(iError, iUtility->StopDirectScreenAccessL());
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNone)
                    {
                    INFO_PRINTF1(_L("Unable to stop direct screen access"));
                    StopTest(iError, EFail);   
                    }
                else
                    {
                    INFO_PRINTF1(_L("Stopped the direct screen access successfully"));
                    valid = ETrue;
                    } 
                TRAP(iError, iUtility->StartDirectScreenAccessL());
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNone)
                    {
                    INFO_PRINTF1(_L("Unable to start direct screen access"));
                    StopTest(iError, EFail);   
                    }
                 else
                    {
                    INFO_PRINTF1(_L("Started the direct screen access successfully"));
                    if(valid)
                        {
                        StopTest(iError, EPass);      
                        }
                    }   
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Stop and start direct screen access before opening the file
// MM-MMF-VCLNT-CHRTZ-I-0123

// static NewL()
RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest* RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest* self = new (ELeave) RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.34 - Stop and start direct screen access before opening the file"));
	}
	
/**
 * RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::Fsm
 */
void RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::Fsm()
    {
    // The entire test is done in Idle state, so no state machine is required.
    TBool valid = EFalse;
    TRAP(iError, iUtility->StopDirectScreenAccessL());
    INFO_PRINTF2(_L("Error : %d"), iError);
    if (iError != KErrNone)
        {
        INFO_PRINTF1(_L("Unable to stop direct screen access"));
        StopTest(iError, EFail);   
        }
    else
        {
        INFO_PRINTF1(_L("Stopped the direct screen access successfully"));
        valid = ETrue;
        } 
    TRAP(iError, iUtility->StartDirectScreenAccessL());
    INFO_PRINTF2(_L("Error : %d"), iError);
    if (iError != KErrNone)
        {
        INFO_PRINTF1(_L("Unable to start direct screen access"));
        StopTest(iError, EFail);   
        }
     else
        {
        INFO_PRINTF1(_L("Started the direct screen access successfully"));
        if(valid)
            {
            StopTest(iError, EPass);      
            }
        }   
    }
    
    
// Set the display window after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0124

// static NewL()
RVidPlyrSetDsplyWndowAftrOpngFilTest* RVidPlyrSetDsplyWndowAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetDsplyWndowAftrOpngFilTest* self = new (ELeave) RVidPlyrSetDsplyWndowAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetDsplyWndowAftrOpngFilTest::RVidPlyrSetDsplyWndowAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetDsplyWndowAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.35 - Set the display window after opening the file"));
	}

/**
 * RVidPlyrSetDsplyWndowAftrOpngFilTest::Fsm
 */
void RVidPlyrSetDsplyWndowAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TRect rect, clipRect;
                TRAP(iError, iUtility->SetDisplayWindowL(iWs, *iScreenDevice, *iWindow, rect, clipRect));
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNone)
                    {
                    INFO_PRINTF1(_L("Unable to set display window"));
                    StopTest(iError, EFail);   
                    }
                else
                    {
                    INFO_PRINTF1(_L("Set the display window successfully"));
                    StopTest(iError, EPass);      
                    } 
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Set the display window after opening the file with invalid dimension of display window
// MM-MMF-VCLNT-CHRTZ-I-0136

// static NewL()
RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest* RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest* self = new (ELeave) RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.35 - Set the display window after opening the file with invalid display window dimension"));
	InitWservL();
	}

/**
 * RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::InitWservL()
 */
void RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::InitWservL()
    {
    TInt err = iWs1.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreenDevice1 = new (ELeave) CWsScreenDevice(iWs1); // make device for this session
    User::LeaveIfError(iScreenDevice1->Construct()); // and complete its construction

    RWindowGroup iRootWindow1 = RWindowGroup(iWs1);
    User::LeaveIfError(iRootWindow1.Construct((TUint32)this, ETrue));

    iWindow1 = new(ELeave) RWindow(iWs1);
    User::LeaveIfError(((RWindow*)iWindow1)->Construct(iRootWindow1,((TUint32)(this)) + 1));
    iWindow1->SetExtent(TPoint(-1,-1), TSize(0,0));
    iWindow1->SetVisible(ETrue);
    iWindow1->Activate();
    iWs1.Flush();
    }

/**
 * RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::DeInitWserv
 */
void RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::DeInitWserv()
    {
    if (iWindow1)
        {
        iWindow1->Close();
        delete iWindow1;
        iWindow1 = NULL;
        }
    iRootWindow1.Close();
    delete iScreenDevice1;
    iScreenDevice1 = NULL;
    iWs1.Flush();
    iWs1.Close();
    }

// cleanup at end
void RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::CloseTest()
	{
	// Call base class' CloseTest() to clear other objects.
	RTestBaseVideoPlay::CloseTest();
	// Destroy Window server objects
    DeInitWserv();
	}
/**
 * RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::Fsm
 */
void RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TRect rect, clipRect;
                TRAP(iError, iUtility->SetDisplayWindowL(iWs1, *iScreenDevice1, *iWindow1, rect, clipRect));
                INFO_PRINTF2(_L("Error : %d"), iError);
                if (iError != KErrNone)
                    {
                    INFO_PRINTF1(_L("Unable to set display window"));
                    StopTest(iError, EFail);   
                    }
                else
                    {
                    INFO_PRINTF1(_L("Set the display window successfully"));
                    StopTest(iError, EPass);      
                    } 
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


//Create video player multiple times
// MM-MMF-VCLNT-CHRTZ-I-0150
// static NewL()
RVPlyrCrtMltpleTimesTest* RVPlyrCrtMltpleTimesTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVPlyrCrtMltpleTimesTest* self = new (ELeave) RVPlyrCrtMltpleTimesTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVPlyrCrtMltpleTimesTest::RVPlyrCrtMltpleTimesTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVPlyrCrtMltpleTimesTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Create multiple instances of video player"));
	}


// cleanup at end
void RVPlyrCrtMltpleTimesTest::CloseTest()
	{
	delete iUtility2;
	iUtility2 = NULL;
	
	// call base class' CloseTest()
	RTestBaseVideoPlay::CloseTest();
	}
	
/**
 * RVPlyrCrtMltpleTimesTest::Fsm
 */
void RVPlyrCrtMltpleTimesTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                TRect rect, clipRect;
                INFO_PRINTF1(_L("Create the video player utility again"));
                TRAP(iError, iUtility2 = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone, iWs, *iScreenDevice, *iWindow, rect, clipRect));
                INFO_PRINTF2(_L("The Error returned is %d"),iError);
                // nothing to do, stop the test, default is EPass
                StopTest(iError);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Open the video file multiple times
// MM-MMF-VCLNT-CHRTZ-I-0151
// static NewL()
RVidPlyrOpnFileMultTest* RVidPlyrOpnFileMultTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrOpnFileMultTest* self = new (ELeave) RVidPlyrOpnFileMultTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrOpnFileMultTest::RVidPlyrOpnFileMultTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrOpnFileMultTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Open the video file multiple times"));
	}

/**
 * RVidPlyrOpnFileMultTest::Fsm
 */
void RVidPlyrOpnFileMultTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Open the file again
                INFO_PRINTF1(_L("Open the file again"));
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening1;
                SetExpectedState(EVPOpenComplete1, KErrNone);
                break;
            case EVPOpenComplete1:
                StopTest(iError);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Close a file without Opening any file
// MM-MMF-VCLNT-CHRTZ-I-0152
// static NewL()
RVidPlyrCloseFileWithoutopngTest* RVidPlyrCloseFileWithoutopngTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrCloseFileWithoutopngTest* self = new (ELeave) RVidPlyrCloseFileWithoutopngTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrCloseFileWithoutopngTest::RVidPlyrCloseFileWithoutopngTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrCloseFileWithoutopngTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Close a video file without opening it"));
	}

/**
 * RVidPlyrCloseFileWithoutopngTest::Fsm
 */
void RVidPlyrCloseFileWithoutopngTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Close the video file
                INFO_PRINTF1(_L("iUtility->Close()"));
                TRAP(iError, iUtility->Close());
                StopTest(iError);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Prepare the clip of the video file multiple times
// MM-MMF-VCLNT-CHRTZ-I-0153
// static NewL()
RVidPlyrPrepareMultTest* RVidPlyrPrepareMultTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPrepareMultTest* self = new (ELeave) RVidPlyrPrepareMultTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPrepareMultTest::RVidPlyrPrepareMultTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPrepareMultTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Prepare the video clips multiple times"));
	}

/**
 * RVidPlyrPrepareMultTest::Fsm
 */
void RVidPlyrPrepareMultTest::Fsm()
    {
   if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare iUtility
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Prepare again 
                INFO_PRINTF1(_L("iUtility->Prepare() again"));
                iUtility->Prepare();
                // set states
                iState = EVPPreparing1;
                SetExpectedState(EVPPrepareComplete1, KErrNone);
                break;
            case EVPPrepareComplete1:
            	// nothing to do, stop the test, default is EPass
                StopTest(iError);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
    
// Play a corrupted file
// MM-MMF-VCLNT-CHRTZ-I-0154

// static NewL()
RVidPlyrPlayCorruptedFileTest* RVidPlyrPlayCorruptedFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayCorruptedFileTest* self = new (ELeave) RVidPlyrPlayCorruptedFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayCorruptedFileTest::RVidPlyrPlayCorruptedFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayCorruptedFileTest::DoKickoffTestL()
	{
	TPtrC corruptfilename;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# 1.1 - Play a corrupted video file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("corruptfilename"), corruptfilename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(corruptfilename);
	}

/**
 * RVidPlyrPlayCorruptedFileTest::Fsm
 */
void RVidPlyrPlayCorruptedFileTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNotSupported);
                break;
            case EVPOpenComplete:
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNotReady);
                // Prepare iUtility
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                break;
            case EVPPrepareComplete:
                // Play the file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNotReady);
                break;
            case EVPPlayComplete:
                INFO_PRINTF1(_L("Play a corrupt file, returns KErrCorrupt from MvpuoOpenComplete."));
                StopTest(iError, EPass);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Open a video file without having any extension
// MM-MMF-VCLNT-CHRTZ-I-0155
// static NewL()
RVidPlyrOpnFileWthOutExtnTest* RVidPlyrOpnFileWthOutExtnTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrOpnFileWthOutExtnTest* self = new (ELeave) RVidPlyrOpnFileWthOutExtnTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrOpnFileWthOutExtnTest::RVidPlyrOpnFileWthOutExtnTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrOpnFileWthOutExtnTest::DoKickoffTestL()
	{
	TPtrC invalidfile;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#  - Open a video file without any extension"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("invalidfile"), invalidfile))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(invalidfile);
	}

/**
 * RVidPlyrOpnFileWthOutExtnTest::Fsm
 */
void RVidPlyrOpnFileWthOutExtnTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                StopTest(iError);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play a video file with invalid start and end points
// MM-MMF-VCLNT-CHRTZ-I-0156
// static NewL()
RVidPlyrPlayFileInvStEndptsTest* RVidPlyrPlayFileInvStEndptsTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayFileInvStEndptsTest* self = new (ELeave) RVidPlyrPlayFileInvStEndptsTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayFileInvStEndptsTest::RVidPlyrPlayFileInvStEndptsTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayFileInvStEndptsTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# 1.1  - Open a video file with invalid start and End points"));
	}

/**
 * RVidPlyrPlayFileInvStEndptsTest::Fsm
 */
void RVidPlyrPlayFileInvStEndptsTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                     StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Prepare the video file
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                break;
            case EVPPrepareComplete:
                // Play the video file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play(TTimeIntervalMicroSeconds(KPlayEndTime), TTimeIntervalMicroSeconds(KPlayStartTime));
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNotSupported);
                break;
            case EVPPlayComplete:
                StopTest(iError, EPass);
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Call stop after completion of video Play 
// MM-MMF-VCLNT-CHRTZ-I-0157

// static NewL()
RVidPlyrStopPlayAfterComplnOfPlayTest* RVidPlyrStopPlayAfterComplnOfPlayTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrStopPlayAfterComplnOfPlayTest* self = new (ELeave) RVidPlyrStopPlayAfterComplnOfPlayTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrStopPlayAfterComplnOfPlayTest::RVidPlyrStopPlayAfterComplnOfPlayTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrStopPlayAfterComplnOfPlayTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# - Call stop after completion of video Play "));
	}

/**
 * RVidPlyrStopPlayAfterComplnOfPlayTest::Fsm
 */
void RVidPlyrStopPlayAfterComplnOfPlayTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNone);
                // Prepare iUtility
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                break;
            case EVPPrepareComplete:
                // Play the file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNone);
                break;
            case EVPPlayComplete:
                iUtility->Stop();
                INFO_PRINTF1(_L("iUtility->Stop()"));
                StopTest();
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set the video frame rate with some invalid value
// MM-MMF-VCLNT-CHRTZ-I-0158
// static NewL()
RVidPlyrSetVidFrameRateInvValTest* RVidPlyrSetVidFrameRateInvValTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetVidFrameRateInvValTest* self = new (ELeave) RVidPlyrSetVidFrameRateInvValTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetVidFrameRateInvValTest::RVidPlyrSetVidFrameRateInvValTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetVidFrameRateInvValTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.16 - Set video frame rate after opening the file"));
	}

/**
 * RVidPlyrSetVidFrameRateInvValTest::Fsm
 */
void RVidPlyrSetVidFrameRateInvValTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 framesPerSecond = -1;
                INFO_PRINTF1(_L("iUtility->SetVideoFrameRateL()"));
                TRAP(iError, iUtility->SetVideoFrameRateL(framesPerSecond));
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);  
                    }
                else
                    {
                    INFO_PRINTF2(_L("SetVideoFrameRateL() leaves with %d "), iError);
                    StopTest(iError, EPass);     
                    }
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set and get the priority with invalid value after opening video file 
// MM-MMF-VCLNT-CHRTZ-I-0159
// static NewL()
RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest* RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest* self = new (ELeave) RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.20  - Set the priority with some invalid value after opening video file"));
	}

/**
 * RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::Fsm
 */
void RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call SetPriorityL() 
                INFO_PRINTF1(_L("iUtility->SetPriorityL()"));
                TInt priority = -1000;
        		INFO_PRINTF2(_L("Priority set is %d"), priority);
                TRAP(iError, iUtility->SetPriorityL(priority, EMdaPriorityPreferenceQuality));
                {
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
                else
                    {
                    TInt getThePrty = 0;
                    TMdaPriorityPreference getThePref; 
                    INFO_PRINTF1(_L("Set the Priority successfully"));  
                    TRAP(iError, iUtility->PriorityL(getThePrty, getThePref));
                    if(iError !=KErrNone)
                        {
                        StopTest(iError, EFail);
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Got the Priority successfully"));   
                        if(getThePrty == priority)
                            {
                            INFO_PRINTF1(_L("Priority set and got matches"));
                            INFO_PRINTF4(_L("Priority set = %d , Priority got =%d, Preference = %d"), priority, getThePrty, getThePref);
                            StopTest(iError, EPass);   
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Priority set and got doesn't match"));
                            INFO_PRINTF3(_L("Priority set = %d and Priority got =%d"), priority, getThePrty);
                            StopTest(iError, EFail);   
                            }
                        }
                    }            		    
                }                    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
 
// Set the playback balance with some invalid value and get the balance after opening video file
// MM-MMF-VCLNT-CHRTZ-I-0160
// static NewL()
RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest* RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest* self = new (ELeave) RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.22  - Set the playback balance with Invalid value and get it after opening video file"));
	}

/**
 * RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::Fsm
 */
void RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call SetBalanceL() 
                INFO_PRINTF1(_L("iUtility->SetBalanceL()"));
                TInt balance = KMMFBalanceMaxRight+10;
        		INFO_PRINTF2(_L("Balance to be  set is %d"), balance);
                TRAP(iError, iUtility->SetBalanceL(balance));
    		    if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    TInt getTheBalance = 0;
                    INFO_PRINTF1(_L("Set the Balance successfully"));  
                    getTheBalance = iUtility->Balance();
                    INFO_PRINTF2(_L("Playback balance got is %d"), getTheBalance);   
                    if(getTheBalance == KMMFBalanceMaxRight)
                        {
                        INFO_PRINTF1(_L("Balance set and got matches"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Balance set and got doesn't match"));
                        INFO_PRINTF3(_L("Balance set = %d , Balance got =%d"), balance, getTheBalance);
                        StopTest(iError, EFail);   
                        }
                    }            		    
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Play an audio file
// MM-MMF-VCLNT-CHRTZ-I-0161

// static NewL()
RVidPlyrPlayAudioFileTest* RVidPlyrPlayAudioFileTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrPlayAudioFileTest* self = new (ELeave) RVidPlyrPlayAudioFileTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrPlayAudioFileTest::RVidPlyrPlayAudioFileTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrPlayAudioFileTest::DoKickoffTestL()
	{
	TPtrC audiofile;
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE# 1.1 - Play an audio file"));
	//Getting corruptfilename from ini file
	if(!GetStringFromConfig(iSectName, _L("audiofile"), audiofile))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	
	GetDriveName(iFilename1);
	iFilename1.Append(audiofile);
	}

/**
 * RVidPlyrPlayAudioFileTest::Fsm
 */
void RVidPlyrPlayAudioFileTest::Fsm()
    {
    if (ValidateState())
        {
        //TInt err = KErrNone;
        switch (iState)
            {
            case EVPIdle:
                // Open iUtility
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename1);
                TRAP(iError, iUtility->OpenFileL(iFilename1));
                // set states
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNotSupported);
                break;
            case EVPOpenComplete:
                // set states
                iState = EVPPreparing;
                SetExpectedState(EVPPrepareComplete, KErrNotReady);
                // Prepare iUtility
                INFO_PRINTF1(_L("iUtility->Prepare()"));
                iUtility->Prepare();
                break;
            case EVPPrepareComplete:
                // Play the file
                INFO_PRINTF1(_L("iUtility->Play()"));
                iUtility->Play();
                // set states
                iState = EVPPlaying;
                SetExpectedState(EVPPlayComplete, KErrNotReady);
                break;
            case EVPPlayComplete:
                INFO_PRINTF1(_L("Playing one audio file receives KErrNotSupported through MvpuoOpenComplete"));            
                StopTest(iError, EPass);
                break;
            }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }


// Set the rotation with some invalid value and get the rotation after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0162

// static NewL()
RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest* RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.25 - Set and get the rotation with invalid value"));
	}

/**
 * RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                // Call RotationL() without setting any rotation value 
                INFO_PRINTF1(_L("iUtility->RotationL()"));
                TVideoRotation getrotation = EVideoRotationNone;
                TVideoRotation setrotation = TVideoRotation(-1);
                TRAP(iError, getrotation = iUtility->RotationL());
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);   
                    }
        		INFO_PRINTF2(_L("Rotation applied to the current image is %d"), getrotation);
        		// Call SetRotationL()
        		INFO_PRINTF1(_L("iUtility->SetRotationL()"));
                TRAP(iError, iUtility->SetRotationL(setrotation));
    		    if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);    
                    }
        		else
                    {
                    // Again call RotationL() after setting the rotation value
                    INFO_PRINTF1(_L("iUtility->RotationL()"));
                    TRAP(iError, getrotation = iUtility->RotationL());
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF2(_L("Rotation set to the current image is %d"), getrotation);   
                        if(getrotation == setrotation)
                            {
                            INFO_PRINTF1(_L("Rotation is successful"));
                            StopTest(iError, EPass);   
                            }
                        else
                            {
                            INFO_PRINTF1(_L("Set and get rotation are not supported"));
                            StopTest(iError, EKnownFailure);   
                            }  
                        }

                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set the scale factor with some invalid value and get it after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0163

// static NewL()
RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest* RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.26 - Set scale factor with some invalid value and get it"));
	}

/**
 * RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TReal32 widthScaleFactor = 0.0f;
                TReal32 heightScaleFactor = 0.0f;
                TBool antiAliasFilter = EFalse;
                // Call SetScaleFactorL() without setting any rotation value 
                INFO_PRINTF3(_L("iUtility->SetScaleFactorL(%f, %f ETrue)"),KScaleWidth, KScaleHeight);
                TRAP(iError, iUtility->SetScaleFactorL(-1, -1, ETrue));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
                    {
                    // call GetScaleFactorL() 
                    INFO_PRINTF1(_L("iUtility->GetScaleFactorL()"));
                    TRAP(iError, iUtility->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else if((widthScaleFactor == KScaleWidth) && (heightScaleFactor == KScaleHeight) && antiAliasFilter)
                        {
                        StopTest(iError, EPass);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Set / Get scale factor is not supported currently"));
                        StopTest(iError, EKnownFailure);   
                        }  
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }

// Set the cropped with some invalid value and get the region after opening the file
// MM-MMF-VCLNT-CHRTZ-I-0164

// static NewL()
RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest* RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::NewL(const TDesC& aStepName, const TDesC& aSectName)
	{
	RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest* self = new (ELeave) RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest(aStepName, aSectName);
	return self;
	}

// private constructor
RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest(const TDesC& aStepName, const TDesC& aSectName) :
	RTestBaseVideoPlay(aStepName, aSectName)
	{
	}
	
// start test
void RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("USECASE#VIDEOPLAYERUTILITY 1.28 - Set the crop region to some invalid value and get the Crop region"));
	}

/**
 * RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::Fsm
 */
void RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::Fsm()
    {
    if (ValidateState())
        {
        switch (iState)
            {
            case EVPIdle:
                // Open the video file
                INFO_PRINTF2(_L("iUtility->OpenFileL() %S"), &iFilename);
                TRAP(iError, iUtility->OpenFileL(iFilename));
                if (iError!= KErrNone)
                    {
                    StopTest(iError, EFail);  
                    }
                iState = EVPOpening;
                SetExpectedState(EVPOpenComplete, KErrNone);
                break;
            case EVPOpenComplete:
                TRect cropRegion(0, 0, 0, 0);
	            TRect region;
                // Call SetCropRegionL() 
                INFO_PRINTF1(_L("iUtility->SetCropRegionL()"));
                TRAP(iError, iUtility->SetCropRegionL(cropRegion));
                INFO_PRINTF2(_L("-- Error : %d"), iError);
                if (iError!= KErrNotSupported)
                    {
                    StopTest(iError, EFail);   
                    }
        		else
                    {
                    // call GetCropRegionL() 
                    INFO_PRINTF1(_L("iUtility->GetCropRegionL()"));
                    TRAP(iError, iUtility->GetCropRegionL(region));
                    INFO_PRINTF2(_L("-- Error : %d"), iError);
                    if (iError!= KErrNotSupported)
                        {
                        StopTest(iError, EFail);   
                        }
                    else
                        {
                        INFO_PRINTF1(_L("Set/Get Crop region is not supported currently"));
                        StopTest(iError, EKnownFailure);   
                        }  
                    }            	
                break;
             }
        }
	else
        {
    	StopTest(iError, EFail);
        }
    }
