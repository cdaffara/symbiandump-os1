// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <graphics/bitmapuid.h>
#include "textendedbitmapcommon.h"
#include "textendedbitmapnegative.h"

// Values for valid test extended bitmap
const TUint8 KTestData[]		= "Extended bitmap test data 123456";
const TInt KTestDataSize		= sizeof(KTestData);
const TSize KSizeInPixels 		= TSize(50,50);
const TDisplayMode KDisplayMode	= EColor64K;

const TInt KMaxPixelSize = KMaxTInt/4; // Maximum pixel size to avoid some overflow problems
const TUint KMaxByteSize = TUint(KMaxTInt/2); // Maximum byte size to avoid other overflow problems

CTExtendedBitmapNegative::CTExtendedBitmapNegative(CTestStep* aStep):
	CTFbsBase(aStep, ETrue)
	{
	}

CTExtendedBitmapNegative::~CTExtendedBitmapNegative()
	{
	((CTExtendedBitmapNegativeStep*)iStep)->CloseTMSGraphicsStep();
	}

void CTExtendedBitmapNegative::RunFbsTestL(TInt aCurTestCase)
    {
    ((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		// CreateExtendedBitmap() tests
	case 1:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0010"));
		CreateWithoutFbsSessionL();
		break;
	case 2:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0011"));
		CreateBadPixelSizeL();
		break;
	case 3:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0012"));
		CreateBadDisplayModeL();
		break;
	case 4:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0013"));
		CreateRestrictedUidL();
		break;
	case 5:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0014"));
		CreateNullDataL();
		break;
	case 6:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0015"));
		CreateBadDataSizeL();
		break;

		// ExtendedBitmapType() tests
	case 7:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0016"));
		TypeUninitialisedBitmapL();
		break;
	case 8:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0017"));
		TypeStandardBitmapL();
		break;
	case 9:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0018"));
		TypeHardwareBitmapL();
		break;	

		// DataSize() tests
	case 10:
		((CTExtendedBitmapNegativeStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0019"));
		SizeUninitialisedBitmapL();

		// Fall through as last test case
	default:
		SetLastTestCase();
		break;
		}
	((CTExtendedBitmapNegativeStep*)iStep)->RecordTestResultL();
    }


// CreateExtendedBitmap() tests

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0010

	@SYMTestCaseDesc
	Create an extended bitmap without an FbsSession

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Test calls RFbsSession::Disconnect() and then attempts to
	create an extended bitmap using test data and test UID.

	@SYMTestExpectedResults
	Extended bitmap creation should fail returning KErrCouldNotConnect.
*/
void CTExtendedBitmapNegative::CreateWithoutFbsSessionL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with no FbsSession"));

	const TUint KMinTestThreadHeapSize = 0x00000100;
	const TUint KMaxTestThreadheapSize = 0x00100000;

	INFO_PRINTF1(_L("Creating new thread with no FbsSession"));
	RThread noFbsThread;
	CleanupClosePushL(noFbsThread);
	_LIT(KNoFbsThreadName, "CTExtendedBitmapNegative_NoFbsThread");
	User::LeaveIfError(noFbsThread.Create(KNoFbsThreadName, CreateWithoutFbsSessionStart, KDefaultStackSize, KMinTestThreadHeapSize, KMaxTestThreadheapSize, NULL));

	TRequestStatus threadStatus;
	noFbsThread.Logon(threadStatus);
	noFbsThread.SetPriority(EPriorityLess);
	
	INFO_PRINTF1(_L("Resuming new thread, which will call CreateExtendedBitmap() with valid parameters"));	
	noFbsThread.Resume();

	User::WaitForRequest(threadStatus);

	TESTEXPECTEDERRORL(noFbsThread.ExitReason(), KErrCouldNotConnect);
	CleanupStack::PopAndDestroy(&noFbsThread);
	}


/**
Function for testing with no RFbsSession connected, used by CreateWithoutFbsSessionL().
@see CreateWithoutFbsSessionL() 
@param aInfo Not used
@return Error code returned by CreateExtendedBitmap, with KErrCouldNotConnect expected.
 */
TInt CTExtendedBitmapNegative::CreateWithoutFbsSessionStart(TAny* /*aInfo*/)
	{
	TInt res = KErrNone;
	CFbsBitmap* bmp = new CFbsBitmap;
	if (bmp == NULL)
		{
		res = KErrNoMemory;
		}
	
	if (res == KErrNone)
		{
		// Call CFbsBitmap::Rasterizer() for coverage of the case where it is called without a valid FbsSession
		if (bmp->Rasterizer() != NULL)
			{
			res = KErrGeneral;
			}
		}
	
	if (res == KErrNone)
		{
		res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);		
		}
	
	delete bmp;
	
	return res;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0011

	@SYMTestCaseDesc
	Create an extended bitmap with bad pixel size

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10854

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using test data and test UID,
	passing invalid values of width/height in aSizeInPixels:
		- negative values
		- exceeding the stated limit KMaxPixelSize

	@SYMTestExpectedResults
	Extended bitmap creation should fail, with KErrArgument
	returned for negative values, and KErrTooBig returned for
	values exceeding the stated limit.
*/
void CTExtendedBitmapNegative::CreateBadPixelSizeL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with invalid pixel sizes"));
	
	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with negative width"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(-1,50), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(-1,50), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with negative height"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(40,-10), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(40,-10), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	
	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with negative width and height"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(-102,-330), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(-102,-330), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with width exceeding stated limit KMaxPixelSize"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(KMaxPixelSize + 1, 33), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(KMaxPixelSize + 1, 33), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with height exceeding stated limit KMaxPixelSize"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(43, KMaxPixelSize + 1), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(43, KMaxPixelSize + 1), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with width and height exceeding stated limit KMaxPixelSize"));	
	TESTCREATEEXTENDEDBITMAPL(TSize(KMaxPixelSize + 50, KMaxPixelSize + 1), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(TSize(KMaxPixelSize + 50, KMaxPixelSize + 1), KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrTooBig);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0012

	@SYMTestCaseDesc
	Create an extended bitmap with bad display modes

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10855

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using test data and test UID,
	passing invalid display modes.

	@SYMTestExpectedResults
	Extended bitmap creation should fail, with KErrArgument
	returned.
*/
void CTExtendedBitmapNegative::CreateBadDisplayModeL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with invalid display modes"));

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with TDisplayMode::ENone"));	
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, ENone, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, ENone, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with TDisplayMode::ERgb"));	
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, ERgb, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, ERgb, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with TDisplayMode::EColorLast"));	
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, EColorLast, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, EColorLast, KUidTestExtendedBitmap, KTestData, KTestDataSize, KErrArgument);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0013

	@SYMTestCaseDesc
	Create an extended bitmap with a UID reserved for OS-use

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10851

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using test data and a UID
	reserved for OS-use.

	@SYMTestExpectedResults
	Extended bitmap creation should fail, with KErrArgument 
	returned.
