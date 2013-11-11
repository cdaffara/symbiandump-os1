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

#include "hctlbcspsequencer.h"

#include "hctlbcspFrameQueue.h"
#include "hctlbcsp.h"
#include "hctlbcspframe.h"
#include "hctlbcspconsts.h"
#include "hctlbcspsender.h"

#include "debug.h"

#include <bluetooth/hci/delay.h>


#define MAX_RX_TIMEOUT_ALLOWED			5
/**
Implementation of Class CHCTLBcspSequencer
*/

CHCTLBcspSequencer::CHCTLBcspSequencer(CHCTLBcsp& aHctlBcsp, CHCTLBcspFrameQueue& aBcspFrameQueue):
	iRelRxState(EIdle), 
	iHctlBcsp(aHctlBcsp),
	iFrameQueue(aBcspFrameQueue)
	{
	LOG_FUNC
	}

CHCTLBcspSequencer* CHCTLBcspSequencer::NewL(CHCTLBcsp &aHCTLBcsp, RBusDevComm& aPort, CHCTLBcspFrameQueue& aBcspFrameQueue)
	{
	LOG_STATIC_FUNC

	CHCTLBcspSequencer* self=new(ELeave) CHCTLBcspSequencer(aHCTLBcsp, aBcspFrameQueue);
    CleanupStack::PushL(self);
    self->ConstructL(aPort);
    CleanupStack::Pop();
    return self;
	}

void CHCTLBcspSequencer::ConstructL(RBusDevComm& aPort)
    {
	LOG_FUNC

	TCallBack cb = TCallBack(CHCTLBcspSequencer::RxTimeout, this);
	iRxAckTimer = CDelay::NewL(cb, CActive::EPriorityStandard);

	cb = TCallBack(CHCTLBcspSequencer::TxTimeout, this);
	iTxAckTimer = CDelay::NewL(cb, CActive::EPriorityStandard);

    iSender  = CHCTLBcspSender::NewL(*this,aPort);
    }

CHCTLBcspSequencer::~CHCTLBcspSequencer()
	{
	LOG_FUNC

	delete iRxAckTimer;
	delete iTxAckTimer;
	delete iSender;
	}

/**
  	Resets the BCSP sequencer to default state.
*/
void CHCTLBcspSequencer::Reset()
 	{
	LOG_FUNC

 	iSender->Cancel();
 	//reset rx variables
 	iRxAck = 0;
 	iRxSeq = 0;
 	iRxSeqExpected = 0;
 	iRxSeqLast = 0;
 	}
 
/**
  	Called when a peer reset of BCSP is detected
 	Passes the notification up the BCSP stack for handling
*/
void CHCTLBcspSequencer::HandlePeerReset()
 	{
	LOG_FUNC

 	iHctlBcsp.HandlePeerReset();
 	}
 
void CHCTLBcspSequencer::HandleRx(CRxHctlBcspFrame* aFrame)
	{
	LOG_FUNC

	iRelRxState = EReceiving;

	iRxSeqLast = iRxSeq;	
	iRxAck = aFrame->Ack();
	
	if (aFrame->IsReliableProtcolType())
		{
		iRxSeq=aFrame->Sequence();
		}


#ifdef __DEBUG_SEQ_Values__

	LOG1(_L8("\tiRxAck value = %d"),iRxAck);
	LOG1(_L8("\tiRxSeq value = %d"),iRxSeq);
	LOG1(_L8("\tiRxSeqLast value = %d"),iRxSeqLast);
	LOG1(_L8("\tiRxSeqExpected value = %d"),iRxSeqExpected);
	LOG1(_L8("\tiTxAck value = %d"),iTxAck);
#endif

	// Call HandleRxAck() to process the Ack Value conveyed in the received frame header
	HandleRxAck();

	// Check if the received frame is unreliable and on the BCSP Link Establihsment channel
	// If it is then route directly to the iHctlBcsp.PacketRouter()
	if(!aFrame->IsReliableProtcolType() && aFrame->ProtocolId() == KBcspLinkChnl) //Route LE packets
		{
#ifdef __DEBUG_SEQ_VERBOSE__ //Enable flogging for Sequencer
		LOG(_L8("HCTLBCSP: CHCTLBcspSequencer::HandleRx() Pass LE Packet to LE Entity\n\n"));
#endif
		iHctlBcsp.PacketRouter();
		return;
		}

	// Otherwise the packet is reliable and passes into this switch statement
	switch (iRelRxState)	//Verify ProtocolId is valid and permissable
		{
		case EIdle: // Fall through.
		case EReceiving:
			{
			if(iRxSeq == iRxSeqExpected && (iHctlBcsp.CheckIsAckPacket()==EFalse))
				{
				iTxAck = static_cast <TUint8>((iRxSeq+1)%8);
				iRxSeqExpected = iTxAck;
				iHctlBcsp.PacketRouter();	// Forward RxFrame to PacketRouter
				}
			//else drop the packet
			StartTxAckTimer();
			}
			break;

		default:
			break;
		};
	}

