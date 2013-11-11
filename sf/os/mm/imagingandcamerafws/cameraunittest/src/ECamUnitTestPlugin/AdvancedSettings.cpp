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
//


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <ecamuids.hrh>
#include <ecam.h>
#include "ECamUnitTestPluginUids.hrh"
#include "AdvancedSettings.h"
#include <ecamerrors.h>

#include <ecamadvsettings.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsconst.h>
#include <ecamadvsettingsdef.h>
#include <ecamimageprocessingconst.h>
#include <ecamconst.h>
#endif

const TInt KContinuousAutoFocusTimeoutValue = 20000;
const TInt KBrightnessAdjMinValue			= -50;
const TInt KBrightnessAdjMaxValue			= 50;
const TInt KDefaultShutterSpeed				= 0;
const TInt KDefaultFocusDistance			= 0;
const TInt KDefaultAperture					= 280;
const TInt KBracketStartIndex				= 0;
const TInt KBracketFrames					= 2;
const TBool KDefaultRedEyeReduce			= EFalse;
const TInt KFlashCompensationStep			= 2;
const TInt KFlashCompensationInSteps		= 1;
const TBool KExternalFlashPresentState		= EFalse;
const TInt KManualFlashPowerLevel			= 0;
const TInt KExposureCompensationStep		= 3;
const TInt KExposureCompensationInSteps		= 2;
const TBool KApertureExposureLockOn         = EFalse;
const TBool KShootClickOn					= EFalse;
const TInt KTimerInterval					= 0;
const TInt KBurstImages						= 0;
const TBool KExposureLockOn         		= EFalse;
const TBool KAutoFocusLockOn         		= EFalse;
const TBool KAutomaticSizeSelectionChangeOn	= EFalse;
const TInt KWBColorTemperature				= 6000;
const TBool KFlashReadyState				= EFalse;
const TInt KSupportedISORateType			= 0x1F;
const TBool KCapableActualISOValue			= EFalse;
const TInt KAFAssistantLightManual			= 0x01;
const TInt KConcurrentColorOpSupported		= 0x01;
const TInt KDefault							= 0x01;
const TInt KFocusRangeSupported				= 0x7F;
const TInt KWhiteBalanceSupported			= 0x0FFF;

//
// CCamPresets
//
CCamPresets::CCamPresets(CCamUnitTestPlugin& aOwner): iOwner(aOwner)
	{
	iSupportedPresets[0] = KUidECamPresetOutdoor;
	iSupportedPresets[1] = KUidECamPresetBeach;
	iSupportedPresets[2] = KUidECamPresetNightPartyIndoor;
	iSupportedPresets[3] = KUidECamPresetNone;

	iCurrentPreset = KUidECamPresetNone;

	iAffectedSettings[0] = KUidECamEventCameraSettingAperture;
	iAffectedSettings[1] = KUidECamEventCameraSettingIsoRateType;
	iAffectedSettings[2] = KUidECamEventCameraSettingIsoRate;

	iRangeRestrictedSettings.Reset();
	iIsPresetUnLockSupported = EFalse;
	iIsPresetLocked = ETrue;
	}
	
CCamPresets::~CCamPresets()
	{
	Dll::FreeTls();
	iRangeRestrictedSettings.Close();
	}
	
CCamPresets* CCamPresets::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamPresets = new (ELeave) CCamPresets(aOwner);
    	globalData->iCamPresets->iRefCount = 1;
    	
		TInt error = Dll::SetTls(globalData);
		if (error)
			{
			delete globalData->iCamPresets;
			User::Leave(error);
			}
		
		CleanupStack::Pop(globalData);
    	
		return static_cast <CCamPresets*> (globalData->iCamPresets);
		}
	else
		{
		CCamPresets* self = globalData->iCamPresets;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 1 )
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			TInt error = Dll::SetTls(globalData);
			if (error)
				{
				delete globalData->iCamPresets;
				User::Leave(error);
				}	
			}
		return static_cast <CCamPresets*> (self);		
		}
	}

void CCamPresets::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}	
	
void CCamPresets::GetSupportedPresetsL(RArray<TUid>& aPresets) const
	{
	aPresets.Reset();
	TInt count = sizeof(iSupportedPresets)/sizeof(TUid);
	
	for (TInt i = 0; i < count; i++)
		{
		aPresets.AppendL(iSupportedPresets[i]);
		}	
	}
	
void CCamPresets::SetPreset(TUid aPreset)
	{
	TInt response = KErrECamParameterNotInRange;
	
	for (TInt i = 0; i < KSupportedPresetsNumber; i++)
		{
		if (iSupportedPresets[i] == aPreset)
			{
			iCurrentPreset = aPreset;
			response = KErrNone;
			
			switch(aPreset.iUid)
				{
				case KUidECamPresetOutdoorUidValue:
					{
					response = iRangeRestrictedSettings.Append(KUidECamEventCameraSettingIsoRate);
					if(iOwner.iSupportedISORates.Count() == KNumSupportedIsoRate)
						{
						iOwner.iSupportedISORates.Remove(KNumSupportedIsoRate-1);
						if(iOwner.iIsoRate == KMaxIsoRate)
							{
							iOwner.iIsoRate = KIsoRate5;
							}
						}
					iIsPresetUnLockSupported = ETrue;
					
					break;
					}
				
				//fall through
				case KUidECamPresetBeachUidValue:
				case KUidECamPresetNightPartyIndoorUidValue:
				case KUidECamPresetNoneUidValue:
				default:
					{
					if(iOwner.iSupportedISORates.Count() == KNumSupportedIsoRate-1)
						{
						iOwner.iSupportedISORates.AppendL(KMaxIsoRate);
						}
					iIsPresetUnLockSupported = EFalse;
					break;	
					}
				}
			
			break;
			}
		}
		
	iOwner.Notify(aPreset, response);
	
	if(aPreset == KUidECamPresetOutdoor)
		{
		iOwner.Notify(KUidECamEventRangeRestricted, KErrNone);
		}
	}
	
TUid CCamPresets::Preset() const
	{
	return iCurrentPreset;
	}
	
void CCamPresets::GetAffectedSettingsL(RArray<TUid>& aSettings) const
	{
	aSettings.Reset();
		
	if (iCurrentPreset!=KUidECamPresetNone)
		{
		TInt count = sizeof(iAffectedSettings)/sizeof(TUid);
		for (TInt i = 0; i < count; i++)
			{
			aSettings.AppendL(iAffectedSettings[i]);
			}
		}
	}
	
void CCamPresets::GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const
	{
	aSettings.Reset();
	if (aPreset == KUidECamPresetNone)
		{
		User::Leave(KErrArgument);
		}
	TInt count = sizeof(iAffectedSettings)/sizeof(TUid);
	for (TInt i = 0; i < count; i++)
		{
		aSettings.AppendL(iAffectedSettings[i]);
		}
	}
	
void CCamPresets::GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) const
	{
	for(TInt index=0; index<iRangeRestrictedSettings.Count(); index++)
		{
		aRangeRestrictedSettings.AppendL(iRangeRestrictedSettings[index]);
		}
	}

void CCamPresets::GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) const
	{
	aFeatureRestrictedSettings.Reset();
	}
	
void CCamPresets::IsPresetUnlockSupportedL(TBool& aUnlockSupported) const
	{
	aUnlockSupported = iIsPresetUnLockSupported;	
	}

void CCamPresets::LockPreset()
	{
	iIsPresetLocked = ETrue;
	iOwner.Notify(KUidECamEventPresetLocked, KErrNone);
	}
	
