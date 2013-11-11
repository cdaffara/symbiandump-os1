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
// testvideoplayer_secdisp.cpp
// 
//

#include "tsi_mvs_secdisp.h"


//
// RTestMVSVideoAgentSetScrIdAndPlay
//

/**
 * RTestMVSVideoAgentSetScrIdAndPlay::Constructor
 */
RTestMVSVideoAgentSetScrIdAndPlay::RTestMVSVideoAgentSetScrIdAndPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestMVSVideoAgentSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }   
    
/****
 * RTestVclntSetScrIdAndPlayAviFile::NewL
 */
RTestMVSVideoAgentSetScrIdAndPlay* RTestMVSVideoAgentSetScrIdAndPlay::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestMVSVideoAgentSetScrIdAndPlay* self = new (ELeave) RTestMVSVideoAgentSetScrIdAndPlay(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }
// start test
void RTestMVSVideoAgentSetScrIdAndPlay::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Set the valid screen id and Play the video file.And also set Invalid screen id and play video file"));
	}

/**
 * RTestMVSVideoAgentSetScrIdAndPlay::FsmL
 */
void RTestMVSVideoAgentSetScrIdAndPlay::FsmL(TMvsTestAgentEvents aEventCode)
    {
    if (ValidateEvent())
        {
        switch (aEventCode)
            {
            case EMvsIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iMvsVideoPlayAgent->SetScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iMVSVideoPlayAgent->SetScreenNumber(iScreenNumber);
                #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("SetScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
                #else
                if (err == KErrNotSupported)
                    {
#ifdef SYMBIAN_BUILD_GCE    
                    // CVideoPlayerUtility2 does not support the SetScreenNumber API, thus is expected to 
                    // fail with KErrNotSupported
                    if (!iMVSVideoPlayAgent->SupportVideoPlayerUtility2())
#endif                    
                        {
                        INFO_PRINTF2(_L("SetScreenNumber() is not supported in this OS, returned %d"), err);
                        StopTest(err, EKnownFailure);
                        }
                    }
                else
                    {
                    ERR_PRINTF2(_L("SetScreenNumber() failed with error : %d"), err);
                    StopTest(err, EFail);
                    }
                    User::Leave(err);
                #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT                                        
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFileName);
				OpenFileL(*iMVSVideoPlayAgent, iFileName);				                
                PrepareState(EVideoOpening, KErrNone);
                break;
                }
            case EVidPlayerOpening:
	            {
	            PrepareState(EVideoOpened,KErrNone);
	            break;	            	
	            }
            case EVidPlayerOpenComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->Play()"));
                PrepareState(EVideoPlaying, KErrNone);
                PlayL(*iMVSVideoPlayAgent);					                
                PrepareState(EVideoOpened, KErrNone);
                break;
                }
            case EVidPlayerPlayComplete:
                {
                PrepareState(ENotReady, KErrNone);
                iMVSVideoPlayAgent->Close();
                StopTest(EPass);
                break;
                }
            }
        }
    }
//
// RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile
//

/**
 * RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::Constructor
 */
RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestMVSVideoAgentSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    iCountNumber = 0;
    }   
    
/**
 * RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::NewL
 */
RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile* RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile* self = new (ELeave) RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }
    
/**
 * RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::DoKickoffTestL
 */
void RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::DoKickoffTestL()
	{
	// Print trace as to what this test does...
	INFO_PRINTF1(_L("Set the screen id during play and Play the video file."));
	}

/**
 * RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::FsmL
 */
void RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile::FsmL(TMvsTestAgentEvents aEventCode)
    {
    if (ValidateEvent())
        {
        switch (aEventCode)
            {
            case EMvsIdle:
                {
                 // Open iMvsVideoPlayAgent
                INFO_PRINTF2(_L("iMvsVideoPlayAgent->OpenFileL() %S"), &iFileName);
				OpenFileL(*iMVSVideoPlayAgent, iFileName);				                                
                PrepareState(EVideoOpening, KErrNone);
                break;
                }
            case EVidPlayerOpening:
	            {
	            PrepareState(EVideoOpened,KErrNone);
	            break;	            	
	            }
            case EVidPlayerOpenComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iMMSVideoPlayAgent->Play()"));
                PrepareState(EVideoPlaying, KErrNone);
                PlayL(*iMVSVideoPlayAgent);					                                
                if (iCountNumber == 0)
					{
					// Set iScreenNumber
					iCurrentScreenNumber = iScreenNumber;
					INFO_PRINTF2(_L("iVideoPlayer->SetScreenNumber(%d)"), iScreenNumber);
					TInt err = KErrNone;					
					err = iMVSVideoPlayAgent->SetScreenNumber(iScreenNumber);
					#ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
					if (err != KErrNone)
						{
#ifdef SYMBIAN_BUILD_GCE    
                        // CVideoPlayerUtility2 does not support the SetScreenNumber API, thus is expected to 
                        // fail with KErrNotSupported
                        if (!(iMVSVideoPlayAgent->SupportVideoPlayerUtility2() && err == KErrNotSupported))
#endif                    
                            {                                    						
    						ERR_PRINTF2(_L("SetScreenNumber failed with error : %d"), err);
    						StopTest(err, EFail);
    						User::Leave(err);
                            }
						}
					#else					
					if (err == KErrNotSupported)
						{
#ifdef SYMBIAN_BUILD_GCE    
                        // CVideoPlayerUtility2 does not support the SetScreenNumber API, thus is expected to 
                        // fail with KErrNotSupported
                        if (!iMVSVideoPlayAgent->SupportVideoPlayerUtility2())
#endif                    
                            {                                    
    						StopTest(err, EKnownFailure);
    						INFO_PRINTF2(_L("SetScreenNumber() is not supported in this OS, returned %d"), err);
                            }
						}
					else
						{
						StopTest(err, EFail);
						ERR_PRINTF2(_L("SetScreenNumber() failed with error : %d"), err);
						}
					StopTest();
					User::Leave(err);
					#endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
					}
				PrepareState(EVideoOpened, KErrNone);
                break;
                }
            case EVidPlayerPlayComplete:
				{
				// Open iMvsVideoPlayAgent
				if(iCountNumber==0)
					{
					iCountNumber++;
					INFO_PRINTF2(_L("iMvsVideoPlayAgent->OpenFileL() %S"), &iFileName);
                    TRAP(iError, OpenFileL(*iMVSVideoPlayAgent, iFileName));
					if (iError != KErrNone)
						{
						ERR_PRINTF2(_L("iMvsVideoPlayAgent->OpenFileL() left with error = %d"), iError);
						StopTest(iError, EFail);
						break;
						}
					else
						{
						PrepareState(EVideoOpening, KErrNone);
						}	
					}
				else
					{
					PrepareState(ENotReady, KErrNone);
					iMVSVideoPlayAgent->Close();
					StopTest(KErrNone, EPass);
					break;
					}
				break;
				}
		    }
		}
	}

    

