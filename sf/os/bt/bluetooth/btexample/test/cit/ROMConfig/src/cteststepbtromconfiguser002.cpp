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

#include "cteststepbtromconfiguser002.h"
#include <bluetoothav.h>

/**
   Unnamed namespaces are a superior replacement for the static
   declaration of variables. They allow variables and functions to be
   visible within an entire translation unit, yet not visible
   externally. Although entities in an unnamed namespace might have
   external linkage, they are effectively qualified by a name unique
   to their translation unit and therefore can never be seen from any
   other translation unit.
*/
namespace
	{
	/** 
		Panic string if an invalid test subcase is specified Panic
		number is the invalid subcase
	*/
	_LIT(KInvalidTestPanicString, "Invalid Test subcase specified");
	_LIT(KUnexpectedError, "Unexpected error");
	_LIT(KProtocolSdp, "SDP");
	_LIT(KProtocolBtLinkManager, "BTLinkManager");
	_LIT(KProtocolAvctp, "AVCTP");
	}

CTestStepBtRomConfigUser002::~CTestStepBtRomConfigUser002()
	{
	if (iSocketServ.Handle())
		{
		iSocketServ.Close();
		}
	}

CTestStepBtRomConfigUser002::CTestStepBtRomConfigUser002(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameUser002);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory
will return NULL;
*/
CTestStepBtRomConfigUser002* 
CTestStepBtRomConfigUser002::New(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigUser002(aParent); 
	}

/**
See BT_ROMCONFIG_USER_002.script
*/
TVerdict CTestStepBtRomConfigUser002::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CTestStepBtRomConfigUser002::doTestStepL()"));
	
	CActiveScheduler* sched(new (ELeave)CActiveScheduler);
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);

	User::LeaveIfError(iSocketServ.Connect());
	
	TInt expectedError(KErrNotSupported);

	const TInt numTestCases(25);
	
	for (TInt testCase(0); testCase < numTestCases; ++testCase)
		{
		TRAPD(err, TestBluetoothSocketConstructorL(testCase, expectedError));
		
		if (expectedError != err)
			{
			// this is only used for logging readability purposes
			const TDesC* expectedErrorDesPtr(0);

			switch (expectedError)
				{
			case KErrNotFound:
				expectedErrorDesPtr = &KErrNotFoundLit;
				break;
			case KErrNone:
				expectedErrorDesPtr = &KErrNoneLit;
				break;
			case KErrBadName:
				expectedErrorDesPtr = &KErrBadNameLit;
				break;
			case KErrNotSupported:
				expectedErrorDesPtr = &KErrNotSupportedLit;
				break;
			default:
				User::Panic(KUnexpectedError, testCase);
				}

			INFO_PRINTF5(_L("Failed: Expected %S(%d) and got %d calling CBluetoothSocket constructor case %d"), 
						 expectedErrorDesPtr, expectedError, err, testCase);
			SetTestStepResult(EFail);
			}
		}
	
	INFO_PRINTF2(_L("Tested %d cases"), numTestCases);
	
	iSocketServ.Close();
	
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(_L("&lt;&lt;CTestStepBtRomConfigUser002::doTestStepL()"));
	
	CheckAndSetTestResult();
	return TestStepResult();
	}

