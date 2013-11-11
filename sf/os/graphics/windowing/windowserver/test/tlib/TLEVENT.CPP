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
// Maintains a window displaying last event details
// 
//

#include <e32std.h>
#include <w32std.h>
#include <e32svr.h>
#include "testbase.h"

const TInt NumLogLinesPerEvent=2;


//
// Event window //
//

EXPORT_C CEventWindow::CEventWindow(TInt aLogSize) : CTWin(), iLogSize(aLogSize)
	{
	}

EXPORT_C CEventWindow::~CEventWindow()
	{
	delete[] iLoggedEvents;
	RelinquishFocus();
	}

EXPORT_C void CEventWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	iLineHeight=iFont->HeightInPixels()+2;
	iLoggedEvents=new(ELeave) TWsEvent[iLogSize];
	iWin.EnableModifierChangedEvents(EModifierFunc|EModifierCapsLock|EModifierNumLock, EEventControlAlways);
	iWin.EnableOnEvents(EEventControlAlways);
	}

EXPORT_C void CEventWindow::SetUpL(const TPoint &pos, CTWinBase *parent, CWindowGc &aGc)
	{
	TRAPD(err,ConstructL(*parent));
	if (err!=KErrNone)
		{
		delete this;
		User::Leave(err);
		}
	SetExt(pos,TSize(600,iLineHeight*NumLogLinesPerEvent*iLogSize));
	Activate();
	AssignGC(aGc);
	}

void CEventWindow::DrawLine(TInt aLine, const TDesC &aText)
	{
	iGc->DrawText(aText, TPoint(10,iLineHeight*aLine+iFont->AscentInPixels()+1));
	}

void CEventWindow::LogEvent(TInt aLogNum, const TWsEvent &aEvent)
	{
	TBuf<0x80> buf1;
	TBuf<0x80> buf2;
	TPtrC type(_L("Unknown event type"));
	TKeyEvent *key=aEvent.Key();
	switch(aEvent.Type())
		{
		case EEventKey:
			type.Set(_L("EEventKey"));
			buf2.Format(TRefByValue<const TDesC>(_L("Code=%d [%c], ScanCode=0x%x, Modifiers=0x%04x, repeats=%d")), key->iCode, key->iCode, key->iScanCode,key->iModifiers,key->iRepeats);
			break;
		case EEventKeyUp:
			type.Set(_L("EEventKeyUp"));
			buf2.Format(TRefByValue<const TDesC>(_L("scanCode=0x%x, Modifiers=0x%04x")), key->iScanCode, key->iModifiers);
			break;
		case EEventKeyDown:
			type.Set(_L("EEventKeyDown"));
			buf2.Format(TRefByValue<const TDesC>(_L("scanCode=0x%x, Modifiers=0x%04x")), key->iScanCode, key->iModifiers);
			break;
		case EEventPointer:
			{
			TPointerEvent *pointer=aEvent.Pointer();
			TPtrC ptrType(_L("Unknown pointer event"));
			switch(pointer->iType)	
				{
				case TPointerEvent::EButton1Up:
					ptrType.Set(_L("EButton1Up"));
					break;
				case TPointerEvent::EButton3Up:
					ptrType.Set(_L("EButton3Up"));
					break;
				case TPointerEvent::EButton2Up:
					ptrType.Set(_L("EButton2Up"));
					break;
				case TPointerEvent::EButton1Down:
					ptrType.Set(_L("EButton1Down"));
					break;
				case TPointerEvent::EButton3Down:
					ptrType.Set(_L("EButton3Down"));
					break;
				case TPointerEvent::EButton2Down:
					ptrType.Set(_L("EButton2Down"));
					break;
				case TPointerEvent::EDrag:
					ptrType.Set(_L("EDrag"));
					break;
				case TPointerEvent::EMove:
					ptrType.Set(_L("EMove"));
					break;
				case TPointerEvent::EButtonRepeat:
					ptrType.Set(_L("EButtonRepeat"));
					break;
				case TPointerEvent::ESwitchOn:
					ptrType.Set(_L("ESwitchOn"));
					break;
				}
			type.Set(_L("EEventPointer"));
			buf2.Format(TRefByValue<const TDesC>(_L("Type=%S, state=0x%x, pos={%d,%d}, parent pos={%d,%d}")),&ptrType, pointer->iModifiers,
				pointer->iPosition.iX,pointer->iPosition.iY,pointer->iParentPosition.iX,pointer->iParentPosition.iY);
			}
			break;
		case EEventPointerEnter:
			type.Set(_L("EEventPointerEnter"));
			break;
		case EEventPointerExit:
			type.Set(_L("EEventPointerExit"));
			break;
		case EEventSwitchOn:
			type.Set(_L("EEventSwitchOn"));
			break;
		case EEventModifiersChanged:
			type.Set(_L("EEventModifiersChanged"));
			buf2.Format(TRefByValue<const TDesC>(_L("Changed=0x%x, State=0x%x ")),aEvent.ModifiersChanged()->iChangedModifiers,aEvent.ModifiersChanged()->iModifiers);
			break;
		case EEventFocusLost:
			type.Set(_L("EEventFocusLost"));
			break;
		case EEventFocusGained:
			type.Set(_L("EEventFocusGained"));
			break;
		default:;
		}
	TBuf<20> timeBuf;
	_LIT(TimeDisc,"%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S%:3");
	TRAPD(err,aEvent.Time().FormatL(timeBuf,TimeDisc));
	if (err!=KErrNone)
		{
		_LIT(DummyTime,"########");
		timeBuf.Append(DummyTime);
		}
	buf1.Format(TRefByValue<const TDesC>(_L("%d: %S [%x], %S")), iCount-aLogNum, &type, aEvent.Handle(), &timeBuf);
	TInt baseLine=(iLogSize-aLogNum-1)*NumLogLinesPerEvent;
	DrawLine(baseLine+0,buf1);
	DrawLine(baseLine+1,buf2);
	}

