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
// Base classes used for building window server test code
// 
//

#include <e32std.h>
#include <w32std.h>
#include <e32svr.h>
#include <e32property.h>
#include "TLIB.H"
#include <graphics/wsgraphicdrawerinterface.h>

//
// Test code classes
//
// CTWinBase
// CTBaseWin
// CTWin
// CTBackedUpWin
// CTGroupWin
// CTClient
//
//

_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");

class CStopTheScheduler : public CAsyncOneShot
	{
public:
	inline CStopTheScheduler(CTClient* aClient,TInt aPriority,TBool aAll) :CAsyncOneShot(aPriority), iClient(aClient), iAll(aAll) {}
	inline CStopTheScheduler(CTClient* aClient,TInt aPriority) :CAsyncOneShot(aPriority), iClient(aClient) {}
	inline CStopTheScheduler(TInt aPriority) :CAsyncOneShot(aPriority) {}
	void RunL();
public:
	CTClient* iClient;
	TBool iAll;
#ifdef __WINS__
	TInt iCStopTheSchedulerRunCount;
#endif
	};


void TbPanic(TInt aPanic)
	{
	User::Panic(_L("TestBase"),aPanic);
	}

EXPORT_C CTWinBase::CTWinBase(TInt aType) : iType(aType)
	{
	__DECLARE_NAME(_S("CTWinBase"));
	}

EXPORT_C CTWinBase *CTWinBase::Parent() const
	{
	return((CTWinBase *)WinTreeNode()->Parent());
	}

EXPORT_C CTWinBase *CTWinBase::NextSibling() const
	{
	return((CTWinBase *)WinTreeNode()->NextSibling());
	}

EXPORT_C CTWinBase *CTWinBase::PrevSibling() const
	{
	return((CTWinBase *)WinTreeNode()->PrevSibling());
	}

EXPORT_C CTClient *CTWinBase::Client() const
	{
	return(iOwnerWin->Client());
	}

EXPORT_C CTBaseWin *CTWinBase::Child() const
	{
	return((CTBaseWin *)WinTreeNode()->Child());
	}

EXPORT_C TPoint CTWinBase::Position() const
	{
	return(TPoint(0,0));
	}

EXPORT_C void CTWinBase::PointerEnter(const TTime&)
	{
	}

EXPORT_C void CTWinBase::PointerExit(const TTime&)
	{
	}

EXPORT_C void CTWinBase::PointerBufferReady(const TTime&)
	{
	}

EXPORT_C void CTWinBase::PointerL(const TPointerEvent&,const TTime&)
	{
	}

EXPORT_C void CTWinBase::SwitchOn(const TTime &)
	{
	}

EXPORT_C void CTWinBase::ModifiersChanged(const TModifiersChangedEvent &,const TTime &)
	{
	}

EXPORT_C void CTWinBase::AdjustOrdinal(TInt)
	{
	}

EXPORT_C void CTWinBase::AdjustShadow(TInt)
	{
	}

EXPORT_C void CTWinBase::SetVisible(TBool)
	{
	}

EXPORT_C TInt CTWinBase::SubType()
	{
	return(0);
	}

EXPORT_C void CTWinBase::ErrorMessage(const TWsErrorMessage&, const TTime &)
	{
	}

EXPORT_C void CTWinBase::__DbgTestInvariant() const
	{
	WinTreeNode()->__DbgTestInvariant();
	}

// CTBaseWin //

EXPORT_C CTBaseWin::CTBaseWin(TInt aType) : CTWinBase(aType)
	{
	__DECLARE_NAME(_S("CTBaseWin"));
	}

EXPORT_C CTBaseWin::~CTBaseWin()
	{
	}

EXPORT_C void CTBaseWin::RelinquishFocus()
	{
	CTWinBase *parent=Parent();
	CTWindowGroup *group=Group();
	CTWinBase *child=group->Child();
	if (child==this)
		child=child->NextSibling();
	group->SetCurrentWindow((parent->iType==EWinTypeClient) ? (CTBaseWin *)parent : (CTBaseWin *)child);
	}

EXPORT_C void CTBaseWin::SetVisible(TBool aState)
	{
	BaseWin()->SetVisible(aState);
	}

EXPORT_C const RWindowTreeNode *CTBaseWin::WinTreeNode() const
	{
	return((const RWindowTreeNode *)BaseWin());
	}

EXPORT_C RWindowTreeNode *CTBaseWin::WinTreeNode()
	{
	return((RWindowTreeNode *)BaseWin());
	}

EXPORT_C void CTBaseWin::InitWin()
	{
	}

EXPORT_C void CTBaseWin::ConstructExtLD(CTWinBase &aParent, const TPoint &aPos, const TSize &aSize)
//
// Call ConstructL, SetExt and either of these fail destroy this and leave
//
	{
	TRAPD(err,ConstructL(aParent));
	if (err!=KErrNone)
		goto celd_err;
	TRAP(err,SetExtL(aPos,aSize));
	if (err!=KErrNone)
		{
celd_err:
		delete this;
		User::Leave(err);
		}
	}

