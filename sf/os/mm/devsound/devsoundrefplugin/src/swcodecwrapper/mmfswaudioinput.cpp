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
// mmfswaudioinput.cpp
// 
//
#include "mmfswaudioinput.h"
#include "mmfswaudioinputpriv.h"
#include <d32soundsc.h>
#include <e32debug.h>
#include "mmf/utils/rateconvert.h" // if we need to resample
_LIT(KPddFileName,"SOUNDSC.PDD");
_LIT(KLddFileName,"ESOUNDSC.LDD");

#ifdef SYMBIAN_SWCODEC_LOGGING

const TText* const KStateNames[] = // must agree with TState
            {
            _S("EStateCreated2"), 
            _S("EStateInitialized2"), 
            _S("EStateRecordWait2"), 
            _S("EStateRecordWaitAck2"), 
            };

static const TText* StateName(TInt aState)
    {
    return KStateNames[aState];
    }

const TText* const KRStateNames[] = // must agree with TRunningState
            {
            _S("ERStateRunning"), 
            _S("ERStatePaused"), 
            _S("ERStateFinishing"), 
            _S("ERStateFinished"), 
            _S("ERStateFailed"), 
            };

static const TText* RStateName(TInt aState)
    {
    return KRStateNames[aState];
    }

#endif // SYMBIAN_SWCODEC_LOGGING

#ifdef _DEBUG

static void Panic(TInt aPanic)
    {
    _LIT(KPanicString, "SwAudioInput");
    User::Panic(KPanicString, aPanic);
    }

void CAudioInput::CheckFullInvariant()
    {
    CheckInvariant();
    CheckActiveRecorders();
    }

void CAudioInput::CheckInvariant(TBool aKnownConstructed)
    {
    // full check would be that each recorder is in one, and only one, queue.
    // However, since the queues share the same infrastructure, checking the overall length of queues
    // is correct should suffice. During construction or deletion this may obviously vary
    TInt totalLength = QLength(iIdleQueue) + QLength(iRecordingQueue) + 
           QLength(iPendingQueue) + QLength(iBusyQueue);
    if (aKnownConstructed)
        {
        __ASSERT_DEBUG(totalLength==KNumRecorders, Panic(KPanicBadTotalQueueLength));
        }
    else
        {
        __ASSERT_DEBUG(totalLength<=KNumRecorders, Panic(KPanicBadTotalQueueLength2));        
        }
    __ASSERT_DEBUG(QLength(iBusyQueue)<=1, Panic(KPanicBadTotalQueueLength));
    }

#else // _DEBUG

// inline versions that do nothing...

inline void CAudioInput::CheckFullInvariant()
	{
	}
	
inline void CAudioInput::CheckInvariant(TBool /*aKnownConstructed*/)
	{
	}

#endif // _DEBUG

const TInt KMinBufferSize = 4; // assume a good default?
//Shared chunk driver does not support max. buffer size. 16K is given in order to simulate the old driver behavior.
const TInt KMaxBufferSize = 0x4000;

//Table that maps given linear value of volume to the corresponding decibel value.
const TUint8 KLinearToDbConstantLookup[] =
                        {
                            0,            // 0
                            158,
                            170,
                            177,
                            182,
                            186,
                            189,
                            192,
                            194,
                            196,
                            198,          // 10
                            200,
                            201,
                            203,
                            204,
                            205,
                            206,
                            207,
                            208,
                            209,
                            210,          // 20
                            211,
                            212,
                            213,
                            213,
                            214,
                            215,
                            215,
                            216,
                            217,
                            217,          // 30
                            218,
                            218,
                            219,
                            219,
                            220,
                            220,
                            221,
                            221,
                            222,
                            222,          // 40
                            223,
                            223,
                            224,
                            224,
                            224,
                            225,
                            225,
                            225,
                            226,
                            226,          // 50
                            226,
                            227,
                            227,
                            227,
                            228,
                            228,
                            228,
                            229,
                            229,
                            229,          // 60
                            230,
                            230,
                            230,
                            230,
                            231,
                            231,
                            231,
                            231,
                            232,
                            232,          // 70
                            232,
                            232,
                            233,
                            233,
                            233,
                            233,
                            234,
                            234,
                            234,
                            234,          // 80
                            235,
                            235,
                            235,
                            235,
                            235,
                            236,
                            236,
                            236,
                            236,
                            236,          // 90
                            237,
                            237,
                            237,
                            237,
                            237,
                            237,
                            238,
                            238,
                            238,
                            238,          // 100
                            238,
                            239,
                            239,
                            239,
                            239,
                            239,
                            239,
                            240,
                            240,
                            240,          // 110
                            240,
                            240,
                            240,
                            240,
                            241,
                            241,
                            241,
                            241,
                            241,
                            241,          // 120
                            241,
                            242,
                            242,
                            242,
                            242,
                            242,
                            242,
                            242,
                            243,
                            243,          // 130
                            243,
                            243,
                            243,
                            243,
                            243,
                            244,
                            244,
                            244,
                            244,
                            244,          // 140
                            244,
                            244,
                            244,
                            245,
                            245,
                            245,
                            245,
                            245,
                            245,
                            245,          // 150
                            245,
                            245,
                            246,
                            246,
                            246,
                            246,
                            246,
                            246,
                            246,
                            246,          // 160
                            246,
                            247,
                            247,
                            247,
                            247,
                            247,
                            247,
                            247,
                            247,
                            247,          // 170
                            247,
                            248,
                            248,
                            248,
                            248,
                            248,
                            248,
                            248,
                            248,
                            248,          // 180
                            248,
                            249,
                            249,
                            249,
                            249,
                            249,
                            249,
                            249,
                            249,
                            249,          // 190
                            249,
                            250,
                            250,
                            250,
                            250,
                            250,
                            250,
                            250,
                            250,
                            250,          // 200
                            250,
                            250,
                            250,
                            251,
                            251,
                            251,
                            251,
                            251,
                            251,
                            251,          // 210
                            251,
                            251,
                            251,
                            251,
                            251,
                            252,
                            252,
                            252,
                            252,
                            252,          // 220
                            252,
                            252,
                            252,
                            252,
                            252,
                            252,
                            252,
                            252,
                            253,
                            253,          // 230
                            253,
                            253,
                            253,
                            253,
                            253,
                            253,
                            253,
                            253,
                            253,
                            253,          // 240
                            253,
                            254,
                            254,
                            254,
                            254,
                            254,
                            254,
                            254,
                            254,
                            254,          // 250
                            254,
                            254,
                            254,
                            254,
                            254
                        };

