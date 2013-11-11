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
//

#ifndef OMXOUTPUTPORTBODY_H
#define OMXOUTPUTPORTBODY_H

#include <mdf/mdfoutputport.h>
#include <mdf/mdfinputport.h>
#include <omxprocessingunit.h>
#include <omxoutputport.h>
class TPuConfig;
class CMMFBuffer;
class COmxProcessingUnit;
class MMDFInputPort;

class COmxOutputPort::CBody : public CBase,
						public MOmxOutputPortCallbacks,
						public MMdfInputPortObserver
	{	
public:	
	// from MMdfOutputPort	
	static CBody* NewL(TInt aIndex, COmxProcessingUnit* aComponent, COmxOutputPort* aParent);
		
	TInt MopConfigure(const TPuConfig&  aConfiguration);
	TInt MopGetConfig(TPuConfig& aConfigurationSetup);
	void MopInitialize();
	void MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver);
	CMMFBuffer* MopCreateBuffer(TInt aBufferSize);
	TInt MopUseBuffer(CMMFBuffer& aBuffer);
	TInt MopFreeBuffer(CMMFBuffer* aBuffer);
	TInt MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	void MopReadData(CMMFBuffer& aBuffer);
	void MopDisconnectTunnel();
	void MopRestartTunnel();
	TBool MopIsTunnelled() const;
	TInt MopIndex() const;
	TUint32 MopBufferSize() const;
	TInt MopCreateCustomInterface(TUid aUid);
	TAny* MopCustomInterface(TUid aUid);

	// from MOmxOutputPortCallbacks
	TInt FillBufferDone(OMX_HANDLETYPE hComponent, CMMFBuffer* pBuffer);

	// From MMdfInputPortObserver
	void MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode);
	void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);
	void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);

	MMdfOutputPortObserver* Observer() const;
	COmxProcessingUnit* Component() const;

private:
	CBody(TInt aIndex, COmxProcessingUnit* aComponent, COmxOutputPort* aParent);

	TInt 					iPortIndex;
	MMdfInputPort* 			iPortConnectedTo;
	COmxProcessingUnit* 	iComponent;
	MMdfOutputPortObserver*	iObserver;
	TBool 					iStopped;
	COmxOutputPort* 		iParent;
	};

#endif // OMXOUTPUTPORTBODY_H
