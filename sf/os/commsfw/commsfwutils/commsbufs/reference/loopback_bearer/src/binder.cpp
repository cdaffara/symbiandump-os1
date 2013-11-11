/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <e32std.h>
#include <eui_addr.h>
#include <ip4_hdr.h>
#include <udp_hdr.h>
#include <tcp_hdr.h>
#include <in_chk.h>
#include <in_iface.h>
#include <comms-infras/nifif.h>
#include <in_sock.h>
#include "var.h"
#include "binder.h"
#include "provision.h"

#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbuf.h>
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufpondop.h>
#else
#include <comms-infras/mbufmanager.h>
#endif

using namespace ESock;

#ifdef _DEBUG
_LIT8(KNif,"Legacy");
_LIT8(KBinder4,"Binder4");
_LIT8(KBinder6,"Binder6");
#endif

//
// CLegacyLoopbackBinder4 //
//

CLegacyLoopbackBinder4::CLegacyLoopbackBinder4(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow) : iLegacyLoopbackSubConnectionFlow(aLegacyLoopbackSubConnectionFlow)
	{
    __FLOG_OPEN(KNif, KBinder4);
   	// generate my local ip address (ip4) - vals potentially will be overwritten by any derived classes
	iLocalAddressBase = KLegacyLoopbackLocalAddressBase; // also used later in control method
	
	TUint32 id = ((TUint32)this) % 255;
	// Avoid the reserved address (least significant byte KLegacyLoopbackReservedHostId) that
	// is never to be allocated as a local address.
	if (id == KLegacyLoopbackReservedHostId)
		{
		++id;
		}
	iLocalAddress = iLocalAddressBase + id;
    __FLOG_3(_L8("CLegacyLoopbackBinder4 %08x:\tCLegacyLoopbackBinder4(CLegacyLoopbackSubConnectionFlow& %08x): iLocalAddress %08x"), this, &aLegacyLoopbackSubConnectionFlow, iLocalAddress);
}

CLegacyLoopbackBinder4::~CLegacyLoopbackBinder4()
/** 
Destroys 'this'
*/
    {
    if (iTestSubscriber)
    	{
    	iTestSubscriber->Cancel();
    	}
    delete iTestSubscriber;
    delete iDrvReceiver;
    delete iDrvSender;
    __FLOG_1(_L8("CLegacyLoopbackBinder4 %08x:\t~CLegacyLoopbackBinder4()"), this);
    ASSERT(iUpperControl == NULL);
    ASSERT(iUpperReceiver == NULL);
	ASSERT(!iErrorOneShot.IsActive());
    RDebug::Printf("closing driver\n");
    iDrv.Close();
    __FLOG_CLOSE;
    }

