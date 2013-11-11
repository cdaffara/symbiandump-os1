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
_LIT8(KNif,"ZeroCopy");
_LIT8(KBinder4,"Binder4");
_LIT8(KBinder6,"Binder6");
_LIT8(KBinderSender4,"Sender4");
#endif
//
// CZeroCopyLoopbackBinder4 //
//

CZeroCopyLoopbackBinder4::CZeroCopyLoopbackBinder4(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow) : iZeroCopyLoopbackSubConnectionFlow(aZeroCopyLoopbackSubConnectionFlow)
	{
    __FLOG_OPEN(KNif, KBinder4);
   	// generate my local ip address (ip4) - vals potentially will be overwritten by any derived classes
	iLocalAddressBase = KZeroCopyLoopbackLocalAddressBase; // also used later in control method
	
	TUint32 id = ((TUint32)this) % 255;
	// Avoid the reserved address (least significant byte KZeroCopyLoopbackReservedHostId) that
	// is never to be allocated as a local address.
	if (id == KZeroCopyLoopbackReservedHostId)
		{
		++id;
		}
	iLocalAddress = iLocalAddressBase + id;
    __FLOG_3(_L8("CZeroCopyLoopbackBinder4 %08x:\tCZeroCopyLoopbackBinder4(CZeroCopyLoopbackSubConnectionFlow& %08x): iLocalAddress %08x"), this, &aZeroCopyLoopbackSubConnectionFlow, iLocalAddress);
}

CZeroCopyLoopbackBinder4::~CZeroCopyLoopbackBinder4()
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
    __FLOG_1(_L8("CZeroCopyLoopbackBinder4 %08x:\t~CZeroCopyLoopbackBinder4()"), this);
    ASSERT(iUpperControl == NULL);
    ASSERT(iUpperReceiver == NULL);
	ASSERT(!iErrorOneShot.IsActive());

	// Tell the driver to unload the pond (actually this ought to be done on close anyway)
	TInt r = iDrv.UnloadPond();
	if(r != KErrNone)
		{
#ifdef _DEBUG
		__FLOG_3(_L8("Unload of driver pond returned error:%d, loc:%s(%d)"), r, __FILE__, __LINE__);
#endif
		}

    // Close our connection with the driver
    iDrv.Close();

    // Have the driver unloaded
	r = User::FreeLogicalDevice(RZeroCopyLoopbackDriver::Name());
	if(r != KErrNone)
		{
		__FLOG_3(_L8("Unload of zc ldd driver returned error:%d, loc:%s(%d)"), r, __FILE__, __LINE__);
		}
	r = User::FreePhysicalDevice(KZeroCopyLoopbackPddFileName);
	if(r != KErrNone)
		{
		__FLOG_3(_L8("Unload of zc pdd driver returned error:%d, loc:%s(%d)"), r, __FILE__, __LINE__);
		}
    
    __FLOG_CLOSE;
    }

