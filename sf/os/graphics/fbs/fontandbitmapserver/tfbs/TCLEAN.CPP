// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../sfbs/UTILS.H"
#include "TFBS.H"
#include "TCLEAN.H"
#include "fbsmessage.h"
#include "fbsdefs.h"

void CTClean::DeleteScanLineBuffer()
	{
	delete iFbs->iScanLineBuffer;
	iFbs->iScanLineBuffer = NULL;
	}

CTClean::CTClean(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL),
	iStore(NULL),
	iBitmap(NULL),
	iTc(NULL)
	{}

CTClean::~CTClean()
	{
	delete iStore;
	if(iShutdownFbs)
		RFbsSession::GetSession()->SendCommand(EFbsMessShutdown);
	RFbsSession::Disconnect();
	}

void CTClean::ConstructL()
	{
	INFO_PRINTF1(_L("Font/Bitmap Server Thread Death Cleanup Testing"));

	if(RFbsSession::Connect()==KErrNone)
		{
		RFbsSession::Disconnect();
		iShutdownFbs = EFalse;
		}
	else
		{
		FbsStartup();
		iShutdownFbs = ETrue;
		}
	RFbsSession::Connect();
	iFbs=RFbsSession::GetSession();
	iStore=CFbsTypefaceStore::NewL(NULL);
	}

