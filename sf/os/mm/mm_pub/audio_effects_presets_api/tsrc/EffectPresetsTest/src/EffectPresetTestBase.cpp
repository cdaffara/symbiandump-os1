/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  EffectsPresets
*
*/


// INCLUDE FILES

#include "EffectPresetsTest.h"
#include "EffectPresetTestCore.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt CEffectPresetsTest::KTestCaseEffectPreset=1201;
const TInt CEffectPresetsTest::MAX_TESTCASE_ID=1201;


// ============================= LOCAL FUNCTIONS ===============================

TInt CEffectPresetsTest::RunTestCaseFunctionL(TInt id, CStifSectionParser *section, TTestResult &aResult)
	{
	TInt retValue=KErrConfigInvalid;
	aResult.iResult=KErrNotExecuted;

	switch (id)
	{
		case KTestCaseEffectPreset:
		    retValue = TestCaseAudioEffectPresetL(section, aResult);
		    break;

		default:
			aResult.iResultDes.Copy(KConfigInvalid());
			aResult.iResult = KErrNotExecuted;
	}

	return retValue;
	}

TInt CEffectPresetsTest::TestCaseAudioEffectPresetL(CStifSectionParser *aSection, TTestResult & aResult)
	{
	TInt err = KErrNone;

	if (CreateEffectUtilityL(aSection, aResult))
		return KErrExecuted;

	if (AssignEffectObjectL(aSection, aResult))
	{
		deleteObject();
		return KErrExecuted;
	}

	CStifItemParser *item = aSection->GetNextItemLineL(_L(""));
	while (item)
	{
		CleanupStack::PushL(item);
		err = iTestAudioEffectPreset->ProcessTag(item, aResult);
		CleanupStack::PopAndDestroy(item);

		if (err)
		{
			item = NULL;
		}
		else
		{
			item = aSection->GetNextItemLineL(_L(""));
		}
	}

	deleteObject();
	return KErrExecuted;
	}


TInt CEffectPresetsTest::CreateEffectUtilityL(CStifSectionParser *aSection, TTestResult & aResult)
	{
	TInt err = KErrNone;

	// read in audio effect
	TPtrC audioEffectPtr;
	if ( aSection->GetLine(KTagAudioEffectUtility, audioEffectPtr, ENoTag) )
	{
		iLogger->Log(_L("Missing tag AudioEffect"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted ;
		return KErrArgument;
	}

	iLogger->Log(_L("Audio Effect [%S]"), &audioEffectPtr);

	err = SetUtility(audioEffectPtr);
	if (err)
	{
		iLogger->Log(_L("Unable to create Audio Effect."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		aResult.iResult = err ;
		return KErrArgument;
	}

	iLogger->Log(_L("Create Audio Effect Successful."));
	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = err ;
	return err;
	}

TInt CEffectPresetsTest::SetUtility(TPtrC aAudioEffectUtilityName)
	{
	TInt err = KErrNone;

	if (!aAudioEffectUtilityName.Compare(KEqualizerUtility))
	{
		TRAP(err, iTestAudioEffectPreset = CTestAudioEqualizerUtility::NewL(iLogger));
		iObject = EEqualizer;
		return err;
	}

	if (!aAudioEffectUtilityName.Compare(KEnvironmentalReverbUtility))
	{
		TRAP(err, iTestAudioEffectPreset = CTestEnvironmentalReverbUtility::NewL(iLogger));
        iObject = EEnvironmentalReverb;
		return err;
	}

	if (!aAudioEffectUtilityName.Compare(KStereoWideningUtility))
	{
		TRAP(err, iTestAudioEffectPreset = CTestStereoWideningUtility::NewL(iLogger));
        iObject = EStereoWidening;
		return err;
	}

	return KErrArgument;
	}


TInt CEffectPresetsTest::AssignEffectObjectL(CStifSectionParser *aSection, TTestResult & aResult)
	{
	TInt err = KErrNone;

	// read in audio effect
	TPtrC assignEffectPtr;
	if ( aSection->GetLine(KTagEffectObject, assignEffectPtr, ENoTag) )
	{
		iLogger->Log(_L("Missing tag AssignEffect"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted ;
		return KErrArgument;
	}

	iLogger->Log(_L("Assign Effect to [%S]"), &assignEffectPtr);

	err = AssignObject(assignEffectPtr);
	if (err)
	{
		iLogger->Log(_L("Unable to assign Audio Effect."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		aResult.iResult = err ;
		return KErrArgument;
	}


	iLogger->Log(_L("Assign Audio Effect Successful."));
	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = err ;
	return err;
	}


TInt CEffectPresetsTest::AssignObject(TPtrC aAssignEffectObjectName)
	{
	TInt err = KErrNone;

	if (!aAssignEffectObjectName.Compare(KPlayer))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioPlayer);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KRecorder))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioRecorder);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KConverter))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioConverter);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KTone))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioTone);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KDevSound))
	{
		err = iTestAudioEffectPreset->AssignObject(iDevSound);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KCustomCommand))
	{
		iCustomCommandUtility = CCustomCommandUtility::NewL(*iAudioPlayer);
		err = iTestAudioEffectPreset->AssignObject(iCustomCommandUtility);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KCustomInterface))
	{
		iCustomCommandUtility = CCustomCommandUtility::NewL(*iAudioPlayer);
		iCustomInterfaceUtility = CCustomInterfaceUtility::NewL(*iCustomCommandUtility);
	    err = iTestAudioEffectPreset->AssignObject(iCustomInterfaceUtility);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KInputStream))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioInputStream);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KOutputStream))
	{
		err = iTestAudioEffectPreset->AssignObject(iAudioOutputStream);
		return err;
	}

	if (!aAssignEffectObjectName.Compare(KMidiClient))
		{
		err = iTestAudioEffectPreset->AssignObject(iMidiClientUtility);
		return err;
		}

	if (!aAssignEffectObjectName.Compare(KDrmPlayer))
		{
		err = iTestAudioEffectPreset->AssignObject(iDrmPlayerUtility);
		return err;
		}

	if (!aAssignEffectObjectName.Compare(KVideoPlayer))
		{
		err = iTestAudioEffectPreset->AssignObject(iVideoPlayerUtility);
		return err;
		}
	return KErrArgument;
	}


