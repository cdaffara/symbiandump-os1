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
// source\server\mmfswcodecrecorddatapath.cpp
//
//

#include "mmfSwCodecRecordDataPath.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/common/mmfpaniccodes.h>

#ifdef SYMBIAN_SCW_DEBUG

const TText* const KStateNames[] = // must agree with TRecordState
            {
            _S("ERecordStateCreated"),
            _S("ERecordStateFailed"),
            _S("ERecordStateRecording"),
            _S("ERecordStateSendingBuffer"),
            _S("ERecordStateSendingPartialBuffer"),
            _S("ERecordStateEmptiedPartialBuffer"),
            _S("ERecordStateRecordingPaused"),
            _S("ERecordStateSendingBufferPaused"),
            _S("ERecordStateSendingPartialBufferPaused"),
            _S("ERecordStateEmptiedPartialBufferPaused"),
            };

static const TText* StateName(TInt aState)
    {
    return KStateNames[aState];
    }

#endif // SYMBIAN_SCW_DEBUG

// Table of next state used when resuming or pausing.
const CMMFSwCodecRecordDataPath::TRecordState CMMFSwCodecRecordDataPath::KResumePauseTable[] =
    {
    ERecordStateCreated,                    //ERecordStateCreated                       // note order here is important - see State(), RecordOrPause() etc
    ERecordStateFailed,                     //ERecordStateFailed
    ERecordStateRecordingPaused,            //ERecordStateRecording
    ERecordStateSendingBufferPaused,        //ERecordStateSendingBuffer
    ERecordStateSendingPartialBufferPaused, //ERecordStateSendingPartialBuffer
    ERecordStateEmptiedPartialBufferPaused, //ERecordStateEmptiedPartialBuffer
    ERecordStateRecording,                  //ERecordStateRecordingPaused
    ERecordStateSendingBuffer,              //ERecordStateSendingBufferPaused
    ERecordStateSendingPartialBuffer,       //ERecordStateSendingPartialBufferPaused
    ERecordStateEmptiedPartialBuffer,       //ERecordStateEmptiedPartialBufferPaused
    };