void CCamPresets::UnlockPreset()
	{
	TInt error = KErrNotSupported;
	if(iIsPresetUnLockSupported)
		{
		iIsPresetLocked = EFalse;
		error = KErrNone;
		}
	iOwner.Notify(KUidECamEventPresetUnlocked, error);
	}
	
//
// CCamImgProc
//
CCamImgProc::CCamImgProc(CCamUnitTestPlugin& aOwner): iOwner(aOwner)
	{
	}
	
CCamImgProc::~CCamImgProc()
	{
	Dll::FreeTls();
	iActiveSequence.Close();
	iSupportedTransformations.Close();
	}
	
CCamImgProc* CCamImgProc::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamImgProc = new (ELeave) CCamImgProc(aOwner);
		CleanupStack::PushL(globalData->iCamImgProc);
		globalData->iCamImgProc->ConstructL();
    	globalData->iCamImgProc->iRefCount = 1;
    	CleanupStack::Pop(globalData->iCamImgProc);
		TInt error = Dll::SetTls(globalData);
		if (error)
			{
			delete globalData->iCamImgProc;
			User::Leave(error);
			}
		CleanupStack::Pop(globalData);
		
		return static_cast <CCamImgProc*> (globalData->iCamImgProc);
		}
	else
		{
		CCamImgProc* self = globalData->iCamImgProc;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 2 )
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			TInt error = Dll::SetTls(globalData);
			if (error)
				{
				delete globalData->iCamImgProc;
				User::Leave(error);
				}	
			}
		return static_cast <CCamImgProc*> (self);		
		}
	}
	
void CCamImgProc::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}	
	
void CCamImgProc::ConstructL()
	{
	iSupportedTransformations.AppendL(KUidECamEventImageProcessingAdjustBrightness);
	iSupportedTransformations.AppendL(KUidECamEventImageProcessingAdjustContrast);
	iSupportedTransformations.AppendL(KUidECamEventImageProcessingEffect);
	iBrightnessRange[0] = KBrightnessAdjMinValue; // min 
	iBrightnessRange[1] = KBrightnessAdjMaxValue; // max value, step assumed 1
	
	iSupportedColorSwapCapabilities.iSupportedSourceRepresentation  = KDefault;
	iSupportedColorSwapCapabilities.iSupportedTargetRepresentation  = KDefault;	
	iSupportedColorSwapCapabilities.iSupportedSourceRgbGroupingMode = KDefault;
	iSupportedColorSwapCapabilities.iSupportedTargetRgbGroupingMode = KDefault;
	iSupportedColorSwapCapabilities.iIsCapabilityUniform = ETrue;
	
	iSupportedColorAccentCapabilities.iSupportedSourceRepresentation   = KDefault;
	iSupportedColorAccentCapabilities.iSupportedSourceRgbGroupingMode  = KDefault;
	iSupportedColorAccentCapabilities.iIsCapabilityUniform = ETrue;
	}
	
void CCamImgProc::GetSupportedTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		for (TInt i=0; i<iSupportedTransformations.Count(); i++)
			{
			aTransformations.AppendL(iSupportedTransformations[i]);
			}
		}
	}
	
void CCamImgProc::GetActiveTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		for (TInt i=0; i<iSupportedTransformations.Count(); i++)
			{
			aTransformations.AppendL(iSupportedTransformations[i]);
			}
		}
	}
	
void CCamImgProc::GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const
	{
	if (iSupportedTransformations.Find(aTransformation)==KErrNotFound)
		{
		aValues.Reset();
		aInfo = ENotActive;
		}
	else
		{
		if (aTransformation == KUidECamEventImageProcessingAdjustBrightness)
			{
			aValues.AppendL(iBrightnessRange[0]);
			aValues.AppendL(iBrightnessRange[1]);
			aInfo = EContinuousRangeMinMax;
			}
		else
			{
			if(aTransformation == KUidECamEventImageProcessingEffect)
				{
				aValues.AppendL(CCamera::CCameraImageProcessing::EEffectColorSwap | CCamera::CCameraImageProcessing::EEffectColorAccent);
				aInfo = EBitField;
				}
			else
				{
				aInfo = EDiscreteSteps;
				if (aTransformation == iTransformation)
					{
					aValues.AppendL(iValue);
					}
				else
					{
					aValues.AppendL(KECamImageProcessingDefaultValue);
					}	
				}
			}
		}
	}

TInt CCamImgProc::TransformationValue(TUid aTransformation) const
	{
	if (iSupportedTransformations.Find(aTransformation) == KErrNotFound)
		{
		return KErrNotFound;
		}
	else
		{
		if (aTransformation == iTransformation)
			{
			return iValue;
			}
		else
			{
			return KECamImageProcessingDefaultValue;
			}
		}
	}
	
TInt CCamImgProc::GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const
	{
	if (iSupportedTransformations.Find(aTransformation) == KErrNotFound)
		{
		return KErrNotFound;
		}
	else
		{
		if (aTransformation == iTransformation)
			{
			aTransformationValue = iValue;
			}
		else
			{
			aTransformationValue = KECamImageProcessingDefaultValue;
			}
			
		return KErrNone;
		}
	}
	
void CCamImgProc::SetTransformationValue(TUid aTransformation, TInt aValue)
	{
	TInt response;
	
	if (iSupportedTransformations.Find(aTransformation) == KErrNotFound)
		{
		response = KErrECamSettingDisabled;
		}
	else
		{
		if (aTransformation == KUidECamEventImageProcessingAdjustBrightness)
			{
			// if outside the range 
			if (aValue < iBrightnessRange[0] || aValue > iBrightnessRange[1])
				{
				response = KErrECamParameterNotInRange;
				}
			else
				{
				response = KErrNone;
				}
			}
		else
			{
			response = KErrNone;
			if(aTransformation == KUidECamEventImageProcessingEffect)
				{
				if(aValue == CCamera::CCameraImageProcessing::EEffectColorSwap)
					{
					iColorSwapParameter.iEntryStatus = ENotActive;
					}
				else
					{
					if(aValue == CCamera::CCameraImageProcessing::EEffectColorAccent)	
						{
						iColorAccentParameter.iEntryStatus = ENotActive;
						}
					else
						{
						response = KErrArgument;	
						}
					}
				}
			}
				
		if (response == KErrNone)
			{
			iTransformation = aTransformation;
			iValue = aValue;
			}
		}
	iOwner.Notify(aTransformation, response);
	}
	
void CCamImgProc::GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const
	{
	aTransformSequence.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		for (TInt i = 0; i < iActiveSequence.Count(); i++)
			{
			aTransformSequence.AppendL(iActiveSequence[i]);
			}
		}		
	}
	
void CCamImgProc::SetActiveTransformSequenceL(RArray<TUid>& aNewTransformSequence)
	{
	iActiveSequence.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		for (TInt i=0; i<aNewTransformSequence.Count(); i++)
			{
			iActiveSequence.AppendL(aNewTransformSequence[i]);
			}
		}	
	}

void CCamImgProc::SetSourceRect( const TRect& aRect)
	{
	iSourceRect = aRect;	
	}
void CCamImgProc::GetSourceRect(TRect& aRect) const
	{
	aRect = iSourceRect;	
	}
	
void CCamImgProc::GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const
	{
	aConcurrentColorSwappingSupported = KConcurrentColorOpSupported;
	}
	
