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

#include <f32file.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <graphics/gdi/gdiconsts.h>
#include "../sfbs/UTILS.H"
#include "trfile.h"
#include "fbsmessage.h"
#include "fbsdefs.h"

// Define FBSTEST_SCREENSHOT to save the screen to a .mbm file
//#define FBSTEST_SCREENSHOT

const TInt KNumBitmap = 3;
const TDisplayMode KDisplayMode = EColor64K;
const TSize KLargeSize(128,128);
const TSize KMidSize(64,64);
const TSize KSmallSize(32,32);


_LIT(KRamFile1,"c:\\rf1.mbm");
_LIT(KRamFile2,"c:\\rf2.mbm");
_LIT(KRamFile3,"c:\\rf3.mbm");
_LIT(KRamFile4,"c:\\rf123.mbm");
_LIT(KRomFile1,"z:\\system\\data\\rf1.mbm");
_LIT(KRomFile2,"z:\\system\\data\\rf2.mbm");
_LIT(KRomFile3,"z:\\system\\data\\rf3.mbm");
_LIT(KRomFile4,"z:\\system\\data\\rf123.mbm");


CTFile::CTFile(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL),
	iSource(KNumBitmap)
	{
	}

CTFile::~CTFile()
	{
	CleanUp();
	if (iShutdownFbs)
		iFbs->SendCommand(EFbsMessShutdown);
	iFbs->Disconnect();
	}
	

void CTFile::ConstructL()
	{
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

	TInt ret = RFbsSession::Connect();
	TEST(ret==KErrNone);
	iFbs = RFbsSession::GetSession();
	TEST(iFbs != NULL);

	ret = iFs.Connect();
	TEST(ret==KErrNone);
	// required to allow file handle to be adopted by another process
	iFs.ShareProtected();
	
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KRomFile1, romAddress)) //any ROM bitmap
		{
		INFO_PRINTF2(_L("Skipping ROM bitmap tests since file \"%S\" is reported to not be a ROM bitmap."),
				&KRomFile1);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		iSkipRomBitmapTests = ETrue;
		}
	}

void CTFile::CleanUp()
	{
	const TBufC<32> names[KNumBitmap] =
		{
		(const TDesC&)KRamFile1,
		(const TDesC&)KRamFile2,
		(const TDesC&)KRamFile3
		};
	TInt i;
	for (i=0; i<KNumBitmap; ++i)
		{
		delete iSource[i];
		iFs.Delete(names[i]);
		}
	iSource.Close();
	iFs.Delete(KRamFile4);
	iFs.Close();	
	}
	
void CTFile::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFileStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFileStep*)iStep)->SetTestStepID(_L("	GRAPHICS-FBSERV-0588"));
		CreateL();
		break;
	case 2:
		((CTFileStep*)iStep)->SetTestStepID(_L("	GRAPHICS-FBSERV-0589"));
		SaveL();
		break;
	case 3:
		((CTFileStep*)iStep)->SetTestStepID(_L("	GRAPHICS-FBSERV-0590"));
		StoreL();
		break;
	case 4:
		((CTFileStep*)iStep)->SetTestStepID(_L("	GRAPHICS-FBSERV-0591"));
		LoadL();
		break;
	case 5:
		((CTFileStep*)iStep)->SetTestStepID(_L("	GRAPHICS-FBSERV-0592"));
		LoadAndCompressL();
		break;
	case 6:
		((CTFileStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFileStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTFileStep*)iStep)->RecordTestResultL();
	}


/**
	Draw union jack
*/
void CTFile::Draw(CFbsBitGc* aGc, const TSize& aSize)
	{
	const TSize thick(aSize.iWidth/4,aSize.iWidth/4);
	const TSize thin(thick.iWidth/2,thick.iWidth/2);
	
	// background
	aGc->SetBrushColor(KRgbBlue);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->DrawRect(aSize);

	// corner to corner section
	aGc->SetPenColor(KRgbWhite);
	aGc->SetPenSize(thick);
	aGc->DrawLine(TPoint(0,0),TPoint(aSize.iWidth,aSize.iHeight));
	aGc->DrawLine(TPoint(aSize.iWidth,0),TPoint(0,aSize.iHeight));	
	aGc->SetPenColor(KRgbRed);
	aGc->SetPenSize(thin);
	aGc->DrawLine(TPoint(0,0),TPoint(aSize.iWidth,aSize.iHeight));
	aGc->DrawLine(TPoint(aSize.iWidth,0),TPoint(0,aSize.iHeight));	
	
	// cross section
	aGc->SetPenColor(KRgbWhite);
	aGc->SetPenSize(thick);
	aGc->DrawLine(TPoint(aSize.iWidth/2,0),TPoint(aSize.iWidth/2,aSize.iHeight));	
	aGc->DrawLine(TPoint(0,aSize.iHeight/2),TPoint(aSize.iWidth,aSize.iHeight/2));	
	aGc->SetPenColor(KRgbRed);
	aGc->SetPenSize(thin);
	aGc->DrawLine(TPoint(aSize.iWidth/2,0),TPoint(aSize.iWidth/2,aSize.iHeight));	
	aGc->DrawLine(TPoint(0,aSize.iHeight/2),TPoint(aSize.iWidth,aSize.iHeight/2));	
	}

