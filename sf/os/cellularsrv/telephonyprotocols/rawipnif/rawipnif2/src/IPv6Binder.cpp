// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the CIPv6Binder class, which handles the transmission
// of IPv6 data to and from the TCP/IP stack.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "IPv6BinderTraces.h"
#endif

#include <etelpckt.h>
#include <in_iface.h>
#include "RawIP2Flow.h"
#include "IPv6Binder.h"
#include <comms-infras/linkprovision.h>
#include <e32hal.h>



#ifdef WCDMA_STUB
#include <networking/umtsnifcontrolif.h>
#endif


CIPv6Binder::CIPv6Binder(CRawIP2Flow& aFlow)
/**
 * Constructor
 */ 
	: CBinderBase(aFlow),
	  iSpeedMetric(KDefaultSpeedMetric)
	{
	}

CIPv6Binder::~CIPv6Binder()
/**
 *	Destructor
 */
	{
	}

ESock::MLowerDataSender* CIPv6Binder::Bind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl)
/**
 * Binds TCP/IP protocol to Flow
 *
 * @param aUpperReceiver A pointer to Upper layer Receive class
 * @param aUpperControl A pointer to Upper layer control class
 */
	{
	CBinderBase::Bind(aUpperReceiver, aUpperControl); // Call the superclass's method.
	return this;
	}

TInt CIPv6Binder::Control(TUint aLevel, TUint aName, TDes8& /*aOption*/)
/**
 * The main function called by the TCP/IP protocol to control the interface.
 * Can perform a variety of general IP tasks (such as getting IP config)
 * and "3G" specific tasks (such as deleting the context).
 *
 * @param aLevel The level of the interface to control - always KSOLInterface
 * @param aName The command to perform
 * @param aOption Data to be input/output as a result of the command
 * @return Standard error codes
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_CONTROL_1, "CIPv6Binder::Control [aLevel=%u, aName=%u]",aLevel, aName);

	if (aLevel == KSOLInterface)
		{
		switch (aName)
			{
		case KSoIfHardwareAddr:
			// unsupported because we don't have a h/w address
			break;

		// 3G-specific configuration commands are below this point.
		
#ifdef WCDMA_STUB
		case KRegisterEventHandler:
			// Raw IP NIF Events are not supported
		case KContextSetEvents:
			// Raw IP NIF Events are not supported
			break;

		case KContextCreate:
			// We don't support creating new secondary contexts.
			break;

		case KContextDelete:
			// Deletes the primary PDP context. This will shut down the Nif.
			return DeleteContext(aOption);

		case KContextActivate:
			// If the IPv6 interface is up, then the context will already have
			// been activated. So this command should fail with 
			// KErrAlreadyExists
			{
			TUint8* ptr = CONST_CAST(TUint8*, aOption.Ptr()); 
			TContextParameters* contextParams =
				REINTERPRET_CAST(TContextParameters*, ptr);

			if (contextParams->iContextInfo.iContextId != 
				STATIC_CAST(TInt8, GetFlow().GetBcaController()->Nsapi()))
				{
				contextParams->iReasonCode = KErrNotFound;
				}
			else
				{
				contextParams->iContextInfo.iStatus =
					GetFlow().GetContextStatus();
				contextParams->iReasonCode = KErrAlreadyExists;
				}
			return KErrNone;
			}

		case KNifSetDefaultQoS:
		case KContextQoSSet:
			// Setting the QoS is meaningless over GPRS, so we just return that
			// we don't support these operations.
			break;

		case KContextTFTModify:
			// As we only have one primary context, we don't support anything
			// to do with traffic flow templates, which are used by secondary
			// contexts.
			break;

		case KContextModifyActive:
			// This command is only valid aftermodifying TFT/QoS parameters. 
			// As we don't support any of these operations,
			// this command is never valid.
			break;
#endif

		default:
			break;
			}
		}
	return KErrNotSupported;
	}

TInt CIPv6Binder::GetConfig(TBinderConfig& aConfig)
	{
    TBinderConfig6* config = TBinderConfig::Cast<TBinderConfig6>(aConfig);
    
   	if(config == NULL)
   		{
   		return KErrNotSupported;
   		}
	
	config->iFamily = KAfInet6;		/* KAfInet6 - selects TBinderConfig6 */

	config->iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast;		/* Feature flags */
	config->iInfo.iMtu = KDefaultMtu;				/* Maximum transmission unit. */
	config->iInfo.iRMtu = KDefaultMtu;				/* Maximum transmission unit for receiving. */
	config->iInfo.iSpeedMetric = iSpeedMetric;		/* approximation of the interface speed in Kbps. */
	
	TEui64Addr& localId = TEui64Addr::Cast(config->iLocalId);
	localId = iSettings.iLocalIfId;
	config->iNameSer1.SetAddress(iSettings.iPrimaryDns);		/* IP primary name server (if any). */
	config->iNameSer2.SetAddress(iSettings.iSecondaryDns);	/* IP secondary name server (if any). */

	return KErrNone;
	}