CMMFSwCodecRecordDataPath* CMMFSwCodecRecordDataPath::NewL()
	{
	CMMFSwCodecRecordDataPath* self = new(ELeave) CMMFSwCodecRecordDataPath;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CMMFSwCodecRecordDataPath::ConstructL()
	{
	iAudioInput = MAudioInput::CreateL(*this);
	TCallBack callback(Callback, this);
	iAsyncCallback = new (ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CMMFSwCodecRecordDataPath::CMMFSwCodecRecordDataPath():
		iShadowData(NULL, 0, 0)
	{
	ASSERT(iState==ERecordStateCreated); // assume this value is 0, so no need to assign
	}

CMMFSwCodecRecordDataPath::~CMMFSwCodecRecordDataPath()
	{
	if (iAudioInput)
		{
		iAudioInput->Release();
		}

	delete iCodecBuffer;
	delete iInputBuffer;
	delete iAsyncCallback;
	}


TInt CMMFSwCodecRecordDataPath::SetObserver(MMMFHwDeviceObserver& aObserver)
	{
	TInt error;
	if (iHwDeviceObserver)
		{
		error =  KErrAlreadyExists;
		}
	else
		{
		iHwDeviceObserver = &aObserver;
		error  = KErrNone;
		}
	return error;
	}


TInt CMMFSwCodecRecordDataPath::AddCodec(CMMFSwCodec& aCodec)
	{
	TInt err = KErrNone;

	if (iCodec)
		{
		err = KErrNotSupported; //doesn't support multiple codecs
		}

	if (!err)
		{
		iCodec = &aCodec;

		iSinkBufferSize = iCodec->SinkBufferSize();
		iAudioInputBufferSize = iCodec->SourceBufferSize(); // the buffer size we want from the input device

		if (!iSinkBufferSize || !iAudioInputBufferSize)
			{
			err = KErrArgument; //codec plugin has not specified buffer size
			}
		}

	if (!err)
		{
		// Allocate data buffer
		if (iCodec->IsNullCodec())
			{//don't need a separate sink buffer if null codec
			iSinkBuffer = NULL; //sink buffer is the sound device buffer
			iAudioInputBufferSize = iSinkBufferSize; // the audio input buffer becomes the sink buffer
			}
		else
			{//need a separate sink buffer for the codec - this is the buffer passed to our client
			ASSERT(!iCodecBuffer); // can't happen because can only call AddCodec once
			TRAP(err,iCodecBuffer = CMMFDataBuffer::NewL(iSinkBufferSize));
			iSinkBuffer = iCodecBuffer;
			}
		}
	if (!err)
		{
		ASSERT(!iInputBuffer); // can't happen because can only call AddCodec once
		TRAP(err,iInputBuffer = CMMFPtrBuffer::NewL());
		}
	if (!err)
		{
		// point iSinkBuffer at the right place
		if (iCodec->IsNullCodec())
			{
			iSinkBuffer = iInputBuffer;
			}
		else
			{
			iSinkBuffer = iCodecBuffer;
			}
		}
	return err;
	}


TInt CMMFSwCodecRecordDataPath::Start()
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::Start() state=%s"), StateName(iState));
#endif
	TInt err = KErrNone;
	if (!iCodec)
		{//check that a codec has been added
		err = KErrNotReady;
		}
	if (!err)
		{
		switch (iState)
		    {
		    case ERecordStateCreated:
		        {
	            TAudioInputParams params;
	            params.iInitialGain = iGain;
	            params.iSampleRate = iSampleRate;
	            params.iNumChannels = iNumChannels;
	            params.iNominalBufferSize = iAudioInputBufferSize;
	            err = iAudioInput->Initialize(params);
                if (!err)
                    {
                    err = iAudioInput->Start();
                    if (err)
                        {
                        iAudioInput->Close();
                        ASSERT(iState == ERecordStateCreated); // end up in same state
                        }
                    else
                        {
                        iState = ERecordStateRecording;
                        iInputHasFinished = EFalse;
                        iRecordedBytesCount = 0; //used for debug purposes
                        }
                    }
		        }
		        break;
		    case ERecordStateRecordingPaused:
		    case ERecordStateSendingPartialBufferPaused:
		    case ERecordStateEmptiedPartialBufferPaused:
		        {
		        // effectively in paused state, resume and switch to equivalent state
		        iAudioInput->Resume();
                iInputHasFinished = EFalse;
		        iState = KResumePauseTable[iState];
		        }
		        break;
            case ERecordStateSendingBufferPaused:
                {
				iAudioInput->Resume();
                if (iInputHasFinished)
                    {
                    iState = ERecordStateRecording; // as we follow InputHasFinished, we don't wait for the buffer
					iInputHasFinished = EFalse;
                    }
                else
                    {
                    // effectively in paused state, resume and switch to equivalent state
                    iState = KResumePauseTable[iState];
                    }
                }
                break;
            case ERecordStateFailed:
		    default:
		        {
		        // anything else assume already recording and ignore
		        }
		        break;
		    }
		}

#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::Start(%d) state=%s"), err, StateName(iState));
#endif
	return err;
	}


void CMMFSwCodecRecordDataPath::InputBufferAvailable(const TDesC8& aBuffer)
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::InputBufferAvailable(%d) state=%s"), aBuffer.Length(), StateName(iState));
#endif
	ASSERT(iState==ERecordStateRecording || iState==ERecordStateRecordingPaused);
	iInputData = &aBuffer;
    TUint length = aBuffer.Length();
	// Update bytes recorded
	iRecordedBytesCount += length;

	//buffer ok can send to sink
	iInputOffset = 0;
	TRAPD(err,ProcessBufferL(EFalse)); //convert to sink data type using codec
	if (err != KErrNone)
	    {
	    iHwDeviceObserver->Error(err);
	    }
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::InputBufferAvailable state=%s"), StateName(iState));
#endif
	}

void CMMFSwCodecRecordDataPath::InputFinished()
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::InputFinished state=%s"), StateName(iState));
#endif
    ASSERT(iState==ERecordStateRecording || iState==ERecordStateRecordingPaused);
	iInputOffset = 0;
	iInputHasFinished = ETrue;
	TRAPD(err,ProcessBufferL(ETrue)); // finish off any conversion	
	if (err != KErrNone)
	    {
	    iHwDeviceObserver->Error(err);
	    }
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::InputFinished state=%s"), StateName(iState));
#endif
	}

