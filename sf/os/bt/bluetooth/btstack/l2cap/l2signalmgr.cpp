// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the l2cap Muxer.
// 
//

#include <bluetooth/logger.h>

#include <math.h>

#include <bluetooth/hci/aclpacketconsts.h>
#include <bluetooth/aclsockaddr.h>

#include "l2signalmgr.h"
#include "L2CapPDU.h"
#include "l2capLinkSignalHandler.h"
#include "l2capSAPSignalHandler.h"
#include "L2CapDataController.h"
#include "l2capMuxController.h"
#include "l2capEntityConfig.h"
#include "l2cap.h"
#include "l2util.h"
#include "btconsts.h"
#include "L2CapDebugControlInterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

#ifdef _DEBUG
PANICCATEGORY("l2cap");
#endif

CL2CAPMux* CL2CAPMux::NewL(CL2CAPMuxController& aMuxController, const TBTDevAddr& aAddr)
	{
	LOG_STATIC_FUNC
	CL2CAPMux* sig=new (ELeave) CL2CAPMux(aMuxController, aAddr);
	CleanupStack::PushL(sig);
	sig->ConstructL();
	CleanupStack::Pop();
	return sig;
	}

CL2CAPMux::CL2CAPMux(CL2CAPMuxController& aMuxController, const TBTDevAddr& aAddr)
 : iMuxController(aMuxController),
   iLinkState(ELinkNone),
   iRemote(aAddr),
   iIdleTimerActive(EFalse),
   iDataHandlers(_FOFF(CL2CapBasicDataController, iLink)),
   iSapSignalHandlers(_FOFF(CL2CapSAPSignalHandler, iLink)),
   iPDUsWaitingResendQ(_FOFF(HL2CapPDU, iLink)),
   iPDUsWaitingSend(_FOFF(HL2CapPDU, iLink)),
   iTryToSendPriority(CActive::EPriorityHigh),
   iUpdateTryToSendPriority(EFalse),
   iNextCID(KL2CapDynamicCIDStart),
   iQlink(this)
	{
	LOG_FUNC
	TCallBack cb(IdleTimerExpired, this);
	iIdleTimerEntry.Set(cb);
	
#ifdef _DEBUG
	// Set up the next CID to be one off of the wrap point to
	// check that functionality
	iNextCID = 0xfffe;
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EMuxes,
	                             L2capDebugInfo::EAllocated));
#endif	                             
	}


void CL2CAPMux::ConstructL()
	{
	LOG_FUNC
	iBoundLinkSignalHandler = CL2CapLinkSignalHandler::NewL(this);
	iFragmentSender = new (ELeave) HFragmentedPDUSender(*this);
	TCallBack canSendCallBack(AsyncCallBackForTryToSend, this);
	iAsyncCallBackForTryToSend = new (ELeave)CAsyncCallBack(canSendCallBack, iTryToSendPriority);
	TCallBack idleMuxCallBack(AsyncCallBackForIdleMux, this);
	iAsyncCallBackForIdleMux = new (ELeave)CAsyncCallBack(idleMuxCallBack, CActive::EPriorityHigh);	
	}


//Passive connections call this with a valid SAP 
//Active connections pass NULL into this and one gets created.
void CL2CAPMux::CompleteACLConnect(CServProviderBase* aSAP)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iBoundSAP, Panic(EL2CAPLinkUpWhenNoLinkRequested));

	if (!aSAP)
		{
		TRAPD(err, iBoundSAP = MuxController().Protocol().LowerProtocol()->NewSAPL(KSockBluetoothTypeACL));
		if (err == KErrNone)
			{
			iBoundSAP->SetNotify(this);
			
			const TBTDevAddr& addr = iRemote;
			TBTSockAddr sockAddr;
			sockAddr.SetBTAddr(addr);
			
			TInt err = iBoundSAP->SetRemName(sockAddr);
			__ASSERT_ALWAYS(!err, Panic(EL2CAPCouldNotSetBasebandAddress));

			// tell the SAP what data we can handle
			TACLSockAddr aclSock;
			aclSock.SetPort(EACLPortL2CAP);
			err = iBoundSAP->SetLocalName(aclSock);
			iLinkState = ELinkPending;
			iBoundSAP->ActiveOpen();
			}
		}
	else
		{
		iBoundSAP = aSAP;	// we take ownership of this connected SAP
		iBoundSAP->SetNotify(this);
		iBoundSAP->Start();	// and open the passive connection
		ConnectComplete();	// gets BB MTU and configures this Mux
		}
	
	//Now the Link is up Subscribe for Physical Link Notifications
	MuxController().Protocol().ControlPlane().SubscribePhysicalLink(*this, iRemote);	
	}
	
	