EXPORT_C void CTBaseWin::ConstructL(CTWinBase &aParent)
	{
	TInt ret;
	__ASSERT_DEBUG(aParent.iOwnerWin!=NULL,TbPanic(ETestBasePanicNullOwnerWin));
	iOwnerWin=aParent.iOwnerWin;
	if ((ret=ConstructWin(aParent))==KErrNone)
		{
		//TFontSpec fspec(KTestFontTypefaceName,200);
		//User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont, fspec));
		TFontSpec fspec(KTestFontTypefaceName,17);
		User::LeaveIfError(Client()->iScreen->GetNearestFontToDesignHeightInPixels((CFont *&)iFont, fspec));
		AdjustShadow(1);
		InitWin();
		iSize=BaseWin()->Size();
		}
	User::LeaveIfError(ret);
	}

EXPORT_C void CTBaseWin::Activate()
	{
	BaseWin()->Activate();
	}

EXPORT_C void CTBaseWin::SetFullScreenExtL()
	{
	SetExtL(TPoint(0,0), Parent()->Size());
	}

EXPORT_C void CTBaseWin::SetDefaultExtL()
	{
	TSize size=Parent()->Size();
	size.iWidth>>=1;
	size.iHeight>>=1;
	SetExtL(TPoint(size.iWidth>>1,size.iHeight>>1), size);
	}

EXPORT_C void CTBaseWin::AssignGC(CWindowGc &aGc)
	{
	iGc= &aGc;
	}

EXPORT_C void CTBaseWin::SetPos(const TPoint &aPos)
	{
	BaseWin()->SetPosition(aPos);
	iPos=aPos;
	}

EXPORT_C void CTBaseWin::SetInitialPos(const TPoint &aPos)
	{
	TSize screen=Client()->iScreen->SizeInPixels();
	TPoint pos(aPos);
	TPoint botLeft=pos+iSize;
	if (botLeft.iX>screen.iWidth)
		pos.iX=Max(0,pos.iX-botLeft.iX+screen.iWidth);
	if (botLeft.iY>screen.iHeight)
		pos.iY=Max(0,pos.iY-botLeft.iY+screen.iHeight);
	SetPos(pos);
	}

EXPORT_C void CTBaseWin::SetExtL(const TPoint &aPos, const TSize &aSize)
	{
	User::LeaveIfError(BaseWin()->SetExtentErr(aPos,aSize));
	iPos=aPos;
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C void CTBaseWin::AdjustSizeL(TInt xMove,TInt yMove,TInt modifiers)
	{
	if (modifiers&EModifierCtrl)	// 4 times the movement
		{
		xMove<<=2;
		yMove<<=2;
		}
	if (modifiers&EModifierShift)
		{
		TSize size(iSize.iWidth+xMove,iSize.iHeight+yMove);
	
		if (size.iWidth<0)
			size.iWidth=0;
		if (size.iHeight<0)
			size.iHeight=0;
		if (modifiers&EModifierCtrl)
			{
			TPoint pos(iPos);
			pos.iX-=(xMove>>1);
			pos.iY-=(yMove>>1);
			SetExtL(pos,size);
			}
		else
			SetSizeL(size);
		}
	else
		SetPos(TPoint(iPos+TPoint(xMove,yMove)));
	}

EXPORT_C void CTBaseWin::SetSizeL(const TSize &aSize)
	{
	User::LeaveIfError(BaseWin()->SetSizeErr(aSize));
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C TSize CTBaseWin::Size() const
	{
	return(iSize);
	}

EXPORT_C void CTBaseWin::Resized(const TSize &aSize)
//
	{
	SetDragRect(TRect(aSize));
	}

EXPORT_C TPoint CTBaseWin::Position() const
	{
	return(BaseWin()->Position());
	}

EXPORT_C void CTBaseWin::Delete(CTBaseWin *aWin)
	{
	RHeap& heap = User::Heap();
	TUint8* base=heap.Base();
	TInt size=heap.Size();
	TUint8* cell=REINTERPRET_CAST(TUint8*,aWin);
	__ASSERT_ALWAYS(base<=cell && cell<base+size, TbPanic(ETestBasePanicInvalidHeapAddress));
	CTWinBase *tmp=aWin;
	CTWinBase *win2;
	CTBaseWin *curwin=NULL;
	if (tmp->Group())
		curwin=tmp->Group()->CurWin();
	do
		{
		win2=tmp;
		while((tmp=win2->Child())!=NULL)
			win2=tmp;
		if ((tmp=win2->NextSibling())==NULL)
			tmp=win2->Parent();
		if (curwin==win2)
			((CTBaseWin *)win2)->RelinquishFocus();
		delete win2;
		} while(win2!=aWin);
	}

EXPORT_C void CTBaseWin::KeyUpL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTBaseWin::KeyDownL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTBaseWin::WinKeyL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTBaseWin::PointerL(const TPointerEvent &aPointer,const TTime&)
	{
	if (iDragging)
		{
		if (aPointer.iType==TPointerEvent::EDrag)
			SetPos(aPointer.iParentPosition-iDragPos);
		else
			iDragging=EFalse;
		return;
		}
	if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		if (aPointer.iModifiers&EModifierCtrl)
			{
			if (aPointer.iModifiers&EModifierShift)
				BaseWin()->SetOrdinalPosition(-1);
			else
				BaseWin()->SetOrdinalPosition(0);
			return;
			}
#if defined(__WINS__)
		else if (aPointer.iModifiers&EModifierShift)
			__DbgTestInvariant();
#endif
		else
			{
			Group()->SetCurrentWindow(this);
			if (iDragRect.Contains(aPointer.iPosition))
				{
				iDragging=ETrue;
				iDragPos=aPointer.iPosition;
				return;
				}
			}
		}
	}

EXPORT_C void CTBaseWin::DragDropL(const TPointerEvent &,const TTime &)
	{
	}

EXPORT_C void CTBaseWin::AdjustOrdinal(TInt aAdjust)
	{
	TInt pos=BaseWin()->OrdinalPosition()+aAdjust;
	if (pos>=0)
		BaseWin()->SetOrdinalPosition(pos);
	}

EXPORT_C void CTBaseWin::AdjustShadow(TInt aAdjust)
	{
	iShadow+=aAdjust;
	if (iShadow<0)
		iShadow=0;
	BaseWin()->SetShadowHeight(iShadow);
	}

EXPORT_C void CTBaseWin::DrawBorder()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->DrawRect(TRect(iSize));
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	}

