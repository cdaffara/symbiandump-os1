// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Very simple test program to connect to the window server
// 
//

#include <e32std.h>
#include "W32STD.H"
#include <e32svr.h>
#include "../SERVER/w32cmd.h"
#include "../test/tlib/testbase.h"
#include "ANIMWIN.H"
#include "HNDLODR.H"

#define TEST_BITMAP_NAME _L("Z:\\WSTEST\\TEST.MBM")
_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");

//
// TestWindow class
//

class TestClient;	// Forward reference

class TestWindow1 : public CTWin
	{
public:
	TestWindow1();
	void Resized(const TSize &aNewSize);
	void Draw();
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
private:
	TBool iDrawPolygon;
	};

class TestWindow2 : public CTWin
	{
	enum EPointerState
		{
		EPointerIn,
		EPointerOut,
		EPointerUndefined,
		};
public:
	TestWindow2();
	~TestWindow2();
	void InitWinL();
	void Draw();
	void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void NewText(TPoint &pos,TInt &width,TDes &des);
	void PointerState(EPointerState aState);
	void PointerEnter(const TTime &aTime);
	void PointerExit(const TTime &aTime);
	void SetBufBits();
	void RequestPointerRepeat();
private:
	void SwitchOn(const TTime &aTime);
private:
	TBuf<0x40> iBuf;
	TBuf<0x40> iBufBits;
	TBuf<0x40> iBufMem;
	TPoint iPos;
	TPoint iPosBits;
	TPoint iPosMem;
	TInt iWid;
	TInt iWidBits;
	TInt iWidMem;
	TInt iFontHeight;
	EPointerState iPointerState;
	TUint32 iFilter;
	TBool iCapture;
	TBool iGrab;
	TRgb iBackColor;
	TInt iRepeatCount;
	TRect iRepeatRect;
	};

class TestWindow3 : public CTWin
	{
public:
	TestWindow3();
	~TestWindow3();
	void Draw();
	void InitWinL();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Resized(const TSize &aNewSize);
	void SetShape(TInt aShapeNum);
	void ScreenDeviceChangedL();
private:
	CWsBitmap *iBitmap;
	TInt iShapeNum;
	TInt iCornerFlags;
	TCornerType iCornerType;
	};

class TestWindow4 : public CTWin
	{
public:
	TestWindow4();
	void InitWin();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Resized(const TSize &aSize);
	void Draw();
	void Draw(TBool aSetCursor);
private:
	void SetSwappedSize();
	void SetCursor(const TPoint &aPos);
private:
	TBuf<300> TxtBuf;
	TPoint iBasePos;
	TBool iVertical;
	TSize iSwappedSize;
	//TInt iTemp;
	};

class TestWindow5 : public CTBackedUpWin
	{
public:
	TestWindow5(TBool aGrey2);
	void Draw();
	void InitWinL();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Resized(const TSize &aNewSize);
private:
	CFbsBitmap iBrushBit;
	TInt iOrdPri;
	};

class TestWindow6 : public CTBackedUpWin
	{
public:
	enum {KPointerMoveBufferSize=32};
public:
	TestWindow6(TBool aGrey2);
	~TestWindow6();
	void Draw();
	void InitWinL();
	void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	virtual void AssignGC(CWindowGc &aGc);
	void Resized(const TSize &aNewSize);
	void PointerBufferReady(const TTime &aTime);
private:
	TSize iOldSize;	// Needed as part of resizing after old size has been lost
	TBool iDragging;
	TInt iGray;
	CWindowGc *iGc2;
	CWindowGc *iCurGc;
	};

class TestWindow7 : public CTBackedUpWin
	{
private:
	enum {EPointerModes=3};
public:
	TestWindow7(TBool aGrey2);
	~TestWindow7();
	void InitWinL();
	void Draw();
	void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Resized(const TSize &aNewSize);
/*private:
	void createBitmapL(CFbsBitmap *aBitmap, CFbsBitmapDevice *&aBitmapDevice, const TSize &aSize, TBool aDoMask);
	void createBitmap2L(CFbsBitmap *aBitmap, CFbsBitmapDevice *&aBitmapDevice, const TSize &aSize, TBool aDoMask);
	void InitPointerCursorL(RWsPointerCursor &aSprite, const TSize &aSize, TInt aFlags);
	void InitSpriteL(RWsSprite &aSprite, const TPoint &aPos, const TSize &aSize, TTimeIntervalMicroSeconds32 aOnTime, TTimeIntervalMicroSeconds32 aOffTime, TInt aFlags);*/
private:
	CTSprite *iSprite1;
	CTSprite *iSprite2;
	CTSprite *iSprite3;
	CTSprite *iSprite4;
	CTSprite *iSprite5;
	CTPointerCursor *iPointerCursor;
	CFbsBitmap *iSpriteBitmap;
	CFbsBitmap *iSpriteMaskBitmap;
	TInt iPointerMode;
	TPoint iPos1;
	TPoint iPos2;
	TPoint iPos3;
	TPoint iPos4;
	TPoint iPos5;
	TPoint *iCurPos;
	CTSprite *iCurSprite;
	};

class TestWindow8 : public CTBlankWindow
	{
public:
	TestWindow8();
	void Draw();
	void InitWin();
	};

class TestWindow9 : public CTWin
	{
public:
	TestWindow9();
	void Draw();
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
	void InitWinL();
	void Play();
private:
	CFbsBitmap iBitmap;
	TSize iVideoSize;
	};

class TestWindow10 : public CTWin
	{
public:
	TestWindow10();
	void Draw();
	void InitWin();
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
	};

class TestWindow12 : public CTWin
	{
public:
	TestWindow12();
	void Draw();
	void InitWin();
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
	};

class TestWindowGroup : public CTWindowGroup
	{
public:
	TestWindowGroup(CTClient *aClient);
	virtual void ConstructL();
	virtual void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void ScreenDeviceChanged();
	void RunLaunchAndShootTestL();
	void MillionsOfSessions();
	inline void SetHandWriting(CHandWriting* aHand) {iHand=aHand;}
private:
	CHandWriting* iHand;
	TBool iIncludeChildren;
	};

class TestClient : public CTClient
	{
public:
	TestClient();
	~TestClient();
	void ConstructL();
	TInt EventLoop();
	void Exit();
	void SetCurrentWindow(CTWinBase *aWindow);
	CTBaseWin *CreateTestWindowL(TInt type,TPoint pos,TSize size,CTWinBase *parent,CWindowGc &gc,TBool aAlternate=EFalse);
	void AdjustLightSource(TInt aX,TInt aY);
private:
	inline TestWindowGroup* TestWindow() {return STATIC_CAST(TestWindowGroup*,iGroup);}
private:
	CHandWriting *iHandWriting;
	TPoint ShadowOffset;
	};

//const TInt Xmove=8;
//const TInt Ymove=6;
const TInt Xmove=1;
const TInt Ymove=1;

LOCAL_D const TUint KTestThreadHeapSize=0x10000;

enum TConnectPanic
	{
	EConnectPanicRedrawToBackedUpWindow,
	EConnectPanicRequiredDisplayMode,
	EConnectPanicDragEvent,
	};

TInt RunLaunchAndShootTest(TAny *);

void Panic(TInt aPanic)
	{
	User::Panic(_L("CONNECT"),aPanic);
	}

//
// Individual window sub-classes
//

TestWindow1::TestWindow1() : CTWin()
	{
	__DECLARE_NAME(_S("TestWindow1"));
	}

