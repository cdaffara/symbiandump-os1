// tsi_mvs_apr.cpp

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

#include "tsi_mvs_apr.h"


// statis NewL()
RTestMVSAgentAPRRegOnce4SameEvent* RTestMVSAgentAPRRegOnce4SameEvent::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegOnce4SameEvent* self = new (ELeave) RTestMVSAgentAPRRegOnce4SameEvent(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegOnce4SameEvent::RTestMVSAgentAPRRegOnce4SameEvent(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iSetPriority=0;
	}
	
// start test
void RTestMVSAgentAPRRegOnce4SameEvent::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("One-time registration for same event by a player test."));
	}
	
// FsmL
void RTestMVSAgentAPRRegOnce4SameEvent::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{  	                                                                                                                                                                                                                                                               
				PrepareState(EVideoOpening, EVidPlayerOpening, KErrNone, ETestVideoPlay);
				// Open iMVSVideoPlayAgent1
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL(%S)"), &iFilename1);
				break;
				}
            case EVidPlayerOpening:
            	{
            	// Check that the video file has an audio stream.
            	// This test relies on there being audio with the video, otherwise the audio agent
            	// will not preempt the video agent.
            	TBool audioEnabled = EFalse;
            	TRAP(iError, audioEnabled = iMVSVideoPlayAgent1->AudioEnabledL());
            	if(iError)
            		{
            		ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->AudioEnabledL() left with error : %d"), iError);
            		StopTest(iError, EFail);
            		break;
            		}
            	if(!audioEnabled)
            		{
            		ERR_PRINTF2(_L("ERROR : audio not enabled in file %S"), &iFilename1);
            		StopTest(iError, EInconclusive);
            		break;
            		}

            	PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
            	if(!iSetPriority)
	            	{         	
					// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
					INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
					TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
					if (iError != KErrNone)
						{
						ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
						StopTest(iError, EFail);
						}                      	
	              	iSetPriority=ETrue;
	              	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying, EVidPlayerOpenComplete, KErrNone, ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
					PlayL(*iMVSVideoPlayAgent1);
	            	}
              	// Open iMVSAudioPlayAgent
				PrepareState(EAudioOpening, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	// Play iMVSAudioPlayAgent
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				TInt retErr;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete, KErrNone, ETestAudioPlay, 1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay, 2);
				break;
				}
            case EAudPlayerPlayComplete:
			case EVidPlayerResourceAvailable:
				{
				// wait for video to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent1"));
				// Play iMVSVideoPlayAgent1
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
            	PlayL(*iMVSVideoPlayAgent1);
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay);				         				
				break;
				}
			case EVidPlayerPlayComplete:
				{
				// iMVSVideoPlayAgent1 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1 Play Complete"));
				StopTest(KErrNone, EPass);
				break;
				}
            }
        }
	}
   