CL2CAPMux::~CL2CAPMux()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iDataHandlers.IsEmpty() && iSapSignalHandlers.IsEmpty(),
	               Panic(EL2CAPMuxDeleteWithResourcesStillRegistered));

	iAsyncCallBackForTryToSend->Cancel();
	delete iAsyncCallBackForTryToSend;

	iAsyncCallBackForIdleMux->Cancel();
	delete iAsyncCallBackForIdleMux;
	 
	MuxController().Protocol().ControlPlane().UnsubscribePhysicalLink(*this,iRemote);
	delete iBoundSAP;

	// Remove timer entry if one has been queued.
	CancelIdle();

	iFragmentSender->PDUSenderFailed();
	
	// Some PDUs may hold a reference to this Mux.
	TDblQueIter<HL2CapPDU> resendIter(iPDUsWaitingResendQ);
	TDblQueIter<HL2CapPDU> sendIter(iPDUsWaitingSend);
	HL2CapPDU* pduPtr;
	
	while((pduPtr = resendIter++) != NULL)
		{
		pduPtr->DeregisterPduOwner();
		}
	while((pduPtr = sendIter++) != NULL)
		{
		pduPtr->DeregisterPduOwner();
		}
	
	iIncomingPDU.Free();
	iMuxControllerLink.Deque();
	delete iBoundLinkSignalHandler;
	delete iFragmentSender;
	
#ifdef _DEBUG
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EMuxes,
	                             L2capDebugInfo::EDeleted));
#endif	                             
	}


TInt CL2CAPMux::SetOption(TUint level,TUint name,const TDesC8 &anOption)
	{
	LOG_FUNC
	return iBoundSAP->SetOption(level, name, anOption);
	}


TInt CL2CAPMux::GetOption(TUint level,TUint name,TDes8& anOption) const
	{
	LOG_FUNC
	return iBoundSAP->GetOption(level, name, anOption);
	}


void CL2CAPMux::DetachFromMux(CL2CapSAPSignalHandler& aSAPSigHandler)
	{
	LOG_FUNC
	// Check if this SSH had a Echo Request outstanding.  If it did discard the 
	// response when it arrives.
	iBoundLinkSignalHandler->DeregisterOutstandingEchoRequests(aSAPSigHandler);
	
	if(Idle())
		{
		LOG(_L("CL2CAPMux: DetatchFromMux -- no current SAPs, queuing idle callback"));

		// The Muxer will be destroyed and
		// the ACL / baseband will be removed by CL2CAPMux::AsyncCallBackForIdleMux()...
		iAsyncCallBackForIdleMux->CallBack();
		}
	else
		{
		LOG(_L("CL2CAPMux: DetatchFromMux -- still have SAPs present or are still sending data"));
		}
	}
	
TInt CL2CAPMux::MuxIdled()
	{
	LOG_FUNC

	// register we have no intention of sending out for now..
	// if data comes in we may if necessary send data later, in which 
	// case we will reopen our link
	if (iLinkState != ELinkNone)
		{
		LOG(_L("CL2CAPMux: MuxIdled -- iLinkState != ELinkNone"));

		// Don't attempt to send anymore data.
		iBoundLinkSignalHandler->FlushPendingCommands();
		
		// Shutdown the ACL.
		iBoundSAP->Shutdown(CServProviderBase::EStopOutput); // we might get overtures back in
		iLinkState = ELinkInbound;
		}
	else
		{
		delete this;
		}
	return KErrNone;
	}

TUint8 CL2CAPMux::NextSigId()
	{
	LOG_FUNC
	// 0 is invalid, so don't use it
	++iSigId;
	if (iSigId == 0)
		{
		iSigId = 1;
		}
	LOG1(_L("L2CAP [l2signalmgr.cpp]: Next signalling Id is %d"), iSigId);
	return iSigId;
	}


TBool CL2CAPMux::Idle()
/**
	Test to see if this L2CAP mux is no longer required.
	If there are no SSHs connected to this muxer, shutdown the ACL
**/
	{
	LOG_FUNC
	return (iSapSignalHandlers.IsEmpty() &&
			iLinkState != ELinkPending &&
			!iFragmentSender->IsPDUBeingSent() &&
			!iIdleTimerActive);
	}

void CL2CAPMux::StartIdleTimer()
	{
	LOG_FUNC
	CancelIdle();
	iIdleTimerActive = ETrue;
    BTSocketTimer::Queue(KL2MuxerIdleTimeout * KL2ProtocolSecondTimerMultiplier, iIdleTimerEntry);
	}


