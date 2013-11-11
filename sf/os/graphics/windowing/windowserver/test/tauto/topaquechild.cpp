// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file is part of manual test (group\inc113743.mmp) to check Wserv behaviour when
// exposing a transparent window that is obscured by some opaque child window above it.
// Without the fix, that transparent window will be drawn as opaque window
// because its transparent regions is null (not re-created due to its update flag is not
// set as it is not traceable via transparent regions linked list of windows above it.
// Some illustration when the situation will trigger the defect (side view),
// ----       Child (opaque)
// ---------- Parent (transparent)
// ----       Other transparent window obscured by opaque child
// This defect is only reproducable when window shadowing is not used, so it cannot be 
// tested from standard Wserv test framework.
// @SYMTestCaseID		GRAPHICS-WSERV-0450
// @SYMDEF  			INC113743
// @SYMTestCaseDesc    Perform test on Wserv behaviour in exposing transparent windows which is obscured indirectly by opaque child window
// @SYMTestPriority    High
// @SYMTestStatus      Implemented
// @SYMTestActions     Create opaque and transparent windows in the following dimension and z-order
// ----       opaque child
// ---------- transparent parent
// ----       other transparent underneath obscured and non traceable via transparent parent iTransparentRegions
// and then make transparent parent window invisible
// @SYMTestExpectedResults The expected result are: other transparent window underneath is exposed and redrawn properly not becoming opaque
// 
//

#include <e32std.h>
#include <w32std.h>
#include <fbs.h>

#define ENullWsHandle 0xffffffff

void MainL()
	{
	const TSize KTestSize(320,240);
	const TSize KHalfSize(160,240);
	const TInt KScreenNo = 1;

	RRegion shape;
	shape.AddRect(TRect(0,0,160,120));
	shape.AddRect(TRect(160,120,320,240));
	CleanupClosePushL(shape);

	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);

	CWsScreenDevice* scr = new(ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(scr);
	User::LeaveIfError(scr->Construct(KScreenNo));
	TDisplayMode displayMode = scr->DisplayMode();

	CWindowGc* gc = NULL;
	User::LeaveIfError(scr->CreateContext(gc));
	CleanupStack::PushL(gc);

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(0x0badface, ETrue));
	CleanupClosePushL(group);

	// create transparent window underneath that will be obscured by some opaque child window above it
	RWindow transWinUnderneath(ws);
	User::LeaveIfError(transWinUnderneath.Construct(group, ENullWsHandle));
	CleanupClosePushL(transWinUnderneath);
	transWinUnderneath.SetShadowDisabled(ETrue);
	transWinUnderneath.SetExtent(TPoint(0,0),KTestSize/*KHalfSize*/);
	transWinUnderneath.SetTransparencyAlphaChannel();
	transWinUnderneath.SetRequiredDisplayMode(EColor64K);
	transWinUnderneath.SetBackgroundColor(TRgb(255,0,0,128));
	transWinUnderneath.SetShape(shape);
	transWinUnderneath.Activate();
	transWinUnderneath.Invalidate();
	transWinUnderneath.BeginRedraw();
	transWinUnderneath.EndRedraw();
	ws.Flush();

	User::After(1*1000*1000);

	// save the reference image (semi-transparent red window)
	CFbsBitmap* refImage = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(refImage);
	User::LeaveIfError(refImage->Create(KTestSize, displayMode));
	scr->CopyScreenToBitmap(refImage);

	// create transparent window parent which will have opaque child
	RWindow transWinParent(ws);
	User::LeaveIfError(transWinParent.Construct(group, ENullWsHandle));
	CleanupClosePushL(transWinParent);
	transWinParent.SetShadowDisabled(ETrue);
	transWinParent.SetExtent(TPoint(0,0),KTestSize);
	transWinParent.SetTransparencyAlphaChannel();
	transWinParent.SetRequiredDisplayMode(EColor64K);
	transWinParent.SetBackgroundColor(TRgb(255,255,255,0));
	transWinParent.Activate();

	transWinParent.Invalidate();
	transWinParent.BeginRedraw();
	transWinParent.EndRedraw();
	ws.Flush();
		
	// create opaque child window that obscure transparent window underneath (not its parent)
	RWindow opaqueWinChild(ws);
	User::LeaveIfError(opaqueWinChild.Construct(transWinParent, ENullWsHandle));
	CleanupClosePushL(opaqueWinChild);
	opaqueWinChild.SetExtent(TPoint(0,0),KTestSize/*KHalfSize*/);
	opaqueWinChild.SetRequiredDisplayMode(EColor64K);
	opaqueWinChild.SetShape(shape);
	opaqueWinChild.Activate();
	
	opaqueWinChild.Invalidate();
	opaqueWinChild.BeginRedraw();
	gc->Activate(opaqueWinChild);
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetPenStyle(CGraphicsContext::ENullPen);
	gc->SetBrushColor(KRgbGreen);
	gc->Clear();
	gc->SetBrushColor(KRgbBlue);
	gc->DrawEllipse(KTestSize/*KHalfSize*/);
	gc->Deactivate();
	opaqueWinChild.EndRedraw();
	ws.Flush();

	User::After(1*1000*1000);

	// preform the test by making transparent window parent invisible
	transWinParent.SetVisible(EFalse);
	ws.Flush();

	User::After(2*1000*1000);

	// save the test image (without the fix this would be an opaque red window)
	CFbsBitmap* testImage = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(testImage);
	User::LeaveIfError(testImage->Create(KTestSize, displayMode));
	scr->CopyScreenToBitmap(testImage);

	_LIT(KPanicMsg, "Test Failed");
	TInt nbytes = CFbsBitmap::ScanLineLength(KTestSize.iWidth, displayMode)*KTestSize.iHeight;
	if (Mem::Compare((TUint8*)testImage->DataAddress(), nbytes, (TUint8*)refImage->DataAddress(), nbytes)!=0)
		User::Panic(KPanicMsg, 0);

	CleanupStack::PopAndDestroy(10, &shape);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cs = CTrapCleanup::New();
	if (!cs)
		return KErrNoMemory;
		
	TRAPD(err,MainL());

	delete cs;
	return err;
	}
