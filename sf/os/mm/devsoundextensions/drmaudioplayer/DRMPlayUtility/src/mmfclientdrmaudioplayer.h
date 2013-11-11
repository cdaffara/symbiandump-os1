/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   DRM Client AudioPlay Utility
*
*/



#ifndef __MMFCLIENTAUDIOPLAY_H__
#define __MMFCLIENTAUDIOPLAY_H__

#include <e32std.h>
#include <e32base.h>
#include <mdaaudiosampleplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include "mmfdrm.h"
//#include "MmfClientUtility.h"

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
Concrete implementation of the CDRMPlayerUtility API.
@see CDRMPlayerUtility
*/
class CMMFDrmAudioPlayerUtility : public CActive
	{
	enum TDRMUtilityState {
		EIdle,
		EInitializing,
		EPlaying,
		EPaused
		};

friend class CDrmPlayerUtility;

public:
	static CMMFDrmAudioPlayerUtility* NewFilePlayerL(const TDesC& aFileName, 
								MDrmAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);
	static CMMFDrmAudioPlayerUtility* NewDesPlayerL(const TDesC8& aData, 
								MDrmAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);
	static CMMFDrmAudioPlayerUtility* NewDesPlayerReadOnlyL(const TDesC8& aData, 
								MDrmAudioPlayerCallback& aCallback, 
								TInt aPriority = EMdaPriorityNormal, 
								TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	static CMMFDrmAudioPlayerUtility* NewL(MDrmAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	~CMMFDrmAudioPlayerUtility();
	void Play();
	void Stop();
	void SetVolume(TInt aVolume);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	const TTimeIntervalMicroSeconds& Duration();
	TInt MaxVolume();
	void OpenFileL(const TDesC& aFileName);
	void OpenFileL(const RFile& aFile);
	void OpenFileL(const TMMSource& aSource);
	void OpenDesL(const TDesC8& aDescriptor);
	void OpenUrlL(const TDesC& aUrl, TInt aIapId = KUseDefaultIap, const TDesC8& aMimeType=KNullDesC8);

	// API Additions since version 7.0	
	TInt Pause();
	void Close();
	TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
	TInt GetVolume(TInt& aVolume);
	TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);
	TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd);
	TInt ClearPlayWindow();
	TInt SetBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalance(TInt& aBalance);
	TInt GetBitRate(TUint& aBitRate);
	
    void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aLoadingObserver);

	void GetAudioLoadingProgressL(TInt& aPercentageProgress);
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();
	
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);


protected:
	CMMFDrmAudioPlayerUtility(MDrmAudioPlayerCallback& aCallback, TInt aActivePriority);
	void DoPlay();
	void RunL();
	void DoCancel();

private:
	void ConstructL(TInt aPriority = EMdaPriorityNormal,
					TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	void ConstructL(const TDesC& aFileName, 
					TInt aPriority = EMdaPriorityNormal, 
					TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	void ConstructL(const TDesC8& aData, 
					TInt aPriority = EMdaPriorityNormal, 
					TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	void ConstructReadOnlyL(const TDesC8& aData, 
					TInt aPriority = EMdaPriorityNormal, 
					TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);

	void ConnectL();
	
	//Returns True if Destination CustomCommand needs to be blocked
	TBool IsValidCustomCommandDestination(TUid aDestinationUid, const TDesC8& aParam);
    TBool CheckCustomInterfaceBuilderImplL(const TDesC8& aParam);
    
	// last play window command
	enum TPlayWindowCommand
		{
		ENone,
		ESet,
		EClear
		};

	MDrmAudioPlayerCallback& iCallback;
	TDRMUtilityState iState;
	TTimeIntervalMicroSeconds iDuration; // Needed because of api "Duration()" that returns a reference

	
	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	CDrm*						iDrm;
	RDrmSession*				iDrmSession;
	TInt						iError;
	TFileName iFileName;
	CBufFlat*					iMetaDataBuffer;
	MAudioLoadingObserver*		iLoadingObserver;
	TAsyncCallbackStatePckgBuf	iAsyncCallback;
	TErrorDurationStructPckgBuf	iErrorDurationStruct;
	TRequestStatus*				iRequestStatus;
	TDes8*						iDataPointer;
	TDRMCustomCommand			iCustomCommand;
	TPckgCustomCommand			iCustomCommandPckg;

    struct TCustomCommands
       {
       const TMMFMessageDestinationPckg* iDestination;
       TInt iFunction;
       const TDesC8* iDataTo1;
       const TDesC8* iDataTo2; 
       const TDes8* iDataFrom;

       TCustomCommands(const TMMFMessageDestinationPckg& aDestination, TInt aFunction,const TDesC8& aDataTo1,const TDesC8& aDataTo2,TDes8& aDataFrom)
          {
          iDestination = &aDestination;
          iFunction = aFunction;
          iDataTo1 = &aDataTo1;
          iDataTo2 = &aDataTo2;
          iDataFrom = &aDataFrom;
          };
       };
     struct TCustomCommandsAsync
       {
       const TMMFMessageDestinationPckg* iDestination;
       TInt iFunction;
       const TDesC8* iDataTo1;
       const TDesC8* iDataTo2; 
       TDes8* iDataFrom;
       TRequestStatus* iRequestStatus;

       TCustomCommandsAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction,const TDesC8& aDataTo1,const TDesC8& aDataTo2, TDes8& aDataFrom,TRequestStatus& aStatus)
          {
          iDestination = &aDestination;
          iFunction = aFunction;
          iDataTo1 = &aDataTo1;
          iDataTo2 = &aDataTo2;
          iDataFrom = &aDataFrom;
          iRequestStatus = &aStatus;
          };
       };
     struct TCustomCommandsAsyncNoData
       {
       const TMMFMessageDestinationPckg* iDestination;
       TInt iFunction;
       const TDesC8* iDataTo1;
       const TDesC8* iDataTo2; 
       TRequestStatus* iRequestStatus;

       TCustomCommandsAsyncNoData(const TMMFMessageDestinationPckg& aDestination, TInt aFunction,const TDesC8& aDataTo1,const TDesC8& aDataTo2,TRequestStatus& aStatus)
          {
          iDestination = &aDestination;
          iFunction = aFunction;
          iDataTo1 = &aDataTo1;
          iDataTo2 = &aDataTo2;
          iRequestStatus = &aStatus;
          };
       };

    typedef TPckg<TCustomCommands> TPckgTCustomCommandStructure;
	}; 

#endif