void CCamImgProc::GetColorSwapCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorSwapCapabilities) const
	{
	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		User::Leave(KErrArgument);
		}
			
	if(aColorSwapCapabilities.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationCapabilities) ||
	   aColorSwapCapabilities.Version() != KECamColorOperationCapabilitiesCurrentVersion)
		{
		// new app. running on old impl.
		User::Leave(KErrNotSupported);
		}
	else
		{
		aColorSwapCapabilities = iSupportedColorSwapCapabilities;
		}	
	}
	
void CCamImgProc::SetColorSwapEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters)
	{
	TInt response = KErrNone;

	if(aColorSwapParameters.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationEntry) ||
	aColorSwapParameters.iNumBitsIgnored.Size() != sizeof(CCamera::CCameraImageProcessing::TBitsIgnore) ||
	aColorSwapParameters.Version() != KECamColorOperationEntryCurrentVersion ||
	aColorSwapParameters.iNumBitsIgnored.Version() != KECamBitsIgnoreCurrentVersion
	)
		{
		// new app. running on old impl.
		iOwner.Notify2(KUidECamEvent2CIPSetColorSwapEntry, KErrNotSupported, aIndex);
		}
	else
		{
		if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
			{
			iOwner.Notify2(KUidECamEvent2CIPSetColorSwapEntry, KErrArgument, aIndex);
			}
		else
			{
			if(!CheckColorSwapEntry(aColorSwapParameters))
				{
				// if color operation entry is different from what is supported....
				iOwner.Notify2(KUidECamEvent2CIPSetColorSwapEntry, KErrArgument, aIndex);
				}
			else
				{
				iColorSwapParameter = aColorSwapParameters;
				iColorSwapParameter.iEntryStatus = EDiscreteSteps;
				
				iOwner.Notify2(KUidECamEvent2CIPSetColorSwapEntry, response, aIndex);	
				}
			}
		}
	}
	
void CCamImgProc::RemoveColorSwapEntry(TInt aIndex)
	{
	TInt response = KErrNone;

	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		iOwner.Notify2(KUidECamEventCIPRemoveColorSwapEntry, KErrArgument, aIndex);
		}
	else
		{
		iColorSwapParameter.iEntryStatus = ENotActive;
		iOwner.Notify2(KUidECamEventCIPRemoveColorSwapEntry, response, aIndex);	
		}
	}
	
void CCamImgProc::GetColorSwapEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const
	{
	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		User::Leave(KErrArgument);
		}
		
	if(aColorSwapParameters.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationEntry) ||
	aColorSwapParameters.iNumBitsIgnored.Size() != sizeof(CCamera::CCameraImageProcessing::TBitsIgnore))
		{
		// new app. running on old impl.
		User::Leave(KErrNotSupported);
		}	
	else
		{
		aColorSwapParameters = iColorSwapParameter;
		}		
	}
	
void CCamImgProc::StartColorSwapping()
	{
	TInt response = KErrNone;
	iOwner.Notify(KUidECamEventCIPStartColorSwap, response);
	}
	
void CCamImgProc::CancelColorSwappingL()
	{
	}
	
void CCamImgProc::GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const
	{
	aConcurrentColorAccentSupported = KConcurrentColorOpSupported;
	}
	
void CCamImgProc::GetColorAccentCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorAccentCapabilities) const
	{
	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		User::Leave(KErrArgument);
		}
			
	if(aColorAccentCapabilities.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationCapabilities))
		{
		// new app. running on old impl.
		User::Leave(KErrNotSupported);
		}
	else
		{
		aColorAccentCapabilities = iSupportedColorAccentCapabilities;
		}	
	}
	
void CCamImgProc::SetColorAccentEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters)
	{
	TInt response = KErrNone;

	if(aColorAccentParameters.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationEntry) ||
	aColorAccentParameters.iNumBitsIgnored.Size() != sizeof(CCamera::CCameraImageProcessing::TBitsIgnore))
		{
		// new app. running on old impl.
		iOwner.Notify2(KUidECamEventCIPSetColorAccentEntry, KErrNotSupported, aIndex);
		}
	else
		{
		if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
			{
			iOwner.Notify2(KUidECamEventCIPSetColorAccentEntry, KErrArgument, aIndex);
			}
		else
			{
			if(!CheckColorAccentEntry(aColorAccentParameters))
				{
				// if color operation entry is different from what is supported....
				iOwner.Notify2(KUidECamEventCIPSetColorAccentEntry, KErrArgument, aIndex);
				}
			else
				{
				iColorAccentParameter = aColorAccentParameters;
				iColorAccentParameter.iEntryStatus = EDiscreteSteps;
				
				iOwner.Notify2(KUidECamEventCIPSetColorAccentEntry, response, aIndex);	
				}
			}
		}
	}
	
void CCamImgProc::RemoveColorAccentEntry(TInt aIndex)
	{
	TInt response = KErrNone;

	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		iOwner.Notify2(KUidECamEventCIPRemoveColorAccentEntry, KErrArgument, aIndex);
		}
	else
		{
		iColorAccentParameter.iEntryStatus = ENotActive;
		iOwner.Notify2(KUidECamEventCIPRemoveColorAccentEntry, response, aIndex);	
		}
	}
	
void CCamImgProc::GetColorAccentEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const
	{
	if(aIndex <0 || aIndex > KConcurrentColorOpSupported-1)
		{
		User::Leave(KErrArgument);
		}
		
	if(aColorAccentParameters.Size() != sizeof(CCamera::CCameraImageProcessing::TColorOperationEntry) ||
	aColorAccentParameters.iNumBitsIgnored.Size() != sizeof(CCamera::CCameraImageProcessing::TBitsIgnore))
		{
		// new app. running on old impl.
		User::Leave(KErrNotSupported);
		}	
	else
		{
		aColorAccentParameters = iColorAccentParameter;
		}
	}
	
void CCamImgProc::StartColorAccent()
	{
	TInt response = KErrNone;
	iOwner.Notify(KUidECamEventCIPStartColorAccent, response);
	}
	
void CCamImgProc::CancelColorAccentL()
	{
	}
	
