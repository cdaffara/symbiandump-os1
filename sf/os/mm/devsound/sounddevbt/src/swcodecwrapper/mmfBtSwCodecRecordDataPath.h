// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFBTSWCODECRECORDDATAPATH_H__
#define __MMFBTSWCODECRECORDDATAPATH_H__

#include "mmfBtSwCodecDataPath.h"

class CMMFSwCodecRecordDataPath; //fwd ref
class CRoutingSoundRecordDevice;	// fwd ref

/**
 * Active object used by the CMMFSwCodecPlayDataPath to get data from the sound
 * driver.  This particular active object encapsulates the asynchronous record
 * function, where a buffer of data is received from the WINS audio device, 
 * and the active object's RunL is called when the buffer has been created by 
 * the WINS audio device.
 * @internalComponent
 */
class CDataPathRecorder : public CActive
	{
public:
	CDataPathRecorder(CMMFSwCodecRecordDataPath& aParent, TInt aPriority);
	~CDataPathRecorder();
	void Start();
	void RecordData(CMMFDataBuffer& aData);
	void Stop();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	virtual void Error(TInt aError);
private:
	CMMFSwCodecRecordDataPath& iParent;
	CMMFDataBuffer* iDataFromSource;
	TBool iStopping;
	};

/**
 * Active object used by CMMFSwCodecPlayDataPath to listening for error messages
 * from the WINS audio device.  If this object's RunL is called, recording has 
 * been terminated for some reason.  The active object then notifies its parent, 
 * so that proper cleanup and client notification can occur.
 * @internalComponent
 */
class CSoundDevRecordErrorReceiver : public CActive
	{
public:
	CSoundDevRecordErrorReceiver(CMMFSwCodecRecordDataPath& aParent, TInt aPriority);
	~CSoundDevRecordErrorReceiver();
	void Start();
	void Stop();
	virtual void RunL();
	virtual void DoCancel();
private:
	CMMFSwCodecRecordDataPath& iParent;
	};


/**
 *  Derived class for play datapath internal to the Sw codec wrapper
 *  @internalComponent
 */
class CMMFSwCodecRecordDataPath : public CMMFSwCodecDataPath
	{ 
public:	
	static CMMFSwCodecRecordDataPath* NewL(CRoutingSoundRecordDevice* aSoundDevice);
	virtual ~CMMFSwCodecRecordDataPath();
	virtual TInt SetObserver(MMMFHwDeviceObserver &aHwObserver);
	virtual TInt AddCodec(CMMFSwCodec& aCodec);
	virtual TInt Start(); //record
	virtual void Stop();
	virtual void Pause();
	virtual void BufferFilledL(CMMFDataBuffer& aBuffer);
	virtual void BufferEmptiedL(const CMMFDataBuffer& aBuffer);	
	virtual void SoundDeviceException(TInt aError);
	virtual CRoutingSoundRecordDevice* Device();
	virtual TUint RecordedBytesCount(); 
protected:	
	CMMFSwCodecRecordDataPath(CRoutingSoundRecordDevice* aSoundDevice) : iSoundDevice(aSoundDevice) {};
	void ConstructL();
	void FillSoundDeviceBufferL();
	void FillSinkBufferL();
protected: 
	CDataPathRecorder* iAudioRecorder;
	CSoundDevRecordErrorReceiver* iSoundDeviceErrorReceiver;
	CRoutingSoundRecordDevice* iSoundDevice;	// Not owned.
	CMMFDataBuffer* iSinkBuffer;
	CMMFDataBuffer* iSoundDeviceBuffer;
	TUint iSinkBufferSize;
	TUint iSoundDevBufferSize;
	TUint iRecordedBytesCount;
	TUid iDeviceUid;
	};

#endif

