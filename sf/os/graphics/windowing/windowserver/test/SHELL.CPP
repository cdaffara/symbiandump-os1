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
// Shell to launch test code
// 
//

#include <e32std.h>
#include <f32file.h>
#include "W32STD.H"
#include "../SERVER/w32cmd.h"
#include <e32svr.h>
#include <hal.h>
#include "../test/tlib/testbase.h"
#include "../test/TClick/CLICK.H"

#define __USING_PROCESS_

#if !defined(__USING_PROCESS_)
#define RP_FILENAME_SET _L(".DLL")
#endif

#define THE_PASSWORD _L("pass")
#define INDEX_FOR_SHELL_TEST 0
#define SHELL_TEST_NAME _L("ShellTest")
#define INDEX_FOR_RESTART_TEST 1
#define RESTART_TEST_NAME _L("RestartTest")
#define INDEX_FOR_RESTART_EVENT_TEST 2
#define RESTART_EVENT_TEST_NAME _L("RestartEventTest")

const TInt EDoubleClickTime=500000;
const TInt EDoubleClickDistance=16;

#ifndef __USING_PROCESS_
LOCAL_D const TUint KHeapSize=0x400000;
#endif

enum TShellPanic
	{
	EShellPanicRunningProcessCancel,
	EShellPanicTemp,
	EShellPanicRunningProcessNullPtr,
	EShellPanicPassword,
	};

typedef TInt (*DllTestAppEntryPoint)(TAny *aParam);

class CShellClient;

class RShellWsSession : public RWsSession
	{
public:
	static void Shutdown();
	void SendShutdown();
	};

class MProcessObserver
	{
public:
	virtual void ProcessDied()=0;
	};

class CRunningProcess : public CActive
	{
public:
	CRunningProcess(MProcessObserver* aObserver);
	~CRunningProcess();
	void ConstructL(const TDesC& aName);
	void ConstructTestExecuteL(const TDesC &aName);
	void Link(CRunningProcess** aRpPtr);
#if defined(__USING_PROCESS_)
	inline RProcess& Process() {return iProcess;}
#else
	inline RThread& Process() {return iProcess;}
#endif
private:
	virtual void DoCancel();
	virtual void RunL();
private:
#if defined(__WINS__)
	RLibrary iLib;
#endif
#if defined(__USING_PROCESS_)
	RProcess iProcess;
#else
	RThread iProcess;
#endif
	CRunningProcess* iNext;
	CRunningProcess** iPrevPtr;
	TParse iFileName;
	MProcessObserver* iObserver;
	};

class CShellErrorDialog : public CTDialog
	{		  
public:
	CShellErrorDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc);
	void ButtonPressL(TInt aButton);
	void ConstructLD();
private:
	CTWindowGroup *iGroupWin;
	CWindowGc *iGc;
	};

class CTCalibWindow : public CTWin
	{
public:
	CTCalibWindow();
	void InitWin();
	void DrawPoint(const TPoint &aPoint, const TDesC &aText);
	void Draw();
	void PointerL(const TPointerEvent &aPointer,const TTime& aTime);
private:
	TInt iCount;
	TDigitizerCalibration iCalibPoints;
	TDigitizerCalibration iReturnValues;
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
	void PointerL(const TPointerEvent &aPointer,const TTime& aTime);
private:
	void Resized(const TSize &aSize);
	void SetListPos(TInt aNewPos);
	TInt TextRowHeight() const;
	void RowBox(TRect &aRect, TInt aRow) const;
private:
	TRgb iColorMap[4];
	TInt iListPos;
	TTime iPrevTime;
	TBool iExitOnEscape;
	};

class CShellWindow : public CListWindow, public MProcessObserver
	{
public:
	CShellWindow();
	~CShellWindow();
	void ConstructL(CTWinBase &parent);
	TBool RunClientL(const TDesC &aName,TBool aTestExecute);
	void SelectedL(TInt aIndex);
	virtual void WinKeyL(const TKeyEvent &aKey,const TTime& aTime);
	void ForegroundAppDialogL();
	void HeapCountDialogL();
	void SetPointerZone(TInt aZone);
	void SetAutoLaunch(TBool aAutoLaunch);
	// Virtual function defined in CTBaseWin and overriden in CListWindow
	void PointerL(const TPointerEvent &aPointer,const TTime& aTime);
	// Virtual function defined in CTBaseWin and overriden in CTTitledWindow
	void FocusChanged(TBool aState);
private:
	void DoShellTests();
	void BugHunt();
	void CheckTerminate();
	//Pure virtual functions defined in CListWindow
	TPtrC GetText(TInt aLine);
	TInt ListCount();
	//Pure virtual function defined in MProcessObserver
	void ProcessDied();
	//Simulates SwitchOffEvent
	void SimulateSwitchOffEvent();
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP	
	//Simulates RestartSystemEvent
	void SimulateRestartEvent();
	//Simulates RestartSystemEvent and test the receiving of off event
	void SimulateAndReceiveRestartEvent();
#endif
	
private:
	TBool iFailModeEnabled;
	TInt iPointerZone;
	CRunningProcess *iProcessList;
	CArrayFixFlat<TFileName> iFileNames;
	CTPointerCursor *iPointerCursor;
	RSoundPlugIn iClick;
	TInt iCaptureKey;
	TBool iAutoLaunch;
	TInt iNumWindowGroups;
	TBool iIsFocused;
	};

class CTIconWindow : public CTBlankWindow
	{
public:
	CTIconWindow(TInt aType);
	void InitWinL();
	void SetExtentL();
private:
	TInt iType;
	};

class CTPasswordWindow: public CTTitledWindow
	{
public:
	CTPasswordWindow();
	void InitWin();
	void Draw();
private:
	TPoint iTextPos;
	};