void TestWindow1::Draw()
//This function is virtual and so cannot have an 'L' at the end of it's name
	{
	TInt i;

	iGc->SetBrushColor(TRgb::Gray16(14));
	iGc->Clear();
	iGc->SetPenColor(TRgb::Gray16(8));
	iGc->SetBrushColor(TRgb::Gray16(0));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	CArrayFixFlat<TPoint> *pnts;
	pnts=new(ELeave) CArrayFixFlat<TPoint>(20);
	TPoint mid(iSize.iWidth>>1,iSize.iHeight>>1);
	for(i=0;i<mid.iX;i+=4)
		{
		pnts->AppendL(TPoint(mid.iX-i,mid.iY-i));
		pnts->AppendL(TPoint(mid.iX+i+1,mid.iY-i-1));
		pnts->AppendL(TPoint(mid.iX+i+2,mid.iY+i+2));
		pnts->AppendL(TPoint(mid.iX-i-3,mid.iY+i+3));
		}
	if (iDrawPolygon)
		iGc->DrawPolygon(pnts,CGraphicsContext::EAlternate);
	else
		iGc->DrawPolyLine(pnts);
	delete pnts;
//
	TPoint polygon[8];
	polygon[0]=TPoint(mid.iX-8,mid.iY-16);
	polygon[1]=TPoint(mid.iX+8,mid.iY-16);
	polygon[2]=TPoint(mid.iX+16,mid.iY-8);
	polygon[3]=TPoint(mid.iX+16,mid.iY+8);
	polygon[4]=TPoint(mid.iX+8,mid.iY+16);
	polygon[5]=TPoint(mid.iX-8,mid.iY+16);
	polygon[6]=TPoint(mid.iX-16,mid.iY+8);
	polygon[7]=TPoint(mid.iX-16,mid.iY-8);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->DrawPolygon(&polygon[0],sizeof(polygon)/sizeof(polygon[0]));
	}

void TestWindow1::Resized(const TSize &aNewSize)
	{
	iWin.Invalidate();
	CTBaseWin::Resized(aNewSize);
	}

/*void AFunction(TDesC& des)
	{
	TDesC *abc=&des;
	TInt a=3+4;
	}

void AFunction2(TDes& des)
	{
	TDes *abc=&des;
	TInt a=3+4;
	}*/

void TestWindow1::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode=='p')
		{
		RWindow win1(Client()->iWs);
		win1.Construct(iWin,1);
		RWindow win2(Client()->iWs);
		win2.Construct(win1,1);
		win1.Close();
		win2.SetExtent(TPoint(1,2),TSize(3,4));
		Client()->iWs.Flush();
		}
	else if (aKey.iCode=='q' || aKey.iCode=='Q')
		{
		RWindowGroup group1(Client()->iWs);
		RWindowGroup group2(Client()->iWs);
		group1.Construct(1);
		group2.Construct(2);
		group1.DefaultOwningWindow();
		group2.DefaultOwningWindow();
		group1.DefaultOwningWindow();
		group1.Close();
		group2.Close();
		Client()->iWs.Flush();
		}
	else if (aKey.iCode=='w' || aKey.iCode=='W')
		{
		RWindowGroup group1(Client()->iWs);
		RWindowGroup group2(Client()->iWs);
		group1.Construct(1);
		group2.Construct(2);
		group1.DefaultOwningWindow();
		group2.DefaultOwningWindow();
		group1.DefaultOwningWindow();
		group2.Close();
		group1.Close();
		Client()->iWs.Flush();
		}
	else if (aKey.iCode=='A' || aKey.iCode=='a')
		{
		/*RWindowGroup group(Client()->iWs);
		group.Construct(1);*/
		RDrawableWindow* win=new(ELeave) RBackedUpWindow(Client()->iWs);
		User::LeaveIfError(((RBackedUpWindow*)win)->Construct(*Client()->iGroup->GroupWin(),EGray2,(TUint32)this));   
		User::LeaveIfError(win->SetExtentErr(TPoint(20,20),TSize(600,200)));
		win->Activate();
		}
	else if (aKey.iCode=='B' || aKey.iCode=='b')
		{
		RWindowGroup group(Client()->iWs);
		group.Construct(344);
		//group.EnableReceiptOfFocus(EFalse);
		RBlankWindow blank(Client()->iWs);
		blank.Construct(group,345);
		blank.SetOrdinalPosition(0,1000);
		//blank.SetShadowHeight(0);
		//blank.SetShadowDisabled(ETrue);
		blank.Activate();
		RWindow window(Client()->iWs);
		window.Construct(group,346);
		}
	/*else if (aKey.iCode=='Z' || aKey.iCode=='z')		//TDes8 TPtr8 TPtrC8 TBufCBase8 TBuf8
		{
		TBuf<4> buf2(_L("1234"));
		TBuf<4> buf(_L("abcd"));
		TBufC<4> bufC(_L("WXYZ"));
		HBufC *hbuf=HBufC::NewL(4);
		TPtr pHBuf=hbuf->Des();
		pHBuf=buf2;
		TPtr ptr(hbuf->Des());
		//AFunction(ptr);
		TPtrC ptrC1(buf);
		//AFunction(ptrC1);
		TPtrC ptrC2(bufC);
		//AFunction(ptrC2);
		TPtrC ptrC3(hbuf->Des());
		AFunction(ptrC3);
		AFunction(buf);
		AFunction(bufC);
		AFunction(*hbuf);
		AFunction2(ptr);
		AFunction2(buf);
		}*/
	if (aKey.iCode==' ')
		{
		iDrawPolygon=!iDrawPolygon;
		iWin.Invalidate();
		}
	}

TestWindow2::TestWindow2() : CTWin(),
		iWid(0), iFontHeight(16), iPointerState(EPointerOut), iFilter(0), iCapture(EFalse), iGrab(EFalse)
	{
	__DECLARE_NAME(_S("TestWindow2"));
	}

TestWindow2::~TestWindow2()
	{
	}

void TestWindow2::InitWinL()
	{
	iPos.SetXY(40,iFont->AscentInPixels()+10);
	TPoint next(0,iFont->HeightInPixels()+10);
	iPosBits=iPos+next;
	iPosMem=iPosBits+next;
	iWin.PointerFilter(0xFFFFFFFF,iFilter);
	User::LeaveIfError(iWin.EnableOnEvents());
	iWin.ClaimPointerGrab();
	}

void TestWindow2::Draw()
	{
	TInt pen=0;
	switch(iPointerState)
		{
		case EPointerIn:
			pen=0;
			break;
		case EPointerOut:
			pen=5;
			break;
		case EPointerUndefined:
			pen=15;
			break;
		}
	iGc->SetPenColor(TRgb::Gray16(pen));
	iGc->SetBrushColor(iBackColor);
	DrawBorder();
	iGc->DrawText(iBuf,iPos);
	iGc->DrawText(iBufBits,iPosBits);
	iGc->DrawText(iBufMem,iPosMem);
	}

void TestWindow2::NewText(TPoint &pos,TInt &width,TDes &des)
	{
	TInt newWid=iFont->TextWidthInPixels(des);
	Invalidate(TRect(pos-TSize(0,iFont->AscentInPixels()),pos+TSize(Max(width,newWid),iFont->DescentInPixels())));
	width=newWid;
	}

void TestWindow2::PointerState(EPointerState aState)
	{
	switch(aState)
		{
		case EPointerIn:
			iBackColor=TRgb::Gray16(15);
			break;
		case EPointerOut:
			iBackColor=TRgb::Gray16(12);
			break;
		case EPointerUndefined:
			iBackColor=TRgb::Gray16(4);
			break;
		}
	iWin.SetBackgroundColor(iBackColor);
	Invalidate();
	iPointerState=aState;
	}

