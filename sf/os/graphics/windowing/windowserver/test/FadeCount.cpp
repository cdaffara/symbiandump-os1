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
// TFADECOUNT.CPP
// Testing counted or absolute fade
// 
//

#include <e32std.h>
#include "W32STD.H"
#include <e32svr.h>
#include "../test/tlib/testbase.h"

const TInt ETestFailed = 1;

class CFadeCountClient;

class CFadeCountWindow : public CTTitledWindow
	{
public:
	CFadeCountWindow(CFadeCountClient* aClient);
	~CFadeCountWindow();
	void ConstructL(CTWinBase &aParent);
	void Draw();
	void SetWindowSize();
	void WinKeyL(const TKeyEvent &aKey,const TTime& aTime);
public:
	CFadeCountClient* iClient;
	TBool iAbsoluteFading;
	TBool iPass;
	TBool iComplete;
	};


class CFadeCountClient : public CTClient
	{
public:
	CFadeCountClient();
	~CFadeCountClient();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void Exit();
	void TestL();
	void TestL(TInt aCondition);
	void CreateTestWindowsL(CTWinBase &aParent);
	TBool IsAbsoluteFadingOnL();
	void TestFadeL(CTWin* aWin, TBool aAbs, TBool aCounted);
private:
	TInt iNum;
	CFadeCountWindow *iFadeCountWin;
	CTWin* iWin1;
	CTWin* iWin2;
	TBool iAbsoluteFading;
	};


//
// Individual window sub-classes
//

CFadeCountWindow::CFadeCountWindow(CFadeCountClient* aClient) 
: CTTitledWindow(), iClient(aClient)
	{
	}

CFadeCountWindow::~CFadeCountWindow()
	{
	}

void CFadeCountWindow::ConstructL(CTWinBase &aParent)
	{
	CTTitledWindow::ConstructL(aParent);
	SetWindowSize();
	TSize screenSize=Client()->iScreen->SizeInPixels();
	TSize dif(screenSize-Size());
	iWin.SetPosition(TPoint(dif.iWidth/2,dif.iHeight/2));

	Invalidate();
	}

void CFadeCountWindow::Draw()
	{
	CTTitledWindow::Draw();
	TInt lineSpacing = iFont->AscentInPixels()+3;
	TPoint pos(3,iTitleHeight+lineSpacing);
	iGc->DrawText(_L("Fade count tests"),pos);

	pos.iY += lineSpacing;
	if (iAbsoluteFading)
		iGc->DrawText(_L("ABSOLUTEFADING is ON"),pos);
	else
		iGc->DrawText(_L("ABSOLUTEFADING is OFF"),pos);

	if (iComplete)
		{
		pos.iY += lineSpacing;
		if (iPass)
			iGc->DrawText(_L("Tests have passed"),pos);
		else
			iGc->DrawText(_L("Tests have failed"),pos);

		pos.iY += lineSpacing;
		iGc->DrawText(_L("Press any key to exit"),pos);
		}
	}

void CFadeCountWindow::SetWindowSize()
	{
	TSize size(400,200);
	SetSize(size);
	}

void CFadeCountWindow::WinKeyL(const TKeyEvent &/*aKey*/,const TTime& )
	{
	iClient->Exit();
	}


//
// End of CFadeCountWindow class //
//


CFadeCountClient::CFadeCountClient()
	{
	}

CFadeCountClient::~CFadeCountClient()
	{
	delete iWin2;
	delete iWin1;
	delete iFadeCountWin;
	}

void CFadeCountClient::ConstructL()
	{
	CTClient::ConstructL();

	iGroup=new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();

	CreateTestWindowsL(*iGroup);

	iFadeCountWin=new(ELeave) CFadeCountWindow(this);
	iFadeCountWin->iAbsoluteFading = iAbsoluteFading;
	iFadeCountWin->ConstructL(*iGroup);
	iFadeCountWin->Activate();
	iFadeCountWin->AssignGC(*iGc);
	iGroup->SetCurrentWindow(iGroup->Child());

	TRAP_IGNORE(TestL());
	iFadeCountWin->iComplete = ETrue;
	}

