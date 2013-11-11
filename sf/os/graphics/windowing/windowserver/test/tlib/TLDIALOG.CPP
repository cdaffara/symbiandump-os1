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
#include <e32def_private.h>
#include "TLIB.H"

const TInt ButtonGap=20;
const TInt ButtonBorderGap=10;
const TInt ButtonBorderMargin=3;
const TInt TextMargin=5;
const TInt KAutoDelaySeconds=60; 

NONSHARABLE_CLASS(CTAutoKey): public CActive
	{
public:
	static CTAutoKey* NewL(RWsSession& aWs);
	~CTAutoKey();
	void Activate(TInt aDelay);
	
	void RunL();
	void DoCancel();
	
private:
	CTAutoKey(RWsSession& aWs);
	void ConstructL();	
	
	RTimer iTimer;
	TInt iDelay;
	RWsSession& iWs;
	};

EXPORT_C CTDialog::CTDialog() : CTTitledWindow(), iNumButtons(1)
	{
	iButton[0].Copy(_L("Continue"));	// Default button
	}

EXPORT_C CTDialog::~CTDialog()
	{
	if (iIsActive)
		CActiveScheduler::Stop();
	delete iAutoKey;
	}

EXPORT_C void CTDialog::ConstructLD(CTWinBase &aParent,CWindowGc &aGc)
	{
	iOldFocus=aParent.Group()->CurWin();
	TRAPD(err,CTTitledWindow::ConstructL(aParent));
	if (err!=KErrNone)
		{
		delete this;
		User::Leave(err);
		}
	iActivated=ETrue;
	iWin.SetBackgroundColor(TRgb::Gray256(238));	// Light gray
	AssignGC(aGc);
	iWin.SetPointerCapture(ETrue);
	Group()->SetCurrentWindow(this, ETrue);
	iAutoKey=CTAutoKey::NewL(Client()->iWs);
	}

EXPORT_C void CTDialog::RelinquishFocus()
	{
	Group()->SetCurrentWindow(iOldFocus);
	}

EXPORT_C TInt CTDialog::Display()
	{
	TInt result;

	SetWindowSize();
	if (iWinActive)
		BaseWin()->SetVisible(ETrue);
	else
		{
		Activate();
		iWinActive=ETrue;
		}
	if (iTakeFocus)
		Group()->SetCurrentWindow(this);
	Group()->GroupWin()->SetOrdinalPosition(0);
	iResultPtr=&result;
	CTClient *client=Client();
	if (client->QueueRead())
		{
		client->iWs.Flush();
		client=NULL;
		}
	iIsActive=ETrue;
	iAutoKey->Activate(KAutoDelaySeconds);
	CActiveScheduler::Start();
	if (client)
		client->CancelRead();
	return(result);
	}

EXPORT_C void CTDialog::SetFlags(TUint aFlags)
	{
	iFlags=aFlags;
	}

void CTDialog::SetMaxWid(TInt &aMax, TInt aWid)
	{
	if (aWid>aMax)
		aMax=aWid;
	}

EXPORT_C void CTDialog::SetWindowSize()
	{
	if (iActivated)
		{
		TInt max=0;
		SetMaxWid(max,iFont->TextWidthInPixels(iTitle));
		SetMaxWid(max,iFont->TextWidthInPixels(iLine1));
		SetMaxWid(max,iFont->TextWidthInPixels(iLine2));
		max+=TextMargin*2;
		iButWid=0;
		if (iNumButtons>0)
			{
			for(TInt index=0;index<iNumButtons;index++)
				SetMaxWid(iButWid,iFont->TextWidthInPixels(iButton[index]));
			iButWid+=ButtonBorderMargin*2;
			SetMaxWid(max,iButWid*iNumButtons+ButtonGap*(iNumButtons-1)+ButtonBorderGap*2);
			}
		TSize parSize=Parent()->Size();
		TSize size(max,iFont->HeightInPixels()*8);
		TPoint pos((parSize.iWidth-size.iWidth)/2,(parSize.iHeight-size.iHeight)/2);
		if (iFlags&EDialogDisplayAtBottom)
			pos.iY*=2;
		if (iFlags&EDialogDisplayAtLeft)
			pos.iX=0;
		SetExt(pos,size);
		Invalidate();
		}
	}

EXPORT_C void CTDialog::SetLine1(const TDesC &aLine1)
	{
	iLine1.Copy(aLine1);
	}

EXPORT_C void CTDialog::SetLine2(const TDesC &aLine2)
	{
	iLine2.Copy(aLine2);
	}

EXPORT_C void CTDialog::SetNumButtons(TInt aNum)
	{
	if ((TUint)aNum>3)
		TbPanic(EDialogButtonCount);
	iNumButtons=aNum;
	}