/**

*/	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0588

	@SYMTestCaseDesc
	Create various size bitmap for testing (original bitmaps) 
	ie small (<4KB), mid (=4KB) and large (>4KB).
	
	@SYMTestActions	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFile::CreateL()
	{
	const TSize sizes[KNumBitmap] = 
		{
		KSmallSize,
		KMidSize,
		KLargeSize
		};
	TInt i;
	for (i=0; i<KNumBitmap; ++i)
		{
		CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bmp);
		User::LeaveIfError(bmp->Create(sizes[i],KDisplayMode));
		
		CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bmp);
		CleanupStack::PushL(dev);
		
		CFbsBitGc* gc;
		User::LeaveIfError(dev->CreateContext(gc));
		
		Draw(gc,sizes[i]);
		
		delete gc;
		CleanupStack::PopAndDestroy(dev);
		
		iSource.AppendL(bmp);
		
		CleanupStack::Pop(bmp);
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0589

	@SYMTestCaseDesc
	Save single bitmap to file using an opened file handle
	
	@SYMTestActions
	Save original bitmap to file using file handle (saved bitmap)
	Load saved bitmap using file name and compare against original
	Leave saved bitmap files for next test (Load,LoadAndCompress)
	
	@SYMTestExpectedResults
	Saved bitmap match original bitmap