*/
void CTExtendedBitmapNegative::CreateRestrictedUidL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with reserved UIDs"));

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with KCBitwiseBitmapUid"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KCBitwiseBitmapUid, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KCBitwiseBitmapUid, KTestData, KTestDataSize, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with KCBitwiseBitmapHardwareUid"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KCBitwiseBitmapHardwareUid, KTestData, KTestDataSize, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KCBitwiseBitmapHardwareUid, KTestData, KTestDataSize, KErrArgument);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0014

	@SYMTestCaseDesc
	Create an extended bitmap with NULL data

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10850

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using a test Uid, with NULL
	passed in aData 

	@SYMTestExpectedResults
	Extended bitmap creation should fail, with KErrArgument
	returned.
*/
void CTExtendedBitmapNegative::CreateNullDataL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with null data"));

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with NULL data and aDataSize set to 0"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, NULL, 0, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, NULL, 0, KErrArgument);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with NULL data and aDataSize set to 100"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, NULL, 100, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, NULL, 100, KErrUnderflow);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0015

	@SYMTestCaseDesc
	Create an extended bitmap with bad data size

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10850

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using test data and test Uid,
	passing a negative aDataSize value.

	@SYMTestExpectedResults
	Extended bitmap creation should fail, with KErrArgument
	returned for zero data size and a negative data size. 
	KErrTooBig should be returned for data size KMaxByteSize+1.