CLegacyLoopbackBinder4* CLegacyLoopbackBinder4::NewL(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow)
	{
	CLegacyLoopbackBinder4* self = new (ELeave) CLegacyLoopbackBinder4(aLegacyLoopbackSubConnectionFlow);
	CleanupStack::PushL(self);
	
	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(
		aLegacyLoopbackSubConnectionFlow.AccessPointConfig().FindExtensionL(
			STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
	TInt ap = providerInfo.APId();
	if(CLegacyLoopbackFlowTestingSubscriber::ShouldRun(ap))
		{
		ASSERT(self->iTestSubscriber==NULL);
		self->iTestSubscriber = CLegacyLoopbackFlowTestingSubscriber::NewL(aLegacyLoopbackSubConnectionFlow, ap);
		}

	// The send and receive paths each have an AO
	self->iDrvReceiver = new(ELeave) CDrvReceiver(CActive::EPriorityHigh);
	self->iDrvSender = new(ELeave) CDrvSender(CActive::EPriorityHigh);

	CleanupStack::Pop(self);
	return self;
	}

void CLegacyLoopbackBinder4::LogPacketDetails(TInet6HeaderIP4* aIp4, const TDesC8& aCaption)
    {
    static_cast<void>(aIp4);
    static_cast<void>(aCaption);

#if defined __FLOG_ACTIVE
    TBuf8<KLogBufferSize> log;
    TInet6HeaderUDP* udp = NULL;
    TInet6HeaderTCP* tcp = NULL;
    TBuf8<10> prot;
    TBuf8<40> length;
    TInt srcPort = -1;
    TInt dstPort = -1;
    if(aIp4->Protocol() == KProtocolInetUdp)
        {
        udp = (TInet6HeaderUDP*) aIp4->EndPtr();
        prot = _L8("UDP");
        length.Format(_L8("payload %d"), udp->Length());
        srcPort = udp->SrcPort();
        dstPort = udp->DstPort();
        }
    else if(aIp4->Protocol() == KProtocolInetTcp)
        {
        tcp = (TInet6HeaderTCP*) aIp4->EndPtr();
        prot = _L8("TCP");
        length.Format(_L8("payload %d"), aIp4->TotalLength() - aIp4->HeaderLength() - tcp->HeaderLength());
        srcPort = tcp->SrcPort();
        dstPort = tcp->DstPort();
        }
    else
        {
        prot.Format(_L8("%d"), aIp4->Protocol());
        length.Format(_L8("tot_len %d"), aIp4->TotalLength());
        }	
    TDes8IgnoreOverflow overflow;
    log.AppendFormat(_L8("%S %S %S src %08x:%d, dst %08x:%d"), &overflow, &aCaption, &prot, &length,
            aIp4->SrcAddr(), srcPort, aIp4->DstAddr(), dstPort);
    __FLOG(log);
#endif    
    }
    
void CLegacyLoopbackBinder4::UpdateHeaders(TInet6HeaderIP4* aIp4, TInet6HeaderUDP* aUdp)
/**
Update the IPv4 and UDP headers to allow the packet to be looped back.
*/
{
	__FLOG_STMT( LogPacketDetails(aIp4, _L8("CLegacyLoopbackBinder4:\tUpdateHeaders(...): ")) );
	
	// swap over the destination and source addresses
	TUint32 temp;
	temp = aIp4->SrcAddr();
	aIp4->SetSrcAddr(aIp4->DstAddr());
	aIp4->SetDstAddr(temp);

   	// swap over the destination and source ports
	if (aUdp)
		{
		TUint tempPort;
		tempPort = aUdp->DstPort();
		aUdp->SetDstPort(aUdp->SrcPort());
		aUdp->SetSrcPort(tempPort);
		}

	// NB: don't need to recalculate any checksums since luckily IP sums don't detect transposition
	__FLOG_4(_L("CLegacyLoopbackBinder4:\tUpdateHeaders(...): became src %08x:%d, dst %08x:%d"), 
			aIp4->SrcAddr(), aUdp? (TInt)aUdp->SrcPort(): -1, (TInt)aIp4->DstAddr(), aUdp? (TInt)aUdp->DstPort(): -1);
}

MLowerDataSender::TSendResult CLegacyLoopbackBinder4::Send(RMBufChain& aData)
/**
Entry point for receiving IPv4 outgoing data

@param aData MBuf chain containing data to send
@return an indication of whether the upper layer should block.
*/
	{
    RMBufPacket& pkt = static_cast<RMBufPacket&>(aData);

    pkt.Unpack();
	TInet6HeaderIP4* ip4 = (TInet6HeaderIP4*) pkt.First()->Ptr();
    __FLOG_STMT( LogPacketDetails(ip4, _L8("CLegacyLoopbackBinder4:\tSend(...): ")) );

	if (ip4->Protocol() == KProtocolInetUdp || ip4->Protocol() == KProtocolInetTcp)
		{
		// get the "udp" header as well - cheating & relying upon UDP & TCP being in same place
		TInet6HeaderUDP* udp = (TInet6HeaderUDP*) ip4->EndPtr();
	
		// use the source or destination port number to decide whether or not to pass it through the loopback driver
		// very weak this; dynamic ports could overlap. Really should do this for all packets and
		// fix any impacted test code
		TUint srcPort = udp->SrcPort();
		TUint dstPort = udp->DstPort();

		if(dstPort >= KDriverReflectionRangeStartPort && dstPort <= KDriverReflectionRangeEndPort ||
			srcPort >= KDriverReflectionRangeStartPort && srcPort <= KDriverReflectionRangeEndPort)
			{
			if(!iLoopbackLoadAttempted)
				{
				iLoopbackLoadAttempted = ETrue;
				TInt r = User::LoadPhysicalDevice(KLegacyLoopbackPddFileName);
				RDebug::Printf("Loading PDD - %d\n", r);
				if(r == KErrNone || r == KErrAlreadyExists)
					{
					r = User::LoadLogicalDevice(KLegacyLoopbackLddFileName);
					RDebug::Printf("Loading LDD - %d\n", r);
					if(r == KErrNone || r == KErrAlreadyExists)
						{
						r = iDrv.Open();
						RDebug::Printf("Opening driver - %d\n", r);
						if(r == KErrNone)
							{
							// Prime the pathways
							iDrvReceiver->Start(this, iDrv);
							iDrvSender->Start(this, iDrv);
							}
						}
					}
				}
			if(iDrv.Handle())
				{
				// Swap the ports, because the receipt path also unconditionally swaps them (probably
				// some test needs this 
				// ugh! should use a queue here when the write is still busy
				udp->SetDstPort(srcPort);
				udp->SetSrcPort(dstPort);
				
				__FLOG_4(_L("CLegacyLoopbackBinder4:\tSend(...): passing packet to driver as src %08x:%d, dst %08x:%d"),  
						ip4->SrcAddr(), dstPort, ip4->DstAddr(), srcPort);

				iDrvSender->QueueForSend(pkt);
				return ESendAccepted;
				}
			}
		else if(dstPort >= KBearerReflectionRangeStartPort && dstPort <= KBearerReflectionRangeEndPort ||
					srcPort >= KBearerReflectionRangeStartPort && srcPort <= KBearerReflectionRangeEndPort)
			{
			// Swap the ports, because the receipt path also unconditionally swaps them (probably
			// some test needs this 
			udp->SetDstPort(srcPort);
			udp->SetSrcPort(dstPort);
			}
		}

	// Loop the data straight back into the TCP/IP stack
	pkt.Pack();
	ProcessPacket(aData);
	return ESendAccepted;
	}
	

MLowerDataSender* CLegacyLoopbackBinder4::Bind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
/**
Return the MLowerDataSender instance (CLegacyLoopbackBinder4) that we
previously allocated.
*/
	{
    __FLOG_3(_L8("CLegacyLoopbackBinder4 %08x:\tBind(aUpperReceiver %08x, aUpperControl %08x)"), this, &aUpperReceiver, &aUpperControl);
	iUpperReceiver = &aUpperReceiver;
	iUpperControl = &aUpperControl;

	// Signal upper layer that we are ready
	BinderReady();

	return this;
	}
	
void CLegacyLoopbackBinder4::Unbind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
/**
Forget our association with upper layer.
*/
	{
    static_cast<void>(aUpperReceiver);
    static_cast<void>(aUpperControl);
    __FLOG_1(_L8("CLegacyLoopbackBinder4 %08x:\tUnbind()"), this);
    ASSERT(&aUpperReceiver == iUpperReceiver);
    ASSERT(&aUpperControl == iUpperControl);
    iUpperReceiver = NULL;
    iUpperControl = NULL;
	}
	
void CLegacyLoopbackBinder4::BinderReady()
/**
Signal upper layer that we are ready
*/
	{
    __FLOG_1(_L8("CLegacyLoopbackBinder4 %08x:\tBinderReady()"), this);

	iUpperControl->StartSending();
	}

void CLegacyLoopbackBinder4::ProcessPacket(RMBufChain& aPdu)
/**
Process incoming data
*/
    {
	// this received data has already been looped back...
	// get the ip header from the RMBufChain
	TInet6HeaderIP4* ip4 = (TInet6HeaderIP4*) aPdu.First()->Next()->Ptr();

	if (ip4->Protocol() != KProtocolInetUdp && ip4->Protocol() != KProtocolInetTcp)
		{
		//Non UDP traffic goes here!
		__FLOG_3(_L("CLegacyLoopbackBinder4 %08x:\tProcessPacket(): IPv4 length %d, protocol %d"), this, ip4->TotalLength(), ip4->Protocol());

		UpdateHeaders(ip4, NULL);
		// now process it (pass up the stack)
		iUpperReceiver->Process(aPdu);
		return;
		}

	// get the udp header as well - assume only udp traffic here
    __FLOG_STMT( LogPacketDetails(ip4, _L8("CLegacyLoopbackBinder4:\tProcessPacket(...): ")) );

	TInet6HeaderUDP* udp = (TInet6HeaderUDP*) ip4->EndPtr();

	// depending on the contents, pass it on up thru the stack 
	// or maybe do something else

	// use the destination port number to decide whether or not the payload is a command
	TUint dstPort = udp->DstPort();
	if (KLegacyLoopbackCmdPort == dstPort)
    	{
		// let's use the first payload byte as the command byte
		switch (*(udp->EndPtr()))
    		{
		    case KForceDisconnect:
			    __FLOG(_L("KForceDisconnect command"));
			    // do some action
	            Flow()->Progress(KLinkLayerClosed, KErrCommsLineFail);

	            Flow()->FlowDown(KErrCommsLineFail, MNifIfNotify::EDisconnect);

			    // no return code so all we can do is respond with what we got
			    UpdateHeaders(ip4, udp);
			    iUpperReceiver->Process(aPdu);
			    break;

		    case KForceReconnect:
			    __FLOG(_L("KForceReconnect command"));
			    // do some action
	            Flow()->Progress(KLinkLayerClosed, KErrCommsLineFail);
	            
	            Flow()->FlowDown(KErrCommsLineFail, MNifIfNotify::EReconnect);

			    // no return code so all we can do is respond with what we got
			    UpdateHeaders(ip4, udp);
			    iUpperReceiver->Process(aPdu);
			    break;

		    case KSendNotification:
			    __FLOG(_L("KSendNotification command"));
			    //let's write the result in the next byte of the reply
			    if (Flow()->AgentProvision()->IsDialIn() == KErrNotSupported)
				    udp->EndPtr()[1] = (unsigned char) KErrNone;
			    else
				    udp->EndPtr()[1] = (unsigned char) KErrGeneral; // this will lose it's sign :-(
			
			    UpdateHeaders(ip4, udp);
                iUpperReceiver->Process(aPdu);
			    break;
			    
			case KForceFinishedSelection:
				__FLOG(_L("KForceFinishedSelection command"));
				// force subConn into KFinishedSelection State
	            Flow()->Progress(KFinishedSelection, KErrNone);
				UpdateHeaders(ip4, udp);
				aPdu.Free();
				break;

    		case KForceBinderError:
			    __FLOG(_L("KForceBinderError command"));
				// We cannot signal an error whilst in the middle of a send in the TCP/IP stack,
				// as the act of signalling the error will eventually result in the CNifIfBase binder
				// being destructed by the TCP/IP stack whilst we're in the middle of using it.
				// Consequently, we would panic on exit from this routine.  So make the call
				// via an asynchronous callback.
				if (!iErrorOneShot.IsActive())
					{
					iErrorOneShot.Schedule(iUpperControl);
					}
    			aPdu.Free();
    			break;

    		case KColourDataByLinkTierAccessPointId:
				{
				const TProviderInfoExt* providerInfo = static_cast<const TProviderInfoExt*>(
					iLegacyLoopbackSubConnectionFlow.AccessPointConfig().FindExtension(
						STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)));
				ASSERT(providerInfo); // Should always be present

				// We are going to simply add the access point id to the command byte
				// A test client can then validate that the socket is connected on the expected access point
			    __FLOG(_L("KColourDataByAccessPointId command"));
				*(udp->EndPtr()) += static_cast<TUint8>(providerInfo->iProviderInfo.APId());

				// Update the udp headers and forward on
				UpdateHeaders(ip4, udp);
				iUpperReceiver->Process(aPdu);
				}
				break;

		    default:
			    __FLOG(_L("Unknown command - ignoring it"));
			    aPdu.Free();
			    // unknown command, just ignore this packet???
		    }
	    }
	else
	    {
        __FLOG(_L("Standard echo packet"));
        if (iTestSubscriber && !iTestSubscriber->IsEnabled())
        	{
        	__FLOG(_L("Bearer not available. Packet dropped."));
        	aPdu.Free();
        	return;
        	}

        // just echo the packet back to the original sender
		// update the headers (addresses, checksums etc)
		UpdateHeaders(ip4, udp);
		// now process it (pass up the stack)
		iUpperReceiver->Process(aPdu);
		}
    }

TInt CLegacyLoopbackBinder4::GetConfig(TBinderConfig& aConfig)
/**
Return IPv4 configuration information.

Called from upper layer.

@param aConfig structure to populate with IPv4 configuration
*/
	{
    TBinderConfig4* config = TBinderConfig::Cast<TBinderConfig4>(aConfig);
    
	if(config == NULL)
		{
		return KErrNotSupported;
		}

	// Setup config
	config->iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast;
	config->iInfo.iMtu = KLoopbackBearerMTU;
	config->iInfo.iRMtu = KLoopbackBearerMTU;
	config->iInfo.iSpeedMetric = 0;

	config->iFamily = KAfInet6;

    TUint32 address;
	const TInt KPort = 65;

    config->iFamily = KAfInet;

	__FLOG_2(_L8("CLegacyLoopbackBinder4 %08x:\tGetConfig(): iLocalAddress %08x"), this, iLocalAddress);        
	
	config->iAddress.SetAddress(iLocalAddress);
	config->iAddress.SetPort(KPort);

	// network mask
	config->iNetMask.SetAddress(KInetAddrNetMaskC);	// 255.255.255.0
	config->iNetMask.SetPort(KPort);

	// broadcast address
	address = iLocalAddressBase + KBroadcastAddressSuffix;
	config->iBrdAddr.SetAddress(address);
	config->iBrdAddr.SetPort(KPort);

	// default gateway
	address = iLocalAddressBase + KDefaultGatewayAddressSuffix;
	config->iDefGate.SetAddress(address);
	config->iDefGate.SetPort(KPort);

	// primary DNS, just make same as default gateway
	config->iNameSer1.SetAddress(address);
	config->iNameSer1.SetPort(KPort);

	// secondary DNS
	address = iLocalAddressBase + KSecondaryDnsAddressSuffix;
	config->iNameSer2.SetAddress(address);
	config->iNameSer2.SetPort(KPort);
	
	return KErrNone;
	}

TInt CLegacyLoopbackBinder4::Control(TUint aLevel, TUint aName, TDes8& aOption)
/**
Called from upper layer for special control functionality.
*/
	{	
	(void) aLevel;
	(void) aName;
 	(void) aOption;
	__FLOG_3(_L("CLegacyLoopbackBinder4 %08x:\tControl(aLevel %x, aName %x)"), this, aLevel, aName);
	return KErrNotSupported;
	}

TInt CLegacyLoopbackBinder4::GetName(TDes& aName)
/**
Called from upper layer to retrieve the binder name.

@param aName populated with name
@return KErrNone on success, else a system wide error code.
*/
	{
	// This name matches the NIF-based DummyNif to match any potential
	// test code expectations on the name.
	aName.Format(_L("legacy_loopback[0x%08x]"), this);

	__FLOG_2(_L("CLegacyLoopbackBinder4 %08x:\tGetName(): %S"), this, &aName);

	return KErrNone;
	}

TInt CLegacyLoopbackBinder4::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
    __FLOG_1(_L8("CLegacyLoopbackBinder4 %08x:\tBlockFlow()"), this);

	return KErrNotSupported;
	}

