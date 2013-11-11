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

#ifndef MMFCLIENTAUDIORECORDER_H
#define MMFCLIENTAUDIORECORDER_H

#include <e32std.h>
#include <e32base.h>
#include <mdaaudiosampleeditor.h>

#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/common/mmfcontroller.h>

#include "mmfclientaudioplayer.h"
#include "mmfclientutility.h"
#include <mmf/common/mmfdurationinfocustomcommands.h>

#include <caf/caf.h>
using namespace ContentAccess;

class CMMFMdaObjectStateChangeObserverCallback;

/**
Concrete implementation of the CMdaAudioRecorderUtility API.
@see CMdaAudioRecorderUtility
*/
class CMMFMdaAudioRecorderUtility;
NONSHARABLE_CLASS( CMMFMdaAudioRecorderUtility ): public CBase,
												  public MMMFControllerEventMonitorObserver, 
												  public MRepeatTrailingSilenceTimerObs,
												  public MMMFFindAndOpenControllerObserver
	{
friend class CMdaAudioRecorderUtility;
// for testing only
friend class CTestStepUnitMMFAudClient;

public:
	static CMMFMdaAudioRecorderUtility* NewL(
		MMdaObjectStateChangeObserver& aObserver, 
		CMdaServer* aServer = NULL, 
		TInt aPriority = EMdaPriorityNormal, 
		TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	virtual ~CMMFMdaAudioRecorderUtility();

	void OpenFileL(const TDesC& aFileName);
	void OpenFileL(const TMMSource& aSource);
	
	void OpenDesL( const TDesC8& aDescriptor);  // old api now considered deprecated
	
	// [ replacement method for deprecated version that takes const TDesC8]
	void OpenDesL( TDes8& aDescriptor);         // fixed api 

	void OpenL(TMdaClipLocation* aLocation,		// Normally file or descriptor
			   TMdaClipFormat* aFormat,			// Data format
			   TMdaPackage* aArg1 = NULL,		// Normally codec to use
			   TMdaPackage* aArg2 = NULL);		// Normally audio settings
	void SetAudioDeviceMode(CMdaAudioRecorderUtility::TDeviceMode aMode);
	TInt MaxVolume();
	TInt MaxGain();
	TInt SetVolume(TInt aVolume);
	void SetGain(TInt aGain);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	void UseSharedHeap();
	//from CMdaAudioClipUtility
	CMdaAudioClipUtility::TState State();
	void Close();
	void PlayL();
	void RecordL();
	void Stop();
	void CropL(TBool aCropToEnd = ETrue);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);
	const TTimeIntervalMicroSeconds& Position();
	const TTimeIntervalMicroSeconds& RecordTimeAvailable();
	const TTimeIntervalMicroSeconds& Duration();	
	void SetPriority(TInt aPriority, TInt aPref);
	void SetMaxWriteLength(TInt aMaxWriteLength /*= KMdaClipLocationMaxWriteLengthNone*/);
	void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	void ClearPlayWindow();
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);

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

	void OpenFileL(const TDesC& aFileName,
				   TUid aRecordControllerUid,				// controller to use
				   TUid aPlaybackControllerUid,				// controller to use
				   TUid aFormatUid,							// Data format
				   TFourCC aCodec=KMMFFourCCCodeNULL);		// codec to use	
	void OpenFileL(const TMMSource& aFileSource,
				   TUid aRecordControllerUid,				// controller to use
				   TUid aPlaybackControllerUid,				// controller to use
				   TUid aFormatUid,							// Data format
				   TFourCC aCodec=KMMFFourCCCodeNULL);		// codec to use	


	void OpenFileL(const RFile& aFile);
	void OpenFileL(const RFile& aFile,
				   TUid aRecordControllerUid,				// controller to use
				   TUid aPlaybackControllerUid,				// controller to use
				   TUid aFormatUid,							// Data format
				   TFourCC aCodec=KMMFFourCCCodeNULL);		// codec to use	


	//[ replacement method for deprecated version that takes TDesC8 ]
	void OpenDesL(      TDes8& aDescriptor,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType);

	void OpenUrlL(const TDesC& aUrl,
						TInt aIapId,
						TUid aRecordControllerUid,			// controller to use
						TUid aPlaybackControllerUid,		// controller to use
						TUid aFormatUid,					// Data format
						TFourCC aCodec=KMMFFourCCCodeNULL);		// codec to use							

	void OpenUrlL(const TDesC& aUrl, TInt aIapId=KUseDefaultIap, const TDesC8& aMimeType=KNullDesC8); // Mime type of Url
	
	void CropL(const TTimeIntervalMicroSeconds & aCropBegin, 
			   const TTimeIntervalMicroSeconds & aCropEnd);
	TInt GetGain(TInt& aGain);
	TInt GetVolume(TInt& aVolume);

	TInt SetPlaybackBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetPlaybackBalance(TInt& aBalance);
	TInt SetRecordBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetRecordBalance(TInt& aBalance);

	TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);
	void AddMetaDataEntryL(CMMFMetaDataEntry& aMetaDataEntry);
	TInt RemoveMetaDataEntry(TInt aMetaDataIndex);
	void ReplaceMetaDataEntryL(TInt aMetaDataIndex, CMMFMetaDataEntry& aMetaDataEntry);

	// Codec support
	void GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDestinationDataTypes);
	void SetDestinationDataTypeL(TFourCC aCodec);
	TFourCC DestinationDataTypeL();
	// Bit rate support	
	void SetDestinationBitRateL(TUint aBitRate);
	TUint DestinationBitRateL();
	TUint SourceBitRateL();
	void GetSupportedBitRatesL(RArray<TUint>& aSupportedBitRates);
	// Sample rate support
	void SetDestinationSampleRateL(TUint aSampleRate);
	TUint DestinationSampleRateL();
	void GetSupportedSampleRatesL(RArray<TUint>& aSupportedSampleRates);
	// Format support
	void SetDestinationFormatL(TUid aFormatUid);
	TUid DestinationFormatL();
	// Number of channels
	void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);
	TUint DestinationNumberOfChannelsL();
	void GetSupportedNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels);
	
	void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);
	void GetAudioLoadingProgressL(TInt& aPercentageComplete);
	
	TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);
	TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
	TInt WillResumePlay();
	const CMMFControllerImplementationInformation& AudioPlayControllerImplementationInformationL();
	const CMMFControllerImplementationInformation& AudioRecorderControllerImplementationInformationL();
	
	TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	MMMFDRMCustomCommand* GetDRMCustomCommand();

	TInt SetThreadPriorityPlayback(const TThreadPriority& aThreadPriority) const;
	TInt SetThreadPriorityRecord(const TThreadPriority& aThreadPriority) const;
	
	TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration);
		
