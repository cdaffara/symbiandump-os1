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

#ifndef __MMFBTSWCODECPLAYDATAPATH_H__
#define __MMFBTSWCODECPLAYDATAPATH_H__


#include "mmfBtSwCodecDataPath.h"
#include "mmfbtswcodecwrapper.h"


class CMMFSwCodecPlayDataPath; //fwd ref
class CMMFSwCodecUtility;	// fwd ref

class CRoutingSoundPlayDevice;	// fwd ref

/**
 * Active object used by the CMMFSwCodecPlayDataPath to send data to the sound
 * driver  This particular active object encapsulates the asynchronous play 
 * function, where a buffer of data is sent to the WINS audio device, and the
 * active object's RunL is called when the buffer has been consumed by the 
 * WINS audio device.
 * @internalComponent
 */
class CDataPathPlayer : public CActive
	{
public:
	CDataPathPlayer(CMMFSwCodecPlayDataPath& aParent, TInt aPriority);
	~CDataPathPlayer();
	void Start();
	void ResumePlaying();
	void PlayData(const CMMFDataBuffer& aData);
	void Stop();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	virtual void Error(TInt aError);
private:
	CMMFSwCodecPlayDataPath& iParent;
	const CMMFDataBuffer* iDataFromSource;
	TBool iResumePlaying;
	};

/*
* Active object used by CMMFSwCodecPlayDataPath to listening for error messages
* from the WINS audio device.  If this object's RunL is called, playback has 
* been terminated for some reason.  The active object then 
* notifies its parent the datapath, so that proper cleanup and client 
* notification can occur.
* @internalComponent
*/
class CSoundDevPlayErrorReceiver : public CActive
	{
public:
	CSoundDevPlayErrorReceiver(CMMFSwCodecPlayDataPath& aParent, TInt aPriority);
	~CSoundDevPlayErrorReceiver();
	void Start();
	void Stop();
	virtual void RunL();
	virtual void DoCancel();
private:
	CMMFSwCodecPlayDataPath& iParent;
	};


/**
 *  Derived class for play datapath internal to the Sw codec wrapper
 *  @internalComponent
 */
class CMMFSwCodecPlayDataPath : public CMMFSwCodecDataPath
	{ 
public:	
	static CMMFSwCodecPlayDataPath* NewL(CRoutingSoundPlayDevice* aSoundDevice,
										 TUid aDeviceUid);
	virtual ~CMMFSwCodecPlayDataPath(); 
	virtual TInt SetObserver(MMMFHwDeviceObserver &aHwObserver);
	virtual TInt AddCodec(CMMFSwCodec& aCodec);
	virtual TInt Start();
	virtual void Stop();
	virtual void Pause();
	virtual void BufferFilledL(CMMFDataBuffer& aBuffer);
	virtual void BufferEmptiedL(const CMMFDataBuffer& aBuffer);
	virtual void SoundDeviceException(TInt aError);
	virtual CRoutingSoundPlayDevice* Device();
	virtual void SetPlayCustomInterface(MPlayCustomInterface& aCustomInterface);
	virtual void SetConfigForAudioRamp(TUint aSampleRate, TUint aChannels); //need for audio ramper
protected:	
	CMMFSwCodecPlayDataPath(CRoutingSoundPlayDevice* aSoundDevice, TUid aDeviceUid) :
							iSoundDevice(aSoundDevice), iDeviceUid(aDeviceUid) {};
	void ConstructL();
	void FillSourceBufferL();
	void FillSoundDeviceBufferL();
protected: 
	CDataPathPlayer* iAudioPlayer;
	CSoundDevPlayErrorReceiver* iSoundDeviceErrorReceiver;
	CRoutingSoundPlayDevice* iSoundDevice;	// Not owned
	CMMFDataBuffer* iSourceBuffer;
	CMMFDataBuffer* iSoundDeviceBuffer;
	TBool iNoMoreSourceData;
	TBool iSinkCanReceive;
	TUint iSourceBufferSize;
	TUint iSoundDevBufferSize;
	CMMFSwCodecUtility* iUtility;
	TBool iRampAudioSample;
	MPlayCustomInterface* iCustomInterface;	// not owned
	TTimeIntervalMicroSeconds iVolumeRamp;
	
	// DEF048512
	TInt iSampleRate;
	TInt iChannels;
	
	TUid iDeviceUid;	// required to close the RoutingSoundPlayDevice
	};

#endif

