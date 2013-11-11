// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TFBSDefect.h"
#include "fbsmessage.h"

CTFbsDefect::CTFbsDefect(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0574

	@SYMTestCaseDesc
	Checks for leaking thread handles during 
	creation of bitmaps
	
	@SYMDEF INC041784
	
	@SYMTestActions
	Retrieves the process handle count and the
	thread handle count. Creates a bitmap with 
	the specified size and display mode. The bitmap 
	is created on the font and bitmap server's shared 
	heap. The thread handle count is retrieved again 
	and compared against the first thread handle count
	and they should match.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsDefect::CreateBitmapTestL()
    {
	INFO_PRINTF1(_L("CreateBitmapTestL (INC041784)"));

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...
	//

    const TSize size(100, 100);

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	TEST (bitmap->Create(size, ERgb)     == KErrArgument);
	TEST (bitmap->Create(size, ENone)    == KErrArgument);
	TEST (bitmap->Create(size, EColor16) == KErrNone);

	CleanupStack::PopAndDestroy(bitmap);

	//
	// Test Ends...

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startThreadHandleCount == endThreadHandleCount);
    }
    
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0575
	
	@SYMDEF	DEF042528

	@SYMTestCaseDesc
	Tests replacing initial display with all the 
	possible higher display mode and tests replacing 
	initial display with all the possible lower display mode.
	
	@SYMTestActions
	Creates a bitmap. Creates an array containing display modes. 
	For each display mode it sets the display mode to a higher
	display mode and checks for errors. Then for each display mode
	it sets the display mode to a lower display mode and checks
	for errors.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsDefect::TestDisplayModeL()
	{
	INFO_PRINTF1(_L("TestDisplayModeL (DEF042528)"));

    const TSize size(10,10);
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	//TDisplayMode value= {   1  ,  2   ,  3    ,  4     ,   5         6        10       7         8          11          12		13
	TDisplayMode mode[] = {EGray2,EGray4,EGray16,EGray256,EColor16,EColor256,EColor4K,EColor64K,EColor16M,EColor16MU,EColor16MA,EColor16MAP};

	TInt arraysize=sizeof(mode)/sizeof(mode[0]);
	INFO_PRINTF2(_L("Array size=%d\n"),arraysize);
	TInt ret;
	
	//Test for replacing initial display with all the possible higher display mode
	//This should not work and all the values returned by SetDisplayMode()==KErrArgument
	for (TInt i=0;i<arraysize;i++)
		{
		ret=bitmap->Create(size,mode[i]);
		INFO_PRINTF2(_L("Initial mode: %d\n"),mode[i]);
		TEST(ret==KErrNone);
		for (TInt j=i+1;j<arraysize;j++)
			{
			if ((mode[i]==EColor16MU && (mode[j]==EColor16MA || mode[j]==EColor16MAP)) || (mode[i]==EColor16MA && mode[j]==EColor16MAP))
				{
				//special cases where we can change from EColor16MU to either EColor16MA or EColor16MAP and from EColor16MA to EColor16MAP.
				ret=bitmap->SetDisplayMode(mode[j]);
				INFO_PRINTF3(_L("Changing to display mode=%d with ret value=%d\n "),mode[j],ret);
				TEST(ret==KErrNone);
				}
			else
				{
				ret=bitmap->SetDisplayMode(mode[j]);
				INFO_PRINTF3(_L("Changing to display mode %d with ret value %d \n"),mode[j],ret);				
				TEST(ret==KErrArgument);	
				}
			}
		INFO_PRINTF1(_L("End of mode \n"));
		}
	
	//Test for replacing initial display with all the possible lower display mode
	//This should work fine and all the values returned by SetDisplayMode()==KErrNone
	INFO_PRINTF1(_L("\n\n"));
	for (TInt k=arraysize-1;k>=0;k--)
		{
		ret=bitmap->Create(size,mode[k]);
		INFO_PRINTF2(_L("Initial mode=%d\n"),mode[k]);
		TEST(ret==KErrNone);
		for (TInt l=k-1;l>=0;l--)
			{
			
			if (mode[k]==EColor16 && mode[l]==EGray256)
				{
				//special case where we cannot change from EColor16 to EGray256
				ret=bitmap->SetDisplayMode(mode[l]);
				INFO_PRINTF3(_L("Changing to display mode=%d with ret value=%d\n "),mode[l],ret);
				TEST(ret==KErrArgument);
				}
			
			else
				{
				ret=bitmap->SetDisplayMode(mode[l]);
				INFO_PRINTF3(_L("Changing to display mode=%d with ret value=%d\n "),mode[l],ret);
				TEST(ret==KErrNone);
				}
			}
		
		}
	
	bitmap->Reset();
	delete bitmap;
	}
    
