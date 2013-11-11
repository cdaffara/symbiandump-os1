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


#include <e32svr.h>
#include <e32cons.h>

#include "EffectPresetTestCore.h"

#include <AudioEffectBase.h>
#include <AudioEqualizerBase.h>
#include <EnvironmentalReverbBase.h>
#include <StereoWideningBase.h>
/*==================================================================================
//  CTestAudioEffect
//
//  This is the base class for Test classes
====================================================================================*/
CTestAudioEffectPreset::CTestAudioEffectPreset(CStifLogger * aLogger)
{
	iLogger = aLogger;
//	iAudioEffectPreset = NULL;
}

CTestAudioEffectPreset::~CTestAudioEffectPreset()
{
}



void CTestAudioEffectPreset::PrintError(TInt err)
{
    iLogger->Log(_L("Return Error: %d"), err);
}


void CTestAudioEffectPreset::CreateNextPresetName(TInt aNextPresetNumber)
{
	TBuf<5> num;
	num.Format(_L("%3d"), aNextPresetNumber);
	// Convert to an 8-bit string for output to Logfile...
	iPresetName.Copy(_L("Preset"));
	iPresetName.Append(num);

	iLogger->Log(_L("CreateNextPresetName = %S"), &iPresetName );

}

TInt CTestAudioEffectPreset::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	// Only invalid tag was met, this function get called.

	TInt err = KErrConfigInvalid;
	TPtrC tag;

	aItem->GetString(_L(""), tag);
	iLogger->Log(_L("unsupported tag = %S"), &tag);

	aResult.iResult = KErrConfigInvalid;
	aResult.iResultDes.Copy(KTestCaseInvalidTag());

	return err;
}



/*==================================================================================
//  CTestAudioEqualizerUtility
====================================================================================*/

TInt CTestAudioEqualizerUtility::CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;

	TPtrC aPresetName;
	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();

	err = aItem->GetString(KTagCreatePreset, aPresetName);

	if(err)
	{
		iLogger->Log(_L("Missing Preset name. use default"));
		CreateNextPresetName(presetData.Count() + 1);
		aPresetName.Set(iPresetName);
	}

	TRAP(err, iAudioEqualizerUtility->CreatePresetL(aPresetName, *iAudioEqualizer));

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ApplyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestAudioEqualizerUtility::ApplyPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();

	err = aItem->GetInt(KTagApplyPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	TRAP(err, iAudioEqualizerUtility->ApplyPresetL(aPresetIndex));

	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ApplyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}
	return err;
}


