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

#ifndef OMXIMPL_H
#define OMXIMPL_H

#include <stdio.h>
#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>


class COmxComponentImpl : public CBase
	{
public:
	COmxComponentImpl(OMX_HANDLETYPE hComponent);
	virtual ~COmxComponentImpl();
		
	virtual OMX_ERRORTYPE GetComponentVersion(
           OMX_STRING pComponentName,
           OMX_VERSIONTYPE* pComponentVersion,
           OMX_VERSIONTYPE* pSpecVersion,
           OMX_UUIDTYPE* pComponentUUID)=0;
           	
    virtual OMX_ERRORTYPE SendCommand(
           OMX_COMMANDTYPE Cmd,
           TUint32 nParam1,
           TAny* pCmdData)=0;
    virtual OMX_ERRORTYPE GetParameter(
           OMX_INDEXTYPE nParamIndex,  
           TAny* ComponentParameterStructure)=0;
    virtual OMX_ERRORTYPE SetParameter(
           OMX_INDEXTYPE nIndex,
           TAny* ComponentParameterStructure)=0;
    virtual OMX_ERRORTYPE GetConfig(
           OMX_INDEXTYPE nIndex, 
           TAny* value)=0;
    virtual OMX_ERRORTYPE SetConfig(
           OMX_INDEXTYPE nIndex, 
           TAny* value)=0;
    virtual OMX_ERRORTYPE GetExtensionIndex(
           OMX_STRING ParameterName,
           OMX_INDEXTYPE* pIndexType)=0;
    virtual OMX_ERRORTYPE GetState(
           OMX_STATETYPE* pState)=0;
	virtual OMX_ERRORTYPE ComponentTunnelRequest(
       		OMX_HANDLETYPE hInput,
       		TUint32 nPortInput,
       		OMX_HANDLETYPE hOutput,
       		TUint32 nPortOutput,
       		OMX_TUNNELSETUPTYPE* pTunnelSetup) = 0;
    virtual OMX_ERRORTYPE UseBuffer(
           OMX_BUFFERHEADERTYPE** ppBufferHdr,
           TUint32 nPortIndex,
           TAny* pAppPrivate,
           TUint32 nSizeBytes,
           TUint8* pBuffer)=0;
    virtual OMX_ERRORTYPE AllocateBuffer(
           OMX_BUFFERHEADERTYPE** pBuffer,
           TUint32 nPortIndex,
           TAny* pAppData,
           TUint32 nSizeBytes)=0;
    virtual OMX_ERRORTYPE FreeBuffer(
    		TUint32 nPortIndex,
           OMX_BUFFERHEADERTYPE* pBuffer)=0;
    virtual OMX_ERRORTYPE EmptyThisBuffer(
           OMX_BUFFERHEADERTYPE* pBuffer)=0;
    virtual OMX_ERRORTYPE FillThisBuffer(
           OMX_BUFFERHEADERTYPE* pBuffer)=0;
    virtual OMX_ERRORTYPE SetCallbacks(
           OMX_CALLBACKTYPE* pCallbacks, 
           TAny* pAppData)=0;


	operator OMX_HANDLETYPE();
		
private:
	
	OMX_COMPONENTTYPE* iHandle;
	};
	
	
#endif // OMXIMPL_H