// rate lookup table

const TInt KNumSampleRates = 9;

struct TSampleRateEnumTable
    {
    TInt iRate;
    TSoundRate iRateEnum;
    TUint iRateConstant;
    };
//Table that maps given samples per second to the corresponding enums in RSoundSc
const TSampleRateEnumTable KRateEnumLookup[] =
    {
        {48000,ESoundRate48000Hz,KSoundRate48000Hz},
        {44100,ESoundRate44100Hz,KSoundRate44100Hz},
        {32000,ESoundRate32000Hz,KSoundRate32000Hz},
        {24000,ESoundRate24000Hz,KSoundRate24000Hz},
        {22050,ESoundRate22050Hz,KSoundRate22050Hz},
        {16000,ESoundRate16000Hz,KSoundRate16000Hz},
        {12000,ESoundRate12000Hz,KSoundRate12000Hz},
        {11025,ESoundRate11025Hz,KSoundRate11025Hz},
        {8000, ESoundRate8000Hz, KSoundRate8000Hz}
    };

// TAudioInputParams

EXPORT_C TAudioInputParams::TAudioInputParams() :
    iSampleRate(0), iNominalBufferSize(0)
    {
    // none	
    }

// CAudioInput

EXPORT_C MAudioInput* MAudioInput::CreateL(MAudioInputObserver& aObserver)
    {
    MAudioInput* result = CAudioInput::NewL(aObserver);
    return result;
    }

CAudioInput* CAudioInput::NewL(MAudioInputObserver& aObserver)
    {
    CAudioInput* result = new CAudioInput(aObserver);
    CleanupStack::PushL(result);
    result->ConstructL();
    CleanupStack::Pop(result);
    return result;
    }

CAudioInput::CAudioInput(MAudioInputObserver& aObserver) :
    iObserver(aObserver),
    iIdleQueue(_FOFF(CRecorder,iLink)),
    iRecordingQueue(_FOFF(CRecorder,iLink)),
    iPendingQueue(_FOFF(CRecorder,iLink)),
    iBusyQueue(_FOFF(CRecorder,iLink))
    {
    ASSERT(iState == EStateCreated2); // assume zero'ing initialises correctly
    }

void CAudioInput::Release()
// effective destructor call
    {
    delete this;
    }

TAny* CAudioInput::Interface(TUid aInterfaceUid)
    {
    if (aInterfaceUid == KUidAIParamInterface)
        {
        MAIParamInterface* self = this;
        return self;
        }
    return NULL;
    }

RSoundSc& CAudioInput::RecordSoundDevice()
    {
    ASSERT(iRecordSoundDevice.Handle()!=0); // should be open
    return iRecordSoundDevice;
    }

CAudioInput::~CAudioInput()
    {
    CheckInvariant(EFalse); // may not be constructed
    Cancel();
    for (TInt i = 0; i < KNumRecorders; i++)
        {
        // just in case, call cancel directly from this point too
        // Cancel depends on the active queue, and might not be quite the same.
        CRecorder* recorder = iRecorders[i];
        if (recorder)
            {
            recorder->Cancel();
            }
        delete recorder;
        }
    delete iAsyncCallBack;
    iConvBuff.Close();
    iRecordSoundDevice.Close();
    iChunk.Close();
    }

