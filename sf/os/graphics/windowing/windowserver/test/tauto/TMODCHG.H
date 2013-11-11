
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TMODCHG_H__
#define __TMODCHG_H__

#include <e32std.h>
#include <e32svr.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"


class CTModifiersChanged;

class CMCWindow : public CTWin
	{
public:
	CMCWindow(CTModifiersChanged *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	void Draw();
	void DrawModSetLine(const TDesC &aDes,TInt aModifier,TInt iSettableModifiers);
protected:
	CTModifiersChanged *iTest;
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
	CTEventWindowGroup(CTClient *aClient, CTModifiersChanged *iTest);
	void ConstructL();
	void ModifiersChanged(const TModifiersChangedEvent &aModifiersChanged ,const TTime &aTime);
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
private:
	CTModifiersChanged *iTest;	
	};
	
class CTModifiersChanged : public CTWsGraphicsBase
	{
public:
	CTModifiersChanged(CTestStep* aStep);
	~CTModifiersChanged();
	TestState DoTestL();
	void ConstructL();
	void EndTest();
	TInt SubState() const;
	void IncSubState();
	void BadParams();
	void ModifierChangedEventsL();
	void SetModifiers();
	void CheckModifier(TEventModifier aModifier);
	void TestModifiersState(const TModifiersChangedEvent &aModifiersChanged);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	inline TestClient* Client() {return TheClient;}
	void SendEvents();
	void ResetModifiers();
	static TInt TimeOut(TAny* aTest);
	void TimeOut();
public:
	TBool iModSetTest;
	TInt iSettable;
private:
	enum {KTimeOutAfter=30000000};	// 30 seconds (test takes 8 sec on h4)
private:
	TSize iWinSize;	
	TInt iSubState;
	CTEventWindowGroup *iEventGroup;
	CMCWindow *iWin;
	TBool iSkipFirstModifierEvents;
	TInt iSkipCounter;
	CTimeOut* iTimeOut;
	TBool iSubSchedulerRunning;
	};

class CTModifiersChangedStep : public CTGraphicsStep
	{
public:
	CTModifiersChangedStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTModifiersChangedStep,"TModifiersChanged");


#endif
