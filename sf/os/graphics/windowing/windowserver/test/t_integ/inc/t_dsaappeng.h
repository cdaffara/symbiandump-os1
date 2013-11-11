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

#ifndef __T_DSA_APP_ENG_H__
#define __T_DSA_APP_ENG_H__

// INCLUDES
#include <w32std.h>

class CTPerfData;

// ENUMERATIONS

enum DirScrAccEngPanics
    {
    DirScrAccEngAlreadyStarted = 1,
	DirScrAccEngAlreadyStopped
    };

class CTDsaAppEng : public CTimer, public MDirectScreenAccess
	{

public:

	// Construction & destruction
	static CTDsaAppEng* NewL 
		(
		RWsSession& aClient, 
		CWsScreenDevice& aScreenDevice, 
		RWindow& aWindow
		);

	~CTDsaAppEng();	

public:

	// Start and stop the drawing
	void StartDrawing();
	void StopDrawing();

	// Are we drawing
	inline TBool Drawing(){return iDrawing;}

private:

	// Implement MDirectScreenAccess 
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);

private:

	// Implement CTimer
	void RunL();
	void DoCancel();

private:

    // private constructors - use NewL to construct a CTDsaAppEng!
    CTDsaAppEng
        (
		RWsSession& aClient, 
		CWsScreenDevice& aScreenDevice, 
		RWindow& aWindow
        );

    void ConstructL();	
    void CreateTestFileL(const TDesC& aFileName);
    void WriteResultsL(const TDesC& aFileName);
 
private:

	// Window server handling
	RWsSession& iClient;
	CWsScreenDevice& iScreenDevice;
	RWindow& iWindow;

	// Direct Screen Access
	CDirectScreenAccess* iDirectScreenAccess;
	CFbsBitGc* iGc;
	RRegion* iRegion;
	
	CTPerfData * iPerfData;
	
	// Are we drawing
	TBool iDrawing;
	
	TInt iFrameCount;
	TInt iFinishTesting;
	TInt iCounterFrequency;
	TUint32 iPrevCounter;

	TInt iPrevRotation;
	TInt iRotationAbortCount;
	
	RSemaphore iSemaphore;
	};

#endif // __T_DSA_APP_ENG_H__


