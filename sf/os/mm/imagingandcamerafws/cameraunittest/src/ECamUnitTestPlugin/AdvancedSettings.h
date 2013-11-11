
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

#ifndef _ECAMSETPLUGIN1_H_
#define _ECAMSETPLUGIN1_H_

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

#include <ecam/ecamadvsettingsintf.h>
#include "ECamUnitTestPlugin.h"
#include "ECamUnitTestPluginUids.hrh"
#include <ecam/implementationfactoryintf.h>

#define KSupportedPresetsNumber 4
#define KSupportedTransformationsNumber 3

const TInt KDefaultIsoRate					= 200;
const TInt KMaxIsoRate						= 1600;
const TInt KIsoRate1						= 320;
const TInt KIsoRate2						= 640;
const TInt KIsoRate3						= 840;
const TInt KIsoRate4						= 1200;
const TInt KIsoRate5						= 1400;
const TInt KMinIsoRate						= 100;
const TInt KNumSupportedIsoRate				= 8;
const TInt KSupportedContinuousZoomType		= 0;

const TUid KUidCameraManagementUid = {KUidCameraManagementUidValue};

class CCamContinuousZoom;
// Implementations of the MCameraAdvancedSettings class
class CCamAdvSet : 	public CBase, 
					public MCameraAdvancedSettings,
					public MCameraAdvancedSettings2,
					public MCameraAdvancedSettings3,
					public MCameraAdvancedSettings4
	{
public:
	static CCamAdvSet* NewL(CCamUnitTestPlugin& aOwner);
	~CCamAdvSet();
	
protected:

	void Release();	
	//from MCameraAdvancedSettings
	CCamera::CCameraAdvancedSettings::TCameraType CameraType() const;	
	CCamera::CCameraAdvancedSettings::TCameraType CameraType(TInt aCameraIndex) const;	
	TBool IsCameraPresent() const;
	TBool IsCameraPresent(TInt aCameraIndex) const;
	TInt CameraIndex() const;
	TInt SupportedStabilizationModes() const;
	CCamera::CCameraAdvancedSettings::TStabilizationMode StabilizationMode() const;
	void SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode aStabilizationMode = CCamera::CCameraAdvancedSettings::EStabilizationModeOff);
	TInt SupportedFocusModes() const;
	CCamera::CCameraAdvancedSettings::TFocusMode FocusMode() const;
	void SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode = CCamera::CCameraAdvancedSettings::EFocusModeAuto);
	TInt SupportedFocusRanges() const;
	CCamera::CCameraAdvancedSettings::TFocusRange FocusRange() const;
	void SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange = CCamera::CCameraAdvancedSettings::EFocusRangeNormal);
	TInt SupportedAutoFocusTypes() const;
	CCamera::CCameraAdvancedSettings::TAutoFocusType AutoFocusType() const;
	void SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType);
	TInt SupportedAutoFocusAreas() const;
	CCamera::CCameraAdvancedSettings::TAutoFocusArea AutoFocusArea() const;
	void SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea aAutoFocusArea);
	TInt FocusDistance() const;
	void SetFocusDistance(TInt aDistance); 
	TInt GetMinFocalLength() const;

    void GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const;
    TInt IsoRate() const;
    void SetIsoRate(TInt aRate);
     	
	void GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const;
	TInt Aperture() const;
	void SetAperture(TInt aFStop); 
	void GetShutterSpeedsL(RArray<TInt>& aShutterSteps, TValueInfo& aInfo) const; 
	TInt ShutterSpeed() const; 
	void SetShutterSpeed(TInt aShutterSpeed); 
	TInt SupportedMeteringModes() const;
	CCamera::CCameraAdvancedSettings::TMeteringMode MeteringMode() const;
	void SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode aMeteringMode);
	TInt SupportedDriveModes() const;
	CCamera::CCameraAdvancedSettings::TDriveMode DriveMode() const;
	void SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode);
	TInt SupportedBracketModes() const;
	CCamera::CCameraAdvancedSettings::TBracketMode BracketMode() const;
	void SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode aBracketMode);
	TInt SupportedBracketParameters() const;
	CCamera::CCameraAdvancedSettings::TBracketParameter BracketParameter() const;
	void SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter aBracketParameter);
	TInt SupportedBracketSteps() const;
	CCamera::CCameraAdvancedSettings::TBracketStep BracketStep() const;
	void SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep aBracketStep);
	void GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const;
	void SetBracketMerge(TInt aStartIndex = 0, TInt aFrames = 2);
	TInt SupportedFlashModes() const;
	CCamera::TFlash FlashMode() const;
	void SetFlashMode(CCamera::TFlash aMode = CCamera::EFlashNone);
	TBool RedEyeReduceOn() const;
	void SetRedEyeReduceOn(TBool aState);
	void GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const; 
	TInt FlashCompensationStep() const;
	TInt GetFlashCompensationStep(TInt& aFlashCompensationStep) const;
	void SetFlashCompensationStep(TInt aFlashCompensationStep);
	void GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const;

	TInt FlashCompensation() const;
	TInt GetFlashCompensation(TInt& aFlashCompensation) const;
	void SetFlashCompensation(TInt aFlashCompensation);
	TBool IsExternalFlashPresent() const;
	void GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const;
	TInt ManualFlashPowerLevel() const;
	void SetManualFlashPowerLevel(TInt aManualFlashPowerLevel);
	TInt SupportedExposureModes() const;
	CCamera::TExposure ExposureMode() const;
	void SetExposureMode(CCamera::TExposure aExposureMode = CCamera::EExposureAuto);
	void GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const;
	TInt ExposureCompensationStep() const; 
	TInt GetExposureCompensationStep(TInt& aExposureCompensationStep) const;
	void SetExposureCompensationStep(TInt aExposureCompensationStep);
	void GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const;

	TInt ExposureCompensation() const;
	TInt GetExposureCompensation(TInt& aExposureCompensation) const;
	void SetExposureCompensation(TInt aExposureCompensation);
	TInt SupportedWhiteBalanceModes() const;
	CCamera::TWhiteBalance WhiteBalanceMode() const;
	void SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode);
	TBool ApertureExposureLockOn() const;
	void SetApertureExposureLockOn(TBool aAELock);
	TBool ShootClickOn() const;
	void SetShootClickOn(TBool aShootClickOn);
	void GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const;
	TInt TimerInterval() const;  
	void SetTimerInterval(TInt aTimerInterval); 
	void GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const;
	void GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const; 
	void SetTimeLapse(const TTime& aStart, const TTime& aEnd, const TTime& aInterval); 
	CCamera::CCameraAdvancedSettings::TPictureOrientation PictureOrientation() const;
	TInt SupportedPixelAspectRatios() const;
	CCamera::CCameraAdvancedSettings::TPixelAspectRatio PixelAspectRatio() const;
	void SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation aOrientation); 
	void SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio); 
	TInt SupportedYuvRanges() const;
	CCamera::CCameraAdvancedSettings::TYuvRange YuvRange() const;
	void SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange aYuvRange);
	TInt BurstImages() const;
	void SetBurstImages(TInt aImages);
	void GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo)  const;
	TInt OpticalZoom() const;
	void SetOpticalZoom(TInt aOpticalZoom);
	void GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo)  const;

	TInt DigitalZoom() const;
	void SetDigitalZoom(TInt aDigitalZoom);
	TBool ExposureLockOn() const;
	void SetExposureLockOn(TBool aState=EFalse);
	TBool AutoFocusLockOn() const;
	void SetAutoFocusLockOn(TBool aState=EFalse);
	void GetSupportedSettingsL(RArray<TUid>& aSettings) const;   
	void GetActiveSettingsL(RArray<TUid>& aActiveSettings) const;
	void GetDisabledSettingsL(RArray<TUid>& aActiveSettings) const;
	void SetAutomaticSizeSelectionChangeOn(TBool aSetOn = EFalse);		
	TBool AutomaticSizeSelectionChangeOn() const;

    void GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const;
    TInt ContinuousAutoFocusTimeout() const;
    void SetContinuousAutoFocusTimeout(TInt aMicrosecondTimeout);
    void SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect aEffect);
	CCamera::CCameraAdvancedSettings::TStabilizationEffect StabilizationEffect() const;
	TInt SupportedStabilizationEffects() const;
	TInt SupportedStabilizationComplexityValues() const;
	CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity StabilizationComplexity() const;
	void SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity aComplexity);
    CCamera::CCameraAdvancedSettings::TWBUnits SupportedWBUnits() const;
    void SetWBRgbValue(const TRgb& aValue);
	void GetWBRgbValue(TRgb& aValue) const;
	void GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const;
	void SetWBColorTemperature(TInt aColorTemperature);	
	TInt WBColorTemperature() const;
	
	//from MCameraAdvancedSettings2
	TInt IsFlashReady(TBool& aReady) const;
	void GetCurrentFocusModeStepsL(RArray<TInt>& aFocusModeSteps, TValueInfo& aInfo) const;
	
	//from MCameraAdvancedSettings3
	void GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const;
	void SetISORate(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam);
	void GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const;
	void SetReferenceScreen(CWsScreenDevice& aScreenDevice);
	void GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const;
	void GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aFrameRateIndex, TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible, CCamera::TExposure aExposure) const;
	void GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode aCameraMode, TInt& aPreCaptureWarningSupported) const;
	void SubscribeToPreCaptureWarningL(TInt aPreCaptureWarning);
	void UnSubscribePreCaptureWarningL();
	void GetPreCaptureWarningL(TInt& aPreCaptureWarning) const;
	void GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const;
	void GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& aAFAssistantLight) const;
	void SetAFAssistantLight(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight);
	
	//from MCameraAdvancedSettings4
	void GetSupportedContinuousZoomTypeL(TUint& aSupportedContinuousZoomType) const;
	void GetFocalLengthInfoL(TInt& aMinFocalLength, TInt& aCurrentFocalLength, TInt& aMaxFocalLength) const;
	void GetNumOperationPreferenceL(TUint& aNumOperationPreferenceSupported) const;

	void EnumerateOperationPreferenceL(TUint aOperationPreferenceIndex, CCamera::CCameraAdvancedSettings::TPerformanceLevel& aSpeedLevel, 
			CCamera::CCameraAdvancedSettings::TPerformanceLevel& aQualityLevel, CCamera::CCameraAdvancedSettings::TPerformanceLevel& aLowMemoryConsumptionLevel, 
			CCamera::CCameraAdvancedSettings::TPerformanceLevel& aLowPowerConsumptionLevel) const;

	void SetOperationPreference(TUint aOperationPreferenceIndex);
	void GetOperationPreferenceL(TInt& aOperationPreferenceIndex) const;
	void GetSupportedEventsL(RArray<TUid>& aSupportedEvents) const;
	void GetIndirectFeatureChangesL(TUid aRequestedSetting, RArray<TUid>& aIndirectFeatureChanges) const;
	void CreateContinuousZoomImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	