/**
 *
 *		RTestMVSAgentAPRRegB4Open
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRRegB4Open* RTestMVSAgentAPRRegB4Open::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegB4Open* self = new (ELeave) RTestMVSAgentAPRRegB4Open(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegB4Open::RTestMVSAgentAPRRegB4Open(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=0;
	}
	
// start test
void RTestMVSAgentAPRRegB4Open::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Register before opening Video play Agent and receive notification of a resource available event."));
	}
	
// FsmL
void RTestMVSAgentAPRRegB4Open::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}				 	                                                                                                                                                                                                                                                               
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
				// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}                      	
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpening, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}				
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	if(!iFlag)
	            	{            	
	            	// Set ExpectedEvent and change the State
	            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
	            	// Play iMVSAudioPlayAgent
	            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
					iMVSAudioPlayAgent->Play();
	              	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying,EAudPlayerOpenComplete, KErrNone,ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
					iFlag=ETrue;
					PlayL(*iMVSVideoPlayAgent1);
	            	} 
            	iFlag=EFalse;
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay,1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable,KErrNone,ETestVideoPlay,2);
				break;
				}
			case EAudPlayerPlayComplete:
			case EVidPlayerResourceAvailable:
				{
				// wait for video to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent1"));
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	// Play iMVSVideoPlayAgent1
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
				PlayL(*iMVSVideoPlayAgent1);
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay);				         				
				break;
				}
			case EVidPlayerPlayComplete:
				{
				// iMVSVideoPlayAgent1 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1 Play Complete"));
				StopTest(KErrNone, EPass);
				break;
				}
            }
        }

	}

/**
 *
 *		RTestMVSAgentAPRRegAfterOpenB4Play
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRRegAfterOpenB4Play* RTestMVSAgentAPRRegAfterOpenB4Play::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegAfterOpenB4Play* self = new (ELeave) RTestMVSAgentAPRRegAfterOpenB4Play(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegAfterOpenB4Play::RTestMVSAgentAPRRegAfterOpenB4Play(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=0;
	}
	
// start test
void RTestMVSAgentAPRRegAfterOpenB4Play::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Register (after Open and before Play of video) , receive notification of a resource available event"));
	}
	
// FsmL
void RTestMVSAgentAPRRegAfterOpenB4Play::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError,EFail);
					}            	
				TInt retErr;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);					
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
            	if(!iFlag)
	            	{
	            	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
					iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);					 
	            	}
            	iFlag=EFalse;
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpening, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}				
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				INFO_PRINTF1(_L("	iActiveAgent = ETestVideoPlay;"));
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete, KErrNone, ETestAudioPlay, 1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay, 2);
				break;
				}
            case EAudPlayerPlayComplete:
			case EVidPlayerResourceAvailable:
				{
				// wait for video to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent1"));
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	// Play iMVSVideoPlayAgent1
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
                PlayL(*iMVSVideoPlayAgent1);				  
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay);				         				
				break;
				}
			case EVidPlayerPlayComplete:
				{
				// iMVSVideoPlayAgent1 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1 Play Complete"));
				StopTest(KErrNone, EPass);
				break;
				}
				
            }
        }

	}


/**
 *
 *		RTestMVSAgentAPRRegAftrHpClntCompltPlay
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRRegAftrHpClntCompltPlay* RTestMVSAgentAPRRegAftrHpClntCompltPlay::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegAftrHpClntCompltPlay* self = new (ELeave) RTestMVSAgentAPRRegAftrHpClntCompltPlay(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegAftrHpClntCompltPlay::RTestMVSAgentAPRRegAftrHpClntCompltPlay(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRRegAftrHpClntCompltPlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Register after the highest priority agent (Audio) completes playing and receive the notification of a resource availalble event."));
	}
	
// FsmL
void RTestMVSAgentAPRRegAftrHpClntCompltPlay::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}            	
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);									
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
            	if(!iFlag)
	            	{
	            	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);				  
	            	}
            	iFlag=EFalse;
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpening, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}				
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay);
				break;
				}
			case EAudPlayerPlayComplete:
				{
				if(!iRegistered)
					{				
					TInt retErr=KErrNone;
					// Register iMVSVideoPlayAgent for Notification
					retErr = iMVSVideoPlayAgent1->RegisterForNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
					if( retErr != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}
					iRegistered=ETrue;
					// Set ExpectedEvent and change the State
					PrepareState(EVideoPlaying, EAudPlayerPlayComplete,KErrNone,ETestVideoPlay );
					// Play iMVSVideoPlayAgent1 again
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play() - Again"));
                    PlayL(*iMVSVideoPlayAgent1);				  									
					}
				if(!iFlag)
					{
					// Set ExpectedEvent and change the State
					PrepareState(EAudioPlaying, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay);
					// Play iMVSAudioPlayAgent again
					INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play() - Again"));
					iFlag=ETrue;
					iSecPlay=ETrue;
					iMVSAudioPlayAgent->Play();								
					}
				PrepareState(ENotReady, EErrInUseVideoPlayer1, KErrInUse, ETestVideoPlay);
				break;
				}
            case EErrInUseVideoPlayer1:
            	{
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete1,KErrNone,ETestAudioPlay,1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable,KErrNone,ETestVideoPlay,2);
				break;				            		
            	}
			case EAudPlayerPlayComplete1:
			case EVidPlayerResourceAvailable:
				{
				// wait for video to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent1"));
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	// Play iMVSVideoPlayAgent1
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
                PlayL(*iMVSVideoPlayAgent1);				  				
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay);				         				
				break;
				}
			case EVidPlayerPlayComplete:
				{
				// iMVSVideoPlayAgent1 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1 Play Complete"));
				iSecPlay=EFalse;
				iFlag=EFalse;
				iRegistered=EFalse;
				StopTest(KErrNone, EPass);
				break;
				}        	
            }
        }

	}


/**
 *
 *		RTestMVSAgentAPRRegSamePriorityPlayers
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRRegSamePriorityPlayers* RTestMVSAgentAPRRegSamePriorityPlayers::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegSamePriorityPlayers* self = new (ELeave) RTestMVSAgentAPRRegSamePriorityPlayers(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegSamePriorityPlayers::RTestMVSAgentAPRRegSamePriorityPlayers(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRRegSamePriorityPlayers::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Receive notification by first registered client [In case of same priority players notification is received in the order of registration]"));
	}
	
// FsmL
void RTestMVSAgentAPRRegSamePriorityPlayers::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);									
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
            	if(!iFlag)
	            	{
	            	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);				  	              	
	            	}
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				if(iFlag)
					{
					PrepareState(EAudioOpening, EVidPlayerOpenComplete, KErrNone,ETestAudioPlay);				
					INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
					iFlag=EFalse;
					TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
					if( iError != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
						StopTest(iError, EFail);
						break;
						}	
					}
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent1 for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Open iMVSVideoPlayAgent2
				PrepareState(EVideoOpening, EVidPlayerOpening2,KErrNone,ETestVideoPlay2);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent2, iFilename3));				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent2->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2->OpenFileL "));									
				break;
				}
            case EVidPlayerOpening2:
            	{
            	// set priority of iMVSVideoPlayAgent2 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent2 : %S, EMdaPriorityMin"), &iFilename3);
				TRAP(iError,iMVSVideoPlayAgent2->SetPriorityL(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent2->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}
				PrepareState(EVideoOpened, EVidPlayerOpenComplete2,KErrNone,ETestVideoPlay2);									
	            break;            	
            	}
            case EVidPlayerOpenComplete2:
	            {
	            if(!iFlag)
		            {
	            	// Play iMVSVideoPlayAgent2
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete2, KErrNone,ETestVideoPlay2);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent2->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent2);
		            }
		         iFlag = EFalse;
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer1, KErrInUse,ETestVideoPlay2);
				break;
	            }
            case EErrInUseVideoPlayer1:
            	{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent2 for Notification
				retErr = iMVSVideoPlayAgent2->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent2->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					} 
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay,1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable,KErrNone,ETestVideoPlay,2);									           							
            	break;
            	}            	
			case EAudPlayerPlayComplete:
            case EVidPlayerResourceAvailable:
				{
				// wait for video to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent1"));
				// Play iMVSVideoPlayAgent1
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
                PlayL(*iMVSVideoPlayAgent1);				
				iExpEventCount=2;
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay, 1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable2, KErrNone, ETestVideoPlay2, 2);
				break;
				}
            case EVidPlayerPlayComplete:
            case EVidPlayerResourceAvailable2:
	            {
	            // wait for iMVSVideoPlayAgent2 to complete playback
				INFO_PRINTF1(_L("Resource Notification CallBack Received for iMVSVideoPlayAgent2"));
				PrepareState(EVideoPlaying, EVidPlayerPlayComplete2, KErrNone, ETestVideoPlay2);
            	// Play iMVSVideoPlayAgent2
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent2->Play()"));	
                PlayL(*iMVSVideoPlayAgent2);
	            PrepareState(EVideoOpened, EVidPlayerPlayComplete2, KErrNone, ETestVideoPlay2);
	            break;
	            }
			case EVidPlayerPlayComplete2:
				{
				// iMVSVideoPlayAgent2 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2 Play Complete"));
				iSecPlay=EFalse;
				iFlag=EFalse;
				iRegistered=EFalse;
				StopTest(KErrNone, EPass);
				break;
				}
            }
        }

	}


/**
 *
 *		RTestMVSAgentAPRRegAtPlayAndCancelNfn
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRRegAtPlayAndCancelNfn* RTestMVSAgentAPRRegAtPlayAndCancelNfn::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRRegAtPlayAndCancelNfn* self = new (ELeave) RTestMVSAgentAPRRegAtPlayAndCancelNfn(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRRegAtPlayAndCancelNfn::RTestMVSAgentAPRRegAtPlayAndCancelNfn(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRRegAtPlayAndCancelNfn::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Register at Play and Cancellation of the notification request. (with main use case)"));
	}
	
// FsmL
void RTestMVSAgentAPRRegAtPlayAndCancelNfn::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}            	
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);									
	            break;
	            }
            case EVidPlayerOpenComplete:
            	{
            	if(!iFlag)
	            	{
	            	// Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);					
	            	}
            	iFlag=EFalse;
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpening, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}				
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
				break;
            	}
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Cancel iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->CancelNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->CancelNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->CancelNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay);
				break;
				}
			case EAudPlayerPlayComplete:
				{
				// Just Pass the test as we have received the correct callbacks
				StopTest(KErrNone, EPass);
				break;
				}
		  	case EVidPlayerPlayComplete:
				{
				// we have received notification, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->NotificationReceived"));
				iSecPlay=EFalse;
				iFlag=EFalse;
				iRegistered=EFalse;
				StopTest(iError, EFail);
				break;
				}
            }
        }

	}


/**
 *
 *		RTestMVSAgentAPRTestWillResumePlay
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRTestWillResumePlay* RTestMVSAgentAPRTestWillResumePlay::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRTestWillResumePlay* self = new (ELeave) RTestMVSAgentAPRTestWillResumePlay(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRTestWillResumePlay::RTestMVSAgentAPRTestWillResumePlay(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iHeapSize=2000000;
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRTestWillResumePlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Test for WillResumePlay() using 3 clients.VideoPlayAgent with high priority will resume playback"));
	}
	
// FsmL
void RTestMVSAgentAPRTestWillResumePlay::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent2 low priority
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay2);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent2, iFilename3));				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent2->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2->OpenFileL "));
				PrepareState(EVideoOpening, EVidPlayerOpening2,KErrNone,ETestVideoPlay2);
				break;
				}
            case EVidPlayerOpening2:
            	{
            	// set priority of iMVSVideoPlayAgent2 to EMdaPriorityMin
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent2 : %S, EMdaPriorityMin"), &iFilename3);
				TRAP(iError,iMVSVideoPlayAgent2->SetPriorityL(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent2->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}    
				PrepareState(EVideoOpened, EVidPlayerOpenComplete2,KErrNone,ETestVideoPlay2);									
	            break;
	            }
            case EVidPlayerOpenComplete2:
            	{
            	if(!iFlag)
	            	{
	            	// Play iMVSVideoPlayAgent2
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete2, KErrNone, ETestVideoPlay2);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent2->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent2);					
	            	}
				if(iFlag)
					{
					// Open iMVSVideoPlayAgent1
					PrepareState(EVideoOpening, EVidPlayerOpenComplete2, KErrNone,ETestVideoPlay);				
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL()"));
					iFlag=EFalse;
				    TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
					if( iError != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL(), returned %d"), iError);
						StopTest(iError, EFail);
						break;
						}	
					}				
				// Set ExpectedEvent and change the State
				PrepareState(EVideoOpening, EVidPlayerOpening, KErrNone, ETestVideoPlay);
				break;
            	}
            case EVidPlayerOpening:
	            {
	            // set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}   
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);									
				break;	            	
	            }	            	
            case EVidPlayerOpenComplete:
	            {
	            if(!iFlag)
		            {
		            // Play iMVSVideoPlayAgent1
	              	PrepareState(EVideoPlaying, EVidPlayerOpenComplete, KErrNone, ETestVideoPlay);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
                    PlayL(*iMVSVideoPlayAgent1);					
					iFlag=ETrue;
		            }
	            iFlag=EFalse;
				// Set ExpectedEvent and change the State (LP Video gets -14)
				PrepareState(ENotReady, EErrInUseVideoPlayer1, KErrInUse, ETestVideoPlay2);
				break;					           	
	            }
            case EErrInUseVideoPlayer1:
	            {
	            if(iRegistered)
	        		{
			        iSecPlay = ETrue;
			        }
				if(!iRegistered && !iSecPlay)
					{
		            TInt retErr=KErrNone;
					// Register iMVSVideoPlayAgent for Notification
					retErr = iMVSVideoPlayAgent2->RegisterForNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent2->RegisterForNotification()"));
					if( retErr != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent2->RegisterForNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}
					iRegistered=ETrue;
					// Open iMVSAudioPlayAgent
					// Set ExpectedEvent and change the State
					PrepareState(EAudioOpening, EErrInUseVideoPlayer1, KErrNone,ETestAudioPlay);
					INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
					TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
					if( iError != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
						StopTest(iError, EFail);
						break;
						}
					// Set ExpectedEvent and change the State
					PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
					}
				break;
	            }
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse, ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete, KErrNone, ETestAudioPlay, 1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay, 2);
				break;
				}
			case EAudPlayerPlayComplete:
            case EVidPlayerResourceAvailable:
				{
				TInt retErr=KErrNone;
				// Call iMVSVideoPlayAgent1 WillResumePlay()
		        INFO_PRINTF1(_L("iMVSVideoPlayAgent1->WillResumePlay()"));
		        retErr = iMVSVideoPlayAgent1->WillResumePlay();
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->WillResumePlay() - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Wait for iMVSVideoPlayAgent1 to complete playback
				// initialise and start the timer (5 Sec), just to check if no other event occurs.
                TRAPD(err, iTimerWillResume = CMVSAPRWillResumeTimer::NewL(this, 5000000));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMVSAPRWillResumeTimer::NewL left with error = %d"), err);
                   	StopTest(err, EFail);
                    break;
                    }
                iTimerWillResume->Start();
                PrepareState(ENotReady, EVidPlayerNoNotificationReceived, KErrNone, ETestVideoPlay2);
				break;
				}
            case EVidPlayerNoNotificationReceived:
	            {
	            // Play iMVSVideoPlayAgent1
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
                PlayL(*iMVSVideoPlayAgent1);				 
				iExpEventCount=2;
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay, 1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable2, KErrNone, ETestVideoPlay2, 2);
				break;
				}
            case EVidPlayerPlayComplete:
            case EVidPlayerResourceAvailable2:
	            {
	            // wait for iMVSVideoPlayAgent2 to complete playback
				PrepareState(EVideoPlaying, EVidPlayerPlayComplete2, KErrNone, ETestVideoPlay2);
            	// Play iMVSVideoPlayAgent2
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent2->Play()"));	
                PlayL(*iMVSVideoPlayAgent2);				
	            PrepareState(EVideoOpened, EVidPlayerPlayComplete2, KErrNone, ETestVideoPlay2);
	            break;
	            }
			case EVidPlayerPlayComplete2:
				{
				// iMVSVideoPlayAgent2 Play Complete, so pass the test				
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2 Play Complete"));
				iSecPlay=EFalse;
				iFlag=EFalse;
				iRegistered=EFalse;
				StopTest(KErrNone, EPass);
				break;
				}		
            }
        }
	}

/**
 * set finished on callback from a timer
 */