TInt CL2CAPMux::AsyncCallBackForIdleMux(TAny *aMuxer)
/**
	Static function called through TCallBack. aMuxer holds the muxer
	that is idle. Ask mux to bring down its own link.
*/
	{
	LOG_STATIC_FUNC

	CL2CAPMux* muxer = static_cast<CL2CAPMux*>(aMuxer);

	if (muxer->Idle())
		{
		// We're (still) idle! Let's disconnect.
		muxer->MuxIdled();
		}
	return EFalse;	 // no more callbacks thanks!
	}


TInt CL2CAPMux::AsyncCallBackForTryToSend(TAny *aMuxer)
/**
	Static function called whenever idle processing can be scheduled.
	This deals with doing async requests to send data over HCI,
	notifications to SSHs 
**/
	{
	LOG_STATIC_FUNC
	CL2CAPMux* muxer = static_cast<CL2CAPMux*>(aMuxer);

	muxer->TryToSend();
	return EFalse;	// Don't call back any more.
	}

// from MSocketNotify
void CL2CAPMux::ConnectComplete()
/**
	The link for this muxer has come up.
	SSHs are notified by the protocol (who notifies us too).
	Set the Idle timer going here, so if no SAP attaches to this
	Mux, it'll just tidy itself up.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iLinkState != EConnected, Panic(EL2CAPLinkDownConnectComplete));

	// Get the MTU
	TPckgBuf<TInt> buf;

	TInt err;

	err = iBoundSAP->GetOption(KSolBtACL, ELMOutboundACLSize, buf);
	ASSERT_DEBUG(!err);
	iACLMTU = buf();

	err = iBoundSAP->GetOption(KSolBtACL, ELMInboundACLSize, buf);
	ASSERT_DEBUG(!err);
	iACLMRU = buf();

	(void)(err != KErrNone); // keep the compiler happy by taking err as an r-value in urel

	iLinkState = EConnected;

	// Send an Information Request as soon as possible.
	TL2CapEntityInfo info;
	iBoundLinkSignalHandler->PeerEntityConfig().PeerL2CapSupportedFeatures(info);
	
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	sapSignalIter.SetToFirst();
	CL2CapSAPSignalHandler* sapPtr;
	
	while((sapPtr = sapSignalIter++) != NULL)
		{
		sapPtr->LinkUp();
		}
	
	// No point hanging around if nobody actually connects to us!
	// Mainly useful for incoming connections
	if(Idle())
		{
		StartIdleTimer();
		}
	}

void CL2CAPMux::NewL2CAPData(RMBufChain& aData, TUint8 aFlag)
	{
	LOG_FUNC
	TUint8 l2Flags = static_cast<TUint8>(aFlag>>KPacketPBBCFlagShift & KPacketPBFlagMask);
	LOG1(_L("L2CAP Mux received data with flags, %x"), aFlag);

	switch(l2Flags)
		{
		case KFirstHLFragment:
			// This is the start of a PDU.  If an incoming PDU already exists
			// delete it and start a new one.
			if(!iIncomingPDU.IsEmpty())
				{
				LOG(_L("L2CAP [l2signalmgr.cpp]: Warning: Dropped previous incoming packet - start of next packet received"));

				iIncomingPDU.Free();
				}
			break;

		case KContinuingHLFragment:
			// This is a continuation so an existing PDU should exist.
			// If it doesn't drop this data and wait for the next start of 
			// PDU.
			if(iIncomingPDU.IsEmpty())
				{
				LOG(_L("L2CAP [l2signalmgr.cpp]: Warning: Continuation fragment received and dropped - waiting for start"));
	
				aData.Free();
				return;
				}
			break;

		default:
			LOG1(_L("L2CAP [l2signalmgr.cpp] Warning: Unexpected ACL packet type received: %d"), aFlag);
			return;
		};

	if(HL2CapPDU::AddFragment(iIncomingPDU, aData))
		{
		// PDU complete.
		PacketComplete();
		__ASSERT_DEBUG(iIncomingPDU.IsEmpty(), Panic(EL2CAPPacketCompleteFailedToConsumeIncomingPDU));
		}
	}
	
// A full packet is now in the buffer.  Process it
void CL2CAPMux::PacketComplete()
	{
	LOG_FUNC
	TBool packetDelivered = EFalse;
	
	if(iIncomingPDU.Length() >= HL2CapPDU::KPDUHeaderLength)
		{
		iIncomingPDU.Align(HL2CapPDU::KPDUHeaderLength);
		TUint16 cid = HL2CapPDU::PDUCID(iIncomingPDU);

		switch(cid)
			{
			case KL2CapSignallingCID:
				{
				// This is a signalling packet
				HCFramePDU* cFramePtr = new HCFramePDU(iIncomingPDU, iACLMTU);
				if(cFramePtr)
					{
					cFramePtr->CheckDecode();
					if(cFramePtr->HasCommands())
						{
						// The C-Frame can contain many L2CAP commands.  Not all of
						// the commands will be for a SAP signal handler.  If a
						// command can be processed it will be removed from the
						// C-Frame.  Any remaining commands will be offered to the
						// other SAP signal handlers, then the whole process of
						// offering the commands to the SAP signal handlers is
						// repeated until no more commands can be handled by them,
						// after which the commands are offered to the link signal
						// handler.  Only one command is handled at a time because a
						// Command Reject may delete a SAP signal handler, thereby
						// rendering it ineffective at processing further commands.

						TBool commandsWereHandled;
						TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
						CL2CapSAPSignalHandler* sshPtr;
						do
							{
							commandsWereHandled = EFalse;
							sapSignalIter.SetToFirst();
							while (!packetDelivered && (sshPtr = sapSignalIter++) != NULL)
								{
								commandsWereHandled |= sshPtr->HandleIncomingCFrame(cFramePtr);

								// If all commands from the C-Frame have been processed then
								// consider the packet delivered.
								packetDelivered = !cFramePtr->HasCommands();
								}
							}
						while (!packetDelivered && commandsWereHandled);

						if (!packetDelivered)
							{
							// Offer the remaining commands to the link signal handler.
							while (iBoundLinkSignalHandler->HandleIncomingCFrame(cFramePtr))
								;
							}
						}

					// Delete the CFrame.
					delete cFramePtr;
					}
				else
					{
					ErrorAllSignalHandlers(KErrNoMemory);
					iIncomingPDU.Free();
					}
				}
				break;

			case KL2CapConnectionlessCID:
				// Connectionless L2CAP is not currently supported.
				iIncomingPDU.Free();
				break;

			default:
				{
				if(cid >= KL2CapDynamicCIDStart)
					{
					// This is a data frame.
					TDblQueIter<CL2CapBasicDataController> dataControllerIter(iDataHandlers);
					CL2CapBasicDataController* dcPrt;
					while((dcPrt = dataControllerIter++) != NULL && !packetDelivered)
						{
						packetDelivered = dcPrt->HandleIncomingDataFrame(iIncomingPDU);
						}
					}

				if(!packetDelivered)
					{
					// This data frame was not for any channel or was using an
					// invalid CID.  Drop it.
					iIncomingPDU.Free();
					}
				}
				break;
			};
		}
	else
		{
		// Invalid PDU length.  Drop it.
		iIncomingPDU.Free();
		}
	}

void CL2CAPMux::ErrorAllSignalHandlers(TInt aError)
	{
	LOG_FUNC
	TDblQueIter<CL2CapSAPSignalHandler> iter(iSapSignalHandlers);
	CL2CapSAPSignalHandler* sapPtr;
	while((sapPtr = iter++) != NULL)
		{
		sapPtr->Error(aError, MSocketNotify::EErrorAllOperations);
		}
	iBoundLinkSignalHandler->Error(aError, MSocketNotify::EErrorAllOperations);
	}



void CL2CAPMux::NewData(TUint aCount)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iBoundSAP, Panic(EL2CAPUnexpectedSocketUpcall));

	// got data in - we may have half-shutdown (in which case link is ENone(!)
	// reactivate link as we're likely to send out data - we're unlikely to be
	// going idle at this rate
	if (iLinkState == ELinkInbound)
		{
		// to regain full duplex
		iBoundSAP->ActiveOpen();
		}

	while (aCount)
		{
		RMBufChain recvBuffer;
		// Params: data, length, options, aAddr.
		// Of these only data is used in CACLLink::GetData()
		iBoundSAP->GetData(recvBuffer, 0, 0, NULL);

		LOG(_L("CL2CAPMux: Getting data from bound SAP"))

		// we get a datagram
		// format...
		// |-----|------------------/
		// |flag |data              /
		// |-----|------------------/

		ASSERT_DEBUG(recvBuffer.Length()>=1);
		LOG1(_L("CL2CAPMux: Got data length, %d"), recvBuffer.Length())
		TUint8 flags = (recvBuffer.First())->Ptr()[0];
		recvBuffer.TrimStart(1);		// Remove the flags
		NewL2CAPData(recvBuffer, flags);
		
		aCount--;
		}
	}

void CL2CAPMux::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	// passive connects should come via the listener
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::CanClose(TDelete aDelete)
	{
	LOG_FUNC
	if (aDelete == EDetach)
		{
		// so we don't delete the bound SAP - it's decided to own itself
		iBoundSAP = NULL;
		}
	// Delete the Mux.	
	delete this;
	}
 
void CL2CAPMux::CanSend()
	{
	LOG_FUNC
	iAsyncCallBackForTryToSend->CallBack();
	}
	
void CL2CAPMux::TryToSend()
/**
   We may now be able to send data.
   Drain out the Q while we don't get an error on write.
   Deal with sent packets: either delete, or put onto response pendingQ
   if the ResponseIsExpected flag is set.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iACLMTU, Panic(EL2CAPZeroAclMtuInTryToSend)); // zero would be useless...

	TBool flowControlledOff = EFalse;

	TBool dataToSend = ETrue;

	TDblQueIter<CL2CapSAPSignalHandler> sigIter(iSapSignalHandlers);
	TDblQueIter<CL2CapBasicDataController> dcIter(iDataHandlers);

	HL2CapPDU* pduToSend = NULL;

	if(iUpdateTryToSendPriority && !iAsyncCallBackForTryToSend->IsActive())
		{
		iAsyncCallBackForTryToSend->SetPriority(iTryToSendPriority);
		iUpdateTryToSendPriority = EFalse;
		}
		
	while(dataToSend && !flowControlledOff)
		{
		if(!iFragmentSender->IsPDUBeingSent())
			{
			pduToSend = iBoundLinkSignalHandler->GetPDU();
			if(!pduToSend && !iSapSignalHandlers.IsEmpty())
				{
				//Check to see if there are any signalling packets ready
				sigIter.SetToFirst();
				
				CL2CapSAPSignalHandler* sapPtr;
				while((sapPtr = sigIter++) != NULL && !pduToSend)
					{
					pduToSend = sapPtr->GetPDU();
					}
				}

			if(!pduToSend && !iDataHandlers.IsEmpty())
				{
				// This data is prioritised.
				dcIter.SetToFirst();
				
				CL2CapBasicDataController* dcPtr;
				while((dcPtr = dcIter++) != NULL && !pduToSend)
					{
					pduToSend = dcPtr->GetPdu();
					// If the next controller in the ordered list has the same priority
					// Remove this one and move it to the end of the controllers with
					// equal priority.
					if(pduToSend && dcIter && 
					   ((CL2CapBasicDataController*)dcIter)->Config().ChannelPriority() == dcPtr->Config().ChannelPriority())
						{
						dcPtr->iLink.Deque();
						PriorityAddDataController(*dcPtr);
						}
					}
				}
				
			if(pduToSend)
				{
				TInt err = iFragmentSender->FragmentPDU(*pduToSend);
				if(err == KErrNone)
					{
					if (!pduToSend->HasOwner())
						{
						// If the data controller doesn't claim ownership of the PDU, we take over.
						// DCs take care of I-Frames and we own pretty much everything else
						// (B-,C-,S-Frames).
						pduToSend->SetPduOwner(this);
						iPDUsWaitingSend.AddLast(*pduToSend);
						}
					}
				else
					{
					// Error all the SAPs
					ErrorAllSignalHandlers(err);
					delete pduToSend;
					}
				}
			else
				{
				dataToSend = EFalse;
				}
			}

		if(iFragmentSender->IsPDUBeingSent())
			{
			HFragmentedPDUSender::TFragmentSenderStatus rStatus = iFragmentSender->WriteNextFragment(*iBoundSAP, iACLMTU);
			switch(rStatus)
				{
				case HFragmentedPDUSender::EFragmentOK:
					break;
					
				case HFragmentedPDUSender::EFragmentationComplete:
					__ASSERT_DEBUG(!iFragmentSender->IsPDUBeingSent(), Panic(EL2CAPPDUBeingSentAfterFragmentationComplete));

					// Check if PDU's are still present in the re-send queue.
					if(!iPDUsWaitingResendQ.IsEmpty())
						{
						pduToSend = iPDUsWaitingResendQ.First();
						pduToSend->iLink.Deque();
						TInt err = iFragmentSender->FragmentPDU(*pduToSend);
						if(err == KErrNone)
							{
							__ASSERT_DEBUG(pduToSend->IsOwner(this), Panic(EL2CAPUnownedPduOnMuxQ));
							iPDUsWaitingSend.AddLast(*pduToSend);
							}
						else
							{
							delete pduToSend;
							// Error all the SAPs
							ErrorAllSignalHandlers(err);
							}
						}
					else
						{
						LOG(_L("CL2CAPMux: TryToSend -- finished sending a PDU, no more waiting"));

						// If we have no PDUs to send, the last PDU *may* have been the last disconnect
						// on the link.  This would mean that we have no SAPs left, so we can destroy the
						// underlying ACL link.
						
						if(Idle())
							{
							LOG(_L("CL2CAPMux: TryToSend -- no current SAPs, queuing idle callback"));

							// The Muxer will be destroyed and
							// the ACL / baseband will be removed by CL2CAPMux::AsyncCallBackForIdleMux()...
							iAsyncCallBackForIdleMux->CallBack();
							}
						else
							{
							LOG(_L("CL2CAPMux: TryToSend -- still have SAPs present"));
							}
						}
					break;
					
				case HFragmentedPDUSender::EFlowControlledOff:
					flowControlledOff = ETrue;
					break;
					
				default:
					Panic(EL2CAPInvalidHFragmentedPDUSenderStatusCode);
					break;
				};
			}
		}
	}

void CL2CAPMux::CanClose(const TDesC8& /*aDisconnectData*/, TDelete /*aDelete*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::Error(TInt aError,TUint /*aOperationMask*/)
/**
   The link has not come up.
   Error any SSHs that are hanging around.
**/
	{
	LOG_FUNC
	iLinkState = ELinkNone;

	// In the case where a SAP requested a connection but then
	// disconnected prior to the connection failing ensure that
	// the idle timer is started.
	if(Idle())
		{
		// The Muxer will be destroyed and
		// the ACL / baseband will be removed by CL2CAPMux::AsyncCallBackForIdleMux()...
		iAsyncCallBackForIdleMux->CallBack();
		}	
	else
		{
		// Leave Mux around - SSHs detach as necessary
		ErrorAllSignalHandlers(aError);
		}
	}

