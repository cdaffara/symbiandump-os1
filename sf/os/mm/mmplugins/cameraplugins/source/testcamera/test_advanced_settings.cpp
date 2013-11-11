// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// testadvancedsettings.cpp
// 
//

#include "test_advanced_settings.h"
#include <ecamerrors.h>
#include "ecamversion.h"

CTestCamAdvSet::CTestCamAdvSet(CTestCamera& aOwner): iOwner(aOwner)
	{
	Init();
	}
	
CTestCamAdvSet::~CTestCamAdvSet()
	{
	iSupportedISORates.Close();
	iSupportedApertures.Close();
	iSupportedShutterSpeeds.Close();
	
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());	
	if(globalData != NULL)
		{
		if(!globalData->iTestCamPresets && !globalData->iTestCamSnapshot && !globalData->iTestCamImgProc)
			{
			delete globalData;
			Dll::FreeTls();
			}	
		else
			{
			globalData->iTestCamAdvSet = NULL;	
			Dll::SetTls(globalData);
			}
		}
	}
		
CTestCamAdvSet* CTestCamAdvSet::NewL(CTestCamera& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(globalData == NULL)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		globalData->iAdvSetReferenceCount = 0;
		globalData->iTestCamAdvSet = new (ELeave) CTestCamAdvSet(aOwner);
		CleanupStack::PushL(globalData->iTestCamAdvSet);
		globalData->iTestCamAdvSet->ConstructL();
		globalData->iTestCamAdvSet->iRefCount = 1;
		User::LeaveIfError(Dll::SetTls(globalData));
		CleanupStack::Pop(globalData->iTestCamAdvSet);
		CleanupStack::Pop(globalData);
		return globalData->iTestCamAdvSet;
		}
	else
		{
		if(globalData->iTestCamAdvSet == NULL)
			{
			globalData->iAdvSetReferenceCount = 0;
			globalData->iTestCamAdvSet = new (ELeave) CTestCamAdvSet(aOwner);
			CleanupStack::PushL(globalData->iTestCamAdvSet);
			globalData->iTestCamAdvSet->ConstructL();
			globalData->iTestCamAdvSet->iRefCount = 1;
			User::LeaveIfError(Dll::SetTls(globalData));
			CleanupStack::Pop(globalData->iTestCamAdvSet);
			return globalData->iTestCamAdvSet;
			}
			
		CTestCamAdvSet* self = globalData->iTestCamAdvSet;
		
		globalData->iAdvSetReferenceCount++;
		self->iRefCount = globalData->iAdvSetReferenceCount + 1;
		if (globalData->iAdvSetReferenceCount == KNumOfAdvSetExtensions-1 )
			{
			globalData->iTestCamAdvSet = NULL;
			if(!globalData->iTestCamPresets && !globalData->iTestCamSnapshot && !globalData->iTestCamImgProc)
				{
				delete globalData;
				Dll::FreeTls();
				}
			else
				{
				User::LeaveIfError(Dll::SetTls(globalData));
				}
			}
		else
			{
			User::LeaveIfError(Dll::SetTls(globalData));
			}
		return self;		
		}
	}

void CTestCamAdvSet::Release()
	{
	iRefCount--; 
	if(iRefCount == 0)
		{
		iOwner.iAdvSettingsImpl = NULL;
		delete this;
		}
	}	
	
void CTestCamAdvSet::Init()
	{
	for(TUint i=0; i< KNumAvailableCameras; i++)
		{
		iCameraTypes[i] = CCamera::CCameraAdvancedSettings::ECameraPluggable;
		}
	
	iFocusDistance = KDefaultFocusDistance;
	iIsoRate = KDefaultIsoRate;
	iISORateType = CCamera::CCameraAdvancedSettings::EISOManual;
	iAperture = KDefaultAperture;
	iShutterSpeed = KDefaultShutterSpeed;
	}
	
void CTestCamAdvSet::ConstructL()
	{
	iSupportedISORates.Reset();
	iSupportedISORates.AppendL(KDefaultIsoRate);
	
	iSupportedApertures.Reset();
	iSupportedApertures.AppendL(KDefaultAperture);
	
	iSupportedShutterSpeeds.Reset();
	iSupportedShutterSpeeds.AppendL(KDefaultShutterSpeed);
	}

CCamera::CCameraAdvancedSettings::TCameraType CTestCamAdvSet::CameraType() const
	{
	return CCamera::CCameraAdvancedSettings::ECameraPluggable;	
	}

CCamera::CCameraAdvancedSettings::TCameraType CTestCamAdvSet::CameraType(TInt aCameraIndex) const
	{
	if (aCameraIndex >= KNumAvailableCameras || aCameraIndex < 0)
		{
		return CCamera::CCameraAdvancedSettings::ECameraUnknown;	
		}
	else 
		{
		return iCameraTypes[aCameraIndex];	
		}
	}
	