void TestWindow2::SwitchOn(const TTime &)
	{
	iWin.SetBackgroundColor(TRgb(0,0,0));
	Client()->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(500000));
	PointerState(iPointerState);
	}

void TestWindow2::PointerEnter(const TTime &)
	{
	PointerState(EPointerIn);
	}

void TestWindow2::PointerExit(const TTime &)
	{
	PointerState(EPointerOut);
	}

void TestWindow2::RequestPointerRepeat()
	{
	iWin.RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(200000),iRepeatRect);
	}

void TestWindow2::PointerL(const TPointerEvent &pointer,const TTime &aTime)
	{
	if (pointer.iType==TPointerEvent::EButtonRepeat)
		{
		iRepeatCount++;
		SetBufBits();
		RequestPointerRepeat();
		}
	else
		{
		iRepeatCount=0;
		CTWin::PointerL(pointer,aTime);
		iBuf.Format(TRefByValue<const TDesC>(_L("Pos(%d,%d)(%d,%d), 0x%x")),pointer.iPosition.iX,pointer.iPosition.iY,pointer.iParentPosition.iX,pointer.iParentPosition.iY,pointer.iModifiers);
		NewText(iPos,iWid,iBuf);
		if (pointer.iType==TPointerEvent::EButton1Down)
			{
			iRepeatRect=TRect(pointer.iPosition-TPoint(5,5),pointer.iPosition+TPoint(5,5));
			RequestPointerRepeat();
			}
		SetBufBits();
		}
	}

void TestWindow2::SetBufBits()
	{
	iBufBits.Format(TRefByValue<const TDesC>(_L("Filter=0x%x, Capture[%s], Grab[%s], Repeat[%d]")),iFilter,
									 iCapture ? _S("On") : _S("Off"),
									 iGrab ? _S("On") : _S("Off"),
									 iRepeatCount);
	NewText(iPosBits,iWidBits,iBufBits);
	Invalidate();
	}

void TestWindow2::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case ',':	// Unshifted <
		case '.':	// Unshifted >
			{
			if (aKey.iCode==',')
				{
				if (iFontHeight>1)
					iFontHeight-=1;
				}
			else
				iFontHeight+=1;
			CFbsFont *newFont;
			TFontSpec fspec(KTestFontTypefaceName,iFontHeight*KTwipsPerPoint);
			User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)newFont, fspec));

			Client()->iScreen->ReleaseFont(iFont);
			iFont=newFont;
			iWid=iFont->TextWidthInPixels(iBuf);
			Invalidate();
			}
			break;
		case '?':
			iBufMem.Format(TRefByValue<const TDesC>(_L("Count=%d")),Client()->iWs.HeapCount());
			NewText(iPosMem,iWidMem,iBufMem);
			break;
		case 'c':
			iCapture=(TBool)!iCapture;
			iWin.SetPointerCapture(iCapture);
			SetBufBits();
			break;
		case 'g':
			iGrab=(TBool)!iGrab;
			iWin.SetPointerGrab(iGrab);
			SetBufBits();
			break;
		case ' ':
			{
			iFilter=(iFilter+1)%8;
			iWin.PointerFilter(0xFFFFFFFF,iFilter);
			SetBufBits();
			if (iFilter&EPointerFilterEnterExit)
				PointerState(EPointerUndefined);
			else
				PointerState(EPointerOut);
			}
		case 's':
		case 'S':
			{
			CWsBitmap* bitmap=new(ELeave) CWsBitmap(Client()->iWs);
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(Client()->iScreen->SizeInPixels(),EGray4));
			User::LeaveIfError(Client()->iScreen->CopyScreenToBitmap(bitmap));
			CleanupStack::Pop();
			}
			break;
		default:
			iBufMem.Format(TRefByValue<const TDesC>(_L("Key=%d")),aKey.iCode);
			NewText(iPosMem,iWidMem,iBufMem);
			break;
		}
	}

TestWindow3::TestWindow3() : CTWin()
	{
	__DECLARE_NAME(_S("TestWindow3"));
	}

TestWindow3::~TestWindow3()
	{
	delete iBitmap;
	}

void TestWindow3::Resized(const TSize &aNewSize)
	{
	CTBaseWin::Resized(aNewSize);
	SetShape(iShapeNum);
	DrawNow();
	}

void TestWindow3::InitWinL()
	{
	iCornerType=EWindowCorner3;
	iWin.PointerFilter(EPointerFilterDrag,0);	// Clear drag filter
	iWin.SetPointerGrab(ETrue);
	iWin.SetBackgroundColor(TRgb::Gray256(230));
	iWin.SetCornerType(iCornerType);
	iBitmap=new(ELeave) CWsBitmap(Client()->iWs);
	User::LeaveIfError(iBitmap->Load(TEST_BITMAP_NAME,0));
	iSize=iBitmap->SizeInPixels();
	}

void TestWindow3::Draw()
	{
	iGc->BitBlt(TPoint(0,0),iBitmap);
	TSize bitSize(iBitmap->SizeInPixels());
	TSize winSize(Size());
	iGc->Clear(TRect(bitSize.iWidth,0,winSize.iWidth,bitSize.iHeight));
	iGc->Clear(TRect(0,bitSize.iHeight,winSize.iWidth,winSize.iHeight));
	}

void TestWindow3::SetShape(TInt aShapeNum)
	{
	iShapeNum=aShapeNum;
	TSize size=Size();
	switch(iShapeNum)
		{
		case 0:
			{
			TRegionFix<4> shape;
			shape.AddRect(TRect(0,0,size.iWidth,size.iHeight>>2));
			shape.AddRect(TRect(0,size.iHeight>>2,size.iWidth>>2,size.iHeight-(size.iHeight>>2)));
			shape.AddRect(TRect(size.iWidth-(size.iWidth>>2),size.iHeight>>2,size.iWidth,size.iHeight-(size.iHeight>>2)));
			shape.AddRect(TRect(0,size.iHeight-(size.iHeight>>2),size.iWidth,size.iHeight));
			iWin.SetShape(shape);
			}
			break;
		case 1:
			{
			TRegionFix<3> shape;
			shape.AddRect(TRect(size.iWidth>>2,0,size.iWidth-(size.iWidth>>2),size.iHeight>>2));
			shape.AddRect(TRect(0,size.iHeight>>2,size.iWidth,size.iHeight-(size.iHeight>>2)));
			shape.AddRect(TRect(size.iWidth>>2,size.iHeight-(size.iHeight>>2),size.iWidth-(size.iWidth>>2),size.iHeight));
			iWin.SetShape(shape);
			}
			break;
		case 2:
			{
			RRegion shape;
			for(TInt index=0;index<(size.iHeight-1);index++)
				{
				TInt xFactor=(size.iWidth>>1)*index/size.iHeight;
				shape.AddRect(TRect((size.iWidth>>1)-xFactor,index,(size.iWidth>>1)+xFactor,index+1));
				}
			iWin.SetShape(shape);
			shape.Close();
			}
			break;
		}
	}