void RTestMVSAgentAPRTestWillResumePlay::Finished()
    {
    // say that no events have occured after calling WillResumePlay()
    // and can proceed to the next step
    TRAP(iError, FsmL(EVidPlayerNoNotificationReceived, KErrNone));
    if (iError != KErrNone)
	    {
	    ERR_PRINTF2(_L("RTestMVSAgentAPRTestWillResumePlay::FsmL left with error = %d"), iError);
	   	StopTest(iError, EFail);
	    }
    }

//
// CVclntAviAprWillResumeTimer
//

/**
 * NewL
 */
CMVSAPRWillResumeTimer* CMVSAPRWillResumeTimer::NewL(RTestMVSAgentAPRTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMVSAPRWillResumeTimer* self = new (ELeave) CMVSAPRWillResumeTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }    
/**
 * Constructor
 */
CMVSAPRWillResumeTimer::CMVSAPRWillResumeTimer(RTestMVSAgentAPRTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }        
/**
 * Start() - Start the timer
 */
void CMVSAPRWillResumeTimer::Start()
    {
    After(iWaitTime);
    }
/**
 * RunL() - see CActive class for more info
 */
void CMVSAPRWillResumeTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }
    	   
/**
 *
 *		RTestMVSAgentAPRCancelNoResourceNfn
 *
 */
