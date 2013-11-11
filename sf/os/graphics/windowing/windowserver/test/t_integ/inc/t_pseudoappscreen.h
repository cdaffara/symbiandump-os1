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

#ifndef __T_PSEUDOAPPSCREEN_H__
#define __T_PSEUDOAPPSCREEN_H__

#include <e32base.h>
#include <gdi.h>					//Display modes
#include <iniparser.h>

#include "t_pseudoappsurfacedwindow.h"
#include "t_pseudoapptestresults.h"
#include "t_pseudoappshared.h"

_LIT(KScreenWindowPopupApp,  "z:\\sys\\bin\\t_pseudoapppopupwindow.exe");
_LIT(KScreenRotationApp,     "z:\\sys\\bin\\t_screenrotationpapp.exe");

_LIT(KScreenWindow,          "screen%d_window_");
_LIT(KScreenWindowNumber,    "screen%d_window%d_");
_LIT(KScreen,                "screen%d_");

class CTestScreen : public CBase
	{
public:
	static CTestScreen* NewL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
									  const TDesC& aConfigFileName, CTPseudoAppShared& aPseudoAppShared);
	IMPORT_C void UpdateL(SurfaceDetails& aSurfDetails);
	IMPORT_C TBool Rotation();
	IMPORT_C void RotateL(TInt aScreenNo, TInt aFrameNumber);
	~CTestScreen();

private:
	CTestScreen(CTPseudoAppShared& aPseudoAppShared);
	void ConstructL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,const TDesC& aConfigFileName);
	void ImportScreenConfigL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults, const TDesC& aConfigFileName);
	void CreatePopupWindow();
	void DestroyPopupWindow();
	void LaunchRotationApp();
	void DestroyRotationApp();

private:
	CArrayPtrFlat<CTestSurfacedWindow>* iWindAndSurf;

	TInt iPopupOccurances;
	TInt iFrameDuration;
	TInt iFrameCounter;
	TInt iRotation;
	TSize iScreenSize;

	RProcess iPopupProcess;
	RProcess iRotationProcess;
	CIniData* iUtils;
	CTPseudoAppShared& iPseudoAppShared;
	};

#endif //__T_PSEUDOAPPSCREEN_H__