protected:
	CMMFMdaAudioRecorderUtility(CMdaAudioRecorderUtility* aParent, MMdaObjectStateChangeObserver& aCallback);
	void ConstructL(TInt aPriority, TInt aPref);
private:
	void DoOpenFileForPlaybackL(const TDesC& aFileName);
	void DoOpenFileForRecordL(const TDesC& aFileName);
	void DoOpenDesForPlaybackL(TDes8& aDescriptor);
	void DoOpenDesForRecordL(TDes8& aDescriptor);
	void DoOpenUrlForPlaybackL(const TDesC& aUrl, TInt aIapId, const TDesC8& mimeType);
	void DoOpenUrlForRecordL(const TDesC& aUrl, TInt aIapId, const TDesC8& mimeType);

	TInt DoOpenFile(const TDesC& aFileName,
						TUid aRecordControllerUid,			
						TUid aPlaybackControllerUid);			

	TInt DoOpenFileSource(const TMMFileSource& aFileSource,
						TUid aRecordControllerUid,			
						TUid aPlaybackControllerUid);	
	TInt DoOpenFileHandleSource(const TMMFileHandleSource& aFileHandleSource,
						TUid aRecordControllerUid,			
						TUid aPlaybackControllerUid);					
	TInt DoOpenDes(TDesC8& aDescriptor,
						TUid aRecordControllerUid,	
						TUid aPlaybackControllerUid);
	TInt DoOpenUrlL(const TDesC& aUrl,
						TInt aIapId,
						TUid aRecordControllerUid,	
						TUid aPlaybackControllerUid);

	void CloseControllers();
	void ClosePlaybackController();
	void CloseRecordController();
						
	void SetupPlaybackDataPath();
	void SetupPlaybackDataPathComplete();
	TInt OpenComplete();
	void DoPlay();
	void DoPlayContinue();
	void Reset();
	void ConvertDescriptor(const TDesC8& aDescriptor );
	TInt RecordOrPlaybackFormat(const RMMFControllerImplInfoArray& aControllers, TUid aSuppliedFormatUid, TUid& aRecordFormatUid, TUid& aPlayFormatUid);

	enum TControllerType
		{
		ERecordController,
		EPlayController
		};

	const RMMFFormatImplInfoArray& ControllersFormatInfo(const CMMFControllerImplementationInformation* aController, TControllerType aControllerType);
	
	TUid FindControllerL(const RMMFControllerImplInfoArray& aControllers, TUid aFormatUid, TControllerType aControllerType, const CDesC8Array*& selectedFormatMimeTypes);
	TUid FindControllerL(const RMMFControllerImplInfoArray& aControllers, const CDesC8Array*& aMimeTypes, TControllerType aControllerType, TUid& aFormat);
	TUid FindControllerL(const RMMFControllerImplInfoArray& aControllers, const TMdaClipLocation* aLocation, TControllerType aControllerType, TUid& aFormat);

	TInt ControllerOpen();
	void PlaySilence();