// statis NewL()
RTestMVSAgentAPRCancelNoResourceNfn* RTestMVSAgentAPRCancelNoResourceNfn::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRCancelNoResourceNfn* self = new (ELeave) RTestMVSAgentAPRCancelNoResourceNfn(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRCancelNoResourceNfn::RTestMVSAgentAPRCancelNoResourceNfn(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRCancelNoResourceNfn::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Cancel when no resource Notification is requested."));
	}

// FsmL
void RTestMVSAgentAPRCancelNoResourceNfn::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}            	
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);					
	            break;
	            }
            case EVidPlayerOpenComplete:
				{
				TInt retErr=KErrNone;
				if (!iFlag)
					{			
					// Play iMVSVideoPlayAgent1
					PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
					iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);
					}
				// Cancel iMVSVideoPlayAgent for Notification
				if(!iSecPlay)
					{
					iSecPlay=ETrue;
					retErr = iMVSVideoPlayAgent1->CancelNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->CancelNotification()"));
					if (retErr == KErrCancel)
						{
						INFO_PRINTF1(_L("iMVSVideoPlayAgent1->CancelNotification() returned KErrCancel - Successful"));
						StopTest(retErr, EPass);
						break;
						}
					else
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent1->CancelNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}				
						
	            	}
           		 }
        	}

		}
	}
	