private:
	CCamAdvSet(CCamUnitTestPlugin& aOwner);
	void Init();
	void RetrieveResult(TInt& aError);
	
public:
	TInt iRefCount;

private:
	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	CCamUnitTestPlugin& iOwner;
	TInt iShutterSpeed;
	TInt iFocusDistance;
	TInt iAperture;
	TInt iSupportedISORateType;
	CCamera::CCameraAdvancedSettings::TISORateType iISORateType;
	TBool iIsCapableActualISOValue;
	CCamera::CCameraAdvancedSettings::TAFAssistantLight iAFAssistantLight;
	TInt iAutoISOIndex;
	TInt iSupportedAFAssistantLight;
	CCamera::CCameraAdvancedSettings::TFocusRange iFocusRange;
	CCamera::TWhiteBalance iWhiteBalance;
	TInt iPixelAspectRatio;
	TInt iFlashMode;
	TInt iSupportedDriveModes;
	CCamera::CCameraAdvancedSettings::TDriveMode iDriveMode;
	};

class CCamPresets : public CBase, 
					public MCameraPresets,
					public MCameraPresets2
	{
public:
	static CCamPresets* NewL(CCamUnitTestPlugin& aOwner);
	~CCamPresets();
	
protected:
	//from MCameraPresets
	void Release();
	void GetSupportedPresetsL(RArray<TUid>& aPresets) const;
	void SetPreset(TUid aPreset);
	TUid Preset() const;  
	void GetAffectedSettingsL(RArray<TUid>& aSettings) const;
    void GetAssociatedSettingsL(TUid aPreset, RArray<TUid>& aSettings) const;
    
    //from MCameraPresets2
    void GetRangeRestrictedSettingsL(RArray<TUid>& aRangeRestrictedSettings) const;
    void GetFeatureRestrictedSettingsL(RArray<TUid>& aFeatureRestrictedSettings) const;
    void IsPresetUnlockSupportedL(TBool& aUnlockSupported) const;
	void LockPreset();
	void UnlockPreset();

private:
	CCamPresets(CCamUnitTestPlugin& aOwner);

public:
	TInt iRefCount;

private:
	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	CCamUnitTestPlugin& iOwner;
	TUid iSupportedPresets[KSupportedPresetsNumber];
	TUid iAffectedSettings[KSupportedPresetsNumber-1];
	TUid iCurrentPreset;
	RArray<TUid> iRangeRestrictedSettings;
	TBool iIsPresetUnLockSupported;
	TBool iIsPresetLocked;
	};


