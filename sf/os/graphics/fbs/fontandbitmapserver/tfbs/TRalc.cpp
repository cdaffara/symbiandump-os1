// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "FbsRalc.h"
#include "UTILS.H"
#include "TRalc.h"
#include "fbsmessage.h"
#include "fbsdefs.h"

_LIT(KMbmFileEikon, "z:\\System\\Data\\tfbs_file4.mbm");
_LIT(KMbmFileTechview, "z:\\System\\Data\\tfbs_file5.mbm");
_LIT(KMbmFileTechviewU,"Z:\\SYSTEM\\DATA\\TFBS_FILE5.MBM");
_LIT(KMbmFileLafcurs, "z:\\System\\Data\\tfbs_file2.mbm");
_LIT(KMbmFileShell, "z:\\System\\Data\\tfbs_file1.mbm");
_LIT(KMbmFileShel, "z:\\System\\Data\\tfbs_file_.mbm");
_LIT(KMbmFileTimew, "z:\\System\\Data\\tfbs_file3.mbm");

_LIT(KMbmFileLafcursLong, "z:\\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\\System\\Data\\tfbs_file2.mbm");
_LIT(KMbmFileShellLong, "z:\\siuhsiughighifhgidfhguifhguidhfguifhghlarglaghghghghghkrjghszkjg\\System\\Data\\tfbs_file1.mbm");

// ============================================================================