/* Creates and destroys large bitmaps */
TInt BitmapCreate(TAny* /* aAny */)
	{
	RFbsSession fbs;
	TInt err = fbs.Connect();
	User::LeaveIfError(err);
	RFbsSession* fbs2 = RFbsSession::GetSession();
	
	FOREVER
		{
		CFbsBitmap bitmap;
		bitmap.Create(TSize(300,200), EColor16MU);
		bitmap.Reset();
		fbs2->SendCommand(EFbsCompress, 0);
		User::After(1);
		}
	}

void CleanupFbsConnection(TAny* aPtr)
	{
	RFbsSession* fbs = static_cast<RFbsSession*>(aPtr);
	fbs->Disconnect();
	}

void CleanupLoadedBitmap(TAny* aPtr)
	{
	RPointerArray<CFbsBitmap>* ptrArray = (static_cast<RPointerArray<CFbsBitmap>*>(aPtr));	
	ptrArray->ResetAndDestroy();
	ptrArray->Close();
	}

void FillBitmapArray(RPointerArray<CFbsBitmap>& aArray)
	{
	for(TInt count=100; count>=0; --count)
		{
		CFbsBitmap* bitmap = new CFbsBitmap;
		if (!bitmap)
			return;

		TInt err = bitmap->Create(TSize(100,100), EColor16MU);

		if (err == KErrNone)
			{
			err = aArray.Append(bitmap);
			}

		if (err != KErrNone)
			{
			delete bitmap;
			return;
			}
		}
	}

void DoBitmapUseL()
	{
	RFbsSession fbs;
	TInt err = fbs.Connect();
	User::LeaveIfError(err);
	TCleanupItem fbsConnectionCleanup(CleanupFbsConnection, &fbs);
	CleanupStack::PushL(fbsConnectionCleanup);

	RPointerArray<CFbsBitmap> stressBitmaps;
	TCleanupItem loadedBitmapCleanup(CleanupLoadedBitmap, &stressBitmaps);
	CleanupStack::PushL(loadedBitmapCleanup);
	FillBitmapArray(stressBitmaps);

	CFbsBitmap* outputBitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(outputBitmap);
	User::LeaveIfError(outputBitmap->Create(TSize(200,200),EColor16MU));
	CFbsBitmapDevice *bgDevice=CFbsBitmapDevice::NewL(outputBitmap);
	CleanupStack::PushL(bgDevice);
	CFbsBitGc* bgGc=NULL;
	User::LeaveIfError(bgDevice->CreateContext(bgGc));
	CleanupStack::PushL(bgGc);

	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(100,100),EColor16MU));
	for (TInt count = 1000; count > 0; count--)
		{
		if ((count % 31)==1)
			{
			bitmap->Reset();
			User::After(1);
			User::LeaveIfError(bitmap->Create(TSize(100,100),EColor16MU));
			}
		if ((count % 11)==1)
			{
			stressBitmaps.ResetAndDestroy();
			}
		if ((count % 11)==5)
			{
			stressBitmaps.ResetAndDestroy();
			FillBitmapArray(stressBitmaps);
			}
		bgGc->DrawBitmap(TPoint(10,10),bitmap);
		User::After(1);
		}

	CleanupStack::PopAndDestroy(bitmap);
	CleanupStack::PopAndDestroy(bgGc);
	CleanupStack::PopAndDestroy(bgDevice);
	CleanupStack::PopAndDestroy(outputBitmap);
	CleanupStack::PopAndDestroy(/*&loadedBitmapCleanup*/);
	CleanupStack::PopAndDestroy(/*&fbsConnectionCleanup*/);
	}

void TestBitmapUseL(CTFbsDefect* aFbsDefect)
	{
	TRAPD(err,DoBitmapUseL());
	aFbsDefect->TEST(err==KErrNone || err==KErrNoMemory); // Out-of-memory doesn't constitute a test failure
	}

/* Draws bitmaps */
TInt BitmapUse(TAny* aAny)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	CTFbsDefect* theTest = static_cast <CTFbsDefect*> (aAny);
	__UHEAP_MARK;
	
	TRAP_IGNORE(TestBitmapUseL(theTest));

	__UHEAP_MARKEND;
	delete CleanUpStack;
	return KErrNone;
	}

