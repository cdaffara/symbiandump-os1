// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MDAAUDIOSAMPLEPLAYER_H
#define __MDAAUDIOSAMPLEPLAYER_H

#include <e32std.h>
#include <f32file.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmfclntutility.h>
#include <mmf/common/mmfdurationinfocustomcommands.h>

class CMdaServer;

/**
@publishedAll
@released

An interface to a set of audio player callback functions.

The class is a mixin and is intended to be inherited by the client
class which is observing the audio playing operation. The functions
encapsulated by this class are called when specific events occur in
the process of initialising and playing an audio sample. A reference to
this object is passed as a parameter when constructing an audio player
utility object.

@since  5.0
*/
class MMdaAudioPlayerCallback
	{
public:

	/**
	Defines required client behaviour when an attempt to open and
	initialise an audio sample has completed, successfully or otherwise.

	@param  aError
	        The status of the audio sample after initialisation.
	        The following values have the same specific meaning
	        across all EPOC platforms: KErrNone the
	        sample is ready to play. KErrNotSupported
	        the audio format or particular encoding type is not
	        recognised or not supported. KErrNotFound
	        the audio sample cannot be found.
	        KErrNoMemory there is insufficient memory
	        to play this audio sample. Other values are possible
	        indicating a problem opening the audio sample. These
	        values are dependent on the EPOC platform.
	@param  aDuration
	        The duration of the audio sample.
	*/
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration) = 0;

	/**
	Defines required client behaviour when an attempt to playback an audio
	sample has completed, successfully or otherwise.

	@param   aError
	         The status of playback. The following values have the
	         same specific meaning across all EPOC platforms:
	         KErrNone playing of the audio sample is
	         complete. KErrCorrupt the sample data is
	         corrupt. KErrInUse the sound device is in
	         use by another higher priority client. This can happen
	         during playback. KErrNoMemory there is
	         insufficient memory to play this audio sample Other
	         values are possible indicating a problem opening the
	         audio sample. These values are dependent on the EPOC
	         platform.
	*/
	virtual void MapcPlayComplete(TInt aError) = 0;
	};


class CMMFMdaAudioPlayerUtility;

/**
@publishedAll
@released

Plays sampled audio data.

The class offers a simple interface to open, play and obtain
information from, sampled audio data. The audio data can be supplied
either in a file (file-based), as a descriptor
(descriptor-based) or as a URL reference (since version 7.0s).

While this class is abstract, the three static functions construct, initialise
and return pointers to instances of concrete classes derived from this
abstract class. 

@since  5.0
*/
class CMdaAudioPlayerUtility : public CBase, 
							   public MMMFClientUtility
	{
public:
	IMPORT_C static CMdaAudioPlayerUtility* NewFilePlayerL(const TDesC& aFileName,
								MMdaAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);
	IMPORT_C static CMdaAudioPlayerUtility* NewDesPlayerL(const TDesC8& aData,
								MMdaAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);
	IMPORT_C static CMdaAudioPlayerUtility* NewDesPlayerReadOnlyL(const TDesC8& aData,
								MMdaAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality,
								CMdaServer* aServer = NULL);

	// new export from version 7.0
	IMPORT_C static CMdaAudioPlayerUtility* NewL(MMdaAudioPlayerCallback& aCallback,
								TInt aPriority = EMdaPriorityNormal,
								TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	IMPORT_C void UseSharedHeap();

	~CMdaAudioPlayerUtility();

	virtual void Play();

	virtual void Stop();

	virtual TInt SetVolume(TInt aVolume);

	virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);

	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	virtual const TTimeIntervalMicroSeconds& Duration();
	IMPORT_C TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration);

	virtual TInt MaxVolume();

	IMPORT_C void OpenFileL(const TDesC& aFileName);
	IMPORT_C void OpenFileL(const RFile& aFile);

	IMPORT_C void OpenFileL(const TMMSource& aSource);

	IMPORT_C void OpenDesL(const TDesC8& aDescriptor);

	IMPORT_C void OpenUrlL(const TDesC& aUrl, TInt aIapId = KUseDefaultIap, const TDesC8& aMimeType=KNullDesC8);

	IMPORT_C TInt Pause();

	IMPORT_C void Close();

	IMPORT_C TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);

	IMPORT_C void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	IMPORT_C TInt SetPriority(TInt aPriority, TInt aPref);

	IMPORT_C TInt GetVolume(TInt& aVolume);

	IMPORT_C TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);

	IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);

	IMPORT_C TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd);

	IMPORT_C TInt ClearPlayWindow();

	IMPORT_C TInt SetBalance(TInt aBalance = KMMFBalanceCenter);

	IMPORT_C TInt GetBalance(TInt& aBalance);

	IMPORT_C void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);

	IMPORT_C void GetAudioLoadingProgressL(TInt& aPercentageComplete);

	IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);

	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);

	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);

	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	
	IMPORT_C TInt GetBitRate(TUint& aBitRate);

	IMPORT_C MMMFDRMCustomCommand* GetDRMCustomCommand();

	IMPORT_C TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);

	IMPORT_C TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
    
	IMPORT_C TInt WillResumePlay();

	IMPORT_C TInt SetThreadPriority(const TThreadPriority& aThreadPriority) const;	
	
private:
	CMdaAudioPlayerUtility();
	
protected:
	/**
	This member is internal and not intended for use.
	*/
	CMMFMdaAudioPlayerUtility* iProperties;
	}; 


#endif
