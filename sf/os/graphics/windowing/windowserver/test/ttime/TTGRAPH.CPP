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
// used for timing graphics
// 
//

#include "TTIME.H"

#define TEST_BITMAP_NAME _L("Z:\\WSTEST\\TEST.MBM")

GLREF_C void Panic(TInt aPanic);

enum TFuncType
	{
	EBitmapTest,
	EXorTest,
	ESmallClearTest,
	ERectCompareTest,
	EUseFontTest,
	EBitBltTest,
	EFullScreenBitBltTest,
	EMaskedBitBltTest,
	EFillPatternTest,
	EBackupWindowDrawingTest,
	};

class TGraphicsTest : public CBase
	{
public:
	void DoTestL(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2);
	void DrawBitmapTestL(TInt aParam1, TAny *aParam2);
	void XorTest(TInt aParam1, TAny *aParam2);
	void SmallClearTest(TInt , TAny *);
	void RectCompareTest(TInt , TAny *);
	void UseFontTestL(TInt , TAny *);
	void BitBltTestL(TInt , TAny *);
	void FullScreenBitBltTestL(TInt , TAny *);
	void MaskedBitBltTestL(TInt , TAny *);
	void FillPatternTestL(TInt , TAny *);
	void BackedUpWindowDrawingL(TInt aMode, TAny *);
private:
	RWsSession iWs;
	CWsScreenDevice *iDevice;
	RWindowGroup iGroup;
	RWindow iWindow;
	RBackedUpWindow iBackedUpWindow;
	RDrawableWindow *iWindowPtr;
	CWindowGc *iGc;
	};
	
TInt CreateGraphicsTest(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2)
	{
	TGraphicsTest *iTest=NULL;
	TRAPD(err,iTest=new(ELeave) TGraphicsTest());
	if (err==KErrNone)
		{
		TRAP(err,iTest->DoTestL(aOwningGroup, aFunc, aParam1, aParam2));
		delete iTest;
		}
	return(err);
	}

void TGraphicsTest::DoTestL(TInt aOwningGroup, TFuncType aFunc, TInt aParam1, TAny *aParam2)
	{
	iWs.Connect();
	iDevice=new(ELeave) CWsScreenDevice(iWs);
	iDevice->Construct();
	iGroup=RWindowGroup(iWs);
	iGroup.Construct(ENullWsHandle);
	iGroup.SetOwningWindowGroup(aOwningGroup);
//
	if (aFunc==EBackupWindowDrawingTest)
		{
		iBackedUpWindow=RBackedUpWindow(iWs);
		iWindowPtr=&iBackedUpWindow;
		iBackedUpWindow.Construct(iGroup,EGray4,ENullWsHandle);
		}
	else
		{
		iWindow=RWindow(iWs);
		iWindowPtr=&iWindow;
		iWindow.Construct(iGroup,ENullWsHandle);
		}
	User::LeaveIfError(iWindowPtr->SetExtentErr(TPoint(), iDevice->SizeInPixels()));
	iWindowPtr->Activate();
	//
	iDevice->CreateContext(iGc);
	iGc->Activate(*iWindowPtr);
	if (iWindowPtr==&iWindow)
		{
		iWindow.BeginRedraw();
		iGc->Clear();
		iWindow.EndRedraw();
		}
	switch(aFunc)
		{
		case EBitmapTest:
			DrawBitmapTestL(aParam1, aParam2);
			break;
		case EXorTest:
			XorTest(aParam1, aParam2);
			break;
		case ESmallClearTest:
			SmallClearTest(aParam1, aParam2);
			break;
		case ERectCompareTest:
			RectCompareTest(aParam1, aParam2);
			break;
		case EUseFontTest:
			UseFontTestL(aParam1, aParam2);
			break;
		case EBitBltTest:
			BitBltTestL(aParam1, aParam2);
			break;
		case EFullScreenBitBltTest:
			FullScreenBitBltTestL(aParam1, aParam2);
			break;
		case EMaskedBitBltTest:
			MaskedBitBltTestL(aParam1, aParam2);
			break;
		case EFillPatternTest:
			FillPatternTestL(aParam1, aParam2);
			break;
		case EBackupWindowDrawingTest:
			BackedUpWindowDrawingL(aParam1, aParam2);
			break;
		default:;
		}
//
	delete iGc;
	iWindowPtr->Close();
	iGroup.Close();
	delete iDevice;
	iWs.Close();
	}

// Draw bitmap //

