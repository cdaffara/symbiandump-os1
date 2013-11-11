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
// This file defines the CIPv6Flow class, which handles the transmission
// of IPv6 data to and from the TCP/IP stack.
// 
//

/**
 @file
*/

#ifndef IPV6BINDER_H__
#define IPV6BINDER_H__

#include "BinderBase.h"
#include <eui_addr.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif


class CIPv6Binder : public CBinderBase
{
public:
	CIPv6Binder(CRawIPFlow& aFlow);
	~CIPv6Binder();

public:
    // from MLowerDataSender
	virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aPdu);

	// from MLowerControl
	virtual TInt GetName(TDes& aName);
	virtual TInt GetConfig(TBinderConfig& aConfig);
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);

	ESock::MLowerDataSender* Bind(ESock::MUpperDataReceiver* aUpperReceiver , ESock::MUpperControl* aUpperControl);
	void Unbind (ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl);

	virtual TInt Notification(TAgentToNifEventType aEvent, void * aInfo);
	virtual void StartSending();

	virtual TBool WantsProtocol(TUint16 aProtocolCode);
	virtual void Process(RMBufChain& aPdu);

	virtual void SetProvision(const CIPConfig& aProvision);

	virtual void UpdateContextConfigL(const TPacketDataConfigBase& aConfig);

	virtual void UpdateConnectionSpeed(TUint aConnectionSpeed);

	// Define the class that will hold the IPv6 setting required by the Nif.
	class TIPv6Settings
		{
	public:
		TIp6Addr iLocalAddr;
		TIp6Addr iPrimaryDns;
		TIp6Addr iSecondaryDns;
		TBool iGetDnsFromServer;
		TBool iGetIpFromServer;
		TE64Addr iLocalIfId;
		//TE64Addr iRemoteIfId;
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
	TIPv6Settings iSettings;
	TUint iSpeedMetric;
};

void CIPv6Binder::WriteIfName(TDes& aName) const
/**
 * Used to uniquely identify the current Nif instance
 *
 * @param aName Descriptor that will be initialised.
 */
	{
	aName.Format(_L("%S[0x%X]"), &KIPv6ProtocolIfName, this);
	}

#endif // IPV6BINDER_H__