void CAudioInput::Cancel()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Cancel()"));
#endif
    CancelRecorders();
    if (iAsyncCallBack)
        {
        iAsyncCallBack->Cancel();
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Cancel()"));
#endif
    }

void CAudioInput::CancelRecorders()
// if a recorder is active, then cancel it. Also move the list if required.
    {
    CheckInvariant(); // semi-invariant check - this is called from destructor
    
    CRecorder* recorder;
    while (QPop(recorder, iRecordingQueue))
        {
        recorder->Cancel();
        iIdleQueue.AddLast(*recorder);
        }
    CheckFullInvariant();
    }

void CAudioInput::CancelPendingRecorders()
// take any recorder in the pending queue. ack the buffer and send to idle
    {
    CheckFullInvariant();
    
    CRecorder* recorder;
    while (QPop(recorder, iPendingQueue))
        {
        recorder->ReleaseBuffer();
        iIdleQueue.AddLast(*recorder);
        }
    CheckFullInvariant();
    }

void CAudioInput::CancelBusyRecorder()
// take busy recorder. ack the buffer and send to idle
    {
    CheckFullInvariant();
    
    CRecorder* recorder;
    if (QPop(recorder, iBusyQueue))
        {
        recorder->ReleaseBuffer();
        iIdleQueue.AddLast(*recorder);
        }
    CheckFullInvariant();
    }

void CAudioInput::RecordAllIdle()
// take any recorder in idle queue and set recording
    {
    CheckFullInvariant();
    
    CRecorder* recorder;
    while (QPop(recorder, iIdleQueue))
        {
        recorder->RecordData();
        iRecordingQueue.AddLast(*recorder);
        }
    CheckFullInvariant();    
    }

void CAudioInput::ConstructL()
    {
    for (TInt i = 0; i < KNumRecorders; i++)
        {
        iRecorders[i] = new (ELeave) CRecorder(*this, i);
        iIdleQueue.AddLast(*(iRecorders[i]));
        }
    iAsyncCallBack = new (ELeave) CAsyncCallBack(CActive::EPriorityStandard);
    TCallBack callback(Callback, this);
    iAsyncCallBack->Set(callback);
    User::LoadPhysicalDevice(KPddFileName);
    User::LoadLogicalDevice(KLddFileName);
    CheckFullInvariant();
    }

TInt CAudioInput::Initialize(const TAudioInputParams& aParams)
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Initialize() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    TInt error = KErrNone;
    if (iState == EStateCreated2)
        {
        if (!iRecordSoundDevice.Handle())
            {
            error = iRecordSoundDevice.Open(KSoundScRxUnit0);
            if (error)
                {
                Close(); // TODO Close() required?
                }
            }
        if (!error)
            {
            iBufferLength = aParams.iNominalBufferSize; // will be updated by SetFormat() if required
            error = SetFormat(aParams);
            if (!error)
                {
                iRecordBufferConfig.iNumBuffers = KNumRecorders*2; // for each AO we create two buffers
                iRecordBufferConfig.iFlags = 0;
                iRecordBufferConfig.iBufferSizeInBytes = iBufferLength;
                ASSERT(iChunk.Handle()==0); // should not be already open
                TPckg<TRecordSharedChunkBufConfig> bufferConfigBuf(
                        iRecordBufferConfig);
                error = iRecordSoundDevice.SetBufferChunkCreate(
                        bufferConfigBuf, iChunk);
#ifdef SYMBIAN_SWCODEC_LOGGING
                RDebug::Print(
                        _L("iRecordBufferConfig.iNumBuffers =  [%d]"),iRecordBufferConfig.iNumBuffers);
                RDebug::Print(
                        _L("iRecordBufferConfig.iFlags =  [%d]"),iRecordBufferConfig.iFlags);
                RDebug::Print(
                        _L("iRecordBufferConfig.iBufferSizeInBytes =  [%d]"),iRecordBufferConfig.iBufferSizeInBytes);
#endif			
                if (error == KErrNone)
                    {
					ASSERT(iChunk.Handle()); // should now be open
                    iRecordSoundDevice.GetBufferConfig(bufferConfigBuf); // overwrite iRecordBufferConfig
					SetGain(aParams.iInitialGain);
                    iState = EStateInitialized2;
                    }
                }
            }
        }
    else
        {
        error = KErrNotReady;
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Initialize(%d) state=%s rstate=%s"), error, StateName(
            iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    return error;
    }

void CAudioInput::Close()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Close() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    InternalStop(); // Technically this should not be required, as client should Stop() first, but just in case
    if (iState == EStateInitialized2)
        {
        iRecordSoundDevice.Close();
        iChunk.Close();
        iConvBuff.Close();
        iState = EStateCreated2;
        }
    ASSERT(iState==EStateCreated2);
    ASSERT(QLength(iIdleQueue)==KNumRecorders);
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Close() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    }