void CL2CAPMux::Disconnect()
/**
   We no longer have a link to run on.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iLinkState == EConnected, Panic(EL2CAPLinkDownWhenNotUp));

	// no link!
	iLinkState = ELinkNone;

	// Signal to all signal handlers that the link has gone.
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	CL2CapSAPSignalHandler* sapPtr;

	while((sapPtr = sapSignalIter++) != NULL)
		{
		sapPtr->Error(KErrHCILinkDisconnection, MSocketNotify::EErrorAllOperations);
		}
	iBoundLinkSignalHandler->Error(KErrHCILinkDisconnection, MSocketNotify::EErrorAllOperations);
	delete this;
	}


TInt CL2CAPMux::IdleTimerExpired(TAny *aMuxer)
/**
	Idle timer has expired.
	Static function called through TCallBack. aMuxer holds the muxer
	that has actually expired. Ask mux to down its own link.
**/
	{
	LOG_STATIC_FUNC
	CL2CAPMux* muxer = static_cast<CL2CAPMux*>(aMuxer);
	// record that no further idle timer is queued:
	muxer->iIdleTimerActive = EFalse;

	if (muxer->Idle())
		{
		// We're (still) idle! Lets disconnect.
		muxer->MuxIdled();
		}
	return EFalse;	 // no more callbacks thanks!
	}