#ifdef WCDMA_STUB

TInt CIPv6Binder::DeleteContext(TDes8& aContextParameters)
/**
 * Deletes a context. As the NIF is responsible for one primary context,
 * this is equivalent to closing down the NIF.
 *
 * @param aContextParameters Parameters of the context to delete
 * @return KErrArgument if an incorrect structure is passed, otherwise KErrNone
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_DELETECONTEXT_1, "CIPv6Binder::DeleteContext");

	if (aContextParameters.Length() != sizeof(TContextParameters))
		{
		return KErrArgument;
		}

	TUint8* ptr = CONST_CAST(TUint8*, aContextParameters.Ptr());
	TContextParameters* params = REINTERPRET_CAST(TContextParameters*, ptr);

	if (params->iContextInfo.iContextId != 
		STATIC_CAST(TInt8, GetFlow().GetBcaController()->Nsapi()))
		{
		params->iReasonCode = KErrBadName;
		}
	else
		{
		params->iReasonCode = KErrNone; 
		GetFlow().Stop(KErrNone, MNifIfNotify::EDisconnect);
		}

	return KErrNone;
	}

#endif

/**
 * Called when the context has been activated to set our IP address and get
 * any other required settings from CommDB.
 *
 * @param aConfig The new context config
 */
 void CIPv6Binder::UpdateContextConfigL(const TPacketDataConfigBase& aConfig)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_1, "CIPv6Binder::UpdateContextConfig");

	// Get our IP address from the GPRS context config.
	TInetAddr address;
	
	TBuf<RPacketContext::KMaxPDPAddressLength> tempAddr;
	
	const RPacketContext::TProtocolConfigOptionV2* pco;
	TInt rel = const_cast<TPacketDataConfigBase&>(aConfig).ExtensionId();
	if (rel == TPacketDataConfigBase::KConfigGPRS) 
	    {
	    tempAddr.Copy(static_cast<const RPacketContext::TContextConfigGPRS&>(aConfig).iPdpAddress);
	    pco = &static_cast<const RPacketContext::TContextConfigGPRS&>(aConfig).iProtocolConfigOption;
	    }
    else
        {
        ASSERT(rel == TPacketDataConfigBase::KConfigRel99Rel4 || rel == TPacketDataConfigBase::KConfigRel5);
	    tempAddr.Copy(static_cast<const RPacketContext::TContextConfigR99_R4&>(aConfig).iPdpAddress);
	    pco = &static_cast<const RPacketContext::TContextConfigR99_R4&>(aConfig).iProtocolConfigOption;
        }
	TInt ret = address.Input(tempAddr);

	// We've got our IP address! Let's save it.
	if (ret == KErrNone)
		{
		iSettings.iLocalAddr = address.Ip6Address();
		OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_2, "Got local IP address = %u:%u:%u:%u from context", iSettings.iLocalAddr.u.iAddr32[3], iSettings.iLocalAddr.u.iAddr32[2], iSettings.iLocalAddr.u.iAddr32[1], iSettings.iLocalAddr.u.iAddr32[0]);
		}
	else
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_3, "Couldn't get IP address from GPRS config (err: %d)",ret);

		// Don't leave on this error: we may still be OK if we read some
		// settings from CommDB.
		}

	// @todo - is this correct. We can only get the DNS addresses
	// from the TSY using the iProtocolConfigOption data. Yet a client could
	// access those DNS config details without knowing about the state of the
	// iSettings.iGetDnsFromServer flag.

	if ((iSettings.iGetDnsFromServer) ||
		((iSettings.iPrimaryDns.IsUnspecified()) &&
		 (iSettings.iSecondaryDns.IsUnspecified())) ) 
		{
		TBuf<RPacketContext::KMaxPDPAddressLength> tempAddr;
		tempAddr.Copy(pco->iDnsAddresses.iPrimaryDns);
		ret = address.Input(tempAddr);

		if (ret == KErrNone)
			{
			iSettings.iPrimaryDns = address.Ip6Address();
			OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_4, "Got primary DNS = %u:%u:%u:%u from context", iSettings.iPrimaryDns.u.iAddr32[3], iSettings.iPrimaryDns.u.iAddr32[2], iSettings.iPrimaryDns.u.iAddr32[1], iSettings.iPrimaryDns.u.iAddr32[0]);
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_5, "Couldn't get primary DNS address from GPRS config (err: %d)",ret);

			// Don't leave on this error: we may still be OK if we read some
			// settings from CommDB.
			}

		tempAddr.Copy(pco->iDnsAddresses.iSecondaryDns);
		ret = address.Input(tempAddr);

		if (ret == KErrNone)
			{
			iSettings.iSecondaryDns = address.Ip6Address();
			OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_6, "Got secondary DNS = %u:%u:%u:%u from context", iSettings.iSecondaryDns.u.iAddr32[3], iSettings.iSecondaryDns.u.iAddr32[2], iSettings.iSecondaryDns.u.iAddr32[1], iSettings.iSecondaryDns.u.iAddr32[0]);
			}
		else
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_7, "Couldn't get secondary DNS address from GPRS config (err: %d)",ret);

			// Don't leave on this error: we may still be OK if we read some
			// settings from CommDB.
			}
		}
	else
		{
        OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_8, "Using CommDB DNS address - Primary  = %u:%u:%u:%u", iSettings.iPrimaryDns.u.iAddr32[3], iSettings.iPrimaryDns.u.iAddr32[2], iSettings.iPrimaryDns.u.iAddr32[1], iSettings.iPrimaryDns.u.iAddr32[0]);
        OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONTEXTCONFIGL_9, "                         - Secondary = %u:%u:%u:%u", iSettings.iSecondaryDns.u.iAddr32[3], iSettings.iSecondaryDns.u.iAddr32[2], iSettings.iSecondaryDns.u.iAddr32[1], iSettings.iSecondaryDns.u.iAddr32[0]);
		}
	}