/** 
	@SYMTestCaseID
	GRAPHICS-FBSERV-0576

	@SYMTestCaseDesc
	Verifies defect DEF079124.
	
	@SYMDEF DEF079124

	@SYMTestActions
	Creates two threads. One threads keeps creating
	and destroying large bitmaps. The second thread 
	draws bitmaps. The exit reason of the threads
	are checked. The thread handles are closed.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsDefect::TestFbservChunkLockingL()
	{
	INFO_PRINTF1(_L("TestDisplayModeL (DEF079124)"));
	
	__UHEAP_MARK;
	TRequestStatus stat;
	RThread bitmapCreate;
	RThread bitmapUse;
	bitmapCreate.Create(_L("BitmapCreate"),BitmapCreate,KDefaultStackSize,0x2200,0x2200,this);
	bitmapUse.Create(_L("BitmapUse"),BitmapUse,KDefaultStackSize,0x2200,0x2200,this);

	bitmapUse.Logon(stat);
	bitmapCreate.Resume();
	bitmapUse.Resume();
	User::WaitForRequest(stat);
	
	INFO_PRINTF2(_L("bitmapCreate ExitReason: %d"),bitmapCreate.ExitReason());
	INFO_PRINTF2(_L("bitmapUse ExitReason: %d"),bitmapUse.ExitReason());
	TEST(bitmapCreate.ExitReason() == KErrNone);
	TEST(bitmapUse.ExitReason() == KErrNone);
	bitmapCreate.Close();
	bitmapUse.Close();
	__UHEAP_MARKEND;
	}
	
/**
 * @SYMTestCaseID GRAPHICS-FBSERV-0621
 *
 * @SYMDEF INC112542, PDEF113158
 *
 * @SYMTestCaseDesc A test to check KErrEof is returned when a bitmap index greater than the 
 * 					number of bitmaps present in the .mbm is requested from a ROM bitamp
 *
 * @SYMTestPriority Normal
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions Loads a bitmap from rom where the index is greater than the numer of bitmaps actually in the mbm
 * 					Checks that KErrEof is returned when this happens.	
 *
*/	
void CTFbsDefect::TestKErrEof()
	{
	INFO_PRINTF1(_L("TestKErrEof - testing error code returned from CFbsBitmap::Load()"));
	_LIT(KTestBitmapOnZZ,"z:\\system\\data\\tfbs.rbm"); // this contains 2 bitmaps


	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KTestBitmapOnZZ, romAddress))
		{
		INFO_PRINTF2(_L("Skipping test of ROM bitmaps since file \"%S\" is reported to not be a ROM bitmap."),
				&KTestBitmapOnZZ);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		return;
		}

	CFbsBitmap rom;
	TInt bitmapIndex = 2;	// this bmp index is one greater than the bitmap contains
	TInt errCode = rom.Load(KTestBitmapOnZZ,bitmapIndex,EFalse);
	
	if(errCode!=KErrEof)
		{
		TEST(EFalse);
		INFO_PRINTF2(_L("TestKErrEof - Accessing ROM bitmap index greater than # contained returned %d instead of KErrEof"),errCode);			
		}
	}

/* Creates  bitmaps under OOM conditions (PDEF129780)*/
void CTFbsDefect::CreateBitmapOOMTestL()
	{
	INFO_PRINTF1(_L("CreateBitmapOOMTestL (PDEF129780) - Creating Bitmap under OOM conditions"));
	
	RFbsSession* fbs = RFbsSession::GetSession();
		
	RPointerArray<CFbsBitmap> bitmaps;
	TCleanupItem bitmapsCleanupItem (CleanupLoadedBitmap, &bitmaps);
	CleanupStack::PushL(bitmapsCleanupItem);
		
	// Loop here is to fill CFbTop::iBitmapObjectIndex and force it reallocate memory inside CBitmapObject::ConstructL
	// The upper bound of the loop depends on how CFbTop::iBitmapObjectIndex is expended and how many pointers are in it by the start of this subtest
	// The upper bound may require to be increased if previous subtests are changed
	for(TInt i=0; i<500; i++)
		{
		TSize size(2,2); 
		
		// Rate starts from 2 to allow allocate memory at least for new CBitmapObject and leave during next attempt to allocate memory
		// The Loop is to make sure that the situation won't be missed if something in code is changed 
		// and it is more memory allocations before ConstructL. Currently it is enough 2.
		for(TInt leaveRate = 2; leaveRate<10; leaveRate++)
			{
			CFbsBitmap* testBitmap = new(ELeave) CFbsBitmap;
			
			fbs->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeServerMemory, leaveRate); 
			
			//Try to create bitmap 
			testBitmap->Create(size, EColor16MAP);
			delete testBitmap;
			}
		
		fbs->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeServerMemory, 0);
		
		// Create bitmap to fill FbTop::iBitmapObjectIndex
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmaps.Append(bitmap));
		CleanupStack::Pop(bitmap);
		User::LeaveIfError(bitmap->Create(size, EColor16MAP));
		}
	INFO_PRINTF2(_L("Created %d bitmaps."), bitmaps.Count());
	CleanupStack::PopAndDestroy(&bitmaps);
	// If it was no Panic in fbserv and we have got here test pass
	}


void CTFbsDefect::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFbsDefectStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFbsDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0574"));
		CreateBitmapTestL();
		break;
	case 2:
		((CTFbsDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0575"));
		TestDisplayModeL();
		break;
	case 3:
		((CTFbsDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0576"));
		TestFbservChunkLockingL();
		break;
	case 4:
		((CTFbsDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0621"));
		TestKErrEof();
		break;
	case 5:
/**
    @SYMTestCaseID GRAPHICS-FBSERV-0620
*/
		((CTFbsDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0620"));
		CreateBitmapOOMTestL();
		break;	
	case 6:
		((CTFbsDefectStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFbsDefectStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTFbsDefectStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(FbsDefect)