class CCamImgProc : public CBase, 
					public MCameraImageProcessing,
					public MCameraImageProcessing2,
					public MCameraImageProcessing3
	{
public:
	static CCamImgProc* NewL(CCamUnitTestPlugin& aOwner);
	~CCamImgProc();
	
protected:
	//from MCameraImageProcessing
	void Release();
	void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const;
	void GetActiveTransformationsL(RArray<TUid>& aTransformations) const;
	void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const;
	TInt TransformationValue(TUid aTransformation) const;
	TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const;
	void SetTransformationValue(TUid aTransformation, TInt aValue);
	void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const;
	void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence);
	void SetSourceRect(const TRect& aRect);
	void GetSourceRect(TRect& aRect) const;
	
	//MCameraImageProcessing2
	void GetConcurrentColorSwappingsSupportedL(TInt& aConcurrentColorSwappingSupported) const;
	void GetColorSwapCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorSwapCapabilities) const;
	void SetColorSwapEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters); 
	void RemoveColorSwapEntry(TInt aIndex);
	void GetColorSwapEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const; 
	void StartColorSwapping();
	void CancelColorSwappingL();
	void GetConcurrentColorAccentSupportedL(TInt& aConcurrentColorAccentSupported) const;
	void GetColorAccentCapabilitiesL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationCapabilities& aColorAccentCapabilities) const;
	void SetColorAccentEntry(TInt aIndex, const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters); 
	void RemoveColorAccentEntry(TInt aIndex);
	void GetColorAccentEntryL(TInt aIndex, CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const; 
	void StartColorAccent();
	void CancelColorAccentL();
	
	//from MCameraImageProcessing3
	void GetSupportedRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
			TUint& aSupportedRelativeRotation, TUint& aSupportedRelativeMirroring, TUint& aSupportedRelativeFlipping) const;
	
	void GetCurrentRelativeOrientationOptionsL(CCamera::CCameraImageProcessing::TOrientationReference& aOrientationReference, 
			CCamera::CCameraImageProcessing::TRelativeRotation& aRelativeRotation, CCamera::CCameraImageProcessing::
			TRelativeMirror& aRelativeMirror, CCamera::CCameraImageProcessing::TRelativeFlipping& aRelativeFlipping) const;	

  	void SetRelativeOrientationOptions(CCamera::CCameraImageProcessing::TOrientationReference aOrientationReference, 
			CCamera::CCameraImageProcessing::TRelativeRotation aRelativeRotation, CCamera::CCameraImageProcessing::
			TRelativeMirror aRelativeMirror, CCamera::CCameraImageProcessing::TRelativeFlipping aRelativeFlipping) const;
    
    void ConstructL();
    
