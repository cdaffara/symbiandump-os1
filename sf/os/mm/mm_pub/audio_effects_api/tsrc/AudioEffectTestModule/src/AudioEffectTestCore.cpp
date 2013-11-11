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
* Description: AudioEffectTestModule
*
*/


// INCLUDE FILES

//#include <e32svr.h>
//#include <e32cons.h>
#include "AudioEffectTestModule.h"
#include "AudioEffectTestObjects.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt CAudioEffectTestModule::KTestCaseAudioEffect=1101;
const TInt CAudioEffectTestModule::MAX_TESTCASE_ID=1101;

// MACROS
//#define ?macro ?macro_def
// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

TInt CAudioEffectTestModule::RunTestCaseFunctionL(TInt id, CStifSectionParser *section, TTestResult &aResult)
{
	TInt retValue=KErrConfigInvalid;
	aResult.iResult=KErrNotExecuted;

	switch (id)
	{
		case KTestCaseAudioEffect:
		    retValue = TestCaseAudioEffectL(section, aResult);
		    break;

		default:
			aResult.iResultDes.Copy(KConfigInvalid());
			aResult.iResult = KErrNotExecuted;
	}

	return retValue;
}


TInt CAudioEffectTestModule::TestCaseAudioEffectL(CStifSectionParser *aSection, TTestResult & aResult)
{
	TInt err = KErrNone;

	if (CreateEffectL(aSection, aResult))
		return KErrExecuted;

	if (AssignEffectL(aSection, aResult))
	{
		DeleteObject();
		return KErrExecuted;
	}

	CStifItemParser *item = aSection->GetItemLineL(_L(""));
	while (item)
	{
		CleanupStack::PushL(item);
		err = iTestAudioEffect->ProcessTag(item, aResult);
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

	DeleteObject();
	return KErrExecuted;
}


TInt CAudioEffectTestModule::CreateEffectL(CStifSectionParser *aSection, TTestResult & aResult)
{
	TInt err = KErrNone;

	// read in audio effect
	TPtrC audioEffectPtr;
	if ( aSection->GetLine(KTagAudioEffect, audioEffectPtr, ENoTag) )
	{
		iLogger->Log(_L("Missing tag AudioEffect"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted ;
		return KErrArgument;
	}

	iLogger->Log(_L("Audio Effect [%S]"), &audioEffectPtr);

	err = SetObject(audioEffectPtr);
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


TInt CAudioEffectTestModule::SetObject(TPtrC aAudioEffectName)
{
	TInt err = KErrNone;

	if (!aAudioEffectName.Compare(KEqualizer))
	{
		TRAP(err, iTestAudioEffect = CTestAudioEqualizer::NewL(iLogger));
		iObject = EEqualizer;
		return err;
	}

	if (!aAudioEffectName.Compare(KBassBoost))
	{
		TRAP(err, iTestAudioEffect = CTestBassBoost::NewL(iLogger));
        iObject = EBassBoost;
		return err;
	}

	if (!aAudioEffectName.Compare(KDistanceAttenuation))
	{
		TRAP(err, iTestAudioEffect = CTestDistanceAttenuation::NewL(iLogger));
        iObject = EDistanceAttenuation;
		return err;
	}

	if (!aAudioEffectName.Compare(KListenerDoppler))
	{
		TRAP(err, iTestAudioEffect = CTestDoppler::NewListenerL(iLogger));
        iObject = EListenerDoppler;
		return err;
	}

	if (!aAudioEffectName.Compare(KSourceDoppler))
	{
		TRAP(err, iTestAudioEffect = CTestDoppler::NewSourceL(iLogger));
        iObject = ESourceDoppler;
		return err;
	}

	if (!aAudioEffectName.Compare(KEnvironmentalReverb))
	{
		TRAP(err, iTestAudioEffect = CTestEnvironmentalReverb::NewL(iLogger));
        iObject = EEnvironmentalReverb;
		return err;
	}

	if (!aAudioEffectName.Compare(KListenerLocation))
	{
		TRAP(err, iTestAudioEffect = CTestLocation::NewListenerL(iLogger));
        iObject = EListenerLocation;
		return err;
	}

	if (!aAudioEffectName.Compare(KSourceLocation))
	{
		TRAP(err, iTestAudioEffect = CTestLocation::NewSourceL(iLogger));
        iObject = ESourceLocation;
		return err;
	}

	if (!aAudioEffectName.Compare(KLoudness))
	{
		TRAP(err, iTestAudioEffect = CTestLoudness::NewL(iLogger));
        iObject = ELoudness;
		return err;
	}

	if (!aAudioEffectName.Compare(KListenerOrientation))
	{
		TRAP(err, iTestAudioEffect = CTestOrientation::NewListenerL(iLogger));
		iObject = EListenerOrientation;
		return err;
	}

	if (!aAudioEffectName.Compare(KSourceOrientation))
	{
		TRAP(err, iTestAudioEffect = CTestOrientation::NewSourceL(iLogger));
		iObject = ESourceOrientation;
		return err;
	}

	if (!aAudioEffectName.Compare(KStereoWidening))
	{
		TRAP(err, iTestAudioEffect = CTestStereoWidening::NewL(iLogger));
		iObject = EStereoWidening;
		return err;
	}

	if (!aAudioEffectName.Compare(KRoomLevel))
	{
		TRAP(err, iTestAudioEffect = CTestRoomLevel::NewL(iLogger));
		iObject = ERoomLevel;
		return err;
	}

	return KErrArgument;
}


TInt CAudioEffectTestModule::AssignEffectL(CStifSectionParser *aSection, TTestResult & aResult)
{
	TInt err = KErrNone;

	// read in audio effect
	TPtrC assignEffectPtr;
	if ( aSection->GetLine(KTagAssignEffect, assignEffectPtr, ENoTag) )
	{
		iLogger->Log(_L("Missing tag AssignEffect"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted ;
		return KErrArgument;
	}

	iLogger->Log(_L("Assign Effect to [%S]"), &assignEffectPtr);

	err = Assign(assignEffectPtr);
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


TInt CAudioEffectTestModule::Assign(TPtrC aAssignEffectName)
{
	TInt err = KErrNone;

	if (!aAssignEffectName.Compare(KIndependent))
	{
		err = iTestAudioEffect->CreateEffect();
		if(err == KErrNotSupported)
			{
			// This is because DevSound does not Support creating
			// independent Effects without a Stream Supporting it.
			err = KErrNone;
			}
		return err;
	}

	if (!aAssignEffectName.Compare(KPlayer))
	{
		err = iTestAudioEffect->CreateEffect(iAudioPlayer);
		return err;
	}

	if (!aAssignEffectName.Compare(KRecorder))
	{
		err = iTestAudioEffect->CreateEffect(iAudioRecorder);
		if ((iObject == ERoomLevel) && (err == KErrNotSupported) )
		  	return KErrNone;
		return err;
	}

	if (!aAssignEffectName.Compare(KConverter))
	{
		err = iTestAudioEffect->CreateEffect(iAudioConverter);
		return err;
	}

	if (!aAssignEffectName.Compare(KTone))
	{
		err = iTestAudioEffect->CreateEffect(iAudioTone);
		return err;
	}

	if (!aAssignEffectName.Compare(KDevSound))
	{
		err = iTestAudioEffect->CreateEffect(iDevSound);
		return err;
	}

	if (!aAssignEffectName.Compare(KCustomCommand))
	{
		iCustomCommandUtility = CCustomCommandUtility::NewL(*iAudioPlayer);
	    err = iTestAudioEffect->CreateEffect(iCustomCommandUtility);
	    if (err)
	    	delete iCustomCommandUtility;
		return err;
	}

	if (!aAssignEffectName.Compare(KCustomInterface))
	{
		iCustomCommandUtility = CCustomCommandUtility::NewL(*iAudioPlayer);
		iCustomInterfaceUtility = CCustomInterfaceUtility::NewL(*iCustomCommandUtility);
	    err = iTestAudioEffect->CreateEffect(iCustomInterfaceUtility);

	    if (err)
	    {
	    	delete iCustomInterfaceUtility;
	    	delete iCustomCommandUtility;
	    }

		return err;
	}

	if (!aAssignEffectName.Compare(KInputStream))
	{
		err = iTestAudioEffect->CreateEffect(iAudioInputStream);
		return err;
	}

	if (!aAssignEffectName.Compare(KOutputStream))
	{
		err = iTestAudioEffect->CreateEffect(iAudioOutputStream);
		return err;
	}

	if (!aAssignEffectName.Compare(KMidiClient))
		{
			err = iTestAudioEffect->CreateEffect(iMidiClientUtility);
			return err;
		}

	if (!aAssignEffectName.Compare(KDrmPlayer))
		{
			err = iTestAudioEffect->CreateEffect(iDrmPlayerUtility);
			return err;
		}

	if (!aAssignEffectName.Compare(KVideoPlayer))
		{
			err = iTestAudioEffect->CreateEffect(iVideoPlayerUtility);
			return err;
		}

	return KErrArgument;
}


void CAudioEffectTestModule::DeleteObject()
{
	if (iTestAudioEffect != NULL)
	{
	    switch(iObject)
    	{
			case EEqualizer:
        		delete (CTestAudioEqualizer*)iTestAudioEffect;
				break;
			case EBassBoost:
    	    	delete (CTestBassBoost*)iTestAudioEffect;
				break;
			case EDistanceAttenuation:
    	    	delete (CTestDistanceAttenuation*)iTestAudioEffect;
				break;
			case EListenerDoppler:
			case ESourceDoppler:
        		delete (CTestDoppler*)iTestAudioEffect;
				break;
			case EEnvironmentalReverb:
    	    	delete (CTestEnvironmentalReverb*)iTestAudioEffect;
				break;
			case EListenerLocation:
			case ESourceLocation:
        		delete (CTestLocation*)iTestAudioEffect;
				break;
			case EListenerOrientation:
			case ESourceOrientation:
        		delete (CTestOrientation*)iTestAudioEffect;
				break;
			case ELoudness:
    	    	delete (CTestLoudness*)iTestAudioEffect;
				break;
			case EStereoWidening:
    	    	delete (CTestStereoWidening*)iTestAudioEffect;
				break;
			case ERoomLevel:
    	    	delete (CTestRoomLevel*)iTestAudioEffect;
				break;
    	}
	}

	iTestAudioEffect = NULL;
	iObject = EENone;
}


// ========== Callback Functions ==========
//
void CAudioEffectTestModule::MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds& aDuration)
	{
	iLogger->Log(_L("MapcInit: %d\n"), aStatus);
	TInt d = I64INT(aDuration.Int64());
	RDebug::Print(_L("CConsoleTest::MapcInitComplete :-> Status[%d] Duration[%d]"), aStatus, d);

	if (aStatus != KErrNone)
		{
		iLogger->Log(_L("Cannot open "));
		iLogger->Log(KAMRTestFile);
		iLogger->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
			iLogger->Log(_L("Ready!\n"), aStatus);
		}

	CActiveScheduler::Stop();
	}


void CAudioEffectTestModule::MapcPlayComplete(TInt aErr)
	{
	iLogger->Log(_L("MapcPlay: %d"), aErr);
	RDebug::Print(_L("CConsoleTest::MapcPlayComplete :-> Error[%d]"), aErr);
	}


void CAudioEffectTestModule::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	RDebug::Print(_L("aPreviousState[%d], aCurrentState[%d], aErrorCode[%d]"), aPreviousState, aCurrentState, aErrorCode);
	iLogger->Log(_L("Mosco: %d\n"), aErrorCode);
	CActiveScheduler::Stop();
	}


void CAudioEffectTestModule::MatoPrepareComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MatoPrepareComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MatoPlayComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MatoPlayComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MaiscOpenComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscOpenComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscBufferCopied: %d\n"), aError);
}


void CAudioEffectTestModule::MaiscRecordComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaiscRecordComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MaoscOpenComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscOpenComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscBufferCopied: %d\n"), aError);
}


void CAudioEffectTestModule::MaoscPlayComplete(TInt aError)
{
	RDebug::Print(_L("aError[%d]"), aError);
	iLogger->Log(_L("MaoscPlayComplete: %d\n"), aError);
}


void CAudioEffectTestModule::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState aNewState,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
{
	RDebug::Print(_L("MmcuoStateChanged->MidiState %d"), aNewState);
	iLogger->Log(_L("MmcuoStateChanged: %d\n"), aError);
}


void CAudioEffectTestModule::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
{
	RDebug::Print(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d"), aMicroBeatsPerMinute);
	iLogger->Log(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d\n"), aMicroBeatsPerMinute);
}


void CAudioEffectTestModule::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 aVolumeInDecibels)
{
	RDebug::Print(_L("MmcuoVolumeChanged->Volume %f"), aVolumeInDecibels);
	iLogger->Log(_L("MmcuoVolumeChanged->Volume %f\n"), aVolumeInDecibels);
}


void CAudioEffectTestModule::MmcuoMuteChanged(TInt /*aChannel*/,TBool aMuted)
{
	RDebug::Print(_L("MmcuoMuteChanged->Muted %d"), aMuted);
	iLogger->Log(_L("MmcuoMuteChanged->Muted %d\n"), aMuted);
}


void CAudioEffectTestModule::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 aMicroBeats)
{
	RDebug::Print(_L("MmcuoSyncUpdate->MicroBeats %d"), aMicroBeats);
	iLogger->Log(_L("MmcuoSyncUpdate->MicroBeats %d\n"), aMicroBeats);
}


void CAudioEffectTestModule::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& /*aPosition*/)
{
	RDebug::Print(_L("MmcuoMetaDataEntryFound->EntryId %d"), aMetaDataEntryId);
	iLogger->Log(_L("MmcuoMetaDataEntryFound->EntryId %d\n"), aMetaDataEntryId);
}


void CAudioEffectTestModule::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
{
	RDebug::Print(_L("MmcuoMipMessageReceived"));
	iLogger->Log(_L("MmcuoMipMessageReceived\n"));
}


void CAudioEffectTestModule::MmcuoPolyphonyChanged(TInt aNewPolyphony)
{
	RDebug::Print(_L("MmcuoPolyphonyChanged: %d"), aNewPolyphony);
	iLogger->Log(_L("MmcuoPolyphonyChanged: %d\n"), aNewPolyphony);
}


void CAudioEffectTestModule::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt aInstrumentId)
{
	RDebug::Print(_L("MmcuoInstrumentChanged->InstrumentId %d"), aInstrumentId);
	iLogger->Log(_L("MmcuoInstrumentChanged->InstrumentId %d\n"), aInstrumentId);
}
void CAudioEffectTestModule::MdapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds& aDuration)
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

