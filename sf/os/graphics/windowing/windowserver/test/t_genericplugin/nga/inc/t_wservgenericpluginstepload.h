// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_WSERV_GENERICPLUGIN_STEP_LOAD_H__
#define __T_WSERV_GENERICPLUGIN_STEP_LOAD_H__

#include <fbs.h>
#include <w32std.h>
#include <test/testexecutestepbase.h>
#include "../../common/inc/t_wservgenericpluginconsts.h"

class CT_WServGenericpluginStepLoad : public CTestStep
	{
public:
	CT_WServGenericpluginStepLoad();
	~CT_WServGenericpluginStepLoad();
	// from CTestStep
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPostambleL();
	
private:
	void CreateRWindowL(const RWindowGroup& aWinGroup, RWindow& aWin, const TPoint& aPos=TPoint(0,0), const TRgb& aBkgdColor=TRgb(0x800000FF), const TSize& aWinSize=TSize(KTempWindowWidth,KTempWindowHeight),  const TUint32 aHandle = KNullWsHandle );
	void DrawShape(RWindow& aWin);
	TBool CompareDisplayL(CWsScreenDevice* aScreen, CFbsBitmap* aBitmap);	
	void CalcAbsRect(TSize& aAbsRect, const TPoint& aPos, const TSize& aWinSize);
	// Test cases
	void GraphicsWservGenericpluginLoadL();
	void GraphicsWservGenericpluginLoad1L();
	void GraphicsWservGenericpluginLoad2L();
	void GraphicsWservGenericpluginLoad3L();
	void GraphicsWservGenericpluginLoad4L();
	void GraphicsWservGenericpluginLoad5L();
	TInt CheckLogL(TDesC8& aMsg);

private:	// Data members
	TInt					iTestId;
	TDisplayMode			iDisplayMode;
	CWindowGc* 				iGc;
	CWsScreenDevice* 		iScreen;
	RWsSession 				iWsSession;
	RWindowGroup 			iWinGroup;
	CWsScreenDevice* 		iScreen1;
	RWindowGroup 			iWinGroup1;
	CFbsBitmap* 			iDefaultPluginBmp;
	CFbsBitmap*				iTestPluginBmp;
	CFbsBitmap*				iTestPluginBmp1;
	};

#endif // __T_WSERV_GENERICPLUGIN_STEP_LOAD_H__
