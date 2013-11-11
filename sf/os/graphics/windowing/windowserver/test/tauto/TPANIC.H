
// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TPANIC_H__
#define __TPANIC_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "AUTODLL.H"
#include "TGraphicsHarness.h"

enum TPanicRedrawMode
	{
	EPanicRedrawModeBackedUpWindow,	// Draw to a RBackedUpWindow; no BeginRedraw()/EndRedraw() needed
	EPanicRedrawModeNormalRedraw,	// Draw in a redraw
	EPanicRedrawModeTransRedraw,	// Draw to a window under a transparent window in a redraw
	EPanicRedrawModeInvisRedraw,	// Draw to an invisible window in a redraw
	};

class CTPanic : public CTWsGraphicsBase
	{
public:
	struct TPanicParams
		{
	public:
		TInt iScreen;
		TPanicRedrawMode iRedrawMode;
		TInt iSubTest;
		};
	
	enum TCancelCaptureType
		{
		ECancelCaptureKey,
		ECancelCaptureKeyUpAndDowns,
		ECancelCaptureLongKey
		};
public:
	CTPanic(CTestStep* aStep);
	~CTPanic();
	void ConstructL();
private:
	void TestDeletedParentPanicsL();
	void TestScreenDevicePanicsL();
	void TestGraphicsPanicsL(TClientPanic aExitReason, TInt aIndex, CTPanic::TPanicParams* aPanicParams);
	void TestGraphicsPanicsL();
	void TestGraphicsPanicsL(TPanicRedrawMode aRedrawMode);
	void TestMiscPanicsL();
	void TestComputeModePanicsL();
	void TestCaptureKeyPanicsL();
	void TestEventPanicsL();
	void TestTPtrPanicsL();
	void TestOpcodePanicsL();
	void TestMultiInitPanicL();
	void TestSpritePanicsL();
	void TestMoveToGroupPanicsL();
	void TestHandleReUseL();
	void TestDeleteScreenPanicL();
	void TestWinHandleErrors();
	void TestGetEventErrors();
	void TestUnInitPanicL();
	void TestAlphaBlendingPanicL();
	void TestDoubleConstructionL();
	void TestDoubleConstructionNoPanic();
	void TestSpriteActivatePanicL();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA	
	void TestNonRedrawRedrawerL();
#endif	
	void TestMismatchedCaptureCancelPanicL();
	void LogHeapInfo();
	void TestEnableAdvancePointerActivatePanicL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TSize iWinSize;
	};

class CTPanicStep : public CTGraphicsStep
	{
public:
	CTPanicStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTPanicStep,"TPanic");

class RUnInitalisedConnect : public RSessionBase
	{
	public:
	TInt Connect(const TDesC &aName, const TVersion &aVersion);
	TInt Send(const TInt aMsg);
	};

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
class CNonRedrawWin : public CBase{
	public:
		~CNonRedrawWin();
		static CNonRedrawWin* NewL(RWsSession &aSession, CWindowGc *aGc);
		virtual void Redraw();
		RWsSession &GetSession();
		void Invalidate();
					
	protected:
		CNonRedrawWin(RWsSession &aSession, CWindowGc *aGc);
		void ConstrucL();

		class CRedrawer : public CActive{
			public:
				CRedrawer(CNonRedrawWin* aWd);
				~CRedrawer();		
			private:
				void HandleReDrawEvent();
				void RunL();
				void DoCancel();
				void HandleRedrawEvent();
				
			private:
				CNonRedrawWin* iWd;
		};
			
	private:		
		RWsSession &iSession;
		RWindowGroup iWdGrp;
		RWindow iWd;
		CWindowGc *iGc;
		
		CRedrawer *iRedrawer;			
		
};
#endif

#endif
