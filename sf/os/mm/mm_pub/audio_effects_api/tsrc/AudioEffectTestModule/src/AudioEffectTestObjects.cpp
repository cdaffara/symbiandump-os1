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
* Description: Effetcs Test
*
*/


//#include <e32svr.h>
//#include <e32cons.h>

#include "AudioEffectTestObjects.h"

#include <AudioEffectBase.h>
#include <AudioEqualizerBase.h>
#include <BassBoostBase.h>
#include <EnvironmentalReverbBase.h>
#include <DistanceAttenuationBase.h>
#include <ListenerDopplerBase.h>
#include <SourceDopplerBase.h>
#include <ListenerLocationBase.h>
#include <SourceLocationBase.h>
#include <LoudnessBase.h>
#include <ListenerOrientationBase.h>
#include <SourceOrientationBase.h>
#include <StereoWideningBase.h>
#include <RoomLevelBase.h>
#include <EnvironmentalReverbUtility.h>


/*==================================================================================
//  CTestAudioEffect
//
//  This is the base class for Test classes
====================================================================================*/
CTestAudioEffect::CTestAudioEffect(CStifLogger * aLogger)
{
	iLogger = aLogger;
	iAudioEffect = NULL;
	iRegister = EFalse;
	iChange = EFalse;
}

CTestAudioEffect::~CTestAudioEffect()
{
}

// Callback
void CTestAudioEffect::EffectChanged( const CAudioEffect* /*aAudioEffect*/, TUint8 aEvent )
{
	RDebug::Print(_L("CTestAudioEffect::EffectChanged, Event = %d "), aEvent);

	switch (aEvent)
	{
		case KEnabled:				// The effect is enabled.
			iLogger->Log(_L("Effect is enabled.\n"));
			break;
		case KDisabled:				// The effect is disabled
			iLogger->Log(_L("Effect is disabled.\n"));
			break;
		case KEnforced:				// The effect is enforced
			iLogger->Log(_L("Effect is enforced.\n"));
			break;
		case KNotEnforced:			// The effect is not enforced
			iLogger->Log(_L("Effect is not enforced.\n"));
			break;
		case KLostUpdateRights:		// The effect has lost update rights
			iLogger->Log(_L("Effect has lost update rights.\n"));
			break;
		case KGainedUpdateRights:	// The effect has gained update rights
			iLogger->Log(_L("Effect has gained update rights.\n"));
			break;
	}
}

TInt CTestAudioEffect::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;

	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagRegisterEffect))
	{
		return RegisterEffect(aItem, aResult);
	}

	if (!tag.Compare(KTagUnRegisterEffect))
	{
		return UnRegisterEffect(aItem, aResult);
	}

	if (!tag.Compare(KTagApplySettings))
	{
		return ApplySettings(aItem, aResult);
	}

	if (!tag.Compare(KTagEnable))
	{
		return Enable(aItem, aResult);
	}

	if (!tag.Compare(KTagDisable))
	{
		return Disable(aItem, aResult);
	}

	if (!tag.Compare(KTagEnforce))
	{
		return Enforce(aItem, aResult);
	}

	return KErrNone;
}

TInt CTestAudioEffect::RegisterEffect(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	if (!iAudioEffect) 
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));
		return KErrNone;
	}

	TRAPD(err, iAudioEffect->RegisterObserverL(*this));
	aResult.iResult = err ;
	if (err)
	{
		iLogger->Log(_L("Unable to Register Observer."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("Observer Registered Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		iRegister = ETrue;
	}
	return err;
}

TInt CTestAudioEffect::UnRegisterEffect(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	if (!iAudioEffect)   
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));
		return KErrNone;
	}

	iAudioEffect->UnRegisterObserver(*this);
	aResult.iResult = KErrNone ;
	iLogger->Log(_L("Observer Unregistered Successful."));
	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	iRegister = EFalse;
	return KErrNone;
}