TInt CTestAudioEqualizerUtility::ModifyPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TPtrC aPresetName;

	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();

	err = aItem->GetInt(KTagModifyPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;
	}

	err = aItem->GetNextString(KTagModifyPreset, aPresetName);

	if(err)
	{
		iLogger->Log(_L("Missing Preset name. use default"));
		CreateNextPresetName(presetData.Count() + 1);
		aPresetName.Set(iPresetName);
	}

	TInt32 oldLevel = iAudioEqualizer->BandLevel(1);
	TRAP(err, iAudioEqualizer->SetBandLevelL(1, oldLevel + 1 ));

	if (err)
	{
		iLogger->Log(_L("SetBandLevelL Failed."));
	}
	else
	{
		TRAP(err, iAudioEqualizerUtility->ModifyPresetL(aPresetIndex,aPresetName,*iAudioEqualizer));
	}
	
	if ( (aPresetIndex > 500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ModifyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ModifyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestAudioEqualizerUtility::ResetPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;

	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();

	err = aItem->GetInt(KTagResetPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	TRAP(err, iAudioEqualizerUtility->ResetPresetL(aPresetIndex));
	
	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ResetPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ResetPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestAudioEqualizerUtility::DeletePreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TUint32 aPredefinedPreset = 0;
	TPtrC aPresetName;

	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();
	TInt count = presetData.Count();

	TRAP(err, aPredefinedPreset=iAudioEqualizerUtility->NumberOfPreDefinedPresets());
	
	iLogger->Log(_L("presetData.Count() = %d"), count);
	iLogger->Log(_L("NumberOfPreDefinedPresets() = %d"), aPredefinedPreset);
	
	if (err or (aPredefinedPreset==count))											
		{																			
		iLogger->Log(_L("DeletePresetL Failed.(Create a Preset before)"));			
		aResult.iResultDes.Copy(KTestCaseResultFail());								
		return err;																	
		}																			
	
	err = aItem->GetInt(KTagDeletePreset, aPresetIndex);

	if(/*count < aPresetIndex ||*/ err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;
	}

	TRAP(err, iAudioEqualizerUtility->DeletePresetL(aPresetIndex));
	
	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("DeletePresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("DeletePresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestAudioEqualizerUtility::GetPreset(CStifItemParser *aItem,TTestResult & aResult)
	{
		TInt err=KErrNone;
		TInt aPresetIndex;
		TBuf16<256> aPreset;

		TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();
		err = aItem->GetInt(KTagGetPreset, aPresetIndex);
		if (err)
			{
			iLogger->Log(_L("Missing Preset index. use default"));
			aPresetIndex = presetData.Count() - 1;
			}

		if (aPresetIndex<0)
			{
			iLogger->Log(_L("Missing Preset index. Can not GetPreset."));
			}
		else
			{
			TRAP(err, aPreset.Copy(iAudioEqualizerUtility->GetPresetL(aPresetIndex)));
			}
			
		if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
		{
			iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
			err = KErrNone;
		}
		
		aResult.iResult = err;
		if (err)
		{
			iLogger->Log(_L("GetPresetL Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFail());
		}
		else
		{
			RDebug::Print(_L("GetPresetL: %S"),&aPreset);
			iLogger->Log(_L("GetPresetL Successful."));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		return err;
	}

TUint32 CTestAudioEqualizerUtility::NumberOfPreDefinePresets(TTestResult & aResult)
	{
		TInt err=KErrNone;
		TUint32 aPredefinedPreset = 0;

		TRAP(err, aPredefinedPreset=iAudioEqualizerUtility->NumberOfPreDefinedPresets());
		aResult.iResult = err;
		if (err)
		{
			iLogger->Log(_L("NumberOfPreDefinePresets Failed."));
			aResult.iResultDes.Copy(KTestCaseResultFail());
		}
		else
		{
			RDebug::Print(_L("NumberOfPreDefinePresets: %d"),aPredefinedPreset);
			iLogger->Log(_L("NumberOfPreDefinePresets Successful."),aPredefinedPreset);
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
		return err;
	}

TInt CTestAudioEqualizerUtility::DisableEffect(CStifItemParser  */*aItem*/, TTestResult & aResult)
{
	TInt err=KErrNone;

	TRAP(err, iAudioEqualizerUtility->DisableEqualizerL());

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("DisableEqualizerL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("DisableEqualizerL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}

TInt CTestAudioEqualizerUtility::BassBoostTest(CStifItemParser  */*aItem*/, TTestResult & aResult)
{
	TInt err=KErrNone;

	CBassBoost* bassBoost = NULL;
	//iBassBoost = &iAudioEqualizerUtility->BassBoost();
	TRAP(err, bassBoost = &iAudioEqualizerUtility->BassBoost());

	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("iAudioEqualizerUtility->BassBoost Failed. [%d]"),err);
		RDebug::Print(_L("iAudioEqualizerUtility->BassBoost Failed. [%d]"),err);
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("iAudioEqualizerUtility->BassBoost Ok. [%d]"),err);
		RDebug::Print(_L("iAudioEqualizerUtility->BassBoost Ok. [%d]"),err);
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}

	return err;
}

CTestAudioEqualizerUtility* CTestAudioEqualizerUtility::NewLC (CStifLogger * aLogger)
{
	CTestAudioEqualizerUtility* self=new (ELeave) CTestAudioEqualizerUtility(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTestAudioEqualizerUtility* CTestAudioEqualizerUtility::NewL (CStifLogger * aLogger)
{
	CTestAudioEqualizerUtility* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
}

CTestAudioEqualizerUtility::CTestAudioEqualizerUtility(CStifLogger * aLogger): CTestAudioEffectPreset(aLogger)
{
}

void CTestAudioEqualizerUtility::ConstructL()
{
}

CTestAudioEqualizerUtility::~CTestAudioEqualizerUtility()
{
    DeleteAudioEffect();
}

void CTestAudioEqualizerUtility::DeleteAudioEffect()
{

	if(iAudioEqualizerUtility)
    	delete iAudioEqualizerUtility;
    iAudioEqualizerUtility = NULL;

}


TBool CTestAudioEqualizerUtility::AssignEffect()
{
	iAudioEqualizer =  &(iAudioEqualizerUtility->Equalizer());
	return (iAudioEqualizer != NULL);
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioPlayerUtility* aPlayer)
{
	DeleteAudioEffect();
	if (aPlayer)
	{
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aPlayer));
		if ( iError == KErrNone)
		{
			iLogger->Log(_L("CAudioEqualizerUtility::NewL(*aPlayer)) successful"));
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
		else
		{
			iLogger->Log(_L("CAudioEqualizerUtility::NewL(*aPlayer)) fail - %d") , iError);
		}
	}
	else
	{
		iLogger->Log(_L("Create failed: Parameter player is NULL!"));
		iError = KErrBadHandle;
	}
	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioRecorderUtility* aRecorder)
{
	DeleteAudioEffect();
	if (aRecorder)
	{
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aRecorder));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
	}
	else
	{
		iLogger->Log(_L("Create failed: Parameter Recorder is NULL!"));
		iError = KErrBadHandle;
	}
	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioConvertUtility* aConverter)
{
    DeleteAudioEffect();
    if (aConverter)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aConverter));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter Converter is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioToneUtility* aTone)
{
    DeleteAudioEffect();
    if (aTone)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aTone));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter Tone object is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMMFDevSound* aDevSound)
{
    DeleteAudioEffect();
    if (aDevSound)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aDevSound));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter DevSound is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}