private:
	CCamImgProc(CCamUnitTestPlugin& aOwner);
	
	TBool CheckColorSwapEntry(const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorSwapParameters) const;
	TBool CheckColorAccentEntry(const CCamera::CCameraImageProcessing::TColorOperationEntry& aColorAccentParameters) const;

public:
	TInt iRefCount;
	
private:
	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	CCamUnitTestPlugin& iOwner;
	// transformations 
	RArray<TUid> iActiveSequence;
	RArray<TUid> iSupportedTransformations;
	TRect iSourceRect;
	TInt iValue;
	TUid iTransformation;
	TInt iBrightnessRange[2];
	
	CCamera::CCameraImageProcessing::TColorOperationCapabilities iSupportedColorSwapCapabilities;
	CCamera::CCameraImageProcessing::TColorOperationCapabilities iSupportedColorAccentCapabilities;
	CCamera::CCameraImageProcessing::TColorOperationEntry iColorSwapParameter;
	CCamera::CCameraImageProcessing::TColorOperationEntry iColorAccentParameter;
	};

class CCamV2DirectViewFinder;
class CCamClientViewFinder;
class CCamOverlay;

class CDataGlobal : public CBase
	{
public:
	CCamAdvSet* iCamAdvSet;
	CCamImgProc* iCamImgProc;
	CCamPresets* iCamPresets;
	CCamOverlay* iCamOverlay;
	CCamV2DirectViewFinder* iCamV2DirectViewFinder;
	CCamClientViewFinder* iCamClientViewFinder;
	TInt 		iReferenceCount;	
	};
	