EXPORT_C void CTDialog::SetButtonText(TInt aNum,const TDesC &aButton)
	{
	if ((TUint)aNum>(TUint)iNumButtons)
		TbPanic(EDialogButtonIndex);
	iButton[aNum].Copy(aButton);
	}

TRect CTDialog::ButtonRect(TInt aIndex) const
	{
	TInt chunk=(iSize.iWidth-ButtonBorderMargin*2)/iNumButtons;
	TInt midPos=ButtonBorderMargin+chunk*aIndex+chunk/2;
	return(TRect(midPos-iButWid/2,iFont->HeightInPixels()*6,midPos+iButWid/2,iFont->HeightInPixels()*7+ButtonBorderMargin*2));
	}

EXPORT_C void CTDialog::Draw()
	{
	CTTitledWindow::Draw();
	iGc->SetPenColor(TRgb::Gray16(0));
	iGc->DrawText(iLine1, TPoint((iSize.iWidth-iFont->TextWidthInPixels(iLine1))/2,iFont->HeightInPixels()*3));
	iGc->DrawText(iLine2, TPoint((iSize.iWidth-iFont->TextWidthInPixels(iLine2))/2,iFont->HeightInPixels()*4+2));
	if (iNumButtons!=0)
		{
		for(TInt index=0;index<iNumButtons;index++)
			{
			TRect rect=ButtonRect(index);
			iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			iGc->SetBrushColor(TRgb::Gray256(255));
			iGc->DrawRect(rect);
			iGc->DrawRect(rect);
			iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
			TInt midPos=(rect.iTl.iX+rect.iBr.iX)/2;
			iGc->DrawText(iButton[index], TPoint((midPos-iFont->TextWidthInPixels(iButton[index])/2),
									iFont->HeightInPixels()*6+iFont->AscentInPixels()+ButtonBorderMargin));
			}
		}
	}

EXPORT_C void CTDialog::WinKeyL(const TKeyEvent &aKey,const TTime&)
	{
	switch(aKey.iCode)
		{
		case EKeyEscape:
		case '0':
			ButtonPressL(0);
			break;
		case EKeyEnter:
		case '1':
			ButtonPressL(iNumButtons>1 ? 1 : 0);	// Same as ESC on a single button dialog
			break;
		case ' ':
		case '2':
			ButtonPressL(2);
			break;
		}
	}

EXPORT_C void CTDialog::PointerL(const TPointerEvent &aPointer,const TTime &aTime)
	{
	if (aPointer.iType==TPointerEvent::EButton1Up)
		{
		if (iButtonClickOn>0)
			{
			ButtonPressL(iButtonClickOn-1);
			return;
			}
		}
	else if (aPointer.iType==TPointerEvent::EButton1Down)
		{
		for(TInt index=0;index<iNumButtons;index++)
			if (ButtonRect(index).Contains(aPointer.iPosition))
				{
				if (iFlags&EDialogWaitForButtonUp)
					iButtonClickOn=index+1;
				else
					{
					ButtonPressL(index);
					return;
					}
				}
		}
	CTTitledWindow::PointerL(aPointer, aTime);
	}

EXPORT_C void CTDialog::ButtonPressL(TInt aButton)
	{
	if (aButton<iNumButtons)
		{
		SetResult(aButton);
		CTTitledWindow::Delete(this);
		}
	}

void CTDialog::SetResult(TInt aButton)
	{
	if (iResultPtr)
		*iResultPtr=aButton;
	}

// Simple display dialog //

class CDisplayDialog : public CTDialog
	{		  
public:
	CDisplayDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc);
	void ConstructLD();
private:
	CTWindowGroup *iGroupWin;
	CWindowGc *iGc;
	};

CDisplayDialog::CDisplayDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc) : CTDialog(),
	iGroupWin(aGroupWin),
	iGc(aGc)
	{
	}

void CDisplayDialog::ConstructLD()
	{
	CTDialog::ConstructLD(*iGroupWin, *iGc);
	}

EXPORT_C void DisplayDialog(CTClient *aClient, const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2)
	{
	CDisplayDialog *dialog=NULL;
	dialog=new(ELeave) CDisplayDialog(aClient->iGroup, aClient->iGc);
	TRAPD(err,dialog->ConstructLD());
	if (err==KErrNone)
		{
		dialog->SetTitle(aTitle);
		dialog->SetLine1(aLine1);
		dialog->SetLine2(aLine2);
		dialog->SetNumButtons(1);
		dialog->SetButtonText(0,_L("Okay"));
		if (dialog->Display()!=0)		// delete dialog
			TbPanic(EDialogDisplay);
		}
	}