TBool CLegacyLoopbackBinder4::MatchesUpperControl(ESock::MUpperControl* aUpperControl) const
/**
Utility function that returns whether this binder is associated with the
MUpperControl object passed as argument.

@param aUpperControl upper layer to match against
@return ETrue on a match else EFalse.
*/
	{
	return aUpperControl == iUpperControl;
	}


CLegacyLoopbackBinder4::CDrvSender::CDrvSender(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
    __FLOG_OPEN(KNif, KBinder4);
	}

CLegacyLoopbackBinder4::CDrvSender::~CDrvSender()
	{
	Cancel();
	do
		{
		iSendPkt.Free();
		}
	while(iSendQueue.Remove(iSendPkt));
    __FLOG_CLOSE;
	}

void CLegacyLoopbackBinder4::CDrvSender::Start(CLegacyLoopbackBinder4* aBinder, RLegacyLoopbackDriver aDrv)
	{
	iBinder = aBinder;
	iDrv = aDrv;
	}

void CLegacyLoopbackBinder4::CDrvSender::RunL()
	{
	// Finished with the buffer just sent
	iSendPkt.Free();

	// Send the next packet waiting
	SendPacket();
	}

void CLegacyLoopbackBinder4::CDrvSender::DoCancel()
	{
	iDrv.SendDataCancel();
	}

