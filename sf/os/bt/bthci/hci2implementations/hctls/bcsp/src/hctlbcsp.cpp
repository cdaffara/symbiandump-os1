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

#include "hctlbcsp.h"

#include "hctlbcspFrameQueue.h"
#include "bcsputils.h"
#include "hctlbcspframe.h"
#include "hctlbcspconsts.h"
#include "linkestablishment.h"
#include "hctlbcspreceiver.h"
#include "hctlbcspsequencer.h"
#include "hctlbcspcontrollermanager.h"
#include "debug.h"

#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hci/hctlchannelobserver.h>
#include <bluetooth/hci/hctldataobserver.h>
#include <bluetooth/hci/hctleventobserver.h>

/**
	Implementation of Class CHCTLBcsp
*/

CHCTLBcsp::CHCTLBcsp()
	{
	LOG_FUNC
	}

CHCTLBcsp::~CHCTLBcsp()
	{
	LOG_FUNC

	HCI_LOG_UNLOAD(this);
	delete iLinkEstablishment;
	delete iReceiver;
	delete iSequencer;
	delete iFrameQueue;
	delete iReceivedFrame;
	delete iControllerMan;
	}

CHCTLBcsp* CHCTLBcsp::NewL()
	{
	LOG_STATIC_FUNC

	CHCTLBcsp* self=new(ELeave) CHCTLBcsp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CHCTLBcsp::ConstructL()
	{
	LOG_FUNC
	
	HCI_LOG_LOADL(this, KHCILoggerDatalinkTypeH1);	// Technically it's BCSP but we're going
													// to log at a higher layer
	BaseConstructL(KIniFileName);													
	iControllerMan = CHCTLBcspControllerManager::NewL(*this, Port(), PowerCtrlMode());
	}

TAny* CHCTLBcsp::Interface(TUid aUid)
	{
	LOG_FUNC

	TAny* ret = NULL;
	switch(aUid.iUid)
		{
		case KHCTLInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCTLInterface*>(this));
			break;
		case KHCTLPowerInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCTLPowerInterface*>(iControllerMan));
			break;
		case KHCHardResetUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHardResetInitiator*>(this));
			break;	
		
		default:
			break;
		};

	return ret;
	}

void CHCTLBcsp::DoConfigL()
/**
	
	Method to set the TCommConfigV01 settings for UART based HCTLs
	enforced implementation by pure virtual method in CHCTLUartBase to allow for the original uart transport (H4) and BCSP
	specific settings

	@leave method will leave if Port().SetConfig() fails
*/
	{
	LOG_FUNC

	Port().ResetBuffers();
	TCommConfig conf;
	Port().Config(conf);

	TCommConfigV01& config = conf(); //Get reference to TCommConfig iConfig

	config.iDataBits = EData8;				
	config.iStopBits = EStop1; 
	config.iTerminatorCount = 1;
	config.iTerminator[0] = 0xc0;
	config.iParity = EParityEven;				 
	config.iParityError = KConfigParityErrorFail;
	config.iHandshake = 0; 
	config.iXonChar = 0;
	config.iXoffChar = 0;
	
	LEAVEIFERRORL(Port().SetConfig(conf));
	}


//Implemenation of MHCTLInterface

TInt CHCTLBcsp::MhiWriteAclData(const TDesC8& aData)
	{
	LOG_FUNC
	
	iFrameQueue->AddReliableFrame(aData, KBcspACLDataChnl);
	HCI_LOG_FRAME(this, KHCILoggerHostToController | KHCILoggerACLDataFrame, aData);
	return KErrNone;
	}

TInt CHCTLBcsp::MhiWriteSynchronousData(const TDesC8& aData)
	{
	LOG_FUNC

	iFrameQueue->AddUnreliableFrame(aData, KBcspSynchronousDataChnl, 0, EFalse);
	HCI_LOG_FRAME(this, KHCILoggerHostToController | KHCILoggerSynchronousDataFrame, aData);
	return KErrNone;
	}

TInt CHCTLBcsp::MhiWriteCommand(const TDesC8& aData)	
	{
	LOG_FUNC

	TRAPD(err,DoWriteCommandL(aData));
	if(err!=KErrNone)
		{
		return KErrBcspWriteCommandDataFailed;
		}
	else
		{
		HCI_LOG_FRAME(this, KHCILoggerHostToController | KHCILoggerCommandOrEvent, aData);
		return err;
		}
	}