TInt CTestAudioEqualizerUtility::AssignObject(CCustomCommandUtility* aCustomCommand)
{
    if (aCustomCommand)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(aCustomCommand));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter CustomCommand is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(MCustomInterface* aCustomInterface)
{
    DeleteAudioEffect();
    if (aCustomInterface)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aCustomInterface));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter CustomInterface is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
   	if (aInputStream)
   	{
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aInputStream));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
   	}
	else
	{
		iLogger->Log(_L("Create failed: Parameter InputStream is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
    if (aOutputStream)
    {
		TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aOutputStream));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter OutputStream is NULL!"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestAudioEqualizerUtility::AssignObject(CMidiClientUtility* aMidiClient)
	{
	    DeleteAudioEffect();
	    if (aMidiClient)
	    {
			TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aMidiClient));
			if ( iError == KErrNone)
			{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
			}
	    }
		else
		{
			iLogger->Log(_L("Create failed: Parameter MidiClient is NULL!"));
			iError = KErrBadHandle;
		}
		return iError;
	}

TInt CTestAudioEqualizerUtility::AssignObject(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		if (aDrmPlayer)
			{
			TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aDrmPlayer));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter DrmPlayer is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}

TInt CTestAudioEqualizerUtility::AssignObject(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		if (aUtility)
			{
			TRAP(iError, iAudioEqualizerUtility = CAudioEqualizerUtility::NewL(*aUtility));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter Utility is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}

TInt CTestAudioEqualizerUtility::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagCreatePreset))
	{
		return CreateNewPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagApplyPreset))
	{
		return ApplyPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagModifyPreset))
	{
		return ModifyPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagResetPreset))
	{
		return ResetPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagDeletePreset))
	{
		return DeletePreset(aItem, aResult);
	}

	if (!tag.Compare(KTagGetPreset))
		{
			return GetPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagNumberOfPreDefinePresets))
		{
			return NumberOfPreDefinePresets(aResult);
		}

	if (!tag.Compare(KTagDisable))
	{
		return DisableEffect(aItem, aResult);
	}
	if (!tag.Compare(KTagBassBoostTest))
	{
		return BassBoostTest(aItem, aResult);
	}

	return CTestAudioEffectPreset::ProcessTag(aItem, aResult);
}

void CTestAudioEqualizerUtility::PrintPresetNamesL()
	{
	// Print preset
	TArray<TEfAudioEqualizerUtilityPreset> presetData = iAudioEqualizerUtility->Presets();
	iPresetCount = presetData.Count();

	for(TInt i = 0; i < iPresetCount; i++)
	{
			iLogger->Log(KAudioIndex,i+1);
			iLogger->Log(iAudioEqualizerUtility->GetPresetL(i));
			iLogger->Log(KAudioNewLine);
	}
}