TInt CTestAudioEffect::ApplySettings(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	if (!iAudioEffect)   
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));
		return KErrNone;
	}

 	TRAPD(err, iAudioEffect->ApplyL());
	aResult.iResult = err ;
	if (err)
	{
		iLogger->Log(_L("Unable to Apply Settings."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("Apply Settings Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestAudioEffect::Enable(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	if (!iAudioEffect)   
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));
		return KErrNone;
	}

	TRAPD(err, iAudioEffect->EnableL());
	aResult.iResult = err ;
	if (err)
	{
		iLogger->Log(_L("Unable to Enable Effect."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("Enable Effect Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}
	return err;
}

TInt CTestAudioEffect::Disable(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	if (!iAudioEffect)  
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));
		return KErrNone;
	}

	TRAPD(err, iAudioEffect->DisableL());
	aResult.iResult = err ;
	if (err)
	{
		iLogger->Log(_L("Unable to Disable Effect."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("Disable Effect Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}
	return err;
}

TInt CTestAudioEffect::Enforce(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt enforceValue;
	TInt err = KErrNone;

	err = aItem->GetInt(KTagEnforce, enforceValue);
	aResult.iResult = err;
	if (!err)
	{
		if (!iAudioEffect)    
		{
			iLogger->Log(_L("Audio effect is not supported. Skip this step"));
			return KErrNone;
		}

		TRAP(err, iAudioEffect->EnforceL((TBool)enforceValue));
		aResult.iResult = err;
		iLogger->Log(_L("Enforce Value: %d"), enforceValue);
		if (err)
		{
			iLogger->Log(_L("Enforce Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFail());
		}
		else
		{
			iLogger->Log(_L("Enforce Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
	}
	else
	{
		iLogger->Log(_L("Missing Enforce Value."));
		aResult.iResultDes.Copy(KConfigInvalid());
	}

	return err;
}

void CTestAudioEffect::DisplayCommon()
{
	TUid ObjID = iAudioEffect->Uid();

	iLogger->Log(_L("Uid: %d"), ObjID.iUid);
	iLogger->Log(_L("Enabled: %d"), iAudioEffect->IsEnabled());
	iLogger->Log(_L("Enforce: %d"), iAudioEffect->IsEnforced());
	iLogger->Log(_L("Update Rights: %d"), iAudioEffect->HaveUpdateRights());
}

void CTestAudioEffect::PrintError(TInt err)
{
    iLogger->Log(_L("Return Error: %d"), err);
}


/*==================================================================================
//  CTestAudioEqualizer
//
//  This class test the Audio Equalizer
====================================================================================*/
CTestAudioEqualizer* CTestAudioEqualizer::NewLC (CStifLogger * aLogger)
{
	CTestAudioEqualizer* self=new (ELeave) CTestAudioEqualizer(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestAudioEqualizer* CTestAudioEqualizer::NewL (CStifLogger * aLogger)
{
	CTestAudioEqualizer* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestAudioEqualizer::CTestAudioEqualizer(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
	iBandId = 0;
}

void CTestAudioEqualizer::ConstructL()
{
}

CTestAudioEqualizer::~CTestAudioEqualizer()
{
	DeleteAudioEffect();
}

void CTestAudioEqualizer::DeleteAudioEffect()
{
    delete (CAudioEqualizer*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestAudioEqualizer::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	if ((aEvent >= KBand1Changed) && (aEvent <= KBand12Changed))
	{
		iLogger->Log(_L("Band %d of Equalizer has changed\n"), aEvent - KSpecificEffectBase);
	}
	else
	{
	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
	}
}

TInt CTestAudioEqualizer::CreateEffect()
{
	DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL());
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
	DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
	DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aMidiClient));
	PrintError(err);
	return err;
}

TInt CTestAudioEqualizer::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}


TInt CTestAudioEqualizer::CreateEffect(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CAudioEqualizer::NewL(*aUtility));
		PrintError(err);
		return err;
	}

TInt CTestAudioEqualizer::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagBandIdLevel))
	{
		return BandIdLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestAudioEqualizer::BandIdLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt bandLevel;
	CAudioEqualizer* audioEqualizer = (CAudioEqualizer*)iAudioEffect;

	err = aItem->GetInt(KTagBandIdLevel, iBandId);
	if (err)
	{
		iLogger->Log(_L("Missing Band ID."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}

	err = aItem->GetNextInt(bandLevel);
	if (err)
	{
		iLogger->Log(_L("Missing Band Level."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}

	TInt oldBandLevel = audioEqualizer->BandLevel(iBandId);
	TRAP(err, audioEqualizer->SetBandLevelL(iBandId, bandLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Band Number: %d, Level: %d"), iBandId, bandLevel);
	if (err)
	{
		iLogger->Log(_L("Set Band Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (audioEqualizer->BandLevel(iBandId) == bandLevel)
		{
			iLogger->Log(_L("Set Band Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			if (oldBandLevel != bandLevel)
				iChange = ETrue;
		}
		else
		{
			iLogger->Log(_L("Set Band Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}
	return err;
}

TInt CTestAudioEqualizer::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	TInt32 min, max;
	CAudioEqualizer* audioEqualizer = (CAudioEqualizer*)iAudioEffect;

	audioEqualizer->DbLevelLimits(min,max);

	iLogger->Log(_L("EQUALIZER DATA"));
	iLogger->Log(_L("--------------"));
	DisplayCommon();
	iLogger->Log(_L("Number of Bands: %d"), audioEqualizer->NumberOfBands());
	iLogger->Log(_L("Band %d Level: %d"), iBandId, audioEqualizer->BandLevel(iBandId));
	iLogger->Log(_L("Band Width: %d"), audioEqualizer->BandWidth(iBandId));
	iLogger->Log(_L("Db Min/Max: %d/%d"), min, max);
	iLogger->Log(_L("CrossOver Frequency: %d"), audioEqualizer->CrossoverFrequency(iBandId));
	iLogger->Log(_L("Center Frequency: %d"), audioEqualizer->CenterFrequency(iBandId));

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestBassBoost
//
//  This class test the Bass Boost
====================================================================================*/
CTestBassBoost* CTestBassBoost::NewLC (CStifLogger * aLogger)
{
	CTestBassBoost* self=new (ELeave) CTestBassBoost(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestBassBoost* CTestBassBoost::NewL (CStifLogger * aLogger)
{
	CTestBassBoost* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestBassBoost::CTestBassBoost(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestBassBoost::ConstructL()
{
}

CTestBassBoost::~CTestBassBoost()
{
    DeleteAudioEffect();
}

void CTestBassBoost::DeleteAudioEffect()
{
    delete (CBassBoost*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestBassBoost::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KBassBoostChanged:
			iLogger->Log(_L("Bass Boost changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
    	    break;
	}
}

TInt CTestBassBoost::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CBassBoost::NewL(*aMidiClient));
	PrintError(err);
	return err;
}

TInt CTestBassBoost::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CBassBoost::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}

TInt CTestBassBoost::CreateEffect(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CBassBoost::NewL(*aUtility));
		PrintError(err);
		return err;
	}
TInt CTestBassBoost::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagBassBoostLevel))
	{
		return BassBoostLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestBassBoost::BassBoostLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint8 bassBoostLevel;
	//CBassBoost* bassBoost = (CBassBoost*)iAudioEffect;

	err = aItem->GetInt(KTagBassBoostLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Bass Boost Level."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	bassBoostLevel = tempInt;

	//TRAP(err, bassBoost->SetBassBoostLevelL(bassBoostLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Bass Boost Level: %d"), bassBoostLevel);
	if (err)
	{
		iLogger->Log(_L("Set Bass Boost Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		//if (bassBoost->BassBoostLevel() == bassBoostLevel)
		//{
		//	iLogger->Log(_L("Set Bass Boost Level Successful."));
		//	aResult.iResultDes.Copy(KTestCaseResultSuccess());
		//}
		//else
		//{
			iLogger->Log(_L("Set Bass Boost Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		//}
	}
	return err;
}

TInt CTestBassBoost::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	TUint8 bassBoostLevel=0;
	//CBassBoost* bassBoost = (CBassBoost*)iAudioEffect;

	//bassBoostLevel = bassBoost->BassBoostLevel();

	iLogger->Log(_L("BASS BOOST DATA"));
	iLogger->Log(_L("---------------"));
	DisplayCommon();
	iLogger->Log(_L("Bass Boost Level: %d"), bassBoostLevel);

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestDistanceAttenuation
//
//  This class test the Distance Attenuation
====================================================================================*/
CTestDistanceAttenuation* CTestDistanceAttenuation::NewLC (CStifLogger * aLogger)
{
	CTestDistanceAttenuation* self=new (ELeave) CTestDistanceAttenuation(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestDistanceAttenuation* CTestDistanceAttenuation::NewL (CStifLogger * aLogger)
{
	CTestDistanceAttenuation* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestDistanceAttenuation::CTestDistanceAttenuation(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestDistanceAttenuation::ConstructL()
{
}

CTestDistanceAttenuation::~CTestDistanceAttenuation()
{
    DeleteAudioEffect();
}

void CTestDistanceAttenuation::DeleteAudioEffect()
{
    delete (CDistanceAttenuation*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestDistanceAttenuation::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KDistanceAttenuationChanged:
			iLogger->Log(_L("Distance Attenuation changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
    	    break;
	}
}

TInt CTestDistanceAttenuation::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aMidiClient));
	PrintError(err);
	return err;
}

TInt CTestDistanceAttenuation::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}

TInt CTestDistanceAttenuation::CreateEffect(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CDistanceAttenuation::NewL(*aUtility));
		PrintError(err);
		return err;
	}
TInt CTestDistanceAttenuation::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagDistanceAttenuation))
	{
		return DistanceAttenuation(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestDistanceAttenuation::DistanceAttenuation(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TUint tempUint;
	TInt32 rMin, rMax;
	TUint32 rollOffFactor, roomRollOffFactor;
	TBool muteAfterMax=EFalse;
    CDistanceAttenuation* distAtten = (CDistanceAttenuation*) iAudioEffect;

	err = aItem->GetInt(KTagDistanceAttenuation, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Min Distance."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	rMin = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Max Distance."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	rMax = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Mute After Max."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	muteAfterMax = tempInt;

	err = aItem->GetNextInt(tempUint);
	if (err)
	{
		iLogger->Log(_L("Missing Roll Off Factor."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	rollOffFactor = tempUint;

	err = aItem->GetNextInt(tempUint);
	if (err)
	{
		iLogger->Log(_L("Missing Room Roll Off Factor."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	roomRollOffFactor = tempUint;

	TRAP(err, distAtten->SetDistanceAttenuationL(rMin, rMax, muteAfterMax, rollOffFactor, roomRollOffFactor));
	aResult.iResult = err;
	iLogger->Log(_L("rMin: %d, rMax: %d, muteAfterMax: %d, rollOffFactor: %d, roomRollOffFactor: %d"),
	                 rMin, rMax, muteAfterMax, rollOffFactor, roomRollOffFactor);
	if (err)
	{
		iLogger->Log(_L("Set Distance Attenuation Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		TInt32 rMinR, rMaxR;
		TBool  muteAfterMaxR;
		TUint32 rollOffFactorR, roomRollOffFactorR;

		distAtten->DistanceAttenuation(rMinR, rMaxR, muteAfterMaxR, rollOffFactorR, roomRollOffFactorR);
		if ((rMin == rMinR) && (rMax == rMaxR) && (muteAfterMax == muteAfterMaxR) &&
		    (rollOffFactor == rollOffFactorR) && (roomRollOffFactor == roomRollOffFactorR))
		{
			iLogger->Log(_L("Set Distance Attenuation Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Distance Attenuation Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestDistanceAttenuation::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	TInt32 rMin, rMax;
	TBool  muteAfterMax;
	TUint32 rollOffFactor, roomRollOffFactor;
	TUint32 rollOffFactorMax, roomRollOffFactorMax;
    CDistanceAttenuation* distAtten = (CDistanceAttenuation*) iAudioEffect;

	distAtten->DistanceAttenuation( rMin, rMax, muteAfterMax, rollOffFactor,
                                    roomRollOffFactor );
	distAtten->RollOffFactorMax( rollOffFactorMax );
	distAtten->RoomRollOffFactorMax( roomRollOffFactorMax );

	iLogger->Log(_L("DISTANCE ATTENUATION DATA"));
	iLogger->Log(_L("-------------------------"));
	DisplayCommon();
	iLogger->Log(_L("Rmin: %d"), rMin);
	iLogger->Log(_L("RMax: %d"), rMax);
	iLogger->Log(_L("MuteAfterMax: %d"), muteAfterMax);
	iLogger->Log(_L("RollOffFactor: %d"), rollOffFactor);
	iLogger->Log(_L("RoomRollOffFactor: %d"), roomRollOffFactor);
	iLogger->Log(_L("RollOffFactorMax: %d"), rollOffFactorMax);
	iLogger->Log(_L("RoomRollOffFactorMax: %d"), roomRollOffFactorMax);

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestDoppler
//
//  This class test both Listener and Source Doppler
====================================================================================*/
CTestDoppler* CTestDoppler::NewListenerLC (CStifLogger * aLogger)
{
	CTestDoppler* self=new (ELeave) CTestDoppler(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = EListener;
	return self;
}

CTestDoppler* CTestDoppler::NewListenerL (CStifLogger * aLogger)
{
	CTestDoppler* self=NewListenerLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestDoppler* CTestDoppler::NewSourceLC (CStifLogger * aLogger)
{
	CTestDoppler* self=new (ELeave) CTestDoppler(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = ESource;
	return self;
}

CTestDoppler* CTestDoppler::NewSourceL (CStifLogger * aLogger)
{
	CTestDoppler* self=NewSourceLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestDoppler::CTestDoppler(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestDoppler::ConstructL()
{
}

CTestDoppler::~CTestDoppler()
{
    DeleteAudioEffect();
}

void CTestDoppler::DeleteAudioEffect()
{
    if(iType==EListener)
        delete (CListenerDoppler*)iAudioEffect;
    else
        delete (CSourceDoppler*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestDoppler::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KFactorChanged:
			iLogger->Log(_L("Factor changed.\n"));
			break;
	    case KCartesianVelocityChanged:
			iLogger->Log(_L("Cartesian Velocity changed.\n"));
			break;
	    case KSphericalVelocityChanged:
			iLogger->Log(_L("Spherical Velocity changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
	}
}

TInt CTestDoppler::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aPlayer));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aPlayer));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aRecorder, EFalse));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aRecorder, EFalse));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aConverter));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aConverter));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioToneUtility* aTone)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aTone));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aTone));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMMFDevSound* aDevSound)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aDevSound));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aDevSound));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aInputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aInputStream));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aOutputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aOutputStream));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(MCustomInterface* aCustomInterface)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aCustomInterface));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aCustomInterface));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(aCustomCommand));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(aCustomCommand));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CMidiClientUtility* aMidiClient)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aMidiClient));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aMidiClient));
    }
	PrintError(err);
	return err;
}

TInt CTestDoppler::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
		TInt err = KErrNone;
		DeleteAudioEffect();
	    if (iType == EListener)
	        {
	    	TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aDrmPlayer));
	        }
	    else
	        {
	    	TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aDrmPlayer));
	        }
		PrintError(err);
		return err;
	}

TInt CTestDoppler::CreateEffect(CVideoPlayerUtility* aUtility)
	{
		TInt err = KErrNone;
		DeleteAudioEffect();
	    if (iType == EListener)
	        {
	    	TRAP(err, iAudioEffect = CListenerDoppler::NewL(*aUtility));
	        }
	    else
	        {
	    	TRAP(err, iAudioEffect = CSourceDoppler::NewL(*aUtility));
	        }
		PrintError(err);
		return err;
	}

TInt CTestDoppler::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagCartesianVelocity))
	{
		return CartesianVelocity(aItem, aResult);
	}

	if (!tag.Compare(KTagSphericalVelocity))
	{
		return SphericalVelocity(aItem, aResult);
	}

	if (!tag.Compare(KTagFactor))
	{
		return Factor(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestDoppler::CartesianVelocity(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 x, y, z;
	CDoppler* doppler = (CDoppler*) iAudioEffect;

	err = aItem->GetInt(KTagCartesianVelocity, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Cartesian Velocity - X"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	x = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Cartesian Velocity - Y"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	y = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Cartesian Velocity - Z"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	z = tempInt;

	TRAP(err, doppler->SetCartesianVelocityL(x, y, z));
	aResult.iResult = err;
	iLogger->Log(_L("x: %d, y: %d, z: %d"), x, y, z);
	if (err)
	{
		iLogger->Log(_L("Set Cartesian Velocity Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		TInt32 xR, yR, zR;

		doppler->CartesianVelocity(xR, yR, zR);
		if ((x == xR) && (y == yR) && (z == zR))
		{
			iLogger->Log(_L("Set Cartesian Velocity Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Cartesian Velocity Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestDoppler::SphericalVelocity(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 azimuth, elevation, radius;
	CDoppler* doppler = (CDoppler*) iAudioEffect;

	err = aItem->GetInt(KTagSphericalVelocity, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Spherical Velocity - Azimuth"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	azimuth = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Spherical Velocity - Elevation"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	elevation = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Spherical Velocity - Radius"));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	radius = tempInt;

	TRAP(err, doppler->SetSphericalVelocityL(azimuth, elevation, radius));
	aResult.iResult = err;
	iLogger->Log(_L("azimuth: %d, elevation: %d, radius: %d"), azimuth, elevation, radius);
	if (err)
	{
		iLogger->Log(_L("Set Spherical Velocity Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		TInt32 azimuthR, elevationR, radiusR;

		doppler->SphericalVelocity(azimuthR, elevationR, radiusR);
		iLogger->Log(_L("Set Spherical Velocity Successful."));
		iLogger->Log(_L("azimuth: %d, elevation: %d, radius: %d"), azimuthR, elevationR, radiusR);
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestDoppler::Factor(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 factor;
	CDoppler* doppler = (CDoppler*) iAudioEffect;

	err = aItem->GetInt(KTagFactor, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Factor."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	factor = tempInt;

	TRAP(err, doppler->SetFactorL(factor));
	aResult.iResult = err;
	iLogger->Log(_L("Factor: %d"), factor);
	if (err)
	{
		iLogger->Log(_L("Set Factor Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (doppler->Factor() == factor)
		{
			iLogger->Log(_L("Set Factor Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Factor Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestDoppler::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    TInt32 x=0, y=0, z=0, azmuth=0, elev=0, rad=0;
	CDoppler* doppler = (CDoppler*) iAudioEffect;

    doppler->CartesianVelocity(x, y, z);
    doppler->SphericalVelocity(azmuth, elev, rad);

	if (iType == EListener)
	{
		iLogger->Log(_L("LISTENER DOPPLER DATA"));
		iLogger->Log(_L("---------------------"));

	}
	else
	{
		iLogger->Log(_L("SOURCE DOPPLER DATA"));
		iLogger->Log(_L("-------------------"));
	}
	DisplayCommon();
	iLogger->Log(_L("Cartesian Velocity:"));
	iLogger->Log(_L("X= %d, Y= %d, Z= %d"), x, y, z);
	iLogger->Log(_L("Sperical Velocity:"));
	iLogger->Log(_L("Azmuth= %d"), azmuth);
	iLogger->Log(_L("Elevation= %d"), elev);
	iLogger->Log(_L("Radius= %d"), rad);
	iLogger->Log(_L("Factor: %d"), doppler->Factor());
	iLogger->Log(_L("FactorMax: %d"), doppler->FactorMax());

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestEnvironmentalReverb
//
//  This class test the Environmental Reverb
====================================================================================*/
CTestEnvironmentalReverb* CTestEnvironmentalReverb::NewLC (CStifLogger * aLogger)
{
	CTestEnvironmentalReverb* self=new (ELeave) CTestEnvironmentalReverb(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestEnvironmentalReverb* CTestEnvironmentalReverb::NewL (CStifLogger * aLogger)
{
	CTestEnvironmentalReverb* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestEnvironmentalReverb::CTestEnvironmentalReverb(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestEnvironmentalReverb::ConstructL()
{
}

CTestEnvironmentalReverb::~CTestEnvironmentalReverb()
{
    DeleteAudioEffect();
}

void CTestEnvironmentalReverb::DeleteAudioEffect()
{
    delete (CEnvironmentalReverb*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestEnvironmentalReverb::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
	    case KDecayTimeChanged:
			iLogger->Log(_L("Decay Time changed.\n"));
			break;
	    case KDecayHFRatioChanged:
			iLogger->Log(_L("Decay HF Ratio changed.\n"));
			break;
	    case KDensityChanged:
			iLogger->Log(_L("Density changed.\n"));
			break;
	    case KDiffusionChanged:
			iLogger->Log(_L("Diffusion changed.\n"));
			break;
	    case KReflectionsDelayChanged:
			iLogger->Log(_L("Reflections Delay changed.\n"));
			break;
	    case KReflectionsLevelChanged:
			iLogger->Log(_L("Reflections Level changed.\n"));
			break;
	    case KReverbDelayChanged:
			iLogger->Log(_L("Reverb Delay changed.\n"));
			break;
	    case KReverbLevelChanged:
			iLogger->Log(_L("Reverb Level changed.\n"));
			break;
	    case KRoomLevelChanged:
			iLogger->Log(_L("Room Level changed.\n"));
			break;
	    case KRoomHFLevelChanged:
			iLogger->Log(_L("Room HF Level changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
    	    break;
	}
}

TInt CTestEnvironmentalReverb::CreateEffect()
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL());
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestEnvironmentalReverb::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aMidiClient));
	PrintError(err);
	return err;
}


TInt CTestEnvironmentalReverb::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}

TInt CTestEnvironmentalReverb::CreateEffect(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		 iAudioEffect = CEnvironmentalReverb::NewL(*aUtility);
		TRAPD(err, iAudioEffect = CEnvironmentalReverb::NewL(*aUtility));
		PrintError(err);
		return err;
	}

TInt CTestEnvironmentalReverb::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagDecayHFRatio))
	{
		return DecayHFRatio(aItem, aResult);
	}

	if (!tag.Compare(KTagDecayTime))
	{
		return DecayTime(aItem, aResult);
	}

	if (!tag.Compare(KTagDensity))
	{
		return Density(aItem, aResult);
	}

	if (!tag.Compare(KTagDiffusion))
	{
		return Diffusion(aItem, aResult);
	}

	if (!tag.Compare(KTagReflectionsDelay))
	{
		return ReflectionsDelay(aItem, aResult);
	}

	if (!tag.Compare(KTagReflectionsLevel))
	{
		return ReflectionsLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagReverbDelay))
	{
		return ReverbDelay(aItem, aResult);
	}

	if (!tag.Compare(KTagReverbLevel))
	{
		return ReverbLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagRoomHFLevel))
	{
		return RoomHFLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagRoomLevel))
	{
		return RoomLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestEnvironmentalReverb::DecayHFRatio(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 decayHfRatio;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagDecayHFRatio, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Decay HF Ratio value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	decayHfRatio = tempInt;

	TRAP(err, envReverb->SetDecayHFRatioL(decayHfRatio));
	aResult.iResult = err;
	iLogger->Log(_L("Decay HF Ratio: %d"), decayHfRatio);
	if (err)
	{
		iLogger->Log(_L("Set Decay HF Ratio Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->DecayHFRatio() == decayHfRatio)
		{
			iLogger->Log(_L("Set Decay HF Ratio Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Decay HF Ratio Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::DecayTime(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 decayTime;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagDecayTime, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Decay Time value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	decayTime = tempInt;

	TRAP(err, envReverb->SetDecayTimeL(decayTime));
	aResult.iResult = err;
	iLogger->Log(_L("Decay Time: %d"), decayTime);
	if (err)
	{
		iLogger->Log(_L("Set Decay Time Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->DecayTime() == decayTime)
		{
			iLogger->Log(_L("Set Decay Time Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Decay Time Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::Density(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 density;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagDensity, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Density value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	density = tempInt;

	TRAP(err, envReverb->SetDensityL(density));
	aResult.iResult = err;
	iLogger->Log(_L("Density: %d"), density);
	if (err)
	{
		iLogger->Log(_L("Set Density Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->Density() == density)
		{
			iLogger->Log(_L("Set Density Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Density Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::Diffusion(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 diffusion;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagDiffusion, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Diffusion value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	diffusion = tempInt;

	TRAP(err, envReverb->SetDiffusionL(diffusion));
	aResult.iResult = err;
	iLogger->Log(_L("Diffusion: %d"), diffusion);
	if (err)
	{
		iLogger->Log(_L("Set Diffusion Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->Diffusion() == diffusion)
		{
			iLogger->Log(_L("Set Diffusion Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Diffusion Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::ReflectionsDelay(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 reflectionsDelay;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagReflectionsDelay, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Reflections Delay value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	reflectionsDelay = tempInt;

	TRAP(err, envReverb->SetReflectionsDelayL(reflectionsDelay));
	aResult.iResult = err;
	iLogger->Log(_L("Reflections Delay: %d"), reflectionsDelay);
	if (err)
	{
		iLogger->Log(_L("Set Reflections Delay Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->ReflectionsDelay() == reflectionsDelay)
		{
			iLogger->Log(_L("Set Reflections Delay Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Reflections Delay Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::ReflectionsLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 reflectionsLevel;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagReflectionsLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Reflections Level value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	reflectionsLevel = tempInt;

	TRAP(err, envReverb->SetReflectionsLevelL(reflectionsLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Reflections Level: %d"), reflectionsLevel);
	if (err)
	{
		iLogger->Log(_L("Set Reflections Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->ReflectionsLevel() == reflectionsLevel)
		{
			iLogger->Log(_L("Set Reflections Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Reflections Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::ReverbDelay(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint32 reverbDelay;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagReverbDelay, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Reverb Delay value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	reverbDelay = tempInt;

	TRAP(err, envReverb->SetReverbDelayL(reverbDelay));
	aResult.iResult = err;
	iLogger->Log(_L("Reverb Delay: %d"), reverbDelay);
	if (err)
	{
		iLogger->Log(_L("Set Reverb Delay Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->ReverbDelay() == reverbDelay)
		{
			iLogger->Log(_L("Set Reverb Delay Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Reverb Delay Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::ReverbLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 reverbLevel;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagReverbLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Reverb Level value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	reverbLevel = tempInt;

	TRAP(err, envReverb->SetReverbLevelL(reverbLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Reverb Level: %d"), reverbLevel);
	if (err)
	{
		iLogger->Log(_L("Set Reverb Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->ReverbLevel() == reverbLevel)
		{
			iLogger->Log(_L("Set Reverb Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Reverb Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::RoomHFLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 roomHFLevel;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagRoomHFLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Room HF Level value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	roomHFLevel = tempInt;

	TRAP(err, envReverb->SetRoomHFLevelL(roomHFLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Room HF Level: %d"), roomHFLevel);
	if (err)
	{
		iLogger->Log(_L("Set Room HF Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->RoomHFLevel() == roomHFLevel)
		{
			iLogger->Log(_L("Set Room HF Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Room HF Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::RoomLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 roomLevel;
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	err = aItem->GetInt(KTagRoomLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Room Level value."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	roomLevel = tempInt;

	TRAP(err, envReverb->SetRoomLevelL(roomLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Room Level: %d"), roomLevel);
	if (err)
	{
		iLogger->Log(_L("Set Room Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (envReverb->RoomLevel() == roomLevel)
		{
			iLogger->Log(_L("Set Room Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Room Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestEnvironmentalReverb::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    CEnvironmentalReverb* envReverb = (CEnvironmentalReverb*) iAudioEffect;

	iLogger->Log(_L("ENVIRONMENTAL REVERB DATA"));
	iLogger->Log(_L("-------------------------"));
	DisplayCommon();
	iLogger->Log(_L("Decay HF Ratio: %d"), envReverb->DecayHFRatio());
	iLogger->Log(_L("DecayTime: %d"), envReverb->DecayTime());
	iLogger->Log(_L("Density: %d"), envReverb->Density());
	iLogger->Log(_L("Diffusion: %d"), envReverb->Diffusion());
	iLogger->Log(_L("Reflections Delay: %d"), envReverb->ReflectionsDelay());
	iLogger->Log(_L("Reflections Level: %d"), envReverb->ReflectionsLevel());
	iLogger->Log(_L("Reverb Delay: %d"), envReverb->ReverbDelay());
	iLogger->Log(_L("Reverb Level: %d"), envReverb->ReverbLevel());
	iLogger->Log(_L("Room HF Level: %d"), envReverb->RoomHFLevel());
	iLogger->Log(_L("Room Level: %d"), envReverb->RoomLevel());

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestLocation
//
//  This class test both Listener and Source Location
====================================================================================*/
CTestLocation* CTestLocation::NewListenerLC (CStifLogger * aLogger)
{
	CTestLocation* self=new (ELeave) CTestLocation(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = EListener;
	return self;
}

CTestLocation* CTestLocation::NewListenerL (CStifLogger * aLogger)
{
	CTestLocation* self=NewListenerLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestLocation* CTestLocation::NewSourceLC (CStifLogger * aLogger)
{
	CTestLocation* self=new (ELeave) CTestLocation(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = ESource;
	return self;
}

CTestLocation* CTestLocation::NewSourceL (CStifLogger * aLogger)
{
	CTestLocation* self=NewSourceLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestLocation::CTestLocation(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestLocation::ConstructL()
{
}

CTestLocation::~CTestLocation()
{
    DeleteAudioEffect();
}

void CTestLocation::DeleteAudioEffect()
{
    if(iType==EListener)
        delete (CListenerLocation*)iAudioEffect;
    else
        delete (CSourceLocation*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestLocation::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KLocationCartesianChanged:
			iLogger->Log(_L("Location Cartesian changed.\n"));
			break;
	    case KLocationSphericalChanged:
			iLogger->Log(_L("Location Spherical changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
	}
}

TInt CTestLocation::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aPlayer));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aPlayer));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aRecorder, EFalse));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aRecorder, EFalse));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aConverter));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aConverter));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioToneUtility* aTone)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aTone));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aTone));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMMFDevSound* aDevSound)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aDevSound));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aDevSound));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aInputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aInputStream));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aOutputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aOutputStream));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(MCustomInterface* aCustomInterface)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aCustomInterface));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aCustomInterface));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(aCustomCommand));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(aCustomCommand));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CMidiClientUtility* aMidiClient)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aMidiClient));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aMidiClient));
    }
	PrintError(err);
	return err;
}

TInt CTestLocation::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
	    TInt err = KErrNone;

	    DeleteAudioEffect();
	    if (iType == EListener)
	    {
		    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aDrmPlayer));
	    }
	    else
	    {
		    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aDrmPlayer));
	    }
		PrintError(err);
		return err;
	}

TInt CTestLocation::CreateEffect(CVideoPlayerUtility* aUtility)
	{
	    TInt err = KErrNone;

	    DeleteAudioEffect();
	    if (iType == EListener)
	    {
		    TRAP(err, iAudioEffect = CListenerLocation::NewL(*aUtility));
	    }
	    else
	    {
		    TRAP(err, iAudioEffect = CSourceLocation::NewL(*aUtility));
	    }
		PrintError(err);
		return err;
	}

TInt CTestLocation::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagLocationCartesian))
	{
		return LocationCartesian(aItem, aResult);
	}

	if (!tag.Compare(KTagLocationSpherical))
	{
		return LocationSpherical(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestLocation::LocationCartesian(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 x, y, z;
	CLocation* location = (CLocation*) iAudioEffect;

	err = aItem->GetInt(KTagLocationCartesian, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Cartesian - X."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	x = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Cartesian - Y."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	y = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Cartesian - Z."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	z = tempInt;

	TRAP(err, location->SetLocationCartesianL(x, y, z));
	aResult.iResult = err;
	iLogger->Log(_L("X: %d, Y: %d, Z: %d"), x, y, z);
	if (err)
	{
		iLogger->Log(_L("Set Location Cartesian Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		TInt32 xR, yR, zR;

		location->LocationCartesian(xR, yR, zR);
		if ((x == xR) && (y == yR) && (z == zR))
		{
			iLogger->Log(_L("Set Location Cartesian Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Location Cartesian Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestLocation::LocationSpherical(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 azimuth, elevation, radius;
	CLocation* location = (CLocation*) iAudioEffect;

	err = aItem->GetInt(KTagLocationSpherical, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Spherical - Azimuth."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	azimuth = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Spherical - Elevation."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	elevation = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Location Spherical - Radius."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	radius = tempInt;

	TRAP(err, location->SetLocationSphericalL(azimuth, elevation, radius));
	aResult.iResult = err;
	iLogger->Log(_L("azimuth: %d, elevation: %d, radius: %d"), azimuth, elevation, radius);
	if (err)
	{
		iLogger->Log(_L("Set Location Spherical Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		TInt32 azimuthR, elevationR, radiusR;

		location->LocationSpherical(azimuthR, elevationR, radiusR);
		if ((radius == radiusR) && (elevation == elevationR) && (radius == radiusR))
		{
			iLogger->Log(_L("Set Location Spherical Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Location Spherical Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}

	return err;
}

TInt CTestLocation::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    TInt32 x=0, y=0, z=0, azmuth=0, elev=0, rad=0;
	CLocation* location = (CLocation*) iAudioEffect;

    location->LocationCartesian(x, y, z);
    location->LocationSpherical(azmuth, elev, rad);

	if (iType == EListener)
	{
		iLogger->Log(_L("LISTENER LOCATION DATA"));
		iLogger->Log(_L("----------------------"));

	}
	else
	{
		iLogger->Log(_L("SOURCE LOCATION DATA"));
		iLogger->Log(_L("--------------------"));
	}
	DisplayCommon();
	iLogger->Log(_L("Location Cartesian:"));
	iLogger->Log(_L("X= %d, Y= %d, Z= %d"), x, y, z);
	iLogger->Log(_L("Location Sperical:"));
	iLogger->Log(_L("Azmuth= %d"), azmuth);
	iLogger->Log(_L("Elevation= %d"), elev);
	iLogger->Log(_L("Radius= %d"), rad);

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestOrientation
//
//  This class test both Listener and Source Orientation
====================================================================================*/
CTestOrientation* CTestOrientation::NewListenerLC (CStifLogger * aLogger)
{
	CTestOrientation* self=new (ELeave) CTestOrientation(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = EListener;
	return self;
}

CTestOrientation* CTestOrientation::NewListenerL (CStifLogger * aLogger)
{
	CTestOrientation* self=NewListenerLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestOrientation* CTestOrientation::NewSourceLC (CStifLogger * aLogger)
{
	CTestOrientation* self=new (ELeave) CTestOrientation(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iType = ESource;
	return self;
}

CTestOrientation* CTestOrientation::NewSourceL (CStifLogger * aLogger)
{
	CTestOrientation* self=NewSourceLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestOrientation::CTestOrientation(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestOrientation::ConstructL()
{
}

CTestOrientation::~CTestOrientation()
{
    DeleteAudioEffect();
}

void CTestOrientation::DeleteAudioEffect()
{
    if(iType==EListener)
        delete (CListenerOrientation*)iAudioEffect;
    else
        delete (CSourceOrientation*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestOrientation::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KOrientationChanged:
			iLogger->Log(_L("Orientation changed.\n"));
			break;
	    case KOrientationVectorsChanged:
			iLogger->Log(_L("Orientation Vector changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
	}
}

TInt CTestOrientation::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aPlayer));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aPlayer));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aPlayer) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aRecorder, EFalse));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aRecorder, EFalse));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aRecorder) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aConverter));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aConverter));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aConverter) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioToneUtility* aTone)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aTone));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aTone));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aTone) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMMFDevSound* aDevSound)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aDevSound));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aDevSound));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aDevSound) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aInputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aInputStream));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aInputStream) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aOutputStream));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aOutputStream));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aOutputStream) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(MCustomInterface* aCustomInterface)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aCustomInterface));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aCustomInterface));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aCustomInterface) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(aCustomCommand));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(aCustomCommand));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aCustomCommand) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code 
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CMidiClientUtility* aMidiClient)
{
    TInt err = KErrNone;

    DeleteAudioEffect();
    if (iType == EListener)
    {
	    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aMidiClient));
    }
    else
    {
	    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aMidiClient));
	    if (err == KErrNotSupported)
	    {
	      iLogger->Log(_L("CSourceOrientation::NewL(*aMidiClient) return KErrNotSupported\n"));
	    	err = KErrNone;  // Not supported is accepted return code
	    }
    }
	PrintError(err);
	return err;
}

TInt CTestOrientation::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
	    TInt err = KErrNone;

	    DeleteAudioEffect();
	    if (iType == EListener)
	    {
		    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aDrmPlayer));
	    }
	    else
	    {
		    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aDrmPlayer));
	    	if (err == KErrNotSupported)
	    	{
	      	iLogger->Log(_L("CSourceOrientation::NewL(*aDrmPlayer) return KErrNotSupported\n"));
	    		err = KErrNone;  // Not supported is accepted return code 
	    	}
	    }
		PrintError(err);
		return err;
	}

TInt CTestOrientation::CreateEffect(CVideoPlayerUtility* aUtility)
	{
	    TInt err = KErrNone;

	    DeleteAudioEffect();
	    if (iType == EListener)
	    {
		    TRAP(err, iAudioEffect = CListenerOrientation::NewL(*aUtility));
	    }
	    else
	    {
		    TRAP(err, iAudioEffect = CSourceOrientation::NewL(*aUtility));
	    	if (err == KErrNotSupported)
	    	{
	      	iLogger->Log(_L("CSourceOrientation::NewL(*aUtility) return KErrNotSupported\n"));
	    		err = KErrNone;  // Not supported is accepted return code
	    	}
	    }
		PrintError(err);
		return err;
	}

TInt CTestOrientation::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagOrientation))
	{
		return Orientation(aItem, aResult);
	}

	if (!tag.Compare(KTagOrientationVectors))
	{
		return OrientationVectors(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestOrientation::Orientation(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 heading, pitch, roll;
	if (!iAudioEffect)
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));   
		return err;
	}
	COrientation* orientation = (COrientation*) iAudioEffect;

	err = aItem->GetInt(KTagOrientation, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation - Heading."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	heading = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation - Pitch."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	pitch = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation - Roll."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	roll = tempInt;

	TRAP(err, orientation->SetOrientationL(heading, pitch, roll));
	aResult.iResult = err;
	iLogger->Log(_L("Heading: %d, Pitch: %d, Roll: %d"), heading, pitch, roll);
	if (err)
	{
		iLogger->Log(_L("Set Orientation Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		TInt32 headingR, pitchR, rollR;

		orientation->Orientation(headingR, pitchR, rollR);
		iLogger->Log(_L("Set Orientation Successful."));
		iLogger->Log(_L("Heading: %d, Pitch: %d, Roll: %d"), headingR, pitchR, rollR);
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestOrientation::OrientationVectors(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 xFront, yFront, zFront;
	TInt32 xAbove, yAbove, zAbove;

	if (!iAudioEffect)
	{
		iLogger->Log(_L("Audio effect is not supported. Skip this step"));   
		return err;
	}

	COrientation* orientation = (COrientation*) iAudioEffect;

	err = aItem->GetInt(KTagOrientationVectors, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Front X."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	xFront = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Front Y."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	yFront = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Front Z."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	zFront = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Above X."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	xAbove = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Above Y."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	yAbove = tempInt;

	err = aItem->GetNextInt(tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Orientation Vector - Above Z."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	zAbove = tempInt;

	TRAP(err, orientation->SetOrientationVectorsL(xFront, yFront, zFront, xAbove, yAbove, zAbove));
	aResult.iResult = err;
	iLogger->Log(_L("Front X: %d, Front Y: %d, Front Z: %d"), xFront, yFront, zFront);
	iLogger->Log(_L("Above X: %d, Above Y: %d, Above Z: %d"), xAbove, yAbove, zAbove);
	if (err)
	{
		iLogger->Log(_L("Set Orientation Vectors Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		TInt32 xFrontR, yFrontR, zFrontR;
		TInt32 xAboveR, yAboveR, zAboveR;

		orientation->OrientationVectors(xFrontR, yFrontR, zFrontR, xAboveR, yAboveR, zAboveR);
		iLogger->Log(_L("Set Orientation Vectors Successful."));
		iLogger->Log(_L("Front X: %d, Front Y: %d, Front Z: %d"), xFrontR, yFrontR, zFrontR);
		iLogger->Log(_L("Above X: %d, Above Y: %d, Above Z: %d"), xAboveR, yAboveR, zAboveR);
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestOrientation::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    TInt32 x, y, z, x2, y2, z2, heading, pitch, roll;
    if (!iAudioEffect)
		{
			iLogger->Log(_L("Audio effect is not supported. Skip this step")); 
			return KErrNone;
		}

		COrientation* orientation = (COrientation*) iAudioEffect;

    orientation->Orientation(heading, pitch, roll);
    orientation->OrientationVectors(x, y, z, x2, y2, z2);

	if (iType == EListener)
	{
		iLogger->Log(_L("LISTENER ORIENTATION DATA"));
		iLogger->Log(_L("-------------------------"));

	}
	else
	{
		iLogger->Log(_L("SOURCE ORIENTATION DATA"));
		iLogger->Log(_L("-----------------------"));
	}
	DisplayCommon();
	iLogger->Log(_L("Orientation:"));
	iLogger->Log(_L("Heading= %d, Pitch= %d, Roll= %d"), heading, pitch, roll);
	iLogger->Log(_L("Orientation Vectors:"));
	iLogger->Log(_L("Front Vector X= %d, Y=%d, Z=%d"), x, y, z);
	iLogger->Log(_L("Above Vector X= %d, Y=%d, Z=%d"), x2, y2, z2);

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestLoudness
//
//  This class test the Loudness
====================================================================================*/
CTestLoudness* CTestLoudness::NewLC (CStifLogger * aLogger)
{
	CTestLoudness* self=new (ELeave) CTestLoudness(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestLoudness* CTestLoudness::NewL (CStifLogger * aLogger)
{
	CTestLoudness* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestLoudness::CTestLoudness(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestLoudness::ConstructL()
{
}

CTestLoudness::~CTestLoudness()
{
    DeleteAudioEffect();
}

void CTestLoudness::DeleteAudioEffect()
{
    delete (CLoudness*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestLoudness::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
}

TInt CTestLoudness::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CLoudness::NewL(*aMidiClient));
	PrintError(err);
	return err;
}

TInt CTestLoudness::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
	    DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CLoudness::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}

TInt CTestLoudness::CreateEffect(CVideoPlayerUtility* aUtility)
	{
	    DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CLoudness::NewL(*aUtility));
		PrintError(err);
		return err;
	}

TInt CTestLoudness::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestLoudness::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
	iLogger->Log(_L("LOUDNESS DATA"));
	iLogger->Log(_L("-------------"));
	DisplayCommon();

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestStereoWidening
//
//  This class test the StereoWidening
====================================================================================*/
CTestStereoWidening* CTestStereoWidening::NewLC (CStifLogger * aLogger)
{
	CTestStereoWidening* self=new (ELeave) CTestStereoWidening(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestStereoWidening* CTestStereoWidening::NewL (CStifLogger * aLogger)
{
	CTestStereoWidening* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestStereoWidening::CTestStereoWidening(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestStereoWidening::ConstructL()
{
}

CTestStereoWidening::~CTestStereoWidening()
{
    DeleteAudioEffect();
}

void CTestStereoWidening::DeleteAudioEffect()
{
    delete (CStereoWidening*)iAudioEffect;
    iAudioEffect = NULL;
}

// Callback
void CTestStereoWidening::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KStereoWideningLevelChanged:
			iLogger->Log(_L("Stereo Widening changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
    	    break;
	}
}

TInt CTestStereoWidening::CreateEffect()
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL());
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aPlayer));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aRecorder, EFalse));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aConverter));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aTone));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aDevSound));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aInputStream));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aOutputStream));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aCustomInterface));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(aCustomCommand));
	PrintError(err);
	return err;
}

TInt CTestStereoWidening::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aMidiClient));
	PrintError(err);
	return err;
}
TInt CTestStereoWidening::CreateEffect(CVideoPlayerUtility* aUtility)
	{
	    DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aUtility));
		PrintError(err);
		return err;
	}


TInt CTestStereoWidening::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
	    DeleteAudioEffect();
		TRAPD(err, iAudioEffect = CStereoWidening::NewL(*aDrmPlayer));
		PrintError(err);
		return err;
	}

TInt CTestStereoWidening::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagStereoWideningLevel))
	{
		return StereoWideningLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestStereoWidening::StereoWideningLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint tempInt;
	TUint8 stereoWideningLevel;
    CStereoWidening* stereoWidening = (CStereoWidening*) iAudioEffect;
    stereoWidening->operator new(5);
	err = aItem->GetInt(KTagStereoWideningLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Stereo Widening Level."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	stereoWideningLevel = tempInt;

	TRAP(err, stereoWidening->SetStereoWideningLevelL(stereoWideningLevel));
	aResult.iResult = err;
	iLogger->Log(_L("Stereo Widening Level: %d"), stereoWideningLevel);
	if (err)
	{
		iLogger->Log(_L("Set Stereo Widening Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (stereoWidening->StereoWideningLevel() == stereoWideningLevel)
		{
			iLogger->Log(_L("Set Stereo Widening Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Stereo Widening Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}
	return err;
}

TInt CTestStereoWidening::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    CStereoWidening* stereoWidening = (CStereoWidening*) iAudioEffect;

	iLogger->Log(_L("STEREO WIDENING DATA"));
	iLogger->Log(_L("--------------------"));
	DisplayCommon();
	iLogger->Log(_L("Stereo Widening Level: %d"), stereoWidening->StereoWideningLevel());

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}


/*==================================================================================
//  CTestRoomLevel
//
//  This class test the Room Level Effect
====================================================================================*/
CTestRoomLevel* CTestRoomLevel::NewLC (CStifLogger * aLogger)
{
	CTestRoomLevel* self=new (ELeave) CTestRoomLevel(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestRoomLevel* CTestRoomLevel::NewL (CStifLogger * aLogger)
{
	CTestRoomLevel* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestRoomLevel::CTestRoomLevel(CStifLogger * aLogger): CTestAudioEffect(aLogger)
{
}

void CTestRoomLevel::ConstructL()
{
	//iMidiClientUtility_R = CMidiClientUtility::NewL(*this);

	iEnvironmentalReverbUtility_B = NULL;
}

CTestRoomLevel::~CTestRoomLevel()
{
    DeleteAudioEffect();
}

void CTestRoomLevel::DeleteAudioEffect()
{
    if (iAudioEffect)
    	{
    	delete (CRoomLevel*)iAudioEffect;
    	iAudioEffect = NULL;
    	}

    if (iEnvironmentalReverbUtility_B)
    	{
    	delete iEnvironmentalReverbUtility_B;
    	}
}

// ========== Callback Functions ==========
void CTestRoomLevel::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
{
	switch (aEvent)
	{
		case KRoomLevelChanged:
			iLogger->Log(_L("Room Level changed.\n"));
			break;
		default:
    	    CTestAudioEffect::EffectChanged(aAudioEffect, aEvent);
    	    break;
	}
}

void CTestRoomLevel::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState aNewState,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
{
	RDebug::Print(_L("MmcuoStateChanged->MidiState %d"), aNewState);
	iLogger->Log(_L("MmcuoStateChanged: %d\n"), aError);
}


void CTestRoomLevel::MmcuoTempoChanged(TInt aMicroBeatsPerMinute)
{
	RDebug::Print(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d"), aMicroBeatsPerMinute);
	iLogger->Log(_L("MmcuoTempoChanged->MicroBeatsPerMinute %d\n"), aMicroBeatsPerMinute);
}


void CTestRoomLevel::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 aVolumeInDecibels)
{
	RDebug::Print(_L("MmcuoVolumeChanged->Volume %f"), aVolumeInDecibels);
	iLogger->Log(_L("MmcuoVolumeChanged->Volume %f\n"), aVolumeInDecibels);
}


void CTestRoomLevel::MmcuoMuteChanged(TInt /*aChannel*/,TBool aMuted)
{
	RDebug::Print(_L("MmcuoMuteChanged->Muted %d"), aMuted);
	iLogger->Log(_L("MmcuoMuteChanged->Muted %d\n"), aMuted);
}


void CTestRoomLevel::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 aMicroBeats)
{
	RDebug::Print(_L("MmcuoSyncUpdate->MicroBeats %d"), aMicroBeats);
	iLogger->Log(_L("MmcuoSyncUpdate->MicroBeats %d\n"), aMicroBeats);
}


void CTestRoomLevel::MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& /*aPosition*/)
{
	RDebug::Print(_L("MmcuoMetaDataEntryFound->EntryId %d"), aMetaDataEntryId);
	iLogger->Log(_L("MmcuoMetaDataEntryFound->EntryId %d\n"), aMetaDataEntryId);
}


void CTestRoomLevel::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aMessage*/)
{
	RDebug::Print(_L("MmcuoMipMessageReceived"));
	iLogger->Log(_L("MmcuoMipMessageReceived\n"));
}


void CTestRoomLevel::MmcuoPolyphonyChanged(TInt aNewPolyphony)
{
	RDebug::Print(_L("MmcuoPolyphonyChanged: %d"), aNewPolyphony);
	iLogger->Log(_L("MmcuoPolyphonyChanged: %d\n"), aNewPolyphony);
}


void CTestRoomLevel::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt aInstrumentId)
{
	RDebug::Print(_L("MmcuoInstrumentChanged->InstrumentId %d"), aInstrumentId);
	iLogger->Log(_L("MmcuoInstrumentChanged->InstrumentId %d\n"), aInstrumentId);
}

//=============================================

TInt CTestRoomLevel::CreateEffect()
{
	TInt err = KErrNotSupported;
    DeleteAudioEffect();
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioPlayerUtility* aPlayer)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aPlayer));
	if (!err)
	{
		CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aPlayer, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioRecorderUtility* aRecorder)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aRecorder));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aRecorder, EFalse, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aConverter));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aConverter, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aTone));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aTone, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aDevSound));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aDevSound, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aInputStream));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aInputStream, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aOutputStream));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aOutputStream, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aCustomInterface));
	RDebug::Print(_L("err: %d"), err);
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aCustomInterface, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CCustomCommandUtility* aCustomCommand)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(aCustomCommand));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(aCustomCommand, *environmentalReverb));
	}
	PrintError(err);
	return err;
}