EXPORT_C void CTBaseWin::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc, TDisplayMode *aMode,TBool aVisible,TInt aTransparency)
	{
	ConstructL(*parent);
	if (aMode)
		BaseWin()->SetRequiredDisplayMode(*aMode);
	SetExtL(pos,size);
	if (!aVisible)
		BaseWin()->SetVisible(aVisible);
	if (aTransparency!=ENoTransparency)
		{
		TInt err=((RWindow*)(BaseWin()))->SetTransparencyFactor(TRgb::_Gray256(aTransparency));	
		User::LeaveIfError(err);	//asked for transparency when not got none!
		}
	Activate();
	AssignGC(aGc);
	}

EXPORT_C void CTBaseWin::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc, TDisplayMode *aMode)
	{
	SetUpL(pos,size,parent,aGc,aMode,ETrue);
	}

EXPORT_C void CTBaseWin::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc,TBool aVisible)
	{
	SetUpL(pos,size,parent,aGc,NULL,aVisible);
	}

EXPORT_C void CTBaseWin::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	SetUpL(pos,size,parent,aGc,NULL,ETrue);
	}

EXPORT_C void CTBaseWin::SetDragRect(const TRect &aRect)
	{
	BaseWin()->PointerFilter(EPointerFilterDrag,0);	// Clear the drag filter
	BaseWin()->SetPointerGrab(ETrue);
	iDragRect=aRect;
	}

EXPORT_C CTWindowGroup *CTBaseWin::Group() const
	{
	return(iOwnerWin);
	}

EXPORT_C void CTBaseWin::FocusChanged(TBool )
	{
	}

EXPORT_C CTBaseWin *CTBaseWin::Next()
	{
	CTWinBase *win;
	CTWinBase *ret=Child();
	if (ret==NULL)
		{
		win=this;
		while((ret=win->NextSibling())==NULL)
			{
			ret=win;
			win=win->Parent();
			if (win==NULL)	// Group window
				{
				ret=ret->Child();
				break;
				}
			}
		}
	return((CTBaseWin *)ret);
	}

EXPORT_C CTBaseWin *CTBaseWin::Prev()
	{
	CTWinBase *par=Parent();
	CTWinBase *win;
	CTWinBase *child;
	if ((win=PrevSibling())==NULL)
		{
		if (par->iType==EWinTypeGroup)
			{
			CTWinBase *win2=par->Child();
			do
				{
				win=win2;
				while((win2=win->NextSibling())!=NULL)
					win=win2;
				win2=win->Child();
				} while(win2!=NULL);
			}
		else
			win=par;
		}
	else
		{
		child=win->Child();
		while(child!=NULL)
			{
			win=child;
			child=child->NextSibling();
			}
		}
	return((CTBaseWin *)win);
	}

//EXPORT_C void CTBaseWin::SetDefaultExt()
//	{
//	TbPanic(ETestBasePanicUnimplementedBaseFunction);
//	}

EXPORT_C void CTBaseWin::Draw()
	{
	TbPanic(ETestBasePanicUnimplementedBaseFunction);
	}

EXPORT_C void CTBaseWin::__DbgTestInvariant() const
	{
	CTWinBase::__DbgTestInvariant();
#if defined(__WINS__)
	if (BaseWin()->Size()!=iSize)
		User::Invariant();
	TPoint offset;
	const CTWinBase *win=this;
	const CTWinBase *win2;
	while((win2=win->Parent())!=NULL)
		{
		offset+=((CTBaseWin *)win)->BaseWin()->Position();
		if (BaseWin()->InquireOffset(*(win2->WinTreeNode()))!=offset)
			User::Invariant();
		win=win2;
		}
#endif
	}

//CTDrawableWin//
//	Drawable window					   //
//

EXPORT_C CTDrawableWin::CTDrawableWin(TInt aType) : CTBaseWin(aType)
	{
	__DECLARE_NAME(_S("CTDrawableWin"));
	}

//CTWin//
//	Standard client window					   //
//

EXPORT_C CTWin::CTWin() : CTDrawableWin(EWinTypeClient)
	{
	__DECLARE_NAME(_S("CTWin"));
	}