void TestWindow3::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode=='f')
		{
		TSize scrSize(Client()->iScreen->SizeInPixels());
		if (scrSize==iWin.Size())
			{
			scrSize.iWidth/=2;
			scrSize.iHeight/=2;
			}
		SetExtL(TPoint(),scrSize);
		}
	else if (aKey.iCode>='0' && aKey.iCode<='5')
		{
		switch(aKey.iCode)
			{
			case '0':
				iCornerType=EWindowCornerSquare;
				break;
			case '1':
				iCornerType=EWindowCorner1;
				break;
			case '2':
				iCornerType=EWindowCorner2;
				break;
			case '3':
				iCornerType=EWindowCorner3;
				break;
			case '5':
				iCornerType=EWindowCorner5;
				break;
			}
		iWin.SetCornerType(iCornerType,iCornerFlags);
		}
	else switch(User::LowerCase(aKey.iCode))
		{
		case '6':
			SetShape(0);
			break;
		case '7':
			SetShape(1);
			break;
		case '8':
			SetShape(2);
			break;
		case 'a':
			iCornerFlags^=EWindowCornerNotTL;
			goto setcorner;
		case 'b':
			iCornerFlags^=EWindowCornerNotTR;
			goto setcorner;
		case 'c':
			iCornerFlags^=EWindowCornerNotBL;
			goto setcorner;
		case 'd':
			iCornerFlags^=EWindowCornerNotBR;
setcorner:	iWin.SetCornerType(iCornerType,iCornerFlags);
			break;
		}
	}

void TestWindow3::ScreenDeviceChangedL()
	{
	TSize scrSize(Client()->iScreen->SizeInPixels());
	if (scrSize==iWin.Size())
		{
		scrSize.iWidth/=2;
		scrSize.iHeight/=2;
		}
	SetExtL(TPoint(),scrSize);
	}

TestWindow4::TestWindow4() : CTWin()
	{
	__DECLARE_NAME(_S("TestWindow4"));
	}

void TestWindow4::SetSwappedSize()
	{
	if (iVertical)
		{
		iSwappedSize.iHeight=iSize.iWidth;
		iSwappedSize.iWidth=iSize.iHeight;
		}
	else
		iSwappedSize=iSize;
	}

void TestWindow4::InitWin()
	{
	iBasePos.SetXY(10,20);
	iWin.SetBackgroundColor();
	SetSwappedSize();
	//iTemp=1;
	}

void TestWindow4::Resized(const TSize &aSize)
	{
	CTBaseWin::Resized(aSize);
	SetSwappedSize();
	Invalidate();
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw(ETrue);
	iGc->Deactivate();
	iWin.EndRedraw();
	}

void TestWindow4::Draw()
	{
	Draw(EFalse);
	}

void TestWindow4::Draw(TBool aSetCursor)
	{
	TPoint tl(iBasePos.iX,iBasePos.iY-iFont->AscentInPixels());
	TRect rect(1,1,iSize.iWidth-1,iSize.iHeight);
//
	/*switch (iTemp)
		{
	case 1:
		iGc->SetBrushColor(TRgb::Gray4(2));
		break;
	case 2:*/
		iGc->SetBrushColor(TRgb::Gray16(13));
		/*break;
	case 3:
		iGc->SetBrushColor(TRgb::Gray16(12));
		break;
	case 4:
		iGc->SetBrushColor(TRgb::Gray16(11));
	default:
		break;
		}*/
	iGc->Clear(rect);
//
	TPoint pos(iBasePos);
	iGc->SetPenColor(TRgb::Gray16(0));
	iGc->DrawRect(TRect(iSize));
	TInt index=0;
	TInt ToGo=TxtBuf.Length();
	TInt lineHeight=iFont->HeightInPixels()+2;
	rect.iTl.iY=rect.iBr.iY=tl.iY;
	while((pos.iY+iFont->DescentInPixels())<(iSwappedSize.iHeight-1))
		{
		rect.iBr.iY+=lineHeight;
		iGc->Clear(rect);
		if (ToGo>0)
			{
			TInt len=iFont->TextCount(TxtBuf.Right(ToGo),iSwappedSize.iWidth-2*pos.iX);
			if (len==0)
				len=1;
			if (iVertical)
				iGc->DrawTextVertical(TxtBuf.Mid(index,len),TPoint(pos.iY,iSize.iHeight-pos.iX),ETrue);
			else
				iGc->DrawText(TxtBuf.Mid(index,len),pos);
			ToGo-=len;
			if (ToGo==0 && aSetCursor)
				{
				TPoint cursorPos;
				if (iVertical)
					cursorPos=TPoint(pos.iY-iFont->AscentInPixels(),iSize.iHeight-pos.iX)+TPoint(0,iFont->TextWidthInPixels(TxtBuf.Mid(index,len)));
				else
					cursorPos=pos+TPoint(iFont->TextWidthInPixels(TxtBuf.Mid(index,len)),0);
				SetCursor(cursorPos);
				}
			index+=len;
			}
		pos.iY+=lineHeight;
		rect.iTl.iY=rect.iBr.iY;
		}
	rect.iBr.iY=iSwappedSize.iHeight-1;
	iGc->Clear(rect);
	}

void TestWindow4::SetCursor(const TPoint &aPos)
	{
	TTextCursor cursor;
	cursor.iType=TTextCursor::ETypeRectangle;
	if (iVertical)
		{
		cursor.iAscent=2;
		cursor.iHeight=2;
		cursor.iWidth=iFont->HeightInPixels();
		}
	else
		{
		cursor.iAscent=iFont->AscentInPixels();
		cursor.iHeight=iFont->HeightInPixels();
		cursor.iWidth=20;
		}
	cursor.iFlags=0;
//	cursor.iColor=TRgb(255,255,255);
	cursor.iColor=TRgb(128,128,128);
	Client()->iGroup->GroupWin()->SetTextCursor(iWin, aPos, cursor);
	}

void TestWindow4::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	TChar chr=aKey.iCode;

	if (aKey.iCode==32 && aKey.iModifiers&EModifierFunc)
		{
		iWin.Invalidate();
		iVertical=!iVertical;
		SetSwappedSize();
		}
	else if (aKey.iCode>=32 && aKey.iCode<4000)
		{
		TBool invalidate=EFalse;
		if (TxtBuf.Length()==TxtBuf.MaxLength())
			{
			TxtBuf=TxtBuf.Right(TxtBuf.Length()-1);
			invalidate=ETrue;
			}
		TxtBuf.Append(chr);
		
		TPoint pos(iBasePos);
		TInt index=0;
		TInt ToGo=TxtBuf.Length();
		while(ToGo>0)
			{
			TInt len=iFont->TextCount(TxtBuf.Right(ToGo),iSwappedSize.iWidth-2*pos.iX);
			if (len==0)
				len=1;
			if (len==ToGo)
				break;
			index+=len;
			ToGo-=len;
			pos.iY+=iFont->HeightInPixels()+2;
			}
		pos.iX+=iFont->TextWidthInPixels(TxtBuf.Mid(index,ToGo-1));
		if (invalidate)
			iWin.Invalidate(TRect(iBasePos.iX,iBasePos.iY-iFont->AscentInPixels(),iSize.iWidth-iBasePos.iX,iSize.iHeight));
		else
			{
			iGc->Activate(iWin);
			iGc->UseFont(iFont);
			if (iVertical)
				iGc->DrawTextVertical(TxtBuf.Mid(index+ToGo-1,1),TPoint(pos.iY,iSize.iHeight-pos.iX),ETrue);
			else
				iGc->DrawText(TxtBuf.Mid(index+ToGo-1,1),pos);
			iGc->Deactivate();
			}
		pos.iX+=iFont->TextWidthInPixels(TxtBuf.Mid(index+ToGo-1));
		SetCursor(iVertical ? TPoint(pos.iY-iFont->AscentInPixels(),iSize.iHeight-pos.iX) : pos);
		}
	}

TestWindow5::TestWindow5(TBool aGrey2) : CTBackedUpWin(aGrey2?EGray2:EGray4)
	{
	__DECLARE_NAME(_S("TestWindow5"));
	}

void TestWindow5::Resized(const TSize &aNewSize)
	{
	CTBaseWin::Resized(aNewSize);
	iGc->Activate(*DrawableWin());
	iGc->SetBrushColor(TRgb::Gray256(128));
	iGc->Clear(TRect(aNewSize));
	iGc->UseBrushPattern(&iBrushBit);
	iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iGc->DrawEllipse(TRect(aNewSize));
	iGc->Deactivate();
	}