/*==================================================================================
//  CTestEnvironmentalReverbUtility
====================================================================================*/
TInt CTestEnvironmentalReverbUtility::CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;

	TPtrC aPresetName;
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetString(KTagCreatePreset, aPresetName);

	if(err)
	{
		iLogger->Log(_L("Missing Preset name. use default"));
		CreateNextPresetName(presetData.Count() + 1);
		aPresetName.Set(iPresetName);
	}

	TRAP(err, iEnvironmentalReverbUtility->CreatePresetL(aPresetName, *iEnvironmentalReverb));

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ApplyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}
TInt CTestEnvironmentalReverbUtility::GetPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetInt(KTagGetPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	if (aPresetIndex < 0)
	{
		iLogger->Log(_L("No preset defined to get"));
		err = KErrNotReady;
	}
	else
	{
		TRAP(err, iEnvironmentalReverbUtility->GetPresetL(aPresetIndex));
	}
	
	if ( (aPresetIndex >500 ) && (err == KErrArgument))  
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("GetPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("GetPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}
	return err;
}
TInt CTestEnvironmentalReverbUtility::NumberOfPreDefinedPresets(TTestResult & aResult)
{
	TInt err=KErrNone;
	TUint32 aPredefinedPresets = 0;

	TRAP(err, aPredefinedPresets = iEnvironmentalReverbUtility->NumberOfPreDefinedPresets());

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("NumberOfPreDefinedPresets Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("NumberOfPreDefinedPresets Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		RDebug::Print(_L("NumberOfPreDefinedPresets:-> [%d]"), aPredefinedPresets);
	}
	return err;
}

TInt CTestEnvironmentalReverbUtility::ApplyPreset(CStifItemParser *aItem, TTestResult & aResult)
	{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetInt(KTagApplyPreset, aPresetIndex);

	if(err)
		{
		iLogger->Log(_L("Missing Preset index. use default\n"));
		aPresetIndex = presetData.Count() - 1;

		}

	TRAP(err, iEnvironmentalReverbUtility->ApplyPresetL(aPresetIndex));
	
	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}

	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("ApplyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
	return err;
	}


TInt CTestEnvironmentalReverbUtility::ModifyPreset(CStifItemParser *aItem, TTestResult & aResult)
	{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TPtrC aPresetName;

	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetInt(KTagModifyPreset, aPresetIndex);

	if(err)
		{
		iLogger->Log(_L("Missing Preset index. use default\n"));
		aPresetIndex = presetData.Count() - 1;
		}

	err = aItem->GetNextString(KTagModifyPreset, aPresetName);

	if(err)
		{
		iLogger->Log(_L("Missing Preset name. use default\n"));
		CreateNextPresetName(presetData.Count() + 1);
		aPresetName.Set(iPresetName);
		}

	TUint32 uMin, uMax;
	iEnvironmentalReverb->DecayHFRatioRange(uMin, uMax);
	TRAP(err, iEnvironmentalReverb->SetDecayHFRatioL((uMin+uMax)/2));

	if (err)
		{
		iLogger->Log(_L("SetDecayHFRatioL Failed."));
		}
	else
		{
		TRAP(err, iEnvironmentalReverbUtility->ModifyPresetL(aPresetIndex,aPresetName,*iEnvironmentalReverb));
		}

  if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("ModifyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("ModifyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}
	return err;
	}

TInt CTestEnvironmentalReverbUtility::ResetPreset(CStifItemParser *aItem, TTestResult & aResult)
	{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetInt(KTagResetPreset, aPresetIndex);

	if(err)
		{
		iLogger->Log(_L("Missing Preset index. use default\n"));
		aPresetIndex = presetData.Count() - 1;

		}

	TRAP(err, iEnvironmentalReverbUtility->ResetPresetL(aPresetIndex));

	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("ResetPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("ResetPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}

	return err;
	}


TInt CTestEnvironmentalReverbUtility::DeletePreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();

	err = aItem->GetInt(KTagDeletePreset, aPresetIndex);

	if(err)
		{
		iLogger->Log(_L("Missing Preset index. use default\n"));
		aPresetIndex = presetData.Count() - 1;

		}

	TRAP(err, iEnvironmentalReverbUtility->DeletePresetL(aPresetIndex));
	
	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	

	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("DeletePresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("DeletePresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}

	return err;
}