void doDisplayDialog(TInt aScreenNumber,const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, CTClient *&aClient, const RWindowGroup *aGroup)
	{
	aClient=new(ELeave) CTClient();
	aClient->SetScreenNumber(aScreenNumber);
	aClient->ConstructL();
//
	aClient->iGroup=new(ELeave) CTWindowGroup(aClient);
	aClient->iGroup->ConstructL();
	aClient->iGroup->GroupWin()->SetOrdinalPosition(0,10);
	if (aGroup)
		aClient->iGroup->GroupWin()->SetOwningWindowGroup(aGroup->Identifier());
//
	TRAP_IGNORE(DisplayDialog(aClient, aTitle, aLine1, aLine2));
	}

void doDisplayDialog(const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, CTClient *&aClient, const RWindowGroup *aGroup)
	{
	doDisplayDialog(KDefaultScreen,aTitle,aLine1,aLine2,aClient,aGroup);
	}

EXPORT_C void DisplayDialog(TInt aScreenNumber,const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, const RWindowGroup *aGroup)
	{
	CTClient *client=NULL;
	TRAP_IGNORE(doDisplayDialog(aScreenNumber,aTitle,aLine1,aLine2,client,aGroup));
	delete client;
	}

EXPORT_C void DisplayDialog(const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, const RWindowGroup *aGroup)
	{
	DisplayDialog(KDefaultScreen,aTitle,aLine1,aLine2,aGroup);
	}



//CInfoDialog

EXPORT_C CInfoDialog::CInfoDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc) :CTDialog(), iGroupWin(aGroupWin), iGc(aGc)
	{}

EXPORT_C void CInfoDialog::ButtonPressL(TInt aButton)
	{
	if (aButton==0)
		{
		SetResult(0);
		BaseWin()->SetVisible(EFalse);
		CActiveScheduler::Stop();
		}
	}

EXPORT_C void CInfoDialog::ConstructLD()
	{
	_LIT(OK,"Okay");
	CTDialog::ConstructLD(*iGroupWin, *iGc);
	SetNumButtons(1);
	SetButtonText(0,OK);
	SetTakeFocus();
	}

EXPORT_C void CInfoDialog::TimerResults()
	{
	TProfile profile[eTimes];
	// TProfile only has default constructor -
	// constructor of TProfile does not initialize its members
	for (TInt jj=0; jj<eTimes; jj++)
		{
		profile[jj].iTime=0;
		profile[jj].iCount=0;
		}
	__PROFILE_DISPLAY(eTimes)
	TBuf<64> times;
	TBuf<32> counts;
	TInt ii=1;
	FOREVER
		{
		AppendProfileTime(times,profile[ii].iTime);
		AppendProfileCount(counts,profile[ii].iCount);
		if (++ii==eTimes)
			break;
		AddComma(times);
		AddComma(counts);
		}
	SetLine1(times);
	SetLine2(counts);
	times.Zero();
	AppendProfileTime(times,profile[0].iTime);
	SetTitle(times);
	}

void CInfoDialog::AppendProfileTime(TDes &aDes, TInt aNum)
	{
	_LIT(ThreeDP,"%d.%03d");
	aDes.AppendFormat(ThreeDP,aNum/eSeconds,(aNum%eSeconds)/1000);
	}

void CInfoDialog::AppendProfileCount(TDes &aDes, TInt aNum)
	{
	_LIT(Int,"%d");
	aDes.AppendFormat(Int,aNum);
	}

void CInfoDialog::AddComma(TDes &aDes)
	{
	_LIT(Comma,", ");
	aDes.Append(Comma);
	}

CTAutoKey::CTAutoKey(RWsSession& aWs): CActive(0), iWs(aWs)
	{
	CActiveScheduler::Add(this);
	}
	
CTAutoKey::~CTAutoKey()
	{
	Cancel();
	iTimer.Close();
	}
	
void CTAutoKey::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	}
	
CTAutoKey* CTAutoKey::NewL(RWsSession& aWs)
	{
	CTAutoKey* self=new(ELeave) CTAutoKey(aWs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CTAutoKey::Activate(TInt aDelay)
	{
	if (IsActive())
		Cancel();
	
	iDelay=aDelay*1000000;
	iTimer.After(iStatus,iDelay);
	SetActive();
	}
void CTAutoKey::RunL()
	{
	// simulate key event, only needed to run once
	TKeyEvent keyEvent;
	keyEvent.iCode=EKeyEnter;
	keyEvent.iScanCode=EKeyEnter;
	keyEvent.iModifiers=0;
	keyEvent.iRepeats=0;
	iWs.SimulateKeyEvent(keyEvent);
	iWs.Flush();
	}
	
void CTAutoKey::DoCancel()
	{
	iTimer.Cancel();
	}
