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

#ifndef __T_PSEUDOAPP_ENG_H__
#define __T_PSEUDOAPP_ENG_H__

#include <w32std.h>
#include <iniparser.h>
#include <graphics/surface.h>		//TSurfaceId
#include <graphics/surfacemanager.h>	//RSurfaceManager
#include <graphics/surfaceupdateclient.h>

#include "t_pseudoappscreen.h"
#include "t_pseudoapptestresults.h"

enum TPseudoAppEngPanics
    {
    TPseudoAppEngAlreadyStarted = 1,
	TPseudoAppEngAlreadyStopped
    };

#ifndef T_PSEUDOAPP1
_LIT(KTPseudoAppPanicTxt,   "t_pseudoapp.exe");
#else
_LIT(KTPseudoAppPanicTxt,   "t_pseudoapp1.exe");
#endif //T_PSEUDOAPP1

_LIT(KGraphicsWservSurfaceTestResult,        			"GraphicsWservSurfaceTestResult");
_LIT(KGraphicsWservSurfaceFrameRate,         			"GraphicsWservSurfaceFrameRate");
_LIT(KGraphicsWservSurfaceTotalTestTime,         "GraphicsWservSurfaceTotalTestTime");

class CTPseudoAppShared;

class CTPseudoAppEng : public CTimer
	{
public:
	static CTPseudoAppEng* NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow);
	~CTPseudoAppEng();

	void ImportPseudoAppConfigL();
	void RotateL();
	TBool Drawing();
	void StartDrawingL();
	void StopDrawing();

private:
	// Implement CTimer
	void RunL();
	void DoCancel();

    // private constructors - use NewL to construct a CTPseudoAppEng!
    CTPseudoAppEng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow);
    void ConstructL();

    void SendUpdateRequest(TInt aScreen);

    void WriteResultsL(const TDesC& aFileName);
	void TestFinishedL(const TDesC& aFileName);

private:
	// Window server handling
	RWsSession& iClient;
	RWindow& iWindow;
	CWsScreenDevice& iScreenDevice;
	CWindowGc * iGc;

	RSurfaceUpdateSession iSurfaceUpdateSession;
	CTPseudoAppShared* iPseudoAppShared;
	
	TSize iScreenSize;
	TInt iScreenCounter;
	TInt iFrameDuration;
	TInt iFrameDelay;

	TUint64 iTimeStampBefore;
	TInt iCounterFreq;
	TBool iInitTimer;
	TTime iStartTime;
	TUint64 iTotalCompositionTime;

	CIniData* iUtils;
	TGceTestResults iGceTestResults;
	SurfaceDetails iSurfDetails;
	TInt iFrameCounter;
	TBool iDrawing;
	TBool iUseGlobalUpdate;
	};

#endif //__T_PSEUDOAPP_ENG_H__