TInt CHCTLBcsp::WriteBcCmd(const TDesC8& aData)	
	{
	LOG_FUNC

	TRAPD(err,DoWriteBcCmdL(aData));
	if(err!=KErrNone)
		{
		return KErrBcspWriteBcCmdDataFailed;
		}
	else
		{
		HCI_LOG_FRAME(this, KHCILoggerHostToController | KHCILoggerCommandOrEvent, aData);
		return err;
		}
	}

void CHCTLBcsp::MhiSetQdpPluginInterfaceFinder(MQdpPluginInterfaceFinder& aQdpPluginInterfaceFinder)
	{
	iQdpPluginInterfaceFinder = &aQdpPluginInterfaceFinder;
	}
	
void CHCTLBcsp::MhriStartHardReset()
	{
	iControllerMan->HardReset();	
	}
	
void CHCTLBcsp::MhiGetAclDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	aHeaderSize = KHCTLAclDataHeaderSize;
	aTrailerSize = KHCTLAclDataTrailerSize;
	}
	

void CHCTLBcsp::MhiGetSynchronousDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	aHeaderSize = KHCTLSynchronousDataHeaderSize;
	aTrailerSize = KHCTLSynchronousDataTrailerSize;
	}

void CHCTLBcsp::MhiGetCommandTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	aHeaderSize = KHCTLCommandHeaderSize;
	aTrailerSize = KHCTLCommandTrailerSize;
	}

//MHCTLInterface  END


void CHCTLBcsp::DoWriteCommandL(const TDesC8 &aData)
/**
	
	Method called from MhiWriteCommand

	@param aData
	aData is the formatted Command payload from the linkmanager

*/
	{
	LOG_FUNC

#ifdef __DEBUG_FLOG_STACK_TX_
	LOG(_L8("STACK TX"));
	LOGHEXDESC(aData);
#endif
	User::LeaveIfError(iFrameQueue->AddReliableFrame(aData, KBcspCommEvntChnl));//KBcspBcCmdChnl
	}

void CHCTLBcsp::DoWriteBcCmdL(const TDesC8 &aData)
/**
	
	Method called from WriteBcCmdData

	@param aData
	aData is the formatted BCCMD payload from the controller manager

*/
	{
	LOG_FUNC

#ifdef __DEBUG_FLOG_STACK_TX_
	LOG(_L8("STACK TX"));
	LOGHEXDESC(aData);
#endif
	User::LeaveIfError(iFrameQueue->AddReliableFrame(aData, KBcspBcCmdChnl));
	}

/**
Process Received data in to the format expected by the interface to the bottom of the stack
*/

void CHCTLBcsp::ProcessACLData() 
/**
	Passes ACL data to the data observer.
*/
	{
	LOG_FUNC

	__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

	HCI_LOG_FRAME(this, KHCILoggerControllerToHost | KHCILoggerACLDataFrame, iReceivedFrame->Payload());
	iDataObserver->MhdoProcessAclData(iReceivedFrame->Payload());
	}

void CHCTLBcsp::ProcessEventData()
/**
	Passes event data to the data observer.
*/
	{
	LOG_FUNC

	__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

	HCI_LOG_FRAME(this, KHCILoggerControllerToHost | KHCILoggerCommandOrEvent, iReceivedFrame->Payload());
	iEventObserver->MheoProcessEvent(iReceivedFrame->Payload());
	}

void CHCTLBcsp::ProcessBcCmdEventData()
/**
	
	Method which parses the received Command data payload from the controller
	and formats the data to be passed to the controller manager
*/
	{
	LOG_FUNC

	__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

	HCI_LOG_FRAME(this, KHCILoggerControllerToHost | KHCILoggerCommandOrEvent, iReceivedFrame->Payload());
	iControllerMan->ProcessBcCmdEvent(iReceivedFrame->Payload());
	}

void CHCTLBcsp::ProcessSynchronousData()
	{
	LOG_FUNC

	__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

	HCI_LOG_FRAME(this, KHCILoggerControllerToHost | KHCILoggerSynchronousDataFrame, iReceivedFrame->Payload());
	iDataObserver->MhdoProcessSynchronousData(iReceivedFrame->Payload());
	}

