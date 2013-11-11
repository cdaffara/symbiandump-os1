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

#include <stdio.h>
#include "OmxImpl.h"

	
OMX_ERRORTYPE OMXComponentGetComponentVersion(
		OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STRING pComponentName,
        OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
        OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
        OMX_OUT OMX_UUIDTYPE* pComponentUUID);
OMX_ERRORTYPE OMXComponentSendCommand(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_COMMANDTYPE Cmd,
        OMX_IN  OMX_U32 nParam1,
        OMX_IN  OMX_PTR pCmdData);
OMX_ERRORTYPE OMXComponentGetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nParamIndex,  
        OMX_INOUT OMX_PTR ComponentParameterStructure);
OMX_ERRORTYPE OMXComponentSetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nIndex,
        OMX_IN  OMX_PTR ComponentParameterStructure);
OMX_ERRORTYPE OMXComponentGetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_INOUT OMX_PTR value);
OMX_ERRORTYPE OMXComponentSetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_IN  OMX_PTR value);
OMX_ERRORTYPE OMXComponentGetExtensionIndex(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_STRING ParameterName,
        OMX_OUT OMX_INDEXTYPE* pIndexType);
OMX_ERRORTYPE OMXComponentGetState(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STATETYPE* pState);
OMX_ERRORTYPE OMXComponentComponentTunnelRequest(
        OMX_IN  OMX_HANDLETYPE hInput,
        OMX_IN  OMX_U32 nPortInput,
        OMX_IN  OMX_HANDLETYPE hOutput,
        OMX_IN  OMX_U32 nPortOutput,
        OMX_IN  OMX_TUNNELSETUPTYPE* pTunnelSetup);
OMX_ERRORTYPE OMXComponentUseBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN	OMX_PTR pAppPrivate,
        OMX_IN	OMX_U32 nSizeBytes,
        OMX_IN	OMX_U8* pBuffer);
OMX_ERRORTYPE OMXComponentAllocateBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN	OMX_PTR pAppData,
        OMX_IN	OMX_U32 nSizeBytes);
OMX_ERRORTYPE OMXComponentFreeBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE OMXComponentEmptyThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE OMXComponentFillThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE OMXComponentSetCallbacks(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
        OMX_IN  OMX_PTR pAppData);   
OMX_ERRORTYPE OMXComponentDeInit(OMX_IN  OMX_HANDLETYPE hComponent);


void EventHandler(OMX_OUT OMX_HANDLETYPE hComponent, 
					OMX_OUT OMX_PTR pAppData,
        			OMX_OUT OMX_EVENTTYPE eEvent, 
        			OMX_OUT OMX_U32 Data,
        			OMX_OUT OMX_STRING cExtraInfo);
        			
void InputBufferCallback(
        OMX_OUT OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_PTR pAppData,
        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);
        
void OutputBufferCallback(
        OMX_OUT OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_PTR pAppData,
        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);
        
COmxComponentImpl::COmxComponentImpl(OMX_HANDLETYPE hComponent)
	{
	iHandle = static_cast<OMX_COMPONENTTYPE*>(hComponent);
	OMX_VERSIONTYPE ver = 
		{
		1,0
		};

	// fill in handle structure
	iHandle->nVersion = ver;
	iHandle->pComponentPrivate = this; 
	iHandle->GetComponentVersion = &::OMXComponentGetComponentVersion;
	iHandle->SendCommand = &::OMXComponentSendCommand;
	iHandle->GetParameter = &::OMXComponentGetParameter;
	iHandle->SetParameter = &::OMXComponentSetParameter;
	iHandle->GetConfig = &::OMXComponentGetConfig;
	iHandle->SetConfig = &::OMXComponentSetConfig;
	iHandle->GetExtensionIndex = &::OMXComponentGetExtensionIndex;
	iHandle->GetState =	&::OMXComponentGetState;
	iHandle->ComponentTunnelRequest = &::OMXComponentComponentTunnelRequest;
	iHandle->UseBuffer = &::OMXComponentUseBuffer;
	iHandle->AllocateBuffer = &::OMXComponentAllocateBuffer;
	iHandle->FreeBuffer = &::OMXComponentFreeBuffer;
	iHandle->EmptyThisBuffer = &::OMXComponentEmptyThisBuffer;
	iHandle->FillThisBuffer = &::OMXComponentFillThisBuffer;
	iHandle->SetCallbacks = &::OMXComponentSetCallbacks;
	iHandle->ComponentDeInit = &::OMXComponentDeInit;
	}
	