void CL2CAPMux::CancelIdle()
	{
	LOG_FUNC
	if(iIdleTimerActive)
		{
		// there's something to cancel
		iIdleTimerActive = EFalse;
		BTSocketTimer::Remove(iIdleTimerEntry);
		}
	}



void CL2CAPMux::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
#ifdef _DEBUG
		Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::NoBearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
#ifdef _DEBUG
	Panic(EL2CAPUnexpectedSocketUpcall);
#endif
	}

void CL2CAPMux::HandlePduSendComplete(HL2CapPDU& aPdu)
	{
	LOG_FUNC
	// We were honored with the ownership of the PDU, so it must be on our list.
	aPdu.iLink.Deque(); 
	delete &aPdu;
	}

void CL2CAPMux::HandlePduSendError(HL2CapPDU& aPdu)
	{
	LOG_FUNC
	L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::EFlushedPDUMuxerResend));
	// Note: this only handles PDUs we're the owner of.

	// Delink it from iPDUsWaitingSendQ.
	aPdu.iLink.Deque(); 

#ifdef _DEBUG
	// Check that the PDU does not already exist in the list.
	TDblQueIter<HL2CapPDU> iter(iPDUsWaitingResendQ);
	HL2CapPDU* pduPtr;
		
	while((pduPtr = iter++) != NULL)
		{
		if(pduPtr == &aPdu)
			{
			Panic(EL2CAPRequestToResendAPDUThatIsAlreadyInTheResendQueue);
			}
		}