TInt CHCTLBcsp::PacketRouter()
/**
	Method called by CHCTLBcspReceiver after packet integrity checks have been performed
	This routes packets according to the ProtocolId associated with the payload type in a 
	given BCSP frame to the appropriate interface to the top of the HCI and thus to the 
	link manager
*/
	{
	LOG_FUNC

	TInt err = KErrNone;
	__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

	switch (iReceivedFrame->ProtocolId())
		{
		case KBcspCommEvntChnl:
			{
			LOG(_L8("HCTLBCSP: Event Packet Received..."));
			ProcessEventData();
			}
			break;

		case KBcspBcCmdChnl:
			{
			LOG(_L8("HCTLBCSP: BC Command Packet Received..."));
			ProcessBcCmdEventData();
			}
			break;

		case KBcspACLDataChnl:
			{
			LOG(_L8("HCTLBCSP: ACL Data Packet Received..."));
			ProcessACLData();
			}
			break;

		case KBcspSynchronousDataChnl:
			{
			LOG(_L8("HCTLBCSP: Synchronous Data Packet Received..."));
			ProcessSynchronousData();
			}
			break;

		case KBcspLinkChnl:
			{
			LOG(_L8("HCTLBCSP: Link Est Packet Received..."));
			__ASSERT_DEBUG(iLinkEstablishment!=NULL, PANIC(KBcspPanicCat, EBadLinkPointer));
			TRAP(err,iLinkEstablishment->ProcessLinkMsg(iReceivedFrame->Payload()));
			}	
			break;

		default:
			LOG(_L8("HCTLBCSP: ERROR: KErrBcspUnRecognizableHCIData..."));
			break;
		};
		
	return err;
	}

void CHCTLBcsp::QueueReadForNextFrame()
	{
	LOG_FUNC

	iReceiver->QueueReadForNextFrame();
	}

void CHCTLBcsp::CanSend(TBool aCanSend)
/**
	For BCSP this is always taken to be all channels or no channels
*/
	{
	LOG_FUNC

	iCanSend = aCanSend;
	// We can only open the channel after unchoked and iChannelObserver has been set
	if(!iChoked && iChannelObserver)
		{
		if(iCanSend)
			{
			iChannelObserver->MhcoChannelOpen(KHCITransportAllChannels);
			}
		else
			{
			iChannelObserver->MhcoChannelClosed(KHCITransportAllChannels);
			}
		}
	}

void CHCTLBcsp::TxAckMsg()
/**
	Method to send a BCSP Ack message
	This is only called when an the Ack timer fires i.e. a certain amount of time has
	passed without there being an outgoing BCSP frame to convey acknowledgement of received 
	reliable frames
*/
	{	
	LOG_FUNC

	iFrameQueue->AddUnreliableFrame(KBcspAckChnl, iSequencer->TxAck(), EFalse);
	}

void CHCTLBcsp::TxLinkMsg(const TDesC8& aData)
/**
	
	This is a method that takes a BCSP Link Establishment message as its parameter
	and adds the data along with a number of other bits of information to make a frame 
	on the Unreliable frame queue via the iFrameQueue::AddUnreliableFrame() method
*/
	{	
	LOG_FUNC

	iFrameQueue->AddUnreliableFrame(aData, KBcspLinkChnl, 0, ETrue);
	}

TInt CHCTLBcsp::HandleRx(const TDesC8 &aReceivedFrame)
/**
	
	This is a method called from the CHCTLBcspReceiver::ProcessDataL()
	with the parameter
	@param const TDesC8 &aReceivedFrame
	@return err
	This method attempts to slip decode the received frame 
	Then if that is successful the remainder of the BCSP frame integrity checks are performed
	using the iReceivedFrame->IsValid() method
	Assuming that this all works without errors control is then passed to the sequencer
	via the iSequencer->HandleRx(iReceivedFrame) call
	and then the iReceivedFrame->Reset() method is called
*/
	{
	LOG_FUNC

	TInt rerr = iReceivedFrame->SlipDecodeFrame(aReceivedFrame);
	if (rerr == KErrNone)
		{
		// We've succeeded in decoding the Slip Frame into a CRxHctlBcspFrame
		__ASSERT_DEBUG(iReceivedFrame->IsValid(), PANIC(KBcspPanicCat, EBadBcspFrame));

#ifdef __DEBUG_RxDecodedFrame__
		FlogRx();
#endif

		iSequencer->HandleRx(iReceivedFrame);
		iReceivedFrame->Reset();
		}

	return rerr;
	}

TBool CHCTLBcsp::CheckIsAckPacket() const
/**
	
	This is a relatively simple method that examines the ProtocolId of the Received Frame
	and attempts to match it to the the KBcspAckChnl constant and then returns a TBool depending
	on whether or not these match
*/
	{
	LOG_FUNC

	return (iReceivedFrame->ProtocolId() == KBcspAckChnl);
	}

