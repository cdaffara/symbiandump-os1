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

#include <s32file.h>
#include <graphics/bitmapuid.h>
#include "../sfbs/UTILS.H"
#include "TFBS.H"
#include "TALLOC.H"
#include "../sfbs/fbsbitmapasyncstreamer.h"
#include "fbsmessage.h"
#include "fbsdefs.h"

const TSize KLargeSize(4096 + 100, 1);
const TSize KLargeSizeAlt(4096 + 200, 2);
const TSize KSmallSize(10, 10);
const TSize KSmallSizeAlt(20, 20);

CTAlloc::CTAlloc(CTestStep* aStep):
	CTGraphicsBase(aStep), 
	iFbs(NULL),
	iStore(NULL),
	iBitmap(NULL),
	iBitmap2(NULL)
	{
	INFO_PRINTF1(_L("Font/Bitmap Server Alloc Failure Recovery Testing"));
	}

CTAlloc::~CTAlloc()
	{
	if (iShutdownFbs)
		{
		iFbs->SendCommand(EFbsMessShutdown);
		}
	iFbs->Disconnect();
	
	delete iStore;
	}

void CTAlloc::ConstructL()
	{
	iStore = CFbsTypefaceStore::NewL(NULL);

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
	}

void CTAlloc::RunTestCaseL(TInt aCurTestCase)
	{
	((CTAllocStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0511"));
		Session();
		break;
	case 2:
		{
/**
	@SYMTestCaseID GRAPHICS-FBSERV-0611
*/
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0611"));
		TInt ret = RFbsSession::Connect();
		TEST2(ret, KErrNone);
		iFbs = RFbsSession::GetSession();
		TEST(iFbs != NULL);
		}
		break;
	case 4:
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0512"));
		BitmapsL();
		break;
	case 5:
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0514"));
		BitmapHandlerL();
		break;
	case 6:
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0513"));
		TypefaceStoreL();
		break;
	case 7:
		((CTAllocStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0515"));
		FontStoreL();
		break;
	case 8:
		((CTAllocStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAllocStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
	default:
		((CTAllocStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		break;
		}
	((CTAllocStep*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0511

	@SYMTestCaseDesc
	Allocated sessions with the FbsServer and simulates memory allocation errors

	@SYMTestActions
	Checks the heap

	@SYMTestExpectedResults
	Test should pass
*/
void CTAlloc::Session()
	{
	INFO_PRINTF1(_L("RFbsSession::Connect()\r\n"));

	for (TInt count = 1; ; count++)
		{
		__UHEAP_FAILNEXT(count);
		__UHEAP_MARK;

		TInt ret = RFbsSession::Connect();

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			RFbsSession::Disconnect();
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0512

	@SYMTestCaseDesc
	Creates bitmaps in different ways and checks if memory allocation succeeded 

	@SYMTestActions
	Creates normal size bitmaps, large bitmaps, loads bitmaps, loads shared bitmaps,
	duplicated bitmaps, resizes bitmaps, compresses bitmaps, externalized bitmaps to write store, 
	internalizes bitmaps back again from write store. All ways of creating bitmaps are tested
	against Out Of Memory error conditions	

	@SYMTestExpectedResults
	Test should pass
*/
void CTAlloc::BitmapsL()
	{
	TInt heapMarkCheckFlag;	
	if( !iStep->GetIntFromConfig(KDefaultSectionName, KFbsHeapMarkCheckFlag, heapMarkCheckFlag) )
		{
		INFO_PRINTF1(_L("Error reading ini file"));
		User::Leave(KErrNotFound);
		}
	
	CFbsBitmap iBitmap;
	CFbsBitmap iBitmap2;

	iTestBitmapName = KTestBitmapOnZ;
	TInt ret = iBitmap.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);

	ret = iBitmap.Create(KLargeSize,EColor256); // Expand server caches
	TEST2(ret, KErrNone);
	iBitmap.Reset();

	TInt count;

// Create bitmap
	INFO_PRINTF1(_L("CFbsBitmap::Create()\r\n"));
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}
		ret = iBitmap.Create(KSmallSize,EColor256);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMark);
				iFbs->SendCommand(EFbsMessUserMark);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMark);
				iFbs->SendCommand(EFbsMessUserMark);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret=iBitmap.Create(KSmallSize,EGray2);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

// Create (large)
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Create(KLargeSize,EColor256);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Create(KLargeSize,EColor256);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

// Load
	INFO_PRINTF1(_L("CFbsBitmap::Load()\r\n"));
	ret = iBitmap.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);
	iBitmap.Reset();
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Load(iTestBitmapName,ETfbs,NULL);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Load(iTestBitmapName,ETfbs,NULL);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
	}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

// Load (shared)
	INFO_PRINTF1(_L("CFbsBitmap::Load() - shared \r\n"));
	
	// Do an initial load to ensure the StreamId cache does not get updated whilst OOM testing
	ret = iBitmap.Load(iTestBitmapName,ETfbs,ETrue);
	TEST2(ret, KErrNone);
	iBitmap.Reset();
	
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Load(iTestBitmapName,ETfbs,ETrue);
		
		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			continue;
			}
		else if (ret != KErrNone)
			TEST2(ret, KErrNone);
			
		ret = iBitmap2.Load(iTestBitmapName,ETfbs,ETrue);	
			
		if (ret == KErrNoMemory)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			continue;
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();	
			iBitmap2.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);		
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Load(iTestBitmapName,ETfbs,ETrue);
		
		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			continue;
			}
		else if (ret != KErrNone)
			TEST2(ret, KErrNone);
			
		ret = iBitmap2.Load(iTestBitmapName,ETfbs,ETrue);

		if (ret == KErrNoMemory)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			continue;
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			iBitmap2.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
	}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);
	
	
// Duplicate
	CFbsBitmap bmpalt;
	ret = bmpalt.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::Duplicate()\r\n"));
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Duplicate(bmpalt.Handle());

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==1);
	ret = bmpalt.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Duplicate(bmpalt.Handle());

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	bmpalt.Reset();
	TEST(iFbs->ResourceCount()==0);

// Resize
	// force fbserv to do a resize. This makes the pile to insert an index in it's array
	// If the array is empty when the tests below are run then the insert will look like a mem leak
	ret=iBitmap.Create(KLargeSize,EColor256);
	iBitmap.Resize(KLargeSizeAlt);
	iBitmap.Reset();
	
	ret=iBitmap.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::Resize()\r\n"));
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Resize(KSmallSizeAlt);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);
	
	
	// force fbserv to do a resize. This makes the pile to insert an index in it's array
	// If the array is empty when the tests below are run then the insert will look like a mem leak
	ret=iBitmap.Create(KLargeSize,EColor256);
	iBitmap.Resize(KLargeSizeAlt);
	iBitmap.Reset();
	
	ret=iBitmap.Create(KLargeSize,EColor256);
	TEST2(ret, KErrNone);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Resize(KLargeSizeAlt);

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