void CMMFSwCodecRecordDataPath::InputError(TInt aError)
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::InputBufferAvailable(%d) state=%s"), aError, StateName(iState));
#endif
	if (iState!=ERecordStateFailed)
	    {
	    iState = ERecordStateFailed;
	    if (iHwDeviceObserver)
	        {
	        // Inform the observer of the exception condition
	        // Assume it will subsequently call Stop(), and thus update policy
	        iHwDeviceObserver->Error(aError);
	        }
	    }
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::InputBufferAvailable() state=%s"), StateName(iState));
#endif
	}


/*
 *  FillSinkBufferL
 *
 *	Function to take the data from an already full source buffer and by using
 *	a codec if necessary fills the sink buffer
 *  If aLastBuffer, treat as a last buffer with zero length
 */
void CMMFSwCodecRecordDataPath::ProcessBufferL(TBool aLastBuffer)
	{
    ASSERT(iState==ERecordStateRecording || iState==ERecordStateRecordingPaused ||
           iState==ERecordStateEmptiedPartialBuffer || iState==ERecordStateEmptiedPartialBufferPaused); // only valid states
	if (iCodec->IsNullCodec())
		{//no codec so sound device buffer can be used directly as sink buffer
		ASSERT(iSinkBuffer==iInputBuffer); // just assume this
		if (aLastBuffer)
			{
			iShadowData.Set(NULL, 0, 0);
			iInputBuffer->SetPtr(iShadowData);
			iInputBuffer->SetLastBuffer(ETrue);
			}
		else
			{
			iShadowData.Set(const_cast<TUint8*>(iInputData->Ptr()), iInputData->Length(), iInputData->Length());
			iInputBuffer->SetPtr(iShadowData);
			iInputBuffer->SetLastBuffer(EFalse);
			}
		iInputBuffer->SetStatus(EFull);	//sink buffer is "full"
		TRecordState oldState = iState;
        switch (iState)
            {
            case ERecordStateRecording:
                iState = ERecordStateSendingBuffer;
                break;
            case ERecordStateRecordingPaused:
                iState = ERecordStateSendingBufferPaused;
                break;
            case ERecordStateEmptiedPartialBuffer:
            case ERecordStateEmptiedPartialBufferPaused:
                ASSERT(EFalse); // Technically these can occur but not if IsNullCodec is true, Complete is effectively always true
                break;
            }
		TInt err = iHwDeviceObserver->EmptyThisHwBuffer(*iSinkBuffer); //pass onto sink
		if (err)
            {
            iState = oldState; // rewind
            User::Leave(err);
            }
		}
	else
		{
		ASSERT(iSinkBuffer==iCodecBuffer); // sink and codec buffers are synonym, so just talk to iSinkBuffer
		if (aLastBuffer)
			{
			iShadowData.Set(NULL, 0, 0);
			iInputBuffer->SetPtr(iShadowData); // empty buffer
			iInputBuffer->SetLastBuffer(ETrue);
			}
		else
			{
			TPtrC8 tempData = iInputData->Mid(iInputOffset);
			iShadowData.Set(const_cast<TUint8*>(tempData.Ptr()), tempData.Length(), tempData.Length());
			iInputBuffer->SetPtr(iShadowData);
			iInputBuffer->SetLastBuffer(EFalse);
			}
		//pass buffer to codec for processing
		CMMFSwCodec::TCodecProcessResult codecProcessResult = iCodec->ProcessL(*iInputBuffer, *iSinkBuffer);
		if ((!iSinkBuffer->BufferSize())&&(codecProcessResult.iDstBytesAdded))
			{//the codec has added data but not set the buffer length
			iSinkBuffer->Data().SetLength(codecProcessResult.iDstBytesAdded);
			}
		//only supports EProcessComplete
		TRecordState oldState = iState;
		TInt err = KErrNone;
		switch (codecProcessResult.iCodecProcessStatus)
			{
			case CMMFSwCodec::TCodecProcessResult::EProcessComplete: //finished procesing source data - all data in sink buffer
			case CMMFSwCodec::TCodecProcessResult::EDstNotFilled: //finished procesing source data - sink buffer not full could be EOF
			case CMMFSwCodec::TCodecProcessResult::EEndOfData: //no more data - send what we've got to the sink
				{
				iSinkBuffer->SetStatus(EFull);	// treat sink buffer as full
				iState = IsPaused() ? ERecordStateSendingBufferPaused : ERecordStateSendingBuffer;
				err = EmptyBufferL();
				break;
				}
			case CMMFSwCodec::TCodecProcessResult::EProcessIncomplete:
				{
				// codec has not yet finished with input - send buffer and come back around
				iSinkBuffer->SetStatus(EFull);	// treat sink buffer as full
				iInputOffset = codecProcessResult.iSrcBytesProcessed;
                iState = IsPaused() ? ERecordStateSendingPartialBufferPaused : ERecordStateSendingPartialBuffer;
                err = EmptyBufferL();
				break;
				}
			default:
				Panic(EMMFSwCodecWrapperBadCodec); //should never get here - bad codec
			}
		if (err)
		    {
		    iState = oldState; // rewind prior to handle
		    User::Leave(err);
		    }
		}
	}