TInt CTestEnvironmentalReverbUtility::DisableEffect(CStifItemParser */*aItem*/, TTestResult & aResult)
	{
	TInt err=KErrNone;

	TRAP(err, iEnvironmentalReverbUtility->DisableEnvironmentalReverbL());

	aResult.iResult = err;
	if (err)
		{
		iLogger->Log(_L("DisableEnvironmentalReverbL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
		}
	else
		{
		iLogger->Log(_L("DisableEnvironmentalReverbL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
		}

	return err;
	}


CTestEnvironmentalReverbUtility* CTestEnvironmentalReverbUtility::NewLC (CStifLogger * aLogger)
	{
	CTestEnvironmentalReverbUtility* self=new (ELeave) CTestEnvironmentalReverbUtility(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestEnvironmentalReverbUtility* CTestEnvironmentalReverbUtility::NewL (CStifLogger * aLogger)
	{
	CTestEnvironmentalReverbUtility* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
	}

CTestEnvironmentalReverbUtility::CTestEnvironmentalReverbUtility(CStifLogger * aLogger): CTestAudioEffectPreset(aLogger)
	{
	}

void CTestEnvironmentalReverbUtility::ConstructL()
	{
	}

CTestEnvironmentalReverbUtility::~CTestEnvironmentalReverbUtility()
	{
    DeleteAudioEffect();
		}

void CTestEnvironmentalReverbUtility::DeleteAudioEffect()
	{
	if(iEnvironmentalReverbUtility)
    	delete iEnvironmentalReverbUtility;
    iEnvironmentalReverbUtility = NULL;

	}

TBool CTestEnvironmentalReverbUtility::AssignEffect()
	{
	iEnvironmentalReverb =  &(iEnvironmentalReverbUtility->EnvironmentalReverb());
	return (iEnvironmentalReverb != NULL);
	}


TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioPlayerUtility* aPlayer)
	{
	DeleteAudioEffect();
	if (aPlayer)
		{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aPlayer));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
		}
	else
		{
		iLogger->Log(_L("Create failed: Parameter player is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioRecorderUtility* aRecorder)
	{
	DeleteAudioEffect();
	if (aRecorder)
		{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aRecorder));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
		}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Recorder is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioConvertUtility* aConverter)
	{
    DeleteAudioEffect();
    if (aConverter)
    	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aConverter));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Converter is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioToneUtility* aTone)
	{
    DeleteAudioEffect();
    if (aTone)
    	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aTone));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Tone object is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMMFDevSound* aDevSound)
	{
    DeleteAudioEffect();
    if (aDevSound)
    	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aDevSound));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter DevSound is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}


TInt CTestEnvironmentalReverbUtility::AssignObject(CCustomCommandUtility* aCustomCommand)
	{
    if (aCustomCommand)
    	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(aCustomCommand));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter CustomCommand is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(MCustomInterface* aCustomInterface)
	{
    if (aCustomInterface)
    	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aCustomInterface));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter CustomInterface is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioInputStream* aInputStream)
{
    DeleteAudioEffect();
   	if (aInputStream)
   	{
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aInputStream));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
			}
		}
   	}
	else
	{
		iLogger->Log(_L("Create failed: Parameter InputStream is NULL!\n"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMdaAudioOutputStream* aOutputStream)
{
    DeleteAudioEffect();
    if (aOutputStream)
    {
		TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aOutputStream));
		if ( iError == KErrNone)
		{
			if (!AssignEffect())
			{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
			}
		}
    }
	else
	{
		iLogger->Log(_L("Create failed: Parameter OutputStream is NULL!\n"));
		iError = KErrBadHandle;
	}

	return iError;
}

TInt CTestEnvironmentalReverbUtility::AssignObject(CMidiClientUtility* aMidiClient)
	{
	    DeleteAudioEffect();
	    if (aMidiClient)
	    {
			TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aMidiClient));
			if ( iError == KErrNone)
			{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
			}
	    }
		else
		{
			iLogger->Log(_L("Create failed: Parameter MidiClient is NULL!"));
			iError = KErrBadHandle;
		}

		return iError;

	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		if (aDrmPlayer)
			{
			TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aDrmPlayer));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter DrmPlayer is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}

TInt CTestEnvironmentalReverbUtility::AssignObject(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		if (aUtility)
			{
			TRAP(iError, iEnvironmentalReverbUtility = CEnvironmentalReverbUtility::NewL(*aUtility));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter Utility is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}

TInt CTestEnvironmentalReverbUtility::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagCreatePreset))
	{
		return CreateNewPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagGetPreset))
	{
		return GetPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagNumberOfPreDefinePresets))
	{
		return NumberOfPreDefinedPresets(aResult);
	}

	if (!tag.Compare(KTagApplyPreset))
	{
		return ApplyPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagModifyPreset))
	{
		return ModifyPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagResetPreset))
	{
		return ResetPreset(aItem, aResult);
	}

	if (!tag.Compare(KTagDeletePreset))
	{
		return DeletePreset(aItem, aResult);
	}

	if (!tag.Compare(KTagDisable))
	{
		return DisableEffect(aItem, aResult);
	}

	return CTestAudioEffectPreset::ProcessTag(aItem, aResult);
}



