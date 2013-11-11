// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_AUDIOTONECONTROLLER_H__
#define __MMF_AUDIOTONECONTROLLER_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>

// Public Media Server includes
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>

#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/server/sounddevice.h>


/** 
Panic category and codes for the Audio Tone Controller 
@internalTechnology
*/
_LIT(KMMFAudioToneControllerPanicCategory, "MMFAudioToneController");
enum TMMFMdaAudioToneControllerPanicCodes
	{
	EMMFMdaAudioToneControllerAlreadyPrepared,
	EMMFMdaAudioToneControllerBadToneConfig,
	EMMFMdaAudioToneControllerBadMixinCall
	};

/**
The public API for clients of the MMFAudioToneController
@internalTechnology
*/

class CMMFAudioToneController : public CMMFController, 
							public MMMFAudioPlayDeviceCustomCommandImplementor,
							public MMMFAudioPlayControllerCustomCommandImplementor,
							public MDevSoundObserver,
							public MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor
	{
public:
	static CMMFController* NewL();
	virtual ~CMMFAudioToneController();

	//implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);

	virtual void ResetL();
	virtual void PrimeL();
	virtual void PrimeL(TMMFMessage& aMessage);
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();

	virtual TTimeIntervalMicroSeconds PositionL() const {User::Leave(KErrNotSupported); TTimeIntervalMicroSeconds zero(0); return zero;}
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/){} 
	virtual TTimeIntervalMicroSeconds DurationL() const {TTimeIntervalMicroSeconds zero(0); return zero;}
	virtual void CustomCommand(TMMFMessage& aMessage);

	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	virtual void GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/){User::Leave(KErrNotSupported);}
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt /*aIndex*/){User::Leave(KErrNotSupported); return NULL;}

	//from MMMFAudioPlayDeviceCustomCommandImplementor
	virtual void MapdSetVolumeL(TInt aVolume);
	virtual void MapdGetMaxVolumeL(TInt& aMaxVolume);
	virtual void MapdGetVolumeL(TInt& aVolume);
	virtual void MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	virtual void MapdSetBalanceL(TInt aBalance);
	virtual void MapdGetBalanceL(TInt& aBalance);

	//from MMMFAudioPlayControllerCustomCommandImplementor
	virtual void MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& /*aStart*/, const TTimeIntervalMicroSeconds& /*aEnd*/) {User::Leave(KErrNotSupported);}
	virtual void MapcDeletePlaybackWindowL() {User::Leave(KErrNotSupported);}
	virtual void MapcGetLoadingProgressL(TInt& /*aPercentageComplete*/) {User::Leave(KErrNotSupported);}



	// From DevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError); 
	void BufferToBeFilled(CMMFBuffer* /*aBuffer*/) {Panic(EMMFMdaAudioToneControllerBadMixinCall);}
	void PlayError(TInt /*aError*/)	{Panic(EMMFMdaAudioToneControllerBadMixinCall);}
	void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)	{Panic(EMMFMdaAudioToneControllerBadMixinCall);} 
	void RecordError(TInt /*aError*/)	{Panic(EMMFMdaAudioToneControllerBadMixinCall);}
	void ConvertError(TInt /*aError*/)  {Panic(EMMFMdaAudioToneControllerBadMixinCall);}
	void DeviceMessage(TUid /*aMessageId*/, const TDesC8& /*aMsg*/) {Panic(EMMFMdaAudioToneControllerBadMixinCall);}	
	void SendEventToClient(const TMMFEvent& aEvent);
	
	//from MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor
	virtual TInt MapcSetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);


public:

/**
 TControllerState is an enumeration within the namespace  CMMFAudioToneController
 The enumeration provides a set of control states:
 EStopped is the initial state the controller resides in after it has been loaded
 EPrimed is the state the controller is in when it has been associated with datapaths but is not streaming data
 EPlaying is the state the controller is in when it is streaming data
*/
	enum TControllerState
		{
		EStopped,
		EPrimed,
		EPriming,
		EPausePlaying,
		EPlaying,
		};

protected:
	CMMFAudioToneController();
protected:
	void ConstructL();
	void NegotiateL();

	// state accessor functions
	TBool SetState( TControllerState aState );
	TControllerState State() const ;
	TBool            IsValidStateTransition( TControllerState aState ) const;
	TBool            Invariant() const;
	TBool            IsValidState( TControllerState aState ) const ;
	TBool            ResetPostCondition() const;

	void             CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const;
	void			 CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const;


	void Panic(TInt aPanicCode) const {User::Panic(KMMFAudioToneControllerPanicCategory, aPanicCode);}

private:
/** The data source provided by the Controller Framework, not owned (do not delete) */
	MDataSource*             iDataSource;

/** buffer that holds the data to be played */
	CMMFDataBuffer*          iToneSequenceData;

/** the Dev Sound that will be used to play the tone*/
	CMMFDevSound*            iMMFDevSound;

/** Current state of the controller, see TControllerState*/
	TControllerState         iState;

/** Priority setting used to controll access the Dev Sound*/	
	TMMFPrioritySettings     iPrioritySettings;

/** Indicates whether both a surce and data sink have been supplied by the Controller Framework*/
	TBool                    iSourceAndSinkAdded;

/** Indicates whether DevSound supports resume for current configuration*/
	TBool                    iIsResumeSupported;

/** Used to store message */	
	TMMFMessage*			 iMessage;
	};


#endif