void TestWindow5::InitWinL()
	{
	iWin.PointerFilter(EPointerFilterMove|EPointerFilterEnterExit,EPointerFilterMove|EPointerFilterEnterExit);
	iWin.SetPointerGrab(ETrue);
	iOrdPri=-1;
	iWin.SetOrdinalPosition(0,iOrdPri);
	User::LeaveIfError(iBrushBit.Load(TEST_BITMAP_NAME,0));
	}

void TestWindow5::Draw()
	{
	Panic(EConnectPanicRedrawToBackedUpWindow);
	}

void TestWindow5::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case ' ':
			iOrdPri=(iOrdPri<0)?1:-1;
			iWin.SetOrdinalPosition(0,iOrdPri);
			break;
		case 'z':
			SetPos(iPos-TPoint(2,1));
			SetPos(iPos-TPoint(-1,-1));
			SetPos(iPos-TPoint(2,3));
			SetPos(iPos-TPoint(-2,-1));
			SetPos(iPos-TPoint(-2,-3));
			SetPos(iPos-TPoint(1,1));
			break;
		}
	}

TestWindow6::TestWindow6(TBool aGrey2) : CTBackedUpWin(aGrey2?EGray2:EGray16), iGray(0)
	{
	__DECLARE_NAME(_S("TestWindow6"));
	}

TestWindow6::~TestWindow6()
	{
	delete iGc;
	delete iGc2;
	}

void TestWindow6::Resized(const TSize &aNewSize)
	{
	CTBaseWin::Resized(aNewSize);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	if (iOldSize.iWidth<aNewSize.iWidth)
		iGc->DrawRect(TRect(iOldSize.iWidth,0,aNewSize.iWidth,iOldSize.iHeight));
	if (iOldSize.iHeight<aNewSize.iHeight)
		iGc->DrawRect(TRect(0,iOldSize.iHeight,aNewSize.iWidth,aNewSize.iHeight));
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iOldSize=aNewSize;
	TPoint pos;
	for(TInt index=0;index<3;index++)
		{
		pos.iY=index*aNewSize.iHeight/3;
		for(pos.iX=0;pos.iX<aNewSize.iWidth;pos.iX++)
			{
			TInt val=pos.iX*255/aNewSize.iWidth;
			TRgb rgb(index==0?val:0,index==1?val:0,index==2?val:0);
			iGc->SetPenColor(rgb);
			iGc->DrawLine(pos,pos+TSize(0,aNewSize.iHeight/3));
			}
		}
	iGc->SetPenColor(TRgb(255,255,255));
	}

void TestWindow6::InitWinL()
	{
	iWin.PointerFilter(EPointerFilterMove|EPointerFilterEnterExit,EPointerFilterMove|EPointerFilterEnterExit);
	User::LeaveIfError(iWin.AllocPointerMoveBuffer(KPointerMoveBufferSize,0));
	iWin.DisablePointerMoveBuffer();
	iWin.SetPointerGrab(ETrue);
	iGc=new(ELeave) CWindowGc(Client()->iScreen);
	User::LeaveIfError(iGc->Construct());
	iGc2=new(ELeave) CWindowGc(Client()->iScreen);
	User::LeaveIfError(iGc2->Construct());
	iWin.SetCornerType(EWindowCorner1);
	iGc->Activate(iWin);
	iGc->SetBrushColor(TRgb::Gray256(230));
	iGc2->Activate(iWin);
	iGc2->SetBrushColor(TRgb::Gray256(230));	
	iCurGc=iGc;
	}

void TestWindow6::Draw()
	{
	Panic(EConnectPanicRedrawToBackedUpWindow);
	}

void TestWindow6::PointerBufferReady(const TTime &)
	{
	TPoint pnts[KPointerMoveBufferSize];
	TPtr8 ptr((TUint8 *)&pnts,sizeof(pnts));
	TInt numPnts=iWin.RetrievePointerMoveBuffer(ptr);
	for(TInt index=0;index<numPnts;index++)
		iCurGc->DrawLineTo(pnts[index]);
	}

void TestWindow6::PointerL(const TPointerEvent &pointer,const TTime &aTime)
	{
	switch(pointer.iType)
		{
		case TPointerEvent::EButton1Down:
			if (pointer.iModifiers&(EModifierCtrl|EModifierShift))
				{
				iDragging=ETrue;
				iWin.EnablePointerMoveBuffer();
				if (pointer.iModifiers&EModifierCtrl)
					iCurGc=iGc;
				else if (pointer.iModifiers&EModifierShift)
					iCurGc=iGc2;
				iCurGc->MoveTo(pointer.iPosition);
				return;
				}
			break;
		case TPointerEvent::EButton1Up:
			if (iDragging)
				{
				iDragging=EFalse;
				iWin.DisablePointerMoveBuffer();
				return;
				}
		default:;
		}
	CTBackedUpWin::PointerL(pointer,aTime);
	}

void TestWindow6::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (TChar(aKey.iCode).IsDigit())
		{
		TInt wid=1;
		for(TUint i=0;i<(aKey.iCode-'0');i++)
			wid+=i;
		iCurGc->SetPenSize(TSize(wid,wid));
		}
	else switch(aKey.iCode)
		{
		case ',':	// Unshifted <
		case '.':	// Unshifted >
			{
			if (aKey.iCode==',')
				{
				if (iGray>0)
					iGray-=1;
				}
			else if (iGray<15)
				iGray+=1;
			iCurGc->SetPenColor(TRgb::Gray16(iGray));
			}
			break;
		}
	}

void TestWindow6::AssignGC(CWindowGc &)
	{
	}

TestWindow7::TestWindow7(TBool aGrey2) : CTBackedUpWin(aGrey2?EGray2:EGray4)
	{
	__DECLARE_NAME(_S("TestWindow1"));
	}

TestWindow7::~TestWindow7()
	{
	delete iSpriteBitmap;
	delete iSpriteMaskBitmap;
	delete iPointerCursor;
	delete iSprite4;
	delete iSprite5;
	delete iSprite2;
	delete iSprite3;
	delete iSprite1;
	}

void DrawPointerCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aMode)
	{
	TInt mode=*(TInt *)aMode;
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	TInt size=mode+1;
	TRect rect1(0,aSize.iHeight/2-size,aSize.iWidth,aSize.iHeight/2+size);
	TRect rect2(aSize.iWidth/2-size,0,aSize.iWidth/2+size,aSize.iHeight);
//
	if (!aDoMask)
		{
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb::Gray4(0));
		aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc->DrawRect(rect1);
		aGc->DrawRect(rect2);
		rect1.Shrink(1,1);
		rect2.Shrink(1,1);
		}
//
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : 1));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->DrawRect(rect1);
	aGc->DrawRect(rect2);
	}

void DrawSprite(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 0));
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 3 : 1));
	aGc->DrawEllipse(TRect(aSize));
	}

