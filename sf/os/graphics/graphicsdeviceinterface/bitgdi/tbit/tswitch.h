// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TSWITCH_H__)
#define __TSWITCH_H__

#include <bitdev.h>
#include "TGraphicsHarness.h"

class CTSwitch : public CTGraphicsBase
	{
public:
	CTSwitch(CTestStep* aStep);
	~CTSwitch();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestL();
	TBool SetModeL(TDisplayMode aDispMode);
	void FillScreen();
	TBool SwitchModeL(TDisplayMode aDispMode);
	void CheckSwitch(TDisplayMode aOldDispMode,TDisplayMode aNewDispMode);
	void CheckBuffers(TDisplayMode aOldDispMode,TDisplayMode aNewDispMode, TUint aLineWidth,TInt aDitherOffset);
	void CheckRgbs(TDisplayMode aOldDispMode,TRgb aSrceRgb,TDisplayMode aNewDispMode,TRgb aDestRgb,TInt aDitherOffset);
	void ConvertRgb(TRgb& aRgb,TDisplayMode aDispMode,TInt aDitherOffset);
private:
	CFbsBitmap* iBitmap;
	CFbsScreenDevice* iDevice;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iAltDevice;
	CFbsBitGc* iAltGc;
	TRgb* iSrceBuffer;
	TRgb* iDestBuffer;
	};

class CTSwitchStep : public CTGraphicsStep
	{
public:
	CTSwitchStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTSwitchStep,"TSwitch");


#endif