void CAudioEffectTestModule::MdapcPlayComplete(TInt aErr)
	{
		iLogger->Log(_L("MdapcPlay: %d"), aErr);
		RDebug::Print(_L("CEffectPresetsTest::MdapcPlayComplete :-> Error[%d]"), aErr);
	}

void CAudioEffectTestModule::MvpuoOpenComplete(TInt aError)
	{

	iLogger->Log(_L("MvpuoOpenComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoOpenComplete :-> Error[%d]"), aError);

	if (aError != KErrNone)
		{
		iLogger->Log(_L("Cannot open "));
		_LIT(K3gpTestFile,"e:\\testing\\data\\videoTest.3gp");
		iLogger->Log(K3gpTestFile);
		iLogger->Log(_L("\nPress any key to exit ..."));
		}
	else
		{
			iLogger->Log(_L("Ready!"));
		}

	CActiveScheduler::Stop();
	}

void CAudioEffectTestModule::MvpuoPrepareComplete(TInt aError)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[%d]"), aError);
	}

void CAudioEffectTestModule::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt aError)
	{
	iLogger->Log(_L("MvpuoFrameReady: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoFrameReady :-> Error[%d]"), aError);
	}

void CAudioEffectTestModule::MvpuoPlayComplete(TInt aError)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: %d"), aError);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[%d]"), aError);
	}

void CAudioEffectTestModule::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	iLogger->Log(_L("MvpuoPrepareComplete: ")/*, aEvent*/);
	RDebug::Print(_L("CEffectPresetsTest::MvpuoPrepareComplete :-> Error[]")/*, aEvent*/);
	}