void CTestEnvironmentalReverbUtility::PrintPresetNamesL()
{
	TArray<TEfEnvironmentalReverbUtilityPreset> presetData = iEnvironmentalReverbUtility->Presets();
	iPresetCount = presetData.Count();

//	ShowMenu3(iPresetCount);

	for(TInt i = 0; i < iPresetCount; i++)
	{
			iLogger->Log(KAudioIndex,i+1);
			iLogger->Log(iEnvironmentalReverbUtility->GetPresetL(i));
			iLogger->Log(KAudioNewLine);
	}

//	iLogger->Log(_L("\nPress any key ..."));

}



/*==================================================================================
//  CTestStereoWideningUtility
====================================================================================*/
TInt CTestStereoWideningUtility::CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TPtrC aPresetName;
	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	err = aItem->GetString(KTagCreatePreset, aPresetName);

	if(err)
	{
		iLogger->Log(_L("Missing Preset name. use default\n"));
		CreateNextPresetName(presetData.Count() + 1);
		aPresetName.Set(iPresetName);
	}

	TRAP(err, iStereoWideningUtility->CreatePresetL(aPresetName, *iStereoWidening));

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("CreatePresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


 TInt CTestStereoWideningUtility::GetPreset(CStifItemParser *aItem, TTestResult & aResult)
 {
 	TInt err=KErrNone;
 	TInt aPresetIndex;
 	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

 	err = aItem->GetInt(KTagGetPreset, aPresetIndex);

 	if(err)
 	{
 		iLogger->Log(_L("Missing Preset index. use default"));
 		aPresetIndex = presetData.Count() - 1;

 	}

 	if (aPresetIndex < 0)
 	{
 		iLogger->Log(_L("No preset defined to get"));
 		err = KErrNotReady;
 	}
 	else
 	{
 		TRAP(err, iStereoWideningUtility->GetPresetL(aPresetIndex));
 	}

	if ( (aPresetIndex >500 ) && (err == KErrArgument))
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
 	aResult.iResult = err;
 	if (err)
 	{
 		iLogger->Log(_L("GetPresetL Failed."));
 		aResult.iResultDes.Copy(KTestCaseResultFail());
 	}
 	else
 	{
 		iLogger->Log(_L("GetPresetL Successful."));
 		aResult.iResultDes.Copy(KTestCaseResultSuccess());
 	}
 	return err;
 }


 TInt CTestStereoWideningUtility::NumberOfPreDefinedPresets(TTestResult & aResult)
 {
 	TInt err=KErrNone;
// 	TInt aPresetIndex;
// 	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();
	TUint32 aPredefinedPresets = 0;

	TRAP(err, aPredefinedPresets = iStereoWideningUtility->NumberOfPreDefinedPresets());

 	aResult.iResult = err;
 	if (err)
 	{
 		iLogger->Log(_L("NumberOfPreDefinedPresets Failed."));
 		aResult.iResultDes.Copy(KTestCaseResultFail());
 	}
 	else
 	{
 		iLogger->Log(_L("NumberOfPreDefinedPresets Successful."));
 		aResult.iResultDes.Copy(KTestCaseResultSuccess());
 		RDebug::Print(_L("NumberOfPreDefinedPresets:-> [%d]"), aPredefinedPresets);
 	}
 	return err;
 }

TInt CTestStereoWideningUtility::ApplyPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	err = aItem->GetInt(KTagApplyPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	if (aPresetIndex <= 0)
	{
		iLogger->Log(_L("No preset defined for apply"));
		err = KErrNotReady;
	}
	else
	{
		TRAP(err, iStereoWideningUtility->ApplyPresetL(aPresetIndex));
	}

	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ApplyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ApplyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestStereoWideningUtility::ModifyPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TPtrC aPresetName;

	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	err = aItem->GetInt(KTagModifyPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;
	}

	if (aPresetIndex < 0)
	{
		iLogger->Log(_L("No preset defined for modify"));
		err = KErrNotReady;
	}
	else
	{
		err = aItem->GetNextString(KTagModifyPreset, aPresetName);
		if(err)
		{
			iLogger->Log(_L("Missing Preset name. use default"));
			CreateNextPresetName(presetData.Count() + 1);
			aPresetName.Set(iPresetName);
		}

		//CStereoWidening* newStereoWidening =  CStereoWidening::NewL();
		TRAP(err, iStereoWideningUtility->ModifyPresetL(aPresetIndex,aPresetName,*iStereoWidening));
		//delete newStereoWidening;
		iStereoWidening =  &(iStereoWideningUtility->StereoWidening());
	}

	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ModifyPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ModifyPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestStereoWideningUtility::ResetPreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	err = aItem->GetInt(KTagResetPreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	if (aPresetIndex < 0)
	{
		iLogger->Log(_L("No preset defined for reset"));
		err = KErrNotReady;
	}
	else
	{
		TRAP(err, iStereoWideningUtility->ResetPresetL(aPresetIndex));
	}

  if (err == KErrNotSupported)    
  {
  	iLogger->Log(_L("ResetPresetL() return KErrNotSupported, It is accepted return code"));
  	err = KErrNone;
  }
  
	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("ResetPresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("ResetPresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
}


TInt CTestStereoWideningUtility::DeletePreset(CStifItemParser *aItem, TTestResult & aResult)
{
	TInt err=KErrNone;
	TInt aPresetIndex;
	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	err = aItem->GetInt(KTagDeletePreset, aPresetIndex);

	if(err)
	{
		iLogger->Log(_L("Missing Preset index. use default"));
		aPresetIndex = presetData.Count() - 1;

	}

	if (aPresetIndex <= 0)
	{
		iLogger->Log(_L("No preset defined for delete"));
		err = KErrNotReady;
	}
	else
	{
		TRAP(err, iStereoWideningUtility->DeletePresetL(aPresetIndex));
	}

	if ( (aPresetIndex >500 ) && (err == KErrArgument)) 
	{
		iLogger->Log(_L("this is a error case, return code KErrArgument is accepted"));
		err = KErrNone;
	}
	
	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("DeletePresetL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("DeletePresetL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}
	return err;
}

TInt CTestStereoWideningUtility::DisableEffect(CStifItemParser */*aItem*/, TTestResult & aResult)
	{
	TInt err=KErrNone;

	TRAP(err, iStereoWideningUtility->DisableStereoWideningL());

	aResult.iResult = err;
	if (err)
	{
		iLogger->Log(_L("DisableStereoWideningL Failed."));
		aResult.iResultDes.Copy(KTestCaseResultFail());
	}
	else
	{
		iLogger->Log(_L("DisableStereoWideningL Successful."));
		aResult.iResultDes.Copy(KTestCaseResultSuccess());
	}

	return err;
	}

CTestStereoWideningUtility* CTestStereoWideningUtility::NewLC (CStifLogger * aLogger)
	{
	CTestStereoWideningUtility* self=new (ELeave) CTestStereoWideningUtility(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestStereoWideningUtility* CTestStereoWideningUtility::NewL (CStifLogger * aLogger)
	{
	CTestStereoWideningUtility* self=NewLC(aLogger);
	CleanupStack::Pop();
	return self;
	}

CTestStereoWideningUtility::CTestStereoWideningUtility(CStifLogger * aLogger): CTestAudioEffectPreset(aLogger)
	{
	}

void CTestStereoWideningUtility::ConstructL()
	{
	}

CTestStereoWideningUtility::~CTestStereoWideningUtility()
	{
    DeleteAudioEffect();
	}

void CTestStereoWideningUtility::DeleteAudioEffect()
	{
	if(iStereoWideningUtility)
	    delete iStereoWideningUtility;
    iStereoWideningUtility = NULL;

	}

TBool CTestStereoWideningUtility::AssignEffect()
	{
	iStereoWidening =  &(iStereoWideningUtility->StereoWidening());
	return (iStereoWidening != NULL);
	}

// Callback// Callback
TInt CTestStereoWideningUtility::AssignObject(CMdaAudioPlayerUtility* aPlayer)
	{
	DeleteAudioEffect();
	if (aPlayer)
		{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aPlayer));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed\n"));
				iError = KErrBadHandle;
				}
			}
		}
	else
		{
		iLogger->Log(_L("Create failed: Parameter player is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMdaAudioRecorderUtility* aRecorder)
	{
	DeleteAudioEffect();
	if (aRecorder)
		{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aRecorder));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
		}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Recorder is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMdaAudioConvertUtility* aConverter)
	{
    DeleteAudioEffect();
    if (aConverter)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aConverter));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Converter is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMdaAudioToneUtility* aTone)
	{
    DeleteAudioEffect();
    if (aTone)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aTone));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter Tone object is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMMFDevSound* aDevSound)
	{
    DeleteAudioEffect();
    if (aDevSound)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aDevSound));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter DevSound is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}


