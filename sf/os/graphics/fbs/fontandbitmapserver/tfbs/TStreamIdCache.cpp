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

// Epoc includes
#include "../inc/FBSMBMC.H"
#include "../sfbs/UTILS.H"
#include "TStreamIdCache.h"
#include "fbsmessage.h"
#include "fbsdefs.h"

//Eikon has 78 bitmaps inside the mbm file(id 0-77)
_LIT(KMbmFileEikon, "z:\\System\\Data\\tfbs_file4.mbm");
_LIT(KMbmFileEikon2, "z:\\System\\Data\\tfbs_file4a.mbm");
_LIT(KMbmFileEikon3, "z:\\System\\Data\\tfbs_file4b.mbm");
_LIT(KMbmFileEikon4, "z:\\System\\Data\\tfbs_file4c.mbm");
_LIT(KMbmFileEikon5, "z:\\System\\Data\\tfbs_file4d.mbm");
//Lafcurs has 28 bitmaps inside the mbm file(id 0-27)
_LIT(KMbmFileLafcurs, "z:\\System\\Data\\tfbs_file2.mbm");
//Resource file containing mbm file
_LIT(KMbmFileTFBSRSC, "z:\\System\\Data\\tfbs_rsc.rsc");
_LIT(KRscFileHeader, "z:\\system\\data\\RscHeader3.bin");
_LIT(KRscFileData, "z:\\system\\data\\DummyRscFile3.rsc");

// File with two 1x1 px mbm files: first is black, second is white. white mbm at 72 bytes into file
#define WHITE_OFFSET 72
_LIT(KBlackAndWhite, "z:\\system\\data\\blackandwhite.mbm");

// ============================================================================

//Default Constructor
CTStreamIdCache::CTStreamIdCache(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL)
	{
	}

CTStreamIdCache::~CTStreamIdCache()
	{
	if(iShutdownFbs)
		RFbsSession::GetSession()->SendCommand(EFbsMessShutdown);
	RFbsSession::Disconnect();
	iFs.Close();
	}

void CTStreamIdCache::ConstructL()
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

	iFs.Connect();

	iFbs = RFbsSession::GetSession();
	iSessionHandle = iFbs->SessionHandle();
	ExpandCleanupStackL();
	
	INFO_PRINTF1(_L("FBS Bitmap StreamId cache testing"));
	}
	
