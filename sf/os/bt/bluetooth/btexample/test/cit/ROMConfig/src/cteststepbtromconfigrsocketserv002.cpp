// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cteststepbtromconfigrsocketserv002.h"
#include <bt_sock.h>
#include <bluetoothav.h>

CTestStepBtRomConfigRSocketServ002::~CTestStepBtRomConfigRSocketServ002()
	{
	if ( iSocketServ.Handle() )
		{
		iSocketServ.Close();
		}
    }	

CTestStepBtRomConfigRSocketServ002::CTestStepBtRomConfigRSocketServ002
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRSocketServ002);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRSocketServ002* CTestStepBtRomConfigRSocketServ002::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRSocketServ002(aParent); 
	}

/**
See BT_ROMCONFIG_RSOCKETSERV_002.script
*/
TVerdict CTestStepBtRomConfigRSocketServ002::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRSocketServ002::doTestStepL()"));
	
	User::LeaveIfError(iSocketServ.Connect());
	
	// TODO: What are the packet types for the extra BT protocols?
	TestStartProtocol(KSockStream, KRFCOMM);
	TestStartProtocol(KSockSeqPacket, KL2CAP);
	TestStartProtocol(KSockStream, KSDP);
	TestStartProtocol(KUndefinedSockType, KBTLinkManager);
	TestStartProtocol(KSockDatagram, KAVCTP);
	TestStartProtocol(KSockDatagram, KAVDTP);
	
	iSocketServ.Close();

	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRSocketServ002::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigRSocketServ002::TestStartProtocol(TUint aPacketType, TUint aProtocolNumber)
	{
	const TInt expectedError = ( iBtExcluded ? KErrBadName : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrBadNameLit : &KErrNoneLit ); 
	TRequestStatus stat;
	iSocketServ.StartProtocol( KBTAddrFamily, aPacketType, aProtocolNumber, stat );
	User::WaitForRequest(stat);
	if ( stat.Int()!=expectedError )
		{
		INFO_PRINTF5(\
			_L("Failed: Expected %S(%d) and got %d when calling StartProtocol for BT protocol number %u"),\
			expectedErrorDesPtr, expectedError, stat.Int(), aProtocolNumber);
		SetTestStepResult(EFail);
		}
	}
	
// EOF