class CTaskListWindow : public CListWindow
	{
public:
	CTaskListWindow();
	void ConstructL(CTWinBase &parent);
	void SelectedL(TInt aIndex);
private:
	virtual TPtrC GetText(TInt aLine);
	virtual TInt ListCount();
private:
	CArrayFixFlat<TInt> iWindowHandles;
	CArrayFixFlat<TFullName> iWindowThreadNames;
	TBuf<KMaxFullName+KMaxName+2> iLatestText;
	};

class CShellWindowGroup : public CTWindowGroup
	{
public:
	CShellWindowGroup(CTClient *aClient);
	~CShellWindowGroup();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime& aTime);
	void ErrorMessage(const TWsErrorMessage &aErrorMessage, const TTime &);
	void ScreenDeviceChangedL();
private:
	TUint32 iCapHandle1;
	TUint32 iCapHandle2;
	TUint32 iCapHandle3;
	TUint32 iCapHandle4;
	};

class CIconWindowGroup : public CTWindowGroup
	{
public:
	CIconWindowGroup(CTClient *aClient);
	void ConstructL();
	};

class CPasswordWindowGroup : public CTWindowGroup
	{
public:
	CPasswordWindowGroup(CTClient *aClient);
	~CPasswordWindowGroup();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime&aTime);
	void Cancel();
	void PasswordL(const TTime &aTime);
private:
	CTPasswordWindow *iWin;
	TBool iPasswordMode;
	};

class CShellClient : public CTClient
	{
public:
	CShellClient();
	~CShellClient();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime& aTime);
	void Exit();
	void CreateTestWindowL(CTWin *aWin, const TPoint &aPos);
	void CreateTestWindowL(CTWin *win, const TPoint &aPos, CTWinBase *parent);
	void ErrorDialog(const TDesC &aTitle, TInt aErr);
	void ScreenDeviceChangedL();
public:
	RFs iFs;
private:
	TInt iNum;
	CTWindowGroup *iIconGroup;
	CTIconWindow *iIconWin1;
	CTIconWindow *iIconWin2;
	CTPointerCursor *iPointerCursor;
	CPasswordWindowGroup *iPasswordGroup;
	};

class ROverrideProtectionInRSessionBase : public RWsSession
	{
public:
	inline TInt Send(TInt aFunction) const {return(RSessionBase::Send(aFunction,TIpcArgs()));};
	};

const TInt Xmove=8;
const TInt Ymove=6;

const TInt ENumPointerCursors=2;

void DrawShellDragCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aParam);
void DrawShellListCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *aParam);

TSpriteCreateParams spriteParams1(TSize(32,32),TPoint(-16,-16),DrawShellListCursor,NULL,EFalse,CGraphicsContext::EDrawModeXOR);
TSpriteCreateParams spriteParams2(TSize(32,32),TPoint(-16,-16),DrawShellDragCursor,NULL,ETrue,CGraphicsContext::EDrawModePEN);

TSpriteCreateParams *PointerParams[ENumPointerCursors]={&spriteParams1,&spriteParams2};


void Panic(TInt aPanic)
	{
	User::Panic(_L("Shell"),aPanic);
	}

void RShellWsSession::Shutdown()
	{
	RShellWsSession ShellSession;
	ShellSession.Connect();
	ShellSession.SendShutdown();
	}

void RShellWsSession::SendShutdown()
	{
	SendReceive(EWservMessShutdown,TIpcArgs(EWservShutdownCheck));
	}

//
// Sprite drawing functions //
//

void DrawPointerCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(3));
	aGc->SetBrushColor(TRgb::Gray4(3));
	aGc->DrawRect(TRect(0,aSize.iHeight/2-1,aSize.iWidth,aSize.iHeight/2+2));
	aGc->DrawRect(TRect(aSize.iWidth/2-1,0,aSize.iWidth/2+2,aSize.iHeight));
	if (!aDoMask)
		{
		aGc->SetPenColor(TRgb::Gray4(0));
		aGc->DrawLine(TPoint(0,aSize.iHeight/2),TPoint(aSize.iWidth,aSize.iHeight/2));
		aGc->DrawLine(TPoint(aSize.iWidth/2,0),TPoint(aSize.iWidth/2,aSize.iHeight));
		}
	}

void DrawShellDragCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool aDoMask, TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(aDoMask ? 0 : 2));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetPenColor(TRgb::Gray4(aDoMask ? 3 : 1));
	for(TInt y=0;y<aSize.iHeight;y++)
		{
		TInt xfact=aSize.iWidth*Abs(y-aSize.iHeight/2)/aSize.iHeight;
		aGc->DrawLine(TPoint(xfact,y),TPoint(aSize.iWidth-xfact,y));
		}
	}

void DrawShellListCursor(CBitmapContext *aGc,TInt , const TSize &aSize, TBool , TAny *)
	{
	aGc->SetBrushColor(TRgb::Gray4(0));
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->DrawRect(TRect(aSize));
	aGc->SetBrushColor(TRgb::Gray4(3));
	aGc->DrawRect(TRect(0,aSize.iHeight/2-1,aSize.iWidth,aSize.iHeight/2+2));
	aGc->DrawRect(TRect(aSize.iWidth/2-1,0,aSize.iWidth/2+2,aSize.iHeight));
	}

//
// Calibration window //
//

CTCalibWindow::CTCalibWindow()
	{
	}

void CTCalibWindow::InitWin()
	{
	UserHal::CalibrationPoints(iCalibPoints);
	AssignGC(*Client()->iGc);
	iWin.SetOrdinalPosition(0,1);
	iWin.Activate();
	}