EXPORT_C void CEventWindow::Draw()
	{
	DrawBorder();
	for(TInt index=0;index<iNumLogged;index++)
		LogEvent(index,iLoggedEvents[index]);
	}

EXPORT_C void CEventWindow::WinKeyL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CEventWindow::LogEvent(const TWsEvent &aEvent)
	{
	iCount++;
	if (iNumLogged<iLogSize)
		iNumLogged++;
	for(TInt index=iNumLogged-1;index>0;index--)
		iLoggedEvents[index]=iLoggedEvents[index-1];
	iLoggedEvents[0]=aEvent;
	DrawNow();
	}


//
// Blank window, just sort of sits there looking blank //
//

EXPORT_C TBool CheckBlankWindow(TRect aArea,TRgb aColor,const CWsScreenDevice* aScreen)
//
// Returns ETrue if the the given rect is all the specified color
// EFalse if it is not
//
	{
	TInt wid=aArea.Width();
	TAny *buf2=User::AllocL(wid*sizeof(TRgb));
	TRgb *pRgb=(TRgb *)buf2;
	Mem::FillZ(buf2,wid*sizeof(TRgb));
	//TRgb tmp(TRgb::Gray16(aColor.Gray16()));
	//Truncate color to match color conversion in EColor64K mode before comparison
	if (aScreen->DisplayMode()==EColor64K)
		aColor=TRgb::Color64K(aColor.Color64K());
	for(TInt i=0;i<wid;i++,pRgb++)
		*pRgb=aColor;
	TPtr8 tstBuf(reinterpret_cast<TUint8*>(buf2),wid*sizeof(TRgb),wid*sizeof(TRgb));

	TAny *buf=User::AllocL(wid*sizeof(TRgb));
	TPtr8 rgbBuf(reinterpret_cast<TUint8*>(buf),wid*sizeof(TRgb));

	TBool ret=ETrue;
	TPoint offset=aArea.iTl;		//iWin.InquireOffset(*TheClient->iGroup->WinTreeNode());
	for(;offset.iY<aArea.iBr.iY;offset.iY++)
		{
		aScreen->GetScanLine(rgbBuf,offset,wid,EColor16MA);
		if (rgbBuf.Compare(tstBuf)!=0)
			{
			ret=EFalse;
			break;
			}
		}
	User::FreeZ(buf);
	User::FreeZ(buf2);
	return(ret);
	}

EXPORT_C CBlankWindow::CBlankWindow() : CTWin()
	{
	iCol=TRgb::Gray256(128);
	}

EXPORT_C CBlankWindow::CBlankWindow(TRgb aCol) : CTWin(), iCol(aCol), iRealDraw(EFalse)
	{
	}

EXPORT_C void CBlankWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	iWin.SetBackgroundColor(iCol);
	}

EXPORT_C void CBlankWindow::SetColor(TRgb aColor)
	{
	iCol=aColor;
	}

EXPORT_C void CBlankWindow::RealDraw(TBool aRealDraw)
	{
	iRealDraw=aRealDraw;
	}

EXPORT_C void CBlankWindow::Draw()
	{
	if (!iRealDraw)
		iGc->Clear();
	else
		{
		iGc->SetPenStyle(CGraphicsContext::ENullPen);
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetBrushColor(iCol);
		iGc->DrawRect(TRect(iSize));
		}
	}

EXPORT_C void CBlankWindow::DrawNow()
	{
	iWin.Invalidate();
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	Draw();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

EXPORT_C void CBlankWindow::DrawNow(TRect& aRect)
	{
	iWin.Invalidate(aRect);
	iWin.BeginRedraw(aRect);
	iGc->Activate(iWin);
	iGc->SetClippingRect(aRect);
	Draw();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

EXPORT_C TBool CBlankWindow::Check(const CTClient& aClient)
//
// Returns ETrue if the window is Ok,
// EFalse if it is not
//
	{
	return CheckBlankWindow(TRect(iWin.InquireOffset(*aClient.iGroup->WinTreeNode()),Size()),TRgb::Gray16(iCol.Gray16()),aClient.iScreen);
	}