void CEffectPresetsTest::deleteObject()
	{
	if (iTestAudioEffectPreset != NULL)
	{
	    switch(iObject)
    	{
			case EEqualizer:
        		delete (CTestAudioEqualizerUtility*)iTestAudioEffectPreset;
				break;
			case EEnvironmentalReverb:
    	    	delete (CTestEnvironmentalReverbUtility*)iTestAudioEffectPreset;
				break;
			case EStereoWidening:
    	    	delete (CTestStereoWideningUtility*)iTestAudioEffectPreset;
				break;
    	}
	}

	iTestAudioEffectPreset = NULL;
	iObject = EENone;
	}


// ========== Callback Functions ==========
//
void CEffectPresetsTest::MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds& aDuration)
	{
	iLogger->Log(_L("MapcInit: %d"), aStatus);
	TInt d = I64INT(aDuration.Int64());
	RDebug::Print(_L("CEffectPresetsTest::MapcInitComplete :-> Status[%d] Duration[%d]"), aStatus, d);

	if (aStatus != KErrNone)
		{
		iLogger->Log(_L("Cannot open "));
		iLogger->Log(KAMRTestFile);
		iLogger->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
			iLogger->Log(_L("Ready!"));
		}

	CActiveScheduler::Stop();
	}


void CEffectPresetsTest::MapcPlayComplete(TInt aErr)
	{
	iLogger->Log(_L("MapcPlay: %d"), aErr);
	RDebug::Print(_L("CEffectPresetsTest::MapcPlayComplete :-> Error[%d]"), aErr);
	}


void CEffectPresetsTest::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	RDebug::Print(_L("aPreviousState[%d], aCurrentState[%d], aErrorCode[%d]"), aPreviousState, aCurrentState, aErrorCode);
	iLogger->Log(_L("Mosco: %d"), aErrorCode);
	CActiveScheduler::Stop();
	}


void CEffectPresetsTest::MatoPrepareComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MatoPrepareComplete: %d"), aError);
	}


void CEffectPresetsTest::MatoPlayComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MatoPlayComplete: %d"), aError);
	}


void CEffectPresetsTest::MaiscOpenComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscOpenComplete: %d"), aError);
	}


void CEffectPresetsTest::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscBufferCopied: %d"), aError);
	}


void CEffectPresetsTest::MaiscRecordComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscRecordComplete: %d"), aError);
	}


void CEffectPresetsTest::MaoscOpenComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscOpenComplete: %d\n"), aError);
	}


void CEffectPresetsTest::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscBufferCopied: %d"), aError);
	}