/**
 *
 *		RTestMVSAgentAPRTestWillResumePlayCancelNfn
 *
 **/ 
// statis NewL()
RTestMVSAgentAPRTestWillResumePlayCancelNfn* RTestMVSAgentAPRTestWillResumePlayCancelNfn::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRTestWillResumePlayCancelNfn* self = new (ELeave) RTestMVSAgentAPRTestWillResumePlayCancelNfn(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRTestWillResumePlayCancelNfn::RTestMVSAgentAPRTestWillResumePlayCancelNfn(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRTestWillResumePlayCancelNfn::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Test WillResumePlay() after cancellation of notification"));
	}

// FsmL
void RTestMVSAgentAPRTestWillResumePlayCancelNfn::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent2 low priority
				PrepareState(EVideoOpening, EVidPlayerOpening2,KErrNone,ETestVideoPlay2);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent2, iFilename3));				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent2->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent2->OpenFileL "));
				break;
				}
            case EVidPlayerOpening2:
            	{
            	// set priority of iMVSVideoPlayAgent2 to EMdaPriorityMin
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent2 : %S, EMdaPriorityMin"), &iFilename3);
				TRAP(iError,iMVSVideoPlayAgent2->SetPriorityL(EMdaPriorityMin, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent2->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					} 
				PrepareState(EVideoOpened, EVidPlayerOpenComplete2,KErrNone,ETestVideoPlay2);									
	            break;
	            }
            case EVidPlayerOpenComplete2:
            	{
            	if(!iFlag )
	            	{
	            	// Play iMVSVideoPlayAgent2
	              	PrepareState(EVideoPlaying,EVidPlayerOpenComplete2, KErrNone, ETestVideoPlay2);
	              	INFO_PRINTF1(_L("iMVSVideoPlayAgent2->Play()"));
	              	iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent2);
	            	}
              	// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				if(iFlag)
					{
					PrepareState(EVideoOpening, EVidPlayerOpenComplete2, KErrNone,ETestVideoPlay);				
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL()"));
					iFlag=EFalse;
				    TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
					if( iError != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL(), returned %d"), iError);
						StopTest(iError, EFail);
						break;
						}				
					}
				// Set ExpectedEvent and change the State
				PrepareState(EVideoOpening, EVidPlayerOpening, KErrNone, ETestVideoPlay);
				break;
            	}
            case EVidPlayerOpening:
	            {
	            // set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}            	
				// Wait for Open to complete
				PrepareState(EVideoOpened, EVidPlayerOpenComplete, KErrNone, ETestVideoPlay);
				break;
	            }	            	
            case EVidPlayerOpenComplete:
	            {
	            // Play iMVSVideoPlayAgent1
              	PrepareState(EVideoPlaying, EVidPlayerOpenComplete, KErrNone, ETestVideoPlay);
              	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
                PlayL(*iMVSVideoPlayAgent1);				
				iFlag = ETrue;
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer1, KErrInUse, ETestVideoPlay2);
				break;
	            }
            case EErrInUseVideoPlayer1:
	            {
				if(!iRegistered  && !iSecPlay)
					{	
		            TInt retErr=KErrNone;
					// Register iMVSVideoPlayAgent for Notification
					retErr = iMVSVideoPlayAgent2->RegisterForNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent2->RegisterForNotification()"));
					if( retErr != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent2->RegisterForNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}
					iRegistered=ETrue;
					iSecPlay=ETrue;
					// Open iMVSAudioPlayAgent
					// Set ExpectedEvent and change the State
				//	PrepareState(EAudioOpening, EErrInUseVideoPlayer1, KErrNone,ETestAudioPlay);				
					INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
					TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
					if( iError != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
						StopTest(iError, EFail);
						break;
						}				
					// Set ExpectedEvent and change the State
					PrepareState(EAudioOpened,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);
					}
				iSecPlay=EFalse;
				break;	            	
	            }
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);
				break;
            	}
            case EErrInUseVideoPlayer:
				{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				retErr = iMVSVideoPlayAgent1->WillResumePlay();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->WillResumePlay()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->WillResumePlay() - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount=2;
				PrepareState(EAudioOpened, EAudPlayerPlayComplete,KErrNone,ETestAudioPlay,1);
				PrepareState(ENotReady, EVidPlayerResourceAvailable,KErrNone,ETestVideoPlay,2);
				break;
				}
			case EAudPlayerPlayComplete:
            case EVidPlayerResourceAvailable:
				{
				TInt retErr=KErrNone;
				// Cancel iMVSVideoPlayAgent for Notification
				if(!iSecPlay)
					{
					iSecPlay=ETrue;
					retErr = iMVSVideoPlayAgent2->CancelNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent2->CancelNotification()"));
					if (retErr != KErrNone)
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent2->CancelNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}	
					}			
				// Play iMVSVideoPlayAgent1
				PrepareState(EVideoPlaying, EVidPlayerResourceAvailable, KErrNone, ETestVideoPlay);
            	INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));	
                PlayL(*iMVSVideoPlayAgent1);				
				// Set ExpectedEvent and change the State
				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay);