TBool CCamImgProc::CheckColorSwapEntry(const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const
	{
	if(aColorSwapParameters.iSourceColorRepresentation != KDefault)
		{
		return EFalse;
		}
		
	if(aColorSwapParameters.iTargetColorRepresentation != KDefault)
		{
		return EFalse;
		}
	
	if(aColorSwapParameters.iColorOperationSourceRgbGroupingMode != KDefault)
		{
		return EFalse;
		}
	
	if(aColorSwapParameters.iColorOperationTargetRgbGroupingMode != KDefault)
		{
		return EFalse;
		}
		
	if(aColorSwapParameters.iNumBitsIgnored.iRedBitsIgnore != 0)
		{
		return EFalse;
		}	
	 
	if(aColorSwapParameters.iNumBitsIgnored.iGreenBitsIgnore != 0)
		{
		return EFalse;
		}	
		
	if(aColorSwapParameters.iNumBitsIgnored.iBlueBitsIgnore != 0)
		{
		return EFalse;
		}		  	
		
	if(aColorSwapParameters.iNumBitsIgnored.iAlphaBitsIgnore != 0)
		{
		return EFalse;
		}	   
	
	return ETrue;		 
	}

TBool CCamImgProc::CheckColorAccentEntry(const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const
	{
	if(aColorAccentParameters.iSourceColorRepresentation != KDefault)
		{
		return EFalse;
		}
		
	if(aColorAccentParameters.iColorOperationSourceRgbGroupingMode != KDefault)
		{
		return EFalse;
		}
	
	if(aColorAccentParameters.iNumBitsIgnored.iRedBitsIgnore != 0)
		{
		return EFalse;
		}	
	 
	if(aColorAccentParameters.iNumBitsIgnored.iGreenBitsIgnore != 0)
		{
		return EFalse;
		}	
		
	if(aColorAccentParameters.iNumBitsIgnored.iBlueBitsIgnore != 0)
		{
		return EFalse;
		}		  	
		
	if(aColorAccentParameters.iNumBitsIgnored.iAlphaBitsIgnore != 0)
		{
		return EFalse;
		}	   
	
	return ETrue;		 
	}

void CCamImgProc::GetSupportedRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference /*aOrientationReference*/, 
			TUint& /*aSupportedRelativeRotation*/, TUint& /*aSupportedRelativeMirroring*/, TUint& /*aSupportedRelativeFlipping*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamImgProc::GetCurrentRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference& /*aOrientationReference*/, 
			CCamera::CCameraImageProcessing::TRelativeRotation& /*aRelativeRotation*/, CCamera::CCameraImageProcessing::
			TRelativeMirror& /*aRelativeMirror*/, CCamera::CCameraImageProcessing::TRelativeFlipping& /*aRelativeFlipping*/) const
	{
	User::Leave(KErrNotSupported);	
	}

void CCamImgProc::SetRelativeOrientationOptions(CCamera::CCameraImageProcessing::TOrientationReference /*aOrientationReference*/, 
			CCamera::CCameraImageProcessing::TRelativeRotation /*aRelativeRotation*/, CCamera::CCameraImageProcessing::
			TRelativeMirror /*aRelativeMirror*/, CCamera::CCameraImageProcessing::TRelativeFlipping /*aRelativeFlipping*/) const
	{
	iOwner.Notify(KUidECamEventImageProcessingTransformRelativeOrientation, KErrNotSupported);
	}
	
//
// CCamAdvSet
//

CCamAdvSet::CCamAdvSet(CCamUnitTestPlugin& aOwner): iOwner(aOwner)
	{
	Init();
	}
	
CCamAdvSet::~CCamAdvSet()
	{
	Dll::FreeTls();
	}
		
CCamAdvSet* CCamAdvSet::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamAdvSet = new (ELeave) CCamAdvSet(aOwner);
		globalData->iCamAdvSet->iRefCount = 1;
		TInt error =  Dll::SetTls(globalData);
		if(error)
			{
			delete globalData->iCamAdvSet;
			User::Leave(error);
			}
		CleanupStack::Pop(globalData);
		return static_cast <CCamAdvSet*> (globalData->iCamAdvSet);
		}
	else
		{
		CCamAdvSet* self = globalData->iCamAdvSet;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 3 )
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			TInt error =  Dll::SetTls(globalData);
			if(error)
				{
				delete globalData->iCamAdvSet;
				User::Leave(error);
				}
			}
		return static_cast <CCamAdvSet*> (self);		
		}
	}

void CCamAdvSet::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}	
	
void CCamAdvSet::Init()
	{
	iShutterSpeed = KDefaultShutterSpeed;
	iFocusDistance = KDefaultFocusDistance;
	iAperture = KDefaultAperture;
	iFocusRange = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
	iWhiteBalance = CCamera::EWBAuto;
	
	iISORateType = CCamera::CCameraAdvancedSettings::EISOManual;
	iSupportedISORateType = KSupportedISORateType;
	iIsCapableActualISOValue = KCapableActualISOValue;
	iAutoISOIndex = -1;
	iSupportedAFAssistantLight = KAFAssistantLightManual;
	iAFAssistantLight = CCamera::CCameraAdvancedSettings::EAFAssistantLightOff;
	iPixelAspectRatio = CCamera::CCameraAdvancedSettings::EPixelAspect1To1;
	iFlashMode = CCamera::EFlashNone;

	iSupportedDriveModes |= CCamera::CCameraAdvancedSettings::EDriveModeSingleShot;
	iSupportedDriveModes |= CCamera::CCameraAdvancedSettings::EDriveModeBurst;
	iSupportedDriveModes |= CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture;
	
	iDriveMode = CCamera::CCameraAdvancedSettings::EDriveModeAuto;
	}

CCamera::CCameraAdvancedSettings::TCameraType CCamAdvSet::CameraType() const
	{
	return iOwner.iCameraTypes[iOwner.iCameraIndex];	
	}

CCamera::CCameraAdvancedSettings::TCameraType CCamAdvSet::CameraType(TInt aCameraIndex) const
	{
	if (aCameraIndex >= KECamSetAvailableCameras)
		{
		return CCamera::CCameraAdvancedSettings::ECameraUnknown;	
		}
	else 
		{
		return iOwner.iCameraTypes[aCameraIndex];	
		}
	}
	
TBool CCamAdvSet::IsCameraPresent() const
	{
	return iOwner.iCameras[iOwner.iCameraIndex];
	}

TBool CCamAdvSet::IsCameraPresent(TInt aCameraIndex) const
	{
	if (aCameraIndex >= KECamSetAvailableCameras)
		{
		return EFalse;
		}
	else
		{
		return iOwner.iCameras[aCameraIndex];
		}
	}
	
TInt CCamAdvSet::CameraIndex() const
	{
	return iOwner.iCameraIndex;
	}
	
TInt CCamAdvSet::SupportedStabilizationModes() const
	{
	return 0;
	}
CCamera::CCameraAdvancedSettings::TStabilizationMode CCamAdvSet::StabilizationMode() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationModeOff;
	}
	
void CCamAdvSet::SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode /*aStabilizationMode*/)
	{
	}
	
TInt CCamAdvSet::SupportedFocusModes() const
	{
	return 0;
	}
	
CCamera::CCameraAdvancedSettings::TFocusMode CCamAdvSet::FocusMode() const
	{
	return CCamera::CCameraAdvancedSettings::EFocusModeAuto;	
	}
	
void CCamAdvSet::SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode /*aFocusMode*/)
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingFocusMode, response);
	}
	
TInt CCamAdvSet::SupportedFocusRanges() const
	{
	return KFocusRangeSupported;
	}
	
CCamera::CCameraAdvancedSettings::TFocusRange CCamAdvSet::FocusRange() const
	{
	return iFocusRange;
	}
	
void CCamAdvSet::SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange )
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iFocusRange = aFocusRange;
	iOwner.Notify(KUidECamEventCameraSettingFocusRange2, response);
	iOwner.Notify(KUidECamEventCameraSettingFocusRange, response);
	}
	
TInt CCamAdvSet::SupportedAutoFocusTypes() const
	{
	return 0;
	}
	
CCamera::CCameraAdvancedSettings::TAutoFocusType CCamAdvSet::AutoFocusType() const
	{
    return CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff;
	}
	
void CCamAdvSet::SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType /*aAutoFocusType*/)
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingAutoFocusType2, response);
	iOwner.Notify(KUidECamEventCameraSettingAutoFocusType, response);
	}
	
TInt CCamAdvSet::SupportedAutoFocusAreas() const
	{
	return 0;
	}
	
CCamera::CCameraAdvancedSettings::TAutoFocusArea CCamAdvSet::AutoFocusArea() const
	{
	return CCamera::CCameraAdvancedSettings::EAutoFocusTypeAuto;
	}
	
void CCamAdvSet::SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea /*aAutoFocusArea*/)
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingAutoFocusArea, response);
	}
	