void CIPv6Binder::UpdateConnectionSpeed(TUint aConnectionSpeed)
/**
 * Sets the speed metric to return to TCP/IP, based on what the TSY tells us.
 *
 * @param aConnectionSpeed Our connection speed
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_UPDATECONNECTIONSPEED_1, "CIPv6Binder::UpdateConnectionSpeed");

	iSpeedMetric = aConnectionSpeed;
	}

ESock::MLowerDataSender::TSendResult CIPv6Binder::Send(RMBufChain& aPdu)
	{
	return Send(static_cast<RCommsBufChain&>(aPdu));
	}

ESock::MLowerDataSender::TSendResult CIPv6Binder::Send(RCommsBufChain& aPdu)
/**
 * Called by the protocol to send an outgoing IP packet to the network.
 *
 * @param aPdu The outgoing packet
 * @return Standard error codes
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_SEND1_1, "CIPv6Binder::Send");

#ifdef __BTT_LOGGING__
		LogPacket(static_cast<RMBufChain&>(aPdu));
#endif
	
		// Return <0: an error occurred
		// Return  0: no error, but don't send any more packets
	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
		iIPTagHeader->AddHeader(aPdu);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
		
		return ESock::MLowerDataSender::TSendResult(iLowerDataSender->Send(aPdu));

	}

TInt CIPv6Binder::Notification(TAgentToNifEventType /*aEvent*/, 
	void* /*aInfo*/)
/**
 * The Nif will ignore any notification sent
 *
 * @param aEvent Not used
 * @param aInfo Not used 
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_NOTIFICATION_1, "CIPv6Binder::Notification");

	return KErrNone;
	}

void CIPv6Binder::StartSending()
/**
 * Indicates to the protocol layer that the NIF is ready to send packets.
 *
 * @param aProtocol A pointer to a protocol
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_STARTSENDING_1, "CIPv6Binder::StartSending()");
	CBinderBase::StartSending();
	}

TBool CIPv6Binder::WantsProtocol(TUint16 aProtocolCode)
/**
 * Indicates the type of protocol implemented by this class.
 *
 * @param aProtocolCode The protocol type
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_WANTSPROTOCOL_1, "CIPv6Binder::WantsProtocol [aProtocolCode=%X]",aProtocolCode);

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	return ((aProtocolCode & 0x00FF) == KIp6FrameType);
#else
	(void) aProtocolCode;
	return ETrue;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	}

void CIPv6Binder::Process(RMBufChain&  aPdu)
	{	
	Process(static_cast<RCommsBufChain&>(aPdu));
	}

void CIPv6Binder::Process(RCommsBufChain& aPdu)
/**
 * Called when an incoming IP packet has arrived. Send packets up to the
 * TCP/IP stack.
 *
 * @param aPdu The incoming packet
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_PROCESS1_1, "CIPv6Binder::Process");

#ifdef __BTT_LOGGING__
	LogPacket(static_cast<RMBufChain&>(aPdu));
#endif

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	TUint16 protocolCode = iIPTagHeader->RemoveHeader(aPdu);
#else
	TUint16 protocolCode = 0;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
		
	
	
	// Pass incoming packets up to the protocol, unless it hasn't
	// been bound yet.
	if (iUpperReceiver && WantsProtocol(protocolCode))
		{		
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_PROCESS1_2, "CIPv6Binder: Packet Sent to TCP/IP Protocol!!!");
		iUpperReceiver->Process(static_cast<RMBufChain&>(aPdu));
		}
	else 
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_PROCESS1_3, "WARNING: dumping incoming packet, no protocol bound");
		aPdu.Free();
		}

	}

//
// MLowerControl methods
//

TInt CIPv6Binder::GetName(TDes& aName)
/**
*/
	{
	WriteIfName(aName);
	return KErrNone;
	}