TInt CTestStereoWideningUtility::AssignObject(CCustomCommandUtility* aCustomCommand)
	{
    if (aCustomCommand)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(aCustomCommand));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter CustomCommand is NULL!\n"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(MCustomInterface* aCustomInterface)
	{
    if (aCustomInterface)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aCustomInterface));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter CustomInterface is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMdaAudioInputStream* aInputStream)
	{
    DeleteAudioEffect();
   	if (aInputStream)
   		{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aInputStream));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
   		}
	else
		{
		iLogger->Log(_L("Create failed: Parameter InputStream is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMdaAudioOutputStream* aOutputStream)
	{
    DeleteAudioEffect();
    if (aOutputStream)
    	{
		TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aOutputStream));
		if ( iError == KErrNone)
			{
			if (!AssignEffect())
				{
				iLogger->Log(_L("Assign Utility Object failed"));
				iError = KErrBadHandle;
				}
			}
    	}
	else
		{
		iLogger->Log(_L("Create failed: Parameter OutputStream is NULL!"));
		iError = KErrBadHandle;
		}

	return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CMidiClientUtility* aMidiClient)
	{
	    DeleteAudioEffect();
	    if (aMidiClient)
	    {
			TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aMidiClient));
			if ( iError == KErrNone)
			{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
			}
	    }
		else
		{
			iLogger->Log(_L("Create failed: Parameter MidiClient is NULL!"));
			iError = KErrBadHandle;
		}

		return iError;

	}