void TGraphicsTest::DrawBitmapTestL(TInt , TAny *)
	{
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap;
	User::LeaveIfError(bitmap->Load(TEST_BITMAP_NAME,0));
	for(TInt nTimes=0;nTimes<10;nTimes++)
		{
		iGc->Clear();
		TSize size(iDevice->SizeInPixels());
		iGc->DrawBitmap(TRect(-size.iWidth,-size.iHeight,size.iWidth<<1,size.iHeight<<1),bitmap);
		iWs.Flush();
		}
	delete bitmap;
	}

TInt DrawBitmapTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EBitmapTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader DrawBitmapTest={_S("Draw bitmap"),DrawBitmapTestFunc};

// XOR Test //

void TGraphicsTest::XorTest(TInt , TAny *)
	{
	iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iGc->SetBrushColor(TRgb::Gray256(255));
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	for(TInt count=0;count<10;count++)
		{
		for(TInt wid=1;wid<320;wid+=3)
			{
			iGc->DrawRect(TRect(10,10,10+wid,150));
	//		iWs.Flush();
			}
		}
	}

TInt XorIngTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EXorTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader XorIngTest={_S("Xor'ing"),XorIngTestFunc};

// XOR Test //

void TGraphicsTest::SmallClearTest(TInt , TAny *)
	{
	iGc->SetBrushColor(TRgb::Gray256(255));
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	for(TInt count=0;count<500;count++)
		{
		for(TInt wid=1;wid<30;wid++)
			{
			iGc->DrawRect(TRect(1,0,10+wid,100));
//			iWs.Flush();
			}
		}
	}

TInt SmallClearTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, ESmallClearTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader SmallClearTest={_S("Small clear rect"),SmallClearTestFunc};

// XOR Test //

enum {EMaxWidth=100};

void TGraphicsTest::RectCompareTest(TInt , TAny *)
	{
	TSize size(iDevice->SizeInPixels());
	for(TInt count=0;count<10;count++)
		iDevice->RectCompare(TRect(0,0,size.iWidth>>1,size.iHeight),TRect(size.iWidth>>1,0,size.iWidth,size.iHeight));
	}

TInt RectCompareTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, ERectCompareTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader RectCompareTest={_S("RectCompare"),RectCompareTestFunc};

// Use Font //

void TGraphicsTest::UseFontTestL(TInt , TAny *)
	{
	CFbsFont *font;
	TFontSpec fspec(KTestFontTypefaceName,200);
	User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInTwips((CFont *&)font, fspec));
	for(TInt count=0;count<1000;count++)
		iGc->UseFont(font);
	iDevice->ReleaseFont(font);
	}

TInt UseFontTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EUseFontTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader UseFontTest={_S("UseFont(x1000)"),UseFontTestFunc};

// Small BitBlt //

