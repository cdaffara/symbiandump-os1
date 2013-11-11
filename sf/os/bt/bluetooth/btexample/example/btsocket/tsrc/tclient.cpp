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

#include <e32test.h>

#ifdef __EPOC32__
#include <c32comm.h>
#endif

#include "cbtclient.h"

class CMyClient: public CBase, MConnectionObserver, MBluetoothSocketNotifier
	{
	public:
		void ConstructL();
		~CMyClient();
		virtual void HandleNewConnection(CBluetoothSocket* aConnection);
		virtual void HandleConnectFailed(TInt aError);
		//Virtual functions from MBluetoothSocketNotifier
		virtual void HandleConnectCompleteL(TInt aErr);
		virtual void HandleAcceptCompleteL(TInt aErr);
		virtual void HandleShutdownCompleteL(TInt aErr);
		virtual void HandleSendCompleteL(TInt aErr);
		virtual void HandleReceiveCompleteL(TInt aErr);
		virtual void HandleIoctlCompleteL(TInt aErr);
		virtual void HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);
		CBtClient* iBtClient;
		RSdp iSdpSession;
		RSocketServ iSocketServ;
		CBluetoothSocket* iConnection;
	};

RTest test(_L("tClient"));
CMyClient *TheClient;

void CMyClient::ConstructL()
	{
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iSdpSession.Connect());
	TBTServiceSecurity sec;
	sec.SetAuthentication(EMitmNotRequired);
	sec.SetAuthorisation(EFalse);
	sec.SetEncryption(EFalse);
	sec.SetDenied(EFalse);
	iBtClient = CBtClient::NewL(TUUID(KSerialPortUUID), //register as a serial port so we can be easily seen
								iSocketServ,
								*this,
								&sec);
	iBtClient->ConnectToRemoteDeviceL(*this);
	}

CMyClient::~CMyClient()
	{
	delete iBtClient;
	delete iConnection;
	iSdpSession.Close();
	iSocketServ.Close();
	}

void CMyClient::HandleNewConnection(CBluetoothSocket* aConnection)
	{
	test(iConnection==NULL);
	iConnection = aConnection;
	}

void CMyClient::HandleConnectFailed(TInt /*aErr*/)
	{
	test(0);
	}

_LIT8(KTestData, "tclient 12345678");
void CMyClient::HandleConnectCompleteL(TInt aErr)
	{
	test(aErr == KErrNone);
	iConnection->Send(KTestData, 0);
	}

void CMyClient::HandleAcceptCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyClient::HandleShutdownCompleteL(TInt /*aErr*/)
	{
	delete iConnection;
	iConnection = NULL;
	User::After(10000000);
	CActiveScheduler::Stop();
	}

void CMyClient::HandleSendCompleteL(TInt aErr)
	{
	test(aErr == KErrNone);
	iConnection->Shutdown(RSocket::EImmediate);
	}

void CMyClient::HandleReceiveCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyClient::HandleIoctlCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyClient::HandleActivateBasebandEventNotifierCompleteL(TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
	{
	test(0);
	}

void RunTestL()
	{
	CActiveScheduler *sched = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	TheClient = new(ELeave) CMyClient;
	CleanupStack::PushL(TheClient);
	TheClient->ConstructL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TheClient);
	CleanupStack::PopAndDestroy(sched);
	}

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

//this is not needed with a UI, only text shell
void LoadLDD_PDD()
	{
	TInt r;
#ifdef __EPOC32__
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
#endif
	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded LDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded PDD\n"));
	}


TInt E32Main()
	{
	User::SetJustInTime(ETrue);
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	LoadLDD_PDD();
	TRAPD(err,RunTestL());	//	Ignore err

	if (err != KErrNone)
		{
		test.Printf(_L("Error %d"), err);
		test.Getch();
		}

	test.Close();
	delete cleanupStack;
	__UHEAP_MARKEND;
   	
	return err;
	}

