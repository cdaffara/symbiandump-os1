// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Functions to act as bench marks for various window server features
// 
//

#include "TTIME.H"
#include <e32svr.h>
#include <e32def_private.h>
typedef TBuf<32> TestNameBuf;

LOCAL_D const TUint KHeapSize=0x10000;

class CTimeClient;

class CResultDialog : public CTDialog
	{		  
public:
	CResultDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc);
	void ButtonPressL(TInt aButton);
	void ConstructLD();
private:
	CTWindowGroup *iGroupWin;
	CWindowGc *iGc;
	};

class CListWindow : public CTTitledWindow
	{
public:
	CListWindow();
	void CloseWindow();
	void ConstructL(CTWinBase &parent, TBool aExitOnEscape);
	void Draw();
	virtual void SelectedL(TInt aIndex)=0;
	virtual void WinKeyL(const TKeyEvent &aKey,const TTime& aTime);
	void SetExt(const TPoint &aPos, const TSize &aSize);
	void SetSize(const TSize &);
protected:
	virtual TPtrC GetText(TInt aLine)=0;
	virtual TInt ListCount()=0;
	void SetSize();
private:
	void Resized(const TSize &aSize);
	void SetListPos(TInt aNewPos);
	TInt TextRowHeight() const;
	void RowBox(TRect &aRect, TInt aRow) const;
	void PointerL(const TPointerEvent &aPointer,const TTime& aTime);
private:
	TInt iListPos;
	TTime iPrevTime;
	TBool iExitOnEscape;
	};

class CTestList : public CListWindow
	{
public:
	CTestList();
	~CTestList();
	void ConstructL(CTWinBase &parent);
	static void AppendProfileNum(TDes &aDes, TInt aNum);
	static void AppendProfileCount(TDes &aDes, TInt aNum);
	void SelectedL(TInt aIndex);
	virtual void WinKeyL(const TKeyEvent &aKey,const TTime& aTime);
	void ForegroundAppDialog();
	void AppendToListL(const TDesC &aDesc);
private:
	virtual TPtrC GetText(TInt aLine);
	virtual TInt ListCount();
private:
	TInt iCount;
	CArrayFixSeg<TestNameBuf> iTestNames;
	RThread iTimeTest;
	};

class CTimeTestWindowGroup : public CTWindowGroup
	{
public:
	CTimeTestWindowGroup(CTClient *aClient);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	};

class CTimeClient : public CTClient
	{
public:
	CTimeClient();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Exit();
	CTWin *CreateTestWindowL(TPoint pos,CTWinBase *parent);
private:
	TInt iNum;
	};

const TInt Xmove=8;
const TInt Ymove=6;

GLREF_D TTimeTestHeader MovingWindowTest1;
GLREF_D TTimeTestHeader MovingWindowTest2;
GLREF_D TTimeTestHeader StackedWindowCreate;
GLREF_D TTimeTestHeader WindowCreateDestroy;
GLREF_D TTimeTestHeader LoadsaText;
GLREF_D TTimeTestHeader DrawBitmapTest;
GLREF_D TTimeTestHeader XorIngTest;
GLREF_D TTimeTestHeader SmallClearTest;
GLREF_D TTimeTestHeader RectCompareTest;
GLREF_D TTimeTestHeader SimpleFlushTest;
GLREF_D TTimeTestHeader SimpleFlushTest2;
GLREF_D TTimeTestHeader UseFontTest;
GLREF_D TTimeTestHeader BitBltTest;
GLREF_D TTimeTestHeader FullScreenBitBltTest;
GLREF_D TTimeTestHeader MaskedBitBltTest;
GLREF_D TTimeTestHeader SpriteTest;
GLREF_D TTimeTestHeader BitmapDeviceTest1;
GLREF_D TTimeTestHeader BitmapDeviceTest2;
GLREF_D TTimeTestHeader FillPatternTest;
GLREF_D TTimeTestHeader BitmapLoadTest;
GLREF_D TTimeTestHeader WsBitmapLoadTest;
GLREF_D TTimeTestHeader RomFileTest;
GLREF_D TTimeTestHeader TrivialFunctionsTest;
GLREF_D TTimeTestHeader BackupWindowDrawingCreate1;
GLREF_D TTimeTestHeader BackupWindowDrawingCreate2;
GLREF_D TTimeTestHeader MenuEmulationCreate;
GLREF_D TTimeTestHeader MenuEmulationCreate2;

