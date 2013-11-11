// wsp_protocolfamily.cpp
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


#include <f32file.h>
#undef SYMBIAN_ENABLE_SPLIT_HEADERS // More tb92 stopgaps, this time for es_sock_partner.h
#include <winsockprt.h>
#include <in_sock.h>
#include "wsp_protocolfamily.h"
#include "wsp_protocol.h"
#include "wsp_log.h"


CWinSockProtocolFamily::~CWinSockProtocolFamily()
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProtocolFamily::~CWinSockProtocolFamily")));
	if (iWin32Factory)
		{
		iWin32Factory->Close();
		delete iWin32Factory;
		}
	WSP_LOG(WspLog::Close());
	}

CWinSockProtocolFamily::CWinSockProtocolFamily()
	{
	}

CWinSockProtocolFamily* CWinSockProtocolFamily::NewL()
	{
	WSP_LOG(WspLog::Write(_L("CWinSockProtocolFamily::NewL")));
	return new(ELeave) CWinSockProtocolFamily();
	}

TInt CWinSockProtocolFamily::Install()
	{
	return KErrNone;
	}

CProtocolBase* CWinSockProtocolFamily::NewProtocolL(TUint aSockType, TUint /*aProtocol*/)
	{
	WSP_LOG(WspLog::Printf(_L("CWinSockProtocolFamily::NewProtocolL, socket type: %d"), aSockType));
	if (iWin32Factory == NULL)
		{
		RWin32Factory* factory = new(ELeave) RWin32Factory();
		CleanupStack::PushL(factory);
		User::LeaveIfError(factory->Open());
		CleanupStack::Pop(factory);
		iWin32Factory = factory;
		}
	return CWinSockProtocol::NewL(aSockType, *iWin32Factory);
	}

TUint CWinSockProtocolFamily::ProtocolList(TServerProtocolDesc*& aProtocolList)
	{
	TBool realTcpipActive(EFalse);
	TRAP_IGNORE(realTcpipActive = RealTcpipActiveL());
	const TInt numProtocols = realTcpipActive ? 2 : 5;
	TRAPD(ret, aProtocolList=new(ELeave) TServerProtocolDesc[numProtocols]);
	if (ret != KErrNone)
		{
		return 0;
		}

	CWinSockProtocol::ProtocolIdentity(&aProtocolList[0], KSockStream, KProtocolWinsockTcp);
	CWinSockProtocol::ProtocolIdentity(&aProtocolList[1], KSockDatagram, KProtocolWinsockUdp);
	if (!realTcpipActive)
		{
		CWinSockProtocol::ProtocolIdentity(&aProtocolList[2], KSockStream, KProtocolInetTcp);
		CWinSockProtocol::ProtocolIdentity(&aProtocolList[3], KSockDatagram, KProtocolInetUdp);
		CWinSockProtocol::ProtocolIdentity(&aProtocolList[4], KSockDatagram, KProtocolInetIp);
		}
	
	return numProtocols;
	}

TBool CWinSockProtocolFamily::RealTcpipActiveL() const
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TUint att;
	_LIT(KTcpipEskFileName, "c:\\Private\\101f7989\\esock\\ip.tcpip.esk");
	TBool active = (fs.Att(KTcpipEskFileName, att) == KErrNone);
	CleanupStack::PopAndDestroy(&fs);
	return active;
	}
