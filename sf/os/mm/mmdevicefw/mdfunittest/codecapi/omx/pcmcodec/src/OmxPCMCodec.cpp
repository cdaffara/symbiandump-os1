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

#include <e32debug.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Audio.h>

#include "OmxImpl.h"
#include "OmxPCMCodec.h"

const TInt KPCMBufferSize = 4096;

const TInt KThreadStackSize = 16384; 

const TInt KShutDownTime = 5000000; 
		

TInt ProcessingThread(TAny* aComponent)
	{
	// get our class
	CCodecProcessor* codecprocessor = static_cast<CCodecProcessor*>(aComponent);

	// run the thread
	TRAPD(err, codecprocessor->RunThreadL());
	// thread has exited or failed to start so return error to the client. 
	return err;
	}


TInt COmxPCMCodec::CreateComponent(OMX_HANDLETYPE hComponent)
	{
	COmxPCMCodec* self = new COmxPCMCodec(hComponent);
	if (self==NULL)
		{
		return KErrNoMemory;
		}		
	TRAPD(err, self->ConstructL());
	// self is stored in the handle, so we won't return it
	return err;
	}
	
OMX_ERRORTYPE COmxPCMCodec::GetComponentVersion(
       OMX_STRING /*pComponentName*/,
       OMX_VERSIONTYPE* /*pComponentVersion*/,
       OMX_VERSIONTYPE* /*pSpecVersion*/,
       OMX_UUIDTYPE* /*pComponentUUID*/)
	{
// to be implemented
	return OMX_ErrorNone;
	}
	

void COmxPCMCodec::ConstructL()
	{
	iCodecProcessor = CCodecProcessor::NewL(*this);	
	iState = OMX_StateLoaded;
	}

COmxPCMCodec::COmxPCMCodec(OMX_HANDLETYPE hComponent)
	:COmxComponentImpl(hComponent)
	{
	}
	
COmxPCMCodec::~COmxPCMCodec()
	{
	if (iState == OMX_StateExecuting)
		{
		iCodecProcessor->Stop();
		iState = OMX_StateIdle;
		}

	if (iCreatedThread &&(iProcessingThread.Handle() != KNullHandle) && (iProcessingThread.ExitType() == EExitPending))
		{
		TRequestStatus logonStatus;
		TBool logonFailed = EFalse;
		iProcessingThread.Logon(logonStatus);
		if(logonStatus != KRequestPending)
			{//logon failed. Mostly due to no memory
			logonFailed = ETrue;
			}		
		iCodecProcessor->Exit();
		RTimer timer;
		TInt err = timer.CreateLocal();
		if(err==KErrNone && !logonFailed) //both timer and logon successful
			{
			TRequestStatus timeout;
			timer.After(timeout, KShutDownTime);
			User::WaitForRequest(logonStatus, timeout);
			if(logonStatus==KRequestPending)
				{//Thread has not exited after the timeout. Kill it!
				iProcessingThread.LogonCancel(logonStatus);
				User::WaitForRequest(logonStatus);
				iProcessingThread.Kill(KErrDied);
				}
			else 
				{//Thread exited. Cancel the timer
				timer.Cancel();
				User::WaitForRequest(timeout);
				}
			}
		else 
			{//either timer or Logon method has failed.Poll the thread status a maximum
			 // of 10 times and kill the thread if it hasn't exited after the polling
			for (TInt i=0; i<10 && iProcessingThread.ExitType() == EExitPending; ++i)
				{
				User::After(KShutDownTime/10);	// wait for a while
				}
			
			if (iProcessingThread.ExitType() == EExitPending)
				{
				// The polling hasn't been succesful so we kill the thread
				iProcessingThread.Kill(KErrDied);
				}
			if(!logonFailed)
				{
				User::WaitForRequest(logonStatus);
				}
			}	
		iProcessingThread.Close();
		}
	delete iCodecProcessor;
	}

OMX_ERRORTYPE COmxPCMCodec::SendCommand(
       OMX_COMMANDTYPE Cmd,
       TUint32 nParam1,
       TAny* /*pCmdData*/)
	{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	switch (Cmd)
		{
	case OMX_CommandStateSet:
		OMX_STATETYPE state = (OMX_STATETYPE)nParam1;
		if (state == iState)
			{
			error = OMX_ErrorSameState;
			}
		else
			{
			// notify client of the state change
			switch (state)
				{
			case OMX_StateIdle:
				{
				if (iState == OMX_StateExecuting)
					{
					iCodecProcessor->Stop();
					}
				break;
				}
			case OMX_StateExecuting:
				StartExecution();
				break;
				};
	
			iState = state;
			
			EventHandlerCallback(
				OMX_EventCmdComplete,
				OMX_CommandStateSet,
				iState,
				NULL);	
			break;
			}
		};	
	return error;
	}
	