TInt CAudioInput::Start()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Start() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    TInt error = KErrNone;
    if (iState == EStateInitialized2)
        {
        RecordAllIdle();
        iState = EStateRecordWait2;
        iRState = ERStateRunning;
        }
    else
        {
        error = KErrNotReady;
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Start(%d) state=%s rstate=%s"), 
            error, StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    return error;
    }

void CAudioInput::BufferAck()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::BufferAck() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    ASSERT(iState==EStateRecordWaitAck2);
    HandleBufferAck();
    iState = EStateRecordWait2;
    RequestCallback();
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::BufferAck() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    }

void CAudioInput::HandleBufferAck()
    {
    CRecorder* recorder = QPop(iBusyQueue);
    recorder->ReleaseBuffer();
    if (iRState == ERStateRunning)
        {
        recorder->RecordData();
        iRecordingQueue.AddLast(*recorder);
        }
    else
        {
        iIdleQueue.AddLast(*recorder);
        if (iRState == ERStatePaused && (QLength(iRecordingQueue)+QLength(iPendingQueue) == 0))
            {
            iRState = ERStateFinishing;
            }
        }
    }

TInt CAudioInput::Pause()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Pause() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    TInt err = KErrNone; // note we are silent if called in wrong state
    
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(
                _L("***!pause irecordingquelength %d pendingquelength %d ibusyquelength=%d"), QLength(iRecordingQueue),
                QLength(iPendingQueue), QLength(iBusyQueue));
#endif
    if ((iState == EStateRecordWait2 || iState == EStateRecordWaitAck2) && iRState==ERStateRunning)
        {
        iRecordSoundDevice.Pause();

        iRState = ERStatePaused;
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Pause(%d) state=%s err=%d"), err, StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    return err; 
    }

TInt CAudioInput::Resume()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Resume() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    TInt err = KErrNone; // note we are silent if called in the wrong state
    if ((iState == EStateRecordWait2 || iState == EStateRecordWaitAck2) && 
            ((iRState==ERStatePaused || iRState==ERStateFinishing || iRState==ERStateFinished)))
        {
        err = RecordSoundDevice().Resume();
        
        RecordAllIdle();
        
        iRState = ERStateRunning;
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Resume(%d) state=%s rstate=%s"), err, StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    return err; 
    }

TInt CAudioInput::Flush()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Flush() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    TInt error = KErrNotReady;
    if (iRState==ERStatePaused)
        {
        if (iState == EStateRecordWait2)
            {
            InternalFlush();
            ASSERT(iState == EStateRecordWait2); // stay put
            error = KErrNone;
            }
        else if (iState == EStateRecordWaitAck2)
            {
            InternalFlush();
            iState = EStateRecordWait2;
            error = KErrNone;
            }       
        }
 #ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Flush(%d) state=%s rstate=%s"), error, StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    return error;
    }

void CAudioInput::Stop()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::Stop() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    InternalStop();
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::Stop() state=%s rstate=%s"), StateName(iState), RStateName(iRState));
#endif
    }

void CAudioInput::InternalStop()
// This stops all recording and returns pending and busy buffers to idle. Must be called when
// client knows the buffer has been grabbed (e.g. _not_ before error callback)
    {
    CheckInvariant(); // Can be called from buffer error, so can't check full invariant.
    if (iState != EStateInitialized2 && iState != EStateCreated2)
        {
        InternalFlush();
        iState = EStateInitialized2;
        }
    CheckFullInvariant();
    ASSERT((QLength(iRecordingQueue) + QLength(iPendingQueue) + 
					QLength(iBusyQueue))==0); // everything is stopped 
    }

void CAudioInput::InternalFlush()
    {
    CancelRecorders();
    CancelPendingRecorders();
    CancelBusyRecorder();
    }

void CAudioInput::BufferArrives(CRecorder* aRecorder)
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(
            _L("--->CAudioInput::BufferArrives(%d,%d) state=%s rstate=%s"), aRecorder->Index(),
            aRecorder->StatusOrOffset(), StateName(iState), RStateName(iRState));
#endif
    CheckInvariant(); // Can't use CheckFullInvariant() from RunL
    ASSERT(iState==EStateRecordWait2 || iState==EStateRecordWaitAck2);
    ASSERT(aRecorder->Offset()>=0); // assert we're not here due to an error
    iRecordingQueue.Remove(*aRecorder);
    iPendingQueue.AddLast(*aRecorder);
    if (iState==EStateRecordWait2)
        {
        RequestCallback();
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::BufferArrives() state=%s rstate=%s"), 
            StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    }