TInt CTestRoomLevel::CreateEffect(CMidiClientUtility* aMidiClient)
{
    DeleteAudioEffect();
	TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aMidiClient));
	if (!err)
	{
	  	CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
		TRAP(err, iAudioEffect = CRoomLevel::NewL(*aMidiClient, *environmentalReverb));
	}
	PrintError(err);
	return err;
}


TInt CTestRoomLevel::CreateEffect(CDrmPlayerUtility* aDrmPlayer)
	{
	    DeleteAudioEffect();
		TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aDrmPlayer));
		if (!err)
		{
			CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
			TRAP(err, iAudioEffect = CRoomLevel::NewL(*aDrmPlayer, *environmentalReverb));
		}
		PrintError(err);
		return err;
	}


TInt CTestRoomLevel::CreateEffect(CVideoPlayerUtility* aUtility)
	{
	    DeleteAudioEffect();
		iLogger->Log(_L("CTestRoomLevel::CreateEffect(CVideoPlayerUtility* aUtility)"));
		TRAPD(err, iEnvironmentalReverbUtility_B = CEnvironmentalReverbUtility::NewL(*aUtility));
		if (!err)
		{
			CEnvironmentalReverb* environmentalReverb = &(iEnvironmentalReverbUtility_B->EnvironmentalReverb());
			TRAP(err, iAudioEffect = CRoomLevel::NewL(*aUtility, *environmentalReverb));
		}
		PrintError(err);
		return err;
	}