EXPORT_C CTWin::~CTWin()
	{
	if (iFont)
		Client()->iScreen->ReleaseFont(iFont);
	iWin.Close();
	}

EXPORT_C TInt CTWin::ConstructWin(const CTWinBase &aParent)
	{
	iWin=RWindow(aParent.Client()->iWs);
	return(iWin.Construct(*(aParent.WinTreeNode()),(TUint32)this));
	}

EXPORT_C void CTWin::SetDefaultExt()
	{
	TSize size=Parent()->Size();
	size.iWidth>>=1;
	size.iHeight>>=1;
	SetExt(TPoint(size.iWidth>>1,size.iHeight>>1), size);
	Invalidate();
	}

EXPORT_C void CTWin::SetExt(const TPoint &aPos, const TSize &aSize)
	{
	iWin.SetExtent(aPos,aSize);
	iPos=aPos;
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C void CTWin::SetSize(const TSize &aSize)
	{
	iWin.SetSize(aSize);
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C void CTWin::AdjustSize(TInt xMove,TInt yMove,TInt modifiers)
	{
	if (modifiers&EModifierCtrl)	// 4 times the movement
		{
		xMove<<=2;
		yMove<<=2;
		}
	if (modifiers&EModifierShift)
		{
		TSize size(iSize.iWidth+xMove,iSize.iHeight+yMove);
	
		if (size.iWidth<0)
			size.iWidth=0;
		if (size.iHeight<0)
			size.iHeight=0;
		if (modifiers&EModifierCtrl)
			{
			TPoint pos(iPos);
			pos.iX-=(xMove>>1);
			pos.iY-=(yMove>>1);
			SetExt(pos,size);
			}
		else
			SetSize(size);
		Invalidate();
		}
	else
		SetPos(TPoint(iPos+TPoint(xMove,yMove)));
	}

EXPORT_C RWindowBase *CTWin::BaseWin()
	{
	return((RWindowBase *)&iWin);
	}

EXPORT_C const RWindowBase *CTWin::BaseWin() const
	{
	return((const RWindowBase *)&iWin);
	}

EXPORT_C RDrawableWindow *CTWin::DrawableWin()
	{
	return((RDrawableWindow *)&iWin);
	}

EXPORT_C const RDrawableWindow *CTWin::DrawableWin() const
	{
	return((const RDrawableWindow *)&iWin);
	}


EXPORT_C void CTWin::Invalidate()
	{
	iWin.Invalidate();
	}

EXPORT_C void CTWin::Invalidate(const TRect &rect)
	{
	iWin.Invalidate(rect);
	}

EXPORT_C void CTWin::DrawNow()
	{
	iWin.Invalidate();
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

EXPORT_C void CTWin::Redraw()
	{
	iWin.BeginRedraw();
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

EXPORT_C void CTWin::Redraw(const TRect &aRect)
	{
	iWin.BeginRedraw(aRect);
	iGc->Activate(iWin);
	iGc->UseFont((CFont *)iFont);
	Draw();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

//CTBackedUpWin//
//	Backed up window						   //
//

EXPORT_C CTBackedUpWin::CTBackedUpWin(TDisplayMode aDisplayMode) : CTDrawableWin(EWinTypeClient), iDisplayMode(aDisplayMode)
	{
	__DECLARE_NAME(_S("CTBackedUpWin"));
	}

EXPORT_C CTBackedUpWin::~CTBackedUpWin()
	{
	if (iFont)
		Client()->iScreen->ReleaseFont(iFont);
	iWin.Close();
	}

EXPORT_C TInt CTBackedUpWin::ConstructWin(const CTWinBase &aParent)
	{
	iWin=RBackedUpWindow(aParent.Client()->iWs);
	return(iWin.Construct(*(aParent.WinTreeNode()), iDisplayMode, (TUint32)this));
	}

EXPORT_C RWindowBase *CTBackedUpWin::BaseWin()
	{
	return((RWindowBase *)&iWin);
	}

EXPORT_C const RWindowBase *CTBackedUpWin::BaseWin() const
	{
	return((const RWindowBase *)&iWin);
	}

EXPORT_C RDrawableWindow *CTBackedUpWin::DrawableWin()
	{
	return((RDrawableWindow *)&iWin);
	}

EXPORT_C const RDrawableWindow *CTBackedUpWin::DrawableWin() const
	{
	return((const RDrawableWindow *)&iWin);
	}

EXPORT_C RBackedUpWindow *CTBackedUpWin::BackedUpWin()
	{
	return &iWin;
	}

EXPORT_C const RBackedUpWindow *CTBackedUpWin::BackedUpWin() const
	{
	return &iWin;
	}

//CTTitledWindow//

EXPORT_C CTTitledWindow::CTTitledWindow() : CTWin(), iWinColor(TRgb(255,255,255)), iPenColor(TRgb(0,0,0))
	{
	__DECLARE_NAME(_S("CTTitledWin"));
	}

EXPORT_C CTTitledWindow::~CTTitledWindow()
	{
	}

EXPORT_C void CTTitledWindow::ConstructL(CTWinBase &parent)
	{
	CTBaseWin::ConstructL(parent);
	iTitleHeight=iFont->HeightInPixels()+4;
	if (iTitle.Length()==0)
		SetTitle(*Client()->Title());
	}

EXPORT_C void CTTitledWindow::SetColor(TRgb aRgb)
	{
	iWinColor=aRgb;
	iWin.SetBackgroundColor(aRgb);
	}

EXPORT_C void CTTitledWindow::Resized(const TSize &aSize)
	{
	SetDragRect(TRect(0,0,aSize.iWidth,iTitleHeight));
	}

EXPORT_C void CTTitledWindow::SetTitle(const TWindowTitle &aTitle)
	{
	iTitle=aTitle;
	}

EXPORT_C void CTTitledWindow::Draw()
	{
	iGc->SetPenColor(iPenColor);
	iGc->SetBrushColor(iWinColor);
	DrawBorder();
	iGc->DrawLine(TPoint(0,iTitleHeight),TPoint(iSize.iWidth,iTitleHeight));
	if (Group()->HasFocus(this))
		{
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetPenColor(~iPenColor);
		iGc->SetBrushColor(~iWinColor);
		}
	iGc->DrawText(iTitle, TRect(1,1,iSize.iWidth-1,iTitleHeight),iFont->AscentInPixels()+1,CGraphicsContext::ECenter);
	iGc->SetPenColor(iPenColor);
	iGc->SetBrushColor(iWinColor);
	}

EXPORT_C void CTTitledWindow::FocusChanged(TBool )
	{
	iWin.Invalidate(TRect(0,0,iSize.iWidth,iTitleHeight));
	}

// CTBlankWindow //

EXPORT_C CTBlankWindow::CTBlankWindow() : CTBaseWin(EWinTypeClient)
	{
	__DECLARE_NAME(_S("CTBlankWin"));
	}

EXPORT_C void CTBlankWindow::ConstructL(CTWinBase &aParent)
	{
	CTBaseWin::ConstructL(aParent);
	}

EXPORT_C CTBlankWindow::~CTBlankWindow()
	{
	if (iFont)
		Client()->iScreen->ReleaseFont(iFont);
	iWin.Close();
	}

EXPORT_C void CTBlankWindow::SetExt(const TPoint &aPos, const TSize &aSize)
	{
	iWin.SetExtent(aPos,aSize);
	iPos=aPos;
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C void CTBlankWindow::SetSize(const TSize &aSize)
	{
	iWin.SetSize(aSize);
	iSize=aSize;
	Resized(iSize);
	}

EXPORT_C void CTBlankWindow::SetColor(const TRgb &aRgb)
	{
	iWin.SetColor(aRgb);
	}

EXPORT_C TInt CTBlankWindow::ConstructWin(const CTWinBase &aParent)
	{
	iWin=RBlankWindow(aParent.Client()->iWs);
	return(iWin.Construct(*(aParent.WinTreeNode()),(TUint32)this));
	}

EXPORT_C const RWindowBase *CTBlankWindow::BaseWin() const
	{
	return((const RWindowBase *)&iWin);
	}

EXPORT_C RWindowBase *CTBlankWindow::BaseWin()
	{
	return((RWindowBase *)&iWin);
	}

//CTWindowGroup//

EXPORT_C CTWindowGroup::CTWindowGroup(CTClient *aClient) : CTWinBase(EWinTypeGroup), iGroupWin(aClient->iWs)
	{
	__DECLARE_NAME(_S("CTWindowGroup"));
	iClient=aClient;
	iOwnerWin=this;
	}

EXPORT_C CTWindowGroup::~CTWindowGroup()
	{
	iGroupWin.Close();
	}

EXPORT_C const RWindowTreeNode *CTWindowGroup::WinTreeNode() const
	{
	return((const RWindowTreeNode *)&iGroupWin);
	}

EXPORT_C RWindowTreeNode *CTWindowGroup::WinTreeNode()
	{
	return((RWindowTreeNode *)&iGroupWin);
	}

EXPORT_C void CTWindowGroup::ConstructL()
	{
	User::LeaveIfError(iGroupWin.Construct((TUint32)this));
	}

EXPORT_C TSize CTWindowGroup::Size() const
	{
	return(iClient->iScreen->SizeInPixels());
	}

EXPORT_C void CTWindowGroup::WinKeyL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTWindowGroup::KeyL(const TKeyEvent &aKey,const TTime&aTime)
	{
	if (iCurWin)
		iCurWin->WinKeyL(aKey,aTime);
	}

EXPORT_C void CTWindowGroup::KeyUpL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTWindowGroup::KeyDownL(const TKeyEvent &,const TTime&)
	{
	}

EXPORT_C void CTWindowGroup::ClearCurrentWindow()
	{
	iCurWin=NULL;
	}

EXPORT_C void CTWindowGroup::SetCurrentWindow(CTBaseWin *aWindow)
	{
	SetCurrentWindow(aWindow, EFalse);
	}

EXPORT_C void CTWindowGroup::SetCurrentWindow(CTBaseWin *aWindow, TBool aLocked)
	{
	if (iCurWin!=aWindow)
		{
		if (iFocus && iCurWin)
			iCurWin->FocusChanged(EFalse);
		iCurWin=aWindow;
		if (iFocus && iCurWin)
			iCurWin->FocusChanged(ETrue);
		}
	iLocked=aLocked;
	}

EXPORT_C CTBaseWin *CTWindowGroup::CurWin(void) const
	{
	return(iCurWin);
	}

EXPORT_C void CTWindowGroup::FocusLost()
	{
	iFocus=EFalse;
	if (iCurWin)
		iCurWin->FocusChanged(EFalse);
	}

EXPORT_C void CTWindowGroup::FocusGained()
	{
	iFocus=ETrue;
	if (iCurWin)
		iCurWin->FocusChanged(ETrue);
	}

EXPORT_C TBool CTWindowGroup::HasFocus(CTBaseWin *aWin) const
	{
	return(iFocus && iCurWin==aWin);
	}

EXPORT_C CTClient *CTWindowGroup::Client() const
	{
	return(iClient);
	}

EXPORT_C CTWindowGroup *CTWindowGroup::Group() const
	{
	return((CTWindowGroup *)this);
	}

EXPORT_C void CTWindowGroup::PasswordL(const TTime &)
	{
	TbPanic(ETestBasePanicPassword);
	}

EXPORT_C void CTWindowGroup::MessageReady(const TWsEvent &)
//
// Dummy handler for un-exepected messages (could panic, but better not as the app sending the messages fault really not ours)
//
	{
	}

EXPORT_C void CTWindowGroup::ScreenDeviceChanged()
	{
	TPixelsAndRotation sizeAndRotation;
	Client()->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	Client()->iScreen->SetScreenSizeAndRotation(sizeAndRotation);
	}

EXPORT_C void CTWindowGroup::UserEvent(TInt /*aEventType*/)
	{
	}


// CTClient //

EXPORT_C CTClient::CTClient()
	{
	__DECLARE_NAME(_S("CTClient"));
	}

EXPORT_C void CTClient::DestroyWindows()
	{
	if (iGroup)
		{
		CTBaseWin *win;
		if (iGroup->GroupWin()->WsHandle()!=0)	// Check it was created okay
			while((win=iGroup->Child())!=NULL && ((TUint)win)!=ENullWsHandle)
				CTBaseWin::Delete(win);
		delete iGroup;
		iGroup=NULL;
		}
	}

EXPORT_C CTClient::~CTClient()
	{
	DestroyWindows();
	delete iGc;
	delete iEventHandler;
	delete iRedrawEventHandler;
	delete iScreen;
	TInt count=iWs.ResourceCount();
	__ASSERT_ALWAYS(count==0,TbPanic(ETestBasePanicResourceCount));
	iWs.Close();
	}

EXPORT_C void CTClient::ConstructEventHandlerL()
	{
	iEventHandler=new(ELeave) CTEvent(&iWs);
	iEventHandler->Construct();
	}

EXPORT_C void CTClient::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScreen=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreen->Construct(iScreenNumber));
	iRedrawEventHandler=new(ELeave) CTRedraw(&iWs);
	iRedrawEventHandler->Construct();
	ConstructEventHandlerL();
	iGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	iTitle.Copy(RThread().FullName());
	}

EXPORT_C void CTClient::ResetFocus()
	{
	iGroup->ClearCurrentWindow();
	iGroup->SetCurrentWindow(iGroup->Child());
	}

EXPORT_C TWindowTitle *CTClient::Title()
	{
	return(&iTitle);
	}

EXPORT_C TBool CTClient::QueueRead()
	{
	TBool ret=iEventHandler->IsActive();
	if (ret==EFalse)
		iEventHandler->Request();
	return(ret);
	}

EXPORT_C void CTClient::CancelRead()
	{
	iEventHandler->Cancel();
	}

EXPORT_C void CTClient::SetCancelFunction(const TCallBack &aCallBack)
	{
	iEventHandler->SetCancelFunction(aCallBack);
	}

EXPORT_C void CTClient::CancelRedrawRead()
	{
	iRedrawEventHandler->Cancel();
	}

EXPORT_C void CTClient::SetRedrawCancelFunction(const TCallBack &aCallBack)
	{
	iRedrawEventHandler->SetCancelFunction(aCallBack);
	}

EXPORT_C void CTClient::RequestRedraw()
	{
	iRedrawEventHandler->Request();
	}

EXPORT_C void CTClient::LogMessage(const TLogMessageText &aMessage)
	{
	iWs.LogMessage(aMessage);
	iWs.Flush();
	}

EXPORT_C TBool CTClient::IsEventWaiting()
	{
	iWs.NumWindowGroups();		//Make sure all asyncronus calls have finished
	return (iEventHandler->iStatus!=KRequestPending);
	}

TBool CTClient::WaitUntilEventPending(const TRequestStatus& aStatus)
	{
	if (aStatus!=KRequestPending)
		return ETrue;
	User::After(2000000);		//Need something smarter than this
	return (aStatus!=KRequestPending);
	}

EXPORT_C TBool CTClient::WaitUntilRedrawPending()
	{
	return WaitUntilEventPending(iRedrawEventHandler->iStatus);
	}

EXPORT_C TBool CTClient::WaitUntilEventPending()
	{
	return WaitUntilEventPending(iEventHandler->iStatus);
	}

EXPORT_C TInt CTClient::WaitForRedrawsToFinish()
	{
	return WaitForEventsToFinish(EFalse);
	}

EXPORT_C TInt CTClient::WaitForAllEventProcessingToFinish()
	{
	return WaitForEventsToFinish(ETrue);
	}

TInt CTClient::WaitForEventsToFinish(TBool aAll)
	{
	CStopTheScheduler* stop=new CStopTheScheduler(this,ETlibRedrawActivePriority-1,aAll);
	if (!stop)
		return KErrNoMemory;
	stop->Call();
	CActiveScheduler::Start();
	delete stop;
	return KErrNone;
	}


// CStopTheScheduler //

void CStopTheScheduler::RunL()
	{
#ifdef __WINS__
	RDebug::Print(_L("CStopTheScheduler::RunL - enter - %d"), iCStopTheSchedulerRunCount);
#endif
	if (iClient)
	    {
		iClient->iWs.NumWindowGroups();		//Make sure all asyncronus calls have finished
	    }
	
	if (!iClient || (iClient->RedrawHandler()->iStatus==KRequestPending && (!iAll || iClient->EventHandler()->iStatus==KRequestPending)))
	    {
#ifdef __WINS__
	    RDebug::Print(_L("CStopTheScheduler::RunL - Stop - %d"), iCStopTheSchedulerRunCount);
#endif
	    CActiveScheduler::Stop();
	    }
	else
	    {  
#ifdef __WINS__
	    RDebug::Print(_L("CStopTheScheduler::RunL - Call - %d"), iCStopTheSchedulerRunCount);
#endif
	    Call();
	    }
#ifdef __WINS__
    RDebug::Print(_L("CStopTheScheduler::RunL - exit - %d"), iCStopTheSchedulerRunCount);
    iCStopTheSchedulerRunCount++;
#endif
	}

void WaitForRedrawsToFinish()
	{
	CStopTheScheduler* ps=new CStopTheScheduler(ETlibRedrawActivePriority-1);
	if(ps)
		{
		ps->Call();
		CActiveScheduler::Start();
		delete ps;
		}
	}


// CTEventBase //

EXPORT_C CTEventBase::CTEventBase(RWsSession *aWs, TInt aPriority) : CActive(aPriority), iWs(aWs)
	{
	}

EXPORT_C CTEventBase::~CTEventBase()
	{
	}

EXPORT_C void CTEventBase::Construct()
	{
	CActiveScheduler::Add(this);
	Request();
	}

EXPORT_C void CTEventBase::RunL()
	{
	if (iStatus==KErrNone)
		{
		++iCount;
		TRAPD(err,doRunL());
		if (err<=0)	// Positive value means this has been destroyed
			{
			if (iCancelRequested)
				CancelHandler();
			Request();
//			if (err!=KErrNone)
//				User::LeaveIfError(err); Should have a guaranteed to work error dialog here I guess
			}
		}
	else if (iStatus==KErrCancel && iCancelRequested)
		{
		CancelHandler();
		Request();
		}
	else
		TbPanic(ETestBasePanicEventStat);
	}

void CTEventBase::SetCancelFunction(const TCallBack &aCallBack)
	{
	if (!IsActive() && iCancelRequested)
		TbPanic(ETestBasePanicCancelFunction);
	iCancelCallBack=aCallBack;
	iCancelRequested=ETrue;
	DoCancel();
	}

EXPORT_C void CTEventBase::CancelHandler()
	{
	iCancelCallBack.CallBack();
	iCancelRequested=EFalse;
	}

// TLibWsEvent //

EXPORT_C CTWindowGroup *TlibWsEvent::WindowGroup()
	{
	CTWindowGroup *group=(CTWindowGroup *)Handle();
	__ASSERT_DEBUG(group->iType==EWinTypeGroup,TbPanic(ETestBasePanicWinType));
	return(group);
	}

EXPORT_C CTBaseWin *TlibWsEvent::BaseWin()
	{
	CTBaseWin *win=(CTBaseWin *)Handle();
	__ASSERT_DEBUG(win->iType==EWinTypeClient || win->iType==EWinTypeGroup,TbPanic(ETestBasePanicWinType));
	return(win);
	}

// CTEvent //

EXPORT_C CTEvent::CTEvent(RWsSession *aWs) : CTEventBase(aWs, ETlibWsEventActivePriority)
	{
	__DECLARE_NAME(_S("CTEvent"));
	}

EXPORT_C CTEvent::~CTEvent()
 	{
	Cancel();
	}

EXPORT_C void CTEvent::DoCancel()
	{
	iWs->EventReadyCancel();
	}

EXPORT_C void CTEvent::Request()
	{
	iWs->EventReady(&iStatus);
	SetActive();
	}

EXPORT_C void CTEvent::LogEvent(const TWsEvent &)
	{
	}

EXPORT_C void CTEvent::doRunL()
	{
	TlibWsEvent event;
	
	iWs->GetEvent(event);
	LogEvent(event);
	if (event.Handle()!=0 && event.Handle()!=ENullWsHandle)
		{
		switch(event.Type())
			{
			case EEventKey:
				event.WindowGroup()->KeyL(*event.Key(),event.Time());
				break;
			case EEventKeyDown:
				__ASSERT_ALWAYS(event.Key()->iCode==0 && event.Key()->iRepeats==0, TbPanic(ETestBasePanicKeyParams));
				event.WindowGroup()->KeyDownL(*event.Key(),event.Time());
				break;
			case EEventKeyUp:
				__ASSERT_ALWAYS(event.Key()->iCode==0 && event.Key()->iRepeats==0, TbPanic(ETestBasePanicKeyParams));
				event.WindowGroup()->KeyUpL(*event.Key(),event.Time());
				break;
			case EEventModifiersChanged:
				event.BaseWin()->ModifiersChanged(*event.ModifiersChanged(),event.Time());
				break;
			case EEventPointer:
				event.BaseWin()->PointerL(*event.Pointer(),event.Time());
				break;
			case EEventDragDrop:
				event.BaseWin()->DragDropL(*event.Pointer(),event.Time());
				break;
			case EEventPointerEnter:
				event.BaseWin()->PointerEnter(event.Time());
				break;
			case EEventPointerExit:
				event.BaseWin()->PointerExit(event.Time());
				break;
			case EEventPointerBufferReady:
				event.BaseWin()->PointerBufferReady(event.Time());
				break;
			case EEventSwitchOn:
				event.BaseWin()->SwitchOn(event.Time());
				break;
			case EEventFocusLost:
				event.WindowGroup()->FocusLost();
				break;
			case EEventFocusGained:
				event.WindowGroup()->FocusGained();
				break;
			case EEventPassword:
				event.WindowGroup()->PasswordL(event.Time());
				break;
			case EEventMessageReady:
				event.WindowGroup()->MessageReady(event);
				break;
			case EEventErrorMessage:
				event.WindowGroup()->ErrorMessage(*event.ErrorMessage(), event.Time());
				break;
			case EEventSwitchOff:
			case EEventKeySwitchOff:
				{
				TTimeIntervalMicroSeconds32 ii=1000000;
				User::After(ii);			//WINS does not always work without this!
				UserHal::SwitchOff();
				}
				break;
			case EEventScreenDeviceChanged:
				event.WindowGroup()->ScreenDeviceChanged();
				break;
			case EEventNull:
				break;
			default:
				if (event.Type()>=EEventUser)
					{
					event.WindowGroup()->UserEvent(event.Type());
					break;
					}
				else
					{
					//if not in BufferSecurity test - panic
					TInt value = EFalse;
					TInt err = RProperty::Get(KUidWServSecurityTesting,EWServSecTestBufferSecurity,value);
					if ((err != KErrNone) || (value != (TInt)ETrue))
						TbPanic(ETestBasePanicInvalidEvent);
					}
				break;
			}
		}
	}

EXPORT_C CTRedraw::CTRedraw(RWsSession *aWs) : CTEventBase(aWs, ETlibRedrawActivePriority)
	{
	__DECLARE_NAME(_S("CTRedraw"));
	}

EXPORT_C CTRedraw::~CTRedraw()
	{
	Cancel();
	}

EXPORT_C void CTRedraw::Request()
	{
	iWs->RedrawReady(&iStatus);
	SetActive();
	}

EXPORT_C void CTRedraw::DoCancel()
	{
	iWs->RedrawReadyCancel();
	}

EXPORT_C void CTRedraw::doRunL()
	{
	TWsRedrawEvent redraw;
	iWs->GetRedraw(redraw);
	if (redraw.Handle()!=0 && redraw.Handle()!=ENullWsHandle)
		{
		__ASSERT_ALWAYS(!redraw.Rect().IsEmpty(),TbPanic(ETestBasePanicNullRedraw));
		((CTWin *)redraw.Handle())->Redraw(redraw.Rect());
		}
	}

EXPORT_C void CTUser::Splat(CTClient *aClient, const TRect &aRect, const TRgb &aRgb)
	{
	RBlankWindow win(aClient->iWs);
	win.Construct(*(aClient->iGroup->WinTreeNode()),1);
	win.SetColor(aRgb);
	win.SetExtent(aRect.iTl,aRect.Size());
	win.Activate();
	aClient->iWs.Flush();
	win.Close();
	}

void doTestLibStartUpL(TInt aScreenNumber, CTClient *&aClient, CActiveScheduler *&aActiveScheduler,CTrapCleanup *&aCleanUpStack,TCreateClientFunc aFunc)
	{
	User::LeaveIfNull(aCleanUpStack=CTrapCleanup::New());
	aActiveScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(aActiveScheduler);
	aClient=aFunc();
	aClient->SetScreenNumber(aScreenNumber);
	aClient->ConstructL();
	CActiveScheduler::Start();
	}

EXPORT_C TInt TestLibStartUp(TCreateClientFunc aFunc,TInt aScreenNumber)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanUpStack=NULL;
	CActiveScheduler *activeScheduler=NULL;
	CTClient *client=NULL;
	TRAPD(err,doTestLibStartUpL(aScreenNumber, client,activeScheduler,cleanUpStack,aFunc));
	delete client;
	delete activeScheduler;
	delete cleanUpStack;
	__UHEAP_MARKEND;
	return(err);
	}

EXPORT_C TInt TestLibStartUp(TCreateClientFunc aFunc)
	{
	return TestLibStartUp(aFunc, KDefaultScreen);
	}

