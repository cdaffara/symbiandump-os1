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

#ifndef TONEDATAPATH_H
#define TONEDATAPATH_H

#include <e32base.h>
#include "mdasoundadapter.h"
#include <mmf/server/mmfdatabuffer.h>
#include "tonehwdevice.h"

//message from  tonedatapath to hw device observer
//to tell it to update the bytes played 
#define KToneHwDeviceObserverUpdateBytesPlayed	0x101FE2A4

class MMMFHwDeviceObserver;
class CToneDataPath;
class CToneCodec;


/**
@internalTechnology
Panic codes for the Tone Data Path
*/
enum TToneDataPathPanicCode
	{
	/** Codec wrapper did not create an internal datapath
	*/
	EToneNoDataPath,

	/** Codec wrapper does not have a device handle
	*/
	EToneNoDevice,

	/** Codec wrapper codec returns non existant process result
	*/
	EToneBadCodec,

	/** Sound driver returns unexpected buffer
	*/
	EToneBadBuffer
	};


/**
 * Active object used by the CToneDataPath to send data to the sound
 * driver  This particular active object encapsulates the asynchronous play 
 * function, where a buffer of data is sent to the WINS audio device, and the
 * active object's RunL is called when the buffer has been consumed by the 
 * WINS audio device.
 * @internalComponent
 */
class CToneDataPathPlayer : public CActive
	{
public:
	CToneDataPathPlayer(CToneDataPath& aParent, TInt aPriority);
	~CToneDataPathPlayer();
	void Start();
	void ResumePlaying();
	void PlayData(const CMMFDataBuffer& aData);
	void Stop();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	virtual void Error(TInt aError);
private:
	CToneDataPath& iParent;
	const CMMFDataBuffer* iDataFromSource;
	TBool iResumePlaying;
	};

/*
* Active object used by CToneDataPath to listening for error messages
* from the WINS audio device.  If this object's RunL is called, playback has 
* been terminated for some reason.  The active object then 
* notifies its parent the datapath, so that proper cleanup and client 
* notification can occur.
* @internalComponent
*/
class CToneSoundDevPlayErrorReceiver : public CActive
	{
public:
	CToneSoundDevPlayErrorReceiver(CToneDataPath& aParent, TInt aPriority);
	~CToneSoundDevPlayErrorReceiver();
	void Start();
	void Stop();
	virtual void RunL();
	virtual void DoCancel();
private:
	CToneDataPath& iParent;
	};


class CToneDataPath : public CBase,
					  public MIgnoreUnderflowEventsCustomInterface

	{ 
public: 
	enum TToneDataPathState
	{
	EStopped,
	EPlaying,
	EPaused
	};
public:	
	static CToneDataPath* NewL();
	~CToneDataPath(); 
	TInt SetObserver(MMMFHwDeviceObserver& aHwObserver);
	TInt AddCodec(CToneCodec& aCodec);
	TInt Start();
	void Stop();
	void Pause();
	RMdaDevSound& Device();
	void BufferFilledL(CMMFDataBuffer& aBuffer);
	void BufferEmptiedL(const CMMFDataBuffer& aBuffer);
	void SoundDeviceException(TInt aError);
	TToneDataPathState State() {return iState;};
	TInt EmptyBuffers();
	virtual void IgnoreUnderflowEvents();
	virtual TAny* CustomInterface(TUid aInterface);


protected:	
	CToneDataPath() {};
	inline void Panic(TInt aPanicCode);
	void ConstructL();
	void FillSourceBufferL();
	void FillSoundDeviceBufferL();
	
#ifdef __CYCLE_MMF_DATABUFFERS__
	CMMFDataBuffer* CycleAudioBuffer(CMMFDataBuffer* aBuffer);
#endif

#ifdef __USE_MMF_TRANSFERBUFFERS__
	CMMFTransferBuffer* CreateTransferBufferL(TUint aBufferSize, CMMFTransferBuffer* aOldBuffer);
#endif

#ifdef __USE_MMF_PTRBUFFERS__
	CMMFPtrBuffer* CreatePtrBufferL(TUint aBufferSize);
#endif

protected: 
	TToneDataPathState iState;
	MMMFHwDeviceObserver* iHwDeviceObserver;
	CToneCodec* iCodec;
	CToneDataPathPlayer* iAudioPlayer;
	CToneSoundDevPlayErrorReceiver* iSoundDeviceErrorReceiver;
	RMdaDevSound iSoundDevice;
	CMMFDataBuffer* iSourceBuffer;
	CMMFDataBuffer* iSoundDeviceBuffer;
	TBool iNoMoreSourceData;
	TBool iSinkCanReceive;
	TUint iSourceBufferSize;
	TUint iSoundDevBufferSize;
	TBool iRampAudioSample;
	
	TTimeIntervalMicroSeconds iVolumeRamp;
	// DEF048512
	TInt iSampleRate;
	TInt iChannels;
	
	TInt iBuffSize;

	TBool iIgnoreUnderflow;

#ifdef __USE_MMF_TRANSFERBUFFERS__
	RTransferBuffer* iTransferBuffer;
	RTransferWindow* iTransferWindow;
#endif

#ifdef __USE_MMF_PTRBUFFERS__
	HBufC8* iPtrBufferMemoryBlock;
#endif

	};

/**
 * Internal panic
 * @internalComponent
 */
inline void CToneDataPath::Panic(TInt aPanicCode)
	{
	_LIT(KTonePanicCategory, "ToneDataPath");
	User::Panic(KTonePanicCategory, aPanicCode);
	}

#endif //TONEDATAPATH_H