void CAudioInput::UseBuffer(CRecorder* aRecorder)
// incomming buffer is pointed to by iBufPtr. Either directly or via convert, use for callback
    {
    iBufPtr.Set(iChunk.Base() + aRecorder->Offset(), aRecorder->Length());
    if (iConverter)
        {
#ifdef SYMBIAN_SWCODEC_LOGGING
        RDebug::Print(_L("iBufPtr length [%d] iconvbuff length [%d,%d]"),
                iBufPtr.Length(), iConvBuff.Length(), iConvBuff.MaxLength());
#endif
        __DEBUG_ONLY(TInt converted =) iConverter->Convert(iBufPtr, iConvBuff);
        // the following assert should check we convert the log. 
        // Actually we sometimes fail at the end of the operation with what is effectively
        // the last buffer. Arguably a driver fault, but there we are
        // ASSERT(converted==iBufPtr.Length());  
#ifdef SYMBIAN_SWCODEC_LOGGING
        RDebug::Print(_L("iBufPtr length [%d] iconvbuff length after [%d,%d]"),
        iBufPtr.Length(), iConvBuff.Length(), iConvBuff.MaxLength());
#endif
        iObserver.InputBufferAvailable(iConvBuff);
        }
    else
        {
        iObserver.InputBufferAvailable(iBufPtr);
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("12345 ibufptr =  [0x%x]"),iBufPtr.Ptr());
#endif
    }

void CAudioInput::BufferError(CRecorder* aRecorder, TInt aError)
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(
            _L("--->CAudioInput::BufferError(%d,%d) state=%s rstate=%s"), aRecorder->Index(),
            aError, StateName(iState), RStateName(iRState));
#endif
    CheckInvariant(); // Can't use CheckFullInvariant() from RunL
    if (aError==KErrCancel || aError==KErrOverflow)
        {
        // Cancel: sign of a Pause operation. If paused etc, then merely add to idle list. potentially generate finished signal
        //         if not paused, then not clear but just in case request record again
        // Overflow: basically try again, but if paused merely add to idle. Check for last buffer just in case
        if (iRState!=ERStateRunning)
            {
            iRecordingQueue.Remove(*aRecorder);
            iIdleQueue.AddLast(*aRecorder);
#ifdef SYMBIAN_SWCODEC_LOGGING
            RDebug::Print(
                        _L("***! irecordingquelength %d pendingquelength %d ibusyquelength=%d"), QLength(iRecordingQueue),
                        QLength(iPendingQueue), QLength(iBusyQueue));
#endif
            if (iRState == ERStatePaused && (QLength(iRecordingQueue)+QLength(iPendingQueue)+QLength(iBusyQueue) == 0))
                {
                iRState = ERStateFinishing;
                RequestCallback();
                }
            }
        else
            {
            aRecorder->RecordData();
            }
        }
    else
        {
        iRecordingQueue.Remove(*aRecorder);
        iIdleQueue.AddLast(*aRecorder);
        iRState = ERStateFailed;
        iObserver.InputError(aError);        
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::BufferError() state=%s rstate=%s"), 
            StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    }

TInt CAudioInput::Callback(TAny* aPtr)
    {
    CAudioInput* self = static_cast<CAudioInput*> (aPtr);
    TRAPD(error,self->AsyncCallbackL());
    return error; // TODO really have to handle error
    }

void CAudioInput::RequestCallback()
    {
    // ensure iAsyncCallBack is active
    if (!iAsyncCallBack->IsActive())
        {
        iAsyncCallBack->Call();
        }
    }

void CAudioInput::AsyncCallbackL()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::AsyncCallbackL() state=%s rstate=%s"), 
            StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    ASSERT(iState==EStateRecordWait2 || iState==EStateRecordWaitAck2); // should not occur in other states. Actually ignore in 2nd
    if (iState==EStateRecordWait2)
        {
        if (QLength(iPendingQueue)>0)
            {
            ASSERT(QLength(iBusyQueue)==0);
            iState = EStateRecordWaitAck2; // change state prior to callback, in case sync call from callback
            CRecorder* recorder = QPop(iPendingQueue);
            iBusyQueue.AddLast(*recorder);
            UseBuffer(recorder);
            }
        else 
            {
            if (iRState == ERStateFinishing)
                {
                ASSERT(QLength(iRecordingQueue)+QLength(iPendingQueue)+QLength(iBusyQueue) == 0); // should be true
                iRState = ERStateFinished;
                iObserver.InputFinished();
                }
            }
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::AsyncCallbackL() state=%s rstate=%s"), 
            StateName(iState), RStateName(iRState));
#endif
    CheckFullInvariant();
    }

TInt CAudioInput::GetBufferSizes(TInt& aMinSize, TInt& aMaxSize)
    {
    aMinSize = KMinBufferSize;
    aMaxSize = KMaxBufferSize;
    return KErrNone;
    }

TInt CAudioInput::SetGain(TInt aGain)
    {
    TInt error = KErrNone; // note: silent if in wrong state
    if (iRecordSoundDevice.Handle())
        {
        // we have to switch from level to dB value
        if(aGain >=0 && aGain<=KSoundMaxVolume)
            {
            error = iRecordSoundDevice.SetVolume(KLinearToDbConstantLookup[aGain]);
            }
        else
            {
            error = KErrArgument;
            }
        }
    return error;
    }