TInt CCamAdvSet::FocusDistance() const
	{
	return iFocusDistance;
	}
	
void CCamAdvSet::SetFocusDistance(TInt aDistance) 
	{
	TInt response = KErrNone;
	if (aDistance >=0)
		{
		iFocusDistance = aDistance;
		
		RetrieveResult(response);
		}
	else 
		{
		response = KErrNotSupported;
		}
	
	iOwner.Notify(KUidECamEventCameraSettingFocusDistance, response);
	}
	
TInt CCamAdvSet::GetMinFocalLength() const
	{
	return KErrNotSupported;
	}
	
void CCamAdvSet::GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const
	{
	//If no ISO is supported by camera, then leave
	if(!iOwner.iSupportedISORates.Count())
		{
		User::Leave(KErrNotSupported);
		}
	
	for(TInt index=0; index < iOwner.iSupportedISORates.Count(); index++)
		{
		aSupportedIsoRates.AppendL(iOwner.iSupportedISORates[index]);
		}
	}
	
TInt CCamAdvSet::IsoRate() const
	{
	//this returned value may not be true if ISO type is other than manual and camera 
	//has no capability to retrieve actual ISO rate.
	return iOwner.iIsoRate; 
	}

void CCamAdvSet::SetIsoRate(TInt aRate)
	{
	TInt response;
	//check whether the desired rate to be set is being supported or not.
	if(iOwner.iSupportedISORates.Find(aRate) == KErrNotFound)
		{
		response = KErrECamParameterNotInRange;
		}
	else 
		{
		//set the ISO type to Manual
		//this function should be called by SetISORateL as well after doing boundary checkings 
		//SetISORate(CCamera::CCameraAdvancedSettings::EISOManual, aRate); 
		iISORateType = CCamera::CCameraAdvancedSettings::EISOManual;			
		iOwner.iIsoRate = aRate;
		
		RetrieveResult(response);
		}
	iOwner.Notify(KUidECamEventCameraSettingIsoRate, response);
	}
	
void CCamAdvSet::GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const
	{
	aSupportedISORateTypes = iSupportedISORateType;	
	}
	
void CCamAdvSet::SetISORate(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam)
	{
	TInt response = KErrNone;
	
	if(!(aISORateType & iSupportedISORateType) || iOwner.iSupportedISORates.Count() == 0)
		{
		response = KErrNotSupported;
		}
	else
		{
		switch(aISORateType)
			{
			case CCamera::CCameraAdvancedSettings::EISOManual:
				{
				if(iOwner.iSupportedISORates.Find(aParam) == KErrNotFound)
					{
					response = KErrECamParameterNotInRange;
					}
				else
					{
					RetrieveResult(response);
					}
				break;
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised:
				{
				RetrieveResult(response);
				break;	
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised:
				{
				if(iOwner.iSupportedISORates.Find(aParam) == KErrNotFound)
					{
					response = KErrECamParameterNotInRange;
					}
				else
					{
					RetrieveResult(response);
					}
				break;
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoShutterSpeedPrioritised:
				{
				RetrieveResult(response);
				break;	
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoAperturePrioritised:
				{
				RetrieveResult(response);
				break;	
				}
			default:
				response = KErrNotSupported;
			}
			
		// this function should be called by SetISORate ,in old API, as well after doing boundary checkings 
		// SetISORate(aISORateType, aParam); 	
		// In real impl, following code should be handled by RunL when server responds to the setting request. Error may also occur.
		iISORateType = aISORateType;
		if(aISORateType == CCamera::CCameraAdvancedSettings::EISOManual)
			{
			if(response == KErrNone)
				{
				iOwner.iIsoRate = aParam;
				}
			}
		else
			{
			if(response == KErrNone)
				{
				iAutoISOIndex = aParam;
				}
			}	
		}	
	
	iOwner.Notify(KUidECamEventCameraSettingIsoRateType, response);
	}

void CCamAdvSet::GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const
	{
	//If camera has no ISO rate supported, then leave
	if(iOwner.iSupportedISORates.Count() == 0)	
		{
		User::Leave(KErrNotSupported);
		}
		
	if(iISORateType == CCamera::CCameraAdvancedSettings::EISONone)
		{
		aISORateType = iISORateType;
		}
	else
		{
		if(iISORateType != CCamera::CCameraAdvancedSettings::EISOManual)
			{
			aParam = iAutoISOIndex;
			
			//if camera is not capable to retrieve actual ISO value in auto ISO modes, KErrNotFound is returned in aISORate.
			if(!iIsCapableActualISOValue)
				{
				aISORate = KErrNotFound;
				}
			else
				{
				aISORate = iOwner.iIsoRate;	
				}
			}
		else
			{
			aISORate = iOwner.iIsoRate;	
			}
		
		aISORateType = iISORateType;	
		}	
	}

void CCamAdvSet::GetAperturesL(RArray<TInt>& /*aFStops*/, TValueInfo& /*aInfo*/) const
	{
	
	}
	
TInt CCamAdvSet::Aperture() const
	{
	return iAperture;
	}

void CCamAdvSet::SetAperture(TInt aFStop)
	{
	TInt response = KErrNone;
	
	iAperture = aFStop;
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingAperture, response);
	}	
	
void CCamAdvSet::GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const
	{
	aShutterSpeeds.Reset();
	aInfo =  ENotActive;
	}
	
TInt CCamAdvSet::ShutterSpeed() const
	{
	return iShutterSpeed;
	}
	
void CCamAdvSet::SetShutterSpeed(TInt aShutterSpeed)
	{
	TInt response = KErrNone;
	if (aShutterSpeed >=0)
		{
		iShutterSpeed = aShutterSpeed;
		
		RetrieveResult(response);
		}
	else 
		{
		response = KErrNotSupported;
		}
	
	iOwner.Notify(KUidECamEventCameraSettingShutterSpeed, response);
	}
	
TInt CCamAdvSet::SupportedMeteringModes() const
	{
	return CCamera::CCameraAdvancedSettings::EMeteringModeAuto;
	}
	
CCamera::CCameraAdvancedSettings::TMeteringMode CCamAdvSet::MeteringMode() const
	{
	return CCamera::CCameraAdvancedSettings::EMeteringModeAuto;
	}
	
void CCamAdvSet::SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode /*aMeteringMode*/)
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingMeteringMode, response);
	}
	
TInt CCamAdvSet::SupportedDriveModes() const
	{
	return iSupportedDriveModes;
	}
	
CCamera::CCameraAdvancedSettings::TDriveMode CCamAdvSet::DriveMode() const
	{
	return iDriveMode;
	}
	
void CCamAdvSet::SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode)
	{
	TInt response = KErrNone;
	
	iDriveMode = aDriveMode;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingDriveMode, response);
	}
	
TInt CCamAdvSet::SupportedBracketModes() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketModeOff;
	}
	
CCamera::CCameraAdvancedSettings::TBracketMode CCamAdvSet::BracketMode() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketModeOff;
	}
	
void CCamAdvSet::SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode /*aBracketMode*/)
	{
	TInt response = KErrNone;
	
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingBracketMode, response);
	}
	
TInt CCamAdvSet::SupportedBracketParameters() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketParameterNone;
	}
	
CCamera::CCameraAdvancedSettings::TBracketParameter CCamAdvSet::BracketParameter() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketParameterNone;
	}
	
void CCamAdvSet::SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter /*aBracketParameter*/)
	{
	}
	