TInt CMMFSwCodecRecordDataPath::EmptyBufferL()
	{
	// This code supports an assumption made by the vorbis encoder, which assumes it can safely generate empty buffers.
	// VbrFlag here implies the vorbis encoder.
	// TODO: Replace this with a generic solution - e.g. on EDstNotFilled we request a buffer from AudioInput instead
	// 		 of calling the client back.
	if(iVbrFlag)
		{
		if(!iSinkBuffer->Data().Length() && !iInputBuffer->LastBuffer())
			{
			BufferEmptiedL(STATIC_CAST(CMMFDataBuffer&, *iSinkBuffer));
			return KErrNone;
			}
		}
	TInt err = iHwDeviceObserver->EmptyThisHwBuffer(*iSinkBuffer); //pass onto sink
	return err;
	}

void CMMFSwCodecRecordDataPath::BufferEmptiedL(const CMMFDataBuffer& aBuffer)
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::BufferEmptiedL() state=%s"), StateName(iState));
#endif
	if (&aBuffer != iSinkBuffer)
		{
		// we are only single buffering at the moment...
		Panic(EMMFSwCodecWrapperBadBuffer);
		}
	ASSERT(iState==ERecordStateSendingBuffer || iState==ERecordStateSendingBufferPaused ||
	       iState==ERecordStateSendingPartialBuffer || iState==ERecordStateSendingPartialBufferPaused ||
	       iState==ERecordStateFailed);
	switch (iState)
		{
		case ERecordStateSendingBuffer:
		case ERecordStateSendingBufferPaused:
			{
            iState = (iState==ERecordStateSendingBuffer) ? ERecordStateRecording : ERecordStateRecordingPaused;
			if (!iInputHasFinished)
			    {
	            iAudioInput->BufferAck();
			    }
			break;
			}
        case ERecordStateSendingPartialBuffer:
        case ERecordStateSendingPartialBufferPaused:
			{
            iState = (iState==ERecordStateSendingPartialBuffer) ?
                ERecordStateEmptiedPartialBuffer : ERecordStateEmptiedPartialBufferPaused;
			RequestCallback(); // go back around to ensure next callback to client is asynchronous
			break;
			}
        default:
            {
            // anything else just ignore - e.g. are waiting for Stop following an error
            }
		}
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::BufferEmptiedL() state=%s"), StateName(iState));
#endif
	}