void TGraphicsTest::BitBltTestL(TInt , TAny *)
	{
	CWsBitmap *bitmap=new(ELeave) CWsBitmap(iWs);
	TSize size(25,50);
	bitmap->Create(size,EGray4);
	CFbsDevice *bitmapDevice=CFbsBitmapDevice::NewL(bitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(bitmapDevice);
	gc->DrawEllipse(TRect(size));
	delete gc;
	delete bitmapDevice;
	for(TInt count=0;count<10;count++)
		{
		iGc->Clear();
		TPoint pos(0,0);
		for(TInt xcount=0;xcount<25;xcount++,pos.iX+=size.iWidth)
			{
			pos.iY=0;
			for(TInt ycount=0;ycount<4;ycount++,pos.iY+=size.iHeight)
				iGc->BitBlt(pos,bitmap);
			}
		}
	delete bitmap;
	}

TInt BitBltTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EBitBltTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader BitBltTest={_S("BitBlt"),BitBltTestFunc};

// Full Screen BitBlt //

void TGraphicsTest::FullScreenBitBltTestL(TInt , TAny *)
	{
	CWsBitmap *bitmap=new(ELeave) CWsBitmap(iWs);
	TSize size(640,240);
	User::LeaveIfError(bitmap->Create(size,EGray4));
	CFbsDevice *bitmapDevice=CFbsBitmapDevice::NewL(bitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(bitmapDevice);
	for(TInt pos=0;pos<size.iWidth;pos+=8)
		{
		gc->DrawRect(TRect(pos,0,pos+16,size.iHeight));
		iGc->BitBlt(TPoint(0,0),bitmap);
		iWs.Flush();
		}
	delete gc;
	delete bitmapDevice;
	delete bitmap;
	}

TInt FullScreenBitBltTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EFullScreenBitBltTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader FullScreenBitBltTest={_S("FullScreenBitBlt"),FullScreenBitBltTestFunc};

// Masked BitBlt //

void TGraphicsTest::MaskedBitBltTestL(TInt , TAny *)
	{
	TSize size(24,48);
	CWsBitmap *bitmap=new(ELeave) CWsBitmap(iWs);
	bitmap->Create(size,EGray4);
	CFbsDevice *bitmapDevice=CFbsBitmapDevice::NewL(bitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(bitmapDevice);
	gc->DrawEllipse(TRect(size));
	delete bitmapDevice;
// Now do the mask
	CWsBitmap *mask=new(ELeave) CWsBitmap(iWs);
	mask->Create(size,EGray4);
	bitmapDevice=CFbsBitmapDevice::NewL(mask);
	gc->Activate(bitmapDevice);
	gc->SetPenColor(TRgb::Gray4(3));
	gc->DrawEllipse(TRect(size));
	delete bitmapDevice;
//
	delete gc;
	for(TInt count=0;count<10;count++)
		{
		iGc->Clear();
		TPoint pos(0,0);
		for(TInt xcount=0;xcount<25;xcount++,pos.iX+=size.iWidth+1)
			{
			pos.iY=0;
			for(TInt ycount=0;ycount<4;ycount++,pos.iY+=size.iHeight)
				iGc->BitBltMasked(pos,bitmap,TRect(size),mask,EFalse);
			}
		}
	delete bitmap;
	delete mask;
	}

TInt MaskedBitBltTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EMaskedBitBltTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader MaskedBitBltTest={_S("MaskedBitBlt"),MaskedBitBltTestFunc};

// Fill Pattern //

void TGraphicsTest::FillPatternTestL(TInt , TAny *)
	{
	TSize scrSize(iDevice->SizeInPixels());
	TSize rectSize(scrSize.iWidth/5-1,scrSize.iHeight/2);

	CWsBitmap *bitmap=new(ELeave) CWsBitmap(iWs);

	TSize bitmapSize(50,40);
	bitmap->Create(bitmapSize,EGray4);
	CFbsDevice *bitmapDevice=CFbsBitmapDevice::NewL(bitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(bitmapDevice);
	gc->SetBrushColor(TRgb::Gray4(2));
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->DrawEllipse(TRect(bitmapSize));
	delete bitmapDevice;
	delete gc;
//
	iGc->UseBrushPattern(bitmap);
	iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	for(TInt count=0;count<50;count++)
		{
		iGc->Clear();
		TPoint pos(0,0);
		for(TInt xcount=0;xcount<5;xcount++,pos.iX+=rectSize.iWidth)
			{
			pos.iY=0;
			for(TInt ycount=0;ycount<2;ycount++,pos.iY+=rectSize.iHeight)
				iGc->DrawRect(TRect(pos,rectSize));
			}
		}
	delete bitmap;
	}

TInt FillPatternTestFunc(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EFillPatternTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader FillPatternTest={_S("FillPattern"),FillPatternTestFunc};

// Backup Window Drawing //

void TGraphicsTest::BackedUpWindowDrawingL(TInt aMode, TAny *)
	{
	TSize scrSize(iDevice->SizeInPixels());
	CFbsFont *font=NULL;
	if (aMode==1)
		{
		TFontSpec fspec(KTestFontTypefaceName,200);
		User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInTwips((CFont *&)font, fspec));
		iGc->UseFont(font);
		}
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	TPoint pos;
	for(TInt count=0;count<10;count++)
		{
		iGc->Clear();
		for(pos.iY=0;pos.iY<scrSize.iHeight;pos.iY++)
			iGc->DrawLine(pos,pos+TSize(scrSize.iWidth,0));
		}
	if (aMode==1)
		iDevice->ReleaseFont(font);
	}

TInt BackupWindowDrawingFunc1(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EBackupWindowDrawingTest, 0, NULL));
	}

GLDEF_D TTimeTestHeader BackupWindowDrawingCreate1={_S("BackupWindowDrawing 1"),BackupWindowDrawingFunc1};

TInt BackupWindowDrawingFunc2(TInt aOwningGroup)
	{
	return(CreateGraphicsTest(aOwningGroup, EBackupWindowDrawingTest, 1, NULL));
	}

GLDEF_D TTimeTestHeader BackupWindowDrawingCreate2={_S("BackupWindowDrawing 2"),BackupWindowDrawingFunc2};