TInt CTestRoomLevel::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagRoomLevel))
	{
		return RoomLevel(aItem, aResult);
	}

	if (!tag.Compare(KTagDisplayData))
	{
		return DisplayData(aItem, aResult);
	}

	return CTestAudioEffect::ProcessTag(aItem, aResult);
}

TInt CTestRoomLevel::RoomLevel(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt tempInt;
	TInt32 level;
    CRoomLevel* roomLevel = (CRoomLevel*) iAudioEffect;

	err = aItem->GetInt(KTagRoomLevel, tempInt);
	if (err)
	{
		iLogger->Log(_L("Missing Room Level."));
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = err ;
		return err;
	}
	level = tempInt;

	TRAP(err, roomLevel->SetRoomLevelL(level));
	aResult.iResult = err;
	iLogger->Log(_L("Room Level: %d"), level);
	if (err)
	{
		iLogger->Log(_L("Set Room Level Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		// verify
		if (roomLevel->Level() == level)
		{
			iLogger->Log(_L("Set Room Level Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		else
		{
			iLogger->Log(_L("Set Room Level Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			aResult.iResult = KErrExpectedValueDifferent;
			err = KErrExpectedValueDifferent;
		}
	}
	return err;
}

TInt CTestRoomLevel::DisplayData(CStifItemParser * /*aItem*/, TTestResult & aResult)
{
    CRoomLevel* roomLevel = (CRoomLevel*) iAudioEffect;

	iLogger->Log(_L("Room Level DATA"));
	iLogger->Log(_L("---------------"));
	DisplayCommon();
	iLogger->Log(_L("Level: %d"), roomLevel->Level());

	aResult.iResultDes.Copy(KTestCaseResultSuccess());
	aResult.iResult = KErrNone;
	return KErrNone;
}

// end file
