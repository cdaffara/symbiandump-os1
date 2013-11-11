
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

#ifndef __TORDINAL_H__
#define __TORDINAL_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTOrdinal;

class COrdinalClient : public CTClient
	{
public:
	COrdinalClient();
	virtual void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	virtual void ConstructL();
	~COrdinalClient();
	};

class COrdinalWindowBase : public CBase
	{
public:
	COrdinalWindowBase(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep);
	~COrdinalWindowBase();
	TInt OrdinalPosition();
	void SetOrdinalPosition(TInt aPos);
	void SetOrdinalPosition(TInt aPos,TInt aPri);
	virtual void Draw();
	void TestOP(TInt aTestPos);
	TInt SetToLastAndGetOP();
	TInt SetToLastAndGetOPPri(TInt aPri);
	void SetAndTestOP(TInt aPos);
	void SetAndTestOP(TInt aPos,TInt aTestPos);
	void SetAndTestOPPri(TInt aPos,TInt aPri,TInt aTestPos);
	void SetAndTestOPPri(TInt aPos,TInt aPri);
	inline COrdinalWindowBase* NextSibling() const;
	inline COrdinalWindowBase* PrevSibling() const;
	inline RWindowTreeNode* WinTreeNode();
	inline TUint32 Handle();
protected:
	RWindowTreeNode* iWin;
private:
	CTClient* iClient;
	CTestBase* iTest;
	CTestStep* iTestStep;
	};

class COrdinalWindow : public COrdinalWindowBase
	{
public:
	COrdinalWindow(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep);
	static COrdinalWindowBase *NewL(CTClient *aClient, RWindowTreeNode *parent, CTestBase* aTest, CTestStep* aTestStep);
private:
	RWindow iClientWin;
	};

class COrdinalWindowGroup : public COrdinalWindowBase
	{
public:
	COrdinalWindowGroup(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep);
	static COrdinalWindowBase *NewL(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep);
private:
	RWindowGroup iGroupWin;
	};

class CTOrdinal : public CTWsGraphicsBase
	{
public:
	CTOrdinal(CTestStep* aStep);
	~CTOrdinal();
	void DestroyWindows();
	void OrdinalPos();
	void OrdinalPriority();
	void ConstructL();
	void CreateWindowsL(TInt aMode);
protected: //virtual function from CTGraphicsStep
	void RunTestCaseL(TInt aCurTestCase);
private:
	void TestWindowOrderNext(TInt aBefore,TInt aAfter);
	void TestWindowOrderNext(TInt aLast);
	void TestWindowOrderPrev(TInt aAfter,TInt aBefore);
	void TestWindowOrderPrev(TInt aFirst);
	void CheckHandlesOnNewWindow(CTWindowGroup* aParent,COrdinalWindowBase* aWin);
	void CheckHandlesOnNewWindow(COrdinalWindowBase* aParent,COrdinalWindowBase* aWin);
	void CheckHandlesOnNewWindow(TInt aErrors,COrdinalWindowBase* aWin);
	void CheckHandles(TUint aParent);
private:
	enum {ENumChildren=6};
private:
	COrdinalClient* iClient;
	TSize iWinSize;
	TBool iGroupTest;
	COrdinalWindowBase* iParent;
	COrdinalWindowBase* iParent2;
	COrdinalWindowBase* iParent3;
	COrdinalWindowBase* iChild[ENumChildren];
	};

class CTOrdinalStep : public CTGraphicsStep
	{
public:
	CTOrdinalStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTOrdinalStep,"TOrdinal");


#endif