*/
void CTExtendedBitmapNegative::CreateBadDataSizeL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with invalid data sizes"));

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with aDataSize set to 0"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, 0, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, 0, KErrArgument);
	
	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with aDataSize set to KMaxByteSize+1"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KMaxByteSize+1, KErrTooBig);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KMaxByteSize+1, KErrTooBig);

	INFO_PRINTF1(_L("Calling CreateExtendedBitmap() with negative aDataSize"));
	TESTCREATEEXTENDEDBITMAPL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, -75, KErrArgument);
	TESTCREATEEXTENDEDBITMAPINITIALIZERL(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, -75, KErrArgument);
	}


// ExtendedBitmapType() tests

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0016

	@SYMTestCaseDesc
	Gets Extended Bitmap Type on an uninitialised bitmap

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10852
	REQ10853

	@SYMTestPriority  
	Medium

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Calls ExtendedBitmapType() on an uninitialised CFbsBitmap.

	@SYMTestExpectedResults
	ExtendedBitmapType() should return KNullUid as an uninitialised CFbsBitmap is not an extended bitmap.
*/
void CTExtendedBitmapNegative::TypeUninitialisedBitmapL()
	{
	INFO_PRINTF1(_L("Get extended bitmap type on uninitialised bitmap"));

	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	TUid returnedUid = bmp->ExtendedBitmapType();
	delete bmp;

	TESTEQUALL(KNullUid, returnedUid);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0017

	@SYMTestCaseDesc
	Gets the Extended Bitmap Type on a standard bitmap

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10852
	REQ10853

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Calls ExtendedBitmapType() on a standard (regular)
	CFbsBitmap.

	@SYMTestExpectedResults
	ExtendedBitmapType() should return KNullUid as a standard CFbsBitmap is not an extended bitmap.
*/
void CTExtendedBitmapNegative::TypeStandardBitmapL()
	{
	INFO_PRINTF1(_L("Get extended bitmap type on a standard bitmap"));

	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	TESTNOERRORL(bmp->Create(KSizeInPixels, KDisplayMode));

	TUid returnedUid = bmp->ExtendedBitmapType();
	CleanupStack::PopAndDestroy(bmp);

	TESTEQUALL(KNullUid, returnedUid);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0018

	@SYMTestCaseDesc
	Gets the Extended Bitmap Type on a hardware bitmap

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10852
	REQ10853

	@SYMTestPriority  
	High

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Calls ExtendedBitmapType() on a hardware CFbsBitmap.

	@SYMTestExpectedResults
	ExtendedBitmapType() should return KNullUid as a hardware CFbsBitmap is not an extended bitmap.
*/
void CTExtendedBitmapNegative::TypeHardwareBitmapL()
	{
	INFO_PRINTF1(_L("Get extended bitmap type on a hardware bitmap"));

	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	
	TInt err = bmp->CreateHardwareBitmap(KSizeInPixels, KDisplayMode, KNullUid);		
	if (err == KErrNotSupported)
		{
		// Hardware bitmaps not supported with the current test configuration
		WARN_PRINTF1(_L("... Hardware bitmaps not supported"));
		}
	else
		{		
		TESTNOERRORL(err);
		TUid returnedUid = bmp->ExtendedBitmapType();
		TESTEQUALL(KNullUid, returnedUid);
		}		
	
	CleanupStack::PopAndDestroy(bmp);	
	}

// DataSize() tests

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0019

	@SYMTestCaseDesc
	Gets Data Size on an uninitialised bitmap

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10852	

	@SYMTestPriority  
	Medium

	@SYMTestStatus 
	Implemented

	@SYMTestActions
	Creates an extended bitmap using test data and test Uid,
	calls RFbsSession::Disconnect() and then attempts to
	call DataSize().

	@SYMTestExpectedResults
	Call should fail, with zero returned.
*/
void CTExtendedBitmapNegative::SizeUninitialisedBitmapL()
	{
	INFO_PRINTF1(_L("Get data size on uninitialised bitmap"));

	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	TInt returnedDataSize = bmp->DataSize();
	delete bmp;

	TESTEQUALL(0, returnedDataSize);
	}


//--------------
__CONSTRUCT_STEP__(ExtendedBitmapNegative)
