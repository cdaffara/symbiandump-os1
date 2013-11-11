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

#ifndef __T_FPSAPP_ENG_H__
#define __T_FPSAPP_ENG_H__

#include <w32std.h>
#include <graphics/surface.h>		//TSurfaceId
#include <graphics/surfacemanager.h>	//RSurfaceManager
#include <graphics/surfaceupdateclient.h>

_LIT(KTFpsAppPanicTxt,   "t_fpsapp.exe");

class CFpsAppEng : public CTimer
	{
public:
	static CFpsAppEng* NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow);
	~CFpsAppEng();

	void StartDrawing();


private:
	// Implement CTimer
	void RunL();
	void DoCancel();

    // private constructors - use NewL to construct a CTPseudoAppEng!
    CFpsAppEng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow);
    void ConstructL();

	// Surface
	void CreateSurfaceManager();
	void CreateSurface(TSurfaceId& aSurfaceId);
	void CreateSurfaceUpdateSessionL();
	void DestroySurface();

private:
	RWsSession& 			iClient;
	RWindow& 				iWindow;
	CWsScreenDevice& 		iScreenDevice;
	RSurfaceManager  		iSurfaceManager;
	TSurfaceId 				iSurfaceId;
	RChunk 					iChunk;
	RSurfaceUpdateSession 	iSurfaceUpdateSession;
	TUint32*				iSurfacePtr;
	TUint32*				iBufPtr1;
	TInt 					iNoOfBytes;
	TInt					iNoOfPixels;
	TBool 					iCol1;
	TBool 					iCol2;
	};

#endif //__T_FPSAPP_ENG_H__



