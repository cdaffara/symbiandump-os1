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

#ifndef __MMFSWCODECRECORDDATAPATH_H__
#define __MMFSWCODECRECORDDATAPATH_H__

#include "mmfSwCodecDataPath.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include "mmfswaudioinput.h"

class CMMFSwCodecRecordDataPath; //fwd ref
class MSetVbrFlagCustomInterface;	// fwd ref

/**
 *  Derived class for record datapath internal to the Sw codec wrapper
 *  @internalComponent
 */
class CMMFSwCodecRecordDataPath : public CMMFSwCodecDataPath,
								  public MSetVbrFlagCustomInterface,
								  public MAudioInputObserver,
								  public MSwSetParamInterface,
								  public MSwInfoInterface
	{
	enum TRecordState
	    {
	    ERecordStateCreated,                       // note order here is important - see State(), RecordOrPause() etc
        ERecordStateFailed,
	    ERecordStateRecording,
        ERecordStateSendingBuffer,
        ERecordStateSendingPartialBuffer,
        ERecordStateEmptiedPartialBuffer,
        ERecordStateRecordingPaused,
        ERecordStateSendingBufferPaused,
        ERecordStateSendingPartialBufferPaused,
        ERecordStateEmptiedPartialBufferPaused,
	    };
public:
	static CMMFSwCodecRecordDataPath* NewL();
	virtual ~CMMFSwCodecRecordDataPath();

	//From CMMFSwCodecDataPath
	TInt SetObserver(MMMFHwDeviceObserver &aHwObserver);
	TInt AddCodec(CMMFSwCodec& aCodec);
	TInt Start(); //record
	void Stop();
	void Pause();
	void BufferFilledL(CMMFDataBuffer& aBuffer);
	void BufferEmptiedL(const CMMFDataBuffer& aBuffer);
	void SoundDeviceException(TInt aError);
	RMdaDevSound& Device();
	TUint RecordedBytesCount();
	TAny* CustomInterface(TUid aInterfaceId);
	TSwCodecDataPathState State() const;
	//From MSetVbrFlagCustomInterface
	void SetVbrFlag();
	//From MAudioInputObserver
	void InputBufferAvailable(const TDesC8& aBuffer);
	void InputFinished();
	void InputError(TInt aError);
	//From MSwSetParamInerface
	TInt SetSampleRate(TInt aSampleRate);
	TInt SetNumChannels(TInt aNumChannels);
	TInt SetGain(TInt aGain);
	TInt GetBufferSizes(TInt& aMinSize, TInt& aMaxSize);
	//From MSwInfoInterface
	TInt GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates);

private:
	CMMFSwCodecRecordDataPath();
	void ConstructL();
	void ProcessBufferL(TBool aLastBuffer);
	TInt EmptyBufferL();

	TBool RecordOrPause() const;
	TBool IsPaused() const;

	void RequestCallback();
	TInt DoCallback();
	static TInt Callback(TAny* aPtr);

	static const TRecordState KResumePauseTable[];

private:
	MAudioInput* iAudioInput;
	CMMFDataBuffer* iCodecBuffer;
	const TDesC8*	iInputData;	  // not owned - cache of past data
	TPtr8			iShadowData;  // need TPtr8
	TInt 			iInputOffset; // track how much data we've processed from audio input
	CMMFPtrBuffer* 	iInputBuffer; // select from iInputData
	CMMFDataBuffer* iSinkBuffer;  // not owned - pointer to buffer to send to client
	CAsyncCallBack*	iAsyncCallback;
	TInt  iSampleRate;
	TInt  iNumChannels;
	TInt  iGain;
	TUint iSinkBufferSize;
	TUint iAudioInputBufferSize;
	TUint iRecordedBytesCount;
	TBool iVbrFlag;
	TRecordState iState;
    TBool iInputHasFinished; // extension to iState, if we've received InputFinished we don't want to call BufferAck() but signal complete
	RMdaDevSound iDummyDevSound; // TODO - remove when Device() is removed.
	};

#endif