#ifdef __DEBUG_RxDecodedFrame__
void CHCTLBcsp::FlogRx()
	{
	LOG(_L8("RxFrame"));
	LOG1(_L8("Protocol type reliable(1) unreliable (0) = %d\n"),iReceivedFrame->IsReliableProtcolType());
	LOG1(_L8("Ack value = %d\n"),iReceivedFrame->Ack());
	LOG1(_L8("Seq value = %d\n"),iReceivedFrame->Sequence());
	LOG1(_L8("Protocol ID (Command/Event(5)) (ACL) (Ack Packet) = %d\n"),iReceivedFrame->ProtocolId());
	LOG1(_L8("Payload Length = %d\n"),iReceivedFrame->PayloadLength());
	LOG1(_L8("Checksum value = %d\n"),iReceivedFrame->CheckSum());
	LOG1(_L8("Header bytes = %d\n"),iReceivedFrame->FlagsField());
	LOG(_L8("Payload..."));
	LOGHEXDESC(iReceivedFrame->Payload());
	}
#endif



/**
	See comments on iChoked	
*/
void CHCTLBcsp::Choke()
	{
	LOG_FUNC

	iChoked = ETrue;
	CanSend(iCanSend);
	}

/**
	See comments on iChoked	
*/
void CHCTLBcsp::UnChoke()
	{
	LOG_FUNC

	iChoked = EFalse;
	if(iControllerMan->BcspLinkEstablished())
		{
		CanSend(iCanSend);
		}
	}

TBool CHCTLBcsp::Muzzled()
	{
	return iMuzzled;
	}

void CHCTLBcsp::ResetMuzzled()
	{
	iMuzzled = EFalse;
	}

/**

	Handle a reset of the BCSP transport.
	1) Reset the BCSP layer
	2) Inform the stack of a HCI error
	3) Stack will reinitialise with a "reset" command etc
*/
void CHCTLBcsp::HandlePeerReset()
	{
	LOG_FUNC

	// If a poweroff is requested, the next reset will lead to a reset of the SP link
	// and will set the state to shy sending a sync frame every 250ms
	// So, Muzzle the host to avoid sending this command 
	if(iControllerMan->PowerOffRequested())
		{
		iMuzzled = ETrue;
		}

	Reset(); //reset BCSP
	
	if(!iControllerMan->ExpectedControllerReset() && !iControllerMan->PowerOffRequested())
		{
		// Inform stack of hardware error	
		TUint8 errReason = static_cast<TUint8>(EInvalidHCIParameter);
		TBuf8<3> buf(3);
		buf[0]= EHardwareErrorEvent; //error code
		buf[1] = 1; //length
		buf[2] = errReason; //parameter
		iEventObserver->MheoProcessEvent(buf);	
		}
	}

/**
	Reset the BCSP layer
*/
void CHCTLBcsp::Reset()
	{
	LOG_FUNC

	Choke();	//Disconnected
	//Reset the link establishment state machine
	iLinkEstablishment->Reset();
	//Reset and clear transmit queues
	iFrameQueue->Reset();
	//Reset the sequencer
	iSequencer->Reset();
	}

//Implementation of pure virtual functions in CHCTLUartBase

void CHCTLBcsp::PortOpenedL()
	{
	LOG_FUNC

	__ASSERT_DEBUG(Port().Handle(), PANIC(KBcspPanicCat, EPortNotOpen));
		
	DoConfigL();
	
	
	iChoked = ETrue;

	iFrameQueue = CHCTLBcspFrameQueue::NewL(*this);

	// Creation of the sequencer creates the sender Active Object. 
	// The sender Active Object must be added to the Active Scheduler before 
	// the receiver Active Object so that it gets preferential treatment. It 
	// is reported that otherwise the response from a command can come in 
	// before the sending client is told that the send has completed!
	iSequencer = CHCTLBcspSequencer::NewL(*this, Port(), *iFrameQueue);
	iReceiver=CHCTLBcspReceiver::NewL(*this, Port());

	iFrameQueue->SetSequencer(*iSequencer);

	iLinkEstablishment= CLinkEstablishment::NewL(*this);

	iReceivedFrame = CRxHctlBcspFrame::NewL();
	}

void CHCTLBcsp::MhiSetDataObserver(MHCTLDataObserver& aDataObserver)
	{
	iDataObserver = &aDataObserver;
	}

void CHCTLBcsp::MhiSetEventObserver(MHCTLEventObserver& aEventObserver)
	{
	iEventObserver = &aEventObserver;
	}

void CHCTLBcsp::MhiSetChannelObserver(MHCTLChannelObserver& aChannelObserver)
	{
	iChannelObserver = &aChannelObserver;
	//give the stack an initial kick
	CanSend(iCanSend);
	}

void CHCTLBcsp::MhiSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver)
	{
	iControllerStateObserver = &aControllerStateObserver;
	iControllerMan->SetObserver(aControllerStateObserver);
	iControllerMan->Start();
	}