//class for external management
class MCamManagement
	{
public: 	
	virtual void Release()=0;
	virtual TBool PlugCameraIn(TInt aIndex)=0;
	virtual TBool PlugCameraOut(TInt aIndex)=0;
	};	
	
class CCamManagement : public CBase, public MCamManagement
	{
public:
	static CCamManagement* NewL(CCamUnitTestPlugin& aOwner);
	~CCamManagement();
	
	TBool PlugCameraIn(TInt aIndex);
	TBool PlugCameraOut(TInt aIndex);
	void Release();
	
protected:
   // void ConstructL();
    
private:
	CCamManagement(CCamUnitTestPlugin& aOwner);

private:
	CCamUnitTestPlugin& iOwner;
	};	


//	
// continuous zoom factory.
class CContinuousZoomFactory : public CBase, public MImplementationFactory
	{
public:
	static CContinuousZoomFactory* NewL();
	~CContinuousZoomFactory();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CContinuousZoomFactory();
	void ConstructL();

private:
	mutable CCamContinuousZoom* iCamContinuousZoomImp;
	};


class CCamContinuousZoom : public CBase, public MCameraContinuousZoom
	{
public:
	static CCamContinuousZoom* NewL(CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType);
	~CCamContinuousZoom();

	//from MCameraContinuousZoom
	void SetContinuousZoomObserverAndHandle(MContinuousZoomObserver& aObserver, CCamera::CCameraContinuousZoom* aContinuousZoomHandle);
	void StartContinuousZoomL(CCamera::CCameraAdvancedSettings::TContinuousZoomParameters aContinuousZoomParameters);
	void StopContinuousZoom();
	void GetContinuousZoomSupportInfoL(CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo& aContinuousZoomInfo) const;
	void GetContinuousZoomId(TInt& aZoomId) const;
	void Release();

private:
	CCamContinuousZoom(CCamera::CCameraAdvancedSettings::TContinuousZoomType aContinuousZoomType);

private:
	MContinuousZoomObserver* iObserver;
	CCamera::CCameraContinuousZoom*   iContinuousZoomHandle;
	
	CCamera::CCameraAdvancedSettings::TContinuousZoomParameters 	iParameters;
	CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo 	iInfo;
	CCamera::CCameraAdvancedSettings::TContinuousZoomType			iZoomType;
	};

#endif // _ECAMSETPlUGIN1_H_
