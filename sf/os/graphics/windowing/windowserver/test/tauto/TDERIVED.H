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

#ifndef __TDERIVED_H__
#define __TDERIVED_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CDerivedWindowGc : public CWindowGc
	{
public:
	CDerivedWindowGc(CWsScreenDevice *aDevice);
	~CDerivedWindowGc();
	void DrawLine(const TPoint &aPoint1,const TPoint &aPoint2);
	};

class RDerivedSession : public RWsSession
	{
public:
	TInt ResourceCount();
	void EnableAutoFlush();
	};

class RDerivedBlankWindow : public RBlankWindow
	{
public:
	RDerivedBlankWindow();
	RDerivedBlankWindow(RWsSession &aWs);
	void SetColor(TRgb aColor);
	void SetColorGray();
	};

class RDerivedWindow : public RWindow
	{
public:
	RDerivedWindow();
	RDerivedWindow(RWsSession &aWs);
	void SetBackgroundColor(TRgb aColor);
	void SetBackgroundColorGray();
	};

class RDerivedBackedUpWindow : public RBackedUpWindow
	{
public:
	RDerivedBackedUpWindow();
	RDerivedBackedUpWindow(RWsSession &aWs);
	TInt BitmapHandle();
	};

class RDerivedWindowGroup : public RWindowGroup
	{
public:
	RDerivedWindowGroup();
	RDerivedWindowGroup(RWsSession &aWs);
	TName Name() const;
	};

class CDerivedScreenDevice : public CWsScreenDevice
	{
public:
	CDerivedScreenDevice(RWsSession &aWs);
	CDerivedScreenDevice();
	TSize SizeInPixels() const;
	};

class RDerivedSprite : public RWsSprite
	{
public:
	RDerivedSprite();
	RDerivedSprite(RWsSession &aWs);
	};

class RDerivedPointerCursor : public RWsPointerCursor
	{
public:
	RDerivedPointerCursor();
	RDerivedPointerCursor(RWsSession &aWs);
	};

class CTDerived : public CTWsGraphicsBase
	{
public:
	CTDerived(CTestStep* aStep);
	void ConstructL();
	void DerivedWindowGcL();
	void DerivedSession();
	void DerivedWindows();
	void DerivedScreenDeviceL();
	void DerivedSprite();
	~CTDerived();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TSize iWinSize;
	RDerivedSession iDSession;
	RDerivedWindow iDRedraw;
	RDerivedBlankWindow iDBlank;
	RDerivedBackedUpWindow iDBackedUp;
	RDerivedWindowGroup iDGroup;
	RDerivedSprite iDSprite;
	RDerivedPointerCursor iDPCursor;
	};

class CTDerivedStep : public CTGraphicsStep
	{
public:
	CTDerivedStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTDerivedStep,"TDerived");


#endif