TTimeTestHeader *tests[]={
	&MenuEmulationCreate,
	&MenuEmulationCreate2,
	&BitmapLoadTest,
	&WsBitmapLoadTest,
	&RomFileTest,
	&TrivialFunctionsTest,
	&BackupWindowDrawingCreate1,
	&BackupWindowDrawingCreate2,
	&MovingWindowTest1,
	&MovingWindowTest2,
	&StackedWindowCreate,
//	&WindowCreateDestroy,
	&LoadsaText,
	&DrawBitmapTest,
//	&BitmapDeviceTest1,
//	&BitmapDeviceTest2,
//	&XorIngTest,		List getting too big
//	&SmallClearTest,	List getting too big
//	&RectCompareTest,
	&SpriteTest,
//	&SimpleFlushTest,
//	&SimpleFlushTest2,
//	&UseFontTest,
	&BitBltTest,
//	&FullScreenBitBltTest,
	&MaskedBitBltTest,
//	&FillPatternTest,
	};

void Panic(TInt aPanic)
	{
	User::Panic(_L("TimeTest"),aPanic);
	}

struct TThreadParams
	{
	TInt iIndex;
	TInt iGroupId;
	};

TInt TimeThread(TAny *aParams)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	//__PROFILE_START(0)
	TInt ret=tests[((TThreadParams *)aParams)->iIndex]->function(((TThreadParams *)aParams)->iGroupId);
	//__PROFILE_END(0)
	delete CleanUpStack;
	return ret;
	}

//
// List Window //
//

CListWindow::CListWindow() : CTTitledWindow(), iPrevTime(0)
	{
	}

void CListWindow::CloseWindow()
	{
	CTClient *client=((CTimeClient *)Client());
	delete this;
	client->ResetFocus();
	}

void CListWindow::SetExt(const TPoint &aPos, const TSize &)
	{
	SetPos(aPos);
	}

void CListWindow::SetSize(const TSize &)
	{
	}

void CListWindow::SetSize()
	{
	iSize.iHeight=ListCount()*(iFont->HeightInPixels()+1)+iTitleHeight+2;
	iSize.iWidth=iFont->TextWidthInPixels(*Client()->Title())+30;
	for(TInt index=0;index<ListCount();index++)
		{
		TInt wid=iFont->TextWidthInPixels(GetText(index));
		if (wid>iSize.iWidth)
			iSize.iWidth=wid;
		}
	iSize.iWidth+=4;
	iWin.SetSize(iSize);
	Resized(iSize);
	}

void CListWindow::ConstructL(CTWinBase &parent, TBool aExitOnEscape)
	{
	iExitOnEscape=aExitOnEscape;
	CTTitledWindow::ConstructL(parent);
	}

void CListWindow::SetListPos(TInt aNewPos)
	{
	if (aNewPos>=0 && aNewPos<ListCount())
		{
		iListPos=aNewPos;
		Invalidate();
		}
	}

void CListWindow::WinKeyL(const TKeyEvent &aKey, const TTime&)
	{
	switch(aKey.iCode)
		{
		case EKeyUpArrow:
			SetListPos(iListPos-1);
			break;
		case EKeyDownArrow:
			SetListPos(iListPos+1);
			break;
		case EKeyEnter:
			SelectedL(iListPos);
			break;
		case EKeyEscape:		// Fall through from EKeyEnter
			if (iExitOnEscape)
				CloseWindow();
			break;
		}
	}