CTRalc::CTRalc(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iHandle(0),
	iFbs(NULL),
	iResourceCount(0),
	iAllocs(0)
	{
	}

CTRalc::~CTRalc()
	{
	if(iShutdownFbs)
		RFbsSession::GetSession()->SendCommand(EFbsMessShutdown);
	RFbsSession::Disconnect();
	}

void CTRalc::ConstructL()
	{
	if(RFbsSession::Connect()==KErrNone)
		{
		iShutdownFbs = EFalse;
		}
	else
		{
		FbsStartup();
		RFbsSession::Connect();
		iShutdownFbs = ETrue;
		}

	iFbs = RFbsSession::GetSession();

	TRAPD(err,ExpandCleanupStackL());
	if (err != KErrNone)
		User::Panic(_L("CleanupStack expansion failed"),err);

	INFO_PRINTF1(_L("FBS Client RALC testing"));
	}

void CTRalc::ExpandCleanupStackL()
	{
	TInt count=0;
	for(;count<10;count++)
		CleanupStack::PushL((TUint32*)0x1);
	CleanupStack::Pop(count);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0604

	@SYMTestCaseDesc
	Tests the creation of CFbsRalCacheEl objects.

	@SYMTestActions
	1. Test the API on a default built element object.
	2. Test a New'd element object followed by delete.
	3. Test a New'd element object followed by destroy & free.
	4. Test matching of an element object.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTRalc::TestRALCEntryClass()
	{
	INFO_PRINTF1(_L("Test of CFbsRalCacheEl API"));

	// Test the API on a defaultly built element object
		{
		CFbsRalCacheEl elOne;
		TEST(elOne.iAddress == 0);
		TEST(elOne.iFilename == 0);
	
		// Test matching method
		TBool r1 = EFalse;
		r1 = elOne.MatchKey (KMbmFileEikon);
		TEST(r1 == EFalse);

		// Test destruction
		}

	// Test a New'd element object followed by delete
		{
		CFbsRalCacheEl *elTwo = 0;
		elTwo = CFbsRalCacheEl::New(KMbmFileTimew, (TAny*)0x58001000);
		TEST(elTwo != 0);
		TEST(elTwo->iFilename->CompareF(KMbmFileTimew) == 0);
		TEST(elTwo->iAddress == (TAny*)0x58001000);

		delete elTwo;
		}
	
	// Test a New'd element object followed by destroy & free
		{
		CFbsRalCacheEl *elThree = 0;
		elThree = CFbsRalCacheEl::New(KMbmFileLafcursLong,  (TAny*)0x58002000);
		TEST(elThree != 0);
		TEST(elThree->iFilename->CompareF(KMbmFileLafcursLong) == 0);
		TEST(elThree->iAddress == (TAny*)0x58002000);

		elThree->~CFbsRalCacheEl();
		TEST(elThree->iFilename == 0);
		CFbsRalCacheEl::FreeOnly(elThree);
		TEST(elThree == 0);
		}

	// Test matching of an element object
		{
		CFbsRalCacheEl *elFour = 0;
		elFour = CFbsRalCacheEl::New(KMbmFileTechview,  (TAny*)0x58003000);
		TEST(elFour != 0);

		TBool r2 = EFalse;
		r2 = elFour->MatchKey (KMbmFileEikon);
		TEST(r2 == EFalse);

		r2 = elFour->MatchKey (KMbmFileTechview);
		TEST(r2);

		r2 = elFour->MatchKey(KMbmFileTechviewU);
		TEST(r2);

		delete elFour;
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0605

	@SYMTestCaseDesc
	Tests the RALC Cache class.

	@SYMTestActions
	Connect to the file server. Creates a CFbsRalCache
	object on the heap. Deletes the object.
	Test the lookup facility of the cache with overflow.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTRalc::TestRALCCacheClass()
	{
	RFs fileServer;
	TInt ret;

	ret = fileServer.Connect();
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Test of CFbsRalCache API"));

	// Test the construction/destruction API
		{
		CFbsRalCache* cache1 = CFbsRalCache::New(0, fileServer);
		TEST(cache1 == 0);

		cache1 = CFbsRalCache::New(5, fileServer);
		TEST(cache1 != 0);
		TEST(cache1->Count() == 0);
		TEST(cache1->Length() == 5);

		delete cache1;
		}

	// Test the lookup facility of the cache with overflow
		{
		CFbsRalCache* cache2 = CFbsRalCache::New(2, fileServer);

		TAny* p1;
		p1 = cache2->Lookup(KMbmFileLafcurs);
		TEST(p1 != 0);
		TEST(cache2->Count() == 1);

		p1 = cache2->Lookup(KMbmFileShel);
		TEST(p1 == 0);
		TEST(cache2->Count() == 1);

		p1 = cache2->Lookup(KMbmFileShell);
		TEST(p1 != 0);
		TEST(cache2->Count() == 2);

		p1 = cache2->Lookup(KMbmFileEikon);
		TEST(p1 != 0);
		TEST(cache2->Count() == 2);

		delete cache2;
		}

	fileServer.Close();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0606

	@SYMTestCaseDesc
	Tests Out Of Memory conditions for the CFbsRalCache class.

	@SYMTestActions
	Connects to the file server. Sets a macro
	that makes heap allocation fail after a certain
	number of heap allocations. Creates a CFbsRalCache
	object on the heap. Deletes the object. Closes the file
	server handle.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTRalc::TestOOMCondition()
	{
	RFs fileServer;
	TInt ret;
	ret = fileServer.Connect();
	TEST(ret==KErrNone);

	INFO_PRINTF1(_L("Test of CFbsRalCache and OOM"));

	for (TInt count = 1; ; count++)
        {
        __UHEAP_SETFAIL(RHeap::EDeterministic,count);
        __UHEAP_MARK;

		CFbsRalCache* cache = 0;
		cache = CFbsRalCache::New(4, fileServer);

		if (cache == 0)
            {
            __UHEAP_MARKEND;
            }
        else 
            {
            delete cache;
            __UHEAP_MARKEND;
        	__UHEAP_SETFAIL(RHeap::ENone,count);
            break;
            }
        }
	
	fileServer.Close();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0607

	@SYMTestCaseDesc
	Intended to measure performance of bitmap loading.

	@SYMTestActions
	Creates to CFbsBitmap objects on the stack and
	loads a bitmap file in to objects in a loop.
	Test case is not complete.
	
	@SYMTestExpectedResults
	No test result is given in the function.
*/
void CTRalc::MeasurePerformanceWith2()
	{
	TInt loopI;
	CFbsBitmap bitmap1;
	CFbsBitmap bitmap2;

	INFO_PRINTF1(_L("Performance Test with 2 Lookups"));

	for (loopI=0; loopI<2700; loopI++)
		bitmap1.Load(KMbmFileLafcurs, 0, EFalse);	

	for (loopI=0; loopI<600; loopI++)
		bitmap2.Load(KMbmFileShell, 0, EFalse);	

/*	RDebug::ProfileEnd(10);
	TProfile profile;
    RDebug::ProfileResult(&profile,10,1);
	RDebug::Print(_L("Performance test end %d.%d s\n"),profile.iTime/1000000,(profile.iTime/1000)%1000); */
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0609

	@SYMTestCaseDesc
	Intended to measure performance of bitmap loading.

	@SYMTestActions
	Creates to CFbsBitmap objects on the stack and
	loads a bitmap file in to objects in a loop.
	Test case is not complete.
	
	@SYMTestExpectedResults
	No test result is given in the function.
*/
void CTRalc::MeasurePerformanceWith2Long()
	{
	TInt loopI;
	CFbsBitmap bitmap1;
	CFbsBitmap bitmap2;

	INFO_PRINTF1(_L("Performance Test with 2 LONG Lookups"));
//	RDebug::ProfileReset(10, 1);
//	RDebug::ProfileStart(10);

	for (loopI=0; loopI<2700; loopI++)
		(void)bitmap1.Load(KMbmFileLafcursLong, 0, EFalse);	

	for (loopI=0; loopI<600; loopI++)
		(void)bitmap2.Load(KMbmFileShellLong, 0, EFalse);	

/*	RDebug::ProfileEnd(10);
	TProfile profile;
    RDebug::ProfileResult(&profile,10,1);
	RDebug::Print(_L("Performance test end %d.%d s\n"),profile.iTime/1000000,(profile.iTime/1000)%1000); */
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0608

	@SYMTestCaseDesc
	Intended to measure performance of bitmap loading.

	@SYMTestActions
	Creates five CFbsBitmap objects on the stack and
	loads different bitmap files in to the objects in a loop.
	
	@SYMTestExpectedResults
	No test result is given in the function.
*/
void CTRalc::MeasurePerformanceWith5()
	{
	TInt loopI;
	CFbsBitmap bitmap1;
	CFbsBitmap bitmap2;
	CFbsBitmap bitmap3;
	CFbsBitmap bitmap4;
	CFbsBitmap bitmap5;

	INFO_PRINTF1(_L("Performance Test with 5 Lookups"));
//	RDebug::ProfileReset(10, 1);
//	RDebug::ProfileStart(10);

	for (loopI=0; loopI<500; loopI++)
		bitmap1.Load(KMbmFileEikon, 0, EFalse);

	for (loopI=0; loopI<200; loopI++)
		bitmap2.Load(KMbmFileTechview, 0, EFalse);	

	for (loopI=0; loopI<2700; loopI++)
		bitmap3.Load(KMbmFileLafcurs, 0, EFalse);	

	for (loopI=0; loopI<600; loopI++)
		bitmap4.Load(KMbmFileShell, 0, EFalse);	

	for (loopI=0; loopI<1100; loopI++)
		bitmap5.Load(KMbmFileTimew, 0, EFalse);	

/*	RDebug::ProfileEnd(10);
	TProfile profile;
    RDebug::ProfileResult(&profile,10,1);
	RDebug::Print(_L("Performance test end %d.%d s\n"),profile.iTime/1000000,(profile.iTime/1000)%1000); */
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0610

	@SYMTestCaseDesc
	Intended to measure performance of bitmap loading.

	@SYMTestActions
	Creates five CFbsBitmap objects on the stack and
	loads different bitmap files in to the objects in a loop.
	
	@SYMTestExpectedResults
	No test result is given in the function.
*/
void CTRalc::MeasurePerformanceWith23Mix()
	{
	TInt loopI;
	CFbsBitmap bitmap1;
	CFbsBitmap bitmap2;
	CFbsBitmap bitmap3;
	CFbsBitmap bitmap4;
	CFbsBitmap bitmap5;

	INFO_PRINTF1(_L("Performance Test with 2/3 Mix of Lookups"));
//	RDebug::ProfileReset(10, 1);
//	RDebug::ProfileStart(10);

	for (loopI=0; loopI<500; loopI++)
		bitmap1.Load(KMbmFileEikon, 0, EFalse);

	for (loopI=0; loopI<200; loopI++)
		bitmap2.Load(KMbmFileTechview, 0, EFalse);	

	for (loopI=0; loopI<2700; loopI++)
		bitmap3.Load(KMbmFileLafcursLong, 0, EFalse);	

	for (loopI=0; loopI<600; loopI++)
		bitmap4.Load(KMbmFileShellLong, 0, EFalse);	

	for (loopI=0; loopI<1100; loopI++)
		bitmap5.Load(KMbmFileTimew, 0, EFalse);	

/*	RDebug::ProfileEnd(10);
	TProfile profile;
    RDebug::ProfileResult(&profile,10,1);
	RDebug::Print(_L("Performance test end %d.%d s\n"),profile.iTime/1000000,(profile.iTime/1000)%1000);*/
	}

void CTRalc::RunTestCaseL(TInt aCurTestCase)
	{
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KMbmFileEikon, romAddress)) //check any rom bitmap
		{
		INFO_PRINTF2(_L("Skipping CFbsRalCache tests since file \"%S\" is reported to not be a ROM bitmap."),
				&KMbmFileEikon);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		TestComplete();
		}
	else
		{
		((CTRalcStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
		switch(aCurTestCase)
			{
		case 1:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0604"));
			TestRALCEntryClass();
			break;
		case 2:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0605"));
			TestRALCCacheClass();
			break;
		case 3:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0606"));
			TestOOMCondition();
			break;
		case 4:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0607"));
			MeasurePerformanceWith2();
			break;
		case 5:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0608"));
			MeasurePerformanceWith5();
			break;
		case 6:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0609"));
			MeasurePerformanceWith2Long();
			break;
		case 7:
			((CTRalcStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0610"));
			MeasurePerformanceWith23Mix();
			break;
		case 8:
			((CTRalcStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTRalcStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
			}
		((CTRalcStep*)iStep)->RecordTestResultL();
		}
	}

//--------------
__CONSTRUCT_STEP__(Ralc)

void CTRalcStep::TestSetupL()
	{
	// Leaking thread handles
	iThread.HandleCount(iStartProcessHandleCount, iStartThreadHandleCount);
	}
	
void CTRalcStep::TestClose()
	{
	// Check for open handles
	iThread.HandleCount(iEndProcessHandleCount, iEndThreadHandleCount);
	if(iStartThreadHandleCount != iEndThreadHandleCount)
		{__DEBUGGER()}							// Oops leaked some handles
	}