void CLegacyLoopbackBinder4::CDrvSender::SendPacket()
	{
	// Fetch packet to send
	TBool havePktToSend = iSendQueue.Remove(iSendPkt);
	
	if(havePktToSend)
		{
		RMBuf* sendBuffer = NULL;

		// If the packet is composed of more than one buffer then we log this and abandon the packet
		if(iSendPkt.NumBufs() > 1)
			{
			__FLOG_VA((_L8("Warning: packet %d bytes %d buffers"), iSendPkt.Length(), iSendPkt.NumBufs() ));
			iManyBufCount++;

			// Copy out to a single buffer
			sendBuffer = RMBuf::Alloc(KLoopbackBearerMTU, iBinder->iAllocator);
			if(sendBuffer)
				{
				sendBuffer->SetLength(0);
				RMBuf* currentBuf = iSendPkt.First();
				while(currentBuf)
					{
					TPtr8 srcData(currentBuf->Ptr(), currentBuf->Length(), currentBuf->Length());
					sendBuffer->Append(srcData);
					currentBuf = currentBuf->Next();
					}
				}
				iSendPkt.Free();
				RMBufChain sendChain(sendBuffer);
				iSendPkt.Assign(sendChain);
			}
		else
			{
			iOneBufCount++;
			sendBuffer = iSendPkt.First();
			}

		// Send the buffer
		if(sendBuffer)
			{
			TInt length(sendBuffer->Length());
			TPtr8 txPtr(sendBuffer->Ptr(), length, length);
			iDrv.SendData(iStatus, txPtr);
			SetActive();
			}
		}
	}

