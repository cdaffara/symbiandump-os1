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

#include "cteststepbtromconfigrsocket001.h"
#include <es_sock.h>
#include <bt_sock.h>
#include <bluetoothav.h>

_LIT(KProtocolSdp, "SDP");
_LIT(KProtocolBtLinkManager, "BTLinkManager");
_LIT(KProtocolAvctp, "AVCTP");

CTestStepBtRomConfigRSocket001::~CTestStepBtRomConfigRSocket001()
	{
    }

CTestStepBtRomConfigRSocket001::CTestStepBtRomConfigRSocket001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRSocket001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRSocket001* CTestStepBtRomConfigRSocket001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRSocket001(aParent); 
	}

/**
See BT_ROMCONFIG_RSOCKET_001.script
*/
TVerdict CTestStepBtRomConfigRSocket001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRSocket001::doTestStepL()"));
	
	RSocketServ socketServer;
	User::LeaveIfError(socketServer.Connect());
	RSocket sock;
	TInt err = KErrNone;
	TInt expectedError = KErrBadName;
	const TDesC* expectedErrorDesPtr = &KErrBadNameLit;
	TBool allCasesTested = EFalse;
	TInt testcase = 0;

	while( !allCasesTested )
		{
		// test one of the RSocket open methods
		switch(testcase)
			{
			case 0:
				expectedError = ( iBtExcluded ?  KErrNotFound : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrNotFoundLit : &KErrNoneLit );
				err = sock.Open( socketServer, KRFCOMMDesC );
				break;
			case 1: 
				expectedError = ( iBtExcluded ?  KErrNotFound : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrNotFoundLit : &KErrNoneLit );
				err = sock.Open( socketServer, KL2CAPDesC );
				break;
			case 2:
				expectedError = ( iBtExcluded ?  KErrNotFound : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrNotFoundLit : &KErrNoneLit );
				err = sock.Open( socketServer, KProtocolBtLinkManager);
				break;
			case 3:
				expectedError = ( iBtExcluded ?  KErrNotFound : KErrNotSupported );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrNotFoundLit : &KErrNotSupportedLit );
				err = sock.Open( socketServer, KProtocolSdp);
				break;
			case 4:
				expectedError = ( iBtExcluded ?  KErrNotFound : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrNotFoundLit : &KErrNoneLit );
				err = sock.Open( socketServer, KAVDTPProtocolName);
				break;	
			
			case 5:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNoneLit );
			 	err = sock.Open( socketServer, KBTAddrFamily, KSockStream, KRFCOMM );
			 	break;
			case 6:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNoneLit );
				err = sock.Open( socketServer, KBTAddrFamily, KSockSeqPacket, KL2CAP );
				break;
			case 7:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNotSupported );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNotSupportedLit );
			 	err = sock.Open( socketServer, KBTAddrFamily, KSockStream, KSDP );
			 	break;				
			case 8:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNoneLit );
			 	err = sock.Open( socketServer, KBTAddrFamily, KUndefinedSockType, KBTLinkManager );
			 	break;				
			case 9:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNoneLit );
			 	err = sock.Open( socketServer, KBTAddrFamily, KSockDatagram, KAVCTP );
			 	break;				
			case 10:
				expectedError = ( iBtExcluded ?  KErrBadName : KErrNone );
				expectedErrorDesPtr = ( iBtExcluded ?  &KErrBadNameLit : &KErrNoneLit );
			 	err = sock.Open( socketServer, KBTAddrFamily, KSockDatagram, KAVDTP );
			 	break;	
			 
			 /*
			 API overloads taking RConnections do not need to be tested as these are
			 covered by BT-ROMCONFIG-PAN-001
			 */
			 							 				 				 	
			default:
				allCasesTested = ETrue;
				break;
			}
		if ( allCasesTested )	
			{
			break;
			}
		// check the result of the call to Open	
		if ( err!=expectedError )
			{
			INFO_PRINTF5(\
				_L("Subcase %d Failed: Expected error %S(%d) and got %d when calling Open for BT protocol"),\
				testcase, expectedErrorDesPtr, expectedError, err);
			SetTestStepResult(EFail);
			}	
		// may need to tidy up the socket if a test failed	
		if ( sock.SubSessionHandle() )	
			{
			sock.Close();
			}
		testcase++;	
		}
	INFO_PRINTF2(\
		_L("%d subcases for RSocket::Open() were tested"), testcase);
	socketServer.Close();
	
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRSocket001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
