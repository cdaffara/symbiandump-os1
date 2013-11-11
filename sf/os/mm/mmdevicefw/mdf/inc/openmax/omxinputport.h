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
 
#ifndef OMXINPUTPORT_H
#define OMXINPUTPORT_H

#include <omxprocessingunit.h>
#include <mdf/mdfinputport.h>

class MMdfOutputPort;
class MMdfOutputPortObserver;
class COmxOutputPort;
class CMMFBuffer;

/**
Base class for OpenMAX Input Ports.
*/
class COmxInputPort :   public CBase,
		      			public MMdfInputPort
	{
public:
	class CBody;
		
	// from MMdfInputPort
	IMPORT_C TInt MipConfigure(const TPuConfig&  aConfiguration);
	IMPORT_C TInt MipGetConfig(TPuConfig& aConfigurationSetup);
	IMPORT_C void MipInitialize();
	IMPORT_C void MipSetObserver(const MMdfInputPortObserver& aInputPortObserver);
	IMPORT_C CMMFBuffer* MipCreateBuffer(TInt aBufferSize);
	IMPORT_C TInt MipUseBuffer(CMMFBuffer& aBuffer);
	IMPORT_C TInt MipFreeBuffer(CMMFBuffer* aBuffer);
	IMPORT_C TInt MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	IMPORT_C void MipWriteData(CMMFBuffer& aBuffer);
	IMPORT_C void MipDisconnectTunnel();
	IMPORT_C void MipRestartTunnel();
	IMPORT_C TBool MipIsTunnelled() const;
	IMPORT_C TInt MipIndex() const;
	IMPORT_C TUint32 MipBufferSize() const;
	IMPORT_C TInt MipCreateCustomInterface(TUid aUid);
	IMPORT_C TAny* MipCustomInterface(TUid aUid);
	
protected:	
	IMPORT_C COmxInputPort();
	IMPORT_C ~COmxInputPort();
	IMPORT_C void ConstructL(TInt aIndex, COmxProcessingUnit* aComponent);
	IMPORT_C MMdfInputPortObserver* Observer() const;
	IMPORT_C COmxProcessingUnit* Component() const;
private:
	CBody* iBody;
	};

#endif // OMXINPUTPORT_H