TInt CTestStereoWideningUtility::AssignObject(CDrmPlayerUtility* aDrmPlayer)
	{
		DeleteAudioEffect();
		if (aDrmPlayer)
			{
			TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aDrmPlayer));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter DrmPlayer is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}

TInt CTestStereoWideningUtility::AssignObject(CVideoPlayerUtility* aUtility)
	{
		DeleteAudioEffect();
		if (aUtility)
			{
			TRAP(iError, iStereoWideningUtility = CStereoWideningUtility::NewL(*aUtility));
			if ( iError == KErrNone)
				{
				if (!AssignEffect())
				{
					iLogger->Log(_L("Assign Utility Object failed"));
					iError = KErrBadHandle;
				}
				}
			}
		else
			{
			iLogger->Log(_L("Create failed: Parameter Utility is NULL!"));
			iError = KErrBadHandle;
			}
		return iError;
	}
TInt CTestStereoWideningUtility::ProcessTag(CStifItemParser *aItem, TTestResult & aResult)
	{
	TPtrC tag;
	aItem->GetString(_L(""), tag);

	if (!tag.Compare(KTagCreatePreset))
		{
		return CreateNewPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagGetPreset))
		{
		return GetPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagNumberOfPreDefinePresets))
		{
		return NumberOfPreDefinedPresets(aResult);
		}
	if (!tag.Compare(KTagApplyPreset))
		{
		return ApplyPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagModifyPreset))
		{
		return ModifyPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagResetPreset))
		{
		return ResetPreset(aItem, aResult);
		}

	if (!tag.Compare(KTagDeletePreset))
		{
		return DeletePreset(aItem, aResult);
		}

	if (!tag.Compare(KTagDisable))
		{
		return DisableEffect(aItem, aResult);
		}

	return CTestAudioEffectPreset::ProcessTag(aItem, aResult);
	}

void CTestStereoWideningUtility::PrintPresetNamesL()
	{
	TArray<TEfStereoWideningUtilityPreset> presetData = iStereoWideningUtility->Presets();

	iPresetCount = presetData.Count();

	for(TInt i = 0; i < iPresetCount; i++)
		{
		iLogger->Log(KAudioIndex,i+1);
		iLogger->Log(iStereoWideningUtility->GetPresetL(i));
		iLogger->Log(KAudioNewLine);
		}

	}


