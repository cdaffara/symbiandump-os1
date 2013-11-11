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

#ifndef __T_APP1_ENG_H__
#define __T_APP1_ENG_H__

#include <w32std.h>

enum TApp1EngPanics
    {
    TApp1EngAlreadyStarted = 1,
	TApp1EngAlreadyStopped
    };


class CTApp1Eng : public CTimer	
	{

public:

	// Construction & destruction
	static CTApp1Eng* NewL 
		(
		RWsSession& aClient, 
		CWsScreenDevice& aScreenDevice, 
		RWindow& aWindow
		);

	~CTApp1Eng();	

public:

	// Start and stop the drawing
	void StartDrawing();
	void StopDrawing();

	// Are we drawing
	inline TBool Drawing(){return iDrawing;}

private:

	// Implement CTimer
	void RunL();
	void DoCancel();

private:

    // private constructors - use NewL to construct a CTApp1Eng!
    CTApp1Eng
        (
		RWsSession& aClient, 
		CWsScreenDevice& aScreenDevice,
		RWindow& aWindow
        );

    void ConstructL();	

private:

	// Window server handling
	RWsSession& iClient;
	RWindow& iWindow;
	CWsScreenDevice& iScreenDevice;
	CWindowGc * iGc;

	// Are we drawing
	TBool iDrawing;
	TBool iRotationFlag;
	
	TInt iFrameCount;
	
	RArray<TInt> iRotationList;
	RArray<TInt> iScrModeList;
	TInt iRotationTimer;
	TInt iRotationCount;
	TInt iIterations;
	
	RSemaphore iSemaphore;
	};

#endif // __T_APP1_ENG_H__