TInt CCamAdvSet::SupportedBracketSteps() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketStepSmall;
	}
	
CCamera::CCameraAdvancedSettings::TBracketStep CCamAdvSet::BracketStep() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketStepSmall;
	}
	
void CCamAdvSet::SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep /*aBracketStep*/)
	{
	
	}
void CCamAdvSet::GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const
	{
	aStartIndex = KBracketStartIndex;
	aFrames     = KBracketFrames;
	}
	
void CCamAdvSet::SetBracketMerge(TInt /*aStartIndex = 0*/, TInt /*aFrames = 2*/)
	{
	
	}
TInt CCamAdvSet::SupportedFlashModes() const
	{
	return (CCamera::EFlashVideoLight << 1) - 1;
	}
	
CCamera::TFlash CCamAdvSet::FlashMode() const
	{
	return CCamera::TFlash(iFlashMode);
	}
	
void CCamAdvSet::SetFlashMode(CCamera::TFlash aMode)
	{
    iFlashMode = aMode;
	}
	
TBool CCamAdvSet::RedEyeReduceOn() const
	{
	return KDefaultRedEyeReduce;
	}
	
void CCamAdvSet::SetRedEyeReduceOn(TBool /*aState*/)
	{
	}
	
void CCamAdvSet::GetFlashCompensationStepsL(RArray<TInt>& /*aFlashCompensationSteps*/, TValueInfo& /*aInfo*/) const
	{
	
	}
	
TInt CCamAdvSet::FlashCompensationStep() const
	{
	return KFlashCompensationStep;
	}

TInt CCamAdvSet::GetFlashCompensationStep(TInt& aFlashCompensationStep) const
	{
	aFlashCompensationStep = KFlashCompensationStep;
	return KErrNone;
	}
	
void CCamAdvSet::GetFlashCompensationRangeInSteps(TInt& /*aNegativeCompensation*/, TInt& /*aPositiveCompensation*/) const
	{
	
	}
void CCamAdvSet::SetFlashCompensationStep(TInt /*aFlashCompensationStep*/)
	{
	
	}
	
TInt CCamAdvSet::FlashCompensation() const
	{
	return KFlashCompensationInSteps;
	}
	
TInt CCamAdvSet::GetFlashCompensation(TInt& aFlashCompensation) const
	{
	aFlashCompensation = KFlashCompensationInSteps;
	return KErrNone;
	}
	
void CCamAdvSet::SetFlashCompensation(TInt /*aFlashCompensation*/)
	{
	}
	
TBool CCamAdvSet::IsExternalFlashPresent() const
	{
	return KExternalFlashPresentState;
	}
	
void CCamAdvSet::GetManualFlashPowerLevelsL(RArray<TInt>& /*aManualFlashPowerLevels*/, TValueInfo& /*aInfo*/) const
	{
	
	}
	
TInt CCamAdvSet::ManualFlashPowerLevel() const
	{
	return KManualFlashPowerLevel;
	}
	
void CCamAdvSet::SetManualFlashPowerLevel(TInt /*aManualFlashPowerLevel*/)
	{
	
	}
	
TInt CCamAdvSet::SupportedExposureModes() const
	{
	return CCamera::EExposureAuto;
	}
	
CCamera::TExposure CCamAdvSet::ExposureMode() const
	{
	return CCamera::EExposureAuto;
	}
	
void CCamAdvSet::SetExposureMode(CCamera::TExposure /*aExposureMode = CCamera::EExposureAuto*/)
	{
	}
	
void CCamAdvSet::GetExposureCompensationStepsL(RArray<TInt>& /*aExposureCompensationSteps*/, TValueInfo& /*aInfo*/) const
	{
	}

void CCamAdvSet::GetExposureCompensationRangeInSteps(TInt& /*aNegativeCompensation*/, TInt& /*aPositiveCompensation*/) const
	{
	}
	
TInt CCamAdvSet::ExposureCompensationStep() const
	{
	return KExposureCompensationStep;
	}
	
TInt CCamAdvSet::GetExposureCompensationStep(TInt& aExposureCompensationStep) const
	{
	aExposureCompensationStep = KExposureCompensationStep;
	return KErrNone;
	}
	
void CCamAdvSet::SetExposureCompensationStep(TInt /*aExposureCompensationStep*/)
	{
	}
	
TInt CCamAdvSet::ExposureCompensation() const
	{
	return KExposureCompensationInSteps;
	}
	
TInt CCamAdvSet::GetExposureCompensation(TInt& aExposureCompensation) const
	{
	aExposureCompensation = KExposureCompensationInSteps;
	return KErrNone;
	}
	
void CCamAdvSet::SetExposureCompensation(TInt /*aExposureCompensation*/)
	{
	}
	
TInt CCamAdvSet::SupportedWhiteBalanceModes() const
	{
	return KWhiteBalanceSupported;
	}
	
CCamera::TWhiteBalance CCamAdvSet::WhiteBalanceMode() const
	{
	return iWhiteBalance;
	}
	
void CCamAdvSet::SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode)
	{
	TInt response = KErrNone;
	
	iWhiteBalance = aWhiteBalanceMode;
	RetrieveResult(response);
	
	iOwner.Notify(KUidECamEventCameraSettingsWBValue, response);
	}
	
TBool CCamAdvSet::ApertureExposureLockOn() const
	{
	return KApertureExposureLockOn;
	}
	
void CCamAdvSet::SetApertureExposureLockOn(TBool /*aAELock*/)
	{
	}
	
TBool CCamAdvSet::ShootClickOn() const
	{
	return KShootClickOn;
	}
	
void CCamAdvSet::SetShootClickOn(TBool /*aShootClickOn*/)
	{
	}
	
void CCamAdvSet::GetTimerIntervalsL(RArray<TInt>& /*aTimerIntervals*/, TValueInfo& /*aInfo*/) const
	{
	}

TInt CCamAdvSet::TimerInterval() const
	{
	return KTimerInterval;
	}
	
void CCamAdvSet::SetTimerInterval(TInt /*aTimerInterval*/)
	{
	}
	
void CCamAdvSet::GetTimeLapsePeriodRange(TTime& /*aTimeLapseMin*/, TTime& /*aTimeLapseMax*/) const
	{
	}
	
void CCamAdvSet::GetTimeLapse(TTime& /*aStart*/, TTime& /*aEnd*/, TTime& /*aInterval*/) const
	{
	}
	
void CCamAdvSet::SetTimeLapse(const TTime& /*aStart*/, const TTime& /*aEnd*/, const TTime& /*aInterval*/)
	{
	}
	
CCamera::CCameraAdvancedSettings::TPictureOrientation CCamAdvSet::PictureOrientation() const
	{
	return CCamera::CCameraAdvancedSettings::EPictureOrientationUnknown;
	}
	
void CCamAdvSet::SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation /*aOrientation*/)
	{
	}
	
TInt CCamAdvSet::SupportedPixelAspectRatios() const
	{
	return (CCamera::CCameraAdvancedSettings::EEPixelAspect40To33 << 1) - 1;
	}
	
CCamera::CCameraAdvancedSettings::TPixelAspectRatio CCamAdvSet::PixelAspectRatio() const
	{
	return CCamera::CCameraAdvancedSettings::TPixelAspectRatio(iPixelAspectRatio);
	}
	
void CCamAdvSet::SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio)
	{
	iPixelAspectRatio = aPixelAspectRatio;
	}
	
TInt CCamAdvSet::SupportedYuvRanges() const
	{
	return CCamera::CCameraAdvancedSettings::EYuvRangeFull;
	}
	
