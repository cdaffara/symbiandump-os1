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

#ifndef HCTLUARTORIGINALRECEIVER_H
#define HCTLUARTORIGINALRECEIVER_H

#include <e32base.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hciframelogger.h>


class CHCTLUartOriginal;
class RBusDevComm;

/**
	This is the class that implements the UART specifics of the Receiver AO and framer.
	
	This class is not intented for derivation.
*/
NONSHARABLE_CLASS(CHCTLUartOriginalReceiver) : public CActive
    {
public:
	static CHCTLUartOriginalReceiver* NewL(CHCTLUartOriginal& aHCTLUartOriginal, RBusDevComm& aPort);
	~CHCTLUartOriginalReceiver();

	// Called to initiate the initial read on the port.
	void Start();
	
private:
	CHCTLUartOriginalReceiver(CHCTLUartOriginal& aHCTLUartOriginal, RBusDevComm& aPort);
	void ConstructL();

	// Helper methods
	void QueueReadForNextFrame(TUint16 aOffset, TUint16 aBytesRequired);
	void ProcessData();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();

private:
    enum THctlReceiverState 
    	{ 
      	EWaitingForHctlHeaderByte,
	  	EWaitingForHciHeader,
	  	EWaitingForHciPayload,
	  	EInvalidDataReceived,
	  	};

	// Set the receive buffer equal to the 3-DH5 packet size.
	// This value should be changed by licensees if the controller
	// is exceeding this packet size.
	
	// 3-DH5.  Payload = 1021 octets.
	// Buffer length = payload + HCTL overhead
	static const TUint16 KHCTLRecvBufSize = 1022;

	CHCTLUartOriginal& iHCTLUartOriginal;
	THctlReceiverState iState;

	TUint8 iCurrentHCIPacketType;

	RBuf8 iReceiveBuffer;
	TPtr8 iReceiveBufPtr;

	RBusDevComm& iPort;
	DECLARE_HCI_LOGGER
	};

#endif // HCTLUARTORIGINALRECEIVER_H