#endif
	iPDUsWaitingResendQ.AddLast(aPdu);
	}

void CL2CAPMux::RegisterDataPDUHandler(CL2CapBasicDataController& aPDUDataHandler)
	{
	LOG_FUNC
	PriorityAddDataController(aPDUDataHandler);
	iNumberOfDataChannels++;
	
	iMuxController.UpdateMuxerPriorities();
	}

void CL2CAPMux::DataChannelRemoved(CL2CapBasicDataController* aDataController)
	{
	LOG_FUNC
	aDataController->iLink.Deque();
	iNumberOfDataChannels--;
	
	iMuxController.UpdateMuxerPriorities();
	}

void CL2CAPMux::ChannelPriorityUpdated(CL2CapBasicDataController& aDataController)
	{
	LOG_FUNC
	// First remove the channel.
	aDataController.iLink.Deque();

	// Now re-register the channel with the Muxer.
	RegisterDataPDUHandler(aDataController);
	}
	
void CL2CAPMux::MuxerPriorityUpdate(TInt aPriority)
	{
	LOG_FUNC
	iTryToSendPriority = aPriority;
	if(!iAsyncCallBackForTryToSend->IsActive())
		{
		iAsyncCallBackForTryToSend->SetPriority(iTryToSendPriority);
		}
	else
		{
		iUpdateTryToSendPriority = ETrue;
		}
	}	

