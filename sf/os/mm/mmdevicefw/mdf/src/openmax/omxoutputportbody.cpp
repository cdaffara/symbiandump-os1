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
// omxoutputport.cpp
// 
//

#include <omxprocessingunit.h>
#include <mdf/mdfpuconfig.h>
#include "omxcomponentbody.h"
#include "omxoutputportbody.h"

COmxOutputPort::CBody* COmxOutputPort::CBody::NewL(TInt aIndex, COmxProcessingUnit* aComponent, COmxOutputPort* aParent)
	{
	return new (ELeave) CBody(aIndex, aComponent, aParent);	
	}  

COmxOutputPort::CBody::CBody(TInt aIndex, COmxProcessingUnit* aComponent, COmxOutputPort* aParent) :
	iPortIndex(aIndex),	
	iComponent(aComponent),
	iParent(aParent)
	{
	}

void COmxOutputPort::CBody::MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver) 
	{
	iObserver = const_cast<MMdfOutputPortObserver*>(&aOutputPortObserver);
	}

void COmxOutputPort::CBody::MipoWriteDataComplete(const MMdfInputPort* /*aInputPort*/,
		CMMFBuffer* aBuffer, TInt /*aErrorCode*/)
	{
	// the parent class read must be called in case it has been overridden in a derived class
	iParent->MopReadData(*aBuffer);
	}

void COmxOutputPort::CBody::MipoDisconnectTunnelComplete(const MMdfInputPort* /*aInputPort*/,
		TInt /*aErrorCode*/)
	{
	}
	
void COmxOutputPort::CBody::MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort*/,
		TInt /*aErrorCode*/)
	{	
	}

TInt COmxOutputPort::CBody::FillBufferDone(OMX_HANDLETYPE /*aComponent*/, CMMFBuffer* aBuffer)
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(_L("OmxOutputPort"), EObserverNotSet));

	if (iPortConnectedTo)
		{
		iPortConnectedTo->MipWriteData(*aBuffer);
		}
	else
		{
		if (iObserver)
			{
			iObserver->MopoReadDataComplete(iParent, aBuffer, KErrNone);				
			}
		}
	return KErrNone;
	}

TInt COmxOutputPort::CBody::MopConfigure(const TPuConfig&  /*aConfig*/)
	{
	return KErrNotSupported;	
	}
	
TInt COmxOutputPort::CBody::MopGetConfig(TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;	
	}		

void COmxOutputPort::CBody::MopInitialize()	
 	{
 	if (iPortConnectedTo != NULL)
 		{
 		iPortConnectedTo->MipSetObserver(*iParent);
 		}
 	}
	
CMMFBuffer* COmxOutputPort::CBody::MopCreateBuffer(TInt aBufferSize)
	{
	CMMFBuffer* buffer = NULL;
	TInt err = KErrNone;
	TRAP(err, buffer = Component()->OmxAllocateBufferL(iPortIndex, aBufferSize));	
	return buffer;
	}

TInt COmxOutputPort::CBody::MopUseBuffer(CMMFBuffer& aBuffer)
	{
	if (iPortConnectedTo)
		{
		iPortConnectedTo->MipUseBuffer(aBuffer);
		}

	return Component()->OmxUseBuffer(&aBuffer, iPortIndex);	
	}
	
TInt COmxOutputPort::CBody::MopFreeBuffer(CMMFBuffer* aBuffer)
	{
	return Component()->OmxFreeBuffer(aBuffer);	
	}
	 
TInt COmxOutputPort::CBody::MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType) 
	{
	if ((Component()->State()!=EProcessingUnitLoaded) && (!iStopped))
		{
		// invalid state
		return EInvalidState;
		}
	
	if (iPortConnectedTo)
		{
		// the port is already connected, return an error
		return EPortAlreadyTunnelled;
		}
	aTunnelFlags = EBufferReadOnly;
	aSupplierType = EBufferSupplyOutput;
	iPortConnectedTo = const_cast<MMdfInputPort*>(&aInputPortToBeConnectedTo);

	
	return KErrNone;
	}

void COmxOutputPort::CBody::MopReadData(CMMFBuffer& aBuffer)
	{
	Component()->OmxFillThisBuffer(&aBuffer, this);	
	}

void COmxOutputPort::CBody::MopDisconnectTunnel()
	{
	if(iObserver) 
		{
		iObserver->MopoDisconnectTunnelComplete(iParent, KErrNone);		
		}
	}

void COmxOutputPort::CBody::MopRestartTunnel()
	{
	}

TBool COmxOutputPort::CBody::MopIsTunnelled() const
	{
	if (!iPortConnectedTo)
		{
		return EFalse;
		}		
	return ETrue;
	}

TInt COmxOutputPort::CBody::MopIndex() const
	{
	return iPortIndex;
	}

TUint32 COmxOutputPort::CBody::MopBufferSize() const
	{
	OMX_PARAM_PORTDEFINITIONTYPE portInfo;
	portInfo.nPortIndex = 1;
	Component()->OmxGetParameter(OMX_IndexParamPortDefinition, &portInfo);	
	return portInfo.nBufferSize;
	}

TInt COmxOutputPort::CBody::MopCreateCustomInterface(TUid /*aUid*/)
	{
	return KErrNotSupported;
	}

TAny* COmxOutputPort::CBody::MopCustomInterface(TUid /*aUid*/)
	{
	return NULL;	
	}

MMdfOutputPortObserver* COmxOutputPort::CBody::Observer() const
	{
	return iObserver;
	}

COmxProcessingUnit* COmxOutputPort::CBody::Component() const
	{
	return iComponent;
	}