TBool CTestCamAdvSet::IsCameraPresent() const
	{
	return ETrue;
	}

TBool CTestCamAdvSet::IsCameraPresent(TInt aCameraIndex) const
	{
	if (aCameraIndex >= KNumAvailableCameras || aCameraIndex < 0)
		{
		return EFalse;	
		}
	else 
		{
		return ETrue;	
		}
	}
	
TInt CTestCamAdvSet::CameraIndex() const
	{
	return iOwner.iCameraIndex;
	}
	
TInt CTestCamAdvSet::SupportedStabilizationModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TStabilizationMode CTestCamAdvSet::StabilizationMode() const
	{
	return CCamera::CCameraAdvancedSettings::EStabilizationModeOff;
	}
	
void CTestCamAdvSet::SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode aStabilizationMode)
	{	
	TInt eventError = KErrNotSupported;
	if(aStabilizationMode == CCamera::CCameraAdvancedSettings::EStabilizationModeOff)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingStabilizationMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedFocusModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TFocusMode CTestCamAdvSet::FocusMode() const
	{
	return CCamera::CCameraAdvancedSettings::EFocusModeAuto;	
	}
	
void CTestCamAdvSet::SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode)
	{
	TInt eventError = KErrNotSupported;
	if(aFocusMode == CCamera::CCameraAdvancedSettings::EFocusModeAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingFocusMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedFocusRanges() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TFocusRange CTestCamAdvSet::FocusRange() const
	{
	return CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
	}
	
void CTestCamAdvSet::SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange)
	{
	TInt eventError = KErrNotSupported;
	if(aFocusRange == CCamera::CCameraAdvancedSettings::EFocusRangeAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent1(KUidECamEventCameraSettingFocusRange2, eventError);
	TECAMEvent ecamevent2(KUidECamEventCameraSettingFocusRange, eventError);
	
	iOwner.iECamEvent = ecamevent1;
	iOwner.iHandleEventAsync.CallBack();
	
	iOwner.iECamEvent = ecamevent2;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedAutoFocusTypes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TAutoFocusType CTestCamAdvSet::AutoFocusType() const
	{
    return CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff;
	}
	
void CTestCamAdvSet::SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType)
	{
	TInt eventError = KErrNotSupported;
	if(aAutoFocusType == CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent1(KUidECamEventCameraSettingAutoFocusType2, eventError);
	TECAMEvent ecamevent2(KUidECamEventCameraSettingAutoFocusType, eventError);
	
	iOwner.iECamEvent = ecamevent1;
	iOwner.iHandleEventAsync.CallBack();
	
	iOwner.iECamEvent = ecamevent2;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedAutoFocusAreas() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TAutoFocusArea CTestCamAdvSet::AutoFocusArea() const
	{
	return CCamera::CCameraAdvancedSettings::EAutoFocusTypeAuto;
	}
	
void CTestCamAdvSet::SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea aAutoFocusArea)
	{
	TInt eventError = KErrNotSupported;
	if(aAutoFocusArea == CCamera::CCameraAdvancedSettings::EAutoFocusTypeAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingAutoFocusArea, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::FocusDistance() const
	{
	return iFocusDistance;
	}
	
void CTestCamAdvSet::SetFocusDistance(TInt aDistance) 
	{
	TInt eventError = KErrNone;
	if(aDistance < 0)
		{
		eventError = KErrNotSupported;
		}
	else
		{
		iFocusDistance = aDistance;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingFocusDistance, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::GetMinFocalLength() const
	{
	return KErrNotSupported;
	}
	
void CTestCamAdvSet::GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const
	{
	if(iSupportedISORates.Count() == 0)
		{
		User::Leave(KErrNotSupported);
		}
	
	for(TInt index=0; index < iSupportedISORates.Count(); index++)
		{
		aSupportedIsoRates.AppendL(iSupportedISORates[index]);
		}
	}
	
TInt CTestCamAdvSet::IsoRate() const
	{
	return iIsoRate; 
	}

void CTestCamAdvSet::SetIsoRate(TInt aRate)
	{
	TInt eventError = KErrNone;
	if(iSupportedISORates.Find(aRate) == KErrNotFound)
		{
		eventError = KErrECamParameterNotInRange;
		}
	else 
		{
		//set the ISO type to Manual
		iISORateType = CCamera::CCameraAdvancedSettings::EISOManual;			
		iIsoRate = aRate;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingIsoRate, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}

void CTestCamAdvSet::GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const
	{
	aSupportedISORateTypes = KSupportedISORateTypes;	
	}
		
void CTestCamAdvSet::SetISORate(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam)
	{
	TInt eventError = KErrNone;
	
	if(!(aISORateType & KSupportedISORateTypes) || iSupportedISORates.Count() == 0)
		{
		eventError = KErrNotSupported;
		}
	else
		{
		switch(aISORateType)
			{
			case CCamera::CCameraAdvancedSettings::EISOManual:
				{
				if(iSupportedISORates.Find(aParam) == KErrNotFound)
					{
					eventError = KErrECamParameterNotInRange;
					}
				else
					{
					iISORateType = aISORateType;
					if(aParam == iIsoRate)
						{
						eventError = KErrAlreadyExists;
						}
					else
						{
						iIsoRate = aParam;
						}
					}
				break;
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised:
				{
				eventError = KErrNotSupported;
				break;	
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised:
				{
				eventError = KErrNotSupported;
				break;
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoShutterSpeedPrioritised:
				{
				eventError = KErrNotSupported;
				break;	
				}
			case CCamera::CCameraAdvancedSettings::EISOAutoAperturePrioritised:
				{
				eventError = KErrNotSupported;
				break;	
				}
			default:
				eventError = KErrNotSupported;
			}
		}
			
	TECAMEvent ecamevent(KUidECamEventCameraSettingIsoRateType, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}

void CTestCamAdvSet::GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& /*aParam*/, TInt& aISORate) const
	{
	//If camera has no ISO rate supported, then leave
	if(iSupportedISORates.Count() == 0)	
		{
		User::Leave(KErrNotSupported);
		}
		
	aISORateType = iISORateType;
	aISORate = iIsoRate;
//	aParam is redundant for manual ISO.
	}
	
void CTestCamAdvSet::GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const
	{
	if(iSupportedApertures.Count() == 0)
		{
		aFStops.Reset();
		aInfo = ENotActive;
		}
	
	for(TInt index=0; index < iSupportedApertures.Count(); index++)
		{
		aFStops.AppendL(iSupportedApertures[index]);
		}
		
	aInfo = EDiscreteSteps;
	}
	
TInt CTestCamAdvSet::Aperture() const
	{
	return iAperture;
	}

void CTestCamAdvSet::SetAperture(TInt aFStop)
	{	
	TInt eventError = KErrNone;
	if(iSupportedApertures.Find(aFStop) == KErrNotFound)
		{
		eventError = KErrECamParameterNotInRange;
		}
	else 
		{
		iAperture = aFStop;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingAperture, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}	
	
void CTestCamAdvSet::GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const
	{
	if(iSupportedShutterSpeeds.Count() == 0)
		{
		aShutterSpeeds.Reset();
		aInfo = ENotActive;
		}
	
	for(TInt index=0; index < iSupportedShutterSpeeds.Count(); index++)
		{
		aShutterSpeeds.AppendL(iSupportedShutterSpeeds[index]);
		aInfo = EDiscreteSteps;
		}
	}
	
TInt CTestCamAdvSet::ShutterSpeed() const
	{
	return iShutterSpeed;
	}
	
void CTestCamAdvSet::SetShutterSpeed(TInt aShutterSpeed)
	{
	TInt eventError = KErrNone;
	if(iSupportedShutterSpeeds.Find(aShutterSpeed) == KErrNotFound)
		{
		eventError = KErrECamParameterNotInRange;
		}
	else 
		{
		iShutterSpeed = aShutterSpeed;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingShutterSpeed, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedMeteringModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TMeteringMode CTestCamAdvSet::MeteringMode() const
	{
	return CCamera::CCameraAdvancedSettings::EMeteringModeAuto;
	}
	
void CTestCamAdvSet::SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode aMeteringMode)
	{
	TInt eventError = KErrNotSupported;
	if(aMeteringMode == CCamera::CCameraAdvancedSettings::EMeteringModeAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingMeteringMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedDriveModes() const
	{
	return KSupportedDriveModes;
	}
	
CCamera::CCameraAdvancedSettings::TDriveMode CTestCamAdvSet::DriveMode() const
	{
	return iOwner.iDriveMode;
	}
	
void CTestCamAdvSet::SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode)
	{
	TInt eventError = KErrNone;
	if(iOwner.iVideoCaptureActive || iOwner.iImageCaptureActive)
		{
		eventError = KErrNotReady;
		}
	else
		{
		if(!(aDriveMode & KSupportedDriveModes))
			{
			eventError = KErrNotSupported;
			}	
		else
			{
			if(eventError == KErrNone)
				{
				iOwner.iDriveMode = aDriveMode;
				}	
			}
		}
	
	TECAMEvent ecamevent(KUidECamEventCameraSettingDriveMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedBracketModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TBracketMode CTestCamAdvSet::BracketMode() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketModeOff;
	}
	
void CTestCamAdvSet::SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode aBracketMode)
	{
	TInt eventError = KErrNotSupported;
	if(aBracketMode == CCamera::CCameraAdvancedSettings::EBracketModeOff)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingBracketMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedBracketParameters() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TBracketParameter CTestCamAdvSet::BracketParameter() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketParameterNone;
	}
	
void CTestCamAdvSet::SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter aBracketParameter)
	{
	TInt eventError = KErrNotSupported;
	if(aBracketParameter == CCamera::CCameraAdvancedSettings::EBracketParameterNone)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingBracketParameter, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedBracketSteps() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::CCameraAdvancedSettings::TBracketStep CTestCamAdvSet::BracketStep() const
	{
	return CCamera::CCameraAdvancedSettings::EBracketStepNonConfig;
	}
	
void CTestCamAdvSet::SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep aBracketStep)
	{
	TInt eventError = KErrNotSupported;
	if(aBracketStep == CCamera::CCameraAdvancedSettings::EBracketStepNonConfig)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingBracketStep, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const
	{
	aStartIndex = KFeatureNotSupported;
	aFrames     = KFeatureNotSupported;
	}
	
void CTestCamAdvSet::SetBracketMerge(TInt /*aStartIndex*/ , TInt /*aFrames*/)
	{
	TECAMEvent ecamevent(KUidECamEventBracketMerge, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedFlashModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::TFlash CTestCamAdvSet::FlashMode() const
	{
	return CCamera::EFlashNone;
	}
	
void CTestCamAdvSet::SetFlashMode(CCamera::TFlash /*aMode*/)
	{
    TECAMEvent ecamevent(KUidECamEventCameraSettingFlashMode, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::RedEyeReduceOn() const
	{
	return KDefaultRedEyeReduce;
	}
	
void CTestCamAdvSet::SetRedEyeReduceOn(TBool /*aState*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingFlashRedEyeReduce, KErrNotSupported);
						 
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const
	{
	aFlashCompensationSteps.Reset();
	aInfo = ENotActive;
	}
	
TInt CTestCamAdvSet::FlashCompensationStep() const
	{
	return KFlashCompensationStep;
	}

TInt CTestCamAdvSet::GetFlashCompensationStep(TInt& aFlashCompensationStep) const
	{
	aFlashCompensationStep = KFlashCompensationStep;
	return KErrNone;
	}
	
void CTestCamAdvSet::GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const
	{
	aNegativeCompensation = KFlashCompensationStep;
	aPositiveCompensation = KFlashCompensationStep;
	}
	
void CTestCamAdvSet::SetFlashCompensationStep(TInt /*aFlashCompensationStep*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingFlashCompensationStep, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::FlashCompensation() const
	{
	return KFlashCompensationStep;
	}
	
TInt CTestCamAdvSet::GetFlashCompensation(TInt& aFlashCompensation) const
	{
	aFlashCompensation = KFlashCompensationStep;
	return KErrNone;
	}
	
void CTestCamAdvSet::SetFlashCompensation(TInt /*aFlashCompensation*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingFlashCompensation, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::IsExternalFlashPresent() const
	{
	return KExternalFlashPresentState;
	}
	
void CTestCamAdvSet::GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const
	{
	aManualFlashPowerLevels.Reset();
	aInfo = ENotActive;
	}
	
TInt CTestCamAdvSet::ManualFlashPowerLevel() const
	{
	return KErrNotSupported;
	}
	
void CTestCamAdvSet::SetManualFlashPowerLevel(TInt /*aManualFlashPowerLevel*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingFlashManualPower, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedExposureModes() const
	{
	return CCamera::EExposureAuto;
	}
	
CCamera::TExposure CTestCamAdvSet::ExposureMode() const
	{
	return CCamera::EExposureAuto;
	}
	
void CTestCamAdvSet::SetExposureMode(CCamera::TExposure aExposureMode)
	{
	TInt eventError = KErrNotSupported;
	if(aExposureMode == CCamera::EExposureAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingExposureMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const
	{
	aExposureCompensationSteps.Reset();
	aInfo = ENotActive;
	}

void CTestCamAdvSet::GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const
	{
	aNegativeCompensation = KExposureCompensationStep;
	aPositiveCompensation = KExposureCompensationStep;
	}
	
TInt CTestCamAdvSet::ExposureCompensationStep() const
	{
	return KExposureCompensationStep;
	}
	
TInt CTestCamAdvSet::GetExposureCompensationStep(TInt& aExposureCompensationStep) const
	{
	aExposureCompensationStep = KExposureCompensationStep;
	return KErrNone;
	}
	
void CTestCamAdvSet::SetExposureCompensationStep(TInt /*aExposureCompensationStep*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingExposureCompensationStep, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::ExposureCompensation() const
	{
	return KExposureCompensationStep;
	}
	
TInt CTestCamAdvSet::GetExposureCompensation(TInt& aExposureCompensation) const
	{
	aExposureCompensation = KExposureCompensationStep;
	return KErrNone;
	}
	
void CTestCamAdvSet::SetExposureCompensation(TInt /*aExposureCompensation*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingExposureCompensation, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedWhiteBalanceModes() const
	{
	return KFeatureNotSupported;
	}
	
CCamera::TWhiteBalance CTestCamAdvSet::WhiteBalanceMode() const
	{
	return CCamera::EWBAuto;
	}
	
void CTestCamAdvSet::SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode)
	{
	TInt eventError = KErrNotSupported;
	if(aWhiteBalanceMode == CCamera::EWBAuto)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingWhiteBalanceMode, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::ApertureExposureLockOn() const
	{
	return KApertureExposureLockOn;
	}
	
void CTestCamAdvSet::SetApertureExposureLockOn(TBool /*aAELock*/)
	{
	TECAMEvent ecamevent(KUidECamEventAELock, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::ShootClickOn() const
	{
	return KShootClickOn;
	}
	
void CTestCamAdvSet::SetShootClickOn(TBool /*aShootClickOn*/)
	{
	TECAMEvent ecamevent(KUidECamEventSoundClick, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const
	{
	aTimerIntervals.Reset();
	aInfo = ENotActive;
	}

TInt CTestCamAdvSet::TimerInterval() const
	{
	return KErrNotSupported;
	}
	
void CTestCamAdvSet::SetTimerInterval(TInt /*aTimerInterval*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingTimerInterval, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const
	{
	TTime timeLapse(KTimeLapse);
	aTimeLapseMin = aTimeLapseMax = timeLapse;
	}
	
void CTestCamAdvSet::GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const
	{
	TTime timeLapse(KTimeLapse);
	aStart = aEnd = aInterval = timeLapse;
	}
	
void CTestCamAdvSet::SetTimeLapse(const TTime& /*aStart*/, const TTime& /*aEnd*/, const TTime& /*aInterval*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingTimeLapse, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
CCamera::CCameraAdvancedSettings::TPictureOrientation CTestCamAdvSet::PictureOrientation() const
	{
	return CCamera::CCameraAdvancedSettings::EPictureOrientationUnknown;
	}
	
void CTestCamAdvSet::SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation aOrientation)
	{
	TInt eventError = KErrNotSupported;
	
	if(iOwner.iVideoCaptureActive || iOwner.iImageCaptureActive)
		{
		eventError = KErrNotReady;
		}
	else
		{
		if(aOrientation == CCamera::CCameraAdvancedSettings::EPictureOrientationUnknown)
			{
			eventError = KErrAlreadyExists;
			}	
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingPictureOrientation, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedPixelAspectRatios() const
	{
	return KSupportedPixelAspectRatio;
	}
	
CCamera::CCameraAdvancedSettings::TPixelAspectRatio CTestCamAdvSet::PixelAspectRatio() const
	{
	return CCamera::CCameraAdvancedSettings::EPixelAspectUnknown;
	}
	
void CTestCamAdvSet::SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio)
	{
	TInt eventError = KErrNotSupported;
	
	if(aPixelAspectRatio == CCamera::CCameraAdvancedSettings::EPixelAspectUnknown)
		{
		eventError = KErrAlreadyExists;
		}	
		
	TECAMEvent ecamevent(KUidECamEventPixelAspectRatio, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::SupportedYuvRanges() const
	{
	return KSupportedYuvRange;
	}
	
CCamera::CCameraAdvancedSettings::TYuvRange CTestCamAdvSet::YuvRange() const
	{
	return CCamera::CCameraAdvancedSettings::EYuvRangeUnknown;
	}
	
void CTestCamAdvSet::SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange aYuvRange)
	{
	TInt eventError = KErrNotSupported;
	
	if(aYuvRange == CCamera::CCameraAdvancedSettings::EYuvRangeUnknown)
		{
		eventError = KErrAlreadyExists;
		}	
		
	TECAMEvent ecamevent(KUidECamEventYuvRange, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TInt CTestCamAdvSet::BurstImages() const
	{
	return iOwner.iBurstImages;
	}
	
void CTestCamAdvSet::SetBurstImages(TInt aImages)
	{
	TInt eventError = iOwner.CheckReserveAndPower();
	
	if(eventError == KErrNone)
		{
		if(aImages == iOwner.iBurstImages)
			{
			eventError = KErrAlreadyExists;
			}
		else
			{
			if(aImages<1)
				{
				eventError = KErrArgument;
				}
			}
		}
		
	if(eventError != KErrNone)
		{
		TECAMEvent ecamevent(KUidECamEventBurstImages, eventError);
	
		iOwner.iECamEvent = ecamevent;
		iOwner.iHandleEventAsync.CallBack();
		return;
		}
		
	if(aImages > KMaxBurstImages)
		{
		iOwner.iBurstImages = KMaxBurstImages;
		}
	else
		{
		iOwner.iBurstImages = aImages;
		}
			
	TECAMEvent ecamevent(KUidECamEventBurstImages, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo)  const
	{	
	aOpticalZoomSteps.Reset();
	aOpticalZoomSteps.AppendL(KECamFineResolutionFactor);
	aInfo = EDiscreteSteps;
	}
	
TInt CTestCamAdvSet::OpticalZoom() const
	{
	return KECamFineResolutionFactor;
	}
	
void CTestCamAdvSet::SetOpticalZoom(TInt aOpticalZoom)
	{
	TInt eventError = KErrNotSupported;
	
	if(aOpticalZoom == KECamFineResolutionFactor)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingOpticalZoom, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps,
								 TValueInfo& aInfo) const
	{
	aDigitalZoomSteps.Reset();
	aDigitalZoomSteps.AppendL(KECamFineResolutionFactor);
	aInfo = EDiscreteSteps;
	}
	
void CTestCamAdvSet::GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt /*aSizeIndex*/,
								 CCamera::TFormat /*aFormat*/, TBool& /*aIsInfluencePossible*/) const
	{
	aDigitalZoomSteps.Reset();
	aDigitalZoomSteps.AppendL(KECamFineResolutionFactor);
	aInfo = EDiscreteSteps;
	}

void CTestCamAdvSet::GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt /*aFrameRateIndex*/,
								 TInt /*aSizeIndex*/, CCamera::TFormat /*aFormat*/, TBool& /*aIsInfluencePossible*/, CCamera::TExposure /*aExposure*/) const
	{
	aDigitalZoomSteps.Reset();
	aDigitalZoomSteps.AppendL(KECamFineResolutionFactor);
	aInfo = EDiscreteSteps;
	}

TInt CTestCamAdvSet::DigitalZoom() const
	{
	return KECamFineResolutionFactor;
	}

void CTestCamAdvSet::SetDigitalZoom(TInt aDigitalZoom)
	{
	TInt eventError = KErrNotSupported;
	
	if(aDigitalZoom == KECamFineResolutionFactor)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingDigitalZoom, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::ExposureLockOn() const
	{
	return KExposureLockOn;		
	}

void CTestCamAdvSet::SetExposureLockOn(TBool aState)
	{
	TInt eventError = KErrNotSupported;
	
	if(aState == KExposureLockOn)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingExposureLock, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();	
	}
		
TBool CTestCamAdvSet::AutoFocusLockOn() const
	{
	return KAutoFocusLockOn;
	}

void CTestCamAdvSet::SetAutoFocusLockOn(TBool aState)
	{	
	TInt eventError = KErrNotSupported;
	
	if(aState == KAutoFocusLockOn)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingAutoFocusLock, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}

void CTestCamAdvSet::GetSupportedSettingsL(RArray<TUid>& aSettings) const
	{
	aSettings.Reset();
	User::Leave(KErrNotSupported);
	}
    
void CTestCamAdvSet::GetActiveSettingsL(RArray<TUid>& aActiveSettings) const
	{
	aActiveSettings.Reset();
	User::Leave(KErrNotSupported);
	}
 
void CTestCamAdvSet::GetDisabledSettingsL(RArray<TUid>& aDisbledSettings) const
	{
	aDisbledSettings.Reset();
	User::Leave(KErrNotSupported);
	}
	    
void CTestCamAdvSet::SetAutomaticSizeSelectionChangeOn(TBool aSetOn)
	{
	TInt eventError = KErrNotSupported;
	
	if(aSetOn == KAutomaticSizeSelectionChangeOn)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingAutomaticSizeSelection, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
TBool CTestCamAdvSet::AutomaticSizeSelectionChangeOn() const
	{
	return KAutomaticSizeSelectionChangeOn;
	}

void CTestCamAdvSet::GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const
	{
	aTimeouts.Reset();
	aInfo = ENotActive;
	}
	
void CTestCamAdvSet::SetContinuousAutoFocusTimeout(TInt /*aTimeout*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingsContinuousAutoFocusTimeout, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect aEffect)
	{
	TInt eventError = KErrNotSupported;
	
	if(aEffect == KStabilizationEffect)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingsStabilizationEffect, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
CCamera::CCameraAdvancedSettings::TStabilizationEffect CTestCamAdvSet::StabilizationEffect() const
	{
	return KStabilizationEffect;
	}

TInt CTestCamAdvSet::SupportedStabilizationEffects() const
	{
	return KStabilizationEffect;
	}

TInt CTestCamAdvSet::SupportedStabilizationComplexityValues() const
	{
	return KStabilizationComplexity; 
	}
	
CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity CTestCamAdvSet::StabilizationComplexity() const
	{
	return KStabilizationComplexity;
	}
	
void CTestCamAdvSet::SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity aComplexity)
	{
	TInt eventError = KErrNotSupported;
	
	if(aComplexity == KStabilizationComplexity)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventSettingsStabilizationAlgorithmComplexity, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
CCamera::CCameraAdvancedSettings::TWBUnits CTestCamAdvSet::SupportedWBUnits() const
	{
	return KSupportedWBUnits;
	}
	
void CTestCamAdvSet::SetWBRgbValue(const TRgb& /*aValue*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingsWBValue, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetWBRgbValue(TRgb& aValue) const
	{
	aValue = KRgbWhite;
	}
	
void CTestCamAdvSet::GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const
	{
	aWBColorTemperatures.Reset();
	aInfo = ENotActive;
	}
	
void CTestCamAdvSet::SetWBColorTemperature(TInt /*aColorTemperature*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingsWBValue, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}

TInt CTestCamAdvSet::WBColorTemperature() const
	{
	return KErrNotSupported;
	}

TInt CTestCamAdvSet::ContinuousAutoFocusTimeout() const
	{
	return KErrNotSupported;
	}

TInt CTestCamAdvSet::IsFlashReady(TBool& aReady) const
	{
	aReady = KFlashReadyState;
	return KErrNone;
	}

void CTestCamAdvSet::GetCurrentFocusModeStepsL(RArray<TInt>& aFocusModeSteps, TValueInfo& aInfo) const
	{
	aFocusModeSteps.Reset();
	aInfo = ENotActive;
	}
	
void CTestCamAdvSet::SetReferenceScreen(CWsScreenDevice& /*aScreenDevice*/)
	{
	TECAMEvent ecamevent(KUidECamEventCameraSettingReferenceScreen, KErrNotSupported);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
void CTestCamAdvSet::GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode /*aCameraMode*/, TInt& aPreCaptureWarningSupported) const
	{
	aPreCaptureWarningSupported = KPreCaptureWarning; 
	}

void CTestCamAdvSet::SubscribeToPreCaptureWarningL(TInt /*aPreCaptureWarning*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamAdvSet::UnSubscribePreCaptureWarningL()
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamAdvSet::GetPreCaptureWarningL(TInt& /*aPreCaptureWarning*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CTestCamAdvSet::GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const
	{
	aSupportedAFAssistantLight = KAFAssisstantLight;
	}

void CTestCamAdvSet::GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& /*aAFAssistantLight*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CTestCamAdvSet::SetAFAssistantLight(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight)
	{
	TInt eventError = KErrNotSupported;
	
	if(aAFAssistantLight == KAFAssisstantLight)
		{
		eventError = KErrAlreadyExists;
		}
		
	TECAMEvent ecamevent(KUidECamEventCameraSettingAFAssistantLight, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();
	}
	
/**********************************************************************************************************/
/* 											CTestCamPresets  											  */
/**********************************************************************************************************/

CTestCamPresets::CTestCamPresets(CTestCamera& aOwner): iOwner(aOwner),iCurrentPreset(KNullUid)
	{}
	
CTestCamPresets::~CTestCamPresets()
	{
	iSupportedPresets.Close();
	iFactoryPresetAffectSettings.Close();
	
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());	
	if(globalData != NULL)
		{
		if(!globalData->iTestCamAdvSet && !globalData->iTestCamSnapshot && !globalData->iTestCamImgProc)
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			globalData->iTestCamPresets = NULL;
			Dll::SetTls(globalData);	
			}
		}
	}
		
CTestCamPresets* CTestCamPresets::NewL(CTestCamera& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(globalData == NULL)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		globalData->iPresetsReferenceCount = 0;
		globalData->iTestCamPresets = new (ELeave) CTestCamPresets(aOwner);
		globalData->iTestCamPresets->ConstructL();
		globalData->iTestCamPresets->iRefCount = 1;
		User::LeaveIfError(Dll::SetTls(globalData));
		CleanupStack::Pop(globalData);
		return globalData->iTestCamPresets;
		}
	else
		{
		if(globalData->iTestCamPresets == NULL)
			{
			globalData->iPresetsReferenceCount = 0;
			globalData->iTestCamPresets = new (ELeave) CTestCamPresets(aOwner);
			CleanupStack::PushL(globalData->iTestCamPresets);
			globalData->iTestCamPresets->ConstructL();
			globalData->iTestCamPresets->iRefCount = 1;
			User::LeaveIfError(Dll::SetTls(globalData));
			CleanupStack::Pop(globalData->iTestCamPresets);
			return globalData->iTestCamPresets;
			}
			
		CTestCamPresets* self = globalData->iTestCamPresets;
		
		globalData->iPresetsReferenceCount++;
		self->iRefCount = globalData->iPresetsReferenceCount + 1;
		if (globalData->iPresetsReferenceCount == KNumOfPresetExtensions-1 )
			{
			globalData->iTestCamPresets = NULL;
			if(!globalData->iTestCamAdvSet && !globalData->iTestCamSnapshot && !globalData->iTestCamImgProc)
				{
				delete globalData;
				Dll::FreeTls();
				}
			else
				{
				User::LeaveIfError(Dll::SetTls(globalData));	
				}
			}
		else
			{
			User::LeaveIfError(Dll::SetTls(globalData));
			}
		return self;		
		}
	}

void CTestCamPresets::Release()
	{
	iRefCount--; 
	if(iRefCount == 0)
		{
		delete this;
		}
	}	
	
void CTestCamPresets::ConstructL()
	{
	iSupportedPresets.Reset();
	iSupportedPresets.AppendL(KUidECamPresetFactoryDefault);
	
	iFactoryPresetAffectSettings.Reset();
	iFactoryPresetAffectSettings.AppendL(KUidECamEventCameraSettingDriveMode);
	iFactoryPresetAffectSettings.AppendL(KUidECamEventBurstImages);	
	}

void CTestCamPresets::GetSupportedPresetsL(RArray<TUid>& aPresets) const
	{
	aPresets.Reset();
	for	(TInt index=0; index < iSupportedPresets.Count(); index++)
		{
		User::LeaveIfError(aPresets.Append(iSupportedPresets[index]));
		}
	}

void CTestCamPresets::SetPreset(TUid aPreset)
	{
	TInt eventError = iOwner.CheckReserveAndPower();
	if(eventError == KErrNone)
		{
		if(aPreset == iSupportedPresets[0])
			{
			//ResetBrightness, Contrast, Drivemode, No. of burst mode images
			iOwner.iBrightness = 0;
			iOwner.iContrast = 0;
			iOwner.iImgProcBrightness = 0;
			iOwner.iImgProcContrast = 0;
			iOwner.iDriveMode = KDefaultDriveMode;
			iOwner.iBurstImages = KBurstImages;
			}
		else
			{
			eventError = KErrNotSupported;	
			}
		}
	
	TECAMEvent ecamevent(aPreset, eventError);
	
	iOwner.iECamEvent = ecamevent;
	iOwner.iHandleEventAsync.CallBack();	
	}
	
TUid CTestCamPresets::Preset() const
	{
	return iCurrentPreset;	
	}
	
void CTestCamPresets::GetAffectedSettingsL(RArray<TUid>& aSettings) const
	{
	aSettings.Reset();
	
	if(iCurrentPreset == iSupportedPresets[0])
		{	
		for	(TInt index=0; index < iFactoryPresetAffectSettings.Count(); index++)
			{
			User::LeaveIfError(aSettings.Append(iFactoryPresetAffectSettings[index]));
			}
		}
	}
	
void CTestCamPresets::GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const
	{
	switch(aPreset.iUid)
		{
		case KUidECamPresetFactoryDefaultUidValue:
			{
			aSettings.Reset();
			for	(TInt index=0; index < iFactoryPresetAffectSettings.Count(); index++)
				{
				User::LeaveIfError(aSettings.Append(iFactoryPresetAffectSettings[index]));
				}
			break;	
			}
		
		//fall through	
		case KUidECamPresetOutdoorUidValue:
		case KUidECamPresetOutdoorSportUidValue:
		case KUidECamPresetSnowUidValue:
		case KUidECamPresetBeachUidValue:
		case KUidECamPresetNightPortraitUidValue:
		case KUidECamPresetNightFireworksUidValue:
		case KUidECamPresetFogUidValue:
		case KUidECamPresetIndoorUidValue:
		case KUidECamPresetIndoorSportUidValue:
			{
			User::Leave(KErrNotSupported);	
			}
		//fall through			
		case KUidECamPresetNightPartyIndoorUidValue:
		case KUidECamPresetNightCloseUpUidValue:
		case KUidECamPresetNightMacroUidValue:
		case KUidECamPresetTextDocumentUidValue:
		case KUidECamPresetBarcodeUidValue:
		case KUidECamPresetAutoUidValue:
		case KUidECamPresetPortraitUidValue:
		case KUidECamPresetLandscapeUidValue:
		case KUidECamPresetAmbienceMoodUidValue:
			{
			if(iOwner.CameraVersion() == KCameraDefaultVersion)	
				{
				User::Leave(KErrArgument);
				}
			else
				{
				User::Leave(KErrNotSupported);
				}
			}
						
		default:
			User::Leave(KErrArgument);
			
		}
	}