void CLegacyLoopbackBinder4::CDrvSender::QueueForSend(RMBufPacket& aPkt)
	{
	aPkt.FreeInfo();
	iSendQueue.Append(aPkt);
	if(!IsActive())
		{
		SendPacket();
		}
	}
	
CLegacyLoopbackBinder4::CDrvReceiver::CDrvReceiver(TInt aPriority)
: CActive(aPriority),
  iRxPtr(NULL, 0)
	{
	CActiveScheduler::Add(this);
	iPond = TCommsBufPondTLSOp::Get();
	}

CLegacyLoopbackBinder4::CDrvReceiver::~CDrvReceiver()
	{
	Cancel();
	}

void CLegacyLoopbackBinder4::CDrvReceiver::Start(CLegacyLoopbackBinder4* aBinder, RLegacyLoopbackDriver aDrv)
	{
	iBinder = aBinder;
	iDrv = aDrv;
	RequestReceipt();
	}

void CLegacyLoopbackBinder4::CDrvReceiver::RequestReceipt()
	{
	ASSERT(iPkt.IsEmpty());
	RMBuf* buf = RMBuf::Alloc(RLegacyLoopbackDriver::KLoopbackMTU);
	if(buf)
		{
		RMBufQ q(buf);
		TRAPD(ret, iPkt.CreateL(q));	// need non-leaving overload!
		if(ret == KErrNone)
			{
			iPkt.Pack();
			iRxPtr.Set(buf->Ptr(), 0, buf->Size());
			}
		else
			{
			// In a real version we could still strive to use the buf, even without the header
			// But someday we'll be free of having the packet info as the magic extra buffer
			// on the front & instead pass a concrete type in tandem: type safety & an end to
			// all this pushing & popping
			buf->Free();
			buf = NULL;
			}
		}
	if(!buf)
		{
		// No pre-built packet; copy through descriptor
		iRxPtr.Set(iDesBuf.MidTPtr(0));
		}

	// Request the next received packet from the driver
	iDrv.ReceiveData(iStatus, iRxPtr);
	SetActive();
	}

