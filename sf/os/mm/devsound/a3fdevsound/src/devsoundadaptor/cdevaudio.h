/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CDEVAUDIO_H
#define CDEVAUDIO_H

#include <e32base.h>

#include "mmfdevsoundadaptationbody.h"
#include <a3f/maudiocontextobserver.h>
#include <a3f/maudiostreamobserver.h>
#include "mdevsoundadaptationinfoobserver.h"

#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fdevsoundaudiopauseresume.h>


// CLASS FORWARD
class CDevAudioControl;
class CDevPlayControl;
class CDevRecordControl;
class CDevToneControl;
class CDevGenControl;
class CAudioContextFactory;
class CA3FDevSoundAdaptationInfo;
class MAudioContext;
class MAudioStream;
class MGlobalProperties;

/**
Panic category and codes that CDevAudio raises on the client
*/
_LIT(KMMFDevAudioPanicCategory, "CDevAudio");

enum TMMFDevAudioPanicCodes
	{
	EValidStateBeforeCommit =1
	};

enum TDevSoundAdaptorState
	{
	EDevSoundAdaptorCreated_Uninitialised,
	EDevSoundAdaptorRemovingProcessingUnits,
	EDevSoundAdaptorUnitialised_Uninitialised,
	EDevSoundAdaptorUninitialising,
	EDevSoundAdaptorInitialising,
	EDevSoundAdaptorInitialised_Initialised,
	EDevSoundAdaptorLoading,
	EDevSoundAdaptorUnloading,
	EDevSoundAdaptorInitialised_Idle,
	EDevSoundAdaptorGoingActive,
	EDevSoundAdaptorPreempted,
	EDevSoundAdaptorBeingPreempted,
	EDevSoundAdaptorStopping,
	EDevSoundAdaptorActivating,
	EDevSoundAdaptorActive_Active,
	EDevSoundAdaptorPausing,
	EDevSoundAdaptorPaused_Primed
	};

/**
 * CDevAudio.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */
NONSHARABLE_CLASS(CDevAudio) :	public CBase,
								public MAudioContextObserver,
								public MA3FDevSoundAdaptationInfoObserver,
								public MA3FDevSoundAutoPauseResumeObserver
	{
	friend class CDevAudioControl;
	friend class CDevPlayControl;
	friend class CDevRecordControl;
	friend class CDevToneControl;
    friend class CDevGenControl;
    friend class CDevCommonControl;

public:

	~CDevAudio();

	static CDevAudio* NewL(MDevSoundAdaptationObserver& aDevSoundObserver,
									MGlobalProperties& aGlobalProperties);

	void ConstructL(MGlobalProperties& aGlobalProperties);

	// 2nd phase async construct - will callback to aDevSoundObserver
	TInt PostOpen();

	/**
	* Sets control operating mode (playing/recording...)
	* @since
	* @param aMode new control mode
	* @param aFormat codec format
	* @return error code
	*/
	TInt Initialize(TUid aFormat, TMMFState aMode);

	/**
	* Cancels Initialization
	* @since
	* @return error code
	*/
	TInt CancelInitialize();

	/**
	* Returns pointer to audio control instance for current operating mode
	* @since
	* @param aAudioControl on return, contains pointer to audio control
	* @return error code
	*/
	CDevAudioControl* GetAudioControl();

	/**
	* Defines the priority settings that should be used for this instance.
	* @since
	* @param const TMMFPrioritySettings &aPrioritySettings A class type
	*        representing the client's priority, priority preference and
	*        state
	* @return Err
	*/
	TInt SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	/**
	* Sets client configuration
	* @since
	* @param aProcessId DevSound's client process
	* @param aActualProcessId DevSound's effective/actual client (supplied by SetClientThreadInfo)
	*/
    TInt SetClientConfig(const TProcessId& aProcessId);
    TInt SetClientConfig(const TProcessId& aActualProcessId, const TProcessId& aProcessId);


	/**
	* Retrieve the current A3F state
	* @since
	* @return the current A3F state. See TAudioState
	*/
	TDevSoundAdaptorState ActiveState() const;

	/**
	* Retrieve the previous DevSound adaptor state
	* @return void
	*/
	TDevSoundAdaptorState PreviousState() const;

	/**
	* Set the current DevSound adaptor state
	* @return void
	*/
	void SetActiveState(TDevSoundAdaptorState aAdaptorState);

	/**
	* Set the previous DevSound adaptor state
	* @return void
	*/
	void SetPreviousState(TDevSoundAdaptorState aAdaptorState);

	/**
	* SetVolume for DevSound
	*/
	TInt SetDevSoundVolume(TInt aVolume, TBool& aAsyncCompletion);

	/**
	* SetGain for DevSound
	*/
	TInt SetDevSoundGain(TInt aGain, TBool& aAsyncCompletion);

	/**
	* SetPlayBalance for DevSound
	*/
	TInt SetDevSoundPlayBalance(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAsyncCompletion);

	/**
	* SetPlayBalance for DevSound
	*/
	TInt SetDevSoundRecordBalance(TInt aLeftPercentage, TInt aRightPercentage, TBool& aAsyncCompletion);

	/**
	* SetVolumeRamp for DevSound
	*/
	TInt SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	/**
	* MaxVolume for DevSound
	*/
	TInt DevSoundMaxVolume() const;
	/**
	* MaxGain for DevSound
	*/
	TInt DevSoundMaxGain() const;
	/**
	* Volume setting
	*/
	TInt DevSoundVolume() const;
	/**
	* Gain setting
	*/
	TInt DevSoundGain() const;

	/**
	* GetPlayBalance for DevSound
	*/
	void GetDevSoundPlayBalance(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	* GetRecordBalance for DevSound
	*/
	void GetDevSoundRecordBalance(TInt& aLeftPercentage, TInt& aRightPercentage);

	/**
	* GetSupportedDataTypes for DevSound
	*/
	void DevSoundSupportedDataTypesL(RArray<TFourCC>& aSupportedDataTypes, TUint aDataType);

	/**
	* Call from control - generates appropriate SetGainAndBalance() call
	*/
	TInt RequestGainAndBalance(CDevAudioControl* aCallingControl);

	// From base class MAudioContextObserver
	/**
	* Callback to context observer to show progression through Commit() and pre-emption cycles
	*
	* @param aEvent  a Uid giving the specific event.
	* @param aError  an error code. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void ContextEvent(TUid aEvent, TInt aError);

	// from MA3FDevSoundAdaptationInfoObserver
	void RequestMaxGainComplete (TUid aCodecType, TInt aError, TInt aResult);
	void RequestSupportedFormatsComplete(TUid aCodecType, TInt aError);

	/**
	* Queries if the low layers does support resume operation.
	* @since
	* @return TBool ETrue if Resume is supported
	*               EFalse otherwise
	*/
	TBool IsResumeSupported();

	void ConvertToFourCcL(RArray<TFourCC>& aSupportedDataTypes, RArray<TUid>& iSupportedFormats);
	
	//from MA3FDevSoundAutoPauseResume
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData);
	TInt CancelRegisterAsClient(TUid aEventType);
	TInt WillResumePlay();
	
	//from MA3FDevSoundAutoPauseResumeObserver
	virtual void NotifyResume();
	
	/**
	* GetPrioritySettings for DevSound
	*/
	void GetPrioritySettings(TAudioTypeSettings& astorage);
	
	TBool IsPrioritySet();
	
	//Panic function
	void Panic(TMMFDevAudioPanicCodes aCode);

protected:

	CDevAudio(MDevSoundAdaptationObserver& aAdaptationObserver);

	// If active, apply devsound volume and balance changes
	TInt SetGainAndBalance(TBool aCallBack, TBool& aAsyncCompletion);

	// Helper methods
	TInt CreateAudioProcessingUnits(TUid aSource, TUid aSink, TUid aCodec);
	void DeleteAudioProcessingUnits();

	TInt CommitAudioContext();
	TBool IsMidState(TDevSoundAdaptorState aAdaptorState);

/*
* most of member data is protected for DevAudioControl access
*/
protected: // data

	/**
	* adaptation observer - used to indicate async callbacks etc
	*/
	MDevSoundAdaptationObserver& iAdaptationObserver;

	/**
	* Pointer to audio context factory - context creation
	* Own.
	*/
	CAudioContextFactory *iAudioContextFactory;

	/**
	* Pointer to audio context.
	* Not own.
	*/
	MAudioContext *iAudioContext;

	/**
	* Pointer to audio stream.
	* Not own.
	*/
	MAudioStream *iAudioStream;

	/**
	* Pointer to audio source.
	* Not own.
	*/
	MAudioProcessingUnit* iAudioSource;

	/**
	* Pointer to audio codec.
	* Not own.
	*/
	MAudioProcessingUnit* iAudioCodec;

	/**
	* Pointer to audio gain control
	* Not own.
	*/
	MAudioProcessingUnit* iGainControl;

	/**
	* Pointer to audio sink
	* Not own.
	*/
	MAudioProcessingUnit* iAudioSink;

	/**
	* Mode to which this DevAudio is initialized in
	*/
	TMMFState iMode;

	TUid 	iFormat;

	/**
	* Active DevSound Adaptor state
	*/
	TDevSoundAdaptorState iActiveState;

	/**
	* Previous DevSound Adaptor state
	*/
	TDevSoundAdaptorState iPreviousState;

	TAudioState		iActiveStreamState;

	TBool 	iReinitializing;
	TBool 	iClosing;
	TBool	iStop;
	TUid 	iTargetFormat;
	TMMFState iTargetMode;

	TInt iDevSoundMaxGain;
	TInt iDevSoundMaxVolume;
	TInt iDevSoundGain;
	TInt iDevSoundVolume;
	TInt iDevSoundPlayBalance[2];
	TInt iDevSoundRecordBalance[2];
	TTimeIntervalMicroSeconds	iRampDuration;
private:
	enum TPreOpenState
		{
		EPreOpenStateIdle,
		EPreOpenStateRequestingMaxVolume,
		EPreOpenStateRequestingMaxGain,
		EPreOpenStateRequestingInputFormats,
		EPreOpenStateRequestingOutputFormats,
		};

	TPreOpenState iPreOpenState;

	RArray<TUid> iSupportedInputFormats;
	RArray<TUid> iSupportedOutputFormats;

	CA3FDevSoundAdaptationInfo* iAdaptationInfo;

	CDevPlayControl*   iDevPlayControl;
	CDevRecordControl* iDevRecordControl;
	CDevToneControl*   iDevToneControl;
	CDevGenControl*    iDevGenControl;

	/**
	* Points to whichever control is active
	* Not own.
	*/
	CDevAudioControl* iCurrentAudioControl;

	// Pointer to global properties
	// Not owned.
	MGlobalProperties*	iGlobalProperties;
	
	TAudioTypeSettings iPrioritySettings;
	TBool iPriorityFlag;

	};

inline TInt CDevAudio::DevSoundMaxVolume() const
	{
	return iDevSoundMaxVolume;
	}

inline TInt CDevAudio::DevSoundMaxGain() const
	{
	return iDevSoundMaxGain;
	}

inline TInt CDevAudio::DevSoundVolume() const
	{
	return iDevSoundVolume;
	}

inline TInt CDevAudio::DevSoundGain() const
	{
	return iDevSoundGain;
	}

#endif // CDEVAUDIO_H