void CTCalibWindow::DrawPoint(const TPoint &aPoint, const TDesC &aText)
	{
	iGc->MoveTo(aPoint);
	iGc->MoveBy(TPoint(0,-20));
	iGc->DrawLineBy(TPoint(0,40));
	iGc->MoveBy(TPoint(-20,-20));
	iGc->DrawLineBy(TPoint(40,0));
	iGc->DrawText(aText, aPoint+TPoint(4,-4));
	}

void CTCalibWindow::Draw()
	{
	iGc->Clear();
	DrawPoint(iCalibPoints.iTl,_L("1"));
	DrawPoint(iCalibPoints.iBl,_L("2"));
	DrawPoint(iCalibPoints.iTr,_L("3"));
	DrawPoint(iCalibPoints.iBr,_L("4"));
	}

void CTCalibWindow::PointerL(const TPointerEvent &aPointer,const TTime& )
	{
	if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		switch(iCount++)
			{
			case 0:
				iReturnValues.iTl=aPointer.iPosition;
				break;
			case 1:
				iReturnValues.iBl=aPointer.iPosition;
				break;
			case 2:
				iReturnValues.iTr=aPointer.iPosition;
				break;
			case 3:
				iReturnValues.iBr=aPointer.iPosition;
				UserHal::SetXYInputCalibration(iReturnValues);
				delete this;
				break;
			}
		}
	}

//
// Error dialog //
//

CShellErrorDialog::CShellErrorDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc) : CTDialog(),
	iGroupWin(aGroupWin),
	iGc(aGc)
	{
	}

void CShellErrorDialog::ButtonPressL(TInt aButton)
	{
	if (aButton==0)
		CTDialog::ButtonPressL(aButton);
	}

void CShellErrorDialog::ConstructLD()
	{
	CTDialog::ConstructLD(*iGroupWin, *iGc);
	}

//
// Offscreen icon->button window //
//

CTIconWindow::CTIconWindow(TInt aType) : iType(aType)
	{
	}

void CTIconWindow::InitWinL()
	{
	SetExtentL();
	iWin.SetShadowDisabled(ETrue);
	Activate();
	}

void CTIconWindow::SetExtentL()
	{
	enum {ENumYKeys=5};
	enum {ENumXKeys=9};
	TRect digRect(Client()->iScreen->PointerRect());
	TSize scrSize(Client()->iScreen->SizeInPixels());
	TInt ypos=0;
	TInt count=0;
	iWin.RemoveAllKeyRects();
	if (iType==0)
		{
		for (;count<ENumYKeys;count++)
			{
			TInt oldypos=ypos;
			ypos=digRect.iTl.iY+(count+1)*digRect.Height()/ENumYKeys;
			User::LeaveIfError(iWin.AddKeyRect(TRect(0,oldypos,-digRect.iTl.iX,ypos),'A'+count,ETrue));
			}
		iWin.SetExtent(digRect.iTl,TSize(-digRect.iTl.iX,scrSize.iHeight));
		}
	else
		{
		TInt xpos=0;
		for (count=0;count<ENumXKeys;count++)
			{
			TInt oldxpos=xpos;
			xpos=digRect.iTl.iY+(count+1)*digRect.Width()/ENumXKeys;
			User::LeaveIfError(iWin.AddKeyRect(TRect(oldxpos,0,xpos,digRect.iBr.iY-scrSize.iHeight), EStdKeyF1+count, EFalse));
			}
		iWin.SetExtent(TPoint(digRect.iTl.iX,scrSize.iHeight),TSize(digRect.Width(),digRect.Height()-scrSize.iHeight));
		}
	}

//
// Individual window sub-classes
//

void CShellClient::ErrorDialog(const TDesC &aTitle, TInt aErr)
	{
	TBuf<0x20> err;
	err.Format(TRefByValue<const TDesC>(_L("Error %d")),aErr);
	DisplayDialog(this, aTitle, err,_L(""));
	}

CListWindow::CListWindow() : CTTitledWindow(), iPrevTime(0)
	{
	iColorMap[0]=TRgb::Gray4(0);
	iColorMap[1]=TRgb::Gray4(3);
	iColorMap[2]=TRgb::Gray4(3);
	iColorMap[3]=TRgb::Gray4(0);
	}

void CListWindow::CloseWindow()
	{
	CTClient *client=((CShellClient *)Client());
	CTWin::Delete(this);
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
				if (index==iListPos && (aPointer.iModifiers&EModifierDoubleClick /*|| index<=INDEX_FOR_SHELL_TEST*/))
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
			TRect rect;
			RowBox(rect,index);
			iGc->MapColors(rect,iColorMap);
			}
		}
	}

void CListWindow::Resized(const TSize &aSize)
	{
	SetDragRect(TRect(0,0,aSize.iWidth,iTitleHeight));
	}

//
// CShellWindow class //
//

CShellWindow::CShellWindow() : CListWindow(), iFileNames(4)
	{
	}

CShellWindow::~CShellWindow()
	{
	if (iCaptureKey)
		Client()->iGroup->GroupWin()->CancelCaptureKey(iCaptureKey);
	delete iPointerCursor;
	while(iProcessList)
		delete iProcessList;
	}

void CShellWindow::SetPointerZone(TInt aZone)
	{
	TRAPD(err,iPointerCursor->UpdateL(0, PointerParams[aZone]));
	if (err!=KErrNone)
		((CShellClient *)Client())->ErrorDialog(_L("Error Changing Pointer"), err);
	else
		iPointerZone=aZone;
	}

void CShellWindow::SetAutoLaunch(TBool aAutoLaunch)
	{
	iAutoLaunch=aAutoLaunch;
	if (aAutoLaunch)
		iNumWindowGroups=Client()->iWs.NumWindowGroups();
	}

