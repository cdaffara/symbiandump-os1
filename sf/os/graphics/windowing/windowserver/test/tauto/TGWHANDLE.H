
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

#ifndef __TGWHANDLE_H__
#define __TGWHANDLE_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTGwHandle : public CTWsGraphicsBase
	{
public:
	CTGwHandle(CTestStep* aStep);
	~CTGwHandle();
	void ConstructL();
	TInt NumberOfWndGroupsWithZeroPriority(TInt aScreenNumber);
	void GetGwListL(CArrayFixFlat<TInt> *aWindowHandles);
	void GetGwListL(CArrayFixFlat<TInt> *aWindowHandles, TInt aScreenNumber);
	void GetGwListL(TInt aPriority, CArrayFixFlat<TInt> *aWindowHandles);
	void GetGwListL(RArray<RWsSession::TWindowGroupChainInfo>* aWindowHandles);
	void GetGwListL(TInt aPriority, RArray<RWsSession::TWindowGroupChainInfo>* aWindowHandles);
	void GwNamesL();
	void GwIdentifierTestL();
	void OwnerToForegroundL();
	void FindWindowGroupIdentifierTestL();
	void IdentifierWrapAroundTestL();
	void DefaultOwningWindowL();
	void DefaultOwningWindow2L();
	void WindowGroupChaining();
	void WindowGroupChaining2();
	void UnchainWindowGroupsL();
	void TestclosingclientL();
	void TestClearingAndSettingChildGroupsL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	enum {ENumGroups=10,ENumChained=5};
private:
	RWindowGroup* CreateWindowGroupLC(RWsSession& aWs,TUint32 aClientHandle,TInt aParentId=0);
	void CreateGroupWindowsLC(RWsSession& aWs);
	void CreateChainedGroupWindowsLC(RWsSession& aWs,TBool aSecondChain = EFalse);
	void TestAgainstLoopsWhenAddingChildGroupsL();
	void MoveGroups(RWindowGroup** aGroups,TInt aGp,TInt aNum,TInt aPos,TInt aInc=2);
	void TestGroups(TInt aPos);
	void TestGroupsBefore(TInt aPos);
	void TestOrdinalPositionL();
	void TestOrdinalPositionNoDebugL();
	void TestOrdinalPos(TInt awndPos);
	void TestOrdinalPosNoDebug(TInt awndPos);
	TInt ChainedWindowCount(RArray<RWsSession::TWindowGroupChainInfo>*);
private:
	TFullName iFullName;
	TSize iWinSize;
	RWindowGroup* iGroups[ENumGroups];
	RWindowGroup* iChained[ENumChained];
	RWindowGroup* iChained2[ENumChained];
	};

class CTGwHandleStep : public CTGraphicsStep
	{
public:
	CTGwHandleStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGwHandleStep,"TGwHandle");


#endif