TInt CAudioInput::SetFormat(const TAudioInputParams& aFormat)
    {
    TInt err = KErrNotFound;
    TCurrentSoundFormatV02Buf formatBuf;
    TFormatData formatData;

    delete iConverter;
    iConverter = NULL; // setting this to NULL indicates we are not using converter. No other flag

    TInt wantedRate = aFormat.iSampleRate;
    for (TInt index = 0; index < KNumSampleRates; index++)
        {
        if (wantedRate == KRateEnumLookup[index].iRate)
            {
            formatBuf().iRate = KRateEnumLookup[index].iRateEnum;
            formatData.iSampleRate = wantedRate;
            err = KErrNone;
            break;
            }
        }

    if (err == KErrNone)
        {
        formatBuf().iChannels = aFormat.iNumChannels;
        formatBuf().iEncoding = ESoundEncoding16BitPCM;
        formatBuf().iDataFormat = ESoundDataFormatInterleaved;
        err = iRecordSoundDevice.SetAudioFormat(formatBuf);
#if defined(SYMBIAN_SOUNDADAPTER_FORCECDRATES) || defined (SYMBIAN_SOUNDADAPTER_FORCESTEREO)
        err = KErrNotSupported; // force Negotiate - for debugging
#endif
        if (err == KErrNotSupported)
            {
            // don't support directly. Perhaps can rate convert?
            err = NegotiateFormat(aFormat, formatData);
            }
        }
    return err;
    }

TInt CAudioInput::NegotiateFormat(const TAudioInputParams& aFormat, TFormatData &aFormatData)
    {
    TInt err = KErrNotFound;
    TCurrentSoundFormatV02Buf formatBuf;
    
    TInt origBufferLength = iBufferLength; // cache in case we change

    // find out first what the driver supports
    TSoundFormatsSupportedV02Buf supportedFormat;
    iRecordSoundDevice.Caps(supportedFormat);
    TUint32 supportedRates = supportedFormat().iRates;
#ifdef SYMBIAN_SOUNDADAPTER_FORCECDRATES
    supportedRates &= KSoundRate11025Hz | KSoundRate22050Hz
            | KSoundRate44100Hz; // only use CD rates - for debugging
#endif

    // For RecordCase:
    //		We want the next rate above consistently - we go down from this to the requested rate.
    //		If there is one, we don't support - we _never_ upsample.
    // note that the table is given in descending order, so we start with the highest
    TInt wantedRate = aFormat.iSampleRate;
    TInt takeTheFirst = EFalse;
    TInt nextUpValidIndex = -1;
    for (TInt index = 0; index < KNumSampleRates; index++)
        {
        TBool lookingAtRequestedRate = wantedRate
                == KRateEnumLookup[index].iRate;
        TSoundRate wantedEnum = KRateEnumLookup[index].iRateEnum;
        TUint32 equivBitmap = KRateEnumLookup[index].iRateConstant;
        TBool isSupported = (equivBitmap & supportedRates) != EFalse;
        if (lookingAtRequestedRate || takeTheFirst)
            {
            if (isSupported)
                {
                // this rate is supported
                formatBuf().iRate = wantedEnum;
                aFormatData.iActualRate = KRateEnumLookup[index].iRate;
                err = KErrNone;
                break;
                }
            }
        else if (!takeTheFirst)
            {
            // while we are still looking for the rate, want to cache any supported index
            // at end of loop, this will be the first rate above ours that is supported
            // use for fallback if required
            if (isSupported)
                {
                nextUpValidIndex = index;
                }
            }
        if (lookingAtRequestedRate)
            {
            // For record we just abort.
            break;
            }
        }

    if (err)
        {
        // if there is one above the requested rate, use that
        if (nextUpValidIndex >= 0)
            {
            TSoundRate wantedEnum =
                    KRateEnumLookup[nextUpValidIndex].iRateEnum;
            formatBuf().iRate = wantedEnum;
            aFormatData.iActualRate = KRateEnumLookup[nextUpValidIndex].iRate;
            err = KErrNone;
            }
        }

    if (err)
        {
        // should have something!
        return err;
        }

    aFormatData.iSampleRate = wantedRate; // iSampleRate is our requested/apparent rate, not the device rate.

    TUint32 channelsSupported = supportedFormat().iChannels;
#ifdef SYMBIAN_SOUNDADAPTER_FORCESTEREO
    channelsSupported &= KSoundStereoChannel; // don't use mono - for debugging
#endif

    if (aFormat.iNumChannels == 1)
        {
        aFormatData.iRequestedChannels = 1;
        // want mono
        if (channelsSupported & KSoundMonoChannel)
            {
            // mono is supported, as usual
            aFormatData.iActualChannels = 1;
            }
        else if (channelsSupported & KSoundStereoChannel)
            {
            aFormatData.iActualChannels = 2;
            iBufferLength *= 2; // double size, will do stereo->mono
            }
        else
            {
            return KErrNotSupported; // should not get this far for real
            }
        }
    else if (aFormat.iNumChannels == 2)
        {
        aFormatData.iRequestedChannels = 2;
        // want stereo
        if (channelsSupported & KSoundStereoChannel)
            {
            // stereo is supported, as usual
            aFormatData.iActualChannels = 2;
            }
        else if (channelsSupported & KSoundMonoChannel)
            {
            aFormatData.iActualChannels = 1;
            iBufferLength /= 2; // halve size, will do mono->stereo
            }
        else
            {
            return KErrNotSupported; // should not get this far for real
            }
        }
    else
        {
        return KErrNotSupported; // unknown number of channels requested!
        }

    formatBuf().iChannels = aFormatData.iActualChannels;

    formatBuf().iEncoding = ESoundEncoding16BitPCM;
    formatBuf().iDataFormat = ESoundDataFormatInterleaved;
    err = iRecordSoundDevice.SetAudioFormat(formatBuf);

    if (!err)
        {
        ASSERT(!iConverter); // pre-condition at top of function anyway
        // when recording we convert from actual to requested
        TInt outputRateToUse = aFormatData.iSampleRate;
#ifdef SYMBIAN_SKIP_RESAMPLE_ON_RECORD
		// with this macro just channel convert at most
        outputRateToUse = aFormatData.iActualRate;
#endif
#ifdef SYMBIAN_SOUNDADAPTER_DEBUG
		RDebug::Print(_L("RMdaDevSound::CBody::NegotiateFormat: Convert:CreateL from %d/%d to %d/%d"),
                        aFormatData.iActualRate, aFormatData.iActualChannels,
                        aFormatData.iSampleRate, aFormatData.iRequestedChannels);
#endif																	       
        TRAP(err, iConverter = CChannelAndSampleRateConverter::CreateL(aFormatData.iActualRate,
                                aFormatData.iActualChannels,
                                outputRateToUse,
                                aFormatData.iRequestedChannels));
        }
    if (!err && iConverter)
        {
        err = iConvBuff.Create(origBufferLength);
#ifdef SYMBIAN_SWCODEC_LOGGING
        RDebug::Print(_L("iBufferLength length [%d] iconvbuff length [%d,%d]"),
        iBufferLength, iConvBuff.Length(), iConvBuff.MaxLength());
#endif
        }

    return err;
    }