void CShellWindow::PointerL(const TPointerEvent &aPointer,const TTime& aTime)
	{
	TInt zone=(aPointer.iPosition.iY<iTitleHeight) ? 1 : 0;
	if (iPointerZone!=zone)
		SetPointerZone(zone);
	CListWindow::PointerL(aPointer,aTime);
	}

void CShellWindow::ConstructL(CTWinBase &parent)
	{
	CListWindow::ConstructL(parent, EFalse);
	RFile dllList;
	RProcess thisProc;
	TParse dllListFileName;
#if defined(__USING_PROCESS_)
	TFileName fName(thisProc.FileName());
	User::LeaveIfError(dllListFileName.Set(_L("\\SYSTEM\\DATA\\DLL_LIST.TXT"),&fName,NULL));
	User::LeaveIfError(dllList.Open(((CShellClient *)Client())->iFs,dllListFileName.FullName(),EFileShareReadersOnly|EFileStreamText));
#else
	User::LeaveIfError(dllList.Open(((CShellClient *)Client())->iFs,_L("Z:\\SYSTEM\\DATA\\DLL_LIST.TXT"),EFileShareReadersOnly|EFileStreamText));
#endif
	TBuf8<0x200> dllNames;
	User::LeaveIfError(dllList.Read(dllNames));
	dllList.Close();
	TPtrC8 name(dllNames);
	FOREVER
		{
		TInt pos=name.Locate('\r');
		if (pos==KErrNotFound)
			break;
		if (pos<=KMaxFileName)
			{
			TFileName buf;
			buf.Copy(name.Left(pos));
			iFileNames.AppendL(buf);
			}
		name.Set(name.Mid(pos+2));
		};
	SetSize();
	BaseWin()->PointerFilter(EPointerFilterMove|EPointerFilterDrag,0);
	iPointerCursor=new(ELeave) CTPointerCursor(Client()->iWs);
	iPointerCursor->ConstructL(0);
	SetPointerZone(0);
	iWin.SetCustomPointerCursor(iPointerCursor->PointerCursor());
	iClick=RSoundPlugIn(Client()->iWs);
	User::LeaveIfError(iClick.Construct(TUid::Uid(CLICK_THIRD_UID)));
	TBool isChangeAble;
	if (iClick.IsLoaded(isChangeAble))
		{
		TInt ret=Client()->iGroup->GroupWin()->CaptureKey(3,EModifierCtrl|EModifierShift|EModifierFunc
																	,EModifierCtrl|EModifierShift|EModifierFunc);	//Ctrl-Alt-Shift-C
		User::LeaveIfError(ret);
		iCaptureKey=ret;
		}
	}

TBool CShellWindow::RunClientL(const TDesC &aName,TBool aTestExecute)
	{
	if (aName.Length()!=0)
		{
		CRunningProcess* rp=new(ELeave) CRunningProcess(this);
		rp->Link(&iProcessList);
		CActiveScheduler::Add(rp);
		TInt err=KErrNone;		//To stop a warning
		if (aTestExecute)
			{
			TRAP(err,rp->ConstructTestExecuteL(aName));
			}
		else
			{
			TRAP(err,rp->ConstructL(aName));
			}
		if (err!=KErrNone)
			{
			delete rp;
			User::Leave(err);
			}
		return ETrue;
		}
	return EFalse;
	}

void CShellWindow::SelectedL(TInt aIndex)
	{
	TPtrC name=GetText(aIndex);
	if (aIndex==INDEX_FOR_SHELL_TEST && name==SHELL_TEST_NAME)
		{
		DoShellTests();
		return;
		}
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP		
	else if (aIndex==INDEX_FOR_RESTART_TEST && name==RESTART_TEST_NAME)
		{
		SimulateRestartEvent();
		return;
		}
	else if (aIndex==INDEX_FOR_RESTART_EVENT_TEST && name==RESTART_EVENT_TEST_NAME)
		{
		SimulateAndReceiveRestartEvent();
		return;
		}
#endif		
	TRAPD(err,RunClientL(GetText(aIndex),EFalse));
	if (err==KErrNotFound)
		{
		TRAP(err,RunClientL(GetText(aIndex),ETrue));
		}
	if (err!=KErrNone)
		((CShellClient *)Client())->ErrorDialog(_L("Error launching process"), err);
	}

TPtrC CShellWindow::GetText(TInt aLine)
	{
	return(TPtrC(iFileNames[aLine]));
	}

TInt CShellWindow::ListCount()
	{
	return(iFileNames.Count());
	}

void CShellWindow::ForegroundAppDialogL()
	{
	TInt focusGroup;
	TName winName;
	TThreadId threadId;
	User::LeaveIfError(focusGroup=Client()->iWs.GetFocusWindowGroup());
	User::LeaveIfError(Client()->iWs.GetWindowGroupClientThreadId(focusGroup, threadId));
	User::LeaveIfError(Client()->iWs.GetWindowGroupNameFromIdentifier(focusGroup,winName));
	RThread thread;
	User::LeaveIfError(thread.Open(threadId));
	CShellErrorDialog *dialog=new(ELeave) CShellErrorDialog(Client()->iGroup, iGc);
	dialog->ConstructLD();
	dialog->SetTitle(_L("The Foreground App Is"));
	dialog->SetLine1(thread.FullName());
	thread.Close();
	dialog->SetLine2(winName);
	dialog->SetNumButtons(1);
	dialog->SetButtonText(0,_L("Okay"));
	if (dialog->Display()!=0)
		Panic(0);
	dialog=NULL;
	}

void CShellWindow::HeapCountDialogL()
	{
	CShellErrorDialog *dialog=new(ELeave) CShellErrorDialog(Client()->iGroup, iGc);
	dialog->ConstructLD();
	dialog->SetTitle(_L("Wserv Heap Count"));
	TBuf<0x20> line1;
	line1.Format(TRefByValue<const TDesC>(_L("Count=%d")),Client()->iWs.HeapCount());
	dialog->SetLine1(line1);
	dialog->SetNumButtons(1);
	dialog->SetButtonText(0,_L("Okay"));
	if (dialog->Display()!=0)
		Panic(0);
	dialog=NULL;
	}

