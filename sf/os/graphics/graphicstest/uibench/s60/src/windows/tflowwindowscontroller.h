// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TFLOWINDOWSCONTROLLER_H
#define TFLOWINDOWSCONTROLLER_H


#include "twindow.h"


class CSurfaceUtility;


/**
 * Creates and manages the windows and moves them around.
 * It uses function pointers to create the windows.
 * 
 */
class CTFlowWindowsController : public CBase
    {
public:    
    virtual ~CTFlowWindowsController();
    static CTFlowWindowsController* NewL (TBool aPreload, RArray<TPtrC>& aFileNames, 
										  TSize& aWindowSize, RArray<pTWindowCreatorFunction>& aNewLs,
										  RArray<TPoint>& aInitialPositions, TBool aBouncing,
										  TBool aMoveHorizontal = ETrue);

    inline TInt NumberOfWindows() const;
    inline void SetFlowRate(TInt aFlowRate);
    
	void Rotate(CFbsBitGc::TGraphicsOrientation aOrientation);
	void SetWindowsVisibility(TBool aVisible);
	
	virtual void MoveL();
	
protected:
    CTFlowWindowsController(TInt aNumWindows, TBool aPreload, TSize& aWindowSize, TBool aBouncing, TBool aMoveHorizontal);
    void ConstructL(RArray<TPtrC>& aFileNames, RArray<pTWindowCreatorFunction>& aNewLs, RArray<TPoint>& aInitialPositions);
	void SubmitUpdateL();
	
protected:
	RWsSession              iWs;
	RWindowGroup            iWinGroup;
	CWsScreenDevice*        iScreenDev;
	CSurfaceUtility*        iUtility;

	RArray<CTWindow*>       iFlowWindows;
	RArray<TPtrC>           iFileNames;
	
	RRegion                 iWindowsRegion;
	    
	TSize                   iWindowSize;
	TRect                   iScreenRect;
	TBool                   iPreload;
	TInt                    iNumWindows;
	TInt                    iDeltaX;
	TInt                    iDeltaY;
	TBool                   iBouncing;
    };


inline TInt CTFlowWindowsController::NumberOfWindows() const
    {
    return iNumWindows;
    }

/**
    Sets the rate at which the images are scrolled.
    
    @param aSpeed The number of pixels to move the image each cycle.
    */
inline void CTFlowWindowsController::SetFlowRate(TInt aFlowRate)
    {
    if (iDeltaX > 0)
        {
        iDeltaX = aFlowRate;
        }
    else 
        {
        iDeltaY = aFlowRate;
        }
    }



#endif /*TFLOWINDOWSCONTROLLER_H*/