void CTClean::RunTestCaseL(TInt aCurTestCase)
	{
	((CTCleanStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0593"));
		INFO_PRINTF1(_L("Thread Cleanup\r\n"));
		DeadThread();
		break;
	case 2:
#if defined(_DEBUG)
		INFO_PRINTF1(_L("Font\r\n"));
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0594"));
		LaunchThread(EFont1);
		((CTCleanStep*)iStep)->RecordTestResultL();
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0595"));
		LaunchThread(EFont2);
#else
		((CTCleanStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
#endif
		break;
	case 3:
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0596"));
		INFO_PRINTF1(_L("Bitmap\r\n"));
		LaunchThread(EBitmap1);
		break;
	case 4:
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0597"));
		INFO_PRINTF1(_L("Pile\r\n"));
		LaunchThread(EPile1);
		break;
	case 5:
		INFO_PRINTF1(_L("Session\r\n"));
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0598"));
		LaunchThread(ESession1);
		((CTCleanStep*)iStep)->RecordTestResultL();
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0599"));
		LaunchThread(ESession2);
		break;
	case 6:
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0600"));
		INFO_PRINTF1(_L("TypefaceStore\r\n"));
		LaunchThread(ETypefaceStore1);
		break;
	case 7:
		((CTCleanStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0601"));
		INFO_PRINTF1(_L("Bad Message\r\n"));
		LaunchThread(EBadMessage);
	case 8:
		((CTCleanStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTCleanStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTCleanStep*)iStep)->RecordTestResultL();
	}

TInt CleanupTestThread(TAny* aAny)
	{
	CTClean* theTest = (CTClean*) aAny;
	CTrapCleanup::New();
	RFbsSession::Connect();
	CFbsBitmapEx bmp1;
	CFbsBitmapEx bmp2;
	CFbsBitmapEx bmp3;
	TInt ret=bmp1.Create(TSize(10,10),EGray2);
	theTest->TEST(ret==KErrNone);
	ret=bmp2.Create(TSize(100,100),EGray16);
	theTest->TEST(ret==KErrNone);
	ret=bmp3.Duplicate(bmp2.Handle());
	theTest->TEST(ret==KErrNone);
	CFbsFontEx* fon1;
	CFbsFontEx* fon2;
	CFbsTypefaceStore* tfs=NULL;
	TRAP(ret,tfs=CFbsTypefaceStore::NewL(NULL));
	theTest->TEST(ret==KErrNone);
	ret=tfs->GetNearestFontToDesignHeightInPixels((CFont*&)fon1,TFontSpec(_L("Swiss"),12));
	theTest->TEST(ret==KErrNone);
	fon2=(CFbsFontEx*)User::Alloc(sizeof(CFbsFontEx));
	theTest->TEST(fon2!=NULL);
	new(fon2) CFbsFontEx;
	ret=fon2->Duplicate(fon1->Handle());
	theTest->TEST(ret==KErrNone);
	TInt rc=RFbsSession::GetSession()->ResourceCount();
	theTest->TEST(rc==5);
	TInt id;
#ifdef __WINS__
	ret=tfs->AddFile(_L("z:\\resource\\fonts\\eon14.gdr"),id);
#else
	ret=tfs->AddFile(_L("z:\\resource\\fonts\\eon.gdr"),id);
#endif
	theTest->TEST(ret==KErrNone);
	bmp2.LockHeap();
	RFbsSession::Disconnect();
	User::Panic(_L("CleanupTest"),KErrNone);
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0593

	@SYMTestCaseDesc
	Tests the cleaning when a thead is panicing.

	@SYMTestActions
	Creates a bitmap. Checks the resource count. Creates
	a thread with high priority. Unlocks the heap and locks
	it back again. Disables just-in-time debugging. Waits for 
	he thead to complete its execution. The created thread creates
	its own cleanup stack and connects to a FbsSession. It creates 
	three bitmaps and duplicates one of them to the other. 
	A FbsTypefaceStore object is created on the heap an 
	GetNearestFontToDesignHeightInPixel is called on it. Disconnect
	from the FbsSession. The thread is paniced and terminated. The
	function that created the thread now continues its execution by
	checking the exit reason for the thread. Closes the thead. Enables
	just-in-time again. Locks the heap and unlocks it again.
	Checks the resource count. Deletes scan line buffer. The
	heap is checked for memory leaks.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTClean::DeadThread()
	{
	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	TInt ret=bmp.Create(TSize(100,100),EGray16);
	TEST(ret==KErrNone);
	TInt rc=iFbs->ResourceCount();
	TEST(rc==1);
	RThread thrd;
	TRequestStatus stat;
	thrd.Create(_L("ctt"),CleanupTestThread,KDefaultStackSize,0x8000,0x8000,this);
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	User::SetJustInTime(EFalse);
	bmp.LockHeap();
	bmp.UnlockHeap();
	thrd.Resume();
	User::WaitForRequest(stat);
	ret = thrd.ExitReason();
	thrd.Close();
	User::SetJustInTime(ETrue);
	TEST(ret == KErrNone);
	INFO_PRINTF1(_L("About to lock heap\r\n"));
	bmp.LockHeap();
	bmp.UnlockHeap();
	rc=iFbs->ResourceCount();
	TEST(rc==1);
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0594

	@SYMTestCaseDesc
	Tests a CFbsFontEx object in a thead function.
	
	@SYMTestActions
	Connects to an FbsSession, creates a CFbsFontEx
	object and gets the font address.
	Thread returns with error code KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Font1(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	theTest->INFO_PRINTF1(_L("Font1"));
	RFbsSession::Connect();
	CFbsFontEx font;
	(void)font.Address();
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0595

	@SYMTestCaseDesc
	Tests a CFbsFontEx object in a thead function.
	
	@SYMTestActions
	Connects to an FbsSession, creates a CFbsFontEx object,
	sets the fonts handle, gets the fonts address.
	Thread returns with error code KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Font2(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	theTest->INFO_PRINTF1(_L("Font2"));
	RFbsSession::Connect();
	CFbsFontEx font;
	font.SetHandle();
	(void)font.Address();
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0596

	@SYMTestCaseDesc
	Creates a bitmap in a thread function, locks the heap
	and panics the thread.
	
	@SYMTestActions
	Connects to an FbsSession, creates a CFbsBitmapEx object,
	creates a bitmap on the stack. Creation is checked for errors
	and thenb locks the heap. The thread is paniced.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Bitmap1(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	theTest->INFO_PRINTF1(_L("Bitmap1"));
	RFbsSession::Connect();
	CFbsBitmapEx bitmap;
	TInt ret=bitmap.Create(TSize(200,200),EGray16);
	if(ret!=KErrNone)
		{
		theTest->ERR_PRINTF1(_L("ERROR - BITMAP NOT CREATED"));
		}
	bitmap.LockHeap();
	User::Panic(_L(""),KErrNone);
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0597

	@SYMTestCaseDesc
	Tests creation of a chunk in a thread function. 

	@SYMTestActions
	Creates a chunk. The thread returns with error code KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Pile1(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	theTest->INFO_PRINTF1(_L("Pile1"));
	RChunk chunk;
	CChunkPile::NewL(chunk);
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0598

	@SYMTestCaseDesc
	Tests sending a close command to a connected
	server from a thread function.

	@SYMTestActions
	Connects to a FbsSession and sends a
	EFbsMessClose-command to the server. The thread
	returns with error code KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Session1(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	RFbsSession::Connect();
	RFbsSession::GetSession()->SendCommand(EFbsMessClose);
	theTest->INFO_PRINTF1(_L("Session1"));
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0599

	@SYMTestCaseDesc
	Tests sending an init command to a connected
	server from a thread function.

	@SYMTestActions
	Connects to a FbsSession and sends a
	EFbsMessInit-command to the server. The thread
	returns with error code KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt Session2(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	RFbsSession session;
	session.SendCommand(EFbsMessInit);
	theTest->INFO_PRINTF1(_L("Session2"));
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0600

	@SYMTestCaseDesc
	Tests the creation of a CFbsTypefaceStore
	object on the heap in a thread function.

	@SYMTestActions
	Creates a CFbsTypefaceStore object on the heap
	in a thread function and returns with error code
	KErrNone.
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt TypefaceStore1(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	CFbsTypefaceStore::NewL(NULL);
	theTest->INFO_PRINTF1(_L("TypefaceStore1"));
	theTest->ERR_PRINTF1(_L("ERROR - NO PANIC"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0601

	@SYMTestCaseDesc
	Tests sending an invalid message from a thread function
	to a connected FbsSession.

	@SYMTestActions
	Connects to a FbsSession. Gets a pointer to the session
	object. Sends an invalid command to the server. The thread
	returns with error code KErrNone.	
	
	@SYMTestExpectedResults
	Test should pass
*/
TInt BadMessage(TAny* aAny)
	{
	CTClean* theTest = static_cast <CTClean*> (aAny);
	CTrapCleanup::New();
	RFbsSession::Connect();
	RFbsSession* sess=RFbsSession::GetSession();
	sess->SendCommand(TFbsMessage(0x0fffffff));
	theTest->INFO_PRINTF1(_L("Bad Message Test Thread"));
	theTest->ERR_PRINTF1(_L("ERROR - BAD MESSAGE DID NOT PANIC THREAD"));
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0602

	@SYMTestCaseDesc
	Launches different threads for testing.

	@SYMTestActions
	Creates a thread and gives it higher priority.
	Disables just-in-time and starts the thread and 
	waits for the it to finish. Closes the thead,
	enables just-in-time and checks the heap for
	memory leaks.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTClean::LaunchThread(TTestType aType)
	{
	__UHEAP_MARK;
	RThread thrd;
	TRequestStatus stat;
	switch(aType)
		{
	case EFont1:
		thrd.Create(_L("Font1"),Font1,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case EFont2:
		thrd.Create(_L("Font2"),Font2,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case EBitmap1:
		thrd.Create(_L("Bitmap1"),Bitmap1,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case EPile1:
		thrd.Create(_L("Pile1"),Pile1,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case ESession1:
		thrd.Create(_L("Session1"),Session1,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case ESession2:
		thrd.Create(_L("Session2"),Session2,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case ETypefaceStore1:
		thrd.Create(_L("TypefaceStore1"),TypefaceStore1,KDefaultStackSize,0x2000,0x2000,this);
		break;
	case EBadMessage:
		thrd.Create(_L("BadMessage"),BadMessage,KDefaultStackSize,0x2000,0x2000,this);
		break;
	default:
		User::Panic(_L("TCLEAN"),KErrArgument);
		}
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	User::SetJustInTime(EFalse);
	thrd.Resume();
	User::WaitForRequest(stat);
	thrd.Close();
	User::SetJustInTime(ETrue);
	__UHEAP_MARKEND;
	}

//--------------
__CONSTRUCT_STEP__(Clean)