void CEffectPresetsTest::MaoscPlayComplete(TInt aError)
	{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscPlayComplete: %d"), aError);
	}

void CEffectPresetsTest::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState aNewState,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	RDebug::Print(_L("MmcuoStateChanged->MidiState %d"), aNewState);
	iLogger->Log(_L("MmcuoStateChanged: %d\n"), aError);
	}

void CEffectPresetsTest::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
	{
	RDebug::Print(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d"), aMicroBeatsPerMinute);
	iLogger->Log(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d\n"), aMicroBeatsPerMinute);
	}


void CEffectPresetsTest::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 aVolumeInDecibels)
	{
	RDebug::Print(_L("MmcuoVolumeChanged->Volume %f"), aVolumeInDecibels);
	iLogger->Log(_L("MmcuoVolumeChanged->Volume %f\n"), aVolumeInDecibels);
	}


void CEffectPresetsTest::MmcuoMuteChanged(TInt /*aChannel*/,TBool aMuted)
	{
	RDebug::Print(_L("MmcuoMuteChanged->Muted %d"), aMuted);
	iLogger->Log(_L("MmcuoMuteChanged->Muted %d\n"), aMuted);
	}

void CEffectPresetsTest::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 aMicroBeats)
	{
	RDebug::Print(_L("MmcuoSyncUpdate->MicroBeats %d"), aMicroBeats);
	iLogger->Log(_L("MmcuoSyncUpdate->MicroBeats %d\n"), aMicroBeats);
	}

void CEffectPresetsTest::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	RDebug::Print(_L("MmcuoMetaDataEntryFound->EntryId %d"), aMetaDataEntryId);
	iLogger->Log(_L("MmcuoMetaDataEntryFound->EntryId %d\n"), aMetaDataEntryId);
	}

void CEffectPresetsTest::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
	{
	RDebug::Print(_L("MmcuoMipMessageReceived"));
	iLogger->Log(_L("MmcuoMipMessageReceived\n"));
	}

void CEffectPresetsTest::MmcuoPolyphonyChanged(TInt aNewPolyphony)
	{
	RDebug::Print(_L("MmcuoPolyphonyChanged: %d"), aNewPolyphony);
	iLogger->Log(_L("MmcuoPolyphonyChanged: %d\n"), aNewPolyphony);
	}

void CEffectPresetsTest::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt aInstrumentId)
	{
	RDebug::Print(_L("MmcuoInstrumentChanged->InstrumentId %d"), aInstrumentId);
	iLogger->Log(_L("MmcuoInstrumentChanged->InstrumentId %d\n"), aInstrumentId);
	}

void CEffectPresetsTest::MdapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds& aDuration)
	{
	iLogger->Log(_L("MdapcInit: %d"), aStatus);
	TInt d = I64INT(aDuration.Int64());
	RDebug::Print(_L("CEffectPresetsTest::MdapcInitComplete :-> Status[%d] Duration[%d]"), aStatus, d);
	if (aStatus != KErrNone)
		{
		iLogger->Log(_L("Cannot open "));
		iLogger->Log(KAMRTestFile);
		iLogger->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
		iLogger->Log(_L("Ready!"));
		}

	CActiveScheduler::Stop();
	}

void CEffectPresetsTest::MdapcPlayComplete(TInt aErr)
	{
		iLogger->Log(_L("MdapcPlay: %d"), aErr);
		RDebug::Print(_L("CEffectPresetsTest::MdapcPlayComplete :-> Error[%d]"), aErr);
	}


void CEffectPresetsTest::MvpuoOpenComplete(TInt aError)
	{

	iLogger->Log(_L("MvpuoOpenComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoOpenComplete :-> Error[%d]"), aError);

	if (aError != KErrNone)
		{
		iLogger->Log(_L("Cannot open "));
		iLogger->Log(K3gpTestFile);
		iLogger->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
			iLogger->Log(_L("Ready!"));
		}

	CActiveScheduler::Stop();
	}

void CEffectPresetsTest::MvpuoPrepareComplete(TInt aError)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[%d]"), aError);
	}

void CEffectPresetsTest::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt aError)
	{
	iLogger->Log(_L("MvpuoFrameReady: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoFrameReady :-> Error[%d]"), aError);
	}

void CEffectPresetsTest::MvpuoPlayComplete(TInt aError)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[%d]"), aError);
	}

void CEffectPresetsTest::MvpuoEvent(const TMMFEvent& aEvent)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: %x"), aEvent);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[%x]"), aEvent);
	}
