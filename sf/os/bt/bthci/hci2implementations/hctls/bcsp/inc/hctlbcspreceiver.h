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

#ifndef HCTLBCSPRECEIVER_H
#define HCTLBCSPRECEIVER_H

#include <e32base.h>

class RBusDevComm;
class CHCTLBcsp;

NONSHARABLE_CLASS(CHCTLBcspReceiver) : CActive
/**
CHCTLBcspReceiver
Implements BCSP specific implementation of UART CHCTLUartReceiver Active Object

Any errors at this stage will cause the RX frame to be discarded and another read to be queued on the UART

If there are no errors then control will be passed to the sequence layer CHCTLBcspSequencer via CHCTLBcsp::HandleRx()
Then another read will be queued on the UART
  
This class is not intended for derivation.
*/
    {
public:
    static CHCTLBcspReceiver* NewL(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort);
    ~CHCTLBcspReceiver();
 	virtual void ProcessData();	
	void QueueReadForNextFrame();

private:
    virtual void RunL();
	virtual TInt RunError(TInt aError);
    CHCTLBcspReceiver(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort);
	void ConstructL();
	void DoCancel();
private:
	// Set the receive buffer equal to the 3-DH5 packet size.
	// This value should be changed by licensees if the controller
	// is exceeding this packet size.
	
	// 3-DH5.  Payload = 1021 octets.
	// Buffer length = payload + HCTL overhead
	static const TUint16 KHCTLRecvBufSize = 1022;

	CHCTLBcsp& iHctlBcsp;
    RBuf8 iReceiveBuffer;
    TPtr8 iReceiveBufPtr;
	RBusDevComm& iPort;
    };

#endif // HCTLBCSPRECEIVER_H


