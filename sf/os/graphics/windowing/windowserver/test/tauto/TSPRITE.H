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

#ifndef __TSPRITE_H__
#define __TSPRITE_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

#define TEST_NEW_BITMAP_NAME _L("Z:\\WSTEST\\mytest.mbm")

class CTTSprite : public CTWsGraphicsBase
	{
public:
	CTTSprite(CTestStep* aStep);
	~CTTSprite();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
public:
	void ConstructL();
	void GeneralTestsL();
	void GroupWindowSpritesL();
	void LotsSpritesL();
	void GeneralPointerCursor();
    void PointerCursorVisibleL();
	void SpriteUpdateMemberTestsL();
	void SpriteUpdateMemberNegTestsL();
	void FindWindowGroupThreadTestsL();
	void SpriteOnWindowOrphanedTestsL();
	void ResizeMemberL();
	void RotateNullSpriteL();
	void DoubleActivateL();
private:
	void SetUpMember(TSpriteMember &aMember);
	void SetUpPointerCursorL(RWsPointerCursor &aCursor, RWsSession &aSession);
	void SetUpSpriteL(RWsSprite &aSprite, RWsSession &aSession, RWindowTreeNode &aWindow,TInt aFlags=0);
	void SetUpSpriteL(RWsSprite &aSprite, RWindowTreeNode &aWindow,TPoint aPoint, TSpriteMember aMember, TInt aFlags=0);

private:
	RWsPointerCursor iCursor1;
	RWsPointerCursor iCursor2;
	CFbsBitmap iBitmap;
	};

class CTTSpriteStep : public CTGraphicsStep
	{
public:
	CTTSpriteStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTTSpriteStep,"TSprite");


#endif