CZeroCopyLoopbackBinder4* CZeroCopyLoopbackBinder4::NewL(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow)
	{
	CZeroCopyLoopbackBinder4* self = new (ELeave) CZeroCopyLoopbackBinder4(aZeroCopyLoopbackSubConnectionFlow);
	CleanupStack::PushL(self);
	
	const TProviderInfo& providerInfo = static_cast<const TProviderInfoExt&>(
		aZeroCopyLoopbackSubConnectionFlow.AccessPointConfig().FindExtensionL(
			STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
	TInt ap = providerInfo.APId();
	if(CZeroCopyLoopbackFlowTestingSubscriber::ShouldRun(ap))
		{
		ASSERT(self->iTestSubscriber==NULL);
		self->iTestSubscriber = CZeroCopyLoopbackFlowTestingSubscriber::NewL(aZeroCopyLoopbackSubConnectionFlow, ap);
		}

	// The send and receive paths each have an AO
	self->iDrvReceiver = new(ELeave) CDrvReceiver(CActive::EPriorityHigh);
	self->iDrvSender = new(ELeave) CDrvSender(CActive::EPriorityHigh);

	CleanupStack::Pop(self);
	return self;
	}

void CZeroCopyLoopbackBinder4::UpdateHeaders(TInet6HeaderIP4* aIp4, TInet6HeaderUDP* aUdp)
/**
Update the IPv4 and UDP headers to allow the packet to be looped back.
*/
{
	__FLOG_5(_L("CZeroCopyLoopbackBinder4:\tUpdateHeaders(...): xxP length %d, src %08x:%d, dst %08x:%d"), aUdp? (TInt)aUdp->Length(): -1, 
			aIp4->SrcAddr(), aUdp? (TInt)aUdp->SrcPort(): -1, aIp4->DstAddr(), aUdp? (TInt)aUdp->DstPort(): -1);

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

	// we've changed the ip hdr so need to recalculate the ip hdr checksum
	//aIp4->SetChecksum(0); 
	//aIp4->SetChecksum(TChecksum::ComplementedFold(TChecksum::Calculate((TUint16*)aIp4, aIp4->HeaderLength())));

	// also want to set the udp checksum to zero cos it will be wrong now that we have 
	// changed the ip hdr - just set to zero and it is ignored
	if (aUdp)
		{
		//aUdp->SetChecksum(0);
		}
	__FLOG_4(_L("CZeroCopyLoopbackBinder4:\tUpdateHeaders(...): became src %08x:%d, dst %08x:%d"), 
			aIp4->SrcAddr(), aUdp? (TInt)aUdp->SrcPort(): -1, aIp4->DstAddr(), aUdp? (TInt)aUdp->DstPort(): -1);
}

MLowerDataSender::TSendResult CZeroCopyLoopbackBinder4::Send(RMBufChain& aData)
/**
Entry point for receiving IPv4 outgoing data

@param aData MBuf chain containing data to send
@return an indication of whether the upper layer should block.
*/
	{
    RMBufPacket& pkt = static_cast<RMBufPacket&>(aData);

	{
	TInet6HeaderIP4* ip4 = (TInet6HeaderIP4*) aData.First()->Next()->Ptr();
	//Non UDP traffic goes here!
	__FLOG_3(_L("CZeroCopyLoopbackBinder4 %08x:\tProcessPacket(): IPv4 length %d, protocol %d"), this, ip4->TotalLength(), ip4->Protocol());
	}

    pkt.Unpack();
	TInet6HeaderIP4* ip4 = (TInet6HeaderIP4*) pkt.First()->Ptr();
	if (ip4->Protocol() == KProtocolInetUdp || ip4->Protocol() == KProtocolInetTcp)
		{
		// get the "udp" header as well - cheating & relying upon UDP & TCP being in same place
		TInet6HeaderUDP* udp = (TInet6HeaderUDP*) ip4->EndPtr();
		__FLOG_5(_L("CZeroCopyLoopbackBinder4:\tSend(...): xxP length %d, src %08x:%d, dst %08x:%d"), udp->Length(), 
				ip4->SrcAddr(), udp->SrcPort(), ip4->DstAddr(), udp->DstPort());
	
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
				TInt r = User::LoadPhysicalDevice(KZeroCopyLoopbackPddFileName);
				RDebug::Printf("Loading PDD - %d\n", r);
				if(r == KErrNone || r == KErrAlreadyExists)
					{
					r = User::LoadLogicalDevice(KZeroCopyLoopbackLddFileName);
					RDebug::Printf("Loading LDD - %d\n", r);
					if(r == KErrNone || r == KErrAlreadyExists)
						{
						r = iDrv.Open();
						RDebug::Printf("Opening driver - %d\n", r);
						if(r == KErrNone)
							{
							// Have the driver open our pools
							RCommsBufPond pond = TCommsBufPondTLSOp::Get();
							r = iDrv.LoadPond(pond);

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
				
				__FLOG_4(_L("CZeroCopyLoopbackBinder4:\tSend(...): passing packet to driver as src %08x:%d, dst %08x:%d"),  
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
	

MLowerDataSender* CZeroCopyLoopbackBinder4::Bind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
/**
Return the MLowerDataSender instance (CZeroCopyLoopbackBinder4) that we
previously allocated.
*/
	{
    __FLOG_3(_L8("CZeroCopyLoopbackBinder4 %08x:\tBind(aUpperReceiver %08x, aUpperControl %08x)"), this, &aUpperReceiver, &aUpperControl);
	iUpperReceiver = &aUpperReceiver;
	iUpperControl = &aUpperControl;

	// Signal upper layer that we are ready
	BinderReady();

	return this;
	}
	
void CZeroCopyLoopbackBinder4::Unbind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
/**
Forget our association with upper layer.
*/
	{
    static_cast<void>(aUpperReceiver);
    static_cast<void>(aUpperControl);
    __FLOG_1(_L8("CZeroCopyLoopbackBinder4 %08x:\tUnbind()"), this);
    ASSERT(&aUpperReceiver == iUpperReceiver);
    ASSERT(&aUpperControl == iUpperControl);
    iUpperReceiver = NULL;
    iUpperControl = NULL;
	}
	
void CZeroCopyLoopbackBinder4::BinderReady()
/**
Signal upper layer that we are ready
*/
	{
    __FLOG_1(_L8("CZeroCopyLoopbackBinder4 %08x:\tBinderReady()"), this);

	iUpperControl->StartSending();
	}

void CZeroCopyLoopbackBinder4::ProcessPacket(RMBufChain& aPdu)
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
		__FLOG_3(_L("CZeroCopyLoopbackBinder4 %08x:\tProcessPacket(): IPv4 length %d, protocol %d"), this, ip4->TotalLength(), ip4->Protocol());

		UpdateHeaders(ip4, NULL);
		// now process it (pass up the stack)
		iUpperReceiver->Process(aPdu);
		return;
		}

	// get the udp header as well - assume only udp traffic here
	TInet6HeaderUDP* udp = (TInet6HeaderUDP*) ip4->EndPtr();
	__FLOG_4(_L("CZeroCopyLoopbackBinder4 %08x:\tProcessPacket(): UDP length %d, src port %d, dst port %d"), this, udp->Length(), udp->SrcPort(), udp->DstPort());

	// depending on the contents, pass it on up thru the stack 
	// or maybe do something else

	// use the destination port number to decide whether or not the payload is a command
	TUint dstPort = udp->DstPort();
	if (KZeroCopyLoopbackCmdPort == dstPort)
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
					iZeroCopyLoopbackSubConnectionFlow.AccessPointConfig().FindExtension(
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

TInt CZeroCopyLoopbackBinder4::GetConfig(TBinderConfig& aConfig)
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

	__FLOG_2(_L8("CZeroCopyLoopbackBinder4 %08x:\tGetConfig(): iLocalAddress %08x"), this, iLocalAddress);        
	
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

TInt CZeroCopyLoopbackBinder4::Control(TUint aLevel, TUint aName, TDes8& aOption)
/**
Called from upper layer for special control functionality.
*/
	{
	(void) aOption;
	(void) aName;
	(void) aLevel;
	__FLOG_3(_L("CZeroCopyLoopbackBinder4 %08x:\tControl(aLevel %x, aName %x)"), this, aLevel, aName);
	return KErrNotSupported;
	}

TInt CZeroCopyLoopbackBinder4::GetName(TDes& aName)
/**
Called from upper layer to retrieve the binder name.

@param aName populated with name
@return KErrNone on success, else a system wide error code.
*/
	{
	// This name matches the NIF-based DummyNif to match any potential
	// test code expectations on the name.
	aName.Format(_L("zerocopy_loopback[0x%08x]"), this);

	__FLOG_2(_L("CZeroCopyLoopbackBinder4 %08x:\tGetName(): %S"), this, &aName);

	return KErrNone;
	}

TInt CZeroCopyLoopbackBinder4::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
    __FLOG_1(_L8("CZeroCopyLoopbackBinder4 %08x:\tBlockFlow()"), this);

	return KErrNotSupported;
	}

TBool CZeroCopyLoopbackBinder4::MatchesUpperControl(ESock::MUpperControl* aUpperControl) const
/**
Utility function that returns whether this binder is associated with the
MUpperControl object passed as argument.

@param aUpperControl upper layer to match against
@return ETrue on a match else EFalse.
*/
	{
	return aUpperControl == iUpperControl;
	}


CZeroCopyLoopbackBinder4::CDrvSender::CDrvSender(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
    __FLOG_OPEN(KNif, KBinderSender4);
	}

CZeroCopyLoopbackBinder4::CDrvSender::~CDrvSender()
	{
	Cancel();
	RMBufPacket pkt;
	do
		{
		pkt.Free();
		}
	while(iSendQueue.Remove(pkt));
    RDebug::Printf("one:%d, many:%d", iOneBufCount, iManyBufCount);
    __FLOG_CLOSE;
	}

void CZeroCopyLoopbackBinder4::CDrvSender::Start(CZeroCopyLoopbackBinder4* aBinder, RZeroCopyLoopbackDriver aDrv)
	{
	iBinder = aBinder;
	iDrv = aDrv;
	}

void CZeroCopyLoopbackBinder4::CDrvSender::RunL()
	{
	// Send the next packet waiting
	SendPacket();
	}

void CZeroCopyLoopbackBinder4::CDrvSender::DoCancel()
	{
	iDrv.SendDataCancel();
	}

void CZeroCopyLoopbackBinder4::CDrvSender::SendPacket()
	{
	// Fetch packet to send
	RMBufPacket sendPkt;
	TBool havePktToSend = iSendQueue.Remove(sendPkt);
	
	if(havePktToSend)
		{
		RMBuf* sendBuffer = NULL;

		// If the packet is composed of more than one buffer then we log this and abandon the packet
		if(sendPkt.NumBufs() > 1)
			{
			__FLOG(_L8("Warning: packet composed of more than one buffer"));
			iManyBufCount++;

			// Copy out to a single buffer
			sendBuffer = RMBuf::Alloc(KLoopbackBearerMTU, iBinder->iAllocator);
			if(sendBuffer)
				{
				sendBuffer->SetLength(0);
				RMBuf* currentBuf = sendPkt.First();
				while(currentBuf)
					{
					TPtr8 srcData(currentBuf->Ptr(), currentBuf->Length(), currentBuf->Length());
					sendBuffer->Append(srcData);
					currentBuf = currentBuf->Next();
					}
				}
			sendPkt.Free();
			}
		else
			{
			iOneBufCount++;
			sendBuffer = sendPkt.Remove();
			}

		// Send the buffer
		if(sendBuffer)
			{
			iDrv.SendData(iStatus, *sendBuffer);
			SetActive();
			}
		}
	}

void CZeroCopyLoopbackBinder4::CDrvSender::QueueForSend(RMBufPacket& aPkt)
	{
	aPkt.FreeInfo();
	iSendQueue.Append(aPkt);
	if(!IsActive())
		{
		SendPacket();
		}
	}

	
CZeroCopyLoopbackBinder4::CDrvReceiver::CDrvReceiver(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	iPond = TCommsBufPondTLSOp::Get();
	}

CZeroCopyLoopbackBinder4::CDrvReceiver::~CDrvReceiver()
	{
	Cancel();
	}

void CZeroCopyLoopbackBinder4::CDrvReceiver::Start(CZeroCopyLoopbackBinder4* aBinder, RZeroCopyLoopbackDriver aDrv)
	{
	iBinder = aBinder;
	iDrv = aDrv;
	RequestReceipt();
	}

void CZeroCopyLoopbackBinder4::CDrvReceiver::RequestReceipt()
	{
	// Peform the receive request -ask driver to send us a packet
	iDrv.ReceiveData(iStatus);
	SetActive();
	}

void CZeroCopyLoopbackBinder4::CDrvReceiver::RunL()
	{
	// Check for an error first
	TInt result = iStatus.Int() ;
	User::LeaveIfError(result);
	
	// Fetch the commsbuf represented by the handle returned
	RCommsBuf* rxBuffer = iPond.FromHandle(result);

	// Form a packet from our buffer
	RMBufQ q((RMBuf*)rxBuffer);
	TRAPD(ret, iRxPkt.CreateL(q));	// need non-leaving overload!
	if(ret == KErrNone)
		{
		iRxPkt.Pack();
		}
	
	// Pass the received packet up the stack
	iBinder->ProcessPacket(iRxPkt);
	ASSERT(iRxPkt.IsEmpty());

	// Round we go. Wait for another packet
	RequestReceipt();
	}

void CZeroCopyLoopbackBinder4::CDrvReceiver::DoCancel()
	{
	iDrv.ReceiveDataCancel();
	}


// =================================================================================
//
// CZeroCopyLoopbackBinder6

CZeroCopyLoopbackBinder6::CZeroCopyLoopbackBinder6(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow) : iZeroCopyLoopbackSubConnectionFlow(aZeroCopyLoopbackSubConnectionFlow)
	{
    __FLOG_OPEN(KNif, KBinder6);
    __FLOG_2(_L8("CZeroCopyLoopbackBinder6 %08x:\tCZeroCopyLoopbackBinder6(CZeroCopyLoopbackSubConnectionFlow& %08x)"), this, &aZeroCopyLoopbackSubConnectionFlow);
	}

CZeroCopyLoopbackBinder6::~CZeroCopyLoopbackBinder6()
/** 
Destroys 'this'
*/
    {
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\t~CZeroCopyLoopbackBinder6()"));
    __FLOG_CLOSE;
    }

CZeroCopyLoopbackBinder6* CZeroCopyLoopbackBinder6::NewL(CZeroCopyLoopbackSubConnectionFlow& aZeroCopyLoopbackSubConnectionFlow)
	{
	return new (ELeave) CZeroCopyLoopbackBinder6(aZeroCopyLoopbackSubConnectionFlow);
	}

void CZeroCopyLoopbackBinder6::UpdateHeaders(TInet6HeaderIP* aIp6, TInet6HeaderUDP* /*aUdp*/)
    {
	// swap over the destination and source addresses
	TIp6Addr temp;
	temp = aIp6->SrcAddr();
	aIp6->SetSrcAddr(aIp6->DstAddr());
	aIp6->SetDstAddr(temp);
    }
    
MLowerDataSender::TSendResult CZeroCopyLoopbackBinder6::Send(RMBufChain& aData)
/**
Send IPv6 data

Note: not clear that this is properly supported or used.

@param aData data to send
*/
	{
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tSend()"));

   	// Loop the data straight back into the TCP/IP stack
	ProcessPacket(aData);
	return ESendAccepted;
	}
	
MLowerDataSender* CZeroCopyLoopbackBinder6::Bind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
	{
    __FLOG_2(_L8("CZeroCopyLoopbackBinder6:\tBind(MUpperDataReceiver %08x, MUpperControl %08x)"), &aUpperReceiver, &aUpperControl);

	iUpperReceiver = &aUpperReceiver;
	iUpperControl = &aUpperControl;
	
	// Signal upper layer that we are ready
	BinderReady();
	
	return this;
	}

void CZeroCopyLoopbackBinder6::BinderReady()
/**
Signal to upper layer that we are ready
*/
    {
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tBinderReady()"));

	iUpperControl->StartSending();
    }

void CZeroCopyLoopbackBinder6::ProcessPacket(RMBufChain& aPdu)
/**
Process incoming IPv6 packets.

Note: not clear that this is properly supported or used.

@param aPdu incoming data packet
*/
    {
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tProcessPacket()"));

	// this received data has already been looped back...
	// get the ip header from the RMBufChain
	TInet6HeaderIP* ip6 = (TInet6HeaderIP*) aPdu.First()->Next()->Ptr();
	TInet6HeaderUDP* udp = NULL;

	if ((TUint)ip6->NextHeader() == KProtocolInetUdp)
		{
		// get the udp header as well - assume only udp traffic here
		udp = (TInet6HeaderUDP*) ip6->EndPtr();

		__FLOG_3(_L("CZeroCopyLoopbackBinder6:\tProcessPacket(...): UDP length %d, src port %d, dst port %d"),
			udp->Length(), udp->SrcPort(), udp->DstPort());

		// depending on the contents, pass it on up thru the stack 
		// or maybe do something else

		// use the destination port number to decide whether or not the payload is a command
		TUint dstPort = udp->DstPort();
		if (KZeroCopyLoopbackCmdPort == dstPort)
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
		__FLOG_2(_L("CZeroCopyLoopbackBinder6:\tProcessPacket(...): IPv6 length %d, next header %d"),
			ip6->PayloadLength(), ip6->NextHeader());
		}

	// just echo the packet back to the original sender

	// update the headers (addresses, checksums etc).  If "udp" is non-NULL, then
	// the UDP ports will be updated as well.
	UpdateHeaders(ip6, udp);
	// now process it (pass up the stack)
	iUpperReceiver->Process(aPdu);		
    }

void CZeroCopyLoopbackBinder6::Unbind(MUpperDataReceiver& aUpperReceiver, MUpperControl& aUpperControl)
	{
    static_cast<void>(aUpperReceiver);
    static_cast<void>(aUpperControl);
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tUnbind()"));

    ASSERT(&aUpperReceiver == iUpperReceiver);
    ASSERT(&aUpperControl == iUpperControl);
    iUpperReceiver = NULL;
    iUpperControl = NULL;
	}
	
TInt CZeroCopyLoopbackBinder6::GetName(TDes& aName)
/**
Called from upper layer to retrieve the binder name.

@param aName populated with name
@return KErrNone on success, else a system wide error code.
*/
	{
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tGetName()"));

	// This name matches the NIF-based DummyNif to match any potential
	// test code expectations on the name.
	aName.Format(_L("zerocopy_loopback6[0x%08x]"), this);
	
	return KErrNone;
	}

TInt CZeroCopyLoopbackBinder6::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tBlockFlow()"));

	return KErrNotSupported;
	}

void CZeroCopyLoopbackBinder6::StaticDnsConfiguration(TBinderConfig6& aConfig)
	{
    __FLOG(_L8("CZeroCopyLoopbackBinder6:\tStaticDnsConfiguration()"));

	const TZeroCopyLoopbackIp6Provision* ip6Provision = Flow()->Ip6Provision();

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

TInt CZeroCopyLoopbackBinder6::GetConfig(TBinderConfig& aConfig)
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

TInt CZeroCopyLoopbackBinder6::Control(TUint aLevel, TUint aName, TDes8& aOption)
	{
	(void) aOption;
	(void) aName;
	(void) aLevel;
	__FLOG_2(_L("CZeroCopyLoopbackBinder6:\tControl(aLevel %x, aName %x, ...)"), aLevel, aName);
	return KErrNotSupported;
	}

//
// Utilities
//

TBool CZeroCopyLoopbackBinder6::MatchesUpperControl(ESock::MUpperControl* aUpperControl) const
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

CZeroCopyLoopbackErrorOneShot::CZeroCopyLoopbackErrorOneShot()
  : CAsyncOneShot(EPriorityStandard)
	{
	}

void CZeroCopyLoopbackErrorOneShot::Schedule(MUpperControl* aUpperControl)
	{
	iUpperControl = aUpperControl;
	Call();
	}

void CZeroCopyLoopbackErrorOneShot::RunL()
	{
	iUpperControl->Error(KErrCommsLineFail);
	}


CZeroCopyLoopbackFlowTestingSubscriber::CZeroCopyLoopbackFlowTestingSubscriber(CZeroCopyLoopbackSubConnectionFlow& aFlow, TUint aApId)
	: CActive(0),
	iFlow(aFlow),
	iApId(aApId)
	{
	}

/*static*/ TBool CZeroCopyLoopbackFlowTestingSubscriber::ShouldRun(TUint aApId)
	{
	RProperty property;
	TInt result = property.Attach(KZeroCopyLoopbackTestingPubSubUid, aApId);
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

void CZeroCopyLoopbackFlowTestingSubscriber::ConstructL()
	{
	CActiveScheduler::Add(this);
//	__DECLARE_NAME(_S("CAvailabilityTestingSubscriber"));

	TInt result = iProperty.Attach(KZeroCopyLoopbackTestingPubSubUid, iApId);
	ASSERT(result == KErrNone);

	RunL();
	}

void CZeroCopyLoopbackFlowTestingSubscriber::RunL()
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

void CZeroCopyLoopbackFlowTestingSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}

/*virtual*/ CZeroCopyLoopbackFlowTestingSubscriber::~CZeroCopyLoopbackFlowTestingSubscriber()
	{
	this->Cancel(); // object must be stoppable by descruction due to cleanup restrictions
	iProperty.Close();
	}