CCamera::CCameraAdvancedSettings::TYuvRange CCamAdvSet::YuvRange() const
	{
	return CCamera::CCameraAdvancedSettings::EYuvRangeFull;
	}
	
void CCamAdvSet::SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange /*aYuvRange*/)
	{
	
	}
TInt CCamAdvSet::BurstImages() const
	{
	return KBurstImages;
	}
	
void CCamAdvSet::SetBurstImages(TInt /*aImages*/)
	{
	}
	
void CCamAdvSet::GetOpticalZoomStepsL(RArray<TInt>& /*aOpticalZoomSteps*/, TValueInfo& /*aInfo*/)  const
	{
	}
	
TInt CCamAdvSet::OpticalZoom() const
	{
	return KECamFineResolutionFactor;
	}
	
void CCamAdvSet::SetOpticalZoom(TInt /*aOpticalZoom*/)
	{
	}
	
void CCamAdvSet::GetDigitalZoomStepsL(RArray<TInt>& /*aDigitalZoomSteps*/,
								 TValueInfo& /*aInfo*/) const
	{
	}
	
void CCamAdvSet::GetDigitalZoomStepsForStillL(RArray<TInt>& /*aDigitalZoomSteps*/, TValueInfo& /*aInfo*/, TInt /*aSizeIndex*/,
								 CCamera::TFormat /*aFormat*/, TBool& /*aIsInfluencePossible*/) const
	{
	}

void CCamAdvSet::GetDigitalZoomStepsForVideoL(RArray<TInt>& /*aDigitalZoomSteps*/, TValueInfo& /*aInfo*/, TInt /*aFrameRateIndex*/,
								 TInt /*aSizeIndex*/, CCamera::TFormat /*aFormat*/, TBool& /*aIsInfluencePossible*/, CCamera::TExposure /*aExposure*/) const
	{
	}

TInt CCamAdvSet::DigitalZoom() const
	{
	return KECamFineResolutionFactor;
	}

void CCamAdvSet::SetDigitalZoom(TInt /*aDigitalZoom*/)
	{
	}
	
TBool CCamAdvSet::ExposureLockOn() const
	{
	return KExposureLockOn;		
	}

void CCamAdvSet::SetExposureLockOn(TBool /*aState*/)
	{
	}
		
TBool CCamAdvSet::AutoFocusLockOn() const
	{
	return KAutoFocusLockOn;
	}

void CCamAdvSet::SetAutoFocusLockOn(TBool /*aState*/)
	{	
	}

void CCamAdvSet::GetSupportedSettingsL(RArray<TUid>& aSettings) const
	{
	aSettings.Reset();
	// if camera is present 
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		aSettings.AppendL(KUidECamEventCameraSettingStabilizationMode);
		aSettings.AppendL(KUidECamEventCameraSettingFocusMode);
		aSettings.AppendL(KUidECamEventCameraSettingIsoRateType);
		}
	}
    
void CCamAdvSet::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const
	{
	aActiveSettings.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		aActiveSettings.AppendL(KUidECamEventCameraSettingFocusMode);
		aActiveSettings.AppendL(KUidECamEventCameraSettingAFAssistantLight);
		}
	}
    
 
void CCamAdvSet::GetDisabledSettingsL(RArray<TUid>& aDisbledSettings) const
	{
	aDisbledSettings.Reset();
	if (iOwner.iCameras[iOwner.iCameraIndex])
		{
		aDisbledSettings.AppendL(KUidECamEventCameraSettingStabilizationMode);
		aDisbledSettings.AppendL(KUidECamEventCameraSettingReferenceScreen);
		}
	}
	    
void CCamAdvSet::SetAutomaticSizeSelectionChangeOn(TBool /*aSetOn = EFalse*/)
	{
	}
	
TBool CCamAdvSet::AutomaticSizeSelectionChangeOn() const
	{
	return KAutomaticSizeSelectionChangeOn;
	}

void CCamAdvSet::GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& /*aTimeouts*/, TValueInfo& /*aInfo*/) const
	{
	}
	
void CCamAdvSet::SetContinuousAutoFocusTimeout(TInt /*aTimeout*/)
	{
	}
	
void CCamAdvSet::SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect /*aEffect*/)
	{
	}
	
CCamera::CCameraAdvancedSettings::TStabilizationEffect CCamAdvSet::StabilizationEffect() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationOff;
	}

TInt CCamAdvSet::SupportedStabilizationEffects() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationOff;
	}

TInt CCamAdvSet::SupportedStabilizationComplexityValues() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationComplexityAuto;
	}
	
CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity CCamAdvSet::StabilizationComplexity() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationComplexityAuto;
	}
	
void CCamAdvSet::SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity /*aComplexity*/)
	{
	}
	
CCamera::CCameraAdvancedSettings::TWBUnits CCamAdvSet::SupportedWBUnits() const
	{
	return CCamera::CCameraAdvancedSettings::EWBColorTemperature;
	}
	
void CCamAdvSet::SetWBRgbValue(const TRgb& /*aValue*/)
	{
	}
	
void CCamAdvSet::GetWBRgbValue(TRgb& /*aValue*/) const
	{
	}
	
void CCamAdvSet::GetWBSupportedColorTemperaturesL(RArray<TInt>& /*aWBColorTemperatures*/, TValueInfo& /*aInfo*/) const
	{
	}
	
void CCamAdvSet::SetWBColorTemperature(TInt /*aColorTemperature*/)
	{
	}

TInt CCamAdvSet::WBColorTemperature() const
	{
	return KWBColorTemperature;
	}

TInt CCamAdvSet::ContinuousAutoFocusTimeout() const
	{
	return KContinuousAutoFocusTimeoutValue;
	}

TInt CCamAdvSet::IsFlashReady(TBool& aReady) const
	{
	aReady = KFlashReadyState;
	return KErrNone;
	}

void CCamAdvSet::GetCurrentFocusModeStepsL(RArray<TInt>& /*aFocusModeSteps*/, TValueInfo& /*aInfo*/) const
	{
	}
	
void CCamAdvSet::SetReferenceScreen(CWsScreenDevice& /*aScreenDevice*/)
	{
	TInt error = KErrNone;
	
	RetrieveResult(error);
	
	iOwner.Notify(KUidECamEventCameraSettingReferenceScreen, error);
	}
	
void CCamAdvSet::GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode /*aCameraMode*/, TInt& /*aPreCaptureWarningSupported*/) const
	{
	}

void CCamAdvSet::SubscribeToPreCaptureWarningL(TInt /*aPreCaptureWarning*/)
	{
	}
	
void CCamAdvSet::UnSubscribePreCaptureWarningL()
	{
	}
	
void CCamAdvSet::GetPreCaptureWarningL(TInt& /*aPreCaptureWarning*/) const
	{
	}
	
void CCamAdvSet::GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const
	{
	aSupportedAFAssistantLight = iSupportedAFAssistantLight;
	}

void CCamAdvSet::GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& aAFAssistantLight) const
	{
	aAFAssistantLight = iAFAssistantLight;
	}

void CCamAdvSet::SetAFAssistantLight(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight)
	{
	TInt error = KErrNone;
	
	iAFAssistantLight = aAFAssistantLight;
	RetrieveResult(error);
	
	iOwner.Notify(KUidECamEventCameraSettingAFAssistantLight, error);
	}