void CShellWindow::WinKeyL(const TKeyEvent &aKey,const TTime& aTime)
	{
	TBool funcKeyPressed=aKey.iModifiers&EModifierFunc;
	if (aKey.iModifiers&EModifierCtrl)
		{
		switch(aKey.iCode)
			{
			case EKeyLeftArrow:
				{
				TInt col=Client()->iWs.GetBackgroundColor().Gray16();
				if (col>0)
					Client()->iWs.SetBackgroundColor(TRgb::Gray16(col-1));
				}
				break;
			case EKeyRightArrow:
				{
				TInt col=Client()->iWs.GetBackgroundColor().Gray16();
				if (col<15)
					Client()->iWs.SetBackgroundColor(TRgb::Gray16(col+1));
				}
				break;
			case EKeyTab:
				if (funcKeyPressed)
					ForegroundAppDialogL();
				else
					{
					Group()->GroupWin()->SetOrdinalPosition(0);
					((CShellClient *)Client())->CreateTestWindowL(new(ELeave) CTaskListWindow(), TPoint(50,30));
					Client()->ResetFocus();
					}
				break;
			case 1:		//Ctrl-A
			case 17:	//Ctrl-Q
				SetPos(TPoint(3,(Client()->iScreen->SizeInPixels().iHeight-Size().iHeight)/2));
				break;
			case 2:		//Ctrl-B
			case 23:	//Ctrl-W
				SetPos(TPoint((Client()->iScreen->SizeInPixels().iWidth-Size().iWidth)/2,(Client()->iScreen->SizeInPixels().iHeight-Size().iHeight)/2));
				break;
			case 3:		//Ctrl-C
			case 5:		//Ctrl-E
				if (funcKeyPressed && aKey.iModifiers&EModifierShift && aKey.iCode==3)			//Ctrl-Alt-Sh-C
					iClick.CommandReply(EClickCommandToggleOutput,TPtrC8(NULL,0));
				else
					SetPos(TPoint(Client()->iScreen->SizeInPixels().iWidth-Size().iWidth-3,(Client()->iScreen->SizeInPixels().iHeight-Size().iHeight)/2));
				break;
			case 18:	//Ctrl-R
				{
				CWsScreenDevice* screen=Client()->iScreen;
				TPixelsAndRotation sizeAndRotation;
				screen->GetScreenModeSizeAndRotation(0,sizeAndRotation);
				Client()->iWs.SetPointerCursorArea(0,TRect(sizeAndRotation.iPixelSize));
				screen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationNormal);
				screen->SetScreenMode(0);
				CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
				CleanupStack::PushL(rotations);
				TInt numModes=screen->NumScreenModes();
				TInt ii,err;
				for (ii=1;ii<numModes;++ii)
					{
					err=screen->GetRotationsList(ii,rotations);
					if (err==KErrNone)
						screen->SetCurrentRotations(ii,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,(*rotations)[0]));
					}
				CleanupStack::PopAndDestroy();
				}
				break;
			default:
				goto not_used;
			}
		}
	else if (funcKeyPressed)
		{
		switch(aKey.iCode)
			{
			case 'c':
				{
				CTCalibWindow *win=new(ELeave) CTCalibWindow();
				win->ConstructL(*Client()->iGroup);
				}
				break;
			case 's':	// Sleep
				User::After(2000000);
				break;
			case 'd':
				DisplayDialog(_L("Dialog test"),_L("Line of text"),_L(""));
				break;
			case 'o':
				UserHal::SwitchOff();
				break;
			case 'f':
				if (iFailModeEnabled)
					((CShellClient *)Client())->iWs.HeapSetFail(RHeap::ENone,0);
				else
					((CShellClient *)Client())->iWs.HeapSetFail(RHeap::ERandom,20);
				iFailModeEnabled=!iFailModeEnabled;
				break;
			case 'x':
			case 'X':
				if (aKey.iModifiers&EModifierShift)
					RShellWsSession::Shutdown();
				else
					((CShellClient *)Client())->Exit();
				break;
			case 'l':
				Client()->iWs.LogMessage(_L("Hello, this is log test"));
				break;
			case 'h':
				HeapCountDialogL();
				break;
			case 'b':
				BugHunt();
				break;
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

void CShellWindow::FocusChanged(TBool aState)
	{
	CListWindow::FocusChanged(aState);
	iIsFocused=aState;
	CheckTerminate();
	}

void CShellWindow::DoShellTests()
//
//Doing testing that can only be done from the shell
//
	{
	/*Group()->GroupWin()->EnableReceiptOfFocus(EFalse);
	BaseWin()->SetSizeErr(TSize(160,220));
	Group()->GroupWin()->EnableReceiptOfFocus(ETrue);
	*/
	SimulateSwitchOffEvent();
	}

/**
Sends the raw events to the current registered window session.
Since no Window Group is allowed to request the off events,
WServ shall handle these raw events.
*/
void CShellWindow::SimulateSwitchOffEvent()
	{
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::ESwitchOff);
	Client()->iWs.SimulateRawEvent(rawEvent);
	Client()->iWs.Flush();
	}	

#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP	
/**
@SYMTestCaseID 			GRAPHICS-WSERV-0404
@SYMTestCaseDesc 		Restart system event can be successfully sent through WServ
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		High
@SYMTestPurpose			Test restart system event in WServ
@SYMTestActions			The test code will simulate a 'restart event'.
@SYMTestExpectedResults	The system should restart (manually observed).
*/
void CShellWindow::SimulateRestartEvent()
	{
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::ERestartSystem);
	Client()->iWs.SimulateRawEvent(rawEvent);
	Client()->iWs.Flush();
	}

