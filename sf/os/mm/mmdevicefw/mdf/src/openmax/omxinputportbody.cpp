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
// omxinputport.cpp
// 
//

#include <omxprocessingunit.h>
#include <omxinputport.h>
#include <mdf/mdfpuconfig.h>
#include "omxcomponentbody.h"
#include "omxinputportbody.h"

_LIT(KOmxInputPort, "OmxInputPortBody");

COmxInputPort::CBody* COmxInputPort::CBody::NewL(TInt aIndex, COmxProcessingUnit* aComponent, COmxInputPort* aParent)
	{
	return new (ELeave) CBody(aIndex, aComponent, aParent);	
	}  

COmxInputPort::CBody::CBody(TInt aIndex, COmxProcessingUnit* aComponent, COmxInputPort* aParent) :
	iPortIndex(aIndex),
	iComponent(aComponent),	
	iParent(aParent)
	{
	}
	
TInt COmxInputPort::CBody::MipConfigure(const TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;	
	}	
	
TInt COmxInputPort::CBody::MipGetConfig(TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;	
	}	

void COmxInputPort::CBody::MipInitialize()
	{	
	}

CMMFBuffer* COmxInputPort::CBody::MipCreateBuffer(TInt aBufferSize)
	{
	CMMFBuffer* buffer = NULL;
	TInt err = KErrNone;
	TRAP(err, buffer = Component()->OmxAllocateBufferL(iPortIndex, aBufferSize));	
	return buffer;
	}

TInt COmxInputPort::CBody::MipUseBuffer(CMMFBuffer& aBuffer)
	{
	return Component()->OmxUseBuffer(&aBuffer, iPortIndex);		
	}

TInt COmxInputPort::CBody::MipFreeBuffer(CMMFBuffer* aBuffer)
	{
	return Component()->OmxFreeBuffer(aBuffer);		
	}

TInt COmxInputPort::CBody::MipTunnelRequest(const MMdfOutputPort& /*aOutputPortToBeConnectedTo*/,
		TTunnelFlags& /*aTunnelFlags*/, TSupplierType& /*aSupplierType*/) 
	{
	return KErrNotSupported;
	}
	
void COmxInputPort::CBody::MipWriteData(CMMFBuffer& aInputBuffer)
	{
	Component()->OmxEmptyThisBuffer(&aInputBuffer, this);	
	}

void COmxInputPort::CBody::MipDisconnectTunnel()
	{
	// here, as this can reasonably be called by the device adapter
	// during unloading of a PU which has been loaded but never initialised,
	// we use a conditional instead of an ASSERT	
	if(iObserver) 
		{
		iObserver->MipoDisconnectTunnelComplete(iParent, KErrNone);
		}
	}

void COmxInputPort::CBody::MipRestartTunnel()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(KOmxInputPort, EObserverNotSet));

	iObserver->MipoRestartTunnelComplete(iParent, KErrNone);
	}		

TBool COmxInputPort::CBody::MipIsTunnelled() const
	{
	if (!iConnectedPort)
		{
		return EFalse;
		}		
	return ETrue;
	}

TInt COmxInputPort::CBody::MipIndex() const
	{
	return iPortIndex;
	}
	
TUint32 COmxInputPort::CBody::MipBufferSize() const
	{
	OMX_PARAM_PORTDEFINITIONTYPE portInfo;
	portInfo.nPortIndex = 0;
	Component()->OmxGetParameter(OMX_IndexParamPortDefinition, &portInfo);	
	return portInfo.nBufferSize;
	}

void COmxInputPort::CBody::MipSetObserver(const MMdfInputPortObserver& aOutputPortObserver)
	{
	iObserver = const_cast<MMdfInputPortObserver*>(&aOutputPortObserver);
	}

TInt COmxInputPort::CBody::EmptyBufferDone(OMX_HANDLETYPE /*aComponent*/, const CMMFBuffer* aBuffer)
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(KOmxInputPort, EObserverNotSet));

	iObserver->MipoWriteDataComplete(iParent, const_cast<CMMFBuffer*>(aBuffer),KErrNone);
	return KErrNone;
	}

TInt COmxInputPort::CBody::MipCreateCustomInterface(TUid /*aUid*/)
	{
	return KErrNotSupported;	
	}
		 
TAny* COmxInputPort::CBody::MipCustomInterface(TUid /*aUid*/)
	{
	return NULL;
	}
	
MMdfInputPortObserver* COmxInputPort::CBody::Observer() const
	{
	return iObserver;
	}

COmxProcessingUnit* COmxInputPort::CBody::Component() const
	{
	return iComponent;
	}