//				PrepareState(EVideoOpened, EVidPlayerPlayComplete, KErrNone, ETestVideoPlay2,2);
				break;
				}
			case EVidPlayerPlayComplete:
				{
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1 Finished playing"));
				// initialise and start the timer (5 Sec), just to check if no other event occurs.
				TRAPD(err, iTimerWillResume = CMVSAPRWillResumeCancelNfnTimer::NewL(this, 5000000));
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("CMVSAPRWillResumeCancelNfnTimer::NewL left with error = %d"), err);
					StopTest(err, EFail);
					break;
					}	                    
				iTimerWillResume->Start();
				// Set ExpectedEvent and change the State
				PrepareState(ENotReady, EVidPlayerNoNotificationReceived,KErrNone,ETestVideoPlay);
				break;
				}				
			case EVidPlayerNoNotificationReceived:
				{
				INFO_PRINTF1(_L("No other client has been notified in 5 seconds after calling WillResumePlay()"));
				// we have not received notification, so pass the test	
				iSecPlay=EFalse;
				iFlag=EFalse;
				iRegistered=EFalse;
				StopTest(KErrNone, EPass);
				break;
				}
            }
        }   

	}


/**
 * set finished on callback from a timer
 */
void RTestMVSAgentAPRTestWillResumePlayCancelNfn::Finished()
    {
    // say that no events have occured after calling WillResumePlay()
    // and can proceed to the next step
    TRAP(iError, FsmL(EVidPlayerNoNotificationReceived, KErrNone));
    if(iError != KErrNone)
		{
		ERR_PRINTF2(_L("RTestMVSAgentAPRTestWillResumePlayCancelNfn::FsmL left with error = %d"), iError);
		StopTest(iError, EFail);
		}
    }