void CLegacyLoopbackBinder4::CDrvReceiver::RunL()
	{
	TInt ret;
	if(iPkt.IsEmpty())
		{
		TRAP(ret, iPkt.CreateL(iDesBuf));	// need non-leaving overload!
		if(ret == KErrNone)
			{
			iPkt.Pack();
			}
		}
	else
		{
		ret = KErrNone;
		}
	if(ret == KErrNone)
		{
		iBinder->ProcessPacket(iPkt);
		ASSERT(iPkt.IsEmpty());
		}
	RequestReceipt();
	}

void CLegacyLoopbackBinder4::CDrvReceiver::DoCancel()
	{
	iDrv.ReceiveDataCancel();
	}


// =================================================================================
//
// CLegacyLoopbackBinder6

CLegacyLoopbackBinder6::CLegacyLoopbackBinder6(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow) : iLegacyLoopbackSubConnectionFlow(aLegacyLoopbackSubConnectionFlow)
	{
    __FLOG_OPEN(KNif, KBinder6);
    __FLOG_2(_L8("CLegacyLoopbackBinder6 %08x:\tCLegacyLoopbackBinder6(CLegacyLoopbackSubConnectionFlow& %08x)"), this, &aLegacyLoopbackSubConnectionFlow);
	}

CLegacyLoopbackBinder6::~CLegacyLoopbackBinder6()
/** 
Destroys 'this'
*/
    {
    __FLOG(_L8("CLegacyLoopbackBinder6:\t~CLegacyLoopbackBinder6()"));
    __FLOG_CLOSE;
    }

CLegacyLoopbackBinder6* CLegacyLoopbackBinder6::NewL(CLegacyLoopbackSubConnectionFlow& aLegacyLoopbackSubConnectionFlow)
	{
	return new (ELeave) CLegacyLoopbackBinder6(aLegacyLoopbackSubConnectionFlow);
	}

void CLegacyLoopbackBinder6::UpdateHeaders(TInet6HeaderIP* aIp6, TInet6HeaderUDP* /*aUdp*/)
    {
	// swap over the destination and source addresses
	TIp6Addr temp;
	temp = aIp6->SrcAddr();
	aIp6->SetSrcAddr(aIp6->DstAddr());
	aIp6->SetDstAddr(temp);
    }
    
MLowerDataSender::TSendResult CLegacyLoopbackBinder6::Send(RMBufChain& aData)
/**
Send IPv6 data

Note: not clear that this is properly supported or used.

@param aData data to send
*/
	{
    __FLOG(_L8("CLegacyLoopbackBinder6:\tSend()"));

   	// Loop the data straight back into the TCP/IP stack
	ProcessPacket(aData);
	return ESendAccepted;
	}
	
MLowerDataSender* CLegacyLoopbackBinder6::Bind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
	{
    __FLOG_2(_L8("CLegacyLoopbackBinder6:\tBind(MUpperDataReceiver %08x, MUpperControl %08x)"), &aUpperReceiver, &aUpperControl);

	iUpperReceiver = &aUpperReceiver;
	iUpperControl = &aUpperControl;
	
	// Signal upper layer that we are ready
	BinderReady();
	
	return this;
	}

void CLegacyLoopbackBinder6::BinderReady()
/**
Signal to upper layer that we are ready
*/
    {
    __FLOG(_L8("CLegacyLoopbackBinder6:\tBinderReady()"));

	iUpperControl->StartSending();
    }