TInt CL2CAPMux::GetSumMuxerChannelPriorities()
	{
	LOG_FUNC
	// The sum is used to calculate the mux priority for inter-mux scheduling.
	TInt prioritySum = 0;

	TDblQueIter<CL2CapBasicDataController> dataControllerIter(iDataHandlers);
	CL2CapBasicDataController* dcPrt;
	while((dcPrt = dataControllerIter++) != NULL)
		{
		prioritySum += dcPrt->Config().ChannelPriority();
		}

	return prioritySum;
	}	


void CL2CAPMux::PDUAvailable()
	{
	LOG_FUNC
	iAsyncCallBackForTryToSend->CallBack();
	}

TBool CL2CAPMux::HardwareFlushAllowed()
// H/W flush allowed if no channel using Basic Mode.
// Channels using flow control are deemed unreliable
// so expect to be flushed if need be.
// Channels using L2Cap retransmission are covered by that
// retransmission, and so can have the current transmission
// flushed if need be.
	{
	LOG_FUNC
	TDblQueIter<CL2CapBasicDataController> iter(iDataHandlers);
	CL2CapBasicDataController* handlerPtr;
	TBool rerr = ETrue;
	
	while((handlerPtr = iter++) != NULL)
		{
		if(handlerPtr->IsBasicDataVersion())
			{
			rerr = EFalse;
			break;
			}
		}
	return rerr;
	}
	
void CL2CAPMux::ProcessFlushTimerExpiry()
	{
	LOG_FUNC
	// The PDU currently being sent could have been flushed.  If it has
	// set the current outgoing PDU pointer to zero.
	iFragmentSender->CheckForFlushed();
	
	if(HardwareFlushAllowed())
		{
		iBoundSAP->SetOption(KSolBtACL, KSolBtACLFlushOccured, KNullDesC8);
		}

	PDUAvailable();
	}

void CL2CAPMux::RegisterSAPSignalHandler(CL2CapSAPSignalHandler& aSAPSigHandler)
	{
	LOG_FUNC
	// Cancel the idle timer if it is running.
	CancelIdle();

#ifdef _DEBUG
	// Check that the SAP signal handler does not already exist in the list.
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	while (sapSignalIter)
		{
		if (&aSAPSigHandler == sapSignalIter++)
			{
			Panic(EL2CAPSAPSignalHandlerRegisteredTwice);
			}
		}
#endif
	iSapSignalHandlers.AddLast(aSAPSigHandler);
	
	aSAPSigHandler.SAPSignalHandlerRegistered(*this, iBoundLinkSignalHandler->PeerEntityConfig());
	// If the ACL link is closing, re-open it by calling active
	// open.  If there is currently is no link and this Muxer
	// is configured for the desired remote address, then
	// call active open to establish a connection.
	if(iLinkState == ELinkInbound || 
	   ((iLinkState == ELinkNone) && iBoundSAP))
		{
		iLinkState = ELinkPending;
		iBoundSAP->ActiveOpen();
		}
	else
		{
		// The link is already up.  Inform the SAP signal handler.	
		if(iLinkState == EConnected)
			{
			aSAPSigHandler.LinkUp();
			}
		}
	}
	
void CL2CAPMux::L2CapEntityConfigUpdated()
	{
	LOG_FUNC
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	sapSignalIter.SetToFirst();
	CL2CapSAPSignalHandler* sapPtr;
		
	while((sapPtr = sapSignalIter++) != NULL)
		{
		sapPtr->L2CapEntityConfigUpdated();
		}
	}

TUint16 CL2CAPMux::SigMTU() const
	{
	LOG_FUNC
	return iBoundLinkSignalHandler->SigMTU();
	}

TInt CL2CAPMux::SendEchoRequest(const TDes8* aData, CL2CapSAPSignalHandler& aEchoResponseHandler)
	{
	LOG_FUNC
	return iBoundLinkSignalHandler->ConstructEchoRequest(aData, aEchoResponseHandler);
	}

