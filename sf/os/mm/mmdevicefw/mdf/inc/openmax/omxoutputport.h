// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef OMXOUTPUTPORT_H
#define OMXOUTPUTPORT_H

#include <mdf/mdfoutputport.h>
#include <mdf/mdfinputport.h>
#include <omxprocessingunit.h>
class TPuConfig;
class CMMFBuffer;
class COmxProcessingUnit;
class MMDFInputPort;

/**
Base class for OpenMAX Output Ports.
*/
class COmxOutputPort :  public CBase,
						public MMdfOutputPort,
						public MMdfInputPortObserver
	{	
public:
	class CBody;
public:	
	// from MMdfOutputPort		
	IMPORT_C TInt MopConfigure(const TPuConfig&  aConfiguration);
	IMPORT_C TInt MopGetConfig(TPuConfig& aConfigurationSetup);
	IMPORT_C void MopInitialize();
	IMPORT_C void MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver);
	IMPORT_C CMMFBuffer* MopCreateBuffer(TInt aBufferSize);
	IMPORT_C TInt MopUseBuffer(CMMFBuffer& aBuffer);
	IMPORT_C TInt MopFreeBuffer(CMMFBuffer* aBuffer);
	IMPORT_C TInt MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	IMPORT_C void MopReadData(CMMFBuffer& aBuffer);
	IMPORT_C void MopDisconnectTunnel();
	IMPORT_C void MopRestartTunnel();
	IMPORT_C TBool MopIsTunnelled() const;
	IMPORT_C TInt MopIndex() const;
	IMPORT_C TUint32 MopBufferSize() const;
	IMPORT_C TInt MopCreateCustomInterface(TUid aUid);
	IMPORT_C TAny* MopCustomInterface(TUid aUid);
	
	// From MMdfInputPortObserver
	IMPORT_C void MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode);
	IMPORT_C void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);
	IMPORT_C void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);

protected:
	IMPORT_C COmxOutputPort();
	IMPORT_C void ConstructL(TInt aIndex, COmxProcessingUnit* aComponent);
	IMPORT_C ~COmxOutputPort();
	IMPORT_C MMdfOutputPortObserver* Observer() const;
	IMPORT_C COmxProcessingUnit* Component() const;

private:
	CBody* iBody;
	};

#endif // OMXOUTPUTPORT_H