void CLegacyLoopbackBinder6::ProcessPacket(RMBufChain& aPdu)
/**
Process incoming IPv6 packets.

Note: not clear that this is properly supported or used.

@param aPdu incoming data packet
*/
    {
    __FLOG(_L8("CLegacyLoopbackBinder6:\tProcessPacket()"));

	// this received data has already been looped back...
	// get the ip header from the RMBufChain
	TInet6HeaderIP* ip6 = (TInet6HeaderIP*) aPdu.First()->Next()->Ptr();
	TInet6HeaderUDP* udp = NULL;

	if ((TUint)ip6->NextHeader() == KProtocolInetUdp)
		{
		// get the udp header as well - assume only udp traffic here
		udp = (TInet6HeaderUDP*) ip6->EndPtr();

		__FLOG_3(_L("CLegacyLoopbackBinder6:\tProcessPacket(...): UDP length %d, src port %d, dst port %d"),
			udp->Length(), udp->SrcPort(), udp->DstPort());

		// depending on the contents, pass it on up thru the stack 
		// or maybe do something else

		// use the destination port number to decide whether or not the payload is a command
		TUint dstPort = udp->DstPort();
		if (KLegacyLoopbackCmdPort == dstPort)
            {
 			// let's use the first payload byte as the command byte
			switch (*(udp->EndPtr()))
				{
			case KForceDisconnect:
				__FLOG(_L("KForceDisconnect command"));
				// do some action
	            Flow()->Progress(KLinkLayerClosed, KErrCommsLineFail);

	            Flow()->FlowDown(KErrCommsLineFail, MNifIfNotify::EDisconnect);

				// no return code so all we can do is respond with what we got
				UpdateHeaders(ip6, udp);
				iUpperReceiver->Process(aPdu);
				break;

			case KForceReconnect:
				__FLOG(_L("KForceReconnect command"));
				// do some action
	            Flow()->Progress(KLinkLayerClosed, KErrCommsLineFail);

                //cause.iReserved=MNifIfNotify::EReconnect;
	            Flow()->FlowDown(KErrCommsLineFail);

				// no return code so all we can do is respond with what we got
				UpdateHeaders(ip6, udp);
				iUpperReceiver->Process(aPdu);
				break;

			case KSendNotification:
				__FLOG(_L("KSendNotification command"));
			    //let's write the result in the next byte of the reply
			    if (Flow()->AgentProvision()->IsDialIn() == KErrNotSupported)
				    udp->EndPtr()[1] = (unsigned char) KErrNone;
			    else
				    udp->EndPtr()[1] = (unsigned char) KErrGeneral; // this will lose it's sign :-(
			
				UpdateHeaders(ip6, udp);
                iUpperReceiver->Process(aPdu);
			    break;


			default:
				__FLOG(_L("Unknown command - ignoring it"));
				break;
				// unknown command, just ignore this packet???
				}
			return;
			}

		}
	else
		{
		__FLOG_2(_L("CLegacyLoopbackBinder6:\tProcessPacket(...): IPv6 length %d, next header %d"),
			ip6->PayloadLength(), ip6->NextHeader());
		}

	// just echo the packet back to the original sender

	// update the headers (addresses, checksums etc).  If "udp" is non-NULL, then
	// the UDP ports will be updated as well.
	UpdateHeaders(ip6, udp);
	// now process it (pass up the stack)
	iUpperReceiver->Process(aPdu);		
    }

void CLegacyLoopbackBinder6::Unbind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
	{
    static_cast<void>(aUpperReceiver);
    static_cast<void>(aUpperControl);
    __FLOG(_L8("CLegacyLoopbackBinder6:\tUnbind()"));

    ASSERT(&aUpperReceiver == iUpperReceiver);
    ASSERT(&aUpperControl == iUpperControl);
    iUpperReceiver = NULL;
    iUpperControl = NULL;
	}
	
TInt CLegacyLoopbackBinder6::GetName(TDes& aName)
/**
Called from upper layer to retrieve the binder name.

@param aName populated with name
@return KErrNone on success, else a system wide error code.
*/
	{
    __FLOG(_L8("CLegacyLoopbackBinder6:\tGetName()"));

	// This name matches the NIF-based DummyNif to match any potential
	// test code expectations on the name.
	aName.Format(_L("legacy_loopback6[0x%08x]"), this);
	
	return KErrNone;
	}

TInt CLegacyLoopbackBinder6::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
    __FLOG(_L8("CLegacyLoopbackBinder6:\tBlockFlow()"));

	return KErrNotSupported;
	}

void CLegacyLoopbackBinder6::StaticDnsConfiguration(TBinderConfig6& aConfig)
	{
    __FLOG(_L8("CLegacyLoopbackBinder6:\tStaticDnsConfiguration()"));

	const TLegacyLoopbackIp6Provision* ip6Provision = Flow()->Ip6Provision();

	if (!ip6Provision->Ip6DNSAddrFromServer()) 
		{
        aConfig.iNameSer1.SetAddress(ip6Provision->Ip6NameServer1());
        aConfig.iNameSer2.SetAddress(ip6Provision->Ip6NameServer2());
		}
	else
		{
		// Ensure that static DNS addresses are set as unspecified,
		// so they are not used in Control(KSoIfConfig).
        aConfig.iNameSer1.SetAddress(KInet6AddrNone);
        aConfig.iNameSer2.SetAddress(KInet6AddrNone);
		}
	}

