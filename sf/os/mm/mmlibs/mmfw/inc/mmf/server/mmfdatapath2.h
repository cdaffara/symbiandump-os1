// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology 
*/

#ifndef MMFDATAPATH2_H
#define MMFDATAPATH2_H

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfsubthreadbase.h>
#include <mmf/server/mmfdatapath.h>

/**

This class is an extended version of CMMFDataPath. CMMFDataPath is extended to implement the loop play 
functionality in the controller. This class overrides some of the virual functions of CMMFDataPath in order to
implement the loop play functionality. All the controller plugins that require loop play functionality should
use CMMFDataPath2 in place of CMMFDataPath.

@see CMMFDataPath
*/
class CMMFDataPath2 : public CMMFDataPath
	{
public:
	//CMMFDataPath2 constructor if codec Uid is not already known by CMMFController
	//and there is no data path ambiguity - ie only one data path is possible
	IMPORT_C static CMMFDataPath2* NewL(MAsyncEventHandler& aEventHandler); 

	//CMMFDataPath2 constructor if codec Uid is not already known by CMMFController
	//and there is ambiguity ie more than one possible data path TMediaId used to select path 
	IMPORT_C static CMMFDataPath2* NewL(TMediaId aMediaId, MAsyncEventHandler& aEventHandler); 

	//CMMFDataPath2 constructor if codec Uid is already known by CMMFController
	//and there is no data path ambiguity - ie only one data path is possible
	IMPORT_C static CMMFDataPath2* NewL(TUid aCodecUid, MAsyncEventHandler& aEventHandler); 

	//CMMFDataPath2 constructor if codec Uid is already known by CMMFController
	//and there is ambiguity ie more than one possible data path TMediaId used to select path 
	IMPORT_C static CMMFDataPath2* NewL(TUid aCodecUid, TMediaId aMediaId, MAsyncEventHandler& aEventHandler);

	virtual ~CMMFDataPath2();
	virtual void BufferFilledL(CMMFBuffer* aBuffer); //called by the CMMFDataPath's MDataSource when it has filled the buffer
	
	virtual void PrimeL();
	virtual void PlayL();
	virtual void Pause();
	virtual void Stop();
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds Position() const;
	// Called when source and sink needs to be de-referenced
	virtual void ResetL();
	void RunL();
	// Set Repeats.
	IMPORT_C void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	IMPORT_C void SetDrmProperties(MDataSource* iDrmSource, TBool *aDisableAutoIntent);
	IMPORT_C void RetainRepeatInfo();
	IMPORT_C void PreEmptionPause();
	
protected:
	CMMFDataPath2(TMediaId aMediaId, MAsyncEventHandler& aEventHandler);
	void ConstructL(TUid aCodecUid = KNullUid);
	void FillSourceBufferL();
	void DoFillSourceBufferL();
	TTimeIntervalMicroSeconds CalculateAudioOutputPosition() const;
	//called when trailing silence playback is complete
	static TInt RepeatTrailingSilenceTimerComplete(TAny* aDataPath);
	TInt DoRepeatTrailingSilenceTimerCompleteL();
	void PlaySilence();
	void DoStopL();
	void DoPauseL();
	void DoCancel();
	TTimeIntervalMicroSeconds OutputPosition() const;
	void DoPreEmptionPauseL();
	
	
protected:
	/**
    The number of times to repeat the audio sample. 
    */
    TInt iNumberOfTimesToRepeat;
    
    /**
    The number of times played. 
    */
    TInt iNumberOfTimesPlayed;
	/**
    The time interval of the trailing silence in microseconds.
	*/
	TTimeIntervalMicroSeconds iTrailingSilence;

	/**
    The time interval of the trailing silence left to play in microseconds.
	*/
	TTimeIntervalMicroSeconds iTrailingSilenceLeftToPlay;
	
	/**
    The time left for the play will be completed in a particular repeat
	*/
	TTimeIntervalMicroSeconds iTimeLeftToPlayComplete;

	/**
	Timer used for trailing silences when repeating
	*/
	CPeriodic* iRepeatTrailingSilenceTimer;
	//This variable stores the devsound time played when the play is repositioned
	TTimeIntervalMicroSeconds iDevSoundRepositionTime;
	//states when the silence period between repeats needs to be verified
	TBool iVerifyPlayComplete;
	
	//For Drm implementation during repeats
	MDataSource* iDrmSource;
	TBool* iDisableAutoIntent;
	TBool iRetainRepeatInfo;
	TBool iGetTimePlayedSupported;
	TBool iIsResumeSupported;
	TBool iIsUsingResumeSupport;
	TBool iPauseCalledInsilence;
	}; 

#endif
