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
// BT incoming SCO notification test code
// 
//

#include <e32test.h>
#include <es_sock.h>
#include <f32file.h>
#include <bt_sock.h>
#include <btextnotifiers.h>
#include <c32comm.h>
#include <btsdp.h>
#include "exchange.h"

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART4")	// port 0 on Assabet
// don't use EUART2 - port 1 is IR
#define ETNA_PDD_NAME _L("EUART3") // for port 2 on Assabet
#define LDD_NAME _L("ECOMM")
#endif

GLDEF_D RTest test(_L("bluemeanie tests"));

const TUint32 KServiceUID1 = 0x6D14AC98;
const TUint32 KServiceUID2 = 0x7A8249bb;
const TUint32 KServiceUID3 = 0xB885BE2E;
const TUint32 KServiceUID4 = 0xF73C909E;

//_LIT(KMenuText1, "A - Listen & Accept         F - Connect ACL");
//_LIT(KMenuText2, "B - Await incoming SCO      G - Connect SCO");
//_LIT(KMenuText3, "C - Await SCO disconnect    H - Disconnect SCO");
//_LIT(KMenuText4, "D - Await ACL disconnect    I - Disconnect ACL");
//_LIT(KMenuText5, "E - Receive Data            J - Send Data");
//_LIT(KMenuText6, "K - Hard disconnect ACL");
//_LIT(KMenuText7, "X - Exit");
//_LIT(KNewLine, "\r\n");

//const TBTDevAddr KDevAddr(MAKE_TINT64(0x0002, 0x5bff0a1e));

class CTestBM : public CActive
	{
	public:
		CTestBM();
		void InitL();
		void DoTestL();
		void RunL();
		void DoCancel();
	private:
	private:
		CBluetoothExchanger* iExch;
		CActiveScheduler* iSched;
	};

CTestBM::CTestBM() : CActive(CActive::EPriorityStandard)
	{}

void CTestBM::InitL()
	{
	TUUID uuid(KServiceUID1,KServiceUID2,KServiceUID3,KServiceUID4);
	iSched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);
	iExch = CBluetoothExchanger::NewL(uuid);
	iSched->Add(this);
	}

void CTestBM::DoTestL()
	{
	test.Printf(_L("Press a key to start search\r\n"));
	test.Getch();
	iExch->LookForPeersL(iStatus);
	SetActive();
	iSched->Start();
	test(iStatus == KErrNone);
	test.Printf(_L("Completed with status %d\r\n"), iStatus.Int());
	TInt count;
	TBTSockAddr addr;
	THostName name;
	TNameEntry* entry;
	iExch->First();
	count = iExch->Count();
	test.Printf(_L("%d compatible devices found\r\n"), count);
	while(iExch->Next(entry) == KErrNone)
		{
		addr = (*entry)().iAddr;
		name = (*entry)().iName;
		test.Printf(_L("Device address 0x%02x%02x%02x%02x%02x%02x\r\n"),
			addr.BTAddr()[0],
			addr.BTAddr()[1],
			addr.BTAddr()[2],
			addr.BTAddr()[3],
			addr.BTAddr()[4],
			addr.BTAddr()[5]);
		test.Printf(_L("Device name: %S\r\n"), &name);
		}
	test.Getch();
	}

void CTestBM::RunL()
	{
	iSched->Stop();
	}

void CTestBM::DoCancel()
	{
	//iExch->Cancel();
	}

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


void RunTestL()
	{
	CTestBM *st = new(ELeave) CTestBM;
	CleanupStack::PushL(st);
	LoadLDD_PDD();
	st->InitL();
	st->DoTestL();
	CleanupStack::PopAndDestroy(st);
	}

TInt E32Main()
	{
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	TRAPD(err,RunTestL());

	delete cleanupStack;
   	
	return err;
	}