//
// CMVSAPRWillResumeCancelNfnTimer
//

/**
 * NewL
 */
CMVSAPRWillResumeCancelNfnTimer* CMVSAPRWillResumeCancelNfnTimer::NewL(RTestMVSAgentAPRTestWillResumePlayCancelNfn* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMVSAPRWillResumeCancelNfnTimer* self = new (ELeave) CMVSAPRWillResumeCancelNfnTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CMVSAPRWillResumeCancelNfnTimer::CMVSAPRWillResumeCancelNfnTimer(RTestMVSAgentAPRTestWillResumePlayCancelNfn* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CMVSAPRWillResumeCancelNfnTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CMVSAPRWillResumeCancelNfnTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }
    
 /**
 *
 *		RTestMVSAgentAPRMulRegAtPlay
 *
 */
// statis NewL()
RTestMVSAgentAPRMulRegAtPlay* RTestMVSAgentAPRMulRegAtPlay::NewL(const TDesC& aStepName, const TDesC& aSectionName)
	{
	RTestMVSAgentAPRMulRegAtPlay* self = new (ELeave) RTestMVSAgentAPRMulRegAtPlay(aStepName, aSectionName);
	return self;
	}

// private constructor
RTestMVSAgentAPRMulRegAtPlay::RTestMVSAgentAPRMulRegAtPlay(const TDesC& aStepName, const TDesC& aSectionName) :
	RTestStepMVSAgentsAPRBase(aStepName, aSectionName)
	{
	iFlag=EFalse;
	iSecPlay=EFalse;
	}
	
// start test
void RTestMVSAgentAPRMulRegAtPlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Register Multiple times for notification"));
	}