void CFadeCountClient::Exit()
	{
	CActiveScheduler::Stop();
	}

void CFadeCountClient::TestL()
	{
	// start condition - after absolute fade has been established
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, EFalse, EFalse);
	TestFadeL(iFadeCountWin, EFalse, EFalse);
	
	// System fade and repeated fade
	// positive fade
	TInt ii;
	const TInt KFadeRepeat = 4;
	for (ii=0; ii<KFadeRepeat; ii++)
		{
		iWs.SetSystemFaded(ETrue);
		TestFadeL(iWin1, ETrue, ETrue);
		TestFadeL(iWin2, ETrue, ETrue);
		TestFadeL(iFadeCountWin, ETrue, ETrue);
		}

	for (ii=KFadeRepeat-1; ii>=0; ii--)
		{
		iWs.SetSystemFaded(EFalse);
		TestFadeL(iWin1, EFalse, ii);
		TestFadeL(iWin2, EFalse, ii);
		TestFadeL(iFadeCountWin, EFalse, ii);
		}

	// negative fade is not counted
	for (ii=0; ii<KFadeRepeat-1; ii++)
		{
		iWs.SetSystemFaded(EFalse);
		TestFadeL(iWin1, EFalse, EFalse);
		TestFadeL(iWin2, EFalse, EFalse);
		TestFadeL(iFadeCountWin, EFalse, EFalse);
		}

	iWs.SetSystemFaded(ETrue);
	TestFadeL(iWin1, ETrue, ETrue);
	TestFadeL(iWin2, ETrue, ETrue);
	TestFadeL(iFadeCountWin, ETrue, ETrue);

	iWs.SetSystemFaded(EFalse);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, EFalse, EFalse);
	TestFadeL(iFadeCountWin, EFalse, EFalse);

	// child fading
	// nested
	iWin2->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, ETrue, ETrue);
	iWin1->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, ETrue, ETrue);
	TestFadeL(iWin2, ETrue, ETrue);
	iWin1->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, EFalse, ETrue);
	iWin2->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, EFalse, EFalse);
	
	// interlaced
	iWin2->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, ETrue, ETrue);
	iWin1->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, ETrue, ETrue);
	TestFadeL(iWin2, ETrue, ETrue);
	iWin2->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, ETrue, ETrue);
	TestFadeL(iWin2, EFalse, ETrue);
	iWin1->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	TestFadeL(iWin1, EFalse, EFalse);
	TestFadeL(iWin2, EFalse, EFalse);

	iFadeCountWin->iPass = ETrue;
	}

void CFadeCountClient::CreateTestWindowsL(CTWinBase &aParent)
	{
	iWin1 = new(ELeave) CTWin();
	iWin1->ConstructL(aParent);
	iWin1->SetSize(TSize(20,20));

	iWin2 = new(ELeave) CTWin();
	iWin2->ConstructL(*iWin1);
	iWin2->SetSize(TSize(10,10));

	iAbsoluteFading = IsAbsoluteFadingOnL();
	}

TBool CFadeCountClient::IsAbsoluteFadingOnL()
	{
	iWin1->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeWindowOnly);
	iWin1->WinTreeNode()->SetFaded(ETrue, RWindowTreeNode::EFadeWindowOnly);
	iWin1->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeWindowOnly);
	TBool absoluteFading = !iWin1->BaseWin()->IsFaded();
	iWin1->WinTreeNode()->SetFaded(EFalse, RWindowTreeNode::EFadeWindowOnly);
	return absoluteFading;
	}

void CFadeCountClient::TestL(TInt aCondition)
	{
	if (!aCondition)
		User::Leave(ETestFailed);
	}

void CFadeCountClient::TestFadeL(CTWin* aWin, TBool aAbs, TBool aCounted)
	{
	TestL(!aWin->BaseWin()->IsFaded() == !(iAbsoluteFading ? aAbs : aCounted));
	}


GLDEF_C CTClient *CreateClientL()
	{
	return(new(ELeave) CFadeCountClient());
	}
	
GLDEF_C TInt E32Main()
	{
	return(TestLibStartUp(CreateClientL));
	}
