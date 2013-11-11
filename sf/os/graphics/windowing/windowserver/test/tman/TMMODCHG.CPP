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
// Test modifier changed message
// 
//

#include <e32std.h>
#include <e32svr.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class CTModifiersChangedTest;

class CMCWindow : public CTWin
	{
public:
	CMCWindow(CTModifiersChangedTest *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void DrawModSetLine(const TDesC &aDes,TInt aModifier,TInt iSettableModifiers);
protected:
	CTModifiersChangedTest *iTest;
	TRgb iBack;
	TInt iLineHeight;
	TInt iFontAscent;
	TInt iYpos;
	TInt iXpos1;
	TInt iXpos2;
	};

class CTEventWindowGroup : public CTWindowGroup
	{
public:
	CTEventWindowGroup(CTClient *aClient, CTModifiersChangedTest *iTest);
	void ConstructL();
	void ModifiersChanged(const TModifiersChangedEvent &aModifiersChanged ,const TTime &aTime);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	CTModifiersChangedTest *iTest;
	};

class CTModifiersChangedTest : public CTestBase
	{
public:
	CTModifiersChangedTest();
	~CTModifiersChangedTest();
	TestState DoTestL();
	void ConstructL();
	void EndTest();
	TInt SubState() const;
	void IncSubState();
	void BadParams();
	void ModifierChangedEventsL();
	void SetModifiersL();
	void CheckModifierL(TEventModifier aModifier);
public:
	TBool iModSetTest;
	TInt iSettable;
private:
	TSize iWinSize;
	TInt iState;
	TInt iSubState;
	CTEventWindowGroup *iEventGroup;
	CMCWindow *iWin;
	};

struct TModifierChangedTestsParams
	{
	TText *txt;			// Text message telling user what to do
	TUint changed;
	TUint state;
	TUint stateMask;
	};

LOCAL_D TModifierChangedTestsParams ModifierChangedTests[]={
#if defined(__WINS__)
	{(TText *)_S("Press Caps"),EModifierCapsLock,EModifierCapsLock,EModifierCapsLock},
	{(TText *)_S("Press Caps (again)"),EModifierCapsLock,0,EModifierCapsLock},
#endif
	{(TText *)_S("Press left shift"),EModifierShift|EModifierLeftShift,EModifierShift|EModifierLeftShift,EModifierShift|EModifierLeftShift},
	{(TText *)_S("Release left shift"),EModifierShift|EModifierLeftShift,0,EModifierShift|EModifierLeftShift},
	{(TText *)_S("Press right shift"),EModifierShift|EModifierRightShift,EModifierShift|EModifierRightShift,EModifierShift|EModifierRightShift},
	{(TText *)_S("Release right shift"),EModifierShift|EModifierRightShift,0,EModifierShift|EModifierRightShift},
	{(TText *)_S("Press (left) func"),EModifierFunc|EModifierLeftFunc,EModifierFunc|EModifierLeftFunc,EModifierFunc|EModifierLeftFunc},
	{(TText *)_S("Release (left) func"),EModifierFunc|EModifierLeftFunc,0,EModifierFunc|EModifierLeftFunc},
	{(TText *)_S("Press (left) control"),EModifierCtrl|EModifierLeftCtrl,EModifierCtrl|EModifierLeftCtrl,EModifierCtrl|EModifierLeftCtrl},
	{(TText *)_S("Release (left) control"),EModifierCtrl|EModifierLeftCtrl,0,EModifierCtrl|EModifierLeftCtrl},
	};

const TInt numTests=sizeof(ModifierChangedTests)/sizeof(ModifierChangedTests[0]);

GLDEF_C CTestBase *CreateModifiersChangedTest()
	{
	return(new(ELeave) CTModifiersChangedTest());
	}

//
// CTEventWindowGroup class //
//

CTEventWindowGroup::CTEventWindowGroup(CTClient *aClient, CTModifiersChangedTest *aTest) : CTWindowGroup(aClient), iTest(aTest)
	{}

void CTEventWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableModifierChangedEvents(EModifierShift|EModifierLeftShift|EModifierRightShift|EModifierCapsLock|
							EModifierFunc|EModifierLeftFunc|EModifierCtrl|EModifierLeftCtrl,EEventControlAlways);
	}

void CTEventWindowGroup::ModifiersChanged(const TModifiersChangedEvent &aModifiersChanged ,const TTime &)
	{
	TInt ss=iTest->SubState();
	TInt getMods=Client()->iWs.GetModifierState();	// Double check the mods with get mods
	if (aModifiersChanged.iChangedModifiers==ModifierChangedTests[ss].changed && 
		(aModifiersChanged.iModifiers&ModifierChangedTests[ss].stateMask)==ModifierChangedTests[ss].state &&
		(getMods&ModifierChangedTests[ss].stateMask)==ModifierChangedTests[ss].state)
		iTest->IncSubState();
	}

void CTEventWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode==EKeyEscape || (aKey.iCode==' ' && iTest->iModSetTest))
		iTest->EndTest();
	}

//
// CMCWindow, class //
//

CMCWindow::CMCWindow(CTModifiersChangedTest *aTest) : CTWin(), iTest(aTest)
	{
	iBack=TRgb::Gray256(230);
	}

void CMCWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	ConstructExtLD(*parent,pos,size);
	iWin.SetBackgroundColor(iBack);
	Activate();
	AssignGC(aGc);
	iLineHeight=iFont->HeightInPixels()*5/4;
	iFontAscent=iFont->AscentInPixels();
	iXpos1=4;
	iXpos2=iXpos1+12*iFont->TextWidthInPixels(_L("M"));
	}

void CMCWindow::DrawModSetLine(const TDesC &aDes,TInt aModifier,TInt iSettableModifiers)
	{
	iYpos+=iLineHeight;
	iGc->DrawText(aDes, TPoint(iXpos1,iYpos));
	iGc->DrawText(aModifier&iSettableModifiers ? _L("Yes"):_L("No"), TPoint(iXpos2,iYpos));
	}

void CMCWindow::Draw()
	{
	iGc->Clear();
	if (iTest->iModSetTest)
		{
		iYpos=iFontAscent+3;
		TBuf<32> buf;
		buf.Format(TRefByValue<const TDesC>(_L("Settable? [0x%4x]")), iTest->iSettable);
		iGc->DrawText(_L("Modifier"), TPoint(iXpos1,iYpos));
		iGc->DrawText(buf, TPoint(iXpos2,iYpos));
		DrawModSetLine(_L("Shift"),EModifierShift,iTest->iSettable);
		DrawModSetLine(_L("Left Shift"),EModifierLeftShift,iTest->iSettable);
		DrawModSetLine(_L("Func"),EModifierFunc,iTest->iSettable);
		DrawModSetLine(_L("Caps"),EModifierCapsLock,iTest->iSettable);
		DrawModSetLine(_L("NumLock"),EModifierNumLock,iTest->iSettable);
		DrawModSetLine(_L("Double Click"),EModifierDoubleClick,iTest->iSettable);
		}
	else
		iGc->DrawText(TPtrC(ModifierChangedTests[iTest->SubState()].txt), TPoint(10,20));
	}

//

CTModifiersChangedTest::CTModifiersChangedTest() : CTestBase(_L("Capture Key"))
	{}

CTModifiersChangedTest::~CTModifiersChangedTest()
	{
	delete iWin;
	delete iEventGroup;
	}

void CTModifiersChangedTest::EndTest()
	{
	if (iModSetTest)
		{
		iModSetTest=EFalse;
		iWin->Invalidate();
		}
	Request();
	}

void CTModifiersChangedTest::ConstructL()
	{
	iWin=new(ELeave) CMCWindow(this);
	iWin->SetUpL(TPoint(10,10),TSize(240,200),Client()->iGroup, *Client()->iGc);
	}

TInt CTModifiersChangedTest::SubState() const
	{
	return(iSubState);
	}

void CTModifiersChangedTest::IncSubState()
	{
	if (iSubState==(numTests-1))
		EndTest();
	else
		{
		iSubState++;
		iWin->Invalidate();
		}
	}

void CTModifiersChangedTest::BadParams()
	{
	}

void CTModifiersChangedTest::ModifierChangedEventsL()
	{
	iEventGroup=new(ELeave) CTEventWindowGroup(Client(), this);
	iEventGroup->ConstructL();
	}

void CTModifiersChangedTest::CheckModifierL(TEventModifier aModifier)
	{
	TInt oldMods=Client()->iWs.GetModifierState();
	Client()->iWs.SetModifierState(aModifier, EToggleModifier);
	TInt getMods=Client()->iWs.GetModifierState();
	if (oldMods!=getMods)
		{
		iSettable|=aModifier;
		Client()->iWs.SetModifierState(aModifier, ETurnOffModifier);
		getMods=Client()->iWs.GetModifierState();
		TestL(!(getMods&aModifier));
		Client()->iWs.SetModifierState(aModifier, ETurnOnModifier);
		getMods=Client()->iWs.GetModifierState();
		TestL(getMods&aModifier);
		Client()->iWs.SetModifierState(aModifier, ETurnOffModifier);
		getMods=Client()->iWs.GetModifierState();
		TestL(!(getMods&aModifier));
		if (oldMods&aModifier)
			Client()->iWs.SetModifierState(aModifier, ETurnOnModifier);
		}
	else
		{
		Client()->iWs.SetModifierState(aModifier, ETurnOffModifier);
		TestL(oldMods==Client()->iWs.GetModifierState());
		Client()->iWs.SetModifierState(aModifier, ETurnOnModifier);
		TestL(oldMods==Client()->iWs.GetModifierState());
		}
	}

void CTModifiersChangedTest::SetModifiersL()
	{
	iModSetTest=ETrue;
	for(TInt mod=1;mod!=0;mod<<=1)
		CheckModifierL((TEventModifier)mod);
	iWin->Invalidate();
	Client()->iWs.Flush();
	}

TestState CTModifiersChangedTest::DoTestL()
	{
	switch(iState)
		{
		case 0:		//The first 2 tests have been converted to TAuto test code.
			ModifierChangedEventsL();
		/*	iState++;
			return(EContinue);
		case 1:
			LogSubTest(_L("Errors"),1);
			BadParams();
			break;
		case 2:*/
			LogSubTest(_L("Set modifiers"),1);
			SetModifiersL();
			iState++;
			return(EContinue);
		default:
			return(EFinished);
		}
	//iState++;
	//return(ENext);
 	}
