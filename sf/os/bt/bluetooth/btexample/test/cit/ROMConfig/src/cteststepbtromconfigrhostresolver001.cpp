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

#include "cteststepbtromconfigrhostresolver001.h"
#include <bt_sock.h>
#include <bluetoothav.h>

CTestStepBtRomConfigRHostResolver001::~CTestStepBtRomConfigRHostResolver001()
	{
	if ( iSocketServ.Handle() )
		{
		iSocketServ.Close();
		}
    }
	
CTestStepBtRomConfigRHostResolver001::CTestStepBtRomConfigRHostResolver001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRHostResolver001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRHostResolver001* CTestStepBtRomConfigRHostResolver001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRHostResolver001(aParent); 
	}

/**
See BT_ROMCONFIG_RHOSTRESOLVER_001.script
*/
TVerdict CTestStepBtRomConfigRHostResolver001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRHostResolver001::doTestStepL()"));
	
	User::LeaveIfError(iSocketServ.Connect());
	
	TestOpenHostResolver( KRFCOMM );
	TestOpenHostResolver( KL2CAP );
	TestOpenHostResolver( KBTLinkManager );
	TestOpenHostResolver( KSDP );
	TestOpenHostResolver( KAVCTP );
	TestOpenHostResolver( KAVDTP );
	
	// RConnection overload APIs aren't tested as we can't start an appropriate
	// RConnection if BT is excluded from the ROM

	iSocketServ.Close();
	
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRHostResolver001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigRHostResolver001::TestOpenHostResolver(TUint aProtocolNumber)
	{
	TInt expectedError = ( iBtExcluded ?  KErrBadName : KErrNotSupported );
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNotSupportedLit );
	// N.B. All but BTLinkManager should return KErrNotSupported if BT is still in the ROM
	if ( KBTLinkManager==aProtocolNumber && !iBtExcluded )
		{
		expectedError = KErrNone;
		expectedErrorDesPtr = &KErrNoneLit;
		}
		
	RHostResolver resolver;
	TInt err = resolver.Open(iSocketServ, KBTAddrFamily, aProtocolNumber);
	if ( err!=expectedError )
		{
		INFO_PRINTF5(\
			_L("Failed: Expected %S(%d) and got %d when calling RHostResolver::Open for BT protocol %u"),\
			expectedErrorDesPtr, expectedError, err, aProtocolNumber);
		SetTestStepResult(EFail);
		}
	if ( resolver.SubSessionHandle() )
		{
		resolver.Close();
		}
	}
	
// EOF
