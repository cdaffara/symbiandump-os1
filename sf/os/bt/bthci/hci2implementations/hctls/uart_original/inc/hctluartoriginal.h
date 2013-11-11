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

/**
 @file
 @internalComponent
*/

#ifndef HCTLUARTORIGINAL_H
#define HCTLUARTORIGINAL_H

#include <bluetooth/hci/hctluartbase.h>
#include <bluetooth/hciframelogger.h>
#include <bluetooth/hci/hcitypes.h>


class CHCTLUartOriginalReceiver;
class CHCTLUartOriginalSender;
class CControllerManager;

// Define HCTL Packet Header Size
static const TInt KHctlHeaderSize = 1;
static const TInt KHctlPacketTypeOffset = 0;

// UART HCI Framing constants for command frames
static const TInt KHCTLCommandHeaderSize = 1;
static const TInt KHCTLCommandTrailerSize = 0;

// UART HCI Framing constants for ACL data frames
static const TInt KHCTLAclDataHeaderSize = 1;
static const TInt KHCTLAclDataTrailerSize = 0;

// UART HCI Framing constants for Synchronous data frames
static const TInt KHCTLSynchronousDataHeaderSize = 1;
static const TInt KHCTLSynchronousDataTrailerSize = 0;

_LIT(KIniFileName, "hctl_uart_original");

/**
This is the class that implements the UART specific HCTL.	
*/
NONSHARABLE_CLASS(CHCTLUartOriginal) : public CHCTLUartBase
	{
public:
	static CHCTLUartOriginal* NewL();
	~CHCTLUartOriginal();

	void DoConfigL();
	virtual void ProcessACLData(const TDesC8& aData);
	virtual void ProcessSynchronousData(const TDesC8& aData);
	virtual void ProcessEvent(const TDesC8& aEvent);
	
	MQdpPluginInterfaceFinder* QdpPluginInterfaceFinder();

	// From MHardResetInitiator
	virtual void MhriStartHardReset();

	// Called from the Controller Manager
	void HandlePowerOff();
	void HandlePowerOn();

	TBTPowerState CurrentPowerState() const;
	
public:
	// HCTL packet types as defined in Section 2, Part H: 4 of the
	// bluetooth specification.
	enum THctlPacketType 
	    {
		ECommandPacket			= 0x01,
		EACLDataPacket			= 0x02,
		ESynchronousDataPacket	= 0x03,
		EEventPacket			= 0x04,
	    };

private:
	CHCTLUartOriginal();
	void ConstructL();
	TAny* Interface(TUid aUid);

	static void SetPacketIndicator(THctlPacketType aType, const TDesC8& aData);

	// From CHCTLUartBase
	virtual void PortOpenedL();
	
	// From MHCTLInterface
	virtual TInt MhiWriteCommand(const TDesC8& aData);
	virtual TInt MhiWriteAclData(const TDesC8& aData);
	virtual TInt MhiWriteSynchronousData(const TDesC8& aData);
	virtual void MhiGetAclDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	virtual void MhiGetSynchronousDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	virtual void MhiGetCommandTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const;
	virtual void MhiSetChannelObserver(MHCTLChannelObserver& aChannelObserver);
	virtual void MhiSetDataObserver(MHCTLDataObserver& aDataObserver);
	virtual void MhiSetEventObserver(MHCTLEventObserver& aEventObserver);
	virtual void MhiSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver);
	virtual void MhiSetQdpPluginInterfaceFinder(MQdpPluginInterfaceFinder& aQdpPluginInterfaceFinder);

private:
	CHCTLUartOriginalReceiver* iReceiver;
	CHCTLUartOriginalSender* iSender;   
	MQdpPluginInterfaceFinder* iQdpPluginInterfaceFinder;
	TBTPowerState iCurrentPowerState;

	CControllerManager* iControllerMan;
	DECLARE_HCI_LOGGER
	};


#endif // HCTLUARTORIGINAL_H