// Async callback support - used on PartialBuffer::BufferEmptiedL() transition

void CMMFSwCodecRecordDataPath::RequestCallback()
	{
	iAsyncCallback->CallBack();
	}

TInt CMMFSwCodecRecordDataPath::Callback(TAny* aPtr)
	{
	CMMFSwCodecRecordDataPath* self = static_cast<CMMFSwCodecRecordDataPath*>(aPtr);
	return self->DoCallback();
	}

TInt CMMFSwCodecRecordDataPath::DoCallback()
	{
	ASSERT(iState==ERecordStateEmptiedPartialBuffer || iState==ERecordStateEmptiedPartialBufferPaused); // only legal ones
	TRAPD(err,ProcessBufferL(EFalse));
	if (err != KErrNone)
	    {
	    iHwDeviceObserver->Error(err);
	    }
	return err;
	}

void CMMFSwCodecRecordDataPath::Stop()
	{
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("CMMFSwcodecRecordDataPath::Stop() state=%s"), StateName(iState));
#endif
	iAudioInput->Close();
	iState = ERecordStateCreated;
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::Stop() state=%s"), StateName(iState));
#endif
	}


void CMMFSwCodecRecordDataPath::Pause()
	{
	// flush it anyway, whether we're active or not
	// if we are active, then this should result in a call to RunL() pretty soon
	//note that the Pause() in the context of record means buffers are
	//continued to be obtained from the sound driver that have already
	//been recorded - it just doesn't record any new audio data
#ifdef SYMBIAN_SCW_DEBUG
	RDebug::Print(_L("CMMFSwcodecRecordDataPath::Pause state=%s"), StateName(iState));
#endif
	switch (iState)
	    {
	    case ERecordStateRecording:
	    case ERecordStateSendingBuffer:
	    case ERecordStateSendingPartialBuffer:
	    case ERecordStateEmptiedPartialBuffer:
	        {
	        iAudioInput->Pause();
	        iState = KResumePauseTable[iState];
	        }
	        break;
	    default: ;
	        // do nothing - treat as no-op
	    }
#ifdef SYMBIAN_SCW_DEBUG
    RDebug::Print(_L("End CMMFSwcodecRecordDataPath::Pause state=%s"), StateName(iState));
#endif
	}


RMdaDevSound& CMMFSwCodecRecordDataPath::Device()
	{
	ASSERT(EFalse); // TODO should not be called - future remove if we can
	return iDummyDevSound;
	}


TUint CMMFSwCodecRecordDataPath::RecordedBytesCount()
	{
	return iRecordedBytesCount;
	}

/**
Retrieves a custom interface to the device.
The reference CMMFSwCodecWrapper supports one custom interfaces,
MSetVbrFlagCustomInterface

@param	aInterface
		Interface UID, defined with the custom interface.
		aInterface = KSetVbrFlagCustomInterfaceTypeUid for MSetVbrFlagCustomInterface.


@return A pointer to the interface implementation, or NULL if the device can not
		implement the interface requested. The return value must be cast to the
		correct type by the user.
*/
TAny* CMMFSwCodecRecordDataPath::CustomInterface(TUid aInterface)
	{
	TAny* ret = NULL;

	if(aInterface.iUid == KSetVbrFlagCustomInterfaceTypeUid)
		{
		SetVbrFlag();
		}
	else if (aInterface == KUidSwSetParamInterface)
		{
		MSwSetParamInterface* self = this;
		return self;
		}
	else if (aInterface == KUidSwInfoInterface)
	    {
        MSwInfoInterface* self = this;
        return self;
	    }
	return ret;
	}