COmxComponentImpl::~COmxComponentImpl()
	{
	}
	
COmxComponentImpl::operator OMX_HANDLETYPE()
	{
	return (OMX_HANDLETYPE)&iHandle;
	}
	
OMX_ERRORTYPE OMXComponentGetComponentVersion(
		OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STRING pComponentName,
        OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
        OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
        OMX_OUT OMX_UUIDTYPE* pComponentUUID)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->GetComponentVersion(pComponentName,pComponentVersion,pSpecVersion, pComponentUUID);
	}

OMX_ERRORTYPE OMXComponentSendCommand(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_COMMANDTYPE Cmd,
        OMX_IN  OMX_U32 nParam1,
        OMX_IN  OMX_PTR pCmdData)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->SendCommand(Cmd ,nParam1, pCmdData);	
	}
	
OMX_ERRORTYPE OMXComponentGetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nParamIndex,  
        OMX_INOUT OMX_PTR ComponentParameterStructure)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->GetParameter(nParamIndex, ComponentParameterStructure);
	}
	
OMX_ERRORTYPE OMXComponentSetParameter(
        OMX_IN  OMX_HANDLETYPE hComponent, 
        OMX_IN  OMX_INDEXTYPE nIndex,
        OMX_IN  OMX_PTR ComponentParameterStructure)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->SetParameter(nIndex, ComponentParameterStructure);
	}
	
OMX_ERRORTYPE OMXComponentGetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_INOUT OMX_PTR value)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->GetConfig(nIndex, value);
	}
	
OMX_ERRORTYPE OMXComponentSetConfig(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_INDEXTYPE nIndex, 
        OMX_IN  OMX_PTR value)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->SetConfig(nIndex, value);
	}
	
OMX_ERRORTYPE OMXComponentGetExtensionIndex(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_STRING ParameterName,
        OMX_OUT OMX_INDEXTYPE* pIndexType)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->GetExtensionIndex(ParameterName, pIndexType);
	}
	
OMX_ERRORTYPE OMXComponentGetState(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STATETYPE* pState)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->GetState(pState);
	}
	
OMX_ERRORTYPE OMXComponentComponentTunnelRequest(
    	OMX_IN	OMX_HANDLETYPE hInput,
        OMX_IN  OMX_U32 nInputPort,
        OMX_IN  OMX_HANDLETYPE hOutput,
        OMX_IN  OMX_U32 nOutputPort,
        OMX_IN  OMX_TUNNELSETUPTYPE* pTunnelSetup)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hInput)->pComponentPrivate)->ComponentTunnelRequest(hInput, nInputPort, hOutput, nOutputPort, pTunnelSetup);
	}
	
OMX_ERRORTYPE OMXComponentUseBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN	OMX_PTR pAppPrivate,
        OMX_IN  OMX_U32 nSizeBytes,
        OMX_IN  OMX_U8* pBuffer)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->UseBuffer(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
	}
	
OMX_ERRORTYPE OMXComponentAllocateBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN  OMX_PTR pAppData,
        OMX_IN	OMX_U32 nSizeBytes)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->AllocateBuffer(pBuffer, nPortIndex, pAppData, nSizeBytes);
	}

OMX_ERRORTYPE OMXComponentFreeBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_U32 nPortIndex,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->FreeBuffer(nPortIndex, pBuffer);
	}
	
OMX_ERRORTYPE OMXComponentEmptyThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->EmptyThisBuffer(pBuffer);
	}
	
OMX_ERRORTYPE OMXComponentFillThisBuffer(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->FillThisBuffer(pBuffer);
	}
	
OMX_ERRORTYPE OMXComponentSetCallbacks(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
        OMX_IN  OMX_PTR pAppData)
	{
	return ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate)->SetCallbacks(pCallbacks, pAppData);
	}
	
OMX_ERRORTYPE OMXComponentDeInit(OMX_IN  OMX_HANDLETYPE hComponent)
	{
	delete ((COmxComponentImpl*)((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
	return OMX_ErrorNone;
	}
