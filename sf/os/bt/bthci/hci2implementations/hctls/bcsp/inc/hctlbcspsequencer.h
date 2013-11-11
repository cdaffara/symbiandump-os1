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


#ifndef HCTLBCSPSEQUENCER_H
#define HCTLBCSPSEQUENCER_H

#include <e32base.h>

class CDelay;
class CHCTLBcsp;
class RBusDevComm;
class CHCTLBcspFrameQueue;
class CHCTLBcspSender;
class CRxHctlBcspFrame;

NONSHARABLE_CLASS(CHCTLBcspSequencer) :public CBase
/**
CHCTLBcspSequencer
Provides the Sequencing layer as described in the BCSP Spec. this permits reliable retransmission
of ACL and Command Data and in the final version of BCSP a windowing mechanism allowing multiple 
frames < 7 to be transmitted without acknowledgment for each individual frame.
  	
This class is not intended for derivation.
*/
	{
public:
	static CHCTLBcspSequencer* NewL(CHCTLBcsp& aHCTLbcsp, RBusDevComm& aPort,CHCTLBcspFrameQueue& aBcspFrameQueue);
	~CHCTLBcspSequencer();
	
	void WakeUp();			// Set the state machine to ESending and then in Entry() does the same as SendNextPacket()
	void SendNextPacket();	// Gets next packet from iBcspFrameQueue and Sends it directly to the UART
	void HandleRxAck();		// Triggers removal of Acked frames
	void SendPacket(TDesC8* aFrame, TBool aIsReliable);

	TInt SendQueuedFrame();

	void HandleRx(CRxHctlBcspFrame* aFrame);
	
	TUint8 TxAck() const;
	void ResendState();
	void HandleRxAckTimeout();

 	//BCSP peer reset functions
 	void Reset();
 	void HandlePeerReset();

private:
	CHCTLBcspSequencer(CHCTLBcsp& aHCTLbcsp, CHCTLBcspFrameQueue& aBcspFrameQueue);
    void ConstructL(RBusDevComm& aPort);
	void UpdateTxAckVal();
	void StartRxAckTimer();
	void StartTxAckTimer();
	void TxAckMsg();
	
	static TInt TxTimeout(TAny* aThis);
	static TInt RxTimeout(TAny* aThis);

private:
	enum TBcspRelRxState
		{
		EIdle,
		EReceiving
		};

	TUint8 iRxAck;	//Updated by HandleRx()
	TUint8 iRxSeq;
	TUint8 iRxSeqExpected;
	TUint8 iRxSeqLast;
	TUint8 iPacketAvail;

	TUint8 iTxAck;

	TBcspRelRxState iRelRxState;

	TUint8 iRxTimeoutCount;
	CDelay* iRxAckTimer;
	CDelay* iTxAckTimer;

	CHCTLBcsp& iHctlBcsp;
	CHCTLBcspFrameQueue& iFrameQueue;

	CHCTLBcspSender* iSender;
	};
	
#endif // HCTLBCSPSEQUENCER_H