TInt CLegacyLoopbackBinder6::GetConfig(TBinderConfig& aConfig)
/**
Return IPv6 configuration information.

Called from upper layer.

@param aConfig structure to populate with IPv6 configuration
*/
	{
    TBinderConfig6* config = TBinderConfig::Cast<TBinderConfig6>(aConfig);
    
	if(config == NULL)
		{
		return KErrNotSupported;
		}
		
	// Setup config
	config->iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast;;
	config->iInfo.iMtu = KLoopbackBearerMTU;
	config->iInfo.iRMtu = KLoopbackBearerMTU;
	config->iInfo.iSpeedMetric = 0;
	
	// Setup addresses 
	
	config->iFamily = KAfInet6;
	
	// Local ID
	
	TInt addr64 = 0x80;
	TE64Addr localAddr(addr64);
	TEui64Addr* id = (TEui64Addr*) &config->iLocalId;
	
	id->Init();
	id->SetAddress(localAddr);
	
	// Remote ID
	addr64 = 0x81;
	TE64Addr remoteAddr(addr64);
	id = (TEui64Addr*) &config->iRemoteId;
	
	id->Init();
	id->SetAddress(remoteAddr);
	
	// Setup static DNS address if required
	StaticDnsConfiguration(*config);
	    
    return KErrNone;
    }

TInt CLegacyLoopbackBinder6::Control(TUint aLevel, TUint aName, TDes8& aOption)
	{
	(void) aLevel;
	(void) aName;
	(void) aOption;
	__FLOG_2(_L("CLegacyLoopbackBinder6:\tControl(aLevel %x, aName %x, ...)"), aLevel, aName);
	return KErrNotSupported;
	}

//
// Utilities
//

TBool CLegacyLoopbackBinder6::MatchesUpperControl(ESock::MUpperControl* aUpperControl) const
/**
Utility function that returns whether this binder is associated with the
MUpperControl object passed as argument.

@param aUpperControl upper layer to match against
@return ETrue on a match else EFalse.
*/
	{
	return aUpperControl == iUpperControl;
	}

//
// Async error callback
//
// Used to schedule asynchronous signalling of a binder error to upper flow in circumstances
// where a direct call is not possible (e.g. in the middle of a TCP/IP send as the binder
// may disappear underneath the TCP/IP stack).

CLegacyLoopbackErrorOneShot::CLegacyLoopbackErrorOneShot()
  : CAsyncOneShot(EPriorityStandard)
	{
	}

void CLegacyLoopbackErrorOneShot::Schedule(MUpperControl* aUpperControl)
	{
	iUpperControl = aUpperControl;
	Call();
	}

void CLegacyLoopbackErrorOneShot::RunL()
	{
	iUpperControl->Error(KErrCommsLineFail);
	}


CLegacyLoopbackFlowTestingSubscriber::CLegacyLoopbackFlowTestingSubscriber(CLegacyLoopbackSubConnectionFlow& aFlow, TUint aApId)
	: CActive(0),
	iFlow(aFlow),
	iApId(aApId)
	{
	}

/*static*/ TBool CLegacyLoopbackFlowTestingSubscriber::ShouldRun(TUint aApId)
	{
	RProperty property;
	TInt result = property.Attach(KLegacyLoopbackTestingPubSubUid, aApId);
	if(result == KErrNone)
		{
		TInt propertyValue;
		result = property.Get(propertyValue);
		if(result == KErrNone)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CLegacyLoopbackFlowTestingSubscriber::ConstructL()
	{
	CActiveScheduler::Add(this);
//	__DECLARE_NAME(_S("CAvailabilityTestingSubscriber"));

	TInt result = iProperty.Attach(KLegacyLoopbackTestingPubSubUid, iApId);
	ASSERT(result == KErrNone);

	RunL();
	}

void CLegacyLoopbackFlowTestingSubscriber::RunL()
	{
	// .. and repeat..
	iProperty.Subscribe(iStatus);
	
	TInt publishedValue;
	TInt result = iProperty.Get(publishedValue);
	ASSERT(result == KErrNone);

	TAvailabilityStatus av(publishedValue);
	if (av.Score())
		{
		iIsEnabled = ETrue;
		iFlow.iDisableStart = EFalse;
		}
	else
		{
		iIsEnabled = EFalse;
		iFlow.iDisableStart = ETrue;
		}

	SetActive();
	}

void CLegacyLoopbackFlowTestingSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}

/*virtual*/ CLegacyLoopbackFlowTestingSubscriber::~CLegacyLoopbackFlowTestingSubscriber()
	{
	this->Cancel(); // object must be stoppable by descruction due to cleanup restrictions
	iProperty.Close();
	}



