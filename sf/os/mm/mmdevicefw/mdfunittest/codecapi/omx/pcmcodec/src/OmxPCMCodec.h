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

#ifndef OMXPCMCODEC_H
#define OMXPCMCODEC_H

#include <e32msgqueue.h>
#include <mdaaudiooutputstream.h>
#include <openmax/il/khronos/v1_x/OMX_Audio.h>
#include "OmxImpl.h"

#include "MMFAudioU8ToS16PcmCodec.h"
#include "MMFAudioPcm8ToPcm16Codec.h"
#include "MMFAudioPcmS16ToPcmS8Codec.h"
#include "MMFAudioS16ToU8PcmCodec.h"

class COmxPCMCodec;


class CCodecProcessor : public CBase
		{

	enum TCodecMessageType
		{
		EStopProcessing,
		EPauseProcessing,
		EInputBuffer,
		EOutputBuffer,
		EExit				
		};

	class TCodecMessage 
		{
	public:
		TCodecMessageType iType;
		OMX_BUFFERHEADERTYPE* iBuffer;
		};
	
	public:		
		static CCodecProcessor* NewL(COmxPCMCodec& aParent);		

		void RunThreadL();
		void Stop();
		void Exit();
	    
        OMX_ERRORTYPE EmptyThisBuffer(
            OMX_BUFFERHEADERTYPE* pBuffer);
	    OMX_ERRORTYPE FillThisBuffer(
            OMX_BUFFERHEADERTYPE* pBuffer);
            
		void SetInputDataType(OMX_NUMERICALDATATYPE aType);
		void SetInputBitsPerSample(TInt aBitsPerSample);
		void SetOutputDataType(OMX_NUMERICALDATATYPE aType);
		void SetOutputBitsPerSample(TInt aBitsPerSample);

		virtual ~CCodecProcessor();		
	    
	private:
		void ConstructL();
		CCodecProcessor(COmxPCMCodec& aParent);

		void ProcessAvailableBuffers();
		void ChooseCodec();
	
	private:
		COmxPCMCodec* iParent;
		RPointerArray<OMX_BUFFERHEADERTYPE> iBuffersToEmpty;
		RPointerArray<OMX_BUFFERHEADERTYPE> iBuffersToFill;	
		
		TRequestStatus iQueueStatus;
		RMsgQueue<TCodecMessage> iMessageQueue;
		
		OMX_BUFFERHEADERTYPE* iInputBuffer;
		OMX_BUFFERHEADERTYPE* iOutputBuffer;
		
		TMMFAudioCodecBase* iCodec;

		// codec configuration settings
		TInt iInputBitsPerSample; 
		TInt iOutputBitsPerSample; 
		OMX_NUMERICALDATATYPE iInputDataType;
		OMX_NUMERICALDATATYPE iOutputDataType;
		
		TInt iInputBufferPos;
		
		TMMFAudioCodecBase* iCurrentCodec;
			
		TMMFAudioU8ToS16PcmCodec iAudioU8ToS16PcmCodec;
		TMMFAudioPcm8ToPcm16Codec iAudioS8ToS16PcmCodec;
		
		TMMFAudioPcmS16ToPcmS8Codec iAudioS16ToS8PcmCodec;
		TMMFAudioS16ToU8PcmCodec iAudioS16ToU8PcmCodec;

		TBool iStarted;
		TReal iOutputSamplesPerInputSample;
		};



class COmxPCMCodec : public COmxComponentImpl
	{
public:
	friend class CCodecProcessor;
	friend class CCodecCallbacks;

	static TInt CreateComponent(OMX_HANDLETYPE hComponent);
	~COmxPCMCodec();
	
	OMX_ERRORTYPE GetComponentVersion(
			OMX_STRING pComponentName,
			OMX_VERSIONTYPE* pComponentVersion,
			OMX_VERSIONTYPE* pSpecVersion,
			OMX_UUIDTYPE* pComponentUUID);
    OMX_ERRORTYPE SendCommand(
			OMX_COMMANDTYPE Cmd,
			TUint32 nParam1,
			TAny* pCmdData);
    OMX_ERRORTYPE GetParameter(
			OMX_INDEXTYPE nParamIndex,  
			TAny* ComponentParameterStructure);
    OMX_ERRORTYPE SetParameter(
			OMX_INDEXTYPE nIndex,
			TAny* ComponentParameterStructure);
    OMX_ERRORTYPE GetConfig(
			OMX_INDEXTYPE nIndex, 
			TAny* value);
    OMX_ERRORTYPE SetConfig(
			OMX_INDEXTYPE nIndex, 
			TAny* value);
    OMX_ERRORTYPE GetExtensionIndex(
			OMX_STRING ParameterName,
			OMX_INDEXTYPE* pIndexType);
    OMX_ERRORTYPE GetState(
			OMX_STATETYPE* pState);
    OMX_ERRORTYPE ComponentTunnelRequest(
			OMX_HANDLETYPE hInput,
			TUint32 nInputPort,
			OMX_HANDLETYPE hOutput,
			TUint32 nOutputPort,
			OMX_TUNNELSETUPTYPE* pTunnelSetup);
    virtual OMX_ERRORTYPE UseBuffer(
			OMX_BUFFERHEADERTYPE** pBufferHdr,
			TUint32 nPortIndex,
			TAny* pAppPrivate,
			TUint32 nSizeBytes,
			TUint8* pBuffer);
    virtual OMX_ERRORTYPE AllocateBuffer(
           OMX_BUFFERHEADERTYPE** pBuffer,
           TUint32 nPortIndex,
           TAny* pAppData,
           TUint32 nSizeBytes);
    OMX_ERRORTYPE FreeBuffer(
    		  TUint32 nPortIndex,	
              OMX_BUFFERHEADERTYPE* pBuffer);
    OMX_ERRORTYPE EmptyThisBuffer(
              OMX_BUFFERHEADERTYPE* pBuffer);
    OMX_ERRORTYPE FillThisBuffer(
              OMX_BUFFERHEADERTYPE* pBuffer);
    OMX_ERRORTYPE SetCallbacks(
              OMX_CALLBACKTYPE* pCallbacks, 
              TAny* pAppData);
              
	TInt StartExecution();
            
	void EventHandlerCallback(
        			 OMX_EVENTTYPE eEvent, 
        			 TUint32 nData1,
        			 TUint32 nData2,
        			 OMX_STRING cExtraInfo);
	void FillBufferDoneCallback(OMX_BUFFERHEADERTYPE* pBuffer);
	void EmptyBufferDoneCallback(OMX_BUFFERHEADERTYPE* pBuffer);
            

private:
	COmxPCMCodec(OMX_HANDLETYPE hComponent);
	void ConstructL();

private:
	OMX_CALLBACKTYPE* iCallback;
	TAny* iAppData;
	OMX_STATETYPE iState;
	
	TRequestStatus iThreadDeath;
	CCodecProcessor* iCodecProcessor;
	TBool iCreatedThread;
	RThread iProcessingThread;
	};
	
#endif // OMXPCMCODEC_H
