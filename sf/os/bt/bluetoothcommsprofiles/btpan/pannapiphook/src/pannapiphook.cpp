// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "pannapiphook.h"
#include "RBnepFrame.h"			// For the DHCP / Forwarding RMBufPacket header flags

#include <in_sock.h>
#include <in_chk.h>
#include <udp_hdr.h>
#include <es_prot_internal.h>

_LIT(KProtocolPanNapIpHookName, "pannapiphook");

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_NAPIPHOOK);
#endif

#ifdef __FLOG_ACTIVE
// Define strings for logging.
_LIT(KStringTrue, "True");
_LIT(KStringFalse,  "False");
_LIT(KStringDropped, "Dropped");
_LIT(KStringPassed,  "Passed");
#endif


static const TUint KDhcpDstPort = 67;

// Returning a negative number from the ApplyL() method in an IP hook will make the IP
// stack drop the packet. No constant is defined in networking so define it here to
// help readability of the code.
static const TInt KIp6Hook_DROP = -1;

//
// CPanNapIpHook
//
CPanNapIpHook* CPanNapIpHook::NewL()
	{
	CPanNapIpHook* self = new(ELeave) CPanNapIpHook();
	return self;
	}

CPanNapIpHook::~CPanNapIpHook()
	{
	Unbind(this,BindHookAll());
	CProtocolPosthook::NetworkDetached();
	}

void CPanNapIpHook::NetworkAttachedL()
	{
	// This hook is dual purpose and checks both that incoming packets destined for the
	// DHCP server and any packets to be forwarded to the uplink have access to do so.
	// This is why the hook gets bound in 2 places, once for forwarding packets and once
	// for all incoming packets. The logic in ApplyL() determines where the hook has 
	// been called from.
	NetworkService()->BindL(this, BindForwardHook());
	NetworkService()->BindL(this, BindHookAll());
	iManager = NetworkService()->Interfacer();
	}

TInt CPanNapIpHook::ApplyL(RMBufHookPacket &aPacket, RMBufRecvInfo &aInfo)
	{
	TInt rCode = KIp6Hook_PASS;
	
	// This hook is for packets coming from BNEP only and performs two purposes depending
	// on whether or not the packet is destined for the local host or not.
	if(aInfo.iFlags & KBnep)
		{
		LOG1(_L("Remote device granted access to uplink: %S"), (aInfo.iFlags & KBnepForwardingAllowed) ? &KStringTrue() : &KStringFalse());
		
		if (iManager->IsForMeAddress(TInetAddr::Cast(aInfo.iDstAddr).Ip6Address(), aInfo.iInterfaceIndex))
			{
			LOG(_L("Packet destined for local host"));

			// Make sure that any packets from BNEP destined for the DHCP Server are processed
			// only if it has come from a remote device granted access to the uplink
			if(aInfo.iProtocol == KProtocolInetUdp)
				{
				TInet6Checksum<TInet6HeaderIP4> ipHeader(aPacket);
				TInet6Packet<TInet6HeaderUDP> udpHeader(aPacket, ipHeader.iHdr->HeaderLength());
				
				if(udpHeader.iHdr->DstPort() == KDhcpDstPort)
					{
					LOG(_L("DHCP packet detected"));
					
					// DHCP packet. Drop if forwarding flag not set.
					if(!(aInfo.iFlags & KBnepForwardingAllowed))
						{
						aPacket.Free();
						rCode = KIp6Hook_DROP;  // Drop packet
						}
					}
				}			
			}
		else
			{
			LOG(_L("Packet destined to be forwarded"));

			// Before forwarding packets from BNEP ensure that they have come from a remote
			// device granted access to the uplink
			if(!(aInfo.iFlags & KBnepForwardingAllowed))
				{
				aPacket.Free();
				rCode = KIp6Hook_DROP;  // Drop packet
				}
			}
		}
		
	// Finished with the flags
	aInfo.iFlags &= ~KBnep;
	aInfo.iFlags &= ~KBnepForwardingAllowed;

	LOG1(_L("Packet %S"), (rCode < 0) ? &KStringDropped() : &KStringPassed());

	return rCode;
	}

void CPanNapIpHook::Identify(TServerProtocolDesc* aProtocolDesc) const 
	{
	Describe(*aProtocolDesc);
	}

void CPanNapIpHook::Describe(TServerProtocolDesc& aEntry)
	{
	aEntry.iName = KProtocolPanNapIpHookName;
	aEntry.iAddrFamily = KAfInet;
	aEntry.iSockType = KSockDatagram;
	aEntry.iProtocol = KProtocolPanNapIpHook;
	aEntry.iVersion = TVersion(1, 0, 0);
	aEntry.iByteOrder = EBigEndian;
	aEntry.iServiceInfo = KSIDatagram | KSIConnectionLess;
	aEntry.iNamingServices = 0;
	aEntry.iSecurity = KSocketNoSecurity;
	aEntry.iMessageSize = 0xffff;
	aEntry.iServiceTypeInfo = 0;
	aEntry.iNumSockets = KUnlimitedSockets;
	aEntry.iServiceTypeInfo = ESocketSupport | EInterface;
	}

//
// CPanNapIpHookFamily
//	
CPanNapIpHookFamily::CPanNapIpHookFamily()
	{
	__DECLARE_NAME(_S("CPanNapIpHookFamily"));
	}

CPanNapIpHookFamily::~CPanNapIpHookFamily()
	{
	}

TInt CPanNapIpHookFamily::Install()
	{
	return KErrNone;
	}

TInt CPanNapIpHookFamily::Remove()
	{
	return KErrNone;
	}

TUint CPanNapIpHookFamily::ProtocolList(TServerProtocolDesc *& aProtocolList)
	{
	aProtocolList = new TServerProtocolDesc[1];
	
	if (!aProtocolList)
		{
		return 0;
		}
		
	CPanNapIpHook::Describe(aProtocolList[0]);
	return 1;
	}

CProtocolBase* CPanNapIpHookFamily::NewProtocolL(TUint /*aSockType*/, TUint aProtocol)
	{
	if (aProtocol != KProtocolPanNapIpHook)
		{
		User::Leave(KErrNotSupported);
		}
	return CPanNapIpHook::NewL();
	}

EXPORT_C CProtocolFamilyBase* Install(void)
	{
	CPanNapIpHookFamily* protocol = new CPanNapIpHookFamily();
	return protocol;
	}