void CListWindow::PointerL(const TPointerEvent &aPointer,const TTime& aTime)
	{
	if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		TRect rect;
		for(TInt index=0;index<ListCount();index++)
			{
			RowBox(rect,index);
			if (rect.Contains(aPointer.iPosition))
				{
				if (index==iListPos && aPointer.iModifiers&EModifierDoubleClick)
					SelectedL(iListPos);
				else
					{
					iPrevTime=aTime;
					SetListPos(index);
					}
				return;
				}
			}
		}
	CTTitledWindow::PointerL(aPointer,aTime);
	}

TInt CListWindow::TextRowHeight() const
	{
	return(iFont->HeightInPixels()+1);
	}

void CListWindow::RowBox(TRect &aRect, TInt aRow) const
	{
	aRect.iTl.iX=2;
	aRect.iTl.iY=iTitleHeight+TextRowHeight()*aRow;
	aRect.iBr.iX=iSize.iWidth-2;
	aRect.iBr.iY=aRect.iTl.iY+TextRowHeight();
	}

void CListWindow::Draw()
	{
	CTTitledWindow::Draw();
	iGc->SetPenColor(TRgb::Gray16(0));
	TPoint pos(2,iTitleHeight+iFont->AscentInPixels()+2);
	TInt gap=TextRowHeight();
	for(TInt index=0;index<ListCount();index++,pos.iY+=gap)
		{
		iGc->DrawText(GetText(index), pos);
		if (index==iListPos)
			{
			iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			iGc->SetBrushColor(TRgb::Gray256(255));
			iGc->SetPenStyle(CGraphicsContext::ENullPen);
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			TRect rect;
			RowBox(rect,index);
			iGc->DrawRect(rect);
			iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
			iGc->SetPenStyle(CGraphicsContext::ESolidPen);
			iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			}
		}
	}

void CListWindow::Resized(const TSize &aSize)
	{
	SetDragRect(TRect(0,0,aSize.iWidth,iTitleHeight));
	}

//
// Test list window
//

CTestList::CTestList() : CListWindow(), iTestNames(4)
	{
	}

CTestList::~CTestList()
	{
	}

void CTestList::ConstructL(CTWinBase &parent)
	{
	CListWindow::ConstructL(parent, EFalse);
	for(TUint index=0;index<(sizeof(tests)/sizeof(tests[0]));index++)
		AppendToListL(TPtrC(tests[index]->title));
	SetSize();
	/*for(TUint index1=0;index1<(sizeof(tests)/sizeof(tests[0]));index1++)
		Selected(index1);*/
	}

void CTestList::AppendProfileNum(TDes &aDes, TInt aNum)
	{
	aDes.AppendFormat(_L("%d.%02d, "),aNum/1000000,(aNum%1000000)/10000);
	}

void CTestList::AppendProfileCount(TDes &aDes, TInt aNum)
	{
	aDes.AppendFormat(_L("%d, "),aNum);
	}

#define USE_PROCESS 1

void CTestList::SelectedL(TInt aIndex)
	{
#if USE_PROCESS
	TThreadParams params;
	params.iIndex=aIndex;
	TName name;
	name.Format(_L("TimeTest-%x"),iCount++);
	params.iGroupId=Client()->iGroup->GroupWin()->Identifier();
	User::LeaveIfError(iTimeTest.Create(name,TimeThread,KDefaultStackSize*2,KHeapSize,KHeapSize,&params,EOwnerThread));
	TRequestStatus status;
	iTimeTest.Logon(status);
	__PROFILE_RESET(8);
	iTimeTest.Resume();
	User::WaitForRequest(status);
#else
	TThreadParams params;
	params.iIndex=aIndex;
	TimeThread(&params);
#endif
	TBuf<64> buf;
	TBuf<64> buf2;
	TBuf<64> buf3;
	CResultDialog *dialog=new(ELeave) CResultDialog(Client()->iGroup, iGc);
	dialog->ConstructLD();
#if USE_PROCESS
	if (status.Int()==KErrNone)
		{
#endif
#if !defined(__PROFILING__)
		buf=_L("Profiling information not available");
#else
		TProfile profile[6];
		__PROFILE_DISPLAY(6);
		for (TInt index=1;index<6;index++)
			AppendProfileNum(buf2,profile[index].iTime);
		for (TInt index2=1;index2<6;index2++)
			AppendProfileCount(buf3,profile[index2].iCount);
		buf.Format(_L("Time=%d.%2d"),profile[0].iTime/1000000,(profile[0].iTime%1000000)/10000);
#endif
		dialog->SetTitle(buf);
#if USE_PROCESS
		}
	else
		{
		dialog->SetTitle(_L("Error in test"));
		buf.Format(_L("Error=%d"),status.Int());
		buf2=iTimeTest.ExitCategory();
		}
#endif
	dialog->SetLine1(buf2);
	dialog->SetLine2(buf3);
	dialog->SetNumButtons(1);
	dialog->SetButtonText(0,_L("Okay"));
	if (dialog->Display()!=0)
		Panic(0);
	}