// FsmL
void RTestMVSAgentAPRMulRegAtPlay::FsmL(TMvsTestAgentEvents aEventCode, TInt aError)
	{
    if (ValidateEvent(aEventCode, aError) && iCanStateChange)
        {
        switch(aEventCode)
            {
            case EMvsIdle:
				{ 
				// Open iMVSVideoPlayAgent1
				PrepareState(EVideoOpening, EVidPlayerOpening,KErrNone,ETestVideoPlay);
				TRAP(iError, OpenFileL(*iMVSVideoPlayAgent1, iFilename1));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->OpenFileL - Failed with Error : %d"), iError);
					StopTest(iError, EFail);
					break;
					}
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->OpenFileL "));
				break;
				}
            case EVidPlayerOpening:
            	{
            	// set priority of iMVSVideoPlayAgent1 to EMdaPriorityNormal
				INFO_PRINTF2(_L("Set Priority of iMVSVideoPlayAgent1 : %S, EMdaPriorityNormal"), &iFilename1);
				TRAP(iError,iMVSVideoPlayAgent1->SetPriorityL(EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality));
				if (iError != KErrNone)
					{
					ERR_PRINTF2(_L("ERROR : iMVSVideoPlayAgent1->SetPriorityL() left with error : %d"), iError);
					StopTest(iError, EFail);
					}            	
				PrepareState(EVideoOpened, EVidPlayerOpenComplete,KErrNone,ETestVideoPlay);					
	            break;
	            }
            case EVidPlayerOpenComplete:
				{
				if (!iFlag)
					{
					// Play iMVSVideoPlayAgent1
					PrepareState(EVideoPlaying,EVidPlayerOpenComplete, KErrNone,ETestVideoPlay);
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->Play()"));
					iFlag=ETrue;
                    PlayL(*iMVSVideoPlayAgent1);					
					}
				if (!iRegistered )
					{
					TInt retErr=KErrNone;
					// Register iMVSVideoPlayAgent for Notification
					retErr = iMVSVideoPlayAgent1->RegisterForNotification();
					INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
					if( retErr != KErrNone )
						{
						ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
						StopTest(retErr, EFail);
						break;
						}
					iRegistered=ETrue;
					}
				// Open iMVSAudioPlayAgent
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpening, EVidPlayerOpenComplete, KErrNone,ETestAudioPlay);				
				INFO_PRINTF1(_L("iMVSAudioPlayAgent->OpenFileL()"));
				TRAP(iError, iMVSAudioPlayAgent->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSAudioPlayAgent->OpenFileL(), returned %d"), iError);
					StopTest(iError, EFail);
					break;
					}		
				iFlag=EFalse;
				// Set ExpectedEvent and change the State
				PrepareState(EAudioOpened, EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);								
				break;
				}
            case EAudPlayerOpenComplete:
            	{
            	// Set ExpectedEvent and change the State
            	PrepareState(EAudioPlaying,EAudPlayerOpenComplete, KErrNone,ETestAudioPlay);				
            	// Play iMVSAudioPlayAgent
            	INFO_PRINTF1(_L("iMVSAudioPlayAgent->Play()"));
				iMVSAudioPlayAgent->Play();
				
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if(retErr == KErrAlreadyExists)
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification 2nd time- Failed with Error : %d"), retErr);
					StopTest(retErr, EPass);
					break;						
					}
				else if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				PrepareState(ENotReady,EErrInUseVideoPlayer, KErrInUse,ETestVideoPlay);				
				break;
            	}            	
            case EErrInUseVideoPlayer:
            	{
				TInt retErr=KErrNone;
				// Register iMVSVideoPlayAgent for Notification
				retErr = iMVSVideoPlayAgent1->RegisterForNotification();
				INFO_PRINTF1(_L("iMVSVideoPlayAgent1->RegisterForNotification()"));
				if(retErr == KErrAlreadyExists)
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification 2nd time- Failed with Error : %d"), retErr);
					StopTest(retErr, EPass);
					break;						
					}
				else if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iMVSVideoPlayAgent1->RegisterForNotification - Failed with Error : %d"), retErr);
					StopTest(retErr, EFail);
					break;
					}
				break;
            	}
        	}
		}
	}