/**
@SYMTestCaseID 			GRAPHICS-WSERV-0405
@SYMTestCaseDesc 		Restart system event can be received by client that have registered for off event
@SYMPREQ				PREQ1089
@SYMREQ					REQ6883
@SYMTestStatus			Implemented
@SYMTestPriority		High
@SYMTestPurpose			Test restart system event in WServ with off event handler
@SYMTestActions			The test code will register for off event, simulate a 'restart event' and then receive the restart event
@SYMTestExpectedResults	Verify that the restart event is received and the testPass dialog displayed for this test (manually observed).
*/
void CShellWindow::SimulateAndReceiveRestartEvent()
	{
	const TInt KEventWaitTimer = 5000000; // 5 sec, The wait time that this test will wait for the restart event before declaring this test fail
	TInt err = Client()->iWs.RequestOffEvents(ETrue, Client()->iGroup->GroupWin());
	if (err != KErrNone)
		{
		static_cast<CShellClient *>(Client())->ErrorDialog(_L("Failed to request off event"),err);
		}

	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::ERestartSystem);
	Client()->iWs.SimulateRawEvent(rawEvent);

	TRequestStatus status;
	Client()->iWs.EventReady(&status);
	Client()->iWs.Flush();
	User::WaitForRequest(status);

	TWsEvent event;
	RTimer timer;
	timer.CreateLocal();
	TRequestStatus timerStatus;
	timer.After(timerStatus,KEventWaitTimer);
	TBool testPass=EFalse;
	while (timerStatus != KErrNone) // wait for the restart event until timer expire
		{
		Client()->iWs.GetEvent(event);
		if (event.Type() == EEventRestartSystem)
			{
			timer.Cancel();
			User::WaitForRequest(timerStatus); // to receive the cancel event

			testPass = ETrue;
			_LIT(KTestPass,"TestPassed");
			TWindowTitle winTitle(KTestPass);
			DisplayDialog(Client(), winTitle, _L("Received Restart Event"), KNullDesC);
			break;
			}

		// probably got pointer event above instead of restart event, so try getting the next event until we see retart event
		Client()->iWs.EventReady(&status);
		User::WaitForRequest(status, timerStatus);
		}

	if (testPass == EFalse)
		{
		// timer expired before getting restart event
		_LIT(KTestPass,"TestFailed");
		TWindowTitle winTitle(KTestPass);
		DisplayDialog(Client(), winTitle, _L("Did not receive Restart Event"), KNullDesC);
		}

	timer.Close();

	Client()->iWs.RequestOffEvents(EFalse);
	}

#endif		

void CShellWindow::BugHunt()
//
// Attempt to reproduce messaging issue while program exiting with message outstanding
//
	{
	RThread().SetPriority(EPriorityRealTime);
	((ROverrideProtectionInRSessionBase *)&(Client()->iWs))->Send(1);
	RThread().Kill(0);
	}

void CShellWindow::CheckTerminate()
	{
	if (iIsFocused && iAutoLaunch && iProcessList==NULL && iNumWindowGroups==Client()->iWs.NumWindowGroups())
		{
#if defined(__WINS__)
		delete this;
		RShellWsSession::Shutdown();
#else 
		// on hardware for automated test purposes cause board to crash to 
		// indicate the completion of test run
		User::SetCritical(User::ESystemCritical);
		User::Panic(_L("test"), KErrGeneral);
#endif
		}
	}

void CShellWindow::ProcessDied()
	{
	CheckTerminate();
	}

//
// End of CShellWindow class //
//

//
// CTaskListWindow class //
//

CTaskListWindow::CTaskListWindow() : CListWindow(), iWindowHandles(4), iWindowThreadNames(4)
	{
	}

void CTaskListWindow::ConstructL(CTWinBase &parent)
	{
	CListWindow::ConstructL(parent,ETrue);
	User::LeaveIfError(Client()->iWs.WindowGroupList(0,&iWindowHandles));
	for(TInt index=0;index<iWindowHandles.Count();index++)
		{
		TThreadId id;
		Client()->iWs.GetWindowGroupClientThreadId(iWindowHandles[index],id);
		RThread thread;
		User::LeaveIfError(thread.Open(id));
		iWindowThreadNames.AppendL(thread.FullName());
		thread.Close();
		}
	SetSize();
	}

void CTaskListWindow::SelectedL(TInt aIndex)
	{
	Client()->iWs.SetWindowGroupOrdinalPosition(iWindowHandles[aIndex], 0);
	CloseWindow();
	}

TPtrC CTaskListWindow::GetText(TInt aLine)
	{
	iLatestText=iWindowThreadNames[aLine];
	iLatestText.Append(_L("::"));
	TName winName;
	if (Client()->iWs.GetWindowGroupNameFromIdentifier(iWindowHandles[aLine],winName)==KErrNone)
		iLatestText.Append(winName);
	return(iLatestText);
	}

TInt CTaskListWindow::ListCount()
	{
	return(iWindowHandles.Count());
	}
//
// End of CTaskListWindow class //
//

CShellWindowGroup::CShellWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

CShellWindowGroup::~CShellWindowGroup()
	{
	iGroupWin.CancelCaptureKey(iCapHandle1);
	iGroupWin.CancelCaptureKey(iCapHandle2);
	iGroupWin.CancelCaptureKey(iCapHandle3);
	iGroupWin.CancelCaptureKey(iCapHandle4);
	}

void CShellWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iCapHandle1=User::LeaveIfError(iGroupWin.CaptureKey(EKeyTab, EModifierCtrl|EModifierShift|EModifierPureKeycode,EModifierCtrl|EModifierShift|EModifierPureKeycode));
	iGroupWin.DefaultOwningWindow();
	iGroupWin.EnableErrorMessages(EEventControlOnlyWithKeyboardFocus);
	iGroupWin.EnableScreenChangeEvents();
	}

void CShellWindowGroup::KeyL(const TKeyEvent &aKey,const TTime& aTime)
	{
	if (iCurWin)
		iCurWin->WinKeyL(aKey,aTime);
	}

void CShellWindowGroup::ErrorMessage(const TWsErrorMessage &aErrorMessage, const TTime &)
	{
	TBuf<0x40> errorText;
	switch(aErrorMessage.iErrorCategory)
		{
		case TWsErrorMessage::EDrawingRegion:
			{
			_LIT(Graphics,"Graphics %d");
			errorText.Format(Graphics,aErrorMessage.iError);
			}
			break;
		case TWsErrorMessage::EBackLight:
			{
			_LIT(Backlight,"Backlight %d");
			errorText.Format(Backlight,aErrorMessage.iError);
			}
			break;
		case TWsErrorMessage::ELogging:
			{
			_LIT(Logging,"Error Starting Logging: %d, Location: %d");
			// minus the error value is stored in the bottom 8 bits
			// an indication of position in the code, that the error occured in, is stored in the top 24 bits
			TInt line=(aErrorMessage.iError)&0xFFFFFF00;
			errorText.Format(Logging,line-aErrorMessage.iError,line>>8);
			}
			break;
		case TWsErrorMessage::EContrast:
			{
			_LIT(Contrast,"Contrast %d");
			errorText.Format(Contrast,aErrorMessage.iError);
			}
			break;
		default:
			{
			_LIT(Unknown,"Unknown %d");
			errorText.Format(Unknown,aErrorMessage.iError);
			}
			break;
		}
	_LIT(WservError,"Wserv error");
	TWindowTitle winTitle(WservError);
	DisplayDialog(Client(), winTitle, errorText, KNullDesC);
	}

void CShellWindowGroup::ScreenDeviceChangedL()
	{
	TPixelsTwipsAndRotation sizeAndRotation;
	Client()->iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	Client()->iScreen->SetScreenSizeAndRotation(sizeAndRotation);
	((CShellClient *)Client())->ScreenDeviceChangedL();
	}

//

CIconWindowGroup::CIconWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

void CIconWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableReceiptOfFocus(EFalse);
	iGroupWin.SetOrdinalPosition(0,100);
	}

//
// password window //
//

CTPasswordWindow::CTPasswordWindow()
	{
	}

void CTPasswordWindow::InitWin()
	{
	TInt wid=iFont->TextWidthInPixels(_L("Press Enter"));
	iTextPos.iX=(iWin.Size().iWidth-wid)/2;
	iTextPos.iY=(iWin.Size().iHeight)/2;
	}

void CTPasswordWindow::Draw()
	{
	CTTitledWindow::Draw();
	iGc->DrawText(_L("Press Enter"),iTextPos);
	}

//

CPasswordWindowGroup::CPasswordWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

CPasswordWindowGroup::~CPasswordWindowGroup()
	{
	delete iWin;
	}

void CPasswordWindowGroup::PasswordL(const TTime &)
	{
	__ASSERT_DEBUG(!iPasswordMode,Panic(EShellPanicPassword));
	iPasswordMode=ETrue;
	iWin->SetTitle(_L("Password Mode Enabled"));
	}

void CPasswordWindowGroup::Cancel()
	{
	iPasswordMode=EFalse;
	iWin->SetVisible(EFalse);
	iGroupWin.SetOrdinalPosition(0,-1000);
	iWin->SetTitle(_L("Why am I here?"));
	}

void CPasswordWindowGroup::KeyL(const TKeyEvent &aKey,const TTime&)
	{
	if (iPasswordMode && aKey.iCode==EKeyEnter)
		{
		Client()->iWs.PasswordEntered();
		Cancel();
		}
	}

void CPasswordWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.SetOrdinalPosition(-1);
	iWin=new(ELeave) CTPasswordWindow();
	iWin->ConstructL(*this);
	iWin->AssignGC(*Client()->iGc);
	Cancel();
	iWin->Win()->PasswordWindow(EPasswordAlwaysTriggerNow);
	iWin->Activate();
	SetCurrentWindow(iWin);
	}

//

CShellClient::CShellClient()
	{
	}

void CShellClient::CreateTestWindowL(CTWin *aWin, const TPoint &aPos,CTWinBase *parent)
	{
	TRAPD(err,aWin->ConstructL(*parent));
	if (err!=KErrNone)
		{
		delete aWin;
		User::Leave(err);
		}
	aWin->SetInitialPos(aPos);
	aWin->Activate();
	aWin->AssignGC(*iGc);
	}

void CShellClient::CreateTestWindowL(CTWin *aWin, const TPoint &aPos)
	{
	CreateTestWindowL(aWin, aPos, iGroup);
	}

CShellClient::~CShellClient()
	{
	delete iPointerCursor;
	delete iIconWin1;
	delete iIconWin2;
	delete iIconGroup;
	delete iPasswordGroup;
	iWs.FreeSystemPointerCursorList();
	iFs.Close();
	}


