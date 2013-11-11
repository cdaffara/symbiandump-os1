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
//


#ifndef MDASOUNDADAPTER_H
#define MDASOUNDADAPTER_H

#include <e32std.h>
#include <e32ver.h>

	
#define MDADEVSOUNDNAME _L8("RMdaDevSound")

/*
 Adapter class which routes the calls on RMdaDevSound(old sound driver interface) to RSoundSc(new sound driver interface). 
 The purpose of this class is to use the new sound driver in MDF without changing the calling code. This class
 does not support any new functionalities supported by the shared chunk sound driver but supports the functionality of the 
 of the old driver with the new driver.
 */
class RMdaDevSound
	{
	class CBody;
public:
	/*
	enum for major, minor and build version of sound device driver.
	*/
	enum TVer {EMajorVersionNumber=1,EMinorVersionNumber=0,EBuildVersionNumber=KE32BuildVersionNumber};
    /*
	enum for sound media device play/record request.
	*/
	enum TMdaRequest
		{
		/* Play data request */
		EMdaRequestPlayData,
		/* Play error notification */
		EMdaRequestPlayErrorNotification,
		/* Record data request */
		EMdaRequestRecordData,
		/* Record error notification */
		EMdaRequestRecordErrorNotification
		};
	
	/* enum for media device control request */
	enum TMdaControl
		{
		/* play format (capability) supported request */
		EMdaControlPlayFormatsSupported,
		/* get the current play format request */
		EMdaControlGetPlayFormat,
		/* set the play format request  */
		EMdaControlSetPlayFormat,
		/* get play volume request */
		EMdaControlGetPlayVolume,
		/* set play volume request */
		EMdaControlSetPlayVolume,
		/* record format supported request */
		EMdaControlRecordFormatsSupported,
		/* get the current record format request */
		EMdaControlGetRecordFormat,
		/* set record format request */
		EMdaControlSetRecordFormat,
		/* get record volume request */
		EMdaControlGetRecordLevel,
		/* set record volume request */
		EMdaControlSetRecordLevel,
		/* stop play request */
		EMdaControlFlushPlayBuffer,
		/* stop record request */
		EMdaControlFlushRecordBuffer,
		/* bytes played request  */
		EMdaControlBytesPlayed,
		/* set base value for the bytes played */
		EMdaControlResetBytesPlayed,
		/* Investigate for sound media driver request */
		EMdaControlIsMdaSound,
		/* pause play request */
		EMdaControlPausePlayBuffer,
		/* resume play request */
		EMdaControlResumePlaying
		};
		
	/*
    This enum corresponds to supported sound encoding schemes
    */
	enum TMdaSoundEncoding
		{
		/* sound encoding(compression) using 8 bit PCM (pulse code modulation) */
		EMdaSoundEncoding8BitPCM		= 0x00000001,
		/* sound encoding using 16 bit PCM */
		EMdaSoundEncoding16BitPCM		= 0x00000002,
		/* sound encoding using 8 bit A law */
		EMdaSoundEncoding8BitALaw		= 0x00000004,
		/* sound encoding using 8 bit Mu law */
		EMdaSoundEncoding8BitMuLaw		= 0x00000008,
		};
		
	/*
    This class corresponds to a supported sound format.
	The format describes the supported audio device's min/max sampling rate, encoding, channels and buffer size of play/record and volume.
    */
	class TSoundFormatsSupported
		{
	public:
	/*
    This corresponds to minimum sample rate supported. This depends on the physical sound device used(example: 8000 hertz). 
    */
		TInt iMinRate;
		
	/*
    This corresponds to maximum sample rate supported. This depends on the physical sound device used(example: 48000 hertz).
    */
		TInt iMaxRate;
		
	/*
    This corresponds to encoding format supported.  
	@see  TMdaSoundEncoding
    */		
		TUint32 iEncodings;
		
	/*
    This corresponds to the number of sound channels supported. Possible values are EMono for a single channel and EStereo for two channel sound.
	Also this depends on the physical device used.
    */
		TInt iChannels;
		
	/*
    This corresponds to minimum buffer size. This depends on the physical device used and sampling rate adapted.
	@see KSoundMinBufferSize
    */
		TInt iMinBufferSize;
		
	/*
    This corresponds to maximum buffer size.This depends on the physical device used and sampling rate adapted. 
	@see KSoundPlayBufferSize
    @see KSoundRecordBufferSize
    */
		TInt iMaxBufferSize;

	/*
    This corresponds to minimum play/record volume. 
    */	
		TInt iMinVolume;
		
	/*
    This corresponds to maximum play/record volume. 
    */
		TInt iMaxVolume;
		};

	/* A typedef'd packaged RMdaDevSound::TSoundFormatsSupported for passing through a generic API method */
	typedef TPckgBuf<TSoundFormatsSupported> TSoundFormatsSupportedBuf;

	/*
    This class corresponds to current supported sound format. 
	The format describes the supported audio device's sampling rate, encoding, channels and buffer size of play/record.
    */
	class TCurrentSoundFormat
		{
	public:
	/*
    This corresponds to sound sampling rate like (44000 Hertz, 8000 Hertz). The possible values depends on the physical device used.
    */
		TInt iRate;
		
	/*
    @see TMdaSoundEncoding
    */
		TMdaSoundEncoding iEncoding;
		
	/*
    This corresponds to the number of sound channels supported. Possible values are EMono for a single channel and EStereo for two channel sound.
	Also this depends on the physical device used.
    */
		TInt iChannels;
		
	/*
    Play or Record buffer size. The possible value depends on the physical device used. 
    */
		TInt iBufferSize;
		};

	/* A typedef'd packaged RMdaDevSound::TCurrentSoundFormat for passing through a generic API method */
	typedef TPckgBuf<TCurrentSoundFormat> TCurrentSoundFormatBuf;
public:
	IMPORT_C RMdaDevSound();
	IMPORT_C TInt Open(TInt aUnit=KNullUnit);
	IMPORT_C TVersion VersionRequired() const;
	IMPORT_C TInt IsMdaSound();
	IMPORT_C void PlayFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported);
	IMPORT_C void GetPlayFormat(TCurrentSoundFormatBuf& aFormat);
	IMPORT_C TInt SetPlayFormat(const TCurrentSoundFormatBuf& aFormat);
	IMPORT_C TInt PlayVolume();
	// This function mimics RMdaDevSound interface with linear volume semantics
	IMPORT_C void SetPlayVolume(TInt aLinearVolume);
	// This function supports volume change with logarithmic semantics
	IMPORT_C void SetVolume(TInt aLogarithmicVolume);
	IMPORT_C void PlayData(TRequestStatus& aStatus,const TDesC8& aData);
	IMPORT_C void CancelPlayData();
	IMPORT_C void NotifyPlayError(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyPlayError();

	IMPORT_C void RecordFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported);
	IMPORT_C void GetRecordFormat(TCurrentSoundFormatBuf& aFormat);
	IMPORT_C TInt SetRecordFormat(const TCurrentSoundFormatBuf& aFormat);
	IMPORT_C TInt RecordLevel();
	IMPORT_C void SetRecordLevel(TInt aLevel);
	IMPORT_C void RecordData(TRequestStatus& aStatus,TDes8& aData);
	IMPORT_C void CancelRecordData();
	IMPORT_C void NotifyRecordError(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyRecordError();

	IMPORT_C void FlushPlayBuffer();
	IMPORT_C void FlushRecordBuffer();
	IMPORT_C TInt BytesPlayed();
	IMPORT_C void ResetBytesPlayed();
	IMPORT_C void PausePlayBuffer();
	IMPORT_C void ResumePlaying();
	IMPORT_C void ResumePlaying(TRequestStatus&);
	IMPORT_C void Close();
	IMPORT_C TInt Handle();
	IMPORT_C void PauseRecordBuffer();
	IMPORT_C void ResumeRecording();
	IMPORT_C TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTimePlayed);
private:
	CBody* iBody;
	};
	
#endif