private:
	// last play window command
	enum TPlayWindowCommand
		{
		ENone,
		ESet,
		EClear
		};

	class TPlayBackSettings {
	public:
		TPlayBackSettings():iVolume(-1){}
		TInt						iVolume;
		TInt						iBalance;
		TTimeIntervalMicroSeconds	iVolumeRampInterval;
	};
	
	MMdaObjectStateChangeObserver& iCallback;
	CMMFMdaObjectStateChangeObserverCallback* iAsyncCallback;
	MMMFAudioResourceNotificationCallback* iAudioResourceNotificationCallBack;

	RMMFController iRecordController;
	RMMFController iPlaybackController;
	TBool iRecordControllerAvailable;
	TBool iPlaybackControllerAvailable;

	// Custom commands that we need to use
	RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCustomCommands;
	RMMFAudioRecordDeviceCustomCommands iAudioRecordDeviceCustomCommands;
	RMMFAudioPlayControllerCustomCommands iAudioPlayControllerCustomCommands;
	RMMFAudioRecordControllerCustomCommands iAudioRecordControllerCustomCommands;
	RMMFAudioControllerCustomCommands iAudioControllerRecCustomCommands;
	RMMFAudioControllerCustomCommands iAudioControllerPlyCustomCommands;
	RMMFResourceNotificationCustomCommands iNotificationRegistrationCommands;
	RMMFDRMCustomCommands iDRMCustomCommands;
	RMMFAudioPlayControllerSetRepeatsCustomCommands iAudioPlayControllerSetRepeatsCommands;
	
	// Source and sink handle info
	TMMFMessageDestination iRecordSourceHandle;
	TMMFMessageDestination iPlaybackSourceHandle;
	TMMFMessageDestination iRecordSinkHandle;
	TMMFMessageDestination iPlaybackSinkHandle;


	CMMFControllerEventMonitor* iRecordEventMonitor;
	CMMFControllerEventMonitor* iPlaybackEventMonitor;


	TMMFPrioritySettings iPrioritySettings;

	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds iRecordTimeAvailable;
	TTimeIntervalMicroSeconds iPlayStart;
	TTimeIntervalMicroSeconds iPlayEnd;
	TPlayWindowCommand iPlayWindowSet;

	TInt iNumberOfTimesToRepeat;
	TTimeIntervalMicroSeconds iTrailingSilence;
	TTimeIntervalMicroSeconds iTrailingSilenceLeftToPlay;
	TInt iNumberOfTimesPlayed;
	CRepeatTrailingSilenceTimer* iRepeatTrailingSilenceTimer;

	CMdaAudioClipUtility::TState iState;

	TUid iPlaybackSourceUid;
	HBufC8* iPlaybackSourceData;

	TBool	iPlayDataPathInitialised;
	TFourCC iPlayDataType;
	CMMFControllerImplementationInformation* iPlayControllerImplementationInformation;
	CMMFControllerImplementationInformation* iRecordControllerImplementationInformation;
	RArray<TUid> iMediaIds;

	// Parent for callback
	CMdaAudioRecorderUtility* iParent;
	MAudioLoadingObserver* iLoadingObserver;
	//[ added to cache TDesC8 so that it can be passed on as a TDes8 
	// in the new interface for OpenDesL ]
	TPtr8 iCachedDescriptor ;

	// clip has been recorded into
	TBool iClipChanged;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;

	// open controller post-processing stuff
	TUid iPlayControllerUid;
	TUid iRecordControllerUid;

	TBool iClientCallbackPending;
	TBool iPlayPending;

	TUid iPlayFormatUid;			// for iAudioControllerPlyCustomCommands.SetSourceFormat
	TUid iRecordFormatUid;			// for iAudioControllerRecCustomCommands.SetSinkFormat
	TFourCC iRecordSourceDataType;	// for iAudioControllerRecCustomCommands.SetCodec()
	TFourCC iRecordSinkDataType;	// for iAudioControllerRecCustomCommands.SetCodec()
	TInt iPlaySampleRate;
	TInt iPlayChannels;
	TBool iPlaybackControllerUidNotSupplied;
	TPlayBackSettings iPlayBackSettings;
	TUid iEventHolder;
	TBuf8<256> iNotificationDataHolder;
	TInt iDurationError;
	}; 

#endif
