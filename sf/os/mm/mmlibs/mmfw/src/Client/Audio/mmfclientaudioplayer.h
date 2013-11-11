// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMFCLIENTAUDIOPLAY_H
#define MMFCLIENTAUDIOPLAY_H

#include <e32std.h>
#include <e32base.h>
#include <mdaaudiosampleplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include "mmfclientutility.h"
#include <mmf/common/mmfdurationinfocustomcommands.h>

static const TUid KUidMmfAudioController = {KMmfUidControllerAudio}; 

/**
Mixin class to allow notification that the timed silence has finished.
*/
class MRepeatTrailingSilenceTimerObs
	{
public:
	virtual void RepeatTrailingSilenceTimerComplete() = 0;
	};

/**
CTimer-based active object that waits the requested time before notifying its observer.
*/
class CRepeatTrailingSilenceTimer : public CTimer
	{
public:
	static CRepeatTrailingSilenceTimer* NewL(MRepeatTrailingSilenceTimerObs& aObs);
	void RunL();
private:
	CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs);
private:
	MRepeatTrailingSilenceTimerObs& iObs;
	};

/**
Active object utility class to allow the callback to be called asynchronously.
This should help prevent re-entrant code in clients of the mediaframework.
*/
class CMMFMdaAudioPlayerCallBack : public CActive
	{
public:
	enum TCallbackState {
		ECallbackInitComplete,
		ECallbackPlayComplete
		};

		
public:
	static CMMFMdaAudioPlayerCallBack* NewL(MMdaAudioPlayerCallback& aCallback);
	~CMMFMdaAudioPlayerCallBack();
	void InitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void PlayComplete(TInt aError);
private:
	CMMFMdaAudioPlayerCallBack(MMdaAudioPlayerCallback& aCallback);
	void RunL();
	void DoCancel();
private:
	MMdaAudioPlayerCallback& iCallback;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;
	TCallbackState iState;
	};

/**
Concrete implementation of the CMdaAudioPlayerUtility API.
@see CMdaAudioPlayerUtility
*/
class CMMFMdaAudioPlayerUtility;
NONSHARABLE_CLASS( CMMFMdaAudioPlayerUtility ): public CBase, 
								  				public MMMFControllerEventMonitorObserver,
								  				public MRepeatTrailingSilenceTimerObs,
								  				public MMMFFindAndOpenControllerObserver
	{
friend class CMdaAudioPlayerUtility;
// friends for Unit testing only
friend class CTestStepUnitMMFAudClient;

public:
	enum TMMFAudioPlayerState
		{
		EStopped,
		EOpening,
		EPaused,
		EPlaying		
		};
public:
	static CMMFMdaAudioPlayerUtility* NewFilePlayerL(const TDesC& aFileName, 
								MMdaAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);
	static CMMFMdaAudioPlayerUtility* NewDesPlayerL(const TDesC8& aData, 
								MMdaAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);
	static CMMFMdaAudioPlayerUtility* NewDesPlayerReadOnlyL(const TDesC8& aData, 
								MMdaAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);

	static CMMFMdaAudioPlayerUtility* NewL(MMdaAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

/**
	Destructor
*/
	~CMMFMdaAudioPlayerUtility();
	
	void UseSharedHeap();

	void OpenFileL(const TDesC& aFileName);
	void OpenFileL(const RFile& aFile);
	void OpenFileL(const TMMSource& aSource);

	void OpenDesL(const TDesC8& aDescriptor);
	void OpenUrlL(const TDesC& aUrl, TInt aIapId=KUseDefaultIap, const TDesC8& aMimeType = KNullDesC8);
	void Play();
	void Stop();
	TInt SetVolume(TInt aVolume);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	const TTimeIntervalMicroSeconds& Duration();
	TInt MaxVolume();

	// API Additions since version 7.0	
	TInt Pause();
	void Close();
	TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	TInt SetPriority(TInt aPriority, TInt aPref);
	TInt GetVolume(TInt& aVolume);
	TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);
	TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd);
	TInt ClearPlayWindow();
	TInt SetBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalance(TInt& aBalance);
	TInt GetBitRate(TUint& aBitRate);
	
	void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);
	void GetAudioLoadingProgressL(TInt& aPercentageProgress);
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
	
	TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);
	TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
	TInt WillResumePlay();
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	
	MMMFDRMCustomCommand* GetDRMCustomCommand();

	TInt SetThreadPriority(const TThreadPriority& aThreadPriority) const;
	
	TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration);
	
	// from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);
	// from MRepeatTrailingSilenceTimerObs
	virtual void RepeatTrailingSilenceTimerComplete();

	// from MMMFFindAndOpenControllerObserver 
	virtual void MfaocComplete(
		TInt& aError, 
		RMMFController* aController, 
		TUid aControllerUid, 
		TMMFMessageDestination* aSourceHandle, 
		TMMFMessageDestination* aSinkHandle);

protected:
	CMMFMdaAudioPlayerUtility(MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref);
	void ConstructL();
	void DoPlay();

private:
	void PlaySilence();

private:
	// last play window command
	enum TPlayWindowCommand
		{
		ENone,
		ESet,
		EClear
		};

	MMdaAudioPlayerCallback& iCallback;
	CMMFMdaAudioPlayerCallBack* iAsyncCallBack;
	MAudioLoadingObserver* iLoadingObserver;
	MMMFAudioResourceNotificationCallback* iAudioResourceNotificationCallBack;
	RMMFController iController;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	TMMFAudioPlayerState iState;
	TTimeIntervalMicroSeconds iDuration; // Needed because of api "Duration()" that returns a reference
	TMMFPrioritySettings iPrioritySettings;

	TInt iNumberOfTimesPlayed;
	TInt iNumberOfTimesToRepeat;
	TTimeIntervalMicroSeconds iTrailingSilence;
	TTimeIntervalMicroSeconds iTrailingSilenceLeftToPlay;	
	CRepeatTrailingSilenceTimer* iRepeatTrailingSilenceTimer;

	// Source and sink handle info
	TMMFMessageDestination iSourceHandle;
	TMMFMessageDestination iSinkHandle;

	// Custom command handlers
	RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCommands;
	RMMFAudioPlayControllerCustomCommands iAudioPlayControllerCommands;
	RMMFResourceNotificationCustomCommands iNotificationRegistrationCommands;
	RMMFDRMCustomCommands iDRMCustomCommands;
	RMMFAudioPlayControllerSetRepeatsCustomCommands iAudioPlayControllerSetRepeatsCommands;
	
	// Current playback time so we can resume from where we were stopped
	TTimeIntervalMicroSeconds iPosition;

	// Play window start and end times and whether it has been set
	TTimeIntervalMicroSeconds iPlayStart;
	TTimeIntervalMicroSeconds iPlayEnd;
	TPlayWindowCommand iPlayWindowSet;
	RArray<TUid> iMediaIds;
	
	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	TUid iControllerUid;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;
	TUid iEventHolder;
	TBuf8<256> iNotificationDataHolder;
	TBool iRepeatCancelled;
	}; 

#endif
