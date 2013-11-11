// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Test code for HCI Proxy class (adapted from TRFCOMM)
// 
//

#include <e32test.h>
#include <es_sock.h>
#include <f32file.h>
#include <bt_sock.h>
#include <c32comm.h>
#include <btmanclient.h>
#include "debug.h"
#include <btsdp.h>
#include <hciproxy.h>
#include <bluetooth/hci/hciopcodes.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
#endif

// Test Ioctl definitions
//const static TUint KRFCOMMFConIoctl =	0x7FFFFFFF;
//const static TUint KRFCOMMFCoffIoctl =	0x7FFFFFFE;

TBuf8<1024> buf;
TBuf16<2048> display;

GLDEF_D RTest test(_L("HCI Proxy module tests"));

const TUint KEventTimeout = 5000000;


void LoadLDD_PDD()
	{
	TInt r;
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
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



void TestMenuL()
	{
	test.SetLogged(EFalse);
	test.Title();
	test.Start(_L("Loading RFCOMM Protocol Module"));

	// This function is essential!
	LoadLDD_PDD();
	// For some reason, you have to do the following to
	// ensure that the file server behaves properly.


	}

CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	// Try to create a console of our preferred size, otherwise we
	// guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		// If we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}

/******************************************************************/
class CVendorSpecificEventTimout;

class CDebugEventConsole : public CBase, private MVendorSpecificHciConduit
	{
	friend class CVendorSpecificEventTimout;
public:
	static CDebugEventConsole* NewL();

	~CDebugEventConsole();

private:
	void ConstructL();
	CDebugEventConsole();
	virtual void CommandCompleted(TInt aError);
	virtual TBool ReceiveEvent(TDesC8& aEvent, TInt aError);
	
	void KillTest();
	void StartVdTestCase();
	void VerifyVdTestCase(TUint aVdTcNum, const TDesC8& aData);

	static const TUint8 VendorDebugTC1[];
	static const TUint8 VendorDebugTC2[];
	static const TUint8 VendorDebugTC3[];
	static const TUint8 VendorDebugTC4[];
	
	typedef struct
		{
		const TUint8* iData;
		TUint iSize;
		}
	TVendorDebugTC;
	
	static const TVendorDebugTC VendorDebugTestCases[];
	static const TUint KNumVdTestCases;
	
	static TInt NextVendorDebugTestCaseCallbackFun(TAny *aPtr);

	CAsyncCallBack				iNextVdTcCallBack;
	
	//  If true, CommandCompleted will start Vendor Debug test case n+1 when n completes.
	TBool						iVdTestCasesRunChained;
	//  The number of currently executing Vendor Debug test case. 1-based.
	TUint						iVdTcNum;
	CConsoleBase*				iConsole;
	CHciExtensionConduit*		iConduit;
	CVendorSpecificEventTimout* iEventTimeout;
	};

NONSHARABLE_CLASS(CVendorSpecificEventTimout) : public CTimer
	{
public:
	virtual ~CVendorSpecificEventTimout();
	static CVendorSpecificEventTimout* NewL(CDebugEventConsole* aContext);
	virtual void CancelTimer();
	virtual void StartTimer();
private:
	void RunL();
	CVendorSpecificEventTimout(CDebugEventConsole* aContext);
	void ConstructL();
private:
	CDebugEventConsole* iClient;
	};


/******************************************************************/


const TUint KLastTestComplete = KMaxTUint;

const TUint8 CDebugEventConsole::VendorDebugTC1[] =
	{
	1, 2, 3,
	};

const TUint8 CDebugEventConsole::VendorDebugTC2[] =
	{
	0
	};

// 250 bytes. If you add 3 bytes of fixed parameters in the Command Completion Event,
// you get 253 bytes of data. We should be able to handle 255 bytes of data,
// but the conduit's ioctl buffer is too small for 255 bytes in Command Completion Events
// - the buffer was meant for commands and then abused for returned completion events,
// and it's 2 bytes too short for them (because you also get 2 bytes extra of packet headers).
const TUint8 CDebugEventConsole::VendorDebugTC3[] =
	{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, //0xA, 0xB, 0xC, 0xD, 0xE
	};

const TUint8 CDebugEventConsole::VendorDebugTC4[] =
	{
	'i', 't', ' ', 'w', 'o', 'r', 'k', 's', '!'
	};

const CDebugEventConsole::TVendorDebugTC CDebugEventConsole::VendorDebugTestCases[] =
	{
	{ VendorDebugTC1, sizeof VendorDebugTC1 },
	{ VendorDebugTC2, sizeof VendorDebugTC2 },
	{ VendorDebugTC3, sizeof VendorDebugTC3 },
	{ VendorDebugTC4, sizeof VendorDebugTC4 }
	};

const TUint CDebugEventConsole::KNumVdTestCases =
	sizeof CDebugEventConsole::VendorDebugTestCases / sizeof CDebugEventConsole::VendorDebugTestCases[0];

	
CDebugEventConsole* CDebugEventConsole::NewL()
	{
	CDebugEventConsole* self=new (ELeave)CDebugEventConsole();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDebugEventConsole::~CDebugEventConsole()
	{
	delete iConsole;
	delete iConduit;
	delete iEventTimeout;
	}

void CDebugEventConsole::ConstructL()
	{
	
	TUint8 Extension[] = //returns response event
			{
			0x99 //...DUMMY VALUE:	Insert extension byte sequence here...
			};
	
/*	TUint8 ActivateInfo[] = //returns response event and couple of extension events
			{
			99 //...DUMMY VALUE:	Insert activation byte sequence here...
			};
	
	TUint8 DeActivateInfo[] = //returns response event and couple of extension events
			{
			99 //...DUMMY VALUE:	Insert deactivation byte sequence here...
			};

	TUint8 ActivateSequence[] = //HCI extension events sent once per second until deactivation
			{
			99 //...DUMMY VALUE:	Insert sequence activation byte sequence here...
			};

	TUint8 DeActivateSequence[] = //deactivate ActivateSequence - no more events!
			{
			99 //...DUMMY VALUE:	Insert sequence deactivation byte sequence here...
			};
*/
	
	TBuf8<100> ExtensionCommand;
	ExtensionCommand.Append(Extension, sizeof(Extension));

//	TPtrC8 ExtensionCommand(Extension, sizeof(Extension));
//	TPtrC8 ExtensionCommand(ActivateInfo, 3);
//	TPtrC8 ExtensionCommand(DeActivateInfo, sizeof(DeActivateInfo));
//	TPtrC8 ExtensionCommand(ActivateSequence, sizeof(ActivateSequence));
//	TPtrC8 ExtensionCommand(DeActivateSequence, sizeof(DeActivateSequence));

	iConsole=AutoSizeNewL(_L("listening for Events..."),TSize(50,40));
	iConduit = CHciExtensionConduit::NewL(*this);
	iEventTimeout = CVendorSpecificEventTimout::NewL(this);

	//  Get the VendorSpecificDebug Command test cases going.
	//  They will run in chain, until each test case from VendorDebugTestCases[] has executed.
	iVdTestCasesRunChained = ETrue;
	iVdTcNum = 0;
	iConsole->Printf(_L("\"No Hardware Response\" Test Case\n"));
	_LIT8(KDummyString, "Dummy");
	iConduit->IssueCommandL(KVendorDebugOGF | 0x00, KDummyString());
	}

CDebugEventConsole::CDebugEventConsole()
	: iNextVdTcCallBack(TCallBack(NextVendorDebugTestCaseCallbackFun, this), CActive::EPriorityStandard)
	{
	}

void CDebugEventConsole::KillTest()
	{
	iConsole->Printf(_L("ALL TESTS HAVE COMPLETED!\nPRESS ANY KEY TO CLOSE TProxy\n"));
	iConsole->Getch();
	CActiveScheduler::Stop();
	return;
	}

void CDebugEventConsole::StartVdTestCase()
	{
	TBuf8<256> CmdDsc;
	const TVendorDebugTC& tc = VendorDebugTestCases[iVdTcNum-1];
	if (tc.iSize > 0)
		CmdDsc.Append(tc.iData, tc.iSize);	
	iConsole->Printf(_L("Starting VendorDebug Test Case number %d\n"), iVdTcNum);
	TRAPD(err, iConduit->IssueCommandExpectingCompleteEventL(KVendorDebugOGF | iVdTcNum, CmdDsc));
	if(err)
		{
		iConsole->Printf(_L("Starting VendorDebug Test Case returned error %d\n"),err);
		}

	}

void CDebugEventConsole::VerifyVdTestCase(TUint aVdTcNum, const TDesC8& aData)
	{
	const TVendorDebugTC& tc = VendorDebugTestCases[aVdTcNum-1];
	const TUint KMaxDataSize		= 255;
	const TUint KCommandHeaderSize	= 2;
	const TUint KFixedParamSize		= 3;
	TUint sizeField     = (tc.iSize + KFixedParamSize < KMaxDataSize) ?
						  (tc.iSize + KFixedParamSize) : KMaxDataSize;

	for (int i = 0; i < aData.Length(); i++)
		{
		iConsole->Printf(_L("Received CCE: %x\n"), aData[i]);
		}
	for (int i = 0; i < tc.iSize; i++)
		{
		test(aData[i] == tc.iData[i]);
		}
	return;
	}

TInt CDebugEventConsole::NextVendorDebugTestCaseCallbackFun(TAny* aPtr)
	{
	CDebugEventConsole* self = static_cast<CDebugEventConsole*>(aPtr);
	self->StartVdTestCase();
	return 0;
	}

void CDebugEventConsole::CommandCompleted(TInt aError)
	{
	iConsole->Printf(_L("Cmd Complete: %d\n"), aError);

	TPtrC8 data;
	iConduit->CommandCompleteEventData(data);

	VerifyVdTestCase(iVdTcNum, data);
	
	if (iVdTestCasesRunChained && iVdTcNum < KNumVdTestCases)
		{
		iVdTcNum++;
		iNextVdTcCallBack.CallBack();
		}
	else
		{
		iVdTcNum = KLastTestComplete;
		iConduit->WaitForEvent(); //now see if we have any events queued
		iEventTimeout->StartTimer();
		}
	}

TBool CDebugEventConsole::ReceiveEvent(TDesC8& aEvent, TInt aError)
	{
	iConsole->Printf(_L("Rcvd Event!\n  err %d Length %d\n"), aError, aEvent.Length());
	for (int i = 0; i < aEvent.Length(); i++)
		{
		iConsole->Printf(_L("Received Dbg Evnt: %x\n"), aEvent[i]);
		}
	iEventTimeout->StartTimer(); // restart evnt timeout
	return ETrue; // Keep receiving events
	}

/******************************************************************/
 
CVendorSpecificEventTimout::CVendorSpecificEventTimout(CDebugEventConsole* aContext) : CTimer(EPriorityStandard),
	iClient(aContext)
	{
	}

CVendorSpecificEventTimout::~CVendorSpecificEventTimout()
	{
	CancelTimer();
	}


CVendorSpecificEventTimout* CVendorSpecificEventTimout::NewL(CDebugEventConsole* aContext)
	{
	CVendorSpecificEventTimout* self = new (ELeave) CVendorSpecificEventTimout(aContext);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CVendorSpecificEventTimout::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CVendorSpecificEventTimout::CancelTimer()
	{
	if(IsActive())
		{
		CActive::Cancel();
		}
	}

void CVendorSpecificEventTimout::StartTimer()
	{
	if(IsActive())
		{
		CActive::Cancel();
		}
	After(KEventTimeout);
	}



void CVendorSpecificEventTimout::RunL()
	{
	iClient->KillTest();
	}


/******************************************************************/

void DoEventListenerL()
	{
	CActiveScheduler *exampleScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(exampleScheduler);
	CActiveScheduler::Install(exampleScheduler); 

	CleanupStack::PushL(CDebugEventConsole::NewL());

/*	TInt count=0;
	while(ETrue)
		{
		User::After(1000000);
		myConsole->Printf(_L("Number %d\n"),++count);
		}
*/
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); // active shed, myDebug
	}


TInt AnotherThreadFunction(TAny*)
	{
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	TRAPD(err,DoEventListenerL());
	delete cleanupStack;
	return err;
	}

TInt E32Main()
	{

	CTrapCleanup* cleanupStack=CTrapCleanup::New();

//	RThread thread;
//	thread.Create(_L("Plim"),AnotherThreadFunction,KDefaultStackSize,KMinHeapSize,0x15000,(TAny*)0);
//	thread.Resume();

	TRAP_IGNORE(TestMenuL());	//	Ignore err
	TRAPD(err, DoEventListenerL());

//	thread.Close();

	delete cleanupStack;
   	
	return err;
	}

