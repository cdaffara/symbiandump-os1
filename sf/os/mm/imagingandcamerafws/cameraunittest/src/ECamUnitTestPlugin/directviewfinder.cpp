// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "directviewfinder.h"

/**
*	For Direct View Finder Pause/Resume functionality
*/
CCamDirectViewFinder* CCamDirectViewFinder::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new (ELeave) CCamDirectViewFinder(aOwner);
	}
	
CCamDirectViewFinder::~CCamDirectViewFinder()
	{
	}
	
CCamDirectViewFinder::CCamDirectViewFinder(CCamUnitTestPlugin& aOwner):iOwner(aOwner),
																	   iVFState(CCamera::CCameraDirectViewFinder::EViewFinderInActive)
	{
	}

void CCamDirectViewFinder::PauseViewFinderDirectL()
	{
	// dummy implementation
	// Pause view finder
	// Boundary checking of CCamera::ViewFinderActive() = ETrue should be there in real implementation 
	// otherwise Leave with KErrECamSettingDisabled
			
	iVFState = CCamera::CCameraDirectViewFinder::EViewFinderPause;
	}
	
void CCamDirectViewFinder::ResumeViewFinderDirectL()
	{
	// dummy implementation
	// Resume view finder
	// Boundary checking of CCamera::ViewFinderActive() = ETrue should be there in real implementation 
	// otherwise Leave with KErrECamSettingDisabled
			
	iVFState = CCamera::CCameraDirectViewFinder::EViewFinderActive;
	}
	
CCamera::CCameraDirectViewFinder::TViewFinderState CCamDirectViewFinder::ViewFinderState() const
	{
	return iVFState;
	}

void CCamDirectViewFinder::Release()
	{
	delete this;
	}