void CShellClient::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	CTClient::ConstructL();
	iWs.RequestOffEvents(EFalse);		//To test this function

	TBool keyClicksSupported=EFalse;
	HAL::Get(HALData::EKeyboardClick,keyClicksSupported);
	if (keyClicksSupported)
		{
		HAL::Set(HALData::EKeyboardClickState,1);
		TInt maxVol=1;
		HAL::Get(HALData::EKeyboardClickVolumeMax,maxVol);
		HAL::Set(HALData::EKeyboardClickVolume,maxVol);
		}	
	
	iWs.SetDoubleClick(TTimeIntervalMicroSeconds32(EDoubleClickTime), EDoubleClickDistance);

	User::LeaveIfError(iWs.SetHotKey(EHotKeyBacklightToggle, 2, EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl|EModifierShift));
	User::LeaveIfError(iWs.SetHotKey(EHotKeyBacklightOn, 14, EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl|EModifierShift));
	User::LeaveIfError(iWs.SetHotKey(EHotKeyBacklightOff, 13, EModifierFunc|EModifierCtrl|EModifierShift,EModifierFunc|EModifierCtrl|EModifierShift));
//
	iGroup=new(ELeave) CShellWindowGroup(this);
	iGroup->ConstructL();
	iWs.RequestOffEvents(ETrue,iGroup->GroupWin());	//To test this function
	iWs.RequestOffEvents(EFalse);
	iIconGroup=new(ELeave) CIconWindowGroup(this);
	iIconGroup->ConstructL();
	iIconWin1=new(ELeave) CTIconWindow(0);
	iIconWin1->ConstructL(*iIconGroup);
	iIconWin2=new(ELeave) CTIconWindow(1);
	iIconWin2->ConstructL(*iIconGroup);

	CShellWindow *win=new(ELeave) CShellWindow();
	CreateTestWindowL(win, TPoint(330,10));
	TWinCommand command;
	User::CommandLine(command);
	win->SetAutoLaunch(win->RunClientL(command,EFalse));

    iPointerCursor=new(ELeave) CTPointerCursor(iWs);
    TSpriteCreateParams params(TSize(24,48),TPoint(-12,-24),DrawPointerCursor,NULL, ETrue,CGraphicsContext::EDrawModePEN);
    iPointerCursor->ConstructL(1,&params,0);
	iGroup->GroupWin()->SetCustomPointerCursor(iPointerCursor->PointerCursor());
	ResetFocus();
	}

void CShellClient::ScreenDeviceChangedL()
	{
	iIconWin1->SetExtentL();
	iIconWin2->SetExtentL();
	}

void CShellClient::Exit()
	{
	CActiveScheduler::Stop();
	}

CRunningProcess::CRunningProcess(MProcessObserver* aObserver) : CActive(-5), iObserver(aObserver) 
	{}

CRunningProcess::~CRunningProcess()
	{
	Cancel();
	if (iNext)
		iNext->iPrevPtr=iPrevPtr;
	*iPrevPtr=iNext;
#if defined(__WINS__)
	iLib.Close();
#endif
#if defined(__USING_PROCESS_)
	iProcess.Close();
#else
	iProcess.Close();
#endif
	if (iObserver)
		iObserver->ProcessDied();
	}

void CRunningProcess::ConstructL(const TDesC &aName)
	{
#if defined(__USING_PROCESS_)
	TParse fileName;
	fileName.Set(_L("E:\\.EXE"),&aName,NULL);
	TInt error = iProcess.Create(fileName.FullName(),_L(""));
	if(error != KErrNone)
		{
		fileName.Set(_L(""),&aName,NULL);
		User::LeaveIfError(iProcess.Create(fileName.FullName(),_L("")));
		}
#else
	User::LeaveIfError(iFileName.Set(RP_FILENAME_SET,&aName,NULL));
	User::LeaveIfError(iLib.Load(iFileName.FullName()));
	DllTestAppEntryPoint libFunc=(DllTestAppEntryPoint)iLib.Lookup(1);
	User::LeaveIfNull((TAny *)libFunc);
	TBuf<KMaxFileName> threadName;
	TInt err;
	TInt num=0;
	do
		{
		threadName.Format(TRefByValue<const TDesC>(_L("%S%02d")),&aName,num++);
		err=iProcess.Create(threadName,libFunc,KDefaultStackSize,NULL,&iLib,NULL,KHeapSize,KHeapSize,EOwnerProcess);
		} while(err==KErrAlreadyExists);
	User::LeaveIfError(err);
#endif
	iProcess.Logon(iStatus);
	iProcess.Resume();
	SetActive();
	}

void CRunningProcess::ConstructTestExecuteL(const TDesC &aName)
	{
	//testexecute z:\wstest\wstest_t_autotest.script
	_LIT(KTestExecute,"testexecute");
	_LIT(KComandLinePrefix,"z:\\wstest\\wstest_t_");
	_LIT(KComandLinePostfix,"test.script");
#if defined(__USING_PROCESS_)
	TParse fileName;
	fileName.Set(KNullDesC,&KTestExecute,NULL);
	TBuf<128> commandLine;
	commandLine.Append(KComandLinePrefix);
	commandLine.Append(aName);
	commandLine.Append(KComandLinePostfix);
	User::LeaveIfError(iProcess.Create(fileName.FullName(),commandLine));
	iProcess.Logon(iStatus);
	iProcess.Resume();
	SetActive();
#endif
	}

void CRunningProcess::Link(CRunningProcess **aRpPtr)
	{
	iNext=*aRpPtr;
	if (iNext)
		iNext->iPrevPtr=&iNext;
	*aRpPtr=this;
	iPrevPtr=aRpPtr;
	}

void CRunningProcess::DoCancel()
	{
	iProcess.LogonCancel(iStatus);
	}

void CRunningProcess::RunL()
	{
	if (iStatus!=KErrNone)
		{
		TBuf<32> buf;
		buf.Num(iStatus.Int());
		DisplayDialog(_L("Program exited with error"),iProcess.ExitCategory(),buf);
		}
	delete this;
	}

GLDEF_C CTClient *CreateClientL()
	{
	return(new(ELeave) CShellClient());
	}

GLDEF_C TInt E32Main()
	{
	return(TestLibStartUp(CreateClientL));
	}
