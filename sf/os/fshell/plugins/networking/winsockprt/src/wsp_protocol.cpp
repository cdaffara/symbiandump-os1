// wsp_protocol.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include <winsockprt.h>
#include "wsp_protocol.h"
#include "wsp_provider.h"
#include "wsp_factory.h"
#include "wsp_resolver.h"
#include "wsp_panic.h"
#include "wsp_def.h"
#include "wsp_log.h"


CWinSockProtocol::~CWinSockProtocol()
	{
	}

CWinSockProtocol::CWinSockProtocol(TUint aSocketType, RWin32Factory& aWin32Factory)
	: iSocketType(aSocketType), iWin32Factory(aWin32Factory)
	{
	}

CWinSockProtocol* CWinSockProtocol::NewL(TUint aSocketType, RWin32Factory& aWin32Factory)
	{
	CWinSockProtocol* self = new(ELeave) CWinSockProtocol(aSocketType, aWin32Factory);
	return self;
	}

void CWinSockProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc, TUint aSocketType, TUint aProtocol)
	{
	switch (aSocketType)
		{
		case KSockStream:
			{
			aDesc->iName = (aProtocol == KProtocolWinsockTcp) ? _S("winsocktcp") : _S("tcp");
			aDesc->iAddrFamily = KAfInet;
			aDesc->iSockType = KSockStream;
			aDesc->iProtocol = aProtocol;
			aDesc->iVersion = TVersion(KWspMajorVersionNumber, KWspMinorVersionNumber, KWspBuildVersionNumber);
			aDesc->iByteOrder = EBigEndian;
			aDesc->iServiceInfo = KSIStreamBased | KSIInOrder | KSIReliable | KSIGracefulClose;
			aDesc->iNamingServices = KNSNameResolution;
			aDesc->iSecurity = KSocketNoSecurity;
			aDesc->iMessageSize = KSocketMessageSizeIsStream;
			aDesc->iServiceTypeInfo = ESocketSupport | EUseCanSend;
			aDesc->iNumSockets = KUnlimitedSockets;
			return;
			}
		case KSockDatagram:
			{
			if (aProtocol == KProtocolWinsockUdp)
				{
				aDesc->iName =  _S("winsockudp");
				}
			else if (aProtocol == KProtocolInetUdp)
				{
				aDesc->iName = _S("udp");
				}
			else if (aProtocol == KProtocolInetIp)
				{
				aDesc->iName = _S("ip");
				}
			aDesc->iAddrFamily = KAfInet;
			aDesc->iSockType = KSockDatagram;
			aDesc->iProtocol = aProtocol;
			aDesc->iVersion = TVersion(KWspMajorVersionNumber,	KWspMinorVersionNumber, KWspBuildVersionNumber);
			aDesc->iByteOrder = EBigEndian;
			aDesc->iServiceInfo = KSIConnectionLess | KSIDatagram | KSIGracefulClose;
			aDesc->iNamingServices = KNSNameResolution;
			aDesc->iSecurity = KSocketNoSecurity;
			aDesc->iMessageSize = 65536-128; /*KSocketMessageSizeUndefined;*/
			aDesc->iServiceTypeInfo = ESocketSupport | EUseCanSend;
			aDesc->iNumSockets = KUnlimitedSockets;
			return;
			}
		}
	}

CServProviderBase* CWinSockProtocol::NewSAPL(TUint aProtocol)
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProtocol::NewSAPL")));

	CWinSockProviderBase* provider = NULL;

	switch (iSocketType)
		{
		case KSockStream:
			{
			provider = CWinSockTcpProvider::NewL(iWin32Factory);
			break;
			}
		case KSockDatagram:
			{
			provider = CWinSockUdpProvider::NewL(iWin32Factory);
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtCWinSockProtocolNewSAPLInvalidProtocol));
			}
		}

	iProtocol = aProtocol;
	
	return provider;
	}

void CWinSockProtocol::Identify(TServerProtocolDesc* aDesc) const
	{
	CWinSockProtocol::ProtocolIdentity(aDesc, iSocketType, iProtocol);
	}

void CWinSockProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*anId*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedBindRequestFromPeer));
	}

void CWinSockProtocol::BindToL(CProtocolBase* /*protocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedBindToRequestFromSocketServer));
	}

TInt CWinSockProtocol::Send(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedSendDownCallFromBindee));
	return 0;
	}

void CWinSockProtocol::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedProcessUpCallFromBindee));
	}

TInt CWinSockProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedSendDownCallFromBindee));
	return KErrNone;
	}

void CWinSockProtocol::Process(TDes8&,TSockAddr* /*from*/,TSockAddr* /*to*/,CProtocolBase* /*aSourceProtocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedProcessUpCallFromBindee));
	}


TInt CWinSockProtocol::GetOption(TUint aLevel,TUint aName,TDes8& aOption,CProtocolBase*)
	{
	if ((aLevel == KNifOptLevel) && (aName == KNifOptGetNifIfUser))
		{
		static_cast<TNifIfUser&>(aOption) = this;
		return KErrNone;
		}
	return KErrNotSupported;
	}

TInt CWinSockProtocol::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return KErrNotSupported;
	}

void CWinSockProtocol::Error(TInt /*anError*/,CProtocolBase* /*aSourceProtocol*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedErrorUpCallFromBindee));
	}

CHostResolvProvdBase* CWinSockProtocol::NewHostResolverL()
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProtocol::NewHostResolverL")));
	return CWinSockResolver::NewL(iWin32Factory);
	}

CServiceResolvProvdBase* CWinSockProtocol::NewServiceResolverL()
	{
	return NULL;
	}

CNetDBProvdBase* CWinSockProtocol::NewNetDatabaseL()
	{
	return NULL;
	}

void CWinSockProtocol::IfUserBindFailure(TInt, TAny*)
	{
	}

void CWinSockProtocol::IfUserNewInterfaceL(CNifIfBase*, TAny*)
	{
	}

void CWinSockProtocol::IfUserInterfaceDown(TInt, CNifIfBase*)
	{
	}

void CWinSockProtocol::IfUserOpenNetworkLayer()
	{
	Open();
	}

void CWinSockProtocol::IfUserCloseNetworkLayer()
	{
	Close();
	}

CProtocolBase* CWinSockProtocol::IfUserProtocol()
	{
	return this;
	}

TBool CWinSockProtocol::IfUserIsNetworkLayerActive()
	{
	}