void TestWindow7::InitWinL()
	{
	iPos1.SetXY(30,10);
	iPos2.SetXY(50,40);
	iPos3.SetXY(10,50);
	iPos4.SetXY(10,10);
	iPos5.SetXY(50,10);
//
	TSpriteCreateParams params(TSize(30,70),TPoint(0,0),DrawSprite);
	TSpriteCreateParams paramarray[10];
	params.iInterval=TTimeIntervalMicroSeconds32(200000);

	paramarray[0]=params;
	paramarray[1]=params;
	paramarray[2]=params;
	paramarray[1].iSize=TSize(20,30);
	paramarray[2].iSize=TSize(100,10);
	paramarray[2].iInterval=TTimeIntervalMicroSeconds32(200000);
	iSprite1=new(ELeave) CTSprite(Client()->iWs);
	iSprite1->ConstructL(iWin,iPos1,3,&paramarray[0],ESpriteNoChildClip);
//
	params.iSize=TSize(20,30);
	for(TInt index=0;index<10;index++)
		{
		paramarray[index]=params;
		paramarray[index].iOffset=TPoint(index*20,index*4);
		}
	iSprite2=new(ELeave) CTSprite(Client()->iWs);
	iSprite2->ConstructL(iWin,iPos2,10,&paramarray[0],0);
//
	params.iSize=TSize(100,10);
	iSprite3=new(ELeave) CTSprite(Client()->iWs);
	iSprite3->ConstructL(iWin,iPos3,1,&params,ESpriteFlash);
//
	params.iSize=TSize(32,32);
	iSprite4=new(ELeave) CTSprite(Client()->iWs);
	iSprite4->ConstructL(*Client()->iGroup->GroupWin(),iPos4,1,&params,0);
//
	params.iSize=TSize(16,64);
	iSprite5=new(ELeave) CTSprite(Client()->iWs);
	iSprite5->ConstructL(*Client()->iGroup->GroupWin(),iPos5,1,&params,ESpriteNoChildClip);
//
	iCurPos= &iPos1;
	iCurSprite= iSprite1;
//	iWin.SetBackgroundColor(TRgb::Gray256(170));
	TSpriteCreateParams pcursor(TSize(24,48),TPoint(-12,-24),DrawPointerCursor,&iPointerMode);
	iPointerCursor=new(ELeave) CTPointerCursor(Client()->iWs);
	iPointerCursor->ConstructL(1,&pcursor,0,iSpriteBitmap,iSpriteMaskBitmap);
	iWin.SetCustomPointerCursor(iPointerCursor->PointerCursor());
	Client()->iGroup->GroupWin()->SetCustomPointerCursor(iPointerCursor->PointerCursor());
	}

	
void TestWindow7::Resized(const TSize &aNewSize)
	{
	CTBaseWin::Resized(aNewSize);
	iGc->Activate(*DrawableWin());
	Draw();
	iGc->Deactivate();
	}

void TestWindow7::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray256(170));
	iGc->Clear();
	for(TInt xPos=0;xPos<iSize.iWidth;xPos+=50)
		iGc->DrawLine(TPoint(xPos,0),TPoint(xPos,iSize.iHeight));
	for(TInt yPos=0;yPos<iSize.iHeight;yPos+=50)
		iGc->DrawLine(TPoint(0,yPos),TPoint(iSize.iWidth,yPos));
	}

void TestWindow7::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case 'c':
		case 'C':
			{
			iPointerMode=(iPointerMode+1)&EPointerModes;
			TSpriteCreateParams params(iSpriteBitmap->SizeInPixels(),TPoint(0,0),DrawPointerCursor,&iPointerMode);
			iPointerCursor->RedrawL(iSpriteBitmap,iSpriteMaskBitmap,&params);
			break;
			}
		case ' ':
			if (iCurSprite==iSprite1)
				{
				iCurPos= &iPos2;
				iCurSprite= iSprite2;
				}
			else if (iCurSprite==iSprite2)
				{
				iCurPos= &iPos3;
				iCurSprite= iSprite3;
				}
			else if (iCurSprite==iSprite3)
				{
				iCurPos= &iPos4;
				iCurSprite= iSprite4;
				}
			else if (iCurSprite==iSprite4)
				{
				iCurPos= &iPos5;
				iCurSprite= iSprite5;
				}
			else if (iCurSprite==iSprite5)
				{
				iCurPos= &iPos1;
				iCurSprite= iSprite1;
				}
			break;
		case '1':
			iCurPos->iX-=5;
			iCurSprite->Sprite().SetPosition(*iCurPos);
			break;
		case '2':
			iCurPos->iX+=5;
			iCurSprite->Sprite().SetPosition(*iCurPos);
			break;
		case '3':
			iCurPos->iY-=5;
			iCurSprite->Sprite().SetPosition(*iCurPos);
			break;
		case '4':
			iCurPos->iY+=5;
			iCurSprite->Sprite().SetPosition(*iCurPos);
			break;
		default:;
		}
	}

TestWindow8::TestWindow8() : CTBlankWindow()
	{
	__DECLARE_NAME(_S("TestWindow8"));
	}

void TestWindow8::InitWin()
	{
	iWin.PointerFilter(EPointerFilterMove|EPointerFilterEnterExit,EPointerFilterMove|EPointerFilterEnterExit);
	iWin.SetPointerGrab(ETrue);
	iWin.SetColor(TRgb::Gray256(170));
	}

void TestWindow8::Draw()
	{
	Panic(EConnectPanicRedrawToBackedUpWindow);
	}

TestWindow9::TestWindow9() : CTWin()
	{
	__DECLARE_NAME(_S("TestWindow9"));
	}

void TestWindow9::InitWinL()
	{
	iWin.SetRequiredDisplayMode(EGray16);
	User::LeaveIfError(iBitmap.Load(VIDEO_MBM_NAME,0));
	iVideoSize=iBitmap.SizeInPixels();
	}

void TestWindow9::Draw()
	{
	iGc->BitBlt(TPoint(),&iBitmap);
	}

void TestWindow9::Play()
	{
	iWin.SetExtent(TPoint(320-(iVideoSize.iWidth>>1),0),iVideoSize);
	iWin.BeginRedraw();
	iWin.EndRedraw();
	iGc->Activate(iWin);
	TInt index=1;
	while(iBitmap.Load(VIDEO_MBM_NAME,index++)==KErrNone)
		{
		iGc->BitBlt(TPoint(),&iBitmap);
		Client()->iWs.Flush();
		}
	iBitmap.Load(VIDEO_MBM_NAME,0);
	iGc->BitBlt(TPoint(),&iBitmap);
	iGc->Deactivate();
	}

void TestWindow9::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode=='p' || aKey.iCode=='P')
		Play();
	}

TestWindow10::TestWindow10() : CTWin()
	{
	__DECLARE_NAME(_S("TestWindow10"));
	}

void TestWindow10::InitWin()
	{
	//iWin.PointerFilter(EPointerFilterMove|EPointerFilterEnterExit,EPointerFilterMove|EPointerFilterEnterExit);
	//iWin.SetPointerGrab(ETrue);
	iWin.SetRequiredDisplayMode(EColor256);
	}

void TestWindow10::Draw()
	{
	TSize size=Size();
	TRect rect(size);
	TInt ii;

	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	rect.iBr.iX=0;
	for(ii=0;ii<256;ii++)
		{
		rect.iTl.iX=rect.iBr.iX;
		rect.iBr.iX=((ii+1)*size.iWidth)/256;
		iGc->SetBrushColor(TRgb::Color256(ii));
		iGc->DrawRect(rect);
		}
	}

void TestWindow10::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	switch(aKey.iCode)
		{
		case '0':
			iWin.SetRequiredDisplayMode(EGray2);
			break;
		case '1':
			iWin.SetRequiredDisplayMode(EGray4);
			break;
		case '2':
			iWin.SetRequiredDisplayMode(EGray16);
			break;
		case '3':
			iWin.SetRequiredDisplayMode(EGray256);
			break;
		case '4':
			iWin.SetRequiredDisplayMode(EColor16);
			break;
		case '5':
			iWin.SetRequiredDisplayMode(EColor256);
			break;
		case '6':
			iWin.SetRequiredDisplayMode(EColor64K);
			break;
		}
	}