void CL2CAPMux::EchoResponseReceived(const TDesC8* aData, MEchoResponseHandler& aEchoResponseHandler)
	{
	LOG_FUNC
	// Pass the response to the originator.
	aEchoResponseHandler.EchoResponseReceived(aData);
	}

TInt CL2CAPMux::GetFreeCID(TL2CAPPort& aReturnPort)
	{
	LOG_FUNC
	TBool CIDIsFree;
	TL2CAPPort startCID = iNextCID;
	
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	CL2CapSAPSignalHandler* handler;

	aReturnPort = 0;
	do
		{
		sapSignalIter.SetToFirst();
		CIDIsFree = ETrue;
		while((handler = sapSignalIter++) != NULL)
			{
			if(handler->LocalPort()	== iNextCID)
				{
				CIDIsFree = EFalse;
				break;
				}
			}

		if(CIDIsFree)
			{
			aReturnPort = iNextCID;
			}

		if(iNextCID++ == KL2CapDynamicCIDEnd)
			{
			iNextCID = KL2CapDynamicCIDStart;
			}
		} while(!aReturnPort && startCID != iNextCID);

	return aReturnPort?KErrNone:KErrL2CAPNoFreeCID;
	}

TInt CL2CAPMux::GetACLMTU() const
	{
	return iACLMTU;
	}

CL2CapSAPSignalHandler* CL2CAPMux::GetSignalHandlerWithRemoteCID(TL2CAPPort aRemoteCID)
	{
	LOG_FUNC
	TDblQueIter<CL2CapSAPSignalHandler> sapSignalIter(iSapSignalHandlers);
	sapSignalIter.SetToFirst();
	CL2CapSAPSignalHandler* sapPtr;
	
	while((sapPtr = sapSignalIter++) != NULL)
		{
		if(sapPtr->RemotePort() == aRemoteCID)
			{
			break;
			}
		}	
	// returns CL2CapSAPSignalHandler with matching remote CID or NULL if one not found
	return sapPtr;
	}

// Adds the data controller to the Muxers list in priority order.
// If a Data controller of equal priority in the queue, the new one 
// will be added after it.
void CL2CAPMux::PriorityAddDataController(CL2CapBasicDataController& aDataController)
	{
	LOG_FUNC
	if(iDataHandlers.IsEmpty())
		{
		iDataHandlers.AddFirst(aDataController);
		}
	else
		{
		TUint8 channelPriority = aDataController.Config().ChannelPriority();
		
		TDblQueIter<CL2CapBasicDataController> iter(iDataHandlers);
		CL2CapBasicDataController* handler;

		if(channelPriority > (iter++)->Config().ChannelPriority())
			{
			iDataHandlers.AddFirst(aDataController);
			}
		else
			{
			TBool controllerAdded = EFalse;
			while(!controllerAdded && (handler = iter++) != NULL)
				{
				if(channelPriority > handler->Config().ChannelPriority())
					{
					aDataController.iLink.AddBefore(&handler->iLink);
					controllerAdded = ETrue;
					}
				}
			if(!controllerAdded)
				{
				iDataHandlers.AddLast(aDataController);
				}
			}
		}
	}
	
	
void CL2CAPMux::PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& /*aPhysicalLink*/)
	{
	LOG_FUNC
	
	if (aEvent.EventType() & ENotifySniffMode)
		{
		//Entered SniffMode
		TPckgBuf<TBasebandTime> buf = 0;
		TInt err = iBoundSAP->GetOption(KSolBtLM, EBBGetSniffInterval, buf);
		__ASSERT_DEBUG(!err, Panic(EL2CAPGetOptionFailed));
		if (KErrNone==err)
			{
			iSniffInterval = buf();
			}
		}
		
	if ((aEvent.EventType() & ENotifyActiveMode) && iSniffInterval)
		{
		//Exited SniffMode
		iSniffInterval = 0;
		}
	}
	
TUint8 CL2CAPMux::AdjustRTXTimerForSniffMode(TUint8 aBaseRTXTimerDuration) const
	{
	LOG_FUNC
	TUint8 sniffModeRTXTimerDuration = aBaseRTXTimerDuration;
	
	if (iSniffInterval != 0)
		{		
		// RTX Timer value must be set to at least twice the sniff interval to allow
		// us to send a request in one transmission slot and for the remote device
		// to respond in the next. This assumes that the remote device can turn around the
		// request in one sniff interval but if we have a small interval this may not
		// be the case so we need to add a constant to allow for the remote to process the
		// request.
		TReal sniffModeAdjustment = (iSniffInterval * KBasebandSlotTime * 2) + KRemoteDeviceProcessRequestDuration;
		
		sniffModeRTXTimerDuration = Max(ceil(sniffModeAdjustment),aBaseRTXTimerDuration);
		}
		
	return sniffModeRTXTimerDuration;
	}