void CTStreamIdCache::RunTestCaseL(TInt aCurTestCase)
	{
	((CTStreamIdCacheStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0577"));
		TestStreamIdCacheEntry();
		break;
	case 2:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0578"));
		TestProcessingBitmapStream();
		break;
	case 3:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0580"));
		TestReplacingFile();
		break;
	case 4:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0579"));
		TestComparingStreams();
		break;
	case 5:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0581"));
		TestInvalidArgument();
		break;
	case 6:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0582"));
		TestOOMCondition();
		break;
	case 7:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0583"));
		TestPerformance();
		break;
	case 8:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0584"));
		TestLoadAtOffset();
		break;
	case 9:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0509"));
		TestSessionClose();
		break;
	case 10:
		((CTStreamIdCacheStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTStreamIdCacheStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTStreamIdCacheStep*)iStep)->RecordTestResultL();
	}
	
//This function is here to measure the offset in the rsc file
TInt CTStreamIdCache::FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}

void CTStreamIdCache::ExpandCleanupStackL()
	{
	TInt count=0;
	for(;count<10;count++)
		CleanupStack::PushL((TUint32*)0x1);
	CleanupStack::Pop(count);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0577

	@SYMTestCaseDesc
	Tests the API functionality of the class  ensuring 
	the internal states are correct during the construction 
	and other operation.

	@SYMTestActions
	Tests the API on a defaultly built element object.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestStreamIdCacheEntry()
	{
	INFO_PRINTF1(_L("Test of CFbTopStreamIdCache API"));

	// Test the API on a defaultly built element object
		
		CFbTopStreamIdCache* mbmcache=new CFbTopStreamIdCache(20,30,1);
		TEST(mbmcache->iEntries.Count() == 0);
		TEST(mbmcache->iMaxCacheFilestores == 1);
		TEST(mbmcache->iBackwardCacheSize ==30);
		TEST(mbmcache->iForwardCacheSize ==20);
		
		delete mbmcache;

	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0578

	@SYMTestCaseDesc
	Tests the loading of a bitmap from an mbm file using 
	the cache and the content of the cache after the loading 
	i.e. how many stream ids are stored, the last bitmap file 
	name, the last id, the cache forward and backward size.

	@SYMTestActions
	Tests the GetStreamIdL method. Loads eikon.mbm with 78
	bitmap files. Tests loading some rsc file which contains offset.
	Tests the flushing.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestProcessingBitmapStream()
	{
	TInt ret;

	INFO_PRINTF1(_L("Test loading a bitmap file and processing the stream id"));

	//Testing the GetStreamIdL method
	CFbTopStreamIdCache* mbmcacheptr = new CFbTopStreamIdCache(30,30,1);
	CFbTopStreamIdCache& mbmcache = *mbmcacheptr;
	INFO_PRINTF1(_L("Loading eikon.mbm with 78 bitmap files"));
	//Loading eikon.mbm with 78 bitmap files
	RFile file;
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,mbmcache.GetStreamIdL(file,KMbmFileEikon,50,0,iSessionHandle));
	TEST(ret==KErrNone);
	//the cache only stores 30 ids before 50 + max of 30 after(including 50)
	//but in this case the bitmap only has 78 bitmaps so it only stores 28 after
	TEST(mbmcache.iEntries.Count() == 1);
	TEST(mbmcache.iEntries[0]->iStreamIdCount==58);
	TEST(mbmcache.iEntries[0]->iLastId==50);
	TEST(mbmcache.iForwardCacheSize==30);
	TEST(mbmcache.iBackwardCacheSize==30);
	TEST(mbmcache.iEntries[0]->iFilename.CompareF(KMbmFileEikon)==0);
	TEST(mbmcache.iEntries[0]->iFilestore!=NULL);

	//Test loading some rsc file which contains offset
	TInt mbm_offset = FileSizeL(KRscFileHeader) + FileSizeL(KRscFileData);
	ret=file.Open(iFs,KMbmFileTFBSRSC,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,mbmcache.GetStreamIdL(file,KMbmFileTFBSRSC,0,mbm_offset,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(mbmcache.iEntries.Count() == 1);
	TEST(mbmcache.iEntries[0]->iLastId==0);
	TEST(mbmcache.iEntries[0]->iStreamIdCount==2);
	TEST(mbmcache.iEntries[0]->iFilename.CompareF(KMbmFileTFBSRSC)==0);

	// Test the flushing
	mbmcache.FlushCache();
	TEST(mbmcache.iEntries.Count() == 0);
	TEST(mbmcache.iMaxCacheFilestores == 1);
	TEST(mbmcache.iBackwardCacheSize == 30);
	TEST(mbmcache.iForwardCacheSize == 30);

	delete mbmcacheptr;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0579

	@SYMTestCaseDesc
	Tests the correct functionality of the cache storing the stream ids. 
	Three different caches are used to load the same mbm file but the 
	bitmap id to be loaded are different however the bitmap ids are chosen in 
	such a way that these three caches will store some common bitmap id. 
	We then accessed the cache content and check that these three caches 
	will give the same stream id values for that common bitmap id.

	@SYMTestActions
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestComparingStreams()
	{
	TInt ret;

	INFO_PRINTF1(_L("Test comparing the stream ids from three different caches"));

	CFbTopStreamIdCache* cache1=new CFbTopStreamIdCache(30,30,1);
	CFbTopStreamIdCache* cache2=new CFbTopStreamIdCache(10,30,1);
	CFbTopStreamIdCache* cache3=new CFbTopStreamIdCache(25,5,1);

	//CACHE1 Loading this will fill up the streams with ids from 0-55
	RFile file;
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1->GetStreamIdL(file,KMbmFileEikon,25,0,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(cache1->iEntries.Count() == 1);
	TEST(cache1->iEntries[0]->iStreamIdCount==55);
	TEST(cache1->iEntries[0]->iLastId==25);
	//now storing the actual TStreamId for bitmap id 26 for future comparison
	//stream id for bitmapid=26 will be @ the 26th position inside the array
	TStreamId id25_26=cache1->iEntries[0]->iStreamIdCache[26];

	delete cache1;	
	
	
	//CACHE2 Loading this will fill up the streams with ids from 36-65
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache2->GetStreamIdL(file,KMbmFileEikon,56,0,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(cache2->iEntries.Count() == 1);
	TEST(cache2->iEntries[0]->iStreamIdCount==40);
	TEST(cache2->iEntries[0]->iLastId==56);
	//stream id for bitmapid=26 will be @ the 0th position inside the array
	TStreamId id56_26=cache2->iEntries[0]->iStreamIdCache[0];
	TEST(id25_26.Value()==id56_26.Value());
	
	delete cache2;

	//CACHE3 Loading this will fill up the streams with ids from 1-30
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache3->GetStreamIdL(file,KMbmFileEikon,6,0,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(cache3->iEntries.Count() == 1);
	TEST(cache3->iEntries[0]->iStreamIdCount==30);
	TEST(cache3->iEntries[0]->iLastId==6);
	TStreamId id6_26=cache3->iEntries[0]->iStreamIdCache[25];
	TEST(id25_26.Value()==id6_26.Value());

	delete cache3;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0580

	@SYMTestCaseDesc
	Tests the functionality of the cache when it has already
	stored some existing bitmap ids The cache is loaded with a 
	different mbm file and the cache content is examined to make 
	sure it has the correct new data.
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestReplacingFile()
	{
	TInt ret;

	INFO_PRINTF1(_L("Test loading a file different from previous stored in cache"));

	CFbTopStreamIdCache* cache1ptr = new CFbTopStreamIdCache(30,30,1);
	CFbTopStreamIdCache& cache1 = *cache1ptr;
	RFile file;
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,25,0,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(cache1.iEntries.Count() == 1);
	TEST(cache1.iEntries[0]->iStreamIdCount==55);
	TEST(cache1.iEntries[0]->iLastId==25);
	TEST(cache1.iEntries[0]->iFilename.Compare(KMbmFileEikon)==0);

	//Now trying to load an entirely different bitmap file
	//and check to ensure the cache has reflushed itself
	//and load itself with new stuff
	ret=file.Open(iFs,KMbmFileLafcurs,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileLafcurs,12,0,iSessionHandle));
	TEST(ret==KErrNone);
	TEST(cache1.iEntries.Count() == 1);
	TEST(cache1.iEntries[0]->iStreamIdCount==28);
	TEST(cache1.iEntries[0]->iLastId==12);
	TEST(cache1.iEntries[0]->iFilename.Compare(KMbmFileLafcurs)==0);

	delete cache1ptr;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0581

	@SYMTestCaseDesc
	Tests the error values that would be returned by
	the cache when supplied with invalid arguments such 
	as bad file name  negative bitmap ids, out of range 
	bitmap ids, and invalid file offset.

	@SYMDEF INC047122 INC046632

	@SYMTestActions
	Creates an FbTopStreamIdCache object. Tests to ensure the cache 
	can deal with non error cases immediately after error cases.
	Passing an in bound bitmap id. Passing an out of bound bitmap id.
	Passing an in bound bitmap id. Passing an out of bound bitmap id.
	Passing an in bound bitmap id. Passing negative bitmap id.
	Passing an invalid file name. Passing an invalid file offset.
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestInvalidArgument()
	{
	
	TInt ret;

	INFO_PRINTF1(_L("Test loading a file bitmap with invalid argument"));

	CFbTopStreamIdCache* cache1ptr = new CFbTopStreamIdCache(30,30,1);
	CFbTopStreamIdCache& cache1 = *cache1ptr;
	//Eikon mbm only can accept ids from 0-77

	//The following sequence of 4 tests ensures the cache can deal with
	//non error cases immediately after error cases.

	//Passing an in bound bitmap id
	RFile file;
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,0,0,iSessionHandle));
	TEST(ret==KErrNone);

	//Passing an out of bound bitmap id
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,78,0,iSessionHandle));
	TEST(ret==KErrEof);

	//Passing an in bound bitmap id
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,74,0,iSessionHandle));
	TEST(ret==KErrNone);
	
	//Passing an out of bound bitmap id
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,78,0,iSessionHandle));
	TEST(ret==KErrEof);
	
	//Passing an in bound bitmap id
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,77,0,iSessionHandle));
   	TEST(ret==KErrNone);


	//Passing negative bitmap id
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,-1,0,iSessionHandle));
	TEST(ret==KErrEof);

	//Passing an invalid file name
	ret=file.Open(iFs,_L("invalid.mbm"),EFileShareReadersOnly);
	cache1.FlushCache();
	TEST((ret==KErrNotFound)||(ret==KErrPathNotFound));

	//Passing an invalid file offset
	ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TRAP(ret,cache1.GetStreamIdL(file,KMbmFileEikon,12,10,iSessionHandle));
	TEST(ret==KErrNotSupported);
	
	//Tests for INC047122 and INC046632
	TInt err(KErrNone);
	TInt id(0);
	for (; id<100; id++)
		{
		err=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(err==KErrNone);
		TRAP(err,cache1.GetStreamIdL(file,KMbmFileEikon,id,0,iSessionHandle));
		if(err != KErrNone)
			break;
		}
	TEST(id==78);
	INFO_PRINTF2(_L("Starting from 0 the number of bitmaps in KMbmFileEikon is calculated to be %d\n\n"), id);

	delete cache1ptr;

	// New cache, start from Id 50
	CFbTopStreamIdCache* cache2ptr = new CFbTopStreamIdCache(30,30,1);
	CFbTopStreamIdCache& cache2 = *cache2ptr;
	for (id=50,	err=KErrNone; id<100; id++)
		{
		err=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(err==KErrNone);
		TRAP(err,cache2.GetStreamIdL(file,KMbmFileEikon,id,0,iSessionHandle));
		if(err != KErrNone)
			break;
		}
	TEST(id==78);
	INFO_PRINTF2(_L("Starting from 50 the number of bitmaps in KMbmFileEikon is calculated to be %d\n\n"), id);

	delete cache2ptr;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0582

	@SYMTestCaseDesc
	Tests the out of memory condition during memory 
	allocation inside the test function GetStreamIdL
	to ensure that there is no memory leaks.

	@SYMTestActions
	Tests the cache by opening a file and calls GetStreamIdL
	and checks the heap for memory leaks. Tests the cache
	for a cache hit. Test the cache for a cache miss.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestOOMCondition()
	{
	INFO_PRINTF1(_L("Test of CFbTopStreamIdCache OOM"));
	
	// Test a cache for the first time
	for (TInt count = 1; ; count++)
        {
        __UHEAP_MARK;
		CFbTopStreamIdCache* cache=new CFbTopStreamIdCache(30,30,1);
		
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		RFile file;
		TInt ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		TRAP(ret,cache->GetStreamIdL(file,KMbmFileEikon,30,0,iSessionHandle));
		__UHEAP_RESET;
		
	    delete cache;
	    __UHEAP_MARKEND;
		
		if (ret==KErrNone)
			break;
		TEST(ret==KErrNoMemory);
		}

	// Test a cache hit
	for (TInt count = 1; ; count++)
        {
        __UHEAP_MARK;
		CFbTopStreamIdCache* cache=new CFbTopStreamIdCache(30,30,1);
		
		RFile file1;
		TInt ret=file1.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		TRAP(ret,cache->GetStreamIdL(file1,KMbmFileEikon,30,0,iSessionHandle));
		TEST(ret==KErrNone);		

		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		RFile file;
		ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		TRAP(ret,cache->GetStreamIdL(file,KMbmFileEikon,30,0,iSessionHandle));
		__UHEAP_RESET;
		
	    delete cache;
	   	__UHEAP_MARKEND;
	   	
		if (ret==KErrNone)
			break;
		TEST(ret==KErrNoMemory);
		}

	// Test a cache miss
	for (TInt count = 1; ; count++)
        {
        __UHEAP_MARK;
		CFbTopStreamIdCache* cache=new CFbTopStreamIdCache(30,30,1);
		
		RFile file1;
		TInt ret=file1.Open(iFs,KMbmFileLafcurs,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		TRAP(ret,cache->GetStreamIdL(file1,KMbmFileLafcurs,0,0,iSessionHandle));
		TEST(ret==KErrNone);		

		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		RFile file;
		ret=file.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		TRAP(ret,cache->GetStreamIdL(file,KMbmFileEikon,30,0,iSessionHandle));
		__UHEAP_RESET;
		
	    delete cache;
	   	__UHEAP_MARKEND;
	   	
		if (ret==KErrNone)
			{
			__UHEAP_SETFAIL(RHeap::ENone,count);
			break;
			}
		TEST(ret==KErrNoMemory);
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0583

	@SYMTestCaseDesc
	This test is used to measure the performance improvement 
	when the font bitmap server is using a cache to store the 
	stream ids.

	@SYMTestActions
	Starts the timing. Gets the current time. Creates a FbsBitmap
	on the heap. Simulates loading during boot time by loading
	bitmaps from ROM. Does forward testing before optimised duration.
	Retreives the current time and calculates the time taken by the tests.
	Does forward testing after optimised duration. Does backward testing
	before optimised duration. Does backward testing after optimised duration.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestPerformance()
	{
	//Start the timing
	TTime starttime;
	starttime.UniversalTime();

	CFbsBitmap* bitmap=new (ELeave) CFbsBitmap;
	TInt ret;
	//simulating loading during boottime
	TInt i,j;
	for (i=0;i<30;i++)
		{
		for (j=0;j<10;j++)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=0;j<2;j++)
			{
			ret=bitmap->Load(_L("z:\\system\\data\\tfbs.mbm"),j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=0;j<28;j++)
			{
			ret=bitmap->Load(KMbmFileLafcurs,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=71;j<73;j++)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=0;j<2;j++)
			{
			ret=bitmap->Load(_L("z:\\system\\data\\tfbs.mbm"),j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=20;j<22;j++)
			{
			ret=bitmap->Load(KMbmFileLafcurs,j,EFalse);
			TEST(ret==KErrNone);
			}
		}
	
	TTime endtime;
	endtime.UniversalTime();
	INFO_PRINTF1(_L("Forward Testing Before optimised duration= 3515625 microseconds \n"));
	TTimeIntervalMicroSeconds difftime=endtime.MicroSecondsFrom(starttime);
	INFO_PRINTF2(_L("Forward Testing After optimised duration=%Ld  microseconds\n\n"),difftime.Int64());

	starttime.UniversalTime();
	for (i=0;i<30;i++)
		{
		for (j=10;j>=0;j--)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=1;j>=0;j--)
			{
			ret=bitmap->Load(_L("z:\\system\\data\\tfbs.mbm"),j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=27;j>=0;j--)
			{
			ret=bitmap->Load(KMbmFileLafcurs,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=73;j>=72;j--)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=1;j>=0;j--)
			{
			ret=bitmap->Load(_L("z:\\system\\data\\tfbs.mbm"),j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=22;j>=20;j--)
			{
			ret=bitmap->Load(KMbmFileLafcurs,j,EFalse);
			TEST(ret==KErrNone);
			}
		}	
	endtime.UniversalTime();
	difftime=endtime.MicroSecondsFrom(starttime);
	INFO_PRINTF1(_L("Backward Testing Before optimised duration= 3515625 microseconds \n"));
	INFO_PRINTF2(_L("Backward Testing After optimised duration=%Ld  microseconds\n\n"),difftime.Int64());

	starttime.UniversalTime();
	for (i=0;i<30;i++)
		{
		for (j=40;j<70;j++)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			ret=bitmap->Load(KMbmFileEikon2,j,EFalse);
			TEST(ret==KErrNone);
			ret=bitmap->Load(KMbmFileEikon3,j,EFalse);
			TEST(ret==KErrNone);
			ret=bitmap->Load(KMbmFileEikon4,j,EFalse);
			TEST(ret==KErrNone);
			ret=bitmap->Load(KMbmFileEikon5,j,EFalse);
			TEST(ret==KErrNone);
			}
		}
	endtime.UniversalTime();
	difftime=endtime.MicroSecondsFrom(starttime);
	INFO_PRINTF1(_L("Multiple File Testing Before optimised duration= ? microseconds \n"));
	INFO_PRINTF2(_L("Multiple File Testing After optimised duration=%Ld  microseconds\n\n"),difftime.Int64());

	starttime.UniversalTime();
	for (i=0;i<30;i++)
		{
		for (j=0;j<30;j++)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		for (j=40;j<70;j++)
			{
			ret=bitmap->Load(KMbmFileEikon,j,EFalse);
			TEST(ret==KErrNone);
			}
		}
	endtime.UniversalTime();
	difftime=endtime.MicroSecondsFrom(starttime);
	INFO_PRINTF1(_L("Single File Testing Before optimised duration= ? microseconds \n"));
	INFO_PRINTF2(_L("Single File Testing After optimised duration=%Ld  microseconds\n\n"),difftime.Int64());

	bitmap->Reset();
	delete bitmap;

	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0584

	@SYMTestCaseDesc
	Tests loading of bitmaps at an offset
	(Bitmap file section offset within the file).

	@SYMTestActions
	Creates an FbsBitmap object on the heap. Loads
	a monochrone bitmap with zero offset. Checks if
	loading completed successfully. Checks the pixel
	size. Gets a pixel. Checks if pixel is KRgbBlack. 
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTStreamIdCache::TestLoadAtOffset()
	{
	CFbsBitmap* bitmap = NULL;
	TRAPD(ret, bitmap = new (ELeave) CFbsBitmap);
	TEST(ret==KErrNone);
	if(iStep->TestStepResult() != EPass)
		{
		return;
		}
	INFO_PRINTF1(_L("Testing repeated CFbsBitmap Load with different offsets"));

	do // use a do{}while(false) so we can break out of the test code and fall back to the cleanup code
		{
		ret = bitmap->Load(KBlackAndWhite, 0, EFalse, 0);
		TEST(ret==KErrNone);
		if(iStep->TestStepResult() != EPass)
			{
			INFO_PRINTF1(_L("TestLoadAtOffset: Failed to load first mbm"));
			break;
			}

		TEST( bitmap->SizeInPixels() == TSize(1,1) && bitmap->IsMonochrome() );
		if(iStep->TestStepResult() == EPass)
			{
			TRgb colour;
			bitmap->GetPixel(colour, TPoint(0,0));
			TEST( colour == KRgbBlack );
			}

		if(iStep->TestStepResult() != EPass)
			{
			INFO_PRINTF1(_L("TestLoadAtOffset: First mbm loaded incorrectly"));
			break;
			}

		ret = bitmap->Load(KBlackAndWhite, 0, EFalse, WHITE_OFFSET);
		TEST(ret==KErrNone);
		if(iStep->TestStepResult() != EPass)
			{
			INFO_PRINTF1(_L("TestLoadAtOffset: Failed to load second mbm"));
			break;
			}

		TEST( bitmap->SizeInPixels() == TSize(1,1) && bitmap->IsMonochrome() );
		if(iStep->TestStepResult() == EPass)
			{
			TRgb colour;
			bitmap->GetPixel(colour, TPoint(0,0));
			TEST( colour == KRgbWhite );
			}

		if(iStep->TestStepResult() != EPass)
			{
			INFO_PRINTF1(_L("TestLoadAtOffset: Second mbm loaded incorrectly"));
			break;
			}
		} while( EFalse );

	bitmap->Reset();
	delete bitmap;
	}
	
/**
@SYMTestCaseID			GRAPHICS-FBSERV-0509

@SYMDEF					DEF104261

@SYMTestCaseDesc		Tests file store cleanup for corresponding fbs session

@SYMTestPriority		Medium

@SYMTestStatus			Implemented

@SYMTestActions			1. Create a file store object from file stream cache using current session handle
						2. Create new file store object using some dummy session handle
						3. Check two file store object belongs to the corresponding dummy session handle.
						4. Close file store and check 
								- if corresponing session's handle file store used to close.
								- if we send wrong session handle none of the file store should close
								- if we haven't send any session handle close all the file store object irrespective of the session handle

@SYMTestExpectedResults	1. File store should close only for the corresponding session, while disconnecting the session
						2. By flushing cache should close all file stores and reset cache entries, irrespective of the session
*/
void CTStreamIdCache::TestSessionClose()
	{
	TInt ret;

	INFO_PRINTF1(_L("Test file store clean up"));

	CFbTopStreamIdCache* mbmcacheptr = new CFbTopStreamIdCache(30,30,2);
	CFbTopStreamIdCache& mbmcache = *mbmcacheptr;
	RFile file1;
	ret=file1.Open(iFs,KMbmFileEikon,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	RFile file2;
	ret=file2.Open(iFs,KMbmFileLafcurs,EFileShareReadersOnly);
	TEST(ret==KErrNone);

	//Create a file store with dummy session handle 1
	TRAP(ret,mbmcache.GetStreamIdL(file1,KMbmFileEikon,50,0,1));
	TEST(ret==KErrNone);
	//Create a file store with dummy session handle 2
	TRAP(ret,mbmcache.GetStreamIdL(file2,KMbmFileLafcurs,12,0,2));
	TEST(ret==KErrNone);
	TEST(mbmcache.iEntries.Count() == 2);
	
	TEST(2==mbmcache.iEntries[0]->SessionHandle());
	TEST(1==mbmcache.iEntries[1]->SessionHandle());
	TEST(mbmcache.iEntries[0]->iFilestore!=NULL);
	TEST(mbmcache.iEntries[1]->iFilestore!=NULL);

	mbmcache.CloseFileStores(1);
	TEST(mbmcache.iEntries.Count() == 2);
	TEST(mbmcache.iEntries[0]->iFilestore!=NULL);
	TEST(mbmcache.iEntries[1]->iFilestore==NULL);
	TEST(2==mbmcache.iEntries[0]->SessionHandle());
	TEST(0==mbmcache.iEntries[1]->SessionHandle());

	//All File stores should deleted
	mbmcache.CloseFileStores(0);
	TEST(mbmcache.iEntries.Count() == 2);
	TEST(mbmcache.iEntries[0]->iFilestore==NULL);
	TEST(mbmcache.iEntries[1]->iFilestore==NULL);
	TEST(0==mbmcache.iEntries[0]->SessionHandle());
	TEST(0==mbmcache.iEntries[1]->SessionHandle());

	// Should remove all the file store objects and reset the cache entries.
	mbmcache.FlushCache();
	TEST(mbmcache.iEntries.Count() == 0);
	
	delete mbmcacheptr;
	}

	
//--------------
__CONSTRUCT_STEP__(StreamIdCache)


void CTStreamIdCacheStep::TestSetupL()
	{
	// Leaking thread handles
	iThread.HandleCount(iStartProcessHandleCount, iStartThreadHandleCount);
	}
	
void CTStreamIdCacheStep::TestClose()
	{
	// Check for open handles
	iThread.HandleCount(iEndProcessHandleCount, iEndThreadHandleCount);
	if(iStartThreadHandleCount != iEndThreadHandleCount)
		{__DEBUGGER()}							// Oops leaked some handles
	}