// Compress
	ret=iBitmap.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::Compress()\r\n"));
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Compress();

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	ret=iBitmap.Create(KLargeSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::Compress()\r\n"));
	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		ret = iBitmap.Compress();

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

// WriteStore creation
	RFs fs;
	ret=fs.Connect();
	TEST2(ret, KErrNone);
	CDirectFileStore* writestore=NULL;

	//Construct filename using the thread id to enable concurrent test runs.
	_LIT(KSbmFileName,"c:\\tall_");
	TBuf<36> buf(KSbmFileName);
	TThreadId threadId = RThread().Id();
	TUint64 id = threadId.Id();
	TBuf<20> threadIdBuf;
	threadIdBuf.Num(id);
	buf.Append(threadIdBuf);
	buf.Append(_L(".sbm"));
	
	TRAP(ret,writestore=CDirectFileStore::ReplaceL(fs,buf,EFileStream|EFileWrite));
	TEST2(ret, KErrNone);
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	TRAP(ret,writestore->SetTypeL(uidtype));
	TEST2(ret, KErrNone);

	RStoreWriteStream writestrm;
	TStreamId headerid(0);
	TRAP(ret,headerid=writestrm.CreateL(*writestore));
	TEST2(ret, KErrNone);
	TRAP(ret,writestore->SetRootL(headerid));
	TEST2(ret, KErrNone);

// Externalize
	ret=iBitmap.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::ExternalizeL()\r\n"));
	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		TRAP(ret,iBitmap.ExternalizeL(writestrm));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	TEST(iFbs->ResourceCount()==0);

// ExternalizeRectangle
	ret=iBitmap.Create(KSmallSize,EColor256);
	TEST2(ret, KErrNone);

	INFO_PRINTF1(_L("CFbsBitmap::ExternalizeRectangleL()\r\n"));
	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		TRAP(ret,iBitmap.ExternalizeRectangleL(writestrm,TRect(3,3,7,7)));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	writestrm.Close();
	delete writestore;
	iBitmap.Reset();
	TEST(iFbs->ResourceCount()==0);

// Internalize
	INFO_PRINTF1(_L("CFbsBitmap::InternalizeL()\r\n"));
	for (count = 1; ; count++)
		{
		// ReadStore creation
		CDirectFileStore* readstore=NULL;
		TRAP(ret,readstore=CDirectFileStore::OpenL(fs,buf,EFileStream|EFileRead|EFileShareAny));
		TEST2(ret, KErrNone);
		RStoreReadStream readstrm;
		headerid=readstore->Root();
		TRAP(ret,readstrm.OpenL(*readstore,headerid));
		TEST2(ret, KErrNone);

		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		TRAP(ret,iBitmap.InternalizeL(readstrm));

		readstrm.Close();
		delete readstore;

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	for (count = 1; ; count++)
		{
		// ReadStore re-creation
		CDirectFileStore* readstore=NULL;
//		TRAP(ret,readstore=CDirectFileStore::OpenL(fs,_L("c:\\tall.sbm"),EFileStream|EFileRead|EFileShareAny));
		TRAP(ret,readstore=CDirectFileStore::OpenL(fs,buf,EFileStream|EFileRead|EFileShareAny));
		TEST2(ret, KErrNone);
		headerid=readstore->Root();
		RStoreReadStream readstrm;
		TRAP(ret,readstrm.OpenL(*readstore,headerid));
		TEST2(ret, KErrNone);

		iFbs->SendCommand(EFbsMessUserAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			iFbs->SendCommand(EFbsMessUserMark);
			}

		TRAP(ret,iBitmap.InternalizeL(readstrm));

		readstrm.Close();
		delete readstore;

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			iBitmap.Reset();
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				iFbs->SendCommand(EFbsMessUserMarkEnd);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);
	iFbs->SendCommand(EFbsMessUserAllocFail,0);
	TEST(iFbs->ResourceCount()==0);

	fs.Delete(buf);
	fs.Close();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0513

	@SYMTestCaseDesc
	Checks allocations of CFbsTypeFaceStore Objects OOM conditions

	@SYMTestActions
	Uses different fonts and allocates CFbsTypeFaceStore objects
	and checks them under Out Of Memory conditions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTAlloc::TypefaceStoreL()
	{
	TInt count;
	TInt ret;
	CFbsTypefaceStore* tfs = NULL;
	INFO_PRINTF1(_L("CFbsTypefaceStore::NewL()\r\n"));

	TInt heapMarkCheckFlag;
	if( !iStep->GetIntFromConfig(KDefaultSectionName, KFbsHeapMarkCheckFlag, heapMarkCheckFlag) )
		{
		INFO_PRINTF1(_L("Error reading ini file"));
		User::Leave(KErrNotFound);
		}
	
// CFbsTypefaceStore::NewL()
	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		TRAP(ret,tfs=CFbsTypefaceStore::NewL(NULL));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			delete tfs;
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;

// CFbsTypefaceStore::SetFontNameAliasL()
	_LIT(KFontNameAlias,"Fubar");
	TRAP(ret,tfs = CFbsTypefaceStore::NewL(NULL));
	TEST2(ret, KErrNone);
	TRAP(ret, tfs->SetFontNameAliasL(_L("ExpandArray"),_L("DejaVu Serif Condensed")));
	TEST2(ret, KErrNone);
	TRAP(ret, tfs->SetFontNameAliasL(_L("ExpandArray"),KNullDesC));
	TEST2(ret, KErrNone);

	for (count = 1; ; count++)
		{
		iFbs->SendCommand(EFbsMessDefaultAllocFail,count);
		if(heapMarkCheckFlag)
			{
			iFbs->SendCommand(EFbsMessDefaultMark);
			}

		TRAP(ret, tfs->SetFontNameAliasL(KFontNameAlias,_L("DejaVu Serif Condensed")));

		if (ret == KErrNoMemory)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd);
				}
			}
		else if (ret == KErrNone)
			{
			if(heapMarkCheckFlag)
				{
				iFbs->SendCommand(EFbsMessDefaultMarkEnd,2);
				}
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	iFbs->SendCommand(EFbsMessDefaultAllocFail,0);

// GetNearestFontToDesignHeightInTwips()
	TFontSpec spec(_L("SwissA"),12);
	CFont* font = NULL;
	ret = tfs->GetNearestFontToDesignHeightInTwips(font,spec);
	TEST2(ret, KErrNone);
	tfs->ReleaseFont(font);
	spec.iTypeface.iName = _L("DejaVu Serif Condensed");

	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;
		
		ret = tfs->GetNearestFontToDesignHeightInTwips(font,spec);

		if (ret == KErrNoMemory)
			{
			ASSERT(!font);
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			ASSERT(font);
			tfs->ReleaseFont(font);
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;

// GetNearestFontToDesignHeightInTwips() with alias
	spec.iTypeface.iName = KFontNameAlias;
	TRAP(ret, tfs->SetFontNameAliasL(KFontNameAlias,_L("DejaVu Serif Condensed")));
	TEST2(ret, KErrNone);

	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		ret = tfs->GetNearestFontToDesignHeightInTwips(font,spec);

		if (ret == KErrNoMemory)
			{
			ASSERT(!font);
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			ASSERT(font);
			tfs->ReleaseFont(font);
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	TRAP(ret, tfs->SetFontNameAliasL(KFontNameAlias,KNullDesC));
	TEST2(ret, KErrNone);
	delete tfs;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0514

	@SYMTestCaseDesc
	Asynchronous loading and saving of bitmaps

	@SYMTestActions
	Creates a loader for bitmaps and simulates Out Of Memory conditions
	and then creates and saver and simulates Out Of Memory conditions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTAlloc::BitmapHandlerL()
	{
	TInt ret=KErrNone;
	CFbsBitmapAsyncStreamer* bmphandler=NULL;

// Create loader
	TInt count;
	INFO_PRINTF1(_L("CFbsBitmapAsyncStreamer::NewL()\r\n"));
	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		TRAP(ret,bmphandler = CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ELoad));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			delete bmphandler;
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	TEST(iFbs->ResourceCount()==0);

// Create saver
	for (count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		TRAP(ret,bmphandler=CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ESave));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			delete bmphandler;
			__UHEAP_MARKEND;
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	__UHEAP_RESET;
	TEST(iFbs->ResourceCount()==0);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0515

	@SYMTestCaseDesc
	Tests memory allocation of CFontStore object

	@SYMTestActions
	CFontStore objects is created on the heap
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTAlloc::FontStoreL()
	{
	INFO_PRINTF1(_L("CFontStore::NewL()\r\n"));

	for (TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		CFontStore* fs = NULL;
		TRAPD(ret,fs = CFontStore::NewL(&User::Heap()));

		if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			delete fs;
			__UHEAP_MARKEND;
			__UHEAP_SETFAIL(RHeap::ENone,count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(ret, KErrNone);
			}
		}

	}

//--------------
__CONSTRUCT_STEP__(Alloc)