TInt CAudioInput::GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates)
    {
    TInt err = KErrNone;

    if (iRecordSoundDevice.Handle())
        {
        GetSupportedSampleRates(aSupportedSampleRates, iRecordSoundDevice);
        }
    else
        {//temporarily open the device if we can
        RSoundSc tempsound;
        err = tempsound.Open(KSoundScRxUnit0);
        if (!err)
            {
            err = GetSupportedSampleRates(aSupportedSampleRates, tempsound);
            tempsound.Close();
            }
        }
    return err;
    }

TInt CAudioInput::GetSupportedSampleRates(
        RArray<TInt>& aSupportedSampleRates, RSoundSc& aSoundDevice)
    {
    ASSERT(aSoundDevice.Handle()); // parent to ensure this is open

    TInt err = KErrNone;

    TSoundFormatsSupportedV02Buf supportedFormat;
    aSoundDevice.Caps(supportedFormat);
    TUint32 rates = supportedFormat().iRates;

    for (TInt i = KNumSampleRates - 1; i > 0; i--)//min to max
        {
        if (rates & KRateEnumLookup[i].iRateConstant)
            {
            err = aSupportedSampleRates.Append(KRateEnumLookup[i].iRate);
            if (err)
                {
                break;
                }
            }
        }
    return err;
    }

TInt CAudioInput::QLength(TSglQue<CRecorder>& aQueue)
// count elements in List/Q. Have to use iterator to do this - it seems.
    {
    TSglQueIter<CRecorder> iter(aQueue);
    TInt count=0;
    while (iter++)
        {
        // like old-fashioned C string manipulations. iterate through all members
        count++;
        }
    return count;
    }

CAudioInput::CRecorder* CAudioInput::QPop(TSglQue<CRecorder>& aQueue)
    {
    CRecorder* recorder = NULL;
    if (! aQueue.IsEmpty())
        {
        recorder = aQueue.First();
        aQueue.Remove(*recorder);
        }
    return recorder;
    }
	
#ifdef _DEBUG

// these functions are used in invariant checking only

void CAudioInput::CheckActiveRecorders(TSglQue<CRecorder>& aQueue, TBool aExpected, TInt aPanicCode)
// check that all the elements in the given Q are IsActive() or vice-versa
    {
    TSglQueIter<CRecorder> iter(aQueue);
    
    CRecorder* recorder;
    while ((recorder=iter++)!=NULL)
        {
        TBool expected = aExpected != EFalse; // ensure these are either true or false
        TBool active = recorder->IsActive() != EFalse;
        __ASSERT_DEBUG(expected == active, Panic(aPanicCode));
        }
    }