TestWindow12::TestWindow12() : CTWin()
	{}

void TestWindow12::InitWin()
	{
	iWin.SetRequiredDisplayMode(EColor16);
	}

void TestWindow12::Draw()
	{
	TRect rect(Size());

	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb(0,255,0));
	iGc->DrawRect(rect);
	rect.iTl.iX=rect.iBr.iX/2;
	iGc->SetBrushColor(TRgb(0,0,255));
	iGc->DrawRect(rect);
	}

void TestWindow12::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	TSize screen(Client()->iScreen->SizeInPixels());
	TSize size(Size());
	TPoint pos(Position());
	TPoint dest1(screen.iWidth-size.iWidth,screen.iHeight-size.iHeight);
	TPoint dest;
	iGc->Activate(iWin);
	switch(aKey.iCode)
		{
	//case '1':
		//break;
	case '2':
		dest.iX=dest1.iX/2;
		break;
	case '3':
		dest.iX=dest1.iX;
		break;
	case '4':
		dest.iY=dest1.iY/2;
		break;
	case '5':
		dest.iX=dest1.iX/2;
		dest.iY=dest1.iY/2;
		break;
	case '6':
		dest.iX=dest1.iX;
		dest.iY=dest1.iY/2;
		break;
	case '7':
		dest.iY=dest1.iY;
		break;
	case '8':
		dest.iX=dest1.iX/2;
		dest.iY=dest1.iY;
		break;
	case '9':
		dest.iX=dest1.iX;
		dest.iY=dest1.iY;
		break;
		}
	TRect rect(screen);
	rect.Move(-pos);
	iGc->CopyRect(pos-dest,rect);
	iGc->Deactivate();
	}

//

TestWindowGroup::TestWindowGroup(CTClient *aClient) : CTWindowGroup(aClient), iIncludeChildren(EFalse)
	{
	}

void TestWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableScreenChangeEvents();
	}

void TestWindowGroup::ScreenDeviceChanged()
	{
	TPixelsTwipsAndRotation sizeAndRotation;
	Client()->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	Client()->iScreen->SetScreenSizeAndRotation(sizeAndRotation);
	}

void TestWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iModifiers&EModifierCtrl && !(aKey.iModifiers&EModifierFunc))
		{
		TInt type=0;
		if (aKey.iCode<=14)
			type=aKey.iCode;
		if (type!=0)
			{
			TPoint pos;
			TSize size;
			CTWinBase *win=this;
			if ((aKey.iModifiers&EModifierLeftShift) && iCurWin)
				win=iCurWin;
			pos.iX=win->Size().iWidth>>2;
			pos.iY=win->Size().iHeight>>2;
			size.iWidth=win->Size().iWidth>>1;
			size.iHeight=win->Size().iHeight>>1;
			iCurWin=((TestClient *)Client())->CreateTestWindowL(type,pos,size,win,*Client()->iGc,aKey.iModifiers&EModifierRightShift);
			return;
			}
		}
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case 'a':
				{
				for(TInt loop=0;loop<100;loop++)
					{
					((TestClient *)Client())->iWs.HeapSetFail(RHeap::EDeterministic,loop);
					CTBaseWin *win=NULL;
					TRAP_IGNORE(win=((TestClient *)Client())->CreateTestWindowL(1,TPoint(15,20),TSize(150,110),Client()->iGroup,*Client()->iGc));
					if (win)
						{
						((CTWin *)win)->Redraw(TRect());
						delete win;
						}
					}
				((TestClient *)Client())->iWs.HeapSetFail(RHeap::ENone,0);
				}
				break;
			case '0':
				{
				TUint *aNullPtr=NULL;
				*aNullPtr=0;
				}
				break;
			case 'p':
				{
				RThread thread;
				thread.Panic(_L("Connect"),123);
				}
				break;
			case 'x':
				((TestClient *)Client())->Exit();
				break;
			case 'f':
				if (Client()->iWs.SetAutoFlush(ETrue))
					Client()->iWs.SetAutoFlush(EFalse);
				break;
			case 'b':
				RunLaunchAndShootTestL();
				break;
			case 'm':
				MillionsOfSessions();
				break;
			case 'o':
				GroupWin()->SetOrdinalPosition(KOrdinalPositionSwitchToOwningWindow);
				break;
			case 10:
				if (aKey.iModifiers&EModifierCtrl && aKey.iModifiers&EModifierShift)
					iHand->ToggleStatus();
				break;
			case EKeyLeftArrow:
				((TestClient *)Client())->AdjustLightSource(-1,0);
				break;
			case EKeyRightArrow:
				((TestClient *)Client())->AdjustLightSource(1,0);
				break;
			case EKeyUpArrow:
				((TestClient *)Client())->AdjustLightSource(0,-1);
				break;
			case EKeyDownArrow:
				((TestClient *)Client())->AdjustLightSource(0,1);
				break;
			default:
				goto keeptrying;
			}
		return;
		}
keeptrying:
	if (iCurWin)
		{
		if (aKey.iModifiers&EModifierFunc)
			{
			//TBool shift=aKey.iModifiers&EModifierShift;
			switch(aKey.iCode)
				{
				case 'c':
					GroupWin()->DisableKeyClick(ETrue);
					break;
				case 'C':
					GroupWin()->DisableKeyClick(EFalse);
					break;
				case 's':
					iCurWin->BaseWin()->SetShadowDisabled(ETrue);
					break;
				case 'S':
					iCurWin->BaseWin()->SetShadowDisabled(EFalse);
					break;
				case 'v':
					iCurWin->SetVisible(ETrue);
					break;
				case 'i':
					iCurWin->SetVisible(EFalse);
					break;
				case 'd':
					iCurWin->BaseWin()->SetFaded(ETrue
						,(iIncludeChildren? RWindowTreeNode::EFadeIncludeChildren:RWindowTreeNode::EFadeWindowOnly));
					break;
				case 'D':
					iCurWin->BaseWin()->SetFaded(EFalse
						,(iIncludeChildren? RWindowTreeNode::EFadeIncludeChildren:RWindowTreeNode::EFadeWindowOnly));
					break;
				case 'w':
				case 'W':
					iIncludeChildren=!iIncludeChildren;
				case 'e':
					iCurWin->BaseWin()->SetNonFading(ETrue);
					break;
				case 'E':
					iCurWin->BaseWin()->SetNonFading(EFalse);
					break;
				case 'r':
					iCurWin->BaseWin()->FadeBehind(ETrue);
					break;
				case 'R':
					iCurWin->BaseWin()->FadeBehind(EFalse);
					break;
				/*case 'b':
					iCurWin->WinTreeNode()->SetOrdinalPosition(-1);
					break;
				case 'f':
					iCurWin->WinTreeNode()->SetOrdinalPosition(0);
					break;*/
				case 'B':
					{
					RWindowTreeNode* win=iCurWin->WinTreeNode();
					win->SetOrdinalPosition(win->OrdinalPosition()+1);
					}
					break;
				case 'F':
					{
					RWindowTreeNode* win=iCurWin->WinTreeNode();
					win->SetOrdinalPosition(win->OrdinalPosition()-1);
					}
					break;
				default:
					goto keeptrying2;
				}
			return;
			}
keeptrying2:
		switch(aKey.iCode)
			{
			case 24:			//Ctrl-X
				CTWin::Delete(iCurWin);
				break;
			case 19:			//Ctrl-S
				if (aKey.iModifiers&EModifierShift)
					iCurWin=iCurWin->Prev();
				else
					iCurWin=iCurWin->Next();
				break;
			case EKeyLeftArrow:
				iCurWin->AdjustSizeL(-Xmove,0,aKey.iModifiers);
				break;
			case EKeyRightArrow:
				iCurWin->AdjustSizeL(Xmove,0,aKey.iModifiers);
				break;
			case EKeyUpArrow:
				iCurWin->AdjustSizeL(0,-Ymove,aKey.iModifiers);
				break;
			case EKeyDownArrow:
				iCurWin->AdjustSizeL(0,Ymove,aKey.iModifiers);
				break;
			case '[':		// Move back 1 ordinal position
				iCurWin->AdjustOrdinal(-1);
				break;
			case ']':		// Move forward 1 ordinal position
				iCurWin->AdjustOrdinal(1);
				break;
			case '<':		// Decrease shadow size
				iCurWin->AdjustShadow(-1);
				break;
			case '>':		// Increase shadow size
				iCurWin->AdjustShadow(1);
				break;
			case 13:
//				iCurWin->SetDefaultExtL();
				iCurWin->SetFullScreenExtL();
				break;
			default:
				iCurWin->WinKeyL(aKey,aTime);
				break;
			}
		}
	}

