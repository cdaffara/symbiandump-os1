
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

#ifndef __TMULTCON_H__
#define __TMULTCON_H__

#include <e32std.h>
#include <e32svr.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTMultiCon;
class CMcWindowBase;

class CMcConnectionBase : public CTClient
	{
public:
	CMcConnectionBase(CTMultiCon *aTest);
	~CMcConnectionBase();
	virtual void ConstructL();
	void SubStateChanged();
protected:
	CTMultiCon *iTest;
	CMcWindowBase *iWin;
	CWindowGc *iGc;
	};

class CMcConnection : public CMcConnectionBase	// Sets AutoForeground off
	{
public:
	CMcConnection(CTMultiCon *aTest);
	void ConstructL();
	};

class CMcWindowGroupAf : public CTWindowGroup
	{
public:
	CMcWindowGroupAf(CTClient *aClient);
	void KeyL(const TKeyEvent &aKey, const TTime &aTime);
	};

class CMcConnectionAf : public CMcConnectionBase	// Sets AutoForeground on
	{
public:
	CMcConnectionAf(CTMultiCon *aTest);
	void ConstructL();
	void KeyL(const TKeyEvent &aKey);
	};

class CMcConnectionDef : public CMcConnectionBase	// Leaves AutoForeground as the default value
	{
public:
	CMcConnectionDef(CTMultiCon *aTest);
	void ConstructL();
	};

class CMcWindowBase : public CTWin
	{
public:
	CMcWindowBase(CTMultiCon *aTest);
	void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	virtual void Draw()=0;
	virtual void PointerL(const TPointerEvent &pointer,const TTime &)=0;
protected:
	CTMultiCon *iTest;
	TRgb iBack;
	};

class CMcWindow : public CMcWindowBase
	{
public:
	CMcWindow(CTMultiCon *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	};

class CMcWindowAf : public CMcWindowBase
	{
public:
	CMcWindowAf(CTMultiCon *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	void ConstructL();
	};

class CMcWindowDef : public CMcWindowBase
	{
public:
	CMcWindowDef(CTMultiCon *aTest);
	virtual void Draw();
	virtual void PointerL(const TPointerEvent &pointer,const TTime &);
	};

class CTMultiCon : public CTWsGraphicsBase
	{
public:
	enum {KTimeOutAfter=10000000};	//10secs
public:
	CTMultiCon(CTestStep* aStep);
	~CTMultiCon();
	void ConstructL();
	void EndAutoForegroundTest();
	TInt SubState() const;
	void IncSubState();
	static TInt TimeOut(TAny* aTest);
	void TimeOut();
	inline TInt ScreenNumber() const {return(iTest->ScreenNumber());}
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void SendEvents();
private:
	CTimeOut* iTimeOut;
	TInt iTimeOutCount;
	CMcConnectionAf *iConn1;
	CMcConnection *iConn2;
	CMcConnectionDef *iConn3;
	TSize iScreenSize;
	//TInt iState;
	TInt iSubState;
	};

class CTMultiConStep : public CTGraphicsStep
	{
public:
	CTMultiConStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMultiConStep,"TMultiCon");


#endif
