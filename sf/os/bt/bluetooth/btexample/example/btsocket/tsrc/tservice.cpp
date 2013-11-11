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

#include "cbtservice.h"

class CMySCOService;
class CMyService: public CBase, MConnectionObserver, MBluetoothSocketNotifier
	{
	public:
		void ConstructL();
		~CMyService();
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
		CBtService* iBtService;
		RSdp iSdpSession;
		RSocketServ iSocketServ;
		CBluetoothSocket* iConnection;
		CMySCOService* iSCOConnection;
	};

class CMySCOService: MBluetoothSynchronousLinkNotifier
	{
	public:
		void ConstructL();
		CMySCOService(RSocketServ& aSocketServ);
		~CMySCOService();
		//virtual functions from MBluetoothSynchronousLinkNotifier
		virtual void HandleSetupConnectionCompleteL(TInt aErr);
		virtual void HandleDisconnectionCompleteL(TInt aErr);
		virtual void HandleAcceptConnectionCompleteL(TInt aErr);
		virtual void HandleSendCompleteL(TInt aErr);
		virtual void HandleReceiveCompleteL(TInt aErr);
		CBluetoothSynchronousLink *iConnection;
		RSocketServ& iSocketServ;
	};

RTest test(_L("tservice"));
CMyService *TheService;

void CMyService::ConstructL()
	{
	User::LeaveIfError(iSocketServ.Connect());
	iSCOConnection = new(ELeave) CMySCOService(iSocketServ);
	iSCOConnection->ConstructL();
	User::LeaveIfError(iSdpSession.Connect());
	TBTServiceSecurity sec;
	sec.SetAuthentication(EMitmNotRequired);
	sec.SetAuthorisation(EFalse);
	sec.SetEncryption(EFalse);
	sec.SetDenied(EFalse);
	iBtService = CBtService::NewL(TUUID(KSerialPortUUID), //register as a serial port so we can be easily seen
								iSdpSession,
								iSocketServ,
								*this,
								KRFCOMM,
								&sec);
	iConnection = CBluetoothSocket::NewL(*this, iSocketServ);
	iBtService->AcceptConnection(*iConnection);
	}

CMyService::~CMyService()
	{
	delete iBtService;
	delete iConnection;
	delete iSCOConnection;
	iSdpSession.Close();
	iSocketServ.Close();
	}

void CMyService::HandleNewConnection(CBluetoothSocket* aConnection)
	{
	test(aConnection==iConnection);
	delete iConnection;
	iConnection = NULL;
	CActiveScheduler::Stop();
	}

void CMyService::HandleConnectFailed(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleConnectCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleAcceptCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleShutdownCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleSendCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleReceiveCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleIoctlCompleteL(TInt /*aErr*/)
	{
	test(0);
	}

void CMyService::HandleActivateBasebandEventNotifierCompleteL(TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
	{
	test(0);
	}

CMySCOService::CMySCOService(RSocketServ& aSS) :
	iSocketServ(aSS)
	{
	}

CMySCOService::~CMySCOService()
	{
	delete iConnection;
	}

void CMySCOService::ConstructL()
	{
	iConnection = CBluetoothSynchronousLink::NewL(*this, iSocketServ);
	iConnection->AcceptConnection();
	}

void CMySCOService::HandleSetupConnectionCompleteL(TInt /*aErr*/)
	{
	test(0);
	}
void CMySCOService::HandleDisconnectionCompleteL(TInt /*aErr*/)
	{
	}
void CMySCOService::HandleAcceptConnectionCompleteL(TInt aErr)
	{
	test(aErr == KErrNone);
	}
void CMySCOService::HandleSendCompleteL(TInt /*aErr*/)
	{
	test(0);
	}
void CMySCOService::HandleReceiveCompleteL(TInt /*aErr*/)
	{
	test(0);
	}


void RunTestL()
	{
	CActiveScheduler *sched = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	TheService = new(ELeave) CMyService;
	CleanupStack::PushL(TheService);
	TheService->ConstructL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(TheService);
	CleanupStack::PopAndDestroy(sched);
	}

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
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