OMX_ERRORTYPE COmxPCMCodec::GetParameter(
       OMX_INDEXTYPE nParamIndex,  
       TAny* ComponentParameterStructure)
	{
	switch (nParamIndex)
		{
	case OMX_IndexParamAudioInit :
		{
		OMX_PORT_PARAM_TYPE* param = static_cast<OMX_PORT_PARAM_TYPE*>(ComponentParameterStructure);
		param->nPorts = 2;
		}
		break;
	case OMX_IndexParamPortDefinition:
		{
		OMX_PARAM_PORTDEFINITIONTYPE* portDef = static_cast<OMX_PARAM_PORTDEFINITIONTYPE*>(ComponentParameterStructure);
		if (portDef->nPortIndex==0)	
			{
			portDef->eDir = OMX_DirInput;	
			portDef->nBufferSize = KPCMBufferSize;
			}
		else
			{
			portDef->eDir = OMX_DirOutput;
			portDef->nBufferSize = KPCMBufferSize;
			}
		}
		break;
	default:
		return OMX_ErrorUnsupportedIndex;
		}
	return OMX_ErrorNone;
	}
	
OMX_ERRORTYPE COmxPCMCodec::SetParameter(
       OMX_INDEXTYPE nIndex,
       TAny* ComponentParameterStructure)
	{
	ASSERT(iState == OMX_StateLoaded);
	switch (nIndex)
		{
		case OMX_IndexParamAudioPcm:
			{
			OMX_AUDIO_PARAM_PCMMODETYPE* param = static_cast<OMX_AUDIO_PARAM_PCMMODETYPE*>(ComponentParameterStructure);
			switch(param->nPortIndex)
				{
				case 0: // Input port
					{
					iCodecProcessor->SetInputBitsPerSample(param->nBitPerSample);
					iCodecProcessor->SetInputDataType(param->eNumData);
					//break;
					return OMX_ErrorNone;
					}			
				case 1: // Output port
					{
					iCodecProcessor->SetOutputBitsPerSample(param->nBitPerSample);
					iCodecProcessor->SetOutputDataType(param->eNumData);
					//break;	
					return OMX_ErrorNone;
					}
				default:
					{
					return OMX_ErrorUnsupportedIndex;	
					}
				};
			}
		default:
			{
			return OMX_ErrorUnsupportedIndex;
			}
		};		
	//return OMX_ErrorNone;
	}
	
OMX_ERRORTYPE COmxPCMCodec::GetConfig(
       OMX_INDEXTYPE /*nIndex*/, 
       TAny* /*value*/)
	{
	return OMX_ErrorUnsupportedIndex;
	}
	
OMX_ERRORTYPE COmxPCMCodec::SetConfig(
       OMX_INDEXTYPE /*nIndex*/, 
       TAny* /*value*/)
	{
	return OMX_ErrorUnsupportedIndex;
	}
	
OMX_ERRORTYPE COmxPCMCodec::GetExtensionIndex(
       OMX_STRING /*ParameterName*/,
       OMX_INDEXTYPE* /*pIndexType*/)
	{
	return OMX_ErrorNotImplemented;
	}
	
