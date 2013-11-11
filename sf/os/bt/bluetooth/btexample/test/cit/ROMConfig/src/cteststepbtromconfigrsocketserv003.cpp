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

#include "cteststepbtromconfigrsocketserv003.h"
#include <es_sock.h>
#include <bt_sock.h>
#include <bluetoothav.h>

CTestStepBtRomConfigRSocketServ003::~CTestStepBtRomConfigRSocketServ003()
	{
    }	

CTestStepBtRomConfigRSocketServ003::CTestStepBtRomConfigRSocketServ003
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRSocketServ003);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRSocketServ003* CTestStepBtRomConfigRSocketServ003::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRSocketServ003(aParent); 
	}

/**
See BT_ROMCONFIG_RSOCKETSERV_003.script
*/
TVerdict CTestStepBtRomConfigRSocketServ003::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRSocketServ003::doTestStepL()"));
	
	RSocketServ socketServer;
	TProtocolDesc protocolDesc;
	User::LeaveIfError(socketServer.Connect());
	
	TUint numProtocols = 0;
	TBool foundBtProtocol = EFalse;
	User::LeaveIfError(socketServer.NumProtocols(numProtocols));
	
	for( TUint index = 1; index <= numProtocols ; index++ )
		{
		User::LeaveIfError(socketServer.GetProtocolInfo( index, protocolDesc ));
		if ( KBTAddrFamily==protocolDesc.iAddrFamily )
			{
			foundBtProtocol = ETrue;
			INFO_PRINTF2(_L("Found BT protocol %S"), &protocolDesc.iName);
			}
		}
	
	if ( (foundBtProtocol && iBtExcluded) || (!foundBtProtocol && !iBtExcluded) )
		{
		SetTestStepResult(EFail);
		}
	
	socketServer.Close();
	
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRSocketServ003::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
