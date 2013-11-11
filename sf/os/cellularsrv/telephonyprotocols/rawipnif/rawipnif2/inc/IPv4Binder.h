// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the CIPv4Flow class, which handles the transmission
// of IPv4 data to and from the TCP/IP stack.
// 
//

/**
 @file
*/

#ifndef IPV4BINDER_H__
#define IPV4BINDER_H__

#include "BinderBase.h"


class CIPv4Binder : public CBinderBase
{
public:
	CIPv4Binder(CRawIP2Flow& aFlow);
	~CIPv4Binder();

public:
    // from BasebandChannelAdaptation2
	virtual ESock::MLowerDataSender::TSendResult Send(RCommsBufChain& aPdu);
	virtual void Process(RCommsBufChain& aPdu);
	
    // from ESock
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aPdu);
	virtual void Process(RMBufChain& aPdu);
	
	// from MLowerControl
	virtual TInt GetName(TDes& aName);
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);

	ESock::MLowerDataSender* Bind(ESock::MUpperDataReceiver* aUpperReceiver , ESock::MUpperControl* aUpperControl);
	void Unbind (ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);

	virtual TInt Notification(TAgentToNifEventType aEvent, void * aInfo);
	virtual void StartSending();

	virtual TBool WantsProtocol(TUint16 aProtocolCode);
	

	virtual void SetProvision(const CIPConfig& aProvision);

	virtual void UpdateContextConfigL(const TPacketDataConfigBase& aConfig);

	virtual void UpdateConnectionSpeed(TUint aConnectionSpeed);

	// Define the class that will hold the IPv4 setting required by the Nif.
	class TIPv4Settings
		{
	public:
		// As this is IPv4, all addresses are 32-bit.
		TUint32	iLocalAddr;
		TUint32	iNetMask;
		TUint32	iBroadcastAddr;
		TUint32	iDefGateway;
		TUint32	iPrimaryDns;
		TUint32	iSecondaryDns;
		TBool iGetGatewayFromServer;
		TBool iGetDnsFromServer;
		};

private:
#ifdef WCDMA_STUB
	TInt DeleteContext(TDes8& aContextParameters);
#endif

	inline void WriteIfName(TDes& aName) const;

#ifdef __BTT_LOGGING__
	void LogPacket(const RMBufChain& aPacket);
#endif // __BTT_LOGGING__

private: //owned
	TIPv4Settings iSettings;
	TUint iSpeedMetric;
};

void CIPv4Binder::WriteIfName(TDes& aName) const
/**
 * Used to uniquely identify the current Nif instance
 *
 * @param aName Descriptor that will be initialised.
 */
	{
	aName.Format(_L("%S[0x%X]"), &KIPv4ProtocolIfName, this);
	}

#endif // IPV4BINDER_H__
