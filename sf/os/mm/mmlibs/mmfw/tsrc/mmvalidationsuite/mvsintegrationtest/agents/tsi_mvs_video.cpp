// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

#ifdef SYMBIAN_BUILD_GCE
#include <gceavailable.h>
#endif

#include "tsi_mvs_video.h"
#include <hal.h>

const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 20000000; //20MB

//
// RTestStepMVSVidAviPlayPos
//

/**
 *
 * RTestStepMVSVidAviPlayPos
 *
 */
RTestStepMVSVidAviPlayPos::RTestStepMVSVidAviPlayPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    iPlayTimer = NULL;
    iPauseCount = 0;
    iRestarted = EFalse;
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayPos::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file. Play and Position test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 * set finished on callback from a timer
 */
void RTestStepMVSVidAviPlayPos::Finished()
    {
    TInt err = KErrNone;
    iPauseCount++;
    
    PrepareState(ENotReady, KErrNotSupported);
    INFO_PRINTF1(_L("-- CMVSVideoPlayAgent->Pause();"));
    TRAP(err, iMVSVideoPlayAgent->PauseL());
    if (((err == KErrNotSupported) || (iError == KErrNotSupported)) || ((err == KErrNone) && (iError == KErrNone)))
        {
        iRestarted = EFalse;
	    TRAP(err, FsmL(EVidPlayerPlayPaused));
        if (err)
	        {
	        INFO_PRINTF2(_L("FsmL() left with error %d"), err);
	        }
        }
    else
        {
        INFO_PRINTF2(_L("Pause() returns with unexpected error :  %d"),err);
        CActiveScheduler::Stop();
        }
    }
/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayPos::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt err = KErrNone;
    TTimeIntervalMicroSeconds position(KTwoSeconds);
    
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {
            // Enquire video duration
             TTimeIntervalMicroSeconds playDur;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->DurationL()"));
            TRAP(err, playDur = iMVSVideoPlayAgent->DurationL());
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->DurationL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            
            // Set Position
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetPositionL()"));
            TRAP(err, iMVSVideoPlayAgent->SetPositionL(position));
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->SetPositionL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->SetPositionL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            
            // check for audio enabled
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->AudioEnabledL()"));
            TRAP(err, iMVSVideoPlayAgent->AudioEnabledL());
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->AudioEnabledL() Left with error : %d"), err);
                }
            
            // start the timer
            if(!iRestarted)
                {
                INFO_PRINTF1(_L("CMmfVideoPlayTimer::NewL"));
                TRAPD(err, iPlayTimer = CMmfVideoPlayTimer::NewL(this, 1000000));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfVideoPlayTimer::NewL left with error = %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                iPlayTimer->Start();
                iRestarted = ETrue;
                } 
            
            // Play till end
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                            
            PrepareState(EVideoPaused, KErrNone);
            break;
            }
            
        case EVidPlayerPlayPaused:
            {
            if(!iRestarted)
                {    
            	#ifdef AVSYNCH //This part of code will be enabled when A/V synch is implemented
            	// Get position
                INFO_PRINTF1(_L("CMVSVideoPlayAgent->PositionL()"));
                TRAP(err, position = iMVSVideoPlayAgent->PositionL());
                if (err != KErrNone)
                    {
                    INFO_PRINTF2(_L("iMVSVideoPlayAgent->PositionL() returned error - %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;    
                    }
                #endif //End AVSYNCH

                // Set position to some other position (before the current position)
                INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetPositionL(OneSecond)"));
                TRAP(err, iMVSVideoPlayAgent->SetPositionL(TTimeIntervalMicroSeconds(KOneSecond)));
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("iMVSVideoPlayAgent->SetPositionL() returned error - KErrNotSupported"));
                    }
                else if (err != KErrNone)
                    {
                    INFO_PRINTF2(_L("iMVSVideoPlayAgent->SetPositionL() returned error - %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;    
                    }
                
                // Stop playing and restart
                INFO_PRINTF1(_L("-- Restarting Play..."));
                iRestarted = ETrue;
                iPlayTimer->Cancel();
	            PrepareState(EVideoStopped, KErrNone);
	            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Stop()"));
	            iMVSVideoPlayAgent->Stop();
				
                // Play till end
                PrepareState(EVideoPlaying, KErrNone);
                INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
                PlayL(*iMVSVideoPlayAgent);					                                
                PrepareState(EVideoOpened, KErrNone);
                }
            break;
            }
                
        case EVidPlayerPlayComplete:
            {
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;    
            }
            
        }
    }

//
// RTestStepMVSVidAviPlayRot
//

/**
 *
 * RTestStepMVSVidAviPlayRot
 *
 */
