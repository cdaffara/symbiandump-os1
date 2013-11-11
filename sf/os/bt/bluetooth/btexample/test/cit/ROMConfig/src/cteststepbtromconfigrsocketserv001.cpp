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

#include "cteststepbtromconfigrsocketserv001.h"
#include <bt_sock.h>
#include <bluetoothav.h>

_LIT(KProtocolSdp, "SDP");
_LIT(KProtocolBtLinkManager, "BTLinkManager");
_LIT(KProtocolAvctp, "AVCTP");

CTestStepBtRomConfigRSocketServ001::~CTestStepBtRomConfigRSocketServ001()
	{
	if ( iSocketServ.Handle() )
		{
		iSocketServ.Close();
		}
    }
	
CTestStepBtRomConfigRSocketServ001::CTestStepBtRomConfigRSocketServ001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRSocketServ001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRSocketServ001* CTestStepBtRomConfigRSocketServ001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRSocketServ001(aParent); 
	}

/**
See BT_ROMCONFIG_RSOCKETSERV_001.script
*/
TVerdict CTestStepBtRomConfigRSocketServ001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRSocketServ001::doTestStepL()"));
	
	User::LeaveIfError(iSocketServ.Connect());

	TestFindProtocol(KRFCOMMDesC);
	TestFindProtocol(KL2CAPDesC);
	TestFindProtocol(KProtocolSdp);
	TestFindProtocol(KProtocolBtLinkManager);
	TestFindProtocol(KProtocolAvctp);
	TestFindProtocol(KAVDTPProtocolName);
	
	iSocketServ.Close();
	
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRSocketServ001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigRSocketServ001::TestFindProtocol(const TDesC& aProtocolName)
	{
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	TProtocolDesc protocolDesc;
	TInt err = iSocketServ.FindProtocol(TProtocolName(aProtocolName), protocolDesc);
	if ( err!=expectedError )
		{
		INFO_PRINTF5(\
			_L("Failed: Expected %S(%d) and got %d when calling FindProtocol for %S"),\
			expectedErrorDesPtr, expectedError, err, &aProtocolName);
		SetTestStepResult(EFail);
		}
	}

// EOF
