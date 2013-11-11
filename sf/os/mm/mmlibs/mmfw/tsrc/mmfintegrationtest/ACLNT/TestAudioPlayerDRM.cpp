// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Integration tests.
// 
//

#include "TestAudioPlayerDRM.h"

// CTestMmfDRMAudioPlayerBase implementation

/**
*
* CTestMmfDRMAudioPlayerBase()
*
* Constructor to set the Test step name, section name and key names.
*
*/
CTestMmfDRMAudioPlayerBase::CTestMmfDRMAudioPlayerBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	{
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iUniqueId = aUniqueId;
	}

/**
*
* FsmL()
*
* @param TMmfAudioEvents aEvent
*
*/
void CTestMmfDRMAudioPlayerBase::FsmL(TMmfAudioEvents aEvent, TInt aError)
    {
    switch (aEvent)
        {
        case EAudioOpen:
            INFO_PRINTF3(_L("Opening the file for PLAY intent : %S, UniqueID : %S"), &iFileName, &iUniqueId);
            iAudioPlayer->OpenFileL(TMMFileSource(iFileName, iUniqueId, ContentAccess::EPlay));
            break;

        case EAudioPlay:
            if (aError != KErrNone) // Check if there's error while opening the file
                {
                ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), aError);
                CActiveScheduler::Stop();
                }
            else
                {
                INFO_PRINTF1(_L("Playing the audio file..."));
                AfterOpenL();
                iAudioPlayer->Play();
                }
            break;

        case EAudioPlayEnd:
            if (aError != KErrNone) // Check if there's error while playing the file
                {
                ERR_PRINTF2(_L("Playing the audio file failed, unexpected error %d."), aError);
                }
            else
                {
                INFO_PRINTF1(_L("Audio file opened and played."));
                iTestStepResult = EPass;
                }
			CActiveScheduler::Stop();
            break;

        default:
            INFO_PRINTF1(_L("Invalid Audio event!"));
        }
    };
    
void CTestMmfDRMAudioPlayerBase::AfterOpenL()
	{
	}
	
/**
*
* Test step Preamble.
*
* @return TVerdict: The result of the test step.
* 
*/
TVerdict CTestMmfDRMAudioPlayerBase::DoTestStepPreambleL()
    {
    // Install the scheduler
    TVerdict verdict = CTestMmfAclntStep::DoTestStepPreambleL();

    iError = KErrNone;

    // Get the filename
    TPtrC filename1;
    if (!GetStringFromConfig(iSectName, iKeyName, filename1))
        {
        return EInconclusive;
        }
    GetDriveName(iFileName);
    iFileName.Append(filename1);

    // Create an audio player
    iAudioPlayer = CMdaAudioPlayerUtility::NewL(*this);
    if (iAudioPlayer == NULL)
        {
        return EInconclusive;
        }

    return verdict;
    }

/**
*
* Test step Postamble.
*
* @return TVerdict: The result of the test step.
* 
*/
TVerdict CTestMmfDRMAudioPlayerBase::DoTestStepPostambleL()
    {
    delete iAudioPlayer;
    iAudioPlayer = NULL;
    User::After(KOneSecond); // wait for deletion to shut down devsound
    return CTestMmfAclntStep::DoTestStepPostambleL(); // Destroy the scheduler
    }

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayerBase::MapcInitComplete(TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Init Complete. Error = %d"), aError);
    iError = aError;
	if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), aError);
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
	else
		{
		TRAP(iError, FsmL(EAudioPlay, aError)); // Call to play audio
		if (iError != KErrNone)
    		{
    		ERR_PRINTF2(_L("Got an error from the FsmL(EAudioPlay) %d."), iError);
    		iAudioPlayer->Close();
    		CActiveScheduler::Stop();    		
    		}
		}
	}

/**
*
* Implementation of the MMdaAudioPlayerCallback interface function.
*
* @param TInt aError
*
*/
void CTestMmfDRMAudioPlayerBase::MapcPlayComplete(TInt aError)
    {
    INFO_PRINTF2(_L("MMdaAudioPlayerCallback Play Complete. Error = %d"), aError);
    iError = aError;
	if (iError != KErrNone)
    	{
    	ERR_PRINTF2(_L("Opening the audio file failed, unexpected error %d."), aError);
    	iAudioPlayer->Close();
    	CActiveScheduler::Stop();
    	}
	else
		{
		TRAP(iError, FsmL(EAudioPlayEnd, aError));
		if (iError != KErrNone)
    		{
    		ERR_PRINTF2(_L("Got an error from the FsmL(EAudioPlayEnd) %d."), iError);
    		iAudioPlayer->Close();
    		CActiveScheduler::Stop();    		
    		}
		}
    }


// Positive Tests

/**
*
* NewL()
*
* @param const TDesC& aTestName
*
* @return CTestMmfAudioPlayDRMEnableAgentUI*
*                       Constructed CTestMmfAudioPlayDRMEnableAgentUI object
*
*/
CTestMmfAudioPlayDRMEnableAgentUI* CTestMmfAudioPlayDRMEnableAgentUI::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
    {
    return new (ELeave) CTestMmfAudioPlayDRMEnableAgentUI(aTestName, aSectName, aKeyName, aUniqueId);
    }