void CCamAdvSet::GetSupportedContinuousZoomTypeL(TUint& aSupportedContinuousZoomType) const
	{
	aSupportedContinuousZoomType = KSupportedContinuousZoomType;	
	}

void CCamAdvSet::GetFocalLengthInfoL(TInt& /*aMinFocalLength*/, TInt& /*aCurrentFocalLength*/, TInt& /*aMaxFocalLength*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamAdvSet::GetNumOperationPreferenceL(TUint& /*aNumOperationPreferenceSupported*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
void CCamAdvSet::EnumerateOperationPreferenceL(TUint /*aOperationPreferenceIndex*/, CCamera::CCameraAdvancedSettings::
						TPerformanceLevel& /*aSpeedLevel*/, CCamera::CCameraAdvancedSettings::TPerformanceLevel& /*aQualityLevel*/, 
						CCamera::CCameraAdvancedSettings::TPerformanceLevel& /*aLowMemoryConsumptionLevel*/, 
						CCamera::CCameraAdvancedSettings::TPerformanceLevel& /*aLowPowerConsumptionLevel*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamAdvSet::SetOperationPreference(TUint /*aOperationPreferenceIndex*/)
	{
	iOwner.Notify(KUidECamEventCameraSettingOperationPreference, KErrNotSupported);
	}
	
	
void CCamAdvSet::GetOperationPreferenceL(TInt& /*aOperationPreferenceIndex*/) const
	{
	User::Leave(KErrNotSupported);		
	}

void CCamAdvSet::GetSupportedEventsL(RArray<TUid>& aSupportedEvents) const
	{
	aSupportedEvents.Reset();
	}
	
void CCamAdvSet::GetIndirectFeatureChangesL(TUid /*aRequestedSetting*/, RArray<TUid>& aIndirectFeatureChanges) const
	{
	aIndirectFeatureChanges.Reset();
	}

void CCamAdvSet::CreateContinuousZoomImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = CContinuousZoomFactory::NewL();
	}

void CCamAdvSet::RetrieveResult(TInt& aError)
	{
	//It is assumed that hardware retrieves this error
	//Assumed that it returns no error
	aError = KErrNone;
	}
/******************************************************/

CCamManagement::CCamManagement(CCamUnitTestPlugin& aOwner): iOwner(aOwner)
	{
	}
	
CCamManagement::~CCamManagement()
	{
	}
	
void CCamManagement::Release()
	{
	delete this;
	}
	
CCamManagement* CCamManagement::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new (ELeave) CCamManagement(aOwner);
	}
	
TBool CCamManagement::PlugCameraIn(TInt aIndex)
	{
	if (aIndex >= KECamSetAvailableCameras)
		{
		return EFalse;
		}
	else
		{
		TInt response = KErrNone;
		TUid uid;
		uid.iUid = (KUidECamEventGlobalCamera00PluggedInUidValue + aIndex);
		iOwner.iCameras[aIndex] = ETrue;
		iOwner.Notify(uid, response);
		}
	return ETrue;
	}

TBool CCamManagement::PlugCameraOut(TInt aIndex)
	{
	if (aIndex >= KECamSetAvailableCameras)
		{
		return EFalse;
		}
	else
		{
		TInt response = KErrNone;
		TUid uid;
		uid.iUid  = KUidECamEventGlobalCamera00PluggedOutUidValue + aIndex;
		iOwner.iCameras[aIndex] = EFalse;
		iOwner.Notify(uid, response);
		}
	return ETrue;
	}


//
//Snapshot factory for Image
CContinuousZoomFactory* CContinuousZoomFactory::NewL()
	{
	CContinuousZoomFactory* self = new(ELeave) CContinuousZoomFactory();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	
void CContinuousZoomFactory::ConstructL()
	{
	}

CContinuousZoomFactory::CContinuousZoomFactory() : iCamContinuousZoomImp(NULL)
	{
	}
	
CContinuousZoomFactory::~CContinuousZoomFactory()
	{
	}
	
TInt CContinuousZoomFactory::GetImpl(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/) const
	{
	return KErrNotSupported;
	}
	
TInt CContinuousZoomFactory::GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraContinuousZoomUidValue:
			{
			CCamera::CCameraAdvancedSettings::TContinuousZoomType zoomType = static_cast<CCamera::CCameraAdvancedSettings::TContinuousZoomType>(aParam1.iIntParam);
			iCamContinuousZoomImp = CCamContinuousZoom::NewL(zoomType);
			aIfPtr = static_cast<MCameraContinuousZoom*>(iCamContinuousZoomImp);
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CContinuousZoomFactory::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CContinuousZoomFactory::Release()
	{
	delete this;
	}


CCamContinuousZoom* CCamContinuousZoom::NewL(CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType)
	{
	CCamContinuousZoom* self = new (ELeave) CCamContinuousZoom(aContinuousZoomType);
	return self;
	}

CCamContinuousZoom::CCamContinuousZoom(CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType)
	: iZoomType(aContinuousZoomType)
	{
	iInfo.iMaxSpeedSupported = 10;
	iInfo.iMinAccelerationSupported = 0;
	iInfo.iMaxAccelerationSupported = 0;
	iInfo.iContinuousZoomMinLimit = 0;
	iInfo.iContinuousZoomMaxLimit = 10;
	}

CCamContinuousZoom::~CCamContinuousZoom()
	{
	}

void CCamContinuousZoom::SetContinuousZoomObserverAndHandle(MContinuousZoomObserver& aObserver, CCamera::CCameraContinuousZoom* aContinuousZoomHandle)
	{
	iObserver = &aObserver;
	iContinuousZoomHandle = aContinuousZoomHandle;
	}

void CCamContinuousZoom::StartContinuousZoomL(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters aContinuousZoomParameters)
	{
	if(aContinuousZoomParameters.Size() != sizeof(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters) ||
			aContinuousZoomParameters.Version() != KContinuousZoomParametersCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}

	if(aContinuousZoomParameters.iContinuousZoomSpeed > iInfo.iMaxSpeedSupported || 
			aContinuousZoomParameters.iContinuousZoomAcceleration < iInfo.iMinAccelerationSupported || 
			aContinuousZoomParameters.iContinuousZoomAcceleration > iInfo.iMaxAccelerationSupported || 
			aContinuousZoomParameters.iContinuousZoomLimit < iInfo.iContinuousZoomMinLimit ||
			aContinuousZoomParameters.iContinuousZoomLimit > iInfo.iContinuousZoomMaxLimit)
		{
		User::Leave(KErrArgument);
		}
	
	iParameters = aContinuousZoomParameters;
	for(TInt i = 0; i < iParameters.iContinuousZoomLimit; i++)
		{
		iObserver->ContinuousZoomProgress(*iContinuousZoomHandle, i, KErrNone);
		}
	
	iObserver->ContinuousZoomCompleted(*iContinuousZoomHandle, iParameters.iContinuousZoomLimit, KErrNone);
	}

void CCamContinuousZoom::StopContinuousZoom()
	{
	return;
	}

void CCamContinuousZoom::GetContinuousZoomSupportInfoL(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo& aContinuousZoomInfo) const
	{
	if(aContinuousZoomInfo.Size() != sizeof(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo) ||
			aContinuousZoomInfo.Version() != KContinuousZoomSupportInfoCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}

	aContinuousZoomInfo = iInfo;
	}

void CCamContinuousZoom::GetContinuousZoomId(TInt& aZoomId) const
	{
	aZoomId = (TInt)(this);
	}

void CCamContinuousZoom::Release()
	{
	delete this;
	}
