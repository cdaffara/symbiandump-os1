// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// omxinputport.h
// 
//

#ifndef OMXINPUTPORTBODY_H
#define OMXINPUTPORTBODY_H

#include <omxprocessingunit.h>
#include <omxinputport.h>

class MMdfOutputPort;
class MMdfOutputPortObserver;
class COmxProcessingUnit;
class CMMFBuffer;

/**
Base class for OpenMAX Input Ports.
*/
class COmxInputPort::CBody : public CBase,
					public MOmxInputPortCallbacks
	{
public:
	static CBody* NewL(TInt aPortIndex, COmxProcessingUnit* aComponent, COmxInputPort* aParent);
	 
	// from MMdfInputPort
	TInt MipConfigure(const TPuConfig&  aConfiguration);
	TInt MipGetConfig(TPuConfig& aConfigurationSetup);
	void MipInitialize();
	void MipSetObserver(const MMdfInputPortObserver& aInputPortObserver);
	CMMFBuffer* MipCreateBuffer(TInt aBufferSize);
	TInt MipUseBuffer(CMMFBuffer& aBuffer);
	TInt MipFreeBuffer(CMMFBuffer* aBuffer);
	TInt MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	void MipWriteData(CMMFBuffer& aBuffer);
	void MipDisconnectTunnel();
	void MipRestartTunnel();
	TBool MipIsTunnelled() const;
	TInt MipIndex() const;
	TUint32 MipBufferSize() const;
	TInt MipCreateCustomInterface(TUid aUid);
	TAny* MipCustomInterface(TUid aUid);
	
	// from MOmxInputPortCallbacks
	TInt EmptyBufferDone(OMX_HANDLETYPE aComponent, const CMMFBuffer* aBuffer);
	
	MMdfInputPortObserver* Observer() const;
	COmxProcessingUnit* Component() const;
private:
	CBody(TInt aPortIndex, COmxProcessingUnit* aComponent, COmxInputPort* aParent);

private:
	TInt 					iPortIndex;
	TBool 					iConnectedPort;
	MMdfOutputPort* 		iPortConnectedTo;
	COmxProcessingUnit* 	iComponent;
	MMdfInputPortObserver*	iObserver;
	COmxInputPort* 			iParent;
	};

#endif // OMXINPUTPORTBODY_H