/**
*
* Test step constructor.
* Ctor for CTestMmfAudioPlayDRMEnableAgentUI.
*
* @param const TDesC& aTestName
*
*/
CTestMmfAudioPlayDRMEnableAgentUI::CTestMmfAudioPlayDRMEnableAgentUI(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	:CTestMmfDRMAudioPlayerBase(aTestName, aSectName, aKeyName, aUniqueId)
    {
    }

void CTestMmfAudioPlayDRMEnableAgentUI::AfterOpenL()
	{
    // Create DRM custom command
    MMMFDRMCustomCommand* drmCustomCommand;
    drmCustomCommand = iAudioPlayer->GetDRMCustomCommand();
    if (drmCustomCommand == NULL)
        {
        ERR_PRINTF1(_L("MMMFDRMCustomCommand is NULL."));
        iError = KErrUnknown;
        User::Leave(KErrUnknown);
        }

    INFO_PRINTF1(_L("Enabling Agent's user interface for errors and confirmation requests..."));
    iError = drmCustomCommand->SetAgentProperty(ContentAccess::EAgentPropertyAgentUI, 3);
    // After DevCR JFOT-6EMFVL (which changes the CContentFile::SetAgentProperty to call 
    // SetProperty on a member of CData instead of CContent), the underlying reference test
    // agent returns a different error value (KErrCANotSupported instead of KErrNone).
    if (iError == KErrCANotSupported)
        {
        // Reset the error code as it's not really an error :)
		iError = KErrNone;
        }
    else
    	{
        ERR_PRINTF2(_L("Enabling Agent's user interface failed, unexpected error %d."), iError);
        User::Leave(iError);
    	}
	}

/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfAudioPlayDRMEnableAgentUI::DoTestStepL()
    {
    INFO_PRINTF1(_L("Enable agent's user interface for errors and confirmation requests property. Open and play a protected audio clip from a file for PLAY intent."));

    iTestStepResult = EFail;

    TRAPD(err,FsmL(EAudioOpen)); // Call to open the audio file
    if (err == KErrNone)
    	{
    	CActiveScheduler::Start(); // ActiveScheduler started ONLY once
    	if (iError == KErrNone)
			{
			iTestStepResult=EPass;
			}
	   	}
    return iTestStepResult;
    }


/**
*
* NewL()
*
* @param const TDesC& aTestName
*
* @return CTestMmfAudioPlayDRMDisableAutoIntent*
*                       Constructed CTestMmfAudioPlayDRMDisableAutoIntent object
*
*/
CTestMmfAudioPlayDRMDisableAutoIntent* CTestMmfAudioPlayDRMDisableAutoIntent::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
    {
    return new (ELeave) CTestMmfAudioPlayDRMDisableAutoIntent(aTestName, aSectName, aKeyName, aUniqueId);
    }

/**
*
* Test step constructor.
* Ctor for CTestMmfAudioPlayDRMDisableAutoIntent.
*
* @param const TDesC& aTestName
*
*/
CTestMmfAudioPlayDRMDisableAutoIntent::CTestMmfAudioPlayDRMDisableAutoIntent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId)
	:CTestMmfDRMAudioPlayerBase(aTestName, aSectName, aKeyName, aUniqueId)
    {
    }
    
void CTestMmfAudioPlayDRMDisableAutoIntent::AfterOpenL()
	{
    // Create DRM custom command
    MMMFDRMCustomCommand* drmCustomCommand;
    drmCustomCommand = iAudioPlayer->GetDRMCustomCommand();

    if (drmCustomCommand == NULL)
        {
        ERR_PRINTF1(_L("MMMFDRMCustomCommand is NULL."));
        iError = KErrUnknown;
        User::Leave(KErrUnknown);
        }

    INFO_PRINTF1(_L("Disabling automatic intent..."));
    iError = drmCustomCommand->DisableAutomaticIntent(EFalse);
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Disabling the automatic intent failed, unexpected error %d."), iError);
        User::Leave(iError);
        }	
	}
	
/**
*
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
*
* @return TVerdict: The result of the test step.
*
*/
TVerdict CTestMmfAudioPlayDRMDisableAutoIntent::DoTestStepL()
	{

    INFO_PRINTF1(_L("Open a protected audio file and disable automatic intent. Play."));

    iTestStepResult = EFail;

    TRAPD(err,FsmL(EAudioOpen)); // Call to open the audio file
      
    if(err == KErrNone)  	
    	{
    	
   		CActiveScheduler::Start(); // ActiveScheduler started ONLY once
       
    	if (iError == KErrNone)
       		{
        		INFO_PRINTF1(_L("Executing intent..."));
	    		MMMFDRMCustomCommand* drmCustomCommand;
	    		drmCustomCommand = iAudioPlayer->GetDRMCustomCommand();

	    	if (drmCustomCommand == NULL)
	        	{
	        	ERR_PRINTF1(_L("MMMFDRMCustomCommand is NULL."));
	        	User::Leave(KErrUnknown);
	        	}
        
        	iError = drmCustomCommand->ExecuteIntent(ContentAccess::EPlay);
        	if (iError != KErrNone)
            	{
            	ERR_PRINTF2(_L("Executing the intent failed, unexpected error %d."), iError);
            	}
        	}
        
	   	if (iError == KErrNone)
	   		{
			iTestStepResult = EPass;
			}

    	}
    return iTestStepResult;
    }