RTestStepMVSVidAviPlayRot::RTestStepMVSVidAviPlayRot(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayRot::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file,  Play and Rotation test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayRot::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {
            // set the value for rotation to EVideoRotationClockwise90
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetRotationL(EVideoRotationClockwise90)"));
            TRAP(err, iMVSVideoPlayAgent->SetRotationL(EVideoRotationClockwise90));
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->SetRotationL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->SetRotationL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                

            // set the value for rotation to EVideoRotationClockwise180
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetRotationL(EVideoRotationClockwise180)"));
            TRAP(err, iMVSVideoPlayAgent->SetRotationL(EVideoRotationClockwise180));
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->SetRotationL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->SetRotationL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            
            // Get the rotation value and check
            TVideoRotation vidRot(EVideoRotationNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->RotationL()"));
            TRAP(err, vidRot = iMVSVideoPlayAgent->RotationL());
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->RotationL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->RotationL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            else if (vidRot != EVideoRotationClockwise180)
                {
                ERR_PRINTF1(_L("Video Rotation factor does not match"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            
            // Stop playing and restart
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Stop()"));
            iMVSVideoPlayAgent->Stop();
            
            // Play till end
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);                        
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
            }
            
        }
    }

//
// RTestStepMVSVidAviPlayScl
//

/**
 *
 * RTestStepMVSVidAviPlayScl
 *
 */
RTestStepMVSVidAviPlayScl::RTestStepMVSVidAviPlayScl(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayScl::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file, Play and Scale test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayScl::FsmL(TMvsTestAgentEvents aEventCode)
    {
    // Local variables
    TReal32 widthScaleFactor = 0.0f;
    TReal32 heightScaleFactor = 0.0f;
    TBool antiAliasFilter = EFalse;
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
        case EVidPlayerOpening:
            PrepareState(EVideoOpened, KErrNone);
            break;
        case EVidPlayerOpenComplete:
            // Set Scaling factor
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetScaleFactorL(1.5f, 1.65f, ETrue)"));
            TRAP(err, iMVSVideoPlayAgent->SetScaleFactorL(KScaleWidth1, KScaleHeight1, ETrue));
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->SetScaleFactorL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->SetScaleFactorL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                
            
            // Get the scaling value and check
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->GetScaleFactorL()"));
            TRAP(err, iMVSVideoPlayAgent->GetScaleFactorL(widthScaleFactor, heightScaleFactor, antiAliasFilter));
            if (err == KErrNotSupported)
                {
                INFO_PRINTF1(_L("iMVSVideoPlayAgent->GetScaleFactorL() returned error - KErrNotSupported"));
                }
            else if (err != KErrNone)
                {
                INFO_PRINTF2(_L("iMVSVideoPlayAgent->GetScaleFactorL() returned error - %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;    
                }
            else if (widthScaleFactor != KScaleWidth1 || heightScaleFactor != KScaleHeight1 || !antiAliasFilter)
                {
                ERR_PRINTF1(_L("CMVSVideoPlayAgent->GetScaleFactorL() - values mismatch!"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Stop()"));
            iMVSVideoPlayAgent->Stop();
            
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Close()"));
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
        }
    }

//
// RTestStepMVSVidPlaySettings
//

/**
 *
 * RTestStepMVSVidPlaySettings
 *
 */
RTestStepMVSVidPlaySettings::RTestStepMVSVidPlaySettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {}

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidPlaySettings::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Video Playback settings Test >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidPlaySettings::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt err = KErrNone;
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {
            // check if audio is enabled
            TBool isAudioEnabled = EFalse;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->AudioEnabledL()"));
            TRAP(err, isAudioEnabled = iMVSVideoPlayAgent->AudioEnabledL());
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->AudioEnabledL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            if(isAudioEnabled)
                {
                INFO_PRINTF1(_L("-- AudioEnabledL() returned TRUE"));
                }
            else
                {
                INFO_PRINTF1(_L("-- AudioEnabledL() returned FALSE"));
                }
            // Video Playback priority
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetPriorityL(-1, EMdaPriorityPreferenceNone)"));
            TRAP(err, iMVSVideoPlayAgent->SetPriorityL(-1, EMdaPriorityPreferenceNone));
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->SetPriorityL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            TInt getPriority;
            TMdaPriorityPreference getPriorityPref;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->PriorityL()"));
            TRAP(err, iMVSVideoPlayAgent->PriorityL(getPriority, getPriorityPref));
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->PriorityL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Set the volume to the maximum
            TInt setVol = 30;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetVolumeL(30)"));
            TRAP(err, iMVSVideoPlayAgent->SetVolumeL(setVol));
            if (err!=KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->SetVolumeL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            TInt getVol = 0;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Volume()"));
            getVol = iMVSVideoPlayAgent->Volume();
            if (getVol!= setVol)
                {
                ERR_PRINTF3(_L("CMVSVideoPlayAgent->Volume() Value (%d) does not match with Actual (%d)"), getVol, setVol);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            INFO_PRINTF2(_L("Get Volume (%d)"), getVol);                
            // Set the display window
            TRect rect, clipRect;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetDisplayWindowL()"));
            TRAP(err, iMVSVideoPlayAgent->SetDisplayWindowL(iWs, *iScreen, *iWindow, rect, clipRect));            
#ifdef SYMBIAN_BUILD_GCE    
            if (iMVSVideoPlayAgent->SupportVideoPlayerUtility2())
                {                    
				// CVideoPlayerUtility2 is expected to return KErrNotSupported when the API SetDisplayWindowL is called.
                if (err != KErrNotSupported)
                    {
                    ERR_PRINTF1(_L("CMVSVideoPlayAgent->SetDisplayWindowL() did not leave KErrNotSupported when using CVideoPlayerUtility2"));
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;                                                            
                    }
                }
            else
#endif            
                {                                            
                if (err!=KErrNone)
                    {
                    ERR_PRINTF2(_L("CMVSVideoPlayAgent->SetDisplayWindowL() Left with error : %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;                
                    }
                }
                
            // Get the controller in use
            const CMMFControllerImplementationInformation* info = NULL;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->ControllerImplementationInformationL()"));
            TRAP(err, info = &iMVSVideoPlayAgent->ControllerImplementationInformationL());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSVideoPlayAgent->ControllerImplementationInformationL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // See if it is the AVI Video Controller
            if (info->DisplayName().Compare(KTestScbVideoControllerName)==0)
                {
                TBuf<256> ext;
                TBuf<256> mime;
                TInt i,j;
                // Check for supported Play Format(s)
                const RMMFFormatImplInfoArray& playFormats = info->PlayFormats();
                INFO_PRINTF1(_L("Play Format Support"));
                if (playFormats.Count() > 0 )
                    {
                    for (i = 0;i<playFormats.Count();i++)
                        {
                        INFO_PRINTF2(_L("supported format %d: "),i);
                        const CDesC8Array& extensions = playFormats[i]->SupportedFileExtensions();
                        for (j = 0;j < extensions.Count(); j++)
                            {
                            ext.Copy(extensions[j]);
                            INFO_PRINTF2(_L("extension : %S"), &ext);
                            }
                        const CDesC8Array& mimeTypes = playFormats[i]->SupportedMimeTypes();
                        for (j = 0;j < mimeTypes.Count(); j++)
                            {
                            mime.Copy(mimeTypes[j]);
                            INFO_PRINTF2(_L("mime type: %S"), &mime);
                            }
                        }
                    }
                else
                    {
                    ERR_PRINTF1(_L("Play Format Support is NOT provided"));
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                // Check for supported Record Format(s)
                const RMMFFormatImplInfoArray& recordFormats = info->RecordFormats();
                INFO_PRINTF1(_L("Record Format Support"));
                if ( recordFormats.Count() > 0 )
                    {
                    ERR_PRINTF1(_L("Record Format Support is provided"));
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Get number of meta data entries information
            TInt numMetaEntries = 0;
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->NumberOfMetaDataEntriesL()"));
            TRAP(err, numMetaEntries = iMVSVideoPlayAgent->NumberOfMetaDataEntriesL());
            WARN_PRINTF2(_L("CMVSVideoPlayAgent->NumberOfMetaDataEntriesL() Left with error : %d"), err);
            if ((err!=KErrNone) && (err!=KErrNotSupported))
                {
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            INFO_PRINTF2(_L("No. of metadata entries : %d"), numMetaEntries);    
            
            // Get the meta data entries
            RPointerArray<CMMFMetaDataEntry> metaArray;
            CleanupClosePushL(metaArray);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->GetMetaDataArrayL()"));
            TRAP(err, iMVSVideoPlayAgent->GetMetaDataArrayL(metaArray));
            WARN_PRINTF2(_L("CMVSVideoPlayAgent->GetMetaDataArrayL() Left with error : %d"), err);
            if ((err!=KErrNone) && (err!=KErrNotSupported))
                {
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            CleanupStack::PopAndDestroy();
            
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                            
            
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Stop()"));
            iMVSVideoPlayAgent->Stop();
            
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Close()"));
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
            }
        }
    }

//
// RTestStepMVSVidAviRecPlay
//

/**
 *
 * RTestStepMVSVidAviRecPlay
 *
 */
RTestStepMVSVidAviRecPlay::RTestStepMVSVidAviRecPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    #ifdef __X86GCC__
    // twice Heap used for x86 platform
    iHeapSize = 2000000; // 2MB
    #endif // __X86GCC__
    
    // Find out whether the platform is Naviengine. If so Increase Heap size.
	TInt value = 0;
	TInt err = HAL::Get(HAL::EMachineUid, value);
	if (err != KErrNone)
    	{
	    INFO_PRINTF2(_L("---Problem calling HAL::Get(HAL::EMachineUid); failed with %d"), err);
	    }
	if(value==HAL::EMachineUid_NE1_TB)
		{
		iHeapSize = 2000000; // 2MB  
		}    
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSVidAviRecPlay::DoTestStepPreambleL()
    {
    INFO_PRINTF1(_L("<<< Create a new AVI (X-Vid) file. Record. Play the recorded file. >>>"));
    
    TVerdict PreamRes;
    PreamRes = EPass;
    
    if(SetCacheSize()!=KErrNone)
    	{
    	return EInconclusive;
    	}
    
    // Call base DoTestStepPreambleL()
    PreamRes = RTestStepMVSVideoAgent::DoTestStepPreambleL();
    if (PreamRes != EPass)
        {
        return PreamRes;
        }

    // Get the Config Settings for Recording
    TInt tmpFrameRate;
    TPtrC tmpVideoType;
    if((!GetIntFromConfig(iSectName, KIniFrameRate, tmpFrameRate)) || 
        (!GetIntFromConfig(iSectName, KIniFrameWidth, iFrameSize.iWidth)) ||
        (!GetIntFromConfig(iSectName, KIniFrameHeight, iFrameSize.iHeight)) ||
        (!GetIntFromConfig(iSectName, KIniVideoBitRate, iVideoBitRate)) ||
        (!GetIntFromConfig(iSectName, KIniAudioBitRate, iAudioBitRate)) ||
        (!GetStringFromConfig(iSectName, KIniVideoType, tmpVideoType)) ||
        (!GetBoolFromConfig(iSectName, KIniAudioEnabled, iAudioEnabled)))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
    iFrameRate = tmpFrameRate / 10;
    iVideoType.Append(tmpVideoType);
    iAudioType = KMMFFourCCCodePCM16;
    
    return PreamRes;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSVidAviRecPlay::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("RTestStepMVSVidAviRecPlay::DoTestStepPostambleL()"));
    
    // Base Postamble.
    return RTestStepMVSVideoAgent::DoTestStepPostambleL();
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviRecPlay::PerformTestL()
    {
    INFO_PRINTF1(_L("Testing MVS Video Client Agent"));
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test
    iActiveAgent = ETestVideoRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)iCurrentCacheSize.iMinSize,(TAny*)iCurrentCacheSize.iMaxSize);
    if(CheckCacheError(error)!=KErrNone)
    	{
    	return EInconclusive;
    	}
    INFO_PRINTF3(_L("Setting Cache Min Size to %d,Setting Cache Max Size is %d"),iCurrentCacheSize.iMinSize,iCurrentCacheSize.iMaxSize);
 
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviRecPlay::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt err = KErrNone;
    TInt cumErr = KErrNone;
    // Local variables for comparison with actual settings.
    TReal32 newFrameRate = 0;
    TSize newFrameSize;
    TInt newVideoBitRate = 0;
    TInt newAudioBitRate = 0;
    TBuf8<256> newVideoType;
    TFourCC newAudioType;
    TBool newAudioEnabled = EFalse;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviRecordControllerUid);
            break;
            }
            
        case EVidRecorderOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidRecorderOpenComplete:
            {
            // Set Video Frame rate
            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameRateL(iFrameRate));
            INFO_PRINTF3(_L("-- CMVSVideoRecordAgent->SetVideoFrameRateL(%d) returned %d"), TInt(iFrameRate), err);
            cumErr = cumErr + err;
            // Set Video Frame size
            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameSizeL(iFrameSize));
            INFO_PRINTF4(_L("-- CMVSVideoRecordAgent->SetVideoFrameSizeL(%d, %d) returned %d"), iFrameSize.iWidth, iFrameSize.iHeight, err);
            cumErr = cumErr + err;
            // Set Audio Enabled
            TRAP(err, iMVSVideoRecordAgent->SetAudioEnabledL(ETrue));
            INFO_PRINTF3(_L("-- CMVSVideoRecordAgent->SetAudioEnabledL(%d) returned %d"), iAudioEnabled, err);
            cumErr = cumErr + err;
            // Set Video Bit rate
            TRAP(err, iMVSVideoRecordAgent->SetVideoBitRateL(iVideoBitRate));
            INFO_PRINTF3(_L("-- CMVSVideoRecordAgent->SetVideoBitRateL(%d) returned %d"), iVideoBitRate, err);
            if (err != KErrNotSupported)
                {
                cumErr = cumErr + err;
                }
            // Set Audio Bit rate
            TRAP(err, iMVSVideoRecordAgent->SetAudioBitRateL(iAudioBitRate));
            INFO_PRINTF3(_L("-- CMVSVideoRecordAgent->SetAudioBitRateL(%d) returned %d"), iAudioBitRate, err);
            if (err != KErrNotSupported)
                {
                cumErr = cumErr + err;
                }
            // Set Video Type
            TRAP(err, iMVSVideoRecordAgent->SetVideoTypeL(iVideoType));
            TBuf16<256> videoType16;
            videoType16.Copy(iVideoType);
            INFO_PRINTF3(_L("-- CMVSVideoRecordAgent->SetVideoTypeL(%S) returned %d"), &videoType16, err);
            cumErr = cumErr + err;
            // Set Audio Type
            TRAP(err, iMVSVideoRecordAgent->SetAudioTypeL(iAudioType));
            TUint32 fourCC = iAudioType.FourCC();
            INFO_PRINTF6(_L("-- CMVSVideoRecordAgent->SetAudioTypeL(%c%c%c%c) returned %d"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24, err);
            cumErr = cumErr + err;
            // check for any errors in the process
            if (cumErr != KErrNone)
                {
                ERR_PRINTF1(_L("Error in Setting config values"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
                
            // Record iMVSVideoRecordAgent
            PrepareState(EVideoRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->RecordL()"));
            iMVSVideoRecordAgent->Record();
            
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->Stop()"));
            iMVSVideoRecordAgent->Stop();

            // Check the duration of the recorded clip
            TTimeIntervalMicroSeconds recDuration;
            TRAP(err, recDuration = iMVSVideoRecordAgent->DurationL());
            INFO_PRINTF3(_L("Returned Duration = %d, Actual Record duration = %d"), I64INT(recDuration.Int64()), KFiveSeconds);
            if (!TimeComparison(I64INT(recDuration.Int64()), KFiveSeconds, KOneSecond))
                {
                ERR_PRINTF1(_L("Record Duration does not match"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                }
            else
                {
                // Close iMVSVideoRecordAgent
                PrepareState(ENotReady, KErrNone);
                iMVSVideoRecordAgent->Close();
                iActiveAgent = ETestVideoPlay;
                // Open iMVSVideoPlayAgent
                PrepareState(EVideoOpening, KErrNone);
                INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
                OpenFileL(*iMVSVideoPlayAgent, iFilename);
                }
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {
            #ifdef AVSYNCH //This part of code will be enabled when A/V synch is implemented
            // Play till end
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                            
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
			#endif //End AVSYNC
            // Get Video Frame rate
            TRAP(err, newFrameRate = iMVSVideoPlayAgent->VideoFrameRateL());
            INFO_PRINTF3(_L("-- CMVSVideoPlayAgent->VideoFrameRateL() returned %d, ERROR : %d"), TInt(newFrameRate), err);
            cumErr = cumErr + err;
            // Get Video Frame size
            TRAP(err, iMVSVideoPlayAgent->VideoFrameSizeL(newFrameSize));
            INFO_PRINTF4(_L("-- CMVSVideoPlayAgent->VideoFrameSizeL() returned (%d, %d), ERROR : %d"), newFrameSize.iWidth, newFrameSize.iHeight, err);
            cumErr = cumErr + err;
            // Get Video Bit rate
            TRAP(err, newVideoBitRate = iMVSVideoPlayAgent->VideoBitRateL());
            INFO_PRINTF3(_L("-- CMVSVideoPlayAgent->VideoBitRateL() returned (%d), ERROR : %d"), newVideoBitRate, err);
            if (err != KErrNotSupported)
                {
                cumErr = cumErr + err;
                }
            // Get Audio Enabled
            TRAP(err, newAudioEnabled = iMVSVideoPlayAgent->AudioEnabledL());
            INFO_PRINTF3(_L("-- CMVSVideoPlayAgent->AudioEnabledL() returned (%d), ERROR : %d"), newAudioEnabled, err);
            cumErr = cumErr + err;
            // Get Audio Bit rate
            TRAP(err, newAudioBitRate = iMVSVideoPlayAgent->AudioBitRateL());
            INFO_PRINTF3(_L("-- CMVSVideoPlayAgent->AudioBitRateL() returned (%d), ERROR : %d"), newAudioBitRate, err);
            if (err != KErrNotSupported)
                {
                cumErr = cumErr + err;
                }
            // Get Video Type
            newVideoType = iMVSVideoPlayAgent->VideoFormatMimeType();
            TBuf16<256> newVideoType16;
            newVideoType16.Copy(newVideoType);
            INFO_PRINTF2(_L("-- CMVSVideoPlayAgent->VideoFormatMimeType() returned (%S)"), &newVideoType16);
            // Get Audio Type
            TRAP(err, newAudioType = iMVSVideoPlayAgent->AudioTypeL());
            TUint32 fourCC1 = newAudioType.FourCC();
            INFO_PRINTF6(_L("-- CMVSVideoPlayAgent->AudioTypeL(%c%c%c%c) returned (%d)"), fourCC1, fourCC1>>8, fourCC1>>16, fourCC1>>24, err);
            cumErr = cumErr + err;
            
            // Get the errors and set pass / fail
            if (cumErr != KErrNone)
                {
                ERR_PRINTF1(_L("Error in Getting config values"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            if ((newFrameRate != iFrameRate) ||
                (newFrameSize != iFrameSize) ||
                (iVideoBitRate != iVideoBitRate) ||
                (iAudioBitRate != iAudioBitRate) ||
                (newVideoType != iVideoType) ||
                (newAudioType != iAudioType) ||
                (newAudioEnabled != iAudioEnabled) )
                {
                ERR_PRINTF1(_L("Mismatch in some config values"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
            }
        }
    }

TInt RTestStepMVSVidAviRecPlay::SetCacheSize()
	{
	TInt ret=KErrNone;
	//save the default cache sizes, set the cache size back these values after the test
	int error=UserSvr::HalFunction(EHalGroupVM,EVMHalGetCacheSize,&iCurrentCacheSize,0);
	ret=CheckCacheError(error);
	if(ret==KErrNone)
		{
		error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
		ret=CheckCacheError(error);
		}
	return ret;
	}

TInt RTestStepMVSVidAviRecPlay::CheckCacheError(TInt aError)
	{
	TInt ret=KErrNone;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError!=KErrNone)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			ret=aError;
			}
		}
	#else
	if(aError!=KErrNone)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		ret=aError;
		}
	#endif
	return ret;
	}

//
// RTestStepMVSVidRecPause
//

/**
 *
 * RTestStepMVSVidRecPause
 *
 */
RTestStepMVSVidRecPause::RTestStepMVSVidRecPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    iFlag=0;
    RecordResume=0;
  
    #ifdef __X86GCC__
    // twice Heap used for x86 platform
    iHeapSize = 2000000; // 2MB
    #endif // __X86GCC__
    
    // Find out whether the platform is Naviengine. If so Increase Heap size.
	TInt value = 0;
	TInt err = HAL::Get(HAL::EMachineUid, value);
	if (err != KErrNone)
    	{
	    INFO_PRINTF2(_L("---Problem calling HAL::Get(HAL::EMachineUid); failed with %d"), err);
	    }
	if(value==HAL::EMachineUid_NE1_TB)
		{
		iHeapSize = 2000000; // 2MB  
		}

    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidRecPause::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Video Record Settings Test (enable Audio) >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoRecordAgent as active agent & begin the test
    iActiveAgent = ETestVideoRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidRecPause::FsmL(TMvsTestAgentEvents aEventCode)
    {
    // Local variables
    TInt cumErr = KErrNone;
    TInt err = KErrNone;
    TInt duration;

    
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviRecordControllerUid);
            break;
            }
            
        case EVidRecorderOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidRecorderOpenComplete:
            {
            if(iFlag == 0)
	            {
	            //set framerate
	            const TReal32 KDefaultFrameRate = 10.00;
	            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetVideoFrameRateL(10.00)"));
	            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameRateL(KDefaultFrameRate));
	            if(err)
	                {
	                INFO_PRINTF2(_L("SetFrameRate() after Prepare failed with  %d "), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                }
	            // set framesize
	            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetVideoFrameSizeL(176,144)"));
	            const TSize KDefaultFrameSize(176,144);
	            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameSizeL(KDefaultFrameSize));
	            if(err)
	                {
	                INFO_PRINTF2(_L("SetFrameSize() after Prepare failed with  %d "), err);
	                iTestStepResult = EFail;
	                CActiveScheduler::Stop();
	                }
	            // Enable audio while recording
	            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetAudioEnabledL(ETrue)"));
	            TRAP(err, iMVSVideoRecordAgent->SetAudioEnabledL(ETrue));
	            cumErr = cumErr + err;
	            if (cumErr != KErrNone)
	                {
	                ERR_PRINTF1(_L("Error in Setting config values"));
	                CActiveScheduler::Stop();
	                iTestStepResult = EFail;
	                break;
	                }
	            iFlag=1;   					
				// Record iMVSVideoRecordAgent
				INFO_PRINTF1(_L("iMVSVideoRecordAgent->Record()"));    
				PrepareState(EVideoRecording, KErrNone);
				iMVSVideoRecordAgent->Record();    
				}
           	TRAP(err, iRecordTimer = CMvsVideoRecordTimer::NewL(this, 2000000));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMvsVideoRecordTimer::NewL left with error = %d"), err);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
			iFlag=0;
			iRecordTimer->Start();      				
			iCurrentState=iCallbackState;
            break;
            }
		case EVidRecorderRecording:
			{
			if(iFlag == 0)
				{
				// Cancel the timer
				iRecordTimer->Cancel();
				delete iRecordTimer;
				iRecordTimer = NULL;
				iFlag=1;
				INFO_PRINTF1(_L("-- iMVSVideoRecordAgent->PauseL();"));
				PrepareState(EVideoPaused, KErrNone);
				TRAP(err,iMVSVideoRecordAgent->PauseL());
				if (err != KErrNone)
					{
					INFO_PRINTF2(_L("Pause() returns with error :  %d"),err);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					}
				}
			TRAP(err, iRecordTimer = CMvsVideoRecordTimer::NewL(this, 2000000));
			if (err != KErrNone)                       
				{
				ERR_PRINTF2(_L("CMvsVideoRecordTimer::NewL left with error = %d"), err);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
			iCurrentState=iCallbackState; // EVideoPaused
			iRecordTimer->Start();    
            //PrepareState(EVideoPaused, KErrNone);
			break; 
        	}
		case EVidRecorderPaused:
			{
			// Cancel the timer
			iRecordTimer->Cancel();
			delete iRecordTimer;
			iRecordTimer = NULL;
			
			// Resume Record iVideoRecorder
			INFO_PRINTF1(_L("iVideoRecorder->Record()"));
			PrepareState(EVideoRecording, KErrNone);
			iMVSVideoRecordAgent->Record();

			RecordResume=1;
			TRAP(err, iRecordTimer = CMvsVideoRecordTimer::NewL(this, 2000000));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("CMvsVideoRecordTimer::NewL left with error = %d"), err);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
			iRecordTimer->Start();      
			iCurrentState=iCallbackState;   //EVideoRecording
			PrepareState(EVideoStopped, KErrNone);
			break; 
			}
		case EVidRecorderRecordComplete:
            {
            // Close iMVSVideoRecordAgent
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("iMVSVideoRecordAgent->Stop()"));
    		iMVSVideoRecordAgent->Stop();
			// Get the duration of the file.
			duration = I64INT(iMVSVideoRecordAgent->DurationL().Int64());
			  
			// Check the results
			if(TimeComparison(duration,4000000,KExpectedDeviation))
				{
				// Close iMVSVideoRecordAgent
				PrepareState(ENotReady, KErrNone);
				iMVSVideoRecordAgent->Close();
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			else
				{
				ERR_PRINTF2(_L("Recorder duration = %d"), duration);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;		                	
				} 
            }
        }
       
    }
    
/**
 * set finished on callback from a timer
 */
void RTestStepMVSVidRecPause::Finished()
    {
    if(iCurrentState == EVideoRecording)
		{
		if(!RecordResume )
			{	
			TRAP(iError,FsmL(EVidRecorderRecording));	
			if(iError != KErrNone)		
				{
				ERR_PRINTF2(_L("RTestStepMVSVidRecPause::FsmL() left with error = %d"), iError);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}
			}
		else if(RecordResume)		
			{
			TRAP(iError,FsmL(EVidRecorderRecordComplete));	
			if(iError != KErrNone)		
				{
				ERR_PRINTF2(_L("RTestStepMVSVidRecPause::FsmL() left with error = %d"), iError);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}			
			}
		}
	else if(iCurrentState == EVideoPaused)
		{
		TRAP(iError,FsmL(EVidRecorderPaused));			
			if(iError != KErrNone)		
				{
				ERR_PRINTF2(_L("RTestStepMVSVidRecPause::FsmL() left with error = %d"), iError);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}		
		}
    }    
    
//
// RTestStepMVSVidRecSettings
//

/**
 *
 * RTestStepMVSVidRecSettings
 *
 */
RTestStepMVSVidRecSettings::RTestStepMVSVidRecSettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    #ifdef __X86GCC__
    // twice Heap used for x86 platform
    iHeapSize = 2000000; // 2MB
    #endif // __X86GCC__
    
    // Find out whether the platform is Naviengine. If so Increase Heap size.
	TInt value = 0;
	TInt err = HAL::Get(HAL::EMachineUid, value);
	if (err != KErrNone)
    	{
	    INFO_PRINTF2(_L("---Problem calling HAL::Get(HAL::EMachineUid); failed with %d"), err);
	    }
	if(value==HAL::EMachineUid_NE1_TB)
		{
		iHeapSize = 2000000; // 2MB  
		}    
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidRecSettings::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Video Record Settings Test (enable Audio) >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test
    iActiveAgent = ETestVideoRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidRecSettings::FsmL(TMvsTestAgentEvents aEventCode)
    {
    // Local variables
    TInt cumErr = KErrNone;
    TInt newGain = 0;
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviRecordControllerUid);
            break;
            }
            
        case EVidRecorderOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidRecorderOpenComplete:
            {
            // Enable audio while recording
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetAudioEnabledL(ETrue)"));
            TRAP(err, iMVSVideoRecordAgent->SetAudioEnabledL(ETrue));
            cumErr = cumErr + err;
            // Set gain to maximum and enquire
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetGainL(MaxGain)"));
            TRAP(err, iMVSVideoRecordAgent->SetGainL(iMVSVideoRecordAgent->MaxGainL()));
            cumErr = cumErr + err;
            // Get gain
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->GainL()"));
            TRAP(err, newGain = iMVSVideoRecordAgent->GainL());
            INFO_PRINTF2(_L("New gain is : %d"), newGain);
            // Get Supported Video types
            CDesC8ArrayFlat* theVideoTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
            CleanupStack::PushL(theVideoTypes);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->GetSupportedVideoTypesL()"));
            TRAP(err, iMVSVideoRecordAgent->GetSupportedVideoTypesL(*theVideoTypes));
            if(err != KErrNone && err != KErrNotSupported)
                {
                cumErr++;
                }
            else
                {
                for (TInt i = 0; i < theVideoTypes->Count(); i++)
                    {
                    TBuf<64> temp;
                    temp.Copy((*theVideoTypes)[i]);
                    INFO_PRINTF2(_L("Mime Type :  %S"), &temp);
                    }
                }
            CleanupStack::PopAndDestroy(theVideoTypes);
            // Get Supported Audio types
            RArray<TFourCC> theAudioTypes;
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->GetSupportedAudioTypesL()"));
            TRAPD(err, iMVSVideoRecordAgent->GetSupportedAudioTypesL(theAudioTypes));
            if(err != KErrNone && err != KErrNotSupported)
                {
                cumErr++;
                }
            // Get the controller in use
            const CMMFControllerImplementationInformation* info = NULL;
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->ControllerImplementationInformationL()"));
            TRAP(err, info = &iMVSVideoRecordAgent->ControllerImplementationInformationL());
            if (err != KErrNone)
                {
                cumErr++;
                }
            // See if it is the AVI Video Record Controller
            if (info->DisplayName().Compare(KTestAviRecordControllerName)==0)
                {
                TBuf<256> ext;
                TBuf<256> mime;
                TInt i,j;
                // Check for supported Play Format(s)
                const RMMFFormatImplInfoArray& playFormats = info->PlayFormats();
                INFO_PRINTF1(_L("Play Format Support"));
                if (playFormats.Count() != 0 )
                    {
                    ERR_PRINTF1(_L("Play Format Support is provided"));
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                // Check for supported Record Format(s)
                const RMMFFormatImplInfoArray& recordFormats = info->RecordFormats();
                INFO_PRINTF1(_L("Record Format Support"));
                if (recordFormats.Count() >0 )
                    {
                    for (i = 0;i<recordFormats.Count();i++)
                        {
                        INFO_PRINTF2(_L("supported format %d: "),i);
                        const CDesC8Array& extensions = recordFormats[i]->SupportedFileExtensions();
                        for (j = 0;j < extensions.Count(); j++)
                            {
                            ext.Copy(extensions[j]);
                            INFO_PRINTF2(_L("extension : %S"), &ext);
                            }
                        const CDesC8Array& mimeTypes = recordFormats[i]->SupportedMimeTypes();
                        for (j = 0;j < mimeTypes.Count(); j++)
                            {
                            mime.Copy(mimeTypes[j]);
                            INFO_PRINTF2(_L("mime type : %S"), &mime);
                            }
                        }
                    }
                else
                    {
                    ERR_PRINTF1(_L("Record Format Support is NOT provided"));
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            if (cumErr != KErrNone)
                {
                ERR_PRINTF1(_L("Error in Setting config values"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Close iMVSVideoRecordAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoRecordAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
            }
        }
    }

//
// RTestStepMVSVidRecMeta
//

/**
 *
 * RTestStepMVSVidRecMeta
 *
 */
RTestStepMVSVidRecMeta::RTestStepMVSVidRecMeta(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    #ifdef __X86GCC__
    // twice Heap used for x86 platform
    iHeapSize = 2000000; // 2MB
    #endif // __X86GCC__    
    
    // Find out whether the platform is Naviengine. If so Increase Heap size.
	TInt value = 0;
	TInt err = HAL::Get(HAL::EMachineUid, value);
	if (err != KErrNone)
    	{
	    INFO_PRINTF2(_L("---Problem calling HAL::Get(HAL::EMachineUid); failed with %d"), err);
	    }
	if(value==HAL::EMachineUid_NE1_TB)
		{
		iHeapSize = 2000000; // 2MB  
		}
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidRecMeta::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Set meta-information >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoRecordAgent as active agent & begin the test
    iActiveAgent = ETestVideoRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidRecMeta::FsmL(TMvsTestAgentEvents aEventCode)
    {
    _LIT(name1,"<Copyright>");
    _LIT(data1,"<Symbian (c) 2005>");
    _LIT(name2,"Artist");
    _LIT(data2,"Tester");
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviRecordControllerUid);
            break;
        case EVidRecorderOpening:
            PrepareState(EVideoOpened, KErrNone);
            break;
        case EVidRecorderOpenComplete:
            // Stop the scheduler
            CActiveScheduler::Stop();
            // build a meta data array object.
            RPointerArray<CMMFMetaDataEntry> metaArray;
            CleanupClosePushL(metaArray);
            // Populate the meta array object.
            CMMFMetaDataEntry* metaData[2];
            metaData[0] = CMMFMetaDataEntry::NewL(name1,data1);
            CleanupStack::PushL(metaData[0]);
            metaArray.Append(metaData[0]);
            metaData[1] = CMMFMetaDataEntry::NewL(name2,data2);
            CleanupStack::PushL(metaData[1]);
            metaArray.Append(metaData[1]);
            
            // Add two meta-Information to be written to the clip header.
            INFO_PRINTF1(_L("iMVSVideoRecordAgent->SetMetaDataL()"));
            TRAP(err, iMVSVideoRecordAgent->SetMetaDataL(metaArray));
            INFO_PRINTF2(_L("iMVSVideoRecordAgent->SetMetaDataL() left with error : %d"), err);
            if((err != KErrNone) && (err != KErrNotSupported))
                {
                CleanupStack::PopAndDestroy(3);
                iTestStepResult = EFail;
                break;
                }
            CleanupStack::PopAndDestroy(3);
            
            // Close iMVSVideoRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->Reset()"));
            iMVSVideoRecordAgent->Close();
            iTestStepResult = EPass;
            break;
        }
    }

//
// RTestStepMVSVideoAgentBasic
//
/**
 *
 * RTestStepMVSVideoAgentBasic
 *
 */
RTestStepMVSVideoAgentBasic::RTestStepMVSVideoAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName), iTestType(aTestType)
    {
    iHeapSize = 1000000; //1MB
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSVideoAgentBasic::DoTestStepPreambleL()
    {    
    // Call base DoTestStepPreambleL()
    return RTestStepMVSVideoAgent::DoTestStepPreambleL();
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSVideoAgentBasic::DoTestStepPostambleL()
    {
    // Base Postamble.
    return RTestStepMVSVideoAgent::DoTestStepPostambleL();
    }

/**
 * DoTestStepL
 */
TVerdict RTestStepMVSVideoAgentBasic::DoTestStepL()
    {
    TPtrC16 ptr(KTestMvsAgents[iAgentUnderTest]);
    INFO_PRINTF3(_L("<<< %S test for %S.>>>"), &iTestType, &ptr);
    TVerdict testResult = EPass;
    TInt err = KErrNone;
    
    if (iTestType.Compare(_L("ALLOC")) == 0)
        {
        //Check for Alloc Test Support
        __UHEAP_SETFAIL(RHeap::EFailNext, 1);
        TAny *testAllocSupport = User::Alloc(1);
        if (testAllocSupport!=NULL)
            {
            delete testAllocSupport;
            testAllocSupport=NULL;
            INFO_PRINTF1(_L("This build does not support ALLOC testing"));
            return EInconclusive;
            }
        __UHEAP_SETFAIL(RHeap::ENone, 0);
        
        //Initialise Variables
        TVerdict allocTestStepResult=EPass;
        TBool badResult = EFalse;
        
        //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
        INFO_PRINTF1(_L("Executing the Test Method call Outside the FOR LOOP."));
        TRAP(err, allocTestStepResult = PerformTestL());
        if (err != KErrNone)
            {
            INFO_PRINTF2(_L("Decoding error, returned error code =  %d"), err);
            return EFail;
            }
        else
            {
            //Check the allocTestStepResult
            if (allocTestStepResult != EPass)
                {
                badResult = ETrue;
                }
            }
        
        TInt failCount = 1;
        TBool completed = EFalse;
        allocTestStepResult = EPass; // reinitialise
        TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
        for(;;)    
            {
            __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
            //__MM_HEAP_MARK;

            //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
            INFO_PRINTF2(_L("Executing the Test Method call #%d."), failCount);
            TRAP(err, allocTestStepResult = PerformTestL());          
            if (err == KErrNone)
                {
                err = iError;       
                iError = KErrNone;
                }
            
            completed = EFalse;
            if (err == KErrNone)
                {
                TAny *testAlloc = NULL;
                testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
                if (testAlloc == NULL)
                    {
                    reachedEnd = ETrue;
                    failCount -= 1;
                    }
                else
                    {
                    User::Free(testAlloc);
                    testAlloc=NULL;
                    }            
                
                //Check the allocTestStepResult
                if (allocTestStepResult != EPass)
                    {
                    badResult = ETrue;
                    }
                
                completed = reachedEnd || badResult;
                }
            else if (err != KErrNoMemory) // bad error code
                {
                completed = ETrue;
                badResult = EFail;
                }            

            //__MM_HEAP_MARKEND;
            __UHEAP_SETFAIL(RHeap::ENone, 0);

            if (completed)
                {
                break; // exit loop
                }

            failCount++;
            }

        //failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc

        if (err != KErrNone || badResult)
            {
            if (badResult)
                {
                INFO_PRINTF3(_L("  Bad result with %d memory allocations tested. The Error returned is %d"), failCount, err);
                }
            else
                {
                INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
                }
            allocTestStepResult = EFail;
            }
        else 
            {
            INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested\n"), failCount);
            allocTestStepResult = EPass;
            }
            testResult = allocTestStepResult;
        }
    else if (iTestType.Compare(_L("LOG")) == 0)
        {
        // Run the test by calling PerformTestL()
        TRAP(err, testResult = PerformTestL());
        if (err != KErrNone)
            {
            INFO_PRINTF2(_L("Encode FAILED with error %d..."), err);
            testResult = EFail;
            }
        
        // Code to Check the existance of new log file here xxxx
        _LIT(KFileLogTestString, "Video Opened");
        _LIT8(KFileLogTestString8, "Video Opened");
        if (CheckLogFileL(KMVSAgentsLogPath, KFileLogTestString8, KFileLogTestString))
            {
            testResult = EPass;
            }
        else
            {
            testResult = EFail;
            }
        }
    return testResult;
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVideoAgentBasic::PerformTestL()
    {
    iTestStepResult = EFail;
    
    // Set the active agent based on iAgentUnderTest & begin the test.
    iActiveAgent = iAgentUnderTest;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    User::LeaveIfError(iError);
    
    return iTestStepResult;
    }


//
// RTestStepMVSVideoRecordAgentBasic
//
/**
 *
 * RTestStepMVSVideoRecordAgentBasic
 *
 */
RTestStepMVSVideoRecordAgentBasic::RTestStepMVSVideoRecordAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType)
    : RTestStepMVSVideoAgentBasic(aTestName, aSectName, aKeyName, aTestType)
    {
    iAgentUnderTest = ETestVideoRecord;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVideoRecordAgentBasic::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds recDuration;
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            User::LeaveIfError(SetCacheSize());
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            TRAP(err, iError = iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviRecordControllerUid));
            if(err!=KErrNone)
            	{
            	RevertCacheSize();
	            }
            User::LeaveIfError(err);
            break;
        case EVidRecorderOpening:
            PrepareState(EVideoOpened, KErrNone);
            break;
        case EVidRecorderOpenComplete:
            // set framerate
            const TReal32 KDefaultFrameRate = 10.00;
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetVideoFrameRateL(10.00)"));
            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameRateL(KDefaultFrameRate));
            if(err)
                {
                RevertCacheSize();
                INFO_PRINTF2(_L("SetFrameRate() after Prepare failed with  %d "), err);
                iTestStepResult = EFail;
                CActiveScheduler::Stop();
                }
            // set framesize
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetVideoFrameSizeL(176,144)"));
            const TSize KDefaultFrameSize(176,144);
            TRAP(err, iMVSVideoRecordAgent->SetVideoFrameSizeL(KDefaultFrameSize));
            if(err)
                {
                RevertCacheSize();                  	
                INFO_PRINTF2(_L("SetFrameSize() after Prepare failed with  %d "), err);
                iTestStepResult = EFail;
                CActiveScheduler::Stop();
                }
                
            // Disable Audio
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->SetAudioEnabledL(EFalse)"));
            TRAP(err, iMVSVideoRecordAgent->SetAudioEnabledL(EFalse));
            if (err != KErrNone)
                {
                RevertCacheSize();
                ERR_PRINTF2(_L("iMVSVideoRecordAgent->SetAudioEnabledL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Record iMVSVideoRecordAgent
            PrepareState(EVideoRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->Record()"));
            iMVSVideoRecordAgent->Record();
            
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EVideoStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->Stop()"));
            iMVSVideoRecordAgent->Stop();
            
            // Get the duration of the recorded clip
            INFO_PRINTF1(_L("CMVSVideoRecordAgent->DurationL()"));
            TRAP(err, recDuration = iMVSVideoRecordAgent->DurationL());
            if (err!=KErrNone)
                {
                RevertCacheSize();
                ERR_PRINTF2(_L("iMVSVideoRecordAgent->DurationL() Left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Check the duration of the recorded clip
            INFO_PRINTF3(_L("Returned Duration = %d, Actual Record duration = %d"), I64INT(recDuration.Int64()), KFiveSeconds);
            if (!TimeComparison(I64INT(recDuration.Int64()), KFiveSeconds, KOneSecond))
                {
                RevertCacheSize();
                ERR_PRINTF1(_L("Record Duration does not match"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Close iMVSVideoRecordAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoRecordAgent->Close();
            iTestStepResult = EPass;
            if(RevertCacheSize()!=KErrNone)
            	{
            	iTestStepResult = EInconclusive;
            	}
            CActiveScheduler::Stop();
            break;
        }
    }

TInt RTestStepMVSVideoRecordAgentBasic::SetCacheSize()
	{
	TInt ret=KErrNone;
	//save the default cache sizes, set the cache size back these values after the test
	int error=UserSvr::HalFunction(EHalGroupVM,EVMHalGetCacheSize,&iCurrentCacheSize,0);
	ret=CheckCacheError(error);
	if(ret==KErrNone)
		{
		error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
		ret=CheckCacheError(error);
		}
	return ret;
	}

TInt RTestStepMVSVideoRecordAgentBasic::CheckCacheError(TInt aError)
	{
	TInt ret=KErrNone;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError!=KErrNone)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			ret=aError;
			}
		}
	#else
	if(aError!=KErrNone)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		ret=aError;
		}
	#endif
	return ret;
	}

TInt  RTestStepMVSVideoRecordAgentBasic::RevertCacheSize()
	{
	TInt ret=0;
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)iCurrentCacheSize.iMinSize,(TAny*)iCurrentCacheSize.iMaxSize);
    ret=CheckCacheError(error);
	    	
    INFO_PRINTF3(_L("Setting Cache Min Size to %d,Setting Cache Max Size is %d"),iCurrentCacheSize.iMinSize,iCurrentCacheSize.iMaxSize);
    return ret;
    	
	}


    
//
// RTestStepMVSVideoPlayAgentBasic
//
/**
 *
 * RTestStepMVSVideoPlayAgentBasic
 *
 */
RTestStepMVSVideoPlayAgentBasic::RTestStepMVSVideoPlayAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType)
    : RTestStepMVSVideoAgentBasic(aTestName, aSectName, aKeyName, aTestType)
    {
    iAgentUnderTest = ETestVideoPlay;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVideoPlayAgentBasic::FsmL(TMvsTestAgentEvents aEventCode)
    {TInt err=KErrNone;
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            User::LeaveIfError(SetCacheSize());
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            TRAP(err,OpenFileL(*iMVSVideoPlayAgent, iFilename));
            if(err)
            	{
            	RevertCacheSize();
	            }
            User::LeaveIfError(err);
            break;
        case EVidPlayerOpening:
            PrepareState(EVideoOpened, KErrNone);
            break;
        case EVidPlayerOpenComplete:
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            TRAP(iError, PlayL(*iMVSVideoPlayAgent));
            
            if (iError != KErrNone)
            	{
            	RevertCacheSize();
            	CActiveScheduler::Stop();
            	}
            else
            	{
            	PrepareState(EVideoOpened, KErrNone);
            	}	
            
            break;
        case EVidPlayerPlayComplete:
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            iTestStepResult = EPass;
            if(RevertCacheSize())
            	{
            	iTestStepResult = EInconclusive;
            	}
            CActiveScheduler::Stop();
            break;
        }
    }

TInt RTestStepMVSVideoPlayAgentBasic::SetCacheSize()
	{
	TInt ret=0;
	//save the default cache sizes, set the cache size back these values after the test
	int error=UserSvr::HalFunction(EHalGroupVM,EVMHalGetCacheSize,&iCurrentCacheSize,0);
	ret=CheckCacheError(error);
	if(!ret)
		{
		error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
		ret=CheckCacheError(error);
		}
	return ret;
	}

TInt RTestStepMVSVideoPlayAgentBasic::CheckCacheError(TInt aError)
	{
	TInt ret=0;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			ret=aError;
			}
		}
	#else
	if(aError)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		ret=aError;
		}
	#endif
	return ret;
	}

TInt  RTestStepMVSVideoPlayAgentBasic::RevertCacheSize()
	{
	TInt ret=0;
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)iCurrentCacheSize.iMinSize,(TAny*)iCurrentCacheSize.iMaxSize);
    ret=CheckCacheError(error);
	    	
    INFO_PRINTF3(_L("Setting Cache Min Size to %d,Setting Cache Max Size is %d"),iCurrentCacheSize.iMinSize,iCurrentCacheSize.iMaxSize);
    return ret;
    	
	}


//
// RTestStepMVSVidPlayUnsupportedFile
//

/**
 *
 * RTestStepMVSVidPlayUnsupportedFile
 *
 */
RTestStepMVSVidPlayUnsupportedFile::RTestStepMVSVidPlayUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    iExpectedError = aExpectedError;
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidPlayUnsupportedFile::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Load corrupted file. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidPlayUnsupportedFile::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            PrepareState(ENotReady, iExpectedError);
            break;
        case EVidPlayerOpening:
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
        }
    }

//
// RTestStepMVSVidRecUnsupportedFile
//

/**
 *
 * RTestStepMVSVidRecUnsupportedFile
 *
 */
RTestStepMVSVidRecUnsupportedFile::RTestStepMVSVidRecUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    #ifdef __X86GCC__
    // twice Heap used for x86 platform
    iHeapSize = 2000000; // 2MB
    #endif // __X86GCC__    
    
    // Find out whether the platform is Naviengine. If so Increase Heap size.
	TInt value = 0;
	TInt err = HAL::Get(HAL::EMachineUid, value);
	if (err != KErrNone)
    	{
	    INFO_PRINTF2(_L("---Problem calling HAL::Get(HAL::EMachineUid); failed with %d"), err);
	    }
	if(value==HAL::EMachineUid_NE1_TB)
		{
		iHeapSize = 2000000; // 2MB  
		}
	}

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidRecUnsupportedFile::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Load a video clip that uses a codec that isn't supported. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoRecordAgent as active agent & begin the test
    iActiveAgent = ETestVideoRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidRecUnsupportedFile::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSVideoRecordAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSVideoRecordAgent->OpenFileL(iFilename, KTestAviPlayControllerUid);
            PrepareState(ENotReady, KErrNotSupported);
            break;
        case EVidRecorderOpening:
            // Close iMVSVideoRecordAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoRecordAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
        }
    }

//
// RTestStepMVSVidAviPlayChangeWin
//

/**
 *
 * RTestStepMVSVidAviPlayChangeWin
 *
 */
RTestStepMVSVidAviPlayChangeWin::RTestStepMVSVidAviPlayChangeWin(const TDesC& aTestName, 
                                                                 const TDesC& aSectName, 
                                                                 const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName),
      iPlayAgain(EFalse)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayChangeWin::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file,  Play with different display window setting test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayChangeWin::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {            
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
            if (!iPlayAgain)
                {
                iPlayAgain = ETrue;
                
                // Play iMVSVideoPlayAgent
                PrepareState(EVideoPlaying, KErrNone);
                INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play() with specific video extent & window clippings"));
                PlayL(*iMVSVideoPlayAgent, 
                      TRect(TPoint(0, 0), TSize(100, 100)), 
                      TRect(TPoint(0, 0), TSize(100, 100)));
                PrepareState(EVideoOpened, KErrNone);                      
                }
            else
                {
                // Close iMVSVideoPlayAgent
                PrepareState(ENotReady, KErrNone);
                iMVSVideoPlayAgent->Close();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                }            
            }
            break;            
        }
    }


//
// RTestStepMVSVidAviPlayOverlayText
//

/**
 *
 * RTestStepMVSVidAviPlayOverlayText
 *
 */
RTestStepMVSVidAviPlayOverlayText::RTestStepMVSVidAviPlayOverlayText(const TDesC& aTestName, 
                                                                     const TDesC& aSectName, 
                                                                     const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)    
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayOverlayText::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file,  Play with overlay text test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayOverlayText::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {     
            // Draw the overlay text
            TRect rect(iWindow->Size());
            TInt baseline = (rect.Height() + iFont->AscentInPixels()) >> 1;
            
            iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
            iGc->SetBrushColor(KRgbRed);
                 
            iGc->SetPenStyle(CGraphicsContext::ESolidPen);
            iGc->SetPenColor(KRgbBlue);
            
            iGc->DrawText(_L("Overlay Testing"), rect, baseline, CGraphicsContext::ELeft);            
            
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            PlayL(*iMVSVideoPlayAgent);					                

            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            }
            break;            
        }
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSVidAviPlayOverlayText::DoTestStepPreambleL()
    {
    // Call base DoTestStepPreambleL()
    TVerdict verdict = RTestStepMVSVideoAgent::DoTestStepPreambleL();
    if (verdict == EPass)
        {
        iGc = new (ELeave) CWindowGc(iScreen);
        User::LeaveIfError(iGc->Construct());
        
        TFontSpec fontSpec; // use default fonts
        fontSpec.iHeight = 20;

        // Get a font to draw overlay text
        if (iScreen->GetNearestFontToDesignHeightInPixels(iFont, fontSpec) != KErrNone)
            {
            // if no font can be retrieved, abort the test
            ERR_PRINTF1(_L("Cannot retrieve font to perform test.  Test aborted."));
            
    		return EInconclusive;
            }

        iGc->Activate(*iWindow);
        iGc->UseFont(iFont);
        }
    
    return verdict;
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSVidAviPlayOverlayText::DoTestStepPostambleL()
    {
    if (iGc)
        {            
        iGc->DiscardFont();    
        iGc->Deactivate();                  
        delete iGc;      
        }
               
    return RTestStepMVSVideoAgent::DoTestStepPostambleL();        
    }


//
// RTestStepMVSVidAviPlayCropRegion
//

/**
 *
 * RTestStepMVSVidAviPlayCropRegion
 *
 */
RTestStepMVSVidAviPlayCropRegion::RTestStepMVSVidAviPlayCropRegion(const TDesC& aTestName, 
                                                                   const TDesC& aSectName, 
                                                                   const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayCropRegion::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file,  Play with different display window setting test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayCropRegion::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {  
            TSize frameSize;
            iMVSVideoPlayAgent->VideoFrameSizeL(frameSize);            
            
            TRect cropRegion(frameSize);
            cropRegion.SetHeight(cropRegion.Height() / 2);
            
            // set the crop region to half of the video frame's height
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetCropRegionL()"));                    
            TRAP(iError, iMVSVideoPlayAgent->SetCropRegionL(cropRegion));
            
            // Expected error for SetCropRegionL and GetCropRegionL.
#ifdef SYMBIAN_BUILD_GCE
			TInt expected = GCEAvailable() ? KErrNone : KErrNotSupported;
#else
			TInt expected = KErrNotSupported;
#endif

		    if (iError != expected)
		        {
	        	ERR_PRINTF3(_L("SetCropRegionL() left with %d.  Expected %d."), iError, expected);
	        	iTestStepResult = EFail;
		        
		        CActiveScheduler::Stop();
		        break;
		        }

            // retrieve the crop region again
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->GetCropRegionL()"));                                            
            TRect newCropRegion;
            TRAP(iError, iMVSVideoPlayAgent->GetCropRegionL(newCropRegion));
            
            if (iError != expected)
            	{
	        	ERR_PRINTF3(_L("GetCropRegionL() left with %d.  Expected %d."), iError, expected);
	        	iTestStepResult = EFail;
            	
                CActiveScheduler::Stop();
                break;
                }            
            
            if (iError == KErrNone &&
            	cropRegion != newCropRegion)
                {
                ERR_PRINTF1(_L("CMVSVideoPlayAgent->GetCropRegionL() did not reture previously set crop region."));        
                CActiveScheduler::Stop();                
                iTestStepResult = EFail;
                break;
                }
                      
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            TRAP(iError, PlayL(*iMVSVideoPlayAgent));					                
            if (iError != KErrNone)
                {
                CActiveScheduler::Stop();
                iTestStepResult = EFail;       
                break;             
                }
            
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            }
            break;            
        }
    }


//
// RTestStepMVSVidAviPlayDisplaySetting
//

/**
 *
 * RTestStepMVSVidAviPlayDisplaySetting
 *
 */
RTestStepMVSVidAviPlayDisplaySetting::RTestStepMVSVidAviPlayDisplaySetting(const TDesC& aTestName, 
                                                                           const TDesC& aSectName, 
                                                                           const TDesC& aKeyName)
    : RTestStepMVSVideoAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSVidAviPlayDisplaySetting::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open a AVI (Xvid) file,  Play with different display window setting test. >>>"));
    iTestStepResult = EFail;
    
    // Set the VideoPlayAgent as active agent & begin the test
    iActiveAgent = ETestVideoPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSVidAviPlayDisplaySetting::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            {
            // Open iMVSVideoPlayAgent
            PrepareState(EVideoOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSVideoPlayAgent->OpenFileL() : %S"), &iFilename);
            OpenFileL(*iMVSVideoPlayAgent, iFilename);
            break;
            }
            
        case EVidPlayerOpening:
            {
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerOpenComplete:
            {  
            TRect videoExtent(iWindow->Size());
            videoExtent.SetHeight(videoExtent.Height() / 2);
            
            TRect windowClipping(videoExtent);
            windowClipping.SetHeight(videoExtent.Height() / 2);
                        
            // Play iMVSVideoPlayAgent
            PrepareState(EVideoPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSVideoPlayAgent->Play()"));
            TRAP(iError, PlayL(*iMVSVideoPlayAgent, videoExtent, windowClipping));            
            if (iError != KErrNone)
                {
                INFO_PRINTF2(_L("PlayL failed with %d."), iError);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                }
            PrepareState(EVideoOpened, KErrNone);
            break;
            }
            
        case EVidPlayerPlayComplete:
            {
            // Close iMVSVideoPlayAgent
            PrepareState(ENotReady, KErrNone);
            iMVSVideoPlayAgent->Close();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            }
            break;            
        }
    }

/**
 *
 *	PlayL
 *
 **/
#ifdef SYMBIAN_BUILD_GCE
void RTestStepMVSVidAviPlayDisplaySetting::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                                 const TRect& aVideoExtent,
                                                 const TRect& aWindowClipping)    
#else
void RTestStepMVSVidAviPlayDisplaySetting::PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                                                 const TRect& /*aVideoExtent*/,
                                                 const TRect& /*aWindowClipping*/)
#endif // SYMBIAN_BUILD_GCE
    {
#ifdef SYMBIAN_BUILD_GCE    
    if (aVideoPlayAgent.SupportVideoPlayerUtility2())
        {
        aVideoPlayAgent.RemoveDisplayWindowL(*iWindow);
        aVideoPlayAgent.AddDisplayWindowL(iWs, *iScreen, *iWindow);
        
        INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetVideoExtentL()"));        
        aVideoPlayAgent.SetVideoExtentL(aVideoExtent);
        
        INFO_PRINTF1(_L("CMVSVideoPlayAgent->SetWindowClipRectL()"));                
        aVideoPlayAgent.SetWindowClipRectL(aWindowClipping);
        }       
#endif        
    aVideoPlayAgent.Play();            
    }




//
// CMmfVideoPlayTimer
//

/**
 * NewL
 */
CMmfVideoPlayTimer* CMmfVideoPlayTimer::NewL(RTestStepMVSVideoAgent* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMmfVideoPlayTimer* self = new (ELeave) CMmfVideoPlayTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CMmfVideoPlayTimer::CMmfVideoPlayTimer(RTestStepMVSVideoAgent* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityAbsoluteHigh), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CMmfVideoPlayTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CMmfVideoPlayTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CMmfVideoPlayTimer::RunError(TInt aError)
    {
  //  iParent->FsmL(EVidGeneralErr);
    return aError;
    }

//
// CMvsVideoRecordTimer
//

/**
 * NewL
 */
CMvsVideoRecordTimer* CMvsVideoRecordTimer::NewL(RTestStepMVSVidRecPause* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMvsVideoRecordTimer* self = new (ELeave) CMvsVideoRecordTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CMvsVideoRecordTimer::CMvsVideoRecordTimer(RTestStepMVSVidRecPause* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CMvsVideoRecordTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CMvsVideoRecordTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CMvsVideoRecordTimer::RunError(TInt aError)
    {
  //  iParent->FsmL(EVidGeneralErr);
    return aError;
    }