*/
void CTFile::SaveL()
	{
	const TBufC<32> names[KNumBitmap] =
		{
		(const TDesC&)KRamFile1,
		(const TDesC&)KRamFile2,
		(const TDesC&)KRamFile3
		};
		
	INFO_PRINTF1(_L("Saving bitmap"));		
	TInt i;
	for (i=0; i<KNumBitmap; ++i)
		{
		RFile f;
		TInt mode = EFileWrite;
		TInt ret = f.Open(iFs,names[i],mode);
		if (ret==KErrNotFound)
			ret = f.Create(iFs,names[i],mode);
		TEST(ret==KErrNone);
		
		TEST(iSource[i]->Save(f)==KErrNone);
		f.Close();
		
		CFbsBitmap bmp;
		TEST(bmp.Load(names[i])==KErrNone);
		
		CFbsBitmap* orig = iSource[i];
		TSize size = orig->SizeInPixels();
		TInt bytes = orig->ScanLineLength(size.iWidth,KDisplayMode)*size.iHeight;
		orig->LockHeap();
		bmp.LockHeap();
		TEST(Mem::Compare((TUint8*)orig->DataAddress(),bytes,(TUint8*)bmp.DataAddress(),bytes)==0);
		orig->UnlockHeap();
		bmp.UnlockHeap();
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0590

	@SYMTestCaseDesc
	Save multi bitmaps to file using an opened file handle.
	
	@SYMTestActions
	Save original bitmaps to file (saved bitmaps).
	Load saved bitmaps and compare it against originals.
	
	@SYMTestExpectedResults
	Saved bitmaps match original bitmaps
*/
void CTFile::StoreL()
	{
	const TBufC<32> names[KNumBitmap] =
		{
		(const TDesC&)KRamFile1,
		(const TDesC&)KRamFile2,
		(const TDesC&)KRamFile3
		};
	INFO_PRINTF1(_L("Storing bitmaps"));
	RFile f;
	TInt mode = EFileWrite;
	TInt ret = f.Open(iFs,KRamFile4,mode);
	if (ret==KErrNotFound)
		ret = f.Create(iFs,KRamFile4,mode);
	TEST(ret==KErrNone);
	
	const TDesC* bmps[] = {&names[0],&names[1],&names[2]};
	TInt32 ids[] = {0,0,0};
	TRAP(ret,CFbsBitmap::StoreL(f,KNumBitmap,bmps,ids));
	TEST(ret==KErrNone);
	f.Close();
	
	TInt id;
	for (id=0; id<KNumBitmap; ++id)
		{
		CFbsBitmap bmp;
		TEST(bmp.Load(KRamFile4,id)==KErrNone);
		
		CFbsBitmap* orig = iSource[id];
		TSize size = orig->SizeInPixels();
		TInt bytes = orig->ScanLineLength(size.iWidth,KDisplayMode)*size.iHeight;
		orig->LockHeap();
		bmp.LockHeap();
		TEST(Mem::Compare((TUint8*)orig->DataAddress(),bytes,(TUint8*)bmp.DataAddress(),bytes)==0);
		orig->UnlockHeap();
		bmp.UnlockHeap();
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0591

	@SYMTestCaseDesc
	Load bitmap from file using an opened file handle.
	
	@SYMTestActions
	Load bitmap using file handle.
	Compare loaded bitmaps against original.
	
	@SYMTestExpectedResults
	Loaded bitmap match original
*/
void CTFile::LoadL()
	{
	// Load single bitmap file
	const TBufC<32> names[2*KNumBitmap] =
		{
		(const TDesC&)KRamFile1, //keep ram files at the top to make it easier to skip rom bitmaps
		(const TDesC&)KRamFile2,
		(const TDesC&)KRamFile3,
		(const TDesC&)KRomFile1,
		(const TDesC&)KRomFile2,
		(const TDesC&)KRomFile3
		};
	INFO_PRINTF1(_L("Loading bitmaps"));
	TInt i;
	TInt maxIndex = (iSkipRomBitmapTests ? KNumBitmap - 1 : 2 * KNumBitmap - 1);
	for (i=0; i <= maxIndex; ++i)
		{
		RFile f;
		TInt mode = EFileShareReadersOnly;
		TInt ret = f.Open(iFs,names[i],mode);
		TEST(ret==KErrNone);
		CFbsBitmap bmp;
		TEST(bmp.Load(f)==KErrNone);
		f.Close();
		
		CFbsBitmap* orig = iSource[i%KNumBitmap];
		TSize size = orig->SizeInPixels();
		TInt bytes = orig->ScanLineLength(size.iWidth,KDisplayMode)*size.iHeight;
		orig->LockHeap();
		bmp.LockHeap();
		TEST(Mem::Compare((TUint8*)orig->DataAddress(),bytes,(TUint8*)bmp.DataAddress(),bytes)==0);
		orig->UnlockHeap();
		bmp.UnlockHeap();
		}
		
	// Load multi bitmap file
	const TBufC<32> mbm[] =
		{
		(const TDesC&)KRamFile4, //keep ram files at the top to make it easier to skip rom bitmaps
		(const TDesC&)KRomFile4
		};
	maxIndex = (iSkipRomBitmapTests ? 0 : 1);
	for (i=0; i <= maxIndex; ++i)
		{
		RFile f;
		TInt mode = EFileShareReadersOnly;
		TInt ret = f.Open(iFs,mbm[i],mode);
		TEST(ret==KErrNone);
		TInt id;
		for (id=0; id<KNumBitmap; ++id)
			{
			CFbsBitmap bmp;
			TEST(bmp.Load(f,id)==KErrNone);
			
			CFbsBitmap* orig = iSource[id];
			TSize size = orig->SizeInPixels();
			TInt bytes = orig->ScanLineLength(size.iWidth,KDisplayMode)*size.iHeight;
			orig->LockHeap();
			bmp.LockHeap();
			TEST(Mem::Compare((TUint8*)orig->DataAddress(),bytes,(TUint8*)bmp.DataAddress(),bytes)==0);
			orig->UnlockHeap();
			bmp.UnlockHeap();
			}
		f.Close();
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0592

	@SYMTestCaseDesc
	Load and compress bitmap from file using an opened file handle.
	
	@SYMTestActions
	Compress original bitmap.
	Load and compress bitmap using file handle.
	Blit loaded+compressed bitmaps and compressed original to screen.
	Compare screen area.
	
	@SYMTestExpectedResults
	LoadAndCompress ROM file return KErrAccessDenied.
	Drawn bitmap on screen match.
*/
void CTFile::LoadAndCompressL()
	{
	INFO_PRINTF1(_L("Loading and compressing bitmaps"));

	RFile f;
	TInt ret = KErrNone;
	if(!iSkipRomBitmapTests)
		{
		// load and compress ROM file
		ret = f.Open(iFs,KRomFile1,EFileShareReadersOnly);
		TEST(ret==KErrNone);
		CFbsBitmap bmp;
		TEST(bmp.LoadAndCompress(f)==KErrAccessDenied);
		f.Close();
		}

	// load single bitmap file
	const TBufC<32> names[KNumBitmap] =
		{
		(const TDesC&)KRamFile1,
		(const TDesC&)KRamFile2,
		(const TDesC&)KRamFile3
		};
	TInt i;
	for (i=0; i<KNumBitmap; ++i)
		{
		ret = f.Open(iFs,names[i],EFileShareReadersOnly);
		TEST(ret==KErrNone);
		CFbsBitmap bmp;
		TEST(bmp.LoadAndCompress(f)==KErrNone);
		f.Close();
		TEST(bmp.IsCompressedInRAM());
		
		CFbsBitmap* orig = iSource[i];
		TEST(orig->Compress()==KErrNone);
		TEST(orig->IsCompressedInRAM());

		TRAP(ret,BlitAndCompareL(orig,&bmp,orig->SizeInPixels()));
		TEST(ret==KErrNone);
		}
	// Load multi bitmap KRamFile4
	ret = f.Open(iFs,KRamFile4,EFileShareReadersOnly);
	TEST(ret==KErrNone);
	TInt id;
	for (id=0; id<KNumBitmap; ++id)
		{
		CFbsBitmap bmp;
		TEST(bmp.LoadAndCompress(f,id)==KErrNone);
		TEST(bmp.IsCompressedInRAM());
		
		CFbsBitmap* orig = iSource[id];
		TEST(orig->Compress()==KErrNone);
		TEST(orig->IsCompressedInRAM());

		TRAP(ret,BlitAndCompareL(orig,&bmp,orig->SizeInPixels()));
		TEST(ret==KErrNone);
		}
	f.Close();
	}

/**
	Blit two bitmap to screen device and compare if both match
*/
void CTFile::BlitAndCompareL(CFbsBitmap* aBmpOrig,CFbsBitmap* aBmpTest,const TSize& aBmpSize)
	{
	CFbsScreenDevice* scd=NULL;
	TRAPD(err,scd=CFbsScreenDevice::NewL(KDefaultScreenNo,KDisplayMode));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device for mode %S  err=%d");
		INFO_PRINTF3(KLog,&ColorModeName(KDisplayMode),err);
		}
	if (err==KErrNotSupported)
		{
		TRAPD(err,scd=CFbsScreenDevice::NewL(KDefaultScreenNo,EColor256));
		if (err!=KErrNone)
			{
			_LIT(KLog,"Failed to create Color256 screen device  err=%d");
			INFO_PRINTF2(KLog,err);
			}
		}
	if (err==KErrNotSupported)
		{
		TRAPD(err,scd=CFbsScreenDevice::NewL(KDefaultScreenNo,EColor16MA));
		if (err!=KErrNone)
			{
			_LIT(KLog,"Failed to create Color16MA screen device  err=%d");
			INFO_PRINTF2(KLog,err);
			}
		}
	if (err==KErrNotSupported)
		{
		TRAPD(err,scd=CFbsScreenDevice::NewL(KDefaultScreenNo,EColor16MAP));
		if (err!=KErrNone)
			{
			_LIT(KLog,"Failed to create Color16MA screen device  err=%d");
			INFO_PRINTF2(KLog,err);
			}
		}
	_LIT(KLog,"Create screen device with mode %S");
	INFO_PRINTF2(KLog,&ColorModeName(scd->DisplayMode()));
	CleanupStack::PushL(scd);
	scd->ChangeScreenDevice(NULL);
	CFbsBitGc* sgc;
	User::LeaveIfError(scd->CreateContext((CGraphicsContext*&)sgc)); 
	CleanupStack::PushL(sgc);

	TSize scrSize = scd->SizeInPixels();
	TPoint left(0,0);
	TPoint right(scrSize.iWidth/2,0);
	TRect bmpRect(TPoint(0,0),aBmpSize);

	// if bitmap width is greater than half screen width,
	// try positioning right bitmap below original.
	if (scrSize.iWidth < aBmpSize.iWidth*2)
		{
		if (scrSize.iHeight < aBmpSize.iHeight/2)
			{
			//That still doesn't work out, so restrict size of rect
			// being blitted to half screen size.
			INFO_PRINTF1(_L("Warning: the whole of the test images may not be compared"));
			bmpRect = TRect(TPoint(0,0),TSize(scrSize.iWidth/2,aBmpSize.iHeight));
			}
		else
			{
			right = TPoint(0, scrSize.iHeight/2);
			}
		}
	
	sgc->BitBlt(left, aBmpOrig, bmpRect);
	sgc->BitBlt(right, aBmpTest, bmpRect);
	scd->Update();
	
#ifdef FBSTEST_SCREENSHOT
	SaveScreenShotL(scd);
#endif //FBSTEST_SCREENSHOT
	
	TBool match=scd->RectCompare(TRect(left,bmpRect.Size()),*scd,TRect(right,bmpRect.Size()));
	if (!match)
		{
		_LIT(KLog,"Two areas of the screen don't match");
		INFO_PRINTF1(KLog);
		}
	TEST(match);

	CleanupStack::PopAndDestroy(2,scd);
	}

//--------------
__CONSTRUCT_STEP__(File)
