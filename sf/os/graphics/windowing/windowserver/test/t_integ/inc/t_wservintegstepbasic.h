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
//

/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_WSERVINTEGSTEPBASIC_H__
#define __T_WSERVINTEGSTEPBASIC_H__

#include <w32std.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#include <test/testexecutestepbase.h>

const TInt KTempWindowHeight = 200;
const TInt KTempWindowWidth = 200;
_LIT(KT_WServIntegStepBasic,"T_WServIntegStepBasic");


class CT_WServIntegStepBasic : public CTestStep
	{
public:
	CT_WServIntegStepBasic();
	~CT_WServIntegStepBasic();
	// from CTestStep
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPostambleL();

private:
	void CreateSurfaceManager();
	void CreateSurfaceL(TSurfaceId& aSurfaceId, TUint aColorPattern=0xFF0000FF);
	void CreateSurfaceUpdateSessionL();
	void DestroySurface();
	void CreateRWindowL(RWindow& aWin, const TPoint& aPos=TPoint(0,0), const TRgb& aBkgdColor=TRgb(0x800000FF), const TSize& aWinSize=TSize(KTempWindowWidth,KTempWindowHeight) );
	void CreateRBlankWindowL(RBlankWindow& aBlankWin);
	void ForceWindowToRedraw(RWindowBase& aWin);
	TVerdict SetSurfaceL(RWindow& aWin, TSurfaceId& aSurfaceId);
	void DrawShape(RWindow& aWin, const TRgb& aColor);

	// Test cases
	void Graphics_Wserv_Gce_1L(RWindow& aWin);
	void Graphics_Wserv_Gce_2L(RBlankWindow& aBlankWin);
	void Graphics_Wserv_Gce_3L();
	void Graphics_Wserv_Gce_4L(RWindow& aWin);
	void Graphics_Wserv_Gce_5L(RWindow& aWin);
	void Graphics_Wserv_Gce_6L(RWindow& aWin);
	void Graphics_Wserv_Gce_11L(RWindow& aWin);
	void Graphics_Wserv_Gce_13L(RWindow& aWin);
	void Graphics_Wserv_Gce_15L(RWindow& aWin);
	void Graphics_Wserv_Gce_17L(RWindow& aWin);
	void Graphics_Wserv_Gce_19L(RWindow& aWin);
	void Graphics_Wserv_Gce_21L(RWindow& aWin);
	void Graphics_Wserv_Gce_22L(RWindow& aWin);
	void Graphics_Wserv_Gce_31L(RBlankWindow& aBlankWin);
	void Graphics_Wserv_Gce_32L(RWindow& aWin);
	void Graphics_Wserv_Gce_33L(RWindow& aWin);
	void Graphics_Wserv_Gce_34L(RWindow& aWin);
	void Graphics_Wserv_Gce_35L(RWindow& aWin);
	void Graphics_Wserv_Gce_36L(RWindow& aWin);

private:	// Data members
	TInt					iTestId;
//	TDisplayMode			iDisplayMode;
	CWindowGc* 				iGc;
	CWsScreenDevice* 		iScreen;
	RWsSession 				iWsSession;
	RSurfaceManager  		iSurfaceManager;
	TSurfaceId 				iSurfaceId;
	RChunk 					iChunk;
	RSurfaceUpdateSession 	iSurfaceUpdateSession;
	RWindowGroup 			iWinGroup;
	};

#endif	// __T_WSERVINTEGSTEPBASIC_H__