OMX_ERRORTYPE COmxPCMCodec::GetState(
       OMX_STATETYPE* pState)
	{
	*pState = iState;
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE COmxPCMCodec::ComponentTunnelRequest(
		OMX_HANDLETYPE /*hInput*/,
		TUint32 /*nInputPort*/,
		OMX_HANDLETYPE /*hOutput*/,
		TUint32 /*nOutputPort*/,
		OMX_TUNNELSETUPTYPE* /*pTunnelSetup*/)
	{
	return OMX_ErrorNotImplemented;
	}
	
OMX_ERRORTYPE COmxPCMCodec::UseBuffer(
       OMX_BUFFERHEADERTYPE** ppBufferHeader,
       TUint32 /*nPortIndex*/,
       TAny* pAppPrivate,
       TUint32 nSizeBytes,
       TUint8* pBuffer)
	{
	ASSERT(iState == OMX_StateLoaded);
	*ppBufferHeader = new OMX_BUFFERHEADERTYPE;
	if (*ppBufferHeader != NULL)
		{
		(*ppBufferHeader)->pBuffer = pBuffer;
		(*ppBufferHeader)->pAppPrivate = pAppPrivate;
		(*ppBufferHeader)->nAllocLen = nSizeBytes;
		(*ppBufferHeader)->nFilledLen = 0;
		(*ppBufferHeader)->nFlags = 0;
		(*ppBufferHeader)->pInputPortPrivate = NULL;
		(*ppBufferHeader)->pOutputPortPrivate = NULL;
		}
		
	if (*ppBufferHeader)
		{
		return OMX_ErrorNone;
		}
	else
		{
		return OMX_ErrorInsufficientResources;
		}
	}
	
OMX_ERRORTYPE COmxPCMCodec::AllocateBuffer(
		OMX_BUFFERHEADERTYPE** pBuffer,
		TUint32 nPortIndex,
		TAny* pAppData,
		TUint32 nSizeBytes)
	{
	ASSERT(iState == OMX_StateLoaded);
	
	*pBuffer = new OMX_BUFFERHEADERTYPE;
	if (*pBuffer != NULL)
		{
		(*pBuffer)->pBuffer = new unsigned char[nSizeBytes];
		// store our allocated memory in component's private store
		switch (nPortIndex)
			{
		case 0:
			(*pBuffer)->pInputPortPrivate = (*pBuffer)->pBuffer;
			(*pBuffer)->pOutputPortPrivate = NULL;
			break;
		case 1:
			(*pBuffer)->pOutputPortPrivate = (*pBuffer)->pBuffer;
			(*pBuffer)->pInputPortPrivate = NULL;
			break;
			};
		
		
		(*pBuffer)->nAllocLen = nSizeBytes;
		(*pBuffer)->nFilledLen = 0;
		(*pBuffer)->pAppPrivate = pAppData;
		}
		
	if (*pBuffer && (*pBuffer)->pBuffer)
		{
		return OMX_ErrorNone;
		}
	else
		{
		return OMX_ErrorInsufficientResources;
		}
	}

OMX_ERRORTYPE COmxPCMCodec::FreeBuffer(
		TUint32 /*nPortIndex*/,
       OMX_BUFFERHEADERTYPE* pBuffer)
	{
	if (pBuffer->pInputPortPrivate || 
		pBuffer->pOutputPortPrivate)
		delete[] pBuffer->pBuffer;
	delete pBuffer;
	return OMX_ErrorNone;
	}
OMX_ERRORTYPE COmxPCMCodec::EmptyThisBuffer(
       OMX_BUFFERHEADERTYPE* pBuffer)
	{
	ASSERT(iState == OMX_StateExecuting ||
			iState == OMX_StateIdle ||
			iState == OMX_StatePause);
	return iCodecProcessor->EmptyThisBuffer(pBuffer);
	}
OMX_ERRORTYPE COmxPCMCodec::FillThisBuffer(
           OMX_BUFFERHEADERTYPE* pBuffer)
	{
	ASSERT(iState == OMX_StateExecuting ||
			iState == OMX_StateIdle ||
			iState == OMX_StatePause);
	return iCodecProcessor->FillThisBuffer(pBuffer);	
	}
	
OMX_ERRORTYPE COmxPCMCodec::SetCallbacks(
           OMX_CALLBACKTYPE* pCallbacks, 
           TAny* pAppData)
	{
	iCallback = pCallbacks;
	iAppData = pAppData;
	return OMX_ErrorNone;
	}
	
	
CCodecProcessor::CCodecProcessor(COmxPCMCodec& aParent) 
	: iParent(&aParent)
	{
	}

void CCodecProcessor::RunThreadL()
	{
	iQueueStatus = KRequestPending;
	iMessageQueue.NotifyDataAvailable(iQueueStatus);
	
	for (;;)
		{
		User::WaitForRequest(iQueueStatus);
		TCodecMessage msg;
		
		TBool exit = EFalse;
		
		while (iMessageQueue.Receive(msg)==KErrNone)
			{
			switch (msg.iType)
				{
				case EStopProcessing:
					iStarted = EFalse;
					break;
				case EExit:
					exit = ETrue;
					break;
				case EInputBuffer:
					iBuffersToEmpty.Append(msg.iBuffer); 
					break;
				case EOutputBuffer:
					iBuffersToFill.Append(msg.iBuffer);
					break;
				}
			}
			
		if (exit)
			{
			break;
			}
		else
			{
			// process all available buffers
			ProcessAvailableBuffers();	
			
			// request notification of further queue events
			iQueueStatus = KRequestPending;
			iMessageQueue.NotifyDataAvailable(iQueueStatus);
			}
		}

	}


CCodecProcessor* CCodecProcessor::NewL(COmxPCMCodec& aParent) 
	{
	CCodecProcessor* self = new (ELeave) CCodecProcessor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}	
	
	
void CCodecProcessor::ConstructL()
	{
	User::LeaveIfError(iMessageQueue.CreateLocal(10));
	// set the default case	
	iInputBitsPerSample = 8;
	iInputDataType = OMX_NumericalDataUnsigned;
	iOutputBitsPerSample = 16;
	iOutputDataType = OMX_NumericalDataSigned;
	}
	
OMX_ERRORTYPE CCodecProcessor::EmptyThisBuffer( 
		OMX_BUFFERHEADERTYPE* pBuffer) 
	{
	TCodecMessage message;
	message.iType = EInputBuffer;
	message.iBuffer = pBuffer;
	if (iMessageQueue.Send(message) == KErrNone)
		{
		return OMX_ErrorNone;	
		}
	else
		{
		return OMX_ErrorUndefined;
		}
	}
	
void CCodecProcessor::Stop()
	{
	TCodecMessage message;
	message.iType = EStopProcessing;
	message.iBuffer = NULL;
	iMessageQueue.Send(message);
	}
	
void CCodecProcessor::Exit()
	{
	TCodecMessage message;
	message.iType = EExit;
	message.iBuffer = NULL;
	iMessageQueue.SendBlocking(message);
	}
	
OMX_ERRORTYPE CCodecProcessor::FillThisBuffer(
									OMX_BUFFERHEADERTYPE* pBuffer) 
	{
	TCodecMessage message;
	message.iType = EOutputBuffer;
	message.iBuffer = pBuffer;
	if (iMessageQueue.Send(message)== KErrNone)
		{
		return OMX_ErrorNone;
		}
	else
		{
		return OMX_ErrorUndefined;
		}
	}
	
void CCodecProcessor::SetInputBitsPerSample(TInt aInputBitsPerSample)
	{
	iInputBitsPerSample = aInputBitsPerSample;
	}
	
void CCodecProcessor::SetInputDataType(OMX_NUMERICALDATATYPE aType)
	{
	iInputDataType = aType;
	}
	
void CCodecProcessor::SetOutputBitsPerSample(TInt aInputBitsPerSample)
	{
	iOutputBitsPerSample = aInputBitsPerSample;
	}
	
void CCodecProcessor::SetOutputDataType(OMX_NUMERICALDATATYPE aType)
	{
	iOutputDataType = aType;
	}

void CCodecProcessor::ChooseCodec()
	{
	// choose correct conversion codec
	if (iInputBitsPerSample == 8 && iOutputBitsPerSample == 16)
		{
		iOutputSamplesPerInputSample = 2;
		if (iInputDataType == OMX_NumericalDataSigned &&
		iOutputDataType == OMX_NumericalDataSigned)
			{
			iCurrentCodec = &iAudioS8ToS16PcmCodec;
			}
		else if (iInputDataType == OMX_NumericalDataUnsigned &&
			iOutputDataType == OMX_NumericalDataSigned)
			{
			iCurrentCodec = &iAudioU8ToS16PcmCodec;
			}
		}
	else if (iInputBitsPerSample == 16 && iOutputBitsPerSample == 8)
		{
		iOutputSamplesPerInputSample = .5;
		if (iInputDataType == OMX_NumericalDataSigned &&
		iOutputDataType == OMX_NumericalDataSigned)
			{
			iCurrentCodec = &iAudioS16ToS8PcmCodec;
			}
		else if (iInputDataType == OMX_NumericalDataSigned &&
			iOutputDataType == OMX_NumericalDataUnsigned)
			{
			iCurrentCodec = &iAudioS16ToU8PcmCodec;
			}
		}
	
	}

void CCodecProcessor::ProcessAvailableBuffers()
	{
	// Setup wait for data in queue
	while (iBuffersToFill.Count()>0 && iBuffersToEmpty.Count() > 0)
		{
		TBool lastBuffer = EFalse;
		if (!iStarted)
			{
			ChooseCodec();
			iStarted = ETrue;
			}
		
		OMX_BUFFERHEADERTYPE* srcBuffer = iBuffersToEmpty[0];
		OMX_BUFFERHEADERTYPE* destBuffer = iBuffersToFill[0];
		if (srcBuffer->nFlags & OMX_BUFFERFLAG_EOS)
			{
			lastBuffer = ETrue;
			}
		TInt destBufferPos = destBuffer->nFilledLen;
		
		TInt destBufferSize = destBuffer->nAllocLen - destBufferPos;
		TInt inputSamplesRequired = (TInt)((TReal)destBufferSize / iOutputSamplesPerInputSample);
		
		TInt availableSamples = srcBuffer->nFilledLen - iInputBufferPos;
		
		if (availableSamples <= inputSamplesRequired)
			{
			TInt samplesToConvert = availableSamples;
			if (iOutputSamplesPerInputSample == .5)
				{
				samplesToConvert >>= 1;
				}
			iCurrentCodec->Convert(&srcBuffer->pBuffer[iInputBufferPos], &destBuffer->pBuffer[destBufferPos], samplesToConvert);
			iInputBufferPos = 0; // finished buffer - so reset
			inputSamplesRequired -= availableSamples;
			destBuffer->nFilledLen = (TInt)((TReal)availableSamples * iOutputSamplesPerInputSample);
			srcBuffer->nFilledLen = 0;
			iBuffersToEmpty.Remove(0);
			iParent->EmptyBufferDoneCallback(srcBuffer);			
			
			if (inputSamplesRequired == 0 || lastBuffer)
				{
				iBuffersToFill.Remove(0);
				if (lastBuffer)
					{
					destBuffer->nFlags |= OMX_BUFFERFLAG_EOS;
					// propagate the EOS flag
					iParent->EventHandlerCallback(
						OMX_EventBufferFlag,
						0,
						destBuffer->nFlags,
						NULL);	
					}
				iParent->FillBufferDoneCallback(destBuffer);
				}
			}
		else
			{
			TInt samplesToConvert = inputSamplesRequired;
			if (iOutputSamplesPerInputSample == .5)
				{
				samplesToConvert >>= 2;
				}

			iCurrentCodec->Convert(&srcBuffer->pBuffer[iInputBufferPos], &destBuffer->pBuffer[destBufferPos], samplesToConvert);
			iInputBufferPos += inputSamplesRequired;
			destBuffer->nFilledLen = destBuffer->nAllocLen;
			iBuffersToFill.Remove(0);
			iParent->FillBufferDoneCallback(destBuffer);
			}		
		}
	}
	
CCodecProcessor::~CCodecProcessor()
	{
	iBuffersToEmpty.Close();
	iBuffersToFill.Close();
	iMessageQueue.Close();
	}
	
	
TInt COmxPCMCodec::StartExecution()
	{
	// create thread with current thread's heap
	// we can thus allocate and free memory across threads
	if (!iCreatedThread)
		{
		TInt err = iProcessingThread.Create(_L("PCMCodec"), 
							&ProcessingThread, 
							KThreadStackSize, 
							&User::Heap(),
							iCodecProcessor);
							
		if (err!=KErrNone)
			{
			return err;
			}
		iCreatedThread = ETrue;
		iThreadDeath = KRequestPending;
		iProcessingThread.Resume();
		}

	return KErrNone;						
	}

// Callbacks for the PCM codec
void COmxPCMCodec::EventHandlerCallback( 
        			OMX_OUT OMX_EVENTTYPE eEvent, 
        			OMX_OUT TUint32 nData1,
        			OMX_OUT TUint32 nData2,
        			OMX_OUT OMX_STRING cExtraInfo)
	{
	iCallback->EventHandler(
			this,
			iAppData,
			eEvent,
			nData1,
			nData2,
			cExtraInfo);	
	}
	
	
void COmxPCMCodec::FillBufferDoneCallback(OMX_BUFFERHEADERTYPE* aBuffer)
	{
	iCallback->FillBufferDone(
		*this,
		iAppData,
		aBuffer);
	}
	
void COmxPCMCodec::EmptyBufferDoneCallback(OMX_BUFFERHEADERTYPE* aBuffer)
	{
	iCallback->EmptyBufferDone(
		*this,
		iAppData,
		aBuffer);		
	}
	
// Component Entry Point
OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComponent)
	{
	TInt err = COmxPCMCodec::CreateComponent(hComponent);
	if (err == KErrNone)
		{
		return OMX_ErrorNone;
		}		
	else 
		{
		// return problem
		return OMX_ErrorInsufficientResources;		
		}
	}