void TestWindowGroup::RunLaunchAndShootTestL()
	{
	RThread thread;
	User::LeaveIfError(thread.Create(_L("Launcher"),RunLaunchAndShootTest,KDefaultStackSize,NULL,
						NULL,NULL,KTestThreadHeapSize,KTestThreadHeapSize,EOwnerThread));
	thread.Resume();
	thread.Close();
	}

void TestWindowGroup::MillionsOfSessions()
	{
	RWsSession session;
	FOREVER
		{
		session=RWsSession();
		if (session.Connect()!=KErrNone)
			break;
		}
	}

//

TestClient::TestClient()
	{
	__DECLARE_NAME(_S("TestClient"));
	}

CTBaseWin *TestClient::CreateTestWindowL(TInt type,TPoint pos,TSize size,CTWinBase *parent,CWindowGc &aGc,TBool aAlternate/*=EFalse*/)
	{
	CTBaseWin *win=NULL;
	switch(type)
		{
		case 1:
			win=new(ELeave) TestWindow1();
			break;
		case 2:
			win=new(ELeave) TestWindow2();
			break;
		case 3:
			win=new(ELeave) TestWindow3();
			break;
		case 4:
			win=new(ELeave) TestWindow4();
			break;
		case 5:
			win=new(ELeave) TestWindow5(aAlternate);
			break;
		case 6:
			win=new(ELeave) TestWindow6(aAlternate);
			break;
		case 7:
			win=new(ELeave) TestWindow7(aAlternate);
			break;
		case 8:
			win=new(ELeave) TestWindow8();
			break;
		case 9:
			win=new(ELeave) TestWindow9();
			break;
		case 10:
			win=new(ELeave) TestWindow10();
			break;
		case 11:
			win=new(ELeave) TestWindow11();
			break;
		case 12:
			win=new(ELeave) TestWindow12();
			break;
		default:
			User::Leave(KErrArgument);
		}
	TRAPD(err,win->ConstructL(*parent));
	if (err!=KErrNone)
		goto ctw_err;
	win->AssignGC(aGc);
	TRAP(err,win->SetExtL(pos,size));
	if (err!=KErrNone)
		{
ctw_err:
		delete win;
		User::Leave(err);
		}
	if (type!=2)
		win->BaseWin()->EnableBackup();
	win->BaseWin()->SetPointerCursor(0);
	win->Activate();
	return(win);
	}

TestClient::~TestClient()
	{
	delete iHandWriting;
	}

//

// Define to include hand writing tests
#define TEST_HWR
void TestClient::ConstructL()
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) TestWindowGroup(this);
	iGroup->ConstructL();
//	CreateTestWindowL(1,TPoint(15,20),TSize(150,110),iGroup,*iGc);
//	CreateTestWindowL(2,TPoint(100,100),TSize(200,90),iGroup,*iGc);
//	CreateTestWindowL(3,TPoint(310,190),TSize(90,40),iGroup,*iGc);
	CreateTestWindowL(4,TPoint(10,10),TSize(160,120),iGroup,*iGc);
//	CreateTestWindowL(5,TPoint(300,14),TSize(260,140),iGroup,*iGc);
//	CreateTestWindowL(6,TPoint(20,200),TSize(160,60),iGroup,*iGc);
	iGroup->SetCurrentWindow(iGroup->Child());
#if defined(TEST_HWR)
	iHandWriting=new(ELeave) CHandWriting(iWs);
	TRAPD(err,iHandWriting->ConstructL(iScreen->SizeInPixels(),*iGroup->GroupWin(),ETrue));   //EFalse don't use seperate Mask bitmap.
	if (err!=KErrNone)
		delete iHandWriting;
	else
		TestWindow()->SetHandWriting(iHandWriting);
#endif
	}

void TestClient::AdjustLightSource(TInt aX,TInt aY)
	{
	TPoint vector=iWs.ShadowVector();
	vector.iX+=aX;
	vector.iY+=aY;
	iWs.SetShadowVector(vector);
	}

void TestClient::Exit()
	{
	CActiveScheduler::Stop();
	}

//
// Special bit of test code to repeatedly launch and shoot a wserv //
//

#define NT_KILLER

TInt LaunchAndShootTestLoop(TAny *)
	{
#if defined(NT_KILLER)
	User::WaitForAnyRequest();
#else 

	RWsSession ws;
	ws.Connect();
	TTimeIntervalMicroSeconds32 initialTime;
	TTimeIntervalMicroSeconds32 time;
	FOREVER
		ws.GetKeyboardRepeatRate(initialTime, time);
#endif
	return(KErrNone);
	}

#if defined(NT_KILLER)
TInt RunLaunchAndShootTest(TAny *)
	{
	RThread().SetPriority(EPriorityRealTime);
	RThread thread;
	thread.Create(_L("xx1"),LaunchAndShootTestLoop,KDefaultStackSize,NULL,
						NULL,NULL,KTestThreadHeapSize,KTestThreadHeapSize,EOwnerThread);
	thread.Resume();
	thread.Kill(KErrNone);
	thread.Close();
	RThread thread2;
	thread2.Create(_L("xx2"),LaunchAndShootTestLoop,KDefaultStackSize,NULL,
						NULL,NULL,KTestThreadHeapSize,KTestThreadHeapSize,EOwnerThread);
	thread2.Resume();
	thread2.Kill(KErrNone);
	thread2.Close();
	return(KErrNone);
	}
#else
TInt RunLaunchAndShootTest(TAny *)
	{
	RThread().SetPriority(EPriorityRealTime);
	for(TInt count=0;count<100;count++)
		{
		RThread thread;
		TBuf<KMaxFileName> threadName;
		TInt num=0;
		TInt err=KErrNone;
		do
			{
			threadName.Format(_L("%S%02d"),&_L("Looper"),num++);
			err=thread.Create(threadName,LaunchAndShootTestLoop,KDefaultStackSize,NULL,
							NULL,NULL,KTestThreadHeapSize,KTestThreadHeapSize,EOwnerThread);
			} while(err==KErrAlreadyExists);
		if (err!=KErrNone)
			return(KErrNone);
		thread.Resume();
		User::After(TTimeIntervalMicroSeconds32(50000));
		thread.Kill(KErrNone);
		thread.Close();
		}
	return(KErrNone);
	}
#endif
	
//
#define leavescan_needs_to_see_some_brackets_here	{	}

GLDEF_C CTClient* CreateClientL()
	{
	return(new(ELeave) TestClient());	   //Leavescan thinks this fn is  user::After!
	}

GLDEF_C TInt E32Main()
	{
	return(TestLibStartUp(CreateClientL));
	}