void CHCTLBcspSequencer::SendNextPacket()
/**
	This method attempts to send the next frame on the queue.
*/
	{
	LOG_FUNC

	if(SendQueuedFrame() == KErrBcspMaxRetries)
		{
		HandlePeerReset();
		}
	}

TUint8 CHCTLBcspSequencer::TxAck() const
	{
	LOG_FUNC

	return iTxAck;
	}

void CHCTLBcspSequencer::HandleRxAck()
	{
	LOG_FUNC

	// An ACK has been received, so reset the Tx timeout counter
	iRxTimeoutCount = 0;
	// Returns ETrue if there are unacknowledged packets remaining
	// in the window
	if(iFrameQueue.AckReceived(iRxAck))
		{
		StartRxAckTimer();
		}
	else
		{
		iRxAckTimer->Cancel();
		}
	}

/*static*/ TInt CHCTLBcspSequencer::RxTimeout(TAny* aThis)
	{
	LOG_STATIC_FUNC
	
	reinterpret_cast<CHCTLBcspSequencer*>(aThis)->HandleRxAckTimeout();
	
	return KErrNone;
	}

void CHCTLBcspSequencer::SendPacket(TDesC8* aFrame, TBool aIsReliable)	
/**
	
	This method writes @param aFrame and starts the RxAckTimer if @param aIsReliable
	is ETrue
*/
	{
	LOG_FUNC

	iSender->Write(*aFrame);	

	if (aIsReliable)
		{
		StartRxAckTimer();
		}
	}

TInt CHCTLBcspSequencer::TxTimeout(TAny* aThis)
	{
	LOG_STATIC_FUNC
	
	reinterpret_cast<CHCTLBcspSequencer*>(aThis)->TxAckMsg();
	
	return KErrNone;
	}

void CHCTLBcspSequencer::TxAckMsg() 
	{
	LOG_FUNC

	iHctlBcsp.TxAckMsg();
	}

void CHCTLBcspSequencer::StartRxAckTimer()
	{
	LOG_FUNC

	iRxAckTimer->Cancel();
	iRxAckTimer->After(KRxAckTimeout);
	}

void CHCTLBcspSequencer::StartTxAckTimer()
	{
	LOG_FUNC

	if ( !iTxAckTimer->IsActive() )
		{
		iTxAckTimer->After(KRxAckTimeout);
		}
	}

void CHCTLBcspSequencer::HandleRxAckTimeout()
	{
	LOG_FUNC

	// Increment the Rx ACK Timeout counter
	iRxTimeoutCount++;
	iFrameQueue.AckTimeout();
	// While the maximum rx timeout allowed is not reached, rearm the timer
	// waiting for the ACK.
	// If the max is reached, we consider that the peer will not respond and
	// has been poweroff. So, do not rearm the timer and reset the counter.
	// Notice that the BC4 ship implements the same mechanism.
	if (iRxTimeoutCount < MAX_RX_TIMEOUT_ALLOWED)
		{
		StartRxAckTimer();
		}
	else
		{
		iRxTimeoutCount = 0;
		}
	}

void CHCTLBcspSequencer::WakeUp()
/**
	Method made available to trigger the TxStateMachine to do something 
	depending upon its current state
*/
	{
	LOG_FUNC
	if(!iSender->IsActive())
		{
		SendNextPacket();
		}
	}

TInt CHCTLBcspSequencer::SendQueuedFrame()
/**
	Method to get the Next Frame from the frame queue and then write it via 
	the SendPacket method
*/
	{
	LOG_FUNC

	TDesC8* frame = NULL;
	TBool isReliable = EFalse;
	TInt err = iFrameQueue.GetNextFrame(frame,isReliable);
	if (!err)
		{
		SendPacket(frame,isReliable);
		}
	return err;
	}