void CAudioInput::CheckActiveRecorders()
// check that all the elements in the recordingQueue are IsActive() etc
// can't be used as CRecorder::RunL() pre-condition
    {
    CheckActiveRecorders(iRecordingQueue, ETrue, EPanicBusyRecorderNotActive);
    CheckActiveRecorders(iIdleQueue, EFalse, EPanicNonBusyRecorderActive);
    CheckActiveRecorders(iPendingQueue, EFalse, EPanicNonBusyRecorderActive);
    CheckActiveRecorders(iBusyQueue, EFalse, EPanicNonBusyRecorderActive);
    }

#endif // _DEBUG

//
// CRecorder
//	


CAudioInput::CRecorder::CRecorder(CAudioInput& aParent, TInt aIndex) :
    CActive(EPriorityStandard), iParent(aParent), iIndex(aIndex)
    {
    CActiveScheduler::Add(this);
    }

CAudioInput::CRecorder::~CRecorder()
    {
    Cancel();
    }

void CAudioInput::CRecorder::Cancel()
    {
    // this override takes into account that ReleaseBuffer must be called - this is not the
    // normal pattern where following Cancel() we're not concerned with the results
    if (IsActive())
        {
        ASSERT(!BufferHeld()); // if active then buffer held should be clear. don't reset then
        CActive::Cancel();
        ReleaseBuffer(ETrue); // release - might have been a successful run!
        }
    else
        {
        ReleaseBuffer(); // this will release buffer if still outstanding	
        }
    }

void CAudioInput::CRecorder::RunL()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::CRecorder::RunL(%d, %d)"), Index(),
            iStatus.Int());
#endif
    TInt errorOrOffset = iStatus.Int(); // negative -> error. non-negative is offset in chunk

    if (errorOrOffset < 0)
        {
#ifdef SYMBIAN_SWCODEC_LOGGING
        RDebug::Print(_L("errorOrOffset =  [%d]"),errorOrOffset);
#endif
        // ReleaseBuffer(ETrue); // calls ReleaseBuffer() on error code. Driver requires this, even though seems wrong
        iParent.BufferError(this, errorOrOffset);
        }
    else
        {
        ASSERT(!iBufferHeld);
        iBufferHeld = ETrue;

#ifdef SYMBIAN_SWCODEC_LOGGING
        RDebug::Print(_L("errorOrOffset =  [%d]"),errorOrOffset);
#endif
        // If a buffer larger than expected arrives truncate it.
        iLength = Min(iLength,iParent.iBufferLength);
        iParent.BufferArrives(this);
        }
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::CRecorder::RunL(%d)"), Index());
#endif
    }

void CAudioInput::CRecorder::RecordData()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::CRecorder::RecordData(%d)"), Index());
#endif
    ASSERT(!iBufferHeld);
    Deque(); // ensure we append to the AO queue, so if it comes to it we process oldest request first
    CActiveScheduler::Add(this);
    iLength = iParent.BufferLength(); // TODO do we have to set this first or is it an OUT param purely
    iParent.RecordSoundDevice().RecordData(iStatus, iLength);
    SetActive();

#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("###****#####!!!! Buffer length [%d], status [%d] "), iLength,
            iStatus.Int());
#endif
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::CRecorder::RecordData(%d)"), Index());
#endif
    }

void CAudioInput::CRecorder::DoCancel()
    {
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("--->CAudioInput::CRecorder::DoCancel(%d)"), Index());
#endif
    iParent.RecordSoundDevice().Cancel(iStatus);
#ifdef SYMBIAN_SWCODEC_LOGGING
    RDebug::Print(_L("<---CAudioInput::CRecorder::DoCancel(%d)"), Index());
#endif
    }

void CAudioInput::CRecorder::ReleaseBuffer(TBool aDoAnyway)
    {
    if (iBufferHeld || aDoAnyway)
        {
        iParent.RecordSoundDevice().ReleaseBuffer(iStatus.Int());
        iBufferHeld = EFalse;
        }
    }

TInt CAudioInput::CRecorder::Index() const
    {
    return iIndex;
    }

TInt CAudioInput::CRecorder::Length() const
    {
    return iLength;
    }

TBool CAudioInput::CRecorder::IsBusy() const
    {
    return IsActive() || BufferHeld();
    }

TBool CAudioInput::CRecorder::BufferHeld() const
// BufferHeld() means we're in control of a passed buffer
    {
    return iBufferHeld;
    }

TInt CAudioInput::CRecorder::Offset() const
// If we call this, we've discounted errors so can assert non-negative
    {
    TInt result = StatusOrOffset();
    ASSERT(result>=0); 
    return result;
    }

TInt CAudioInput::CRecorder::StatusOrOffset() const
// The iStatus assuming is valid
    {
    ASSERT(!IsActive()); // or would not be valid
    TInt result = iStatus.Int();
    return result;    
    }
