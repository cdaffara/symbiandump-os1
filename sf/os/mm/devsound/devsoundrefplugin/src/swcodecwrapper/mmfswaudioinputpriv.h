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
// mmfswaudioinputpriv.h
// 
//

#ifndef SWAUDIOINPUTPRIV_H
#define SWAUDIOINPUTPRIV_H

#include <e32def.h>
#include "mmfswaudioinput.h"
#include <d32soundsc.h>
#include <e32base.h>
#include <e32std.h>

#define KNumRecorders (2) // 2 for double buffering, 4 quadruple etc

enum TPanicCodes
    {
    KPanicBadTotalQueueLength=1,    // once constructed, total queue length should be KNumRecorders
    KPanicBadTotalQueueLength2,     // during construction or delation, total queue length should be <= KNumRecorders
    KPanicBadBusyQueueLength,       // Busy queue should never exceed length of 1
    EPanicBusyRecorderNotActive,    // If in the busy queue, should be active
    EPanicNonBusyRecorderActive,    // opposite
    };

//Total Number of sample rates
class CChannelAndSampleRateConverter;  // forward dec

class TRecordSharedChunkBufConfig : public TSharedChunkBufConfigBase
	{
public:
	TInt iBufferOffsetList[3];
	};

NONSHARABLE_CLASS(CAudioInput) : public CBase, 
					public MAudioInput,
					public MAIParamInterface
	{
	NONSHARABLE_CLASS( CRecorder ) : public CActive
		{
	public:
		CRecorder(CAudioInput& aParent, TInt aIndex);
		~CRecorder();
		void Cancel();
		
		void RecordData();
		TInt Index() const;
		void ReleaseBuffer(TBool aDoAnyway=EFalse);
		TBool IsBusy() const;
		TBool BufferHeld() const;
		TInt Length() const; 
		TInt Offset() const;
		TInt StatusOrOffset() const;
		
		TSglQueLink iLink; // used to form queues. Treat as private.
		
		// from CActive
		void RunL();
		void DoCancel(); 
	private:
		CAudioInput& iParent;
		const TInt iIndex;
		TInt iLength;
		TBool iBufferHeld;
		};

private:
	class TFormatData
		{
	public:
		inline TFormatData():
			iSampleRate(8000), iRequestedChannels(1) // default
			{
			}
	public:
		TInt iSampleRate;
		TInt iActualRate;
		TInt iRequestedChannels;
		TInt iActualChannels;			
		};
	
public:
	static CAudioInput* NewL(MAudioInputObserver& aObserver);
	~CAudioInput();
	
	// from MAudioInput
	void Release();
	TInt Initialize(const TAudioInputParams& aParams);
	void Close();
	TInt Start();
	void BufferAck();
	TInt Pause();
	TInt Resume();
	TInt Flush();
	void Stop();
	TAny* Interface(TUid aInterfaceUid);
	
	// from MAIParamInterface
	TInt SetGain(TInt aGain);
	TInt GetBufferSizes(TInt& aMinSize, TInt& aMaxSize);
	TInt GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates);
	
	RSoundSc& RecordSoundDevice();
	void BufferArrives(CRecorder* aRecorder);
	void BufferError(CRecorder* aRecorder, TInt aError);
	TInt SetFormat(const TAudioInputParams& aFormat);
	TInt NegotiateFormat(const TAudioInputParams& aFormat, TFormatData &aFormatData);
	TInt GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates, RSoundSc& aSoundDevice);

private:
	CAudioInput(MAudioInputObserver& aObserver);
	void ConstructL();
	void Cancel();

	static TInt Callback(TAny* aPtr);
	void AsyncCallbackL();
    void RequestCallback();
	
	void CancelRecorders();
    void CancelPendingRecorders();
    void CancelBusyRecorder();
	void InternalStop();
	void InternalFlush();
	void RecordAllIdle();
	
	void UseBuffer(CRecorder* aRecorder);
	void HandleBufferAck();
	
	void CheckFullInvariant();
	void CheckInvariant(TBool aKnownConstructed=ETrue);
	
#ifdef _DEBUG
    void CheckActiveRecorders();
    void CheckActiveRecorders(TSglQue<CRecorder>& aQueue, TBool aExpected, TInt aPanicCode);
#endif

	static TInt QLength(TSglQue<CRecorder>& aQueue);
	CRecorder* QPop(TSglQue<CRecorder>& aQueue);
	TBool QPop(CRecorder*& aRecorder, TSglQue<CRecorder>& aQueue);
	
	inline TInt BufferLength() const { return iBufferLength; } // TODO required?
		
private:
	MAudioInputObserver& iObserver;
	
	enum TState
		{
		EStateCreated2,
		EStateInitialized2,
		EStateRecordWait2,
		EStateRecordWaitAck2,
		};
	
	enum TRunningState // when in RecordWait2 or RecordWaitAck2
	    {
	    ERStateRunning,        // recording
	    ERStatePaused,         // paused state
	    ERStateFinishing,      // look to do InputFinished() on next tick
	    ERStateFinished,       // InputFinished() sent
	    ERStateFailed,         // InputError() sent 
	    };
		
	TState iState;
	TRunningState iRState;
	TRecordSharedChunkBufConfig iRecordBufferConfig;
	RSoundSc iRecordSoundDevice;
	RChunk iChunk;
	CRecorder* iRecorders[KNumRecorders];
	CAsyncCallBack* iAsyncCallBack;
	TInt iBufferLength;	// this is the length of buffers we request
    CChannelAndSampleRateConverter* iConverter;
    TPtrC8 iBufPtr;     // this is usually the descriptor sent to the observer
    RBuf8 iConvBuff;    // extra buffer from when we use a converter
    
    TSglQue<CRecorder> iIdleQueue;        // just sitting there
    TSglQue<CRecorder> iRecordingQueue;   // record operation outstanding
    TSglQue<CRecorder> iPendingQueue;     // buffer has been recorded, waiting to be processed
    TSglQue<CRecorder> iBusyQueue;        // mid InputAvailable()/BufferAck() cycle. Length <= 1
	};

inline TBool CAudioInput::QPop(CRecorder*& aRecorder, TSglQue<CRecorder>& aQueue)
// overload of QPop(), since "while (recorder = QPop(...))" etc gives warnings
// This allows "while (QPop(recorder, ...))" instead
	{
	CRecorder* recorder = QPop(aQueue);
	if (recorder)
		{
		aRecorder = recorder;
		return ETrue;
		}
	return EFalse;
	}

#endif // SWAUDIOINPUTPRIV_H