/**
Used to set iVbrFlag on the datapath.

This method is used to set the iVbrFlag in datapath. This flag is added to datapath to avail the
alternative dataflow wherein datapath makes sure that destinationbuffer is filled to its maximum length
before sending it to the sound driver. Sending the buffer directly to the device causes underflow incase of VBR codecs.
*/
void CMMFSwCodecRecordDataPath::SetVbrFlag()
	{
	iVbrFlag = ETrue; // TODO this is seemingly redundant in a record case and could be pruned
	}

// MSwSetParamInterface - set various parameters etc

TInt CMMFSwCodecRecordDataPath::SetSampleRate(TInt aSampleRate)
	{
	iSampleRate = aSampleRate;
	return KErrNone;
	}

TInt CMMFSwCodecRecordDataPath::SetNumChannels(TInt aNumChannels)
	{
	iNumChannels = aNumChannels;
	return KErrNone;
	}

TInt CMMFSwCodecRecordDataPath::SetGain(TInt aGain)
	{
	iGain = aGain; // cache here so would be used on next Initialize()
	TInt error = KErrNone;
	if (iAudioInput)
		{
		MAIParamInterface* paramInterface = static_cast<MAIParamInterface*>(iAudioInput->Interface(KUidAIParamInterface));
		if (paramInterface)
			{
			error = paramInterface->SetGain(aGain);
			}
		}
	return error;
	}

TInt CMMFSwCodecRecordDataPath::GetBufferSizes(TInt& aMinSize, TInt& aMaxSize)
	{
	TInt error = KErrNotReady;
	if (iAudioInput)
		{
		MAIParamInterface* paramInterface = static_cast<MAIParamInterface*>(iAudioInput->Interface(KUidAIParamInterface));
		if (paramInterface)
			{
			error = paramInterface->GetBufferSizes(aMinSize, aMaxSize);
			}
		}
	return error;
	}

TInt CMMFSwCodecRecordDataPath::GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates)
    {
    TInt error = KErrNotReady;
    if (iAudioInput)
        {
        MAIParamInterface* paramInterface = static_cast<MAIParamInterface*>(iAudioInput->Interface(KUidAIParamInterface));
        if (paramInterface)
            {
            error = paramInterface->GetSupportedSampleRates(aSupportedSampleRates);
            }
        }
    return error;
    }

CMMFSwCodecRecordDataPath::TSwCodecDataPathState CMMFSwCodecRecordDataPath::State() const
    {
    // Note: code assumes stopped, record and paused states are grouped consecutively
    if (iState==ERecordStateCreated)
        {
        return EStopped;
        }
    else if (iState >= ERecordStateRecording && iState <= ERecordStateEmptiedPartialBuffer)
        {
        return EPlaying;
        }
    else
        {
        return EPaused;
        }
    }

TBool CMMFSwCodecRecordDataPath::RecordOrPause() const
    {
    // Note: code assumes stopped, record and paused states are grouped consecutively
    return iState >= ERecordStateRecording;
    }

TBool CMMFSwCodecRecordDataPath::IsPaused() const
    {
    // Note: code assumes stopped, record and paused states are grouped consecutively
    return iState >= ERecordStateRecordingPaused;
    }

// TODO - these functions are padding out from the old RMdaDevSound scheme.
// They are no longer used here, but are used for playing...

void CMMFSwCodecRecordDataPath::BufferFilledL(CMMFDataBuffer& /*aBuffer*/)
	{
	ASSERT(EFalse);
	}

void CMMFSwCodecRecordDataPath::SoundDeviceException(TInt /*aError*/)
	{
	ASSERT(EFalse);
	}