void CTestStepBtRomConfigUser002::
TestBluetoothSocketConstructorL(TInt aTestSubCase, TInt& aExpectedError)
	{
		
	// must set the expected error BEFORE the test calls a leaving function
	CBluetoothSocket* btsocket(0);
	switch(aTestSubCase)
		{
	case 0:
		{
		// Even if BT is missing from the ROM, this test will succeed.
		// This is because we are only passing a hint to the socket server,
		// not actually instantiating a Bluetooth connection
		aExpectedError = KErrNone; 
		
		RSocketServ socketServ;
		TSessionPref pref;
		
		// ToDo Check that these are correct
		pref.iAddrFamily = KSockStream;
		pref.iProtocol = KRFCOMM;

		User::LeaveIfError(socketServ.Connect(pref));
		if (socketServ.Handle())
			{
			socketServ.Close();
			}
		}
		break;

	case 1:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KSockStream, KRFCOMM);
		delete btsocket;
		break;

	case 2:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KSockSeqPacket, KL2CAP);
		delete btsocket;
		break;
	
	case 3:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNotSupported;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KSockStream, KSDP);
		delete btsocket;
		break;	

	case 4:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KUndefinedSockType, KBTLinkManager);
		delete btsocket;
		break;
		
	case 5:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KSockDatagram, KAVCTP);
		delete btsocket;
		break;					
		
	case 6:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KSockDatagram, KAVDTP);
		delete btsocket;
		break;						
			
	case 7:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KSockStream, KRFCOMM);
		CleanupStack::PopAndDestroy(btsocket);
		break;

	case 8:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KSockSeqPacket, KL2CAP);
		CleanupStack::PopAndDestroy(btsocket);
		break;
	
	case 9:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNotSupported;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KSockStream, KSDP);
		CleanupStack::PopAndDestroy(btsocket);
		break;	

	case 10:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KUndefinedSockType, KBTLinkManager);
		CleanupStack::PopAndDestroy(btsocket);
		break;
		
	case 11:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KSockDatagram, KAVCTP);
		CleanupStack::PopAndDestroy(btsocket);
		break;					
		
	case 12:
		aExpectedError = iBtExcluded ? KErrBadName : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										  KSockDatagram, KAVDTP);
		CleanupStack::PopAndDestroy(btsocket);
		break;	
		
	case 13: // use the protocol descriptor from bt_sock
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, KRFCOMMDesC);
		delete btsocket;
		break;		

	case 14: // use the protocol descriptor from bt_sock
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, KL2CAPDesC);
		delete btsocket;
		break;		
		
	case 15:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNotSupported;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, KProtocolSdp);
		delete btsocket;
		break;				

	case 16:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KProtocolBtLinkManager);
		delete btsocket;
		break;				

	case 17:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewL(*this, iSocketServ, 
										  KAVDTPProtocolName);
		delete btsocket;
		break;				

	case 18: // use the protocol descriptor from bt_sock
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, KRFCOMMDesC);
		CleanupStack::PopAndDestroy(btsocket);
		break;		

	case 19: // use the protocol descriptor from bt_sock
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, KL2CAPDesC);
		CleanupStack::PopAndDestroy(btsocket);
		break;		
		
	case 20:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNotSupported;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, KProtocolSdp);
		CleanupStack::PopAndDestroy(btsocket);
		break;				

	case 21:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										   KProtocolBtLinkManager);
		CleanupStack::PopAndDestroy(btsocket);
		break;				

	case 22:
		aExpectedError = iBtExcluded ? KErrNotFound : KErrNone;

		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ, 
										   KAVDTPProtocolName);
		CleanupStack::PopAndDestroy(btsocket);
		break;			

	case 23:
		aExpectedError = KErrNone; // even with no BT, this will succeed 
		btsocket = CBluetoothSocket::NewL(*this, iSocketServ);
		// Even though we can create a blank (CBluetooth)Socket, this isn't
		// of any use without an instantiated Bluetooth socket, so it doesn't
		// make sense to do any further API calls with this
		delete btsocket;
		break;

	case 24:
		aExpectedError = KErrNone; // even with no BT, this will succeed
		btsocket = CBluetoothSocket::NewLC(*this, iSocketServ);
		// Even though we can create a blank (CBluetooth)Socket, this isn't
		// of any use without an instantiated Bluetooth socket, so it doesn't
		// make sense to do any further API calls with this
		CleanupStack::PopAndDestroy(btsocket);
		break;		

	/*
	CBluetoothSocket* NewL and NewLC taking an RConnection does not
	have to be tested, the RConnection cannot be started over
	Bluetooth BT_ROMCONFIG_PAN_001 confirms that this is not possible
	without BT on ROM
	*/	
	
	/* CBluetoothSocket* NewL and NewLC taking parameters
	(MBluetoothSocketNotifier &aNotifier, RSocketServ &aServer,
	RSocket &aSocket); does not have to be tested, as the API states:
	"This should be used where an existing API returned a RSocket
	representing a Bluetooth connection."  BT_ROMCONFIG_RSOCKET_001
	confirms that this is not possible without BT on ROM
	*/	

	default:
		User::Panic(KInvalidTestPanicString, aTestSubCase);
		break;
		}
	}

// stubs
void CTestStepBtRomConfigUser002::HandleConnectCompleteL(TInt/*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser002::HandleAcceptCompleteL(TInt/*aErr*/)
	{
	}
		
void CTestStepBtRomConfigUser002::HandleShutdownCompleteL(TInt/*aErr*/)
	{
	}
		
void CTestStepBtRomConfigUser002::HandleSendCompleteL(TInt/*aErr*/)
	{
	}
		
void CTestStepBtRomConfigUser002::HandleReceiveCompleteL(TInt/*aErr*/)
	{
	}
		
void CTestStepBtRomConfigUser002::HandleIoctlCompleteL(TInt/*aErr*/)
	{
	}
		
void CTestStepBtRomConfigUser002::
HandleActivateBasebandEventNotifierCompleteL(TInt, 
											 TBTBasebandEventNotification&)
	{
	}	
	
// EOF