//
// CBinderBase methods
//

void CIPv6Binder::SetProvision(const CIPConfig& aProvision)
/**
Set provisioning information for IPv6 binder.

Called from RawIP Flow.

@param aProvision Provisioning structure from Control side.
*/
	{
	iSettings.iPrimaryDns = aProvision.GetIp6NameServer1();
	iSettings.iSecondaryDns	= aProvision.GetIp6NameServer2();
	iSettings.iGetDnsFromServer = aProvision.GetIp6DNSAddrFromServer();

	// Read whether to get IPv4 address from the server
	// This is only needed for the integration tests. If it's true then 
	// the IPv4 address will be used to build up the IPv6 address. 
	iSettings.iGetIpFromServer = aProvision.GetIpAddrFromServer();
	
	if (iSettings.iGetIpFromServer == EFalse)
		{
		// Sets the IPv6 Link-local address from IpAddr.
		// LocalId is derived from IpAddr and it's further used to set the Link-local
		// address elsewhere by adding a prefix (FE80::) in front.
		// For IpAddr: 192.168.1.1, link-local address will be FE80::C0A8:101.		
		TUint32 ipAddr = aProvision.GetIpAddress();
		const TUint8 constantId[8] = { 0, 0, 0, 0, 
									ipAddr >> 24, (ipAddr >> 16) & 0xFF, 
									(ipAddr >> 8) & 0xFF, ipAddr & 0xFF }; 
		iSettings.iLocalIfId.SetAddr(constantId, sizeof (constantId));
		}
	else
		{	
		//
		// Use the 64 bit id of MARM machines as our interface id
		//
		TMachineInfoV1Buf machineInfo;	
		UserHal::MachineInfo(machineInfo);
		iSettings.iLocalIfId.SetAddr(machineInfo().iMachineUniqueId);
		iSettings.iLocalIfId.SetUniversalBit(0);
		//
		// In WINS environment the id is zero which is no-no
		//
		if (iSettings.iLocalIfId.IsZero())
			{
				iSettings.iLocalIfId.SetAddrRandomNZ();
			}		
		}
	}

#ifdef __BTT_LOGGING__
void CIPv6Binder::LogPacket(const RMBufChain& aPacket)
/**
* Logs packet information into log file.
*
* @param aPacket The packet 
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_1, "CIPv6Binder::LogPacket");

	TInt mBufLength = aPacket.Length() - aPacket.First()->Length();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_2, "Analysis of %d byte packet:", mBufLength);

	//Note: All the constants used on this method are a pragmatic guess of the
	//IP header fields. The only porpose of this method is logging.

	if (mBufLength < 40)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_3, " -doesn't appear to be a valid IPv6 packet (length=%d)", mBufLength);
		return;
		}

	// Get a pointer to the packet's payload.
	const TUint8* payloadPtr = aPacket.First()->Next()->Ptr();

	if ((payloadPtr[0] & 0xF0) != 0x60)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_4, " - doesn't appear to be an IPv6 packet (version=0x%X)",(payloadPtr[0] & 0xF0) >> 4);
		return;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_5, " - traffic class: 0x%X", ((payloadPtr[0] & 0xF) << 4) | ((payloadPtr[1] & 0xF0) >> 4));
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_6, " - flow label: 0x%X", ((payloadPtr[1] & 0x0F) << 16) | (payloadPtr[2] << 8) | payloadPtr[3]);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_7, " - payload length: 0x%X", (payloadPtr[4] << 16) | payloadPtr[5]);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_8, " - next header: 0x%08X", payloadPtr[6]);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIPV6BINDER_LOGPACKET_9, " - hop limit: 0x%08X", payloadPtr[7]);
	}
#endif // __BTT_LOGGING__