TPtrC CTestList::GetText(TInt aLine)
	{
	return(TPtrC(iTestNames[aLine]));
	}

TInt CTestList::ListCount()
	{
	return(iTestNames.Count());
	}

void CTestList::WinKeyL(const TKeyEvent &aKey,const TTime& aTime)
	{
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case EKeyLeftArrow:
				AdjustSize(-Xmove,0,aKey.iModifiers);
				break;
			case EKeyRightArrow:
				AdjustSize(Xmove,0,aKey.iModifiers);
				break;
			case EKeyUpArrow:
				AdjustSize(0,-Ymove,aKey.iModifiers);
				break;
			case EKeyDownArrow:
				AdjustSize(0,Ymove,aKey.iModifiers);
				break;
			default:
				goto not_used;
			}
		}
	else
		goto not_used;
	return;
not_used:
	CListWindow::WinKeyL(aKey,aTime);
	}

void CTestList::AppendToListL(const TDesC &aDesc)
	{
	TestNameBuf buf(aDesc);	
	iTestNames.AppendL(buf);
	}

//

CResultDialog::CResultDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc) : CTDialog(),
	iGroupWin(aGroupWin),
	iGc(aGc)
	{
	}

void CResultDialog::ButtonPressL(TInt aButton)
	{
	if (aButton==0)
		CTDialog::ButtonPressL(aButton);
	}

void CResultDialog::ConstructLD()
	{
	CTDialog::ConstructLD(*iGroupWin, *iGc);
	}

//
// CTimeTestWindowGroup class //
//

CTimeTestWindowGroup::CTimeTestWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

void CTimeTestWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case 'x':
				((CTimeClient *)Client())->Exit();
				break;
			}
		}
	else
		iCurWin->WinKeyL(aKey,aTime);
	}

//

CTimeClient::CTimeClient()
	{
	}

CTWin *CTimeClient::CreateTestWindowL(TPoint pos,CTWinBase *parent)
	{
	CTWin *win=new(ELeave) CTestList();
	TRAPD(err,win->ConstructL(*parent));
	if (err!=KErrNone)
		goto ctw_err;
	TRAP(err,win->SetPos(pos));
	if (err!=KErrNone)
		{
ctw_err:
		delete win;
		User::Leave(err);
		}
	win->Activate();
	win->AssignGC(*iGc);
	return(win);
	}

void CTimeClient::ConstructL()
	{
	CTClient::ConstructL();

	iGroup=new(ELeave) CTimeTestWindowGroup(this);
	iGroup->ConstructL();

	CreateTestWindowL(TPoint(30,4),iGroup);
	iGroup->SetCurrentWindow(iGroup->Child());
	}

void CTimeClient::Exit()
	{
	CActiveScheduler::Stop();
	}

GLDEF_C CTClient *CreateClientL()
	{
	return(new(ELeave) CTimeClient());
	}

GLDEF_C TInt E32Main()
	{
	return(TestLibStartUp(CreateClientL));
	}
